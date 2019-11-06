/*______________________________________________
 |
 | tuki2_controller.c
 |
 */    
            
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "giocatore.h"
#include "tuki2_model.h"
#include "tuki2_view.h"

int rows,cols;

/*______________________________________________
 |
 |SEZIONE 1
 |Impostazioni del terminale di gioco
 |
 */
#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

struct configurazione_terminale {
  int cx, cy;
  int righe;
  int colonne;
  struct termios orig_termios;
};
struct configurazione_terminale E;

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

int posizione_cursore(int *rows, int *cols)
{
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  return 0;
}

int dimensioni_finestra(int *rows, int *cols)
{
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
   {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
      return -1;
    
    return posizione_cursore(rows, cols);
  } 
  else 
  {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

char leggi_tasto()
{
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
      if (nread == -1 && errno != EAGAIN) die("read");
    }
  return c;
}

void terminale_cucinato()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void terminale_crudo() 
{
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) 
    die("tcgetattr");
  
  atexit(terminale_cucinato);

  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 5;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
    die("tcsetattr");
}

int processa_tasto()
{
  char c = leggi_tasto();
  
  switch (c) {
    case CTRL_KEY('q'):      
      write(STDOUT_FILENO, "\x1b[?25l", 6);
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  case 'p':
    return 1;
  case 'a':
    return 2;
  }
}

void inizializza_terminale()
{
  if (dimensioni_finestra(&E.righe, &E.colonne) == -1) 
    die("dimensioni_finestra");
  
  rows = E.righe;
  cols = E.colonne;
  
  if(E.righe<BFHEIGHT||E.colonne<BFWIDTH)
    {
      char str[200];
      sprintf(str,
      "Ridimensiona (%d,%d) la finestra del terminale e rilancia",
      BFHEIGHT,BFWIDTH);
      die(str);
    }
}

/*________________________________________________________
 |
 | SEZIONE 2
 | Controller del Gioco
 */
#define MAJOR    0
#define MINOR    1
#define RELEASE  1

extern int pali_giuli [NPOLES];
extern int pali_tuki [NPOLES];

/* I giocatori*/
Gioca giuli_gioca, tuki_gioca;

/*Stato del gioco*/
int t_or_g_1_or_2;
int gioco = 1;
int jritardo = 200;
int dl1 = 50,dl2 = 100,dl3 = 200;

/*____________________________________________________________________
 |
 | Prototipi
 */
/*interni*/
static void linea_comando(int argc, char **argv);

/*esterni*/
direzione turno_giuli(); //turno_giuli.c
direzione turno_tuki(); //turno_tuki.c
direzione turno_interattivo(); //turno_interattivo.c
direzione (*challenger)(void);

int main(int argc, char **argv)
{  
  strcpy(tuki_gioca.name, "tuki");
  strcpy(giuli_gioca.name, "giuli");
  tuki_gioca.marcato[0] = -1;
  giuli_gioca.marcato[0] = -1;
  linea_comando(argc, argv);

  terminale_crudo();
  inizializza_terminale();

  //view_presentazione();
  
  while(1)
    {
      /*__________________________________________________________
       |
       | Preparazione dei giocatori sul campo di gioco
       */
      model_assegna_coordinate
      (XCENTRALPOLE-NPOLES*SCALE,BASELINE+1,&tuki_gioca);

      tuki_gioca.UTF = TUTF;

      model_assegna_coordinate
      (XCENTRALPOLE+NPOLES*SCALE,BASELINE+1,&giuli_gioca);

      giuli_gioca.UTF = GUTF;
      
      model_prepara_campo();

      model_inizializza_campo();
      
      view_opzioni();
            
      while (1) 
      {
        char chs;
        chs = processa_tasto();

        /*__________________________________
         |
         | Gioca contro l'algoritmo utente
         */
        if(chs == 2)
        {
          jritardo = 200;
          challenger = &turno_tuki;
          break;
        }
        /*__________________________________
         |
         | Gioca contro l'utente
         */
      if(chs == 1)
        {
          jritardo = 20; dl1 = 10; dl2 = 20; dl3 = 40;
          challenger = &turno_interattivo;
          view_cancella_messaggio();
          view_istruzioni();
          break;
        }
      }
      
      view_cancella_messaggio();
      
      gioco = 1;
      while(gioco)
      {
        direzione d;

        t_or_g_1_or_2 = 2;
        d  =  turno_giuli();
        model_turno(d,&giuli_gioca);
        
        t_or_g_1_or_2 = 1;
        d = (*challenger)();
        model_turno(d,&tuki_gioca);
      }  
      /*--FINE--*/
    }
}


static void linea_comando(int argc, char **argv)
{
  
  if(argc == 2 && strcmp(argv[1], "-gpl") == 0) 
  {
    fprintf(stderr,"GPL");
  }
  else if ((argc == 2 && strcmp(argv[1], "-h") == 0)||argc==1)
  {
    fprintf(stderr,"Usage  ./tuki [OPTIONS]\n");
    fprintf(stderr,"-p\tGioca a Tuki 2\n");
    fprintf(stderr,"-i\tStampa le istruzioni\n");
    fprintf(stderr,"-v\tVersione\n");
    fprintf(stderr,"-h\tQueste istruzioni\n");
    fprintf(stderr,"-c\tCrediti del gioco\n");
    exit(0);
  }
  else if (argc == 2 && strcmp(argv[1], "-v") == 0) 
  {
    fprintf(stderr,
    "*** tuki %i.%i.%i\nCopyleft 2019 iSisiniPazzi\n",
    MAJOR, MINOR, RELEASE);
    getchar();
    exit(1);
  }
  else if (argc == 2 && strcmp(argv[1], "-p") == 0) 
  {
    return;
  }
  else if (argc == 2 && strcmp(argv[1], "-i") == 0) 
  {
    fprintf(stderr,
    "Apri il file turno_tuki.c e implementa turno_tuki\n");
    fprintf(stderr,"Usa le API descritte in  giocatore.h\n");
    exit(1);	
  }			
  else if (argc == 2 && strcmp(argv[1], "-c") == 0)
  {
    fprintf(stderr,"\nCredits:\n\n");
    fprintf(stderr,
    "(c) Scuola Sisini facebook.com/scuolasisini/\n");
    exit(1);
  }
  else if (argc >1) 
  {
      fprintf(stderr," ");
      exit(1);
  }
}

/*_______________________________________________
 *|
 *| tuki1_view.c
 */

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include "tuki1_model.h"

#define COFFSET(width) (cols-width)/2
#define ROFFSET(heigh) (rows-heigh)/2

struct termios orig_termios;
int rows,cols;

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}
  
int dimensioni_finestra(int *rows, int *cols) 
{
  struct winsize ws;  
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
      return -1;
    } else
    {
      *cols = ws.ws_col;
      *rows = ws.ws_row;
      return 0;
    }
}
  
void terminale_cucinato()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void terminale_crudo()
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
    die("tcgetattr");
  
  atexit(terminale_cucinato);
  
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
    die("tcsetattr");  
}

int vista_init()
{
  terminale_crudo();
  dimensioni_finestra(&rows, &cols);
  setlocale(LC_CTYPE, "");
}

char * get_str(elemento_campo e)
{
  if(e==TLC) return "\u250F\u2501";
  if(e==TRC) return "\u2513";
  if(e==BRC) return "\u251B";
  if(e==BLC) return "\u2517\u2501";
  if(e==HLN) return "\u2501\u2501";
  if(e==VLN) return "\u2503";
  if(e==TT) return "\u253b\u2501";
  if(e==TB) return "\u2533\u2501";
  if(e==TL) return "\u2523\u2501";
  if(e==TR) return "\u252b";
  if(e==FUTF) return "\u2618";
  if(e==GUTF) return "oo";//"\u263b";
  if(e==TUTF) return "@>";
  return "  ";
}

int vista_stampa_campo(elemento_campo **maze)
{
  int x,y;
  int rof=ROFFSET(CAMPO_ALTEZZA*1);
  int cof=COFFSET(CAMPO_LARGHEZZA*2);
  char str[25];
  for(int i=0;i<CAMPO_ALTEZZA;i++)
    for(int j=0;j<CAMPO_LARGHEZZA;j++)
      {
        x=j*2+cof;
        y=i+rof;
        
        sprintf(str,
        "\x1b[%d;%dH%s",y,x,get_str(*(*(maze+i)+j)));
        
        write(STDOUT_FILENO, str, strlen(str));
      }
  return 0;
}

int vista_stampa_sfondo(int r, int c,elemento_campo **maze)
{
  int x,y;
  int rof=ROFFSET(CAMPO_ALTEZZA*1);
  int cof=COFFSET(CAMPO_LARGHEZZA*2);
  char str[25];
  
  x=c*2+cof;
  y=r+rof;
  
  sprintf(str,
  "\x1b[%d;%dH%s",y,x,get_str(*(*(maze+r)+c)));
  
  write(STDOUT_FILENO, str, strlen(str));
  
  return 0;
}

int vista_stampa_giocatore(Giocatore* p)
{
  int x,y;
  int rof=ROFFSET(CAMPO_ALTEZZA*1);
  int cof=COFFSET(CAMPO_LARGHEZZA*2);
  char str[25];
  x=p->posX*2+cof;
  y=p->posY+rof;
  
  sprintf(str,
  "\x1b[%d;%dH%s",y,x,get_str(p->UTF));
  
  write(STDOUT_FILENO, str, strlen(str));
}

void vista_punteggio(int t_score, int g_score, int flowers)
{
  char str2[50];
  char str[60];
  
  sprintf(str2,
  "@>:%3d Flowers:%3d  oo:%3d",t_score,flowers,g_score);
  int l=strlen(str2);
  int rof=ROFFSET(CAMPO_ALTEZZA*1);
  int cof=COFFSET(l);
  sprintf(str,"\x1b[%d;%dH%s",rof-1,cof,str2);
  write(STDOUT_FILENO, str, strlen(str));
}

void vista_gameover(char * message)
{
  char str2[50];
  char str[60];
  
  int l=strlen(message);
  int rof=ROFFSET(CAMPO_ALTEZZA*1);
  int cof=COFFSET(l);
  
  sprintf(str,
  "\x1b[%d;%dH%s\r\n\r\n",rof+CAMPO_ALTEZZA+1,cof,message);
  
  write(STDOUT_FILENO, str, strlen(str));
  
  printf("\x1b[%d;%dH",rows,1);
}

void delay(int milliseconds){
  long pause;
  clock_t now,then;
  
  pause = milliseconds*(CLOCKS_PER_SEC/1000);
  now = then = clock();
  while( (now-then) < pause )
    now = clock();
}

void fading( char * str,int r, int c)
{
  float gray=255;
  float inc=0.03;
  float vrs=1;
  while(gray>232)
    {
      printf("\x1b[%d;%dH\x1b[38;5;%dm%s\x1b[m"
      ,r,c,(int)gray,str);
      
      if(gray >= 255){
        vrs=-1;
      }
      if(gray <= 232){
        vrs=1;
      }
      delay(1);
      gray+=vrs*inc;
    } 
  fflush(stdout);
}

void vista_presentazione()
{
  char str2[50];
  char str[60];
  
  char * message[]={"TUKI & GIULI","Usa le tue abilità di",
  "programmatore e","vinci la sfida!",
  "Coding-game prodotto e distribuito",
  "Produzioni i SisiniPazzi(R)",
  "visita il sito pumar.it",
  "e scopri le news e i prodotti della tecnologia",
  "del futuro."};
  
  int l,rof,cof;
  
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);
  for(int i=0;i<9;i++)
    {
      l=strlen(message[i]);
      rof=ROFFSET(CAMPO_ALTEZZA*1.5);
      cof=COFFSET(l);
      fading(message[i],rof+CAMPO_ALTEZZA/2+i,cof);
      delay(500);
    }
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);
}

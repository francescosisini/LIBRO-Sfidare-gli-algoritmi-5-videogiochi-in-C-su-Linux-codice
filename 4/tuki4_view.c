    
/*______________________________
 *|
 *| tuki4_view.c
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
#include "tuki4_model.h"


#define COFFSET(width) (cols-width)/2
#define ROFFSET(heigh) (rows-heigh)/2

enum btf{
  GUTF=9787,
  TUTF=0040,
  TLC =9487,
  TRC =9491,
  BRC =9499,
  BLC =9495,
  HLN =9473,
  VLN =9475,
  TT =9531,
  TB =9523,
  TL =9507,
  TR =9515,
  FUTF =9752,
  APPLE=1,
  BOMB=2,
  DIS=3,
  SPREC=4
};
typedef enum btf btf_elm;

/*** Data ***/
struct termios orig_termios;
int rows,cols;


/*** Terminal ***/


void die(const char *s) 
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}
  
int get_window_size(int *rows, int *cols)
{
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
   {
    return -1;
   } 
  else 
   {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
   }
}


void disable_raw_mode() 
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enable_raw_mode() 
{
  
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disable_raw_mode);
  
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
  
}



int view_inizializza()
{
  enable_raw_mode();
  get_window_size(&rows, &cols);
  setlocale(LC_CTYPE, "");
}


/*** output ***/
char * get_str(btf_elm e)
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
  if(e==BOMB) return "\u25CF";
  if(e==APPLE) return "\u272A";
  if(e==DIS) return "\u2714";
  if(e==SPREC) return "\u2639";
  return "  ";
}



int view_campo()
{
  int x,y;
  int rof=ROFFSET(ALTEZZA*1);
  int cof=COFFSET(LARGHEZZA*2);
  char str[25];
  for(int i=0;i<ALTEZZA;i++)
    for(int j=0;j<LARGHEZZA;j++)
      {
        if((j == 0  && i == 0))
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(TLC));
            write(STDOUT_FILENO, str, strlen(str));
          }
        if((j == LARGHEZZA-1  && i == 0))
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(TRC));
            write(STDOUT_FILENO, str, strlen(str));
          }
        if((j == LARGHEZZA-1  && i == ALTEZZA-1))
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(BRC));
            write(STDOUT_FILENO, str, strlen(str));
          }
        if((j == 0  && i == ALTEZZA-1))
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(BLC));
            write(STDOUT_FILENO, str, strlen(str));
          }
        if((j == 0||j == LARGHEZZA-1)  && i>0  && i<ALTEZZA-1)
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(VLN));
            write(STDOUT_FILENO, str, strlen(str));
          }
        

        if((i == 0||i == ALTEZZA-1)  && j>0  && j<LARGHEZZA-1)
          {
            x=j*2+cof;
            y=i+rof;
            sprintf(str,"\x1b[%d;%dH%s",y,x,get_str(HLN));
            write(STDOUT_FILENO, str, strlen(str));
          }
      }
  return 0;
}

int view_evento(evento e)
{
  int x,y;
  int rof=ROFFSET(ALTEZZA*1);
  int cof=COFFSET(LARGHEZZA*2);
  char str[25];
  x=e.l.x+LARGHEZZA/2.;
  y=e.l.y+ALTEZZA/2.;
  
  if(e.l.proiettile == BOMBA && e.az == MANGIA)
    sprintf(str,"\x1b[%d;%dH%s",y+rof,x*2+cof,get_str(BOMB));
  else if(e.l.proiettile == BOMBA && e.az == DISTRUGGI)
    sprintf(str,"\x1b[%d;%dH%s",y+rof,x*2+cof,get_str(DIS));
  else if(e.l.proiettile == MELA && e.az == DISTRUGGI)
    sprintf(str,"\x1b[%d;%dH%s",y+rof,x*2+cof,get_str(SPREC));
  else if(e.l.proiettile == MELA && e.az == MANGIA)
    sprintf(str,"\x1b[%d;%dH%s",y+rof,x*2+cof,get_str(APPLE));
  
  
  write(STDOUT_FILENO, str, strlen(str)+1);
  fflush(stdout);
}

void view_punteggio(int energy, int damage, int launches)
{
  char str2[80];
  char str[90];
  if(damage>0)
    sprintf(str2,
    "@> Energia:%3d   Danni:%3d  Lanci restanti:%3d",
    energy,damage,launches);
  else
    sprintf(str2,
    "@> Energia:%3d   Bonus:%3d  Lanci restanti:%3d",
    energy,-damage,launches);
    
  int l = strlen(str2);
  int rof = ROFFSET(ALTEZZA*1);
  int cof = COFFSET(l);
  
  sprintf(str,"\x1b[%d;%dH%s",rof-1,cof,str2);
  write(STDOUT_FILENO, str, strlen(str));
  fflush(stdout);
}

void view_gameover(char * message)
{
  char str2[50];
  char str[60];
  
  int l = strlen(message);
  int rof = ROFFSET(ALTEZZA*1);
  int cof = COFFSET(l);
  
  sprintf(str,"\x1b[%d;%dH%s\r\n\r\n",rof+ALTEZZA+1,cof,message);
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
  float gray = 255;
  float inc = 0.03;
  float vrs = 1;
  while(gray>232)
    {
      printf("\x1b[%d;%dH\x1b[38;5;%dm%s\x1b[m",r,c,(int)gray,str);
      if(gray>=255){
        vrs=-1;
      }
      if(gray<=232){
        vrs=1;
      }
      delay(1);
      gray+=vrs*inc;
    } 
  fflush(stdout);
    

}


void view_presentazione()
{
  char str2[50];
  char str[60];
  char * message[] = {"TUKI & GIULI",
  "Usa le tue abilità di",
  "programmatore e","vinci la sfida!",
  "Coding-game prodotto e distribuito",
  "Produzioni i SisiniPazzi(R)",
  "visita il sito isisinipazzi.it",
  "e scopri le news e i prodotti della tecnologia",
  "del futuro."};
  int l,rof,cof;
  
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);
  for(int i=0;i<9;i++)
    {
      l = strlen(message[i]);
      rof = ROFFSET(ALTEZZA*1.5);
      cof = COFFSET(l);
      fading(message[i],rof+ALTEZZA/2+i,cof);
      delay(500);
    }
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);  
}
 

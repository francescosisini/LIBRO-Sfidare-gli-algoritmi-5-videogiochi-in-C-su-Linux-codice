/*____________________________________________
 *|
 *| tuki2_view.c
 */
  
#include <sys/ioctl.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "giocatore.h"
#include "tuki2_view.h"
#include "tuki2_model.h"

extern Gioca tuki_gioca;
extern Gioca giuli_gioca;
extern int dl1,dl2,dl3;
extern int rows,cols;


char chat[BFHEIGHT] [BFWIDTH];
int clat[BFHEIGHT] [BFWIDTH];

void stampa_palo(int d, char side,int l,int bgc);



/*____________________________________________
 *|
 *| Vittoria di Tuki
 */
void view_tuki_win()
{
  int rof = ROFFSET(BFHEIGHT*1);
  int cof = COFFSET(BFWIDTH*1);
  
  char* strw = "****Tuki ha vinto!!!****";
  int y = 5;
  int x = (BFWIDTH-strlen(strw))/2;
  printf("\x1b[%d;%dH%s",y+rof,x+cof,strw);
  printf("\x1b[%d;%dH",BFHEIGHT+2+rof,1+cof);
  ritardo(3000);
}

/*____________________________________________
 *|
 *| Vittoria di Giuli
 */
void view_giuli_win()
{
  int rof = ROFFSET(BFHEIGHT*1);
  int cof = COFFSET(BFWIDTH*1);
  
  char* strw = "****Giuli ha vinto!!!****";
  int y = 5;
  int x = (BFWIDTH-strlen(strw))/2;
  printf("\x1b[%d;%dH%s",y+rof,x+cof,strw);
  printf("\x1b[%d;%dH",BFHEIGHT+2+rof,1+cof);
  ritardo(3000);
}

void view_cancella_messaggio()
{
  int rof = ROFFSET(BFHEIGHT*1);
  int cof = COFFSET(BFWIDTH*1);
  
  for(int i = 1;i<= 8;i++)
    for(int j = 1;j<BFWIDTH;j++)
      //printf("\x1b[%dm\x1b[%d;%dH ",BGC,i+rof,j+cof);
      printf("\x1b[%d;%dH ",i+rof,j+cof);
  fflush(stdout);
  
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
        vrs = -1;
      }
      if(gray<=232){
        vrs = 1;
      }
      ritardo(1);
      gray+=vrs*inc;
    } 
  fflush(stdout);
}

void view_presentazione()
{
  
  
  char str2[50];
  char str[60];
  char * message[] = {"TUKI & GIULI",
  "Usa le tue abilità di","programmatore e",
  "vinci la sfida!","Coding-game prodotto e distribuito",
  "Produzioni i SisiniPazzi(R)","visita il sito pumar.it",
  "e scopri le news e i prodotti della tecnologia",
  "del futuro."};
  int l,rof,cof;
  
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);
  for(int i = 0;i<9;i++)
    {
      l = strlen(message[i]);
      rof = ROFFSET(BFHEIGHT*1.5);
      cof = COFFSET(l);
      fading(message[i],rof+i,cof);
      ritardo(500);
    }
  printf("\x1b[%d;%dH",1,1);
  write(STDOUT_FILENO, "\x1b[2J", 4);
}

void view_opzioni()
{
  int rof = ROFFSET(BFHEIGHT);
  int cof = COFFSET(BFWIDTH);
  
  char wc[50];
  strcpy(wc,"Giuly e Tuki: Sfida per Lady Beetle");
  char opt1[35];
  strcpy(opt1,"Premi <p> per usare la tastiera");
  char opt2[35];
  strcpy(opt2,"Premi <a> per sfida tra Algoritmi");
  char opt3[30];
  strcpy(opt3,"<ctrl-q> uscire");
  printf("\x1b[?25l");
  printf("\x1b[%d;%dH\x1b[1m\x1b[;32m%s\x1b[0m",4+rof,(int)(MARGINI(wc))+cof,wc);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  5+rof,(int)(MARGINI(opt1))+cof,opt1);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  6+rof,(int)(MARGINI(opt2))+cof,opt2);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  7+rof,(int)(MARGINI(opt3))+cof,opt3);
  fflush(stdout);
}

void view_istruzioni()
{
  int rof = ROFFSET(BFHEIGHT*1);
  int cof = COFFSET(BFWIDTH*1);
  
  char i1[50];
  strcpy(i1,"Ordina i pali scambiadoli due a due");
  char i2[30];
  strcpy(i2,"<l> destra");
  char i3[30];
  strcpy(i3,"<j> sinistra");
  char i4[30];
  strcpy(i4,"<i> seleziona palo");
  char i5[30];
  strcpy(i5,"<space> scambia");
  printf("\x1b[%d;%dH\x1b[1m\x1b[;32m%s\x1b[0m",4+rof,(int)(MARGINI(i1))+cof,i1);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  5+rof,(int)(MARGINI(i2))+cof,i2);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  6+rof,(int)(MARGINI(i3))+cof,i3);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  7+rof,(int)(MARGINI(i4))+cof,i4);
  printf("\x1b[%d;%dH\x1b[;30m%s\x1b[0m",
  7+rof,(int)(MARGINI(i5))+cof,i5);
  fflush(stdout);
  ritardo(3000);
}

void view_mostra_erba()
{
  int rof  =  ROFFSET(BFHEIGHT);
  int cof  =  COFFSET(BFWIDTH);
  
  for(int i = 1;i<BFWIDTH;i++)
    for(int j = 1;j<BFHEIGHT;j++)
      {
        chat[j][i] = 32;
        clat[j][i] = BGC;

        printf("\x1b[%d;%dH \x1b[0m",j+rof,i+cof);
      }
  for(int i = 1;i<BFWIDTH;i++)
    {
      printf("\x1b[%d;%dH\x1b[0;%dm\x1b[1;36mv\x1b[0m",
      BASELINE+rof,i+cof,GGC);
      printf("\x1b[%d;%dH\x1b[0;%dm\x1b[1;36mv\x1b[0m",
      BASELINE+1+rof,i+cof,GGC);
      chat[BASELINE][i] = 'v';
      chat[BASELINE+1][i] = 'v';
      clat[BASELINE][i] = GGC;
      clat[BASELINE+1][i] = GGC;
    } 
}

void view_marca_palo(int d, char side,int l)
{
  int bgc = 105;
  stampa_palo(d, side, l, bgc);
}

void view_smarca_palo(int d, char side,int l)
{
  int bgc = 43;
  stampa_palo(d, side, l, bgc);
}

void view_mostra_palo(int d, char side,int l)
{
  int bgc = 43;
  stampa_palo(d, side, l, bgc);
}

void view_evidenzia_palo(int d1, char side1,int l1,int d2, char side2,int l2)
{
  int C1 = 101,C2 = 43;
  stampa_palo(d1, side1, l1, C1);
  stampa_palo(d2, side2, l2, C1);
  ritardo(dl1);
  stampa_palo(d1, side1, l1, C2);
  stampa_palo(d2, side2, l2, C2);
  ritardo(dl2);
  stampa_palo(d1, side1, l1, C1);
  stampa_palo(d2, side2, l2, C1);
  ritardo(dl2);
  stampa_palo(d1, side1, l1, C2);
  stampa_palo(d2, side2, l2, C2);
  ritardo(dl2);
  stampa_palo(d1, side1, l1, C1);
  stampa_palo(d2, side2, l2, C1);
  ritardo(dl3);
}

void stampa_palo(int d, char side,int l,int bgc)
{
  int rof = ROFFSET(BFHEIGHT);
  int cof = COFFSET(BFWIDTH);
  
  int bfwm = XCENTRALPOLE;
  int sgn = 1;
  if(side=='L') sgn = -1;
  int x = bfwm+sgn*d;
  /*clear the space for the pole*/
  for(int i = 0;i<NPOLES;i++)
    {
      chat[BASELINE-i][x] = 32;
      clat[BASELINE-i][x] = BGC;

      printf("\x1b[%d;%dH \x1b[0m",BASELINE-i+rof,x+cof);
    }
  for(int i = 0;i<l;i++)
    {
      char polec = ' ';
     
      printf("\x1b[%d;%dH\x1b[1;%dm%c\x1b[0m",
      BASELINE-i+rof,x+cof,bgc,polec);
      
      chat[BASELINE-i][x] = polec;
      clat[BASELINE-i][x] = bgc;
    }

  /*_modifica la y del giocatore se si trova su un palo in movimento_*/
  if(giuli_gioca.posX==x)
    {
      model_assegna_coordinate(x,BASELINE-l,&giuli_gioca);
    }
  if(tuki_gioca.posX==x)
    {
      model_assegna_coordinate(x,BASELINE-l,&tuki_gioca);
    }
  fflush(stdout);
}

void view_mostra_palo_centrale(int l)
{
  int rof = ROFFSET(BFHEIGHT);
  int cof = COFFSET(BFWIDTH);
  
  char polec = ' ';
  int x = XCENTRALPOLE;
  view_mostra_palo(0, 'R', l);

  printf("\x1b[%d;%dH\x1b[1;32m\u2618\x1b[0m",BASELINE-l+1+rof,x+cof);
  chat[BASELINE-l+1][x] = polec;
  clat[BASELINE-l+1][x] = 43;
}

void view_cancella_traccia_giocatore(Gioca* p)
{
  int x = p->posX;
  int y = p->posYold-1;
}


void view_mostra_giocatore(Gioca p)
{
  int rof = ROFFSET(BFHEIGHT*1);
  int cof = COFFSET(BFWIDTH*1);
  
  int x = p.posX;
  int y = p.posY;
  int xo = p.posXold;
  int yo = p.posYold;
  
  /*_ripristina il fondo_*/
  printf("\x1b[%d;%dH\x1b[;%dm%c\x1b[0m",
  yo+rof,xo+cof,clat[yo][xo],chat[yo][xo]);
  printf("\x1b[%d;%dH\x1b[;%dm%c\x1b[0m",
  yo+rof,xo+1+cof,clat[yo][xo+1],chat[yo][xo+1]);
  if(xo>0)
    printf("\x1b[%d;%dH\x1b[;%dm%c\x1b[0m",
    yo+rof,xo-1+cof,clat[yo][xo-1],chat[yo][xo-1]);
  
  if(strcmp(p.name,"tuki")==0)
    {
      if(p.dir == DESTRA)
      {
        printf("\x1b[%d;%dH\x1b[1;39m@\x1b[0m\x1b[1;91m>\x1b[0m\x1b[0m",
        y+rof,x+cof);
    	}
      else
      {
        printf("\x1b[%d;%dH\x1b[1;91m<\x1b[1;39m@\x1b[0m\x1b[0m",
        y+rof,x-1+cof);
          
        if(xo == 0)
        {
         printf("\x1b[%d;%dH%c\x1b[0m",yo+rof,xo+cof,chat[yo][xo-1]);
        }
      else
        {
         printf("\x1b[%d;%dH%c\x1b[0m",yo+rof,xo-1+cof,chat[yo][xo-1]);
        }
      }
    }
  if(strcmp(p.name,"giuli")==0)
    {
      if(p.dir == DESTRA)
        printf("\x1b[%d;%dH\x1b[1;32moò\x1b[0m",y+rof,x+cof);
      else
        printf("\x1b[%d;%dH\x1b[1;32mòo\x1b[0m",y+rof,x+cof);
    }
  fflush(stdout);
}


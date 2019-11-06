   

/*__________________________________________
 *
 * tuki3_mvc.c
 */   
   
#include<stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include "giocatore.h"



#define CTRL_KEY(k) ((k) & 0x1f)
#define LARGHEZZA_T 140
#define ALTEZZA_T 30
#define LUNGHEZZA_CAMMINO 512 
#define LUNGHEZZA_CAMMINO_VISIBILE 140 
#define Y_CAMMINO 15 
#define NUMERO_OGGETTI 10
#define GDELAY 40000
#define TUKI_DIM 8

#define GEMMA_TOX 0.1
#define POZIONE_TOX 0.7
#define MELANZANA_TOX 0.05
#define GEMMA_NTRL 0.2
#define POZIONE_NTRL 0.2
#define MELANZANA_NTRL 0.75
#define GEMMA_NRGY 0.7
#define POZIONE_NRGY 0.1
#define MELANZANA_NRGY 0.2
#define X0_TUKI 10

#define MAX_TOX (float)(GEMMA_TOX+POZIONE_TOX+MELANZANA_TOX)/3.*((float)NUMERO_OGGETTI/2.)
#define MIN_NTRL (MELANZANA_NTRL)*(float)NUMERO_OGGETTI/2.0
#define MIN_NRGY (GEMMA_NRGY)*(float)NUMERO_OGGETTI/2.0

#define T0_NTRL MIN_NTRL
#define T0_NRGY MIN_NRGY

#define D_NTRL T0_NTRL*1.5/LUNGHEZZA_CAMMINO
#define D_NRGY T0_NRGY*1.5/LUNGHEZZA_CAMMINO
#define MARGIN(str) (LARGHEZZA_T-strlen(str))/2.
#define MENU_ROW 1

#define LIVELLO 10

struct termios orig_termios;
struct configurazione_terminale {
  int cx, cy;
  int righe;
  int colonne;
  struct termios orig_termios;
};
struct configurazione_terminale E;

typedef struct{
  float nutrienti;
  float tossine;
  float energia;
  tipo_oggetto t;
  int pos_x;
  char pxl[DIM*DIM];
  char exists;
}oggetto;

typedef struct  {
  int id; 
  int pos_x; 
  int pos_xold;
  int pos_yold; 
  int pos_y; 
  int score;
  char name[10];
  char pxl[11][TUKI_DIM*TUKI_DIM];
  stato stato_giocatore;
 } Giocatore;

char ladyb[TUKI_DIM*TUKI_DIM]={
    0,0,1,0,0,1,0,0,
    0,0,0,1,1,0,0,0,
    0,0,2,2,2,2,0,0,
    0,2,1,2,2,1,2,0,
    2,1,2,2,2,2,1,2,
    0,2,1,2,2,1,2,0,
    0,0,2,2,2,2,0,0,
    0,0,1,0,0,1,0,0
  };
char goal[TUKI_DIM*TUKI_DIM]={
    3,4,0,0,0,0,0,0,
    4,3,4,3,0,0,0,0,
    5,0,3,4,3,4,0,0,
    5,0,0,0,4,3,4,3,
    5,0,0,0,0,0,3,4,
    0,0,0,0,0,0,0,5,
    0,0,0,0,0,0,0,5,
    0,0,0,0,0,0,0,5
  };

/*__________________________________________
 *
 * Controllo modalità monitor e tastiera
 */
void cancella_schermo();
void initEditor();
void die(const char *s);
void  terminale_cucinato();
void terminale_crudo();
void inizializza_terminale();
int  posizione_cursore(int *rows, int *cols);
int dimensioni_finestra(int *rows, int *cols);
char leggi_tasto();
int processa_tasto();

/*______________________________________
 *|
 *| Model
 */
void model_aggiungi_rumore(oggetto *o,int livello);
void model_distruggi(oggetto *objt,int x_left);
void model_inizializza(Giocatore *gctr,int x, float ntrl, float tox,float nrgy,char *name);
void model_oggetto(tipo_oggetto t,double nutrienti, double energia,double tossine,int pos_x,int exists,  oggetto *ob);
void model_inizializza_cammino(char cammino[]);
void fine_giocatore(char *message);
void model_imposta_pixels(char px1,char px2,char px3,char px4,char px5,char px6,char px7,char px8,char *p);
void model_punteggio(Giocatore *gctr,oggetto *bjt);
stato model_azione(azione a);
char * model_disfatta(Giocatore *gctr);

/*______________________________________
 *|
 *| View
 */
void view_punteggio(stato *gctr_stato,float percentage);
void view_stampa_giocatore(Giocatore *gctr,int x_left);
void view_oggetto(oggetto *objt,int x_left,int x_right);
void view_aumentata(int atx, int fromx,char *ch);
void view_nuvoletta(oggetto *ob,int x_left);

/*______________________________________
 *|
 *| Controller
 */
void delay(int milliseconds);
void mostra_presentazione();
void mostra_livello(int livello, char completo);
void mostra_finale();



azione turno_tuki(char oggetto_inviato[DIM*DIM], stato stato_tuki);

/*** Globali***/
oggetto *g_oggetto;
Giocatore *g_gctr;

/*** Gioco ***/
int main()
{
  int livello_corrente=0;

  terminale_crudo();
  inizializza_terminale();

  Giocatore gctr_tuki;
  Giocatore gctr_giuli;
  
  char cammino[LUNGHEZZA_CAMMINO];
  char dsp_cammino[LUNGHEZZA_CAMMINO_VISIBILE+1];
  oggetto ob[NUMERO_OGGETTI]; //The object along the cammino
  oggetto cloud[5];//Clouds animation
  
  /* Inizio gioco */
  
  gctr_tuki.id=1;
  g_gctr=&gctr_tuki;
  model_inizializza(&gctr_tuki,X0_TUKI,T0_NTRL,0,T0_NRGY,"Tuki");
  
  mostra_presentazione();
  
  /* Cicla sui LIVELLI livelli di gioco */
  for(livello_corrente = 0;
  livello_corrente<LIVELLO;livello_corrente++)
    {
      int avanzamento_cammino=1;
      int oggetto_prossimo=0;

      cancella_schermo();

      mostra_livello(livello_corrente,0);

      cancella_schermo();
      
      
      model_inizializza(&gctr_tuki,X0_TUKI,T0_NTRL,0,T0_NRGY,"Tuki");
      model_inizializza_cammino(cammino);
      
      srand(time(0));
      int k;

      /* Prepara gli oggetti sul campo
       e li trasfigura in base alla difficoltà del livello */
       
      for(int i=0;i<NUMERO_OGGETTI;i++)
      {
        k=rand()%3;
        int x=(i+2)*((float)LUNGHEZZA_CAMMINO/(float)NUMERO_OGGETTI);    
        switch (k)
        {
         case GEMMA:
          model_oggetto
          (GEMMA,GEMMA_NTRL,GEMMA_NRGY,GEMMA_TOX,x,1,ob+i);
          
          model_aggiungi_rumore(ob+i,livello_corrente);
          
          break;
         case MELANZANA:
          model_oggetto
          (MELANZANA,MELANZANA_NTRL,MELANZANA_NRGY,
          MELANZANA_TOX,x,1, ob+i);
          
          model_aggiungi_rumore(ob+i,livello_corrente);
          
          break;
         case POZIONE:
          model_oggetto
          (POZIONE,POZIONE_NTRL,POZIONE_NRGY,POZIONE_TOX,x,1,ob+i);
          
          model_aggiungi_rumore(ob+i,livello_corrente);
          break;
        }
      }
      
      cancella_schermo();
      
      char *gtitle="  -Il duro cammino- (c)Scuola Sisini 2018-19 ";
      char *tup=   " --------------- ";
      
      printf("\x1b[%d;%dH\x1b[38;5;26m%s\x1b[0m Livello %d",
      Y_CAMMINO+1,((int)(MARGIN(gtitle))),gtitle,livello_corrente+1);
      
      for(int r=0;
      r<LUNGHEZZA_CAMMINO-LUNGHEZZA_CAMMINO_VISIBILE;
      r++)
      {
        view_stampa_giocatore(&gctr_tuki,r);
        view_punteggio
        (&(gctr_tuki.stato_giocatore),
        (float)r/(float)	(LUNGHEZZA_CAMMINO-LUNGHEZZA_CAMMINO_VISIBILE));
          
        for(int i=0;i<NUMERO_OGGETTI;i++)
        {
          view_oggetto(ob+i,r,r+LUNGHEZZA_CAMMINO_VISIBILE);
        }
        if(!ob[oggetto_prossimo].exists)oggetto_prossimo+=1;
        
        g_oggetto=ob+oggetto_prossimo;
          
        memcpy(dsp_cammino,cammino+avanzamento_cammino,
        LUNGHEZZA_CAMMINO_VISIBILE);
        
        dsp_cammino[LUNGHEZZA_CAMMINO_VISIBILE]=0;
        
        printf("\x1b[%d;%dH\x1b[38;5;94m%s\x1b[0m",
        Y_CAMMINO,1,dsp_cammino);
          
        fflush(stdout);
          
        /* Passo avanti*/
        avanzamento_cammino+=1;
        gctr_tuki.pos_xold=gctr_tuki.pos_x;
        gctr_tuki.pos_x+=1;
        gctr_tuki.stato_giocatore.nutrienti-=D_NTRL;
        gctr_tuki.stato_giocatore.energia-=D_NRGY;
          
          
        /*Verifica collisione*/
        if(ob[oggetto_prossimo].pos_x == gctr_tuki.pos_x+TUKI_DIM-1)
        {
          azione a = turno_tuki
          (ob[oggetto_prossimo].pxl, gctr_tuki.stato_giocatore);
            
            model_azione(a);
            
            if(!ob[oggetto_prossimo].exists){
              view_nuvoletta(ob+oggetto_prossimo,r);
            }
          }
           
          
        if(ob[oggetto_prossimo].exists && 
        (ob[oggetto_prossimo].pos_x == gctr_tuki.pos_x+TUKI_DIM-1))
        {
          printf("\x1b[%d;1HAzione: <Nessuna azione>   ",
          MENU_ROW+1);
          
          model_punteggio(&gctr_tuki,ob+oggetto_prossimo);
          
          (ob+oggetto_prossimo)->exists=0;
          
          model_distruggi(ob+oggetto_prossimo,r);
        }
          
        char * cm = model_disfatta(&gctr_tuki);
          
        if(cm)
          {
            fine_giocatore(cm);
            exit(0);
          }
          
        delay(GDELAY);
      }
      cancella_schermo();
      
      mostra_livello(livello_corrente,1);
      
      cancella_schermo();
    }
  /*Completati tutti i livelli*/
  mostra_finale(dsp_cammino);
  
}

char * model_disfatta(Giocatore *gctr)
{
   if(gctr->stato_giocatore.tossine>MAX_TOX)
     return("TUKI INTOSSICATO!");
   
   if(gctr->stato_giocatore.nutrienti<0)
     return("TUKI SENZA NUTRIENTI");
   
   if(gctr->stato_giocatore.energia<0)
     return("TUKI SENZA ENERGIA");
   
   return 0;
}

void mostra_finale(char *dsp_cammino)
{
  cancella_schermo();
  printf("\x1b[%d;%dH\x1b[38;5;94m%s\x1b[0m",Y_CAMMINO,1,dsp_cammino);
  (*g_gctr).pos_x=X0_TUKI;
  for(int i=(*g_gctr).pos_x;i<LUNGHEZZA_CAMMINO_VISIBILE-35;i++)
  { 
    (*g_gctr).pos_x=i+1;
    view_stampa_giocatore(g_gctr,0);
    view_aumentata(X0_TUKI+20, LUNGHEZZA_CAMMINO_VISIBILE-10,goal);
    (*g_gctr).pos_xold=(*g_gctr).pos_x;
    fflush(stdout);
    delay(100000);
  }
  
  view_aumentata(LUNGHEZZA_CAMMINO_VISIBILE-10,
   LUNGHEZZA_CAMMINO_VISIBILE-10,ladyb);
    fflush(stdout);
    delay(1000000);
  
  for(int i=(*g_gctr).pos_x;i<LUNGHEZZA_CAMMINO_VISIBILE-18;i++)
  { 
    (*g_gctr).pos_x=i+1;
    view_stampa_giocatore(g_gctr,0);  
    (*g_gctr).pos_xold=(*g_gctr).pos_x;
    fflush(stdout);
    delay(100000);
  }
}

void model_aggiungi_rumore(oggetto *o,int livello)
{
  int r;
  for(int i=0;i<DIM;i++){
    for(int j=0;j<DIM;j++){
      r=(float)rand()/(float)RAND_MAX*10;
      if(r<livello)
        if(o->pxl[i*DIM+j])
          o->pxl[i*DIM+j]=!o->pxl[i*DIM+j];
    }
  }
}

void fine_giocatore(char *msg){
  cancella_schermo();
  printf("\x1b[%d;%dH %s",10,10,msg);
  printf("\x1b[1;1H");
}

void view_punteggio(stato *gctr_stato,float p){
  static char prbar[LARGHEZZA_T-20];
  memset (prbar,'-',(LARGHEZZA_T-20));
  memset (prbar,'#',(int)(LARGHEZZA_T-20)*(p));
  prbar[LARGHEZZA_T-20-2]='|';
  prbar[LARGHEZZA_T-20-1]=0;
  
  printf("\x1b[%d;%dHPercorso: \x1b[38;5;26m%s\x1b[0m",MENU_ROW,1,prbar);
  printf("\x1b[%d;%dHNutrienti:\t %2.2f",MENU_ROW+2,1,gctr_stato->nutrienti);
  printf("\x1b[%d;%dHTossine:\t %2.2f/%2.2f",MENU_ROW+3,1,gctr_stato->tossine,MAX_TOX);
  printf("\x1b[%d;%dHEnergia:\t %2.2f",MENU_ROW+4,1,gctr_stato->energia);
  fflush(stdout);
}

void model_punteggio(Giocatore *gctr,oggetto *bjt)
{
  gctr->stato_giocatore.nutrienti+=bjt->nutrienti;
  gctr->stato_giocatore.tossine+=bjt->tossine;
  gctr->stato_giocatore.energia+=bjt->energia;
}

void view_aumentata(int atx, int fromx,char *ch)
{
  int rw;
  int cl;
  for(int i=0;i<TUKI_DIM;i++)
  {
    for(int j=0;j<TUKI_DIM;j++)
    {
      rw=i-TUKI_DIM;
      cl=fromx+j;
      printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
    }
  }
  
  for(int i=0;i<TUKI_DIM;i++)
  {
    for(int j=0;j<TUKI_DIM;j++)
    {
      char c=ch[i*TUKI_DIM+j];
      rw=i-TUKI_DIM;
      cl=atx+j;
      if(c==0)
        printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
      if(c==1)
        printf("\x1b[%d;%dH\x1b[48;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
      if(c==2)
        printf("\x1b[%d;%dH\x1b[48;5;9m \x1b[0m",rw+Y_CAMMINO,cl);
      if(c==3)
        printf("\x1b[%d;%dH\x1b[48;5;9m \x1b[0m",rw+Y_CAMMINO,cl);
      if(c==4)
        printf("\x1b[%d;%dH\x1b[48;5;11m \x1b[0m",rw+Y_CAMMINO,cl);
      if(c==5)
        printf("\x1b[%d;%dH\x1b[48;5;1m \x1b[0m",rw+Y_CAMMINO,cl);
      }
    }
}

void view_nuvoletta(oggetto *ob,int x_left){
  int x=ob->pos_x;
  int rw;
  int cl;
  char cld[5][TUKI_DIM*TUKI_DIM]={
    {0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,
     0,0,0,2,1,0,0,0,
     0,2,2,3,2,1,0,0, 
     3,2,1,0,2,1,1,2, 
     0,0,3,3,2,1,2,0, 
     0,0,0,3,2,0,0,0, 
     0,0,0,0,0,0,0,0},
    {0,0,0,0,0,2,0,0, 
    1,0,1,0,0,0,0,0, 
    0,0,1,1,2,0,1,0, 
    2,0,1,2,1,1,1,0, 
    0,0,0,1,2,1,0,0, 
    0,0,0,0,2,0,0,1, 
    0,0,0,0,0,0,1,0, 
    0,0,0,0,0,0,0,0 },
    {0,0,0,1,0,0,0,0, 
    0,0,0,0,0,0,0,1, 
    0,0,0,0,0,0,0,0, 
    1,0,0,1,1,0,0,0, 
    0,0,1,1,1,1,0,0, 
    0,0,0,1,1,0,0,0, 
    0,0,0,0,0,0,0,0, 
    0,0,0,0,0,0,2,0},
    {0,0,1,0,0,0,0,0, 
    0,0,0,0,0,0,1,0, 
    0,0,0,0,0,2,0,0, 
    0,0,0,1,1,0,0,0, 
    0,1,0,0,0,0,0,0, 
    0,0,0,0,0,2,0,0, 
    0,0,0,0,0,0,0,0, 
    0,0,1,0,0,0,2,0},
    {0,0,0,0,0,0,0,0, 
    0,0,0,0,0,0,0,0 
    ,0,0,0,0,0,0,0,0, 
    0,0,0,0,0,0,0,0, 
    0,0,0,0,0,0,0,0}
  };

  for(int k=0;k<5;k++)
  {
    for(int i=0;i<TUKI_DIM;i++)
    {
      for(int j=0;j<TUKI_DIM;j++)
      {
        char c=cld[k][i*TUKI_DIM+j];
        rw=i-TUKI_DIM;
        cl=x-x_left+j;
        if(c==0)
          printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
        if(c==1)
          printf("\x1b[%d;%dH\x1b[48;5;45m \x1b[0m",rw+Y_CAMMINO,cl);
        if(c==2)
          printf("\x1b[%d;%dH\x1b[48;5;97m \x1b[0m",rw+Y_CAMMINO,cl);
        if(c==3)
          printf("\x1b[%d;%dH\x1b[48;5;226m \x1b[0m",rw+Y_CAMMINO,cl);
       }
      }
      
    fflush(stdout);
    delay(50000);
  }
}

/*destroy objects*/
void model_distruggi(oggetto *ob,int x_left)
{
  int x=ob->pos_x;
  int rw;
  int cl;
  
  for(int i=0;i<DIM;i++)
  {
    for(int j=0;j<DIM;j++)
    {
      rw=i-DIM;
      cl=x-x_left+j;
      printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
    }
  }
  
}

/*display objects at their coordinates when visibles*/
void view_oggetto( oggetto *objt,int x_left,int x_right)
{
  if(!objt->exists)return;
  int x=objt->pos_x;
  int rw;
  int cl;
  int range=DIM;
  int offset=0;
  
  if(x<x_left) offset=x_left-x;
  if(x>x_right-DIM) range=x_right-x;

  //delete previous trace
  if((x+DIM)==x_left)
  {
    for(int i=0;i<DIM;i++)
    {
      for(int j=0;j<1;j++)
      {
        rw=i-DIM;
        cl=x_right-x_left-LUNGHEZZA_CAMMINO_VISIBILE+1;
        printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
      }
    }
  }
  
  if((x+DIM)<x_left||x>x_right) return;

  for(int i=0;i<DIM;i++)
  {
    for(int j=offset;j<range;j++)
    {
      rw=i-DIM;
      cl=x+j-(x_left-1);
      printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw+Y_CAMMINO,cl);
    }
  }

  int color;
  if(objt->t==GEMMA) color=92;
  if(objt->t==MELANZANA) color=40;
  if(objt->t==POZIONE) color=13;
  for(int i=0;i<DIM;i++)
  {
    for(int j=offset;j<range;j++)
    {
      rw=i-DIM;
      cl=objt->pos_x+j-x_left;
      switch (objt->pxl[i*DIM+j])
      {
        case 0:
          printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",
          rw+Y_CAMMINO,cl);
          break;
      case 1:
        printf("\x1b[%d;%dH\x1b[48;5;%dm \x1b[0m",
        rw+Y_CAMMINO,cl,color);
	break;
      }
    }
  }


  
}


void view_stampa_giocatore(Giocatore *gctr,int x_left)
{
  static int fr=0;  
  if(fr==11){
    fr=0;
  }
  
  int rw;
  int cl;
  for(int i=0;i<TUKI_DIM;i++)	
  {
    for(int j=0;j<TUKI_DIM;j++)
    {
      rw=gctr->pos_yold+i;
      cl=gctr->pos_xold+j-x_left;
      printf("\x1b[%d;%dH ",rw,cl);
    }
  }
  
  for(int i=0;i<TUKI_DIM;i++)
  {
    for(int j=0;j<TUKI_DIM;j++)
    {
      rw=gctr->pos_y+i;
      cl=gctr->pos_x+j-x_left;
      switch (gctr->pxl[fr][i*TUKI_DIM+j])
      {
      case 0:
        printf("\x1b[%d;%dH\x1b[38;5;0m \x1b[0m",rw,cl);
        break;
      case 1:
        printf("\x1b[%d;%dH\x1b[48;5;244m \x1b[0m",rw,cl);
        break;
      case 2:
        printf("\x1b[%d;%dH\x1b[48;5;244m\x1b[38;5;36m@\x1b[0m",rw,cl);
        break;
      case 3:
        printf("\x1b[%d;%dH\x1b[48;5;238m \x1b[0m",rw,cl);
        break;
      case 4:
        printf("\x1b[%d;%dH\x1b[48;5;226m \x1b[0m",rw,cl);
        break;
      case 5:
        printf("\x1b[%d;%dH\x1b[38;5;214m|\x1b[0m",rw,cl);
        break;
      case 6:
        printf("\x1b[%d;%dH\x1b[38;5;214m \x1b[0m",rw,cl);
        break;
      case 7:
        printf("\x1b[%d;%dH\x1b[38;5;214m \x1b[0m",rw,cl);
        break;
      case 8:
        printf("\x1b[%d;%dH\x1b[38;5;214m_\x1b[0m",rw,cl);
        break;
      case 9:
        printf("\x1b[%d;%dH\x1b[38;5;214m|\x1b[0m",rw,cl);
        break;
      }
    }
  }
  fr++;
  fflush(stdout);
  if(fr==3||fr==8)
    delay(2.5*GDELAY);
}


void model_inizializza
(Giocatore *gctr,int x, float ntrl, float tox,float nrgy,char *name)
{
  strncpy(gctr->name,name,10);
 
    gctr->pos_x=x;
    gctr->stato_giocatore.nutrienti=ntrl;
    gctr->stato_giocatore.tossine=tox;
    gctr->stato_giocatore.energia=nrgy;
    gctr->pos_y=Y_CAMMINO-TUKI_DIM;
    gctr->pos_xold=gctr->pos_x;
    gctr->pos_yold=gctr->pos_y;
    //Frame 1
    char *p=gctr->pxl[0];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,2,0,0,p+16);
    model_imposta_pixels(0,0,0,1,3,4,4,4,p+24);
    model_imposta_pixels(1,0,1,3,3,1,4,4,p+32);
    model_imposta_pixels(0,1,3,1,1,1,4,4,p+40);
    model_imposta_pixels(0,1,1,1,1,1,4,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 2
    p=gctr->pxl[1];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,2,0,0,p+16);
    model_imposta_pixels(0,0,0,1,1,3,4,0,p+24);
    model_imposta_pixels(1,0,3,1,3,3,4,0,p+32);
    model_imposta_pixels(0,1,1,1,3,1,4,0,p+40);
    model_imposta_pixels(0,1,1,1,1,1,4,0,p+48);
    model_imposta_pixels(0,0,0,0,6,5,0,0,p+56);
    //Frame 3
    p=gctr->pxl[2];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+16);
    model_imposta_pixels(0,0,3,1,1,1,3,0,p+24);
    model_imposta_pixels(0,0,3,3,1,3,3,0,p+32);
    model_imposta_pixels(0,1,1,1,3,1,0,0,p+40);
    model_imposta_pixels(0,0,1,1,1,1,0,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 4
    p=gctr->pxl[3];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+16);
    model_imposta_pixels(0,0,3,1,1,1,3,0,p+24);
    model_imposta_pixels(0,0,3,3,1,3,3,0,p+32);
    model_imposta_pixels(0,1,1,1,3,1,0,0,p+40);
    model_imposta_pixels(0,0,1,1,1,1,0,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 5
    p=gctr->pxl[4];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,2,0,0,p+16);
    model_imposta_pixels(0,0,0,1,1,3,4,0,p+24);
    model_imposta_pixels(1,0,3,1,3,3,4,0,p+32);
    model_imposta_pixels(0,1,1,1,3,1,4,0,p+40);
    model_imposta_pixels(0,1,1,1,1,1,4,0,p+48);
    model_imposta_pixels(0,0,0,0,6,5,0,0,p+56);
    //Frame 6
    p=gctr->pxl[5];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,2,0,0,p+16);
    model_imposta_pixels(0,0,0,1,3,4,4,4,p+24);
    model_imposta_pixels(1,0,1,3,3,1,4,4,p+32);
    model_imposta_pixels(0,1,3,1,1,1,4,4,p+40);
    model_imposta_pixels(0,1,1,1,1,1,4,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 7
    p=gctr->pxl[6];
    model_imposta_pixels(0,0,0,0,0,1,0,0,p);
    model_imposta_pixels(0,0,0,0,1,1,1,0,p+8);
    model_imposta_pixels(0,0,0,0,1,2,2,0,p+16);
    model_imposta_pixels(0,0,0,3,1,1,4,4,p+24);
    model_imposta_pixels(1,0,3,3,1,1,4,4,p+32);
    model_imposta_pixels(0,3,3,1,1,1,4,4,p+40);
    model_imposta_pixels(0,0,1,1,1,4,4,0,p+48);
    model_imposta_pixels(0,0,0,0,5,7,0,0,p+56);
    //Frame 8
    p=gctr->pxl[7];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,2,2,0,0,p+16);
    model_imposta_pixels(0,0,0,3,4,4,3,0,p+24);
    model_imposta_pixels(1,1,3,3,4,4,3,3,p+32);
    model_imposta_pixels(0,3,3,1,4,4,3,3,p+40);
    model_imposta_pixels(0,0,1,1,1,4,0,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 9
    p=gctr->pxl[8];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,2,2,0,0,p+16);
    model_imposta_pixels(0,0,3,1,4,4,3,0,p+24);
    model_imposta_pixels(1,3,3,1,4,4,3,3,p+32);
    model_imposta_pixels(0,3,1,1,4,4,1,3,p+40);
    model_imposta_pixels(0,0,1,1,1,4,0,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    //Frame 10
    p=gctr->pxl[9];
    model_imposta_pixels(0,0,0,0,0,1,0,0,p);
    model_imposta_pixels(0,0,0,0,1,1,1,0,p+8);
    model_imposta_pixels(0,0,0,0,1,2,2,0,p+16);
    model_imposta_pixels(0,0,0,3,1,4,4,4,p+24);
    model_imposta_pixels(1,0,3,3,1,1,4,4,p+32);
    model_imposta_pixels(0,3,3,1,1,1,4,4,p+40);
    model_imposta_pixels(0,0,1,1,1,4,4,0,p+48);
    model_imposta_pixels(0,0,0,0,5,7,0,0,p+56);
    //Frame 11
    p=gctr->pxl[10];
    model_imposta_pixels(0,0,0,0,1,0,0,0,p);
    model_imposta_pixels(0,0,0,1,1,1,0,0,p+8);
    model_imposta_pixels(0,0,0,1,1,2,0,0,p+16);
    model_imposta_pixels(0,0,0,1,3,4,4,4,p+24);
    model_imposta_pixels(1,0,1,3,3,1,4,4,p+32);
    model_imposta_pixels(0,1,3,1,1,1,4,4,p+40);
    model_imposta_pixels(0,1,1,1,1,1,4,0,p+48);
    model_imposta_pixels(0,0,0,0,5,5,0,0,p+56);
    
 
}

void imposta_pixel_oggetto
(char px1,char px2,char px3,char px4,char px5,char *p)
{
  *p=px1;
  *(p+1)=px2;
  *(p+2)=px3;
  *(p+3)=px4;
  *(p+4)=px5;
}

void model_imposta_pixels
(char px1,char px2,char px3,char px4,
char px5,char px6,char px7,char px8,char *p)
{
  *p=px1;
  *(p+1)=px2;
  *(p+2)=px3;
  *(p+3)=px4;
  *(p+4)=px5;
  *(p+5)=px6;
  *(p+6)=px7;
  *(p+7)=px8;
}

/* Posiziona un oggetto alla distanza pos_x*/
void model_oggetto(tipo_oggetto t,double nutrienti, double energia,double tossine,int pos_x,int exists,  oggetto *ob)
{
  ob->t=t;
  ob->nutrienti=nutrienti;
  ob->energia=energia;
  ob->tossine=tossine;
  ob->pos_x=pos_x;
  ob->exists=exists;
  char *p=ob->pxl;
  switch (ob->t)
    {
    case GEMMA:
      imposta_pixel_oggetto(0,0,0,0,0,p);
      imposta_pixel_oggetto(0,1,1,1,0,p+5);
      imposta_pixel_oggetto(1,0,1,0,1,p+10);
      imposta_pixel_oggetto(0,1,0,1,0,p+15);
      imposta_pixel_oggetto(0,0,1,0,0,p+20);
      break;
    case MELANZANA:
      imposta_pixel_oggetto(0,1,1,0,0,p);
      imposta_pixel_oggetto(1,0,0,1,0,p+5);
      imposta_pixel_oggetto(1,0,0,1,0,p+10);
      imposta_pixel_oggetto(0,1,0,1,0,p+15);
      imposta_pixel_oggetto(0,0,1,1,1,p+20);
      break;
    case POZIONE:
      imposta_pixel_oggetto(0,0,1,0,0,p);
      imposta_pixel_oggetto(0,0,1,0,0,p+5);
      imposta_pixel_oggetto(0,1,1,1,0,p+10);
      imposta_pixel_oggetto(1,1,1,1,1,p+15);
      imposta_pixel_oggetto(0,1,1,1,0,p+20);
      
      break;

    }
  
}

void model_inizializza_cammino(char cammino[]){
  for(int i=0;i<LUNGHEZZA_CAMMINO;i++){
    if(i%6==0){
      cammino[i++]='#';
      cammino[i]='#';
    }
    else cammino[i]=' ';
  }
}

void cancella_schermo()
{
  write(STDOUT_FILENO, "\x1b[?25l", 6);
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** Giocatore ***/
stato model_azione(azione a)
{
  tipo_oggetto t=g_oggetto->t;
  switch (a)
    {
    case MANGIA:
      printf("\x1b[%d;1HAzione: <MANGIA!>             ",MENU_ROW+1);
      if(t==MELANZANA)
	{
	  g_gctr->stato_giocatore.nutrienti+=g_oggetto->nutrienti;
	  g_gctr->stato_giocatore.energia+=g_oggetto->energia;
	}
      if(t==POZIONE)
	{
	  g_gctr->stato_giocatore.tossine+=g_oggetto->tossine;
	}
      if(t==GEMMA)
	{
	  g_gctr->stato_giocatore.tossine+=g_oggetto->tossine;
	}
      break;
    case SALTA:
      printf("\x1b[%d;1HAzione: <SALTA!>          ",MENU_ROW+1);
      break;
    case PRENDI:
      printf("\x1b[%d;1HAzione: <PRENDI!>          ",MENU_ROW+1);
      if(t==MELANZANA)
	{
	  g_gctr->stato_giocatore.energia+=g_oggetto->energia;
	}
      if(t==POZIONE)
	{
	  g_gctr->stato_giocatore.energia+=g_oggetto->energia;
	  g_gctr->stato_giocatore.tossine+=g_oggetto->tossine;
	}
      if(t==GEMMA)
	{
	  g_gctr->stato_giocatore.energia+=g_oggetto->energia;
	}
      break;
    }
  g_oggetto->exists=0;

}




void delay(int milliseconds){
  long pause;
  clock_t now,then;
  
  pause = milliseconds*(1000/1000);
  now = then = clock();
  while( (now-then) < pause )
    now = clock();
}

void mostra_livello(int livello,char completo)
{
   char i1[70];
   if(completo)
     {
       sprintf(i1,"Congratulazioni! Completato livello %d",livello+1);
       printf("\x1b[4;%dH%s",(int)(MARGIN(i1)),i1);
       fflush(stdout);
       delay(1000000);
     }
   else
     {
       sprintf(i1,"Pronto livello %d",livello+1);
       printf("\x1b[4;%dH%s",(int)(MARGIN(i1)),i1);
       fflush(stdout);
       delay(1000000);
     }
   
}

void mostra_presentazione()
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  for(int i=1;i<ALTEZZA_T;i++)
    for(int j=1;j<LARGHEZZA_T;j++)
      printf("\x1b[%d;%dH ",i,j);
  fflush(stdout);
  delay(1000);
  char i1[40];
  strcpy(i1,"@scuola_sisini presents:");
  char i2[30];
  strcpy(i2,"Il duro cammino");
  char i3[70];
  strcpy(i3,"Un gioco programma di pattern recognition");
  char i4[30];
  strcpy(i4,"Copyleft @scuola_sisini");
  char i5[50];
  strcpy(i5,"find more games on facebook.com/scuolasisini");
  char i6[120];
  strcpy(i6,"Premi un tasto per continuare o <CTRL>-q per uscire");
  
  printf("\x1b[4;%dH%s",(int)(MARGIN(i1)),i1);
  fflush(stdout);
  delay(200000);
  printf("\x1b[5;%dH%s",(int)(MARGIN(i2)),i2);
  fflush(stdout);
   printf("\x1b[7;%dH%s",(int)(MARGIN(i3)),i3);
   delay(200000);
  fflush(stdout);
   printf("\x1b[9;%dH%s",(int)(MARGIN(i4)),i4);
   delay(200000);
  fflush(stdout);
  printf("\x1b[11;%dH%s",(int)(MARGIN(i5)),i5);
   delay(200000);
  fflush(stdout);
  printf("\x1b[13;%dH%s",(int)(MARGIN(i6)),i6);
   delay(200000);
  fflush(stdout);
  char chs=0;
  
  while (1) {
    chs=  processa_tasto();
    break;
  }
  cancella_schermo();
  for(int i=0;i<(int)MARGIN("XXXXXXXX")-3;i++){
    
    (*g_gctr).pos_x=i+1;
    view_stampa_giocatore(g_gctr,0);  
    (*g_gctr).pos_xold=(*g_gctr).pos_x;
    delay(100000);
  }
  delay(1000000);
  strcpy(i6,"Ciao sono Tuki e voglio raggiungere LadyB");
  printf("\x1b[1;%dH%s",(int)(MARGIN(i6)),i6);
  fflush(stdout);
  delay(1500000);
  strcpy(i6,"Ho bisogno di cibo ed energia, ma temo le tossine");
  printf("\x1b[2;%dH%s",(int)(MARGIN(i6)),i6);
  fflush(stdout);
  delay(1500000);
  strcpy(i6,"Sono guidato dal tuo algoritmo...");
  printf("\x1b[3;%dH%s",(int)(MARGIN(i6)),i6);
  fflush(stdout);
  strcpy(i6,"                                         ");
  printf("\x1b[1;%dH%s",(int)(MARGIN(i6)),i6);
  delay(1500000);
  strcpy(i6,"che deve riconoscere quello che trovo");
  printf("\x1b[4;%dH%s",(int)(MARGIN(i6)),i6);
  fflush(stdout);
  delay(1500000);
  cancella_schermo();
  strcpy(i6,"per farmi ragguiungere la mia amica");
  printf("\x1b[1;%dH%s",(int)(MARGIN(i6)),i6);
  fflush(stdout);
  delay(1500000);

  

  /*Mostra gli oggetti*/
  cancella_schermo();
  oggetto ob;
  //Gemma
  strcpy(i6,"Questa è la GEMMA: <PRENDI> la! ");
  printf("\x1b[5;%dH%s",(int)(MARGIN(i6)),i6);
  int ds=(int)MARGIN("xxxxx");;
  model_oggetto(GEMMA,0.1,0.7,0.2,ds,1,&ob);
  view_oggetto(&ob,0,LUNGHEZZA_CAMMINO_VISIBILE);
  fflush(stdout);
  delay(1000000);
  view_nuvoletta(&ob,0);
  fflush(stdout);
  delay(1000000);
  //Melanzana
  strcpy(i6,"Questa è la MELANZANA: <MANGIA> la!");
  printf("\x1b[5;%dH%s",(int)(MARGIN(i6)),i6);
  model_oggetto(MELANZANA,0.1,0.7,0.2,ds,1,&ob);
  view_oggetto(&ob,0,LUNGHEZZA_CAMMINO_VISIBILE);
  fflush(stdout);
  delay(1000000);
  view_nuvoletta(&ob,0);
  fflush(stdout);
  delay(1000000);
  //Veleno
  strcpy(i6,"Questa è la POZIONE: <SALTA> la!    ");
  printf("\x1b[5;%dH%s",(int)(MARGIN(i6)),i6);
  model_oggetto(POZIONE,0.1,0.7,0.2,ds,1,&ob);
  view_oggetto(&ob,0,LUNGHEZZA_CAMMINO_VISIBILE);
  fflush(stdout);
  delay(1000000);
  view_nuvoletta(&ob,0);
  fflush(stdout);
  delay(1000000);

  
  
}


/***termimal***/
void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

int posizione_cursore(int *rows, int *cols) {
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

int dimensioni_finestra(int *rows, int *cols) {
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

void  terminale_cucinato()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}
void terminale_crudo()
{

  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
   die("tcgetattr");
   
  atexit( terminale_cucinato);

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

/*** input ***/
int processa_tasto() 
{
  char c = leggi_tasto();
  switch (c) 
  {
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
   default:
    return 0;
  }
}

/*** init ***/

void inizializza_terminale()
{
  if (dimensioni_finestra(&E.righe, &E.colonne) == -1)
   die("dimensioni_finestra");
  if(E.righe<ALTEZZA_T||E.colonne<LARGHEZZA_T)
    {
      char str[200];
      sprintf(str," Ridimensiona la finestra (%d,%d)",
      ALTEZZA_T,LARGHEZZA_T);
      die(str);
    }
}


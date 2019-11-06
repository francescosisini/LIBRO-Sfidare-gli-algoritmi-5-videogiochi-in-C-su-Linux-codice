/*_____________________________________________________________
 *|
 *| tuki2_model.c
 */ 
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "giocatore.h"
#include "tuki2_view.h"
#include "tuki2_model.h"

int pali_giuli [NPOLES];
int pali_tuki [NPOLES];

extern Gioca tuki_gioca;
extern Gioca giuli_gioca;

extern int t_or_g_1_or_2;
extern char g_marcato[2];
extern char t_marcato[2];
extern int jritardo;
extern int gioco;

/*_____________________________________________________________
 *|
 *| Turno del giocatore 
 *| Stabilisce l'indice del paolo su cui saltare
 *| in base al palo corrente e alla direzione
 *| Il giocaore salta dal <palo_corrente> al <paolo_obiettivo>
 */
void model_turno(direzione d, Gioca *giocatore){
  
  int *poles;

  int ll,rl;
  
  int palo_corrente,palo_obiettivo;

  giocatore->dir = d;
 
  
  /*_Tuki_*/
  if(t_or_g_1_or_2 == 1)
    {
      ll = 1;
      rl = XCENTRALPOLE;
      poles = pali_tuki;
    }
   /*_Giuli_*/
  else
    {
      rl = BFWIDTH-1;
      ll = XCENTRALPOLE;
      poles = pali_giuli;
  } 
  
  palo_corrente = indice_palo_corrente(*giocatore);
  palo_obiettivo = palo_corrente;
  
  int x_old = giocatore->posX;
  int y_old = giocatore->posY;
  int y = y_old;
  
  if(d == IN)
    if(t_or_g_1_or_2 == 1)
      {
        model_assegna_coordinate
        (giocatore->posX,y,giocatore);
        
        if(giocatore->scambia == 1)
        {
          salta(giocatore);

          ritardo(jritardo);

         _scambia_pali();

          ritardo(jritardo);
        }
      return;
      }
  
  if(t_or_g_1_or_2 == 1)
    {
      if(d==DESTRA && x_old == XCENTRALPOLE) d=SINISTRA;
      if(x_old<=(XCENTRALPOLE-(NPOLES+1)*SCALE))
      {
        palo_corrente=NPOLES;
        d=DESTRA;
      }
      if(d == SINISTRA)
      {
        if(palo_corrente<(NPOLES-1))
          palo_obiettivo = palo_corrente+1;
        else palo_obiettivo =-1;
        
        if(palo_obiettivo ==-1) y = BFHEIGHT+1;
        else y = BFHEIGHT-(poles[palo_obiettivo]-1);  
      }
      if(d==DESTRA)
      {
        if(palo_corrente>0)
          palo_obiettivo = palo_corrente-1;
        else 
        if (palo_corrente == 0)palo_obiettivo=-1;
        else palo_obiettivo=NPOLES-1;
          
        if(palo_obiettivo ==-1)
          y = BFHEIGHT-HEIGHTCENTRALPOLE+1;
        else 
          y= BFHEIGHT-(poles[palo_obiettivo]-1);  
      }
    }
  if(t_or_g_1_or_2 == 2)
    {
      if(d == SINISTRA && x_old == XCENTRALPOLE) 
        d = DESTRA;
      
      if(d==DESTRA && x_old==(XCENTRALPOLE+(NPOLES+1)*SCALE))
        d=SINISTRA;
      
      if(d == DESTRA)
      {
        if(palo_corrente<(NPOLES-1))
          palo_obiettivo=palo_corrente+1;
        else palo_obiettivo =-1;
      
       if(palo_obiettivo ==-1) 
          y=BFHEIGHT+1;
        else y=BFHEIGHT-(poles[palo_obiettivo]-1);  
      }
      if(d == SINISTRA)
       {
	  
        if(palo_corrente>0)
          palo_obiettivo=palo_corrente-1;
        else 
        if (palo_corrente == 0)
          palo_obiettivo =-1;
        else
        palo_obiettivo = NPOLES-1;
	  
       if(palo_obiettivo ==-1)
          y = BFHEIGHT-HEIGHTCENTRALPOLE+1;
        else y = BFHEIGHT-(poles[palo_obiettivo]-1);  
      }
    }
      
  /*Coordinata X*/
  if(d == SINISTRA)
    if(x_old>(ll))
      {
        model_assegna_coordinate
        (giocatore->posX-SCALE,y,giocatore);
      }
  if(d == DESTRA)
    if(x_old<(rl))
      {
        model_assegna_coordinate
        (giocatore->posX+SCALE,y,giocatore);
      }
  
  salta(giocatore);
  
  if(giocatore->scambia == 1)
    {
      ritardo(jritardo);
      _scambia_pali();
      ritardo(jritardo);
    }
  cerca_vincitore();
  
}

/*_______________________________________________________
 *|
 *| Assegna gli attributi degli elementi del campo
 */
void model_inizializza_campo()
{
  view_mostra_erba();
  for (int i=0;i<NPOLES;i++)
  {
    view_mostra_palo(distanza_palo(i),'L',pali_tuki[i]);
    view_mostra_palo(distanza_palo(i),'R',pali_giuli[i]);
  }
  
  view_mostra_palo_centrale(HEIGHTCENTRALPOLE);
 
  /*Players positions*/
  tuki_gioca.posX = XCENTRALPOLE-NPOLES*SCALE;
  giuli_gioca.posX = XCENTRALPOLE+NPOLES*SCALE;
  giuli_gioca.posY = BFHEIGHT-pali_giuli[9];
  tuki_gioca.posY = BFHEIGHT-pali_tuki[9];
  
  view_mostra_giocatore(tuki_gioca);
  view_mostra_giocatore(giuli_gioca);
}

/*______________________________________________________
*|
*| Assegna l'altezza iniziale di tutti i pali del gioco
*| 
*/
void model_prepara_campo()
{

  for (int i = 0;i<NPOLES;i++)
  {
    pali_giuli[i] = i+1;
    pali_tuki[i] = i+1;
  }
  
  srand(time(NULL));
  
  for (int i = 0;i<50;i++)
  {
    for (int j = 0;j<NPOLES-1;j++)
    {
      if(rand()%2)
      {
        int t = pali_giuli[j];
        pali_giuli[j] = pali_giuli[j+1];
        pali_giuli[j+1] = t;
      }
    }
    
    for (int j = 0;j<NPOLES-1;j++)
    {
      if(rand()%2)
      {
        int t = pali_tuki[j];
        pali_tuki[j] = pali_tuki[j+1];
        pali_tuki[j+1] = t;
      }
    }
  }
}

 /*_______________________________________________________________
   *|
   *| Aggiorna le coordinate del giocarore <p> ai valori <x> e <y>
   *| e chiama il viewer per stamparlo
   */
void model_assegna_coordinate(int x, int y, Gioca *p)
{
  static int init = 0;
  init++;
  
  if(init>1){
    init = 1;
    p->posXold = p->posX;
    p->posYold = p->posY;
  }
  else
  {
    p->posXold = x;
    p->posYold = y;
  }
  
  p->posX = x;
  p->posY = y;
  view_mostra_giocatore(*p);
}

/*________________________________________________________
 *|
 *| Calcola la traiettoria parabolica del salto e chiama
 *| la funzione di stampa per mostrare l'intero salto
 */
void salta(Gioca * p)
{ 
  /*Coordinate monitor*/
  int xo = p->posXold;
  int yo = p->posYold;
  int x = p->posX;
  int yf = p->posY;
  
  /*Coordinate fisiche*/
  float a = x;
  float ao = xo;
  float bf = BFHEIGHT-yf;
  float bo = BFHEIGHT-yo;
  float b = bo;
  
  float dt = 0.005;/*secondo*/
  float t = 0;
  float g = 9.81;/*m/s^2*/
  float vy0;//m/s
  float vb;
  int ax = xo;
  int my,mb =-1;
  
  view_cancella_traccia_giocatore(p);

  /* Stabilisce la velocità iniziale del salto*/
  if((bf-bo)<0) vy0 = 3;
  else if((bf-bo) == 0)
    {
      vy0 = 5;
    }
  else 
    {
      vy0 = sqrt(2*g*(bf-bo)*1.5);
    }
  while(!(ax == a && (int)b == bf))
    {
      float _b = b;
      vb = vy0-g*(t);
      
      /* Moto uniformemente vario*/
      b = bo+vy0*t-0.5*g*(t*t);
      
      my = (int)(BFHEIGHT-b);
      t += dt;
      if(b>0 && b<BFHEIGHT)
        {
          
          if(vb>0)
            {
              fflush(stdout);
              ax = xo;
            }
          else
            {
              ax = a;
            }
          
          model_assegna_coordinate(ax, my, p);
          ritardo(1);
          mb = my;
        }
    }
  ritardo(1);
}

/*_______________________________________________________________
 *|
 *| Ritorna l'indice del palo sotto il giocatore
 *| 
 */
int indice_palo_corrente(Gioca p)
{
  char r = 0;
  int inx;
  int i = inv_distanza_palo(p.posX,&r);
  
  if(i>(NPOLES-1) || r != 0)
    {
      inx =-1;
    }
  else
    {
      inx =i;
    }
  return inx;
}

void ritardo(int millisecondi)
{
  long pausa;
  clock_t ora,fine;
  
  pausa = millisecondi*(CLOCKS_PER_SEC/1000);
  ora = fine = clock();
  while( (ora-fine) < pausa )
    ora = clock();
}

int distanza_palo(int pole_index)
{  
  return (pole_index+1)*SCALE; 
}

int inv_distanza_palo(int x,char *r)
{
  int sgn = 1;

  if(x<XCENTRALPOLE)sgn = -1;

  int dist = (x-XCENTRALPOLE)/SCALE*sgn-1;

  *r = (x-XCENTRALPOLE)%SCALE;
  
  if(dist<0)
    {
      *r=1;
    }
  return dist;
}

 
void scambia_pali()
{
  if(t_or_g_1_or_2 == 2)
    {
      giuli_gioca.scambia = 1;
    }
  else
    {
      tuki_gioca.scambia = 1;
    }
}

void _scambia_pali()
{
  fflush(stdout);
  Gioca* p;
  int *pole;
  char side;
  
  if(t_or_g_1_or_2 == 2)
    {
      giuli_gioca.scambia = 0;
      p = &giuli_gioca;
      side = 'R';
      pole = pali_giuli;
    }
  else
    {
      tuki_gioca.scambia = 0;
      p = &tuki_gioca;
      side = 'L';
      pole = pali_tuki;
    }
  
  /* _Non è possibile scambiare i pali se solo uno è marcato_ */
  if(p->marcato[0]==-1||p->marcato[1]==-1) return;

  view_evidenzia_palo(distanza_palo(p->marcato[1]),
                 side,
                 pole[p->marcato[1]],
                 distanza_palo(p->marcato[0]),
                 side,
                 pole[p->marcato[0]]);
 
  int tmp=pole[p->marcato[0]];
  
  pole[p->marcato[0]] = pole[p->marcato[1]];

  pole[p->marcato[1]] = tmp;

  view_mostra_palo(distanza_palo(p->marcato[1]),side,pole[p->marcato[1]]);

  view_mostra_palo(distanza_palo(p->marcato[0]),side,pole[p->marcato[0]]);

  view_smarca_palo(distanza_palo(p->marcato[1]),side,pole[p->marcato[1]]);

  view_smarca_palo(distanza_palo(p->marcato[0]),side,pole[p->marcato[0]]);
  
  p->marcato[0] = -1;
  p->marcato[1] = -1;
}

int altezza_palo()
{
  Gioca* p = &tuki_gioca;
  int *pole = pali_tuki;
  char side = 'L';
  if(t_or_g_1_or_2 == 2)
    {
      p = &giuli_gioca;
      side = 'R';
      pole = pali_giuli;
    }
  int i = indice_palo_corrente(*p);
  if(i >= 0)
    {
      return pole[i];
    }
  return -1;
}

int cancella_marca()
{
  Gioca* p = &tuki_gioca;
  if(t_or_g_1_or_2 == 2) p = &giuli_gioca;
    
  p->marcato[0] =-1;
  p->marcato[1] =-1;
  
  return -1;
}

int marca()
{
  Gioca* p;
  int *pole;
  char side;

  if(t_or_g_1_or_2 == 2)
    {
      p=&giuli_gioca;
      side = 'R';
      pole = pali_giuli;
    }
  else
    {
      p = &tuki_gioca;
      side = 'L';
      pole = pali_tuki;
    }
  int i = indice_palo_corrente(*p);
  if(i >= 0)
    {
      if(p->marcato[1]>=0)
    	{
        view_smarca_palo
        (distanza_palo(p->marcato[1]),side,pole[p->marcato[1]]);
      }
      p->marcato[1] = p->marcato[0];
      p->marcato[0] = i;
      view_marca_palo
      (distanza_palo(p->marcato[0]),side,pole[p->marcato[0]]);
      
      if(p->marcato[1]>=0){
      view_marca_palo
      (distanza_palo(p->marcato[1]),side,pole[p->marcato[1]]);
      fflush(stdout);
      }
    }
  return i;
}


int verifica_ordinamento(int array[])
{
  for(int i = 0;i<NPOLES-1;i++)
    {
      if(array[i]<array[i+1]) return 0;
    }
  return 1;
}

void cerca_vincitore()
{
  if(verifica_ordinamento(pali_tuki))
    {
      gioco = 0;
      view_tuki_win();
    }
  if(verifica_ordinamento(pali_giuli))
    {
      gioco = 0;
      view_giuli_win();
    }
}   
   

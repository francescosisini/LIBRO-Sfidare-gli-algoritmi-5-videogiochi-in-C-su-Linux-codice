/*______________________________
 *|
 *| tuki4_model.c
 */
      
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tuki4_model.h"

/*** section data ***/
int lanci_totali;
int energia;
int danni;


/** SECTION 1: preparazione **/
void model_inizializza()
{ 
  lanci_totali = 0;
  energia = ENERGIA;
  danni = DANNI;  
}

/** SECTION 2: comunicazione **/
int model_energia()
{
  return energia;
}

int model_danni()
{
  return danni;
}

int model_lanci_totali()
{
  return LANCI;
}

int model_lanci_restanti()
{
  return LANCI - lanci_totali;
}

/*______________________________________________________________
 *|
 *| Il model genera pseudo casualmente le coordinate del lancio
 */
lancio model_genera()
{
  double x,y;
  lancio l;
  
  x = (double)rand()/(double)RAND_MAX*(LARGHEZZA-2);
  y = (double)rand()/(double)RAND_MAX*(ALTEZZA-2);
  x -= (LARGHEZZA-2)/2;
  y -= (ALTEZZA-2)/2;
  l.x = (int)x;
  l.y = (int)y;

  return l;
}


int model_aggiorna_stato(evento e)
{
  if(e.l.proiettile == MELA)
  {
    if(e.az == MANGIA)
      {
        //Hai fatto bene
        energia += 1;
      }
    else
      {
        ;//Non fa nulla
      }
  }
  else
  {
    if(e.az==MANGIA)
      {
        danni += 2;
      }
    else
      {
        danni -= 1;
        energia += 1;
      }
  }

  /* Aggoirnamento */
  lanci_totali++;
  energia -= 1;
  
  if(danni>MAXDANNI||energia<0) return 0;
  return 1;
  
}

   
/*______________________________
 *|
 *| turno_tuki.c
 */
  
#include "giocatore.h"
#include <stdlib.h>
#include <time.h>


/**
       -REGOLE DEL GIOCO
       Il gioco consiste in una battaglia tra Giuli (oo) il 
       dinamitardo e Tuki (@>) l'artificere. 
       Giuli lancia dei pacchi conteneti o delle mele (O) o delle bombe (+).
       Le mele e le bombe sono indistinguibili se non per il fatto
       che le bombe esplodono mentre le mele nutrono.
       Giuli lancia mele e bombe in sue zone bene distinte del campo
       di gioco. Giuli conosce i confini delle due zone mentre Tuki no. 
       Tuki riesce a intercettare tutti i lanci di Giuli ma poi,
       una volta afferrato il pacco, deve decidere se distruggerlo 
       o mangiarlo.
       0) Ogni turno di gioco (lancio) Tuki perde 1 livello di energia
       1) Se mangia una mela acquista un livello di energia
       2) Se distrugge una mela non capita nulla
       3) Se mangia una bomba produce 2 livelli di danni
       4) se distrugge una bomba recupera un livello di danni e uno di energia
 */


/**
 * Implementa in questa funzione la decisione di Tuki 
 * sull'azione da prenere con il pacco
 * Al monento c'è una scelta casuale (ma anche un suggerimento)
 */
azione turno_tuki(int energia_rimasta, int danni_subiti, int x_pacco, int y_paco){
  static int init=0;
  if(!init)
    {
      srand(0);
      init=1;
    }
  
  azione azione_scelta;
  

  /* ...logica */
  if(rand()%2)
    azione_scelta=DISTRUGGI;
  else
    azione_scelta=MANGIA;
  
  return azione_scelta;
}


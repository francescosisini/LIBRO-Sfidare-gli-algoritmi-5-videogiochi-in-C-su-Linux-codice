   
/*______________________________
 *|
 *| tuki4_controller.c
 */
        
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "tuki4_model.h"
#include "tuki4_view.h"

/* RESPONSABILITA */
/**
  1) Attiva il turno di Giuli e di Tuki alternativamente
  2) verifica con il model le conseguenze delle loro azioni
  3) richiede al viewer di mostrare la scena
  4) verifica se il gioco è terminato
  
 */

/* MACRO  */
#define MAJOR    0
#define MINOR    1
#define RELEASE  1

/* GLOBALS  */


/* PROTOTYPES  */
azione turno_tuki(int energia, int danni,int x_pacco,int y_pacco);
pacco turno_giuli();

int main(int argc, char **argv)
{

  view_inizializza();
    
  setlocale(LC_CTYPE,"");
    
  model_inizializza();
  
  view_presentazione();
  
  view_campo();
  
  
  /**
   * INIZIO GIOCO
   */
  char c=0;
  int r=0;
  lancio l;
  evento e;
  
  while(c!='q'){
    int xb,yb;
    read(STDIN_FILENO,&c,1);

    l = model_genera();
    
    pacco pac = turno_giuli(l);

    l.proiettile = pac;
    
    azione a=turno_tuki(
                       model_energia(),
                       model_danni(),
                       l.x,
                       l.y);

    e.l=l;
    e.az=a;

    int s=model_aggiorna_stato(e);

    view_evento(e);

    if( model_lanci_restanti() == 0)
      {
        view_punteggio
        (model_energia(), model_danni(), model_lanci_restanti());
        
        view_gameover("Game Over: Ha vinto Tuki!!! :)");
        break;
      }
    if(s == 0)
      {
        view_gameover("Game Over: Ha vinto Giuli, tu hai perso :(");
        break;
      }
    
    view_punteggio
    (model_energia(), model_danni(), model_lanci_restanti());
  }  
  
}

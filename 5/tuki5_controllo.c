/*______________________________
*|  
*| tuki5_controllo.c
*/
    
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "tuki5_modello.h"
#include "tuki5_visore.h"

/** DESCRIZIONE */
/**
   - Nel campo di gioco sono presenti trifogli, pillole, muri

   - La distribuzione degli oggetti nel campo di gioco è memorizzata in un array 2d

   - La posizione dei 5 giocatori è memorizzata in una singola struttura
   
   - Ad ogni personaggio del gioco è associata una funzione con il nome: gioca_<personaggio>
   che torna la mossa che il personaggio fa in un turno di gioco.
   
*/
/**
   -game-controller.c
   Il controller offre ai giocatori le 5 funzioni di controllo
   a turno chiama le 5 funzioni per permettere ad ogni giocatore di giocare
   il controller chiama poi il model comunicando la mossa del giocatore
   
   -game-model.c 
   il model elabora la mossa del giocatore valuatando la nuova posizione e verifica:
   1) se avviene una collisione tra giocatori
   2) se avviene una collisione con il muro
   3) se viene mangiato un trifoglio
   4) se viene mangiata una pillola
   
   -game-viewer.c
   stampa sullo schermo lo stato del gioco
   
*/

/* MACRO  */
#define MAJOR    0
#define MINOR    1
#define RELEASE  1

/* GLOBALS  */

/* PROTOTIPI  */
direzione gioca_tuki(posizioni p, oggetto** lab);
direzione gioca_blinky(posizioni p, oggetto** lab);
direzione gioca_inky(posizioni p, oggetto** lab);
direzione gioca_pinky(posizioni p, oggetto** lab);
direzione gioca_clyde(posizioni p, oggetto** lab);



int main(int argc, char **argv)
{

  /* Presentazione */
  view_init();
  //view_presentazione();
  
  /* Inizio gioco */
  int inizio=  mdl_genera_campo();

  if(!inizio) exit(1);
  
  /* Labirinto */
  oggetto ** lab = mdl_campo();
  view_labirinto(lab);
  
  
  /**
   * START GAME
   */
  int r=1;
  while(r){
  
    posizioni p=mdl_posizioni();
    
    if(lab==0) exit(1);

    direzione t = gioca_tuki(p, lab);
    
    direzione b = gioca_blinky(p, lab);
    
    direzione i = gioca_inky(p, lab);
    
    direzione pi = gioca_pinky(p, lab);
    
    direzione c = gioca_clyde(p, lab);

    char go = mdl_passo(t,b,i,pi,c);
    int pnt = mdl_punteggio();
    char inblu=mdl_superpacman();

    /** mostra campo e giocatori */
    r+=1;
    view_punteggio(pnt);
    view_giocatori(p,lab,inblu);
    delay(150);

    /** Pacman è stato mangiato **/
    if(go==0)
      {
        view_mangiato(p);
        view_gameover("GAME OVER");
        exit(0);
      }

    /** Pacman ha finito le pastiglie 245 */
    if(pnt == 244)
      {
        view_gameover("Ha vinto TUKI!");
        exit(0);
      }
    
  }  
  /*end of the game*/
  
}



/*_______________________________________
*|
*| tuki1_controller.c
*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tuki1_model.h"
#include "tuki1_view.h"

#define MAJOR    0
#define MINOR    1
#define RELEASE  1

/* VARIABILI GLOBALI  */
Giocatore giuli_player, tuki_player;
int t_or_g_1_or_2; //Giocatore attivo

/* PROTOTIPI  */
int turno_tuki();
int turno_giuli();

int main(int argc, char **argv)
{
  /* Disabilita la tasiera */
  vista_init();
  
  strcpy(tuki_player.name, "tuki");
  strcpy(giuli_player.name, "giuli");
  setlocale(LC_CTYPE,"");
  
  if(modello_prepara_campo() != 0) exit(1);
  modello_giocatore(1,1,TUTF,&tuki_player);
  modello_giocatore(1,14,GUTF,&giuli_player);
  
  /* Presentazione del gioco */
  vista_presentazione();

  /* Visulalizzazione campo da gioco */
  vista_stampa_campo(modello_campo());
  
  char c=0;
  int r=0;
  
  while(c != 'q')
    {
      direzione d;
      int xb,yb;
      read(STDIN_FILENO,&c,1);
      
      
      t_or_g_1_or_2 = 1;
      xb = tuki_player.posX;
      yb = tuki_player.posY;
      
      d = turno_tuki();
      
      r = modello_passo(&tuki_player,d);
      
      vista_stampa_giocatore(&tuki_player);
      
      if(r == 2)
        {
          vista_gameover("Game Over: Tuki ha urtato il muro!");
          break;
        }
        
      if(r == 1)
        {
          if(tuki_player.score>=giuli_player.score)
            vista_gameover("Vince Tuki!");
          else
            vista_gameover("Vince Giuli!");
          break;
        }
      
      vista_stampa_sfondo(yb,xb,modello_campo());
      
      t_or_g_1_or_2 = 2;
      xb = giuli_player.posX;
      yb = giuli_player.posY;
      d = turno_giuli();
      
      r = modello_passo(&giuli_player,d);
      
      vista_stampa_giocatore(&giuli_player);
      vista_stampa_sfondo(yb,xb,modello_campo());
      
      if(r == 2)
        {
          vista_gameover("Game Over: Giuli ha urtato il muro!");
          break;
        }
        
      if(r == 1)
        {
          if(tuki_player.score <= giuli_player.score)
            vista_gameover("Vince Giuli!");
          else
            vista_gameover("Vince Tuki!");
          break;
        }
      
      vista_punteggio
      (tuki_player.score, giuli_player.score,
       modello_numero_fiori());
    }  
  
  modello_libera();
}

/*________________________________________________________
 *|
 *| Usata dal giocatore per leggere il campo da gioco
 */
elemento_campo controller_leggi_elemento_campo(direzione d)
{
  Giocatore *pl = t_or_g_1_or_2 == 1?&tuki_player:&giuli_player;
  return(modello_elemento_in_campo(d,pl));
}

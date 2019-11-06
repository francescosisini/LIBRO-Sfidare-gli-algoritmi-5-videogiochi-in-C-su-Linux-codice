/*__________________________________________________________
 *|
 *| giocatore.h
 */

#ifndef PLAYER
typedef enum  {SINISTRA,ALTO
               ,DESTRA,BASSO,IN} direzione;
#define PLAYER 
#endif

/*___________________________________________________
 *|
 *| Elementi del campo di gioco
 */
enum cmp{
  GUTF =9787, //Giuli
  TUTF =0040, //Tuki
  TLC = 9487, //Angolo sinistro alto
  TRC = 9491, //Angolo destro alto
  BRC = 9499, //Angolo destro basso
  BLC = 9495, //Angolo sinistro basso
  HLN = 9473, //Muro orizzontale 
  VLN = 9475, //Muro verticale
  TT =  9531, //Giunzione alto
  TB =  9523, //Giunzione basso
  TL =  9507, //Giunzione sisnistra
  TR =  9515, //Giunzione destra
  FUTF =9752  //Fiore (trifoglio) 
};

typedef enum cmp elemento_campo;

/*_________________________________________________________
 *|
 *| Legge il campo in corrispondenza della posizione/cella 
 *| successiva a quella occupata dal giocarore, in direzione
 *| 
 */
elemento_campo controller_leggi_elemento_campo( direzione d);

/*_________________________________________________________
 *|
 *| Crea una pausa nell'esecuzione del gioco. Da usare per
 *| rallentare l'eseczione e osservarla meglio
 */
void delay(int milliseconds);

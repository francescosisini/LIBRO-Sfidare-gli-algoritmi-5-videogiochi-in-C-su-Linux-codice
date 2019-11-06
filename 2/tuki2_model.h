/*__________________________________
*|
*| tuki2_model.h
*/   

#ifndef TUKI2_MODEL_H
#define TUKI2_MODEL_H

#define GUTF 9787
#define TUTF 0040
#define SADGUTF 9785

#define DELAY 50
#define NPOLES 10
#define HEIGHTCENTRALPOLE 12
#define SCALE 2

typedef unsigned int utf8code;

typedef struct  {

  /*__________________________________
   *|
   *| Posizione del giocatore
   */
  int posX; 
  int posY;

   /*_________________________________
   *|
   *| Posizione passata del giocatore
   */
  int posXold; 
  int posYold;

   /*________________________________
   *|
   *| Direzione di spostamento
   *| scelta dal giocatore
   */
  direzione dir;

  /*_________________________________
   *|
   *| Indice dei due pali marcati
   *| marcato[0] indice primo palo
   *| marcato[1] indice secondo palo
   *| -1->non marcato
   */
  int  marcato[2];
  
   /*_________________________________
   *|
   *| Logico 1,0
   *| 1-> il giocatore vuole scambiare
   *| i pali marcati
   */
  int scambia;
  
  char name[10];

  utf8code UTF;
 }
  Gioca;

/*__________________________________________________
 *|
 *| Funzioni pubbliche chiamate dal controller
 */
void model_assegna_coordinate(int x, int y, Gioca *p);
void model_inizializza_campo();
void model_prepara_campo();
void model_turno(direzione d, Gioca *giocatore);

/*__________________________________________________
 *|
 *| Funzione pubbliche chiamate dai giocatori
 */
int altezza_palo();
int cancella_marca();
int marca();
void scambia_pali();

/*__________________________________________________
 *|
 *| Funzione private
 */
void cerca_vincitore();
int distanza_palo(int pole_index);
int indice_palo_corrente(Gioca p);
int inv_distanza_palo(int x,char *r);
void ritardo(int millisecondi);
void salta(Gioca * p);
void _scambia_pali();
int verifica_ordinamento(int array[]);

#endif
   

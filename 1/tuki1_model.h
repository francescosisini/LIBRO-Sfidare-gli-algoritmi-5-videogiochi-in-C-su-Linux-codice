/*__________________________________________________________
 *|
 *| tuki1_model.h
 */

#define CAMPO_ALTEZZA 16 
#define CAMPO_LARGHEZZA 16

/*__________________________________________________________
 *|
 *| Elementi presenti nel campo di gioco
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

#ifndef PLAYER
typedef enum  {SINISTRA,ALTO,DESTRA,BASSO,IN} direzione;
#define PLAYER 
#endif

typedef enum {UL, UR, DR,DL,FC} angolo;

/*_____________________________________________________________
 *|
 *| Il giocatore
 */
typedef struct  {
  int posX; 
  int posY; 
  direzione dir; 
  int score;
  char name[10];
  int code;
  elemento_campo UTF;
} Giocatore;

enum chartype 
{WALL,ITEM,OBSTACLE,DOOR,FOOD,
FLOWER,UNKNOWN,GIULI,TUKI,EMPTY};

typedef enum chartype char_type;



/*__________________________________________________________________
 *|
 *| Funzioni private
 */
void aggiungi_ostacolo_quadrato(int xc,int yc,int l);
char_type tipo_carattere(elemento_campo code);

/*__________________________________________________________________
 *|
 *| Funzioni pubbliche
 */
void modello_giocatore(int x, int y, elemento_campo e, Giocatore *p);
void modello_libera();
elemento_campo modello_elemento_in_campo(direzione d,Giocatore *ap);
int modello_numero_fiori();
int modello_passo(Giocatore*,direzione);
elemento_campo ** modello_campo();
int modello_prepara_campo();
void modello_ruota_giocatore(direzione d, Giocatore * p);

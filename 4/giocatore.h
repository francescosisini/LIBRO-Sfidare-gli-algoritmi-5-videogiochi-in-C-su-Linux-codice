/*______________________________
 *|
 *| giocatore.h
 */
     
#define ALTEZZA 32 
#define LARGHEZZA 32

#ifndef PLAYER

typedef enum  {MELA,BOMBA} pacco;
typedef enum  {MANGIA,DISTRUGGI} azione;
typedef struct  {
  int x; 
  int y; 
  pacco proiettile; 
} lancio;

#define PLAYER 
#endif


/*______________________________
 *|
 *| tuki4_model.h
 */   
         
#define ALTEZZA 32 
#define LARGHEZZA 32
#define LANCI 300
#define ENERGIA 100
#define DANNI 0
#define MAXDANNI 100


/*** types ***/
#ifndef PLAYER
typedef enum  {MELA,BOMBA} pacco;
typedef enum  {MANGIA,DISTRUGGI} azione;
typedef struct  {
  int x; 
  int y; 
  pacco proiettile; //cosa ha lanciato
} lancio;
#define PLAYER 
#endif

typedef struct {
  lancio l;
  azione az;
} evento;

/*** interface ***/
int model_aggiorna_stato(evento le);
void model_inizializza();
int model_energia();
int model_danni();
lancio model_genera();
int model_lanci_totali();
int model_lanci_restanti();
  
   

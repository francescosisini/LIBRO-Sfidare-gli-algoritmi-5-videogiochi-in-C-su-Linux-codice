   
/*_________________________________
*|
*| giocatore.h
*/
 
#define DIM 5
typedef enum {GEMMA,MELANZANA,POZIONE} tipo_oggetto;
typedef enum {MANGIA,SALTA,PRENDI} azione;
typedef struct 
{
  float nutrienti;
  float tossine;
  float energia;
} stato;

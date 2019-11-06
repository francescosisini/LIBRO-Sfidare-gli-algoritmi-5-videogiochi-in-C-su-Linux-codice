/*_________________________________________
 *|
 *| giocatore.h
 */

#define DIREZIONI
/*_________________________________________
 *|
 *| Direzione scelta dal giocarore 
 *| al termine del turno di gioco
 */
typedef enum  {SINISTRA,DESTRA,IN} direzione;

/*__________________________________________
 *|
 *| Marca/smarca il palo sotto al giocatore
 *| per essere scambiato
 *| Ritorna l'altezza del paolo
 */
int marca();
int cancella_marca();

/*__________________________________________
 *|
 *| Scambia i pali marcati
 */
void scambia_pali();

/*__________________________________________
 *|
 *| Altezza del palo sotto il giocatore
 */
int altezza_palo();

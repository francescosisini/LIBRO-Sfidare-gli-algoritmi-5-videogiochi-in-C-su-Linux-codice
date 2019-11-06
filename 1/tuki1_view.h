/*_______________________________________________________
 *| 
 *| Funzioni pubbliche
 */
void vista_gameover(char * message);
int vista_init();
void vista_punteggio(int t_score, int g_score, int flowers);
void vista_presentazione();
int vista_stampa_campo(elemento_campo **maze);
int vista_stampa_giocatore(Giocatore * p);
int vista_stampa_sfondo(int r, int c,elemento_campo **maze);

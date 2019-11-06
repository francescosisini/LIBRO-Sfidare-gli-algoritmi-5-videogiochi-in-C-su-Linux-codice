/*______________________________________________
 *|
 *| tuki2_view.h
 */
   
#ifndef TUKI2_VIEW_H
#define TUKI2_VIEW_H

#include "tuki2_model.h"

#define BFWIDTH NPOLES*2+1+30
#define BFHEIGHT NPOLES+2+2+1+5
#define BASELINE BFHEIGHT
#define XCENTRALPOLE (BFWIDTH)/2
#define BGC 49
#define GGC 32

#define COFFSET(width) (cols-(width))/2
#define ROFFSET(heigh) (rows-(heigh))/2
#define MARGINI(str) (BFWIDTH-strlen(str))/2.



void view_mostra_palo(int raggio, char lato,int altezza);

void view_evidenzia_palo(int d1, char side1,int l1,int d2, char side2,int l2);

void view_mostra_giocatore(Gioca p);

void view_cancella_traccia_giocatore(Gioca* p);

void view_mostra_erba();

void view_mostra_palo_centrale(int l);

void view_marca_palo(int d, char side,int l);

void view_smarca_palo(int d, char side,int l);

void view_presentazione();

void view_opzioni();

void view_istruzioni();

void view_cancella_messaggio();

void view_tuki_win();

void view_giuli_win();
#endif

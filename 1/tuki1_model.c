/*________________________
*|
*| tuki1_model.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tuki1_model.h"

elemento_campo ** bf;

Giocatore tuki_player;

Giocatore giuli_player;

int flowers_number;

void aggiungi_ostacolo_quadrato(int xc,int yc,int l)
{
  if(l<2) return;
  
  int xl = 0+xc-l/2;
  int xr = xl+l-1;
  int yt = 0+yc-l/2;
  int yb = yc+l-1;

  if(xl >= 0 && xr < CAMPO_LARGHEZZA
   && yt >= 0 && yb < CAMPO_LARGHEZZA)
    {
      bf[yt][xl] = TLC;
      bf[yb][xl] = BLC;
      bf[yb][xr] = BRC;
      bf[yt][xr] = TRC;
      
      for(int i=xl+1;i<xr;i++)bf[yt][i]=HLN;
      for(int i=xl+1;i<xr;i++)bf[yb][i]=HLN;
      for(int i=yt+1;i<yb;i++)bf[i][xl]=VLN;
      for(int i=yt+1;i<yb;i++)bf[i][xr]=VLN;
      
      /* Elimina i fiori sotto l'ostacolo */
      for(int i=xl+1;i<xr;i++)
        {
          for(int j=yt+1;j<yb;j++)
            {
              bf[j][i] = 0;
            }
        }
            
    for(int i=yt;i<=yb;i++)
      bf[i][xr+1] = 0;

    for(int i=yt;i<=yb;i++)
    bf[i][xl-1] = 0; 
  }
}

void modello_libera()
{
  for(int in=0;in<CAMPO_ALTEZZA;in++)
    {
      free(*(bf+in));
    }
  free(bf);
}

int modello_prepara_campo()
{
  int level = 1;
  int l = 1, i;
  int h = CAMPO_ALTEZZA;
  int w = CAMPO_LARGHEZZA;
  
  /* Riserva la memoria per gli elementi del campo */
  bf = (elemento_campo**)malloc(h*sizeof(elemento_campo*));
  if(!bf) return 1;
  for(int in=0;in<h;in++)
    {
      *(bf+in) = malloc(w*sizeof(elemento_campo));
      if(!(*(bf+in))) return 1;
    }
    
  /*Riempie il campo di trifogli*/
  for(int i=1;i<h-1;i+=1)
    {
      for(int j=1;j<w-1;j+=1)
	{
	  bf[i][j] = FUTF;
	}
    }

  /* Disegna il labirinto */
  
  /* Angolo alto a sinistra*/
  bf[0][0] = TLC;

  /* Muro sopra*/
  for(int j=1;j<CAMPO_LARGHEZZA-1;j++)
    {
      bf[0][j] = HLN;
    } 

  /* Angolo alto a destra*/
  bf[0][CAMPO_LARGHEZZA-1] = TRC;

  /* Muro destro*/
  for(i=1;i<CAMPO_ALTEZZA-1;i++)
    {
      bf[i][CAMPO_LARGHEZZA-1] = VLN;

    }
  
  /* Angolo basso destra*/
  bf[CAMPO_ALTEZZA-1][CAMPO_LARGHEZZA-1]=BRC;

  /*Muro basso*/
  for(i=CAMPO_LARGHEZZA-2;i>0;i--)
    {
      bf[CAMPO_ALTEZZA-1][i]=HLN;
    }
  
  /*Angolo basso sinistra*/
  bf[CAMPO_ALTEZZA-1][0]=BLC;
  
  /*Muro sinistra*/
  for(i=CAMPO_ALTEZZA-2;i>0;i--)
    {
      bf[i][0]=VLN;	
    }
  
  /* Giunti e tramezze */
  bf[CAMPO_ALTEZZA-1][CAMPO_LARGHEZZA/2]=TT;
  bf[CAMPO_ALTEZZA-2][CAMPO_LARGHEZZA/2]=VLN;
  bf[CAMPO_ALTEZZA-3][CAMPO_LARGHEZZA/2]=VLN;

  bf[0][CAMPO_LARGHEZZA/2]=TB;
  bf[1][CAMPO_LARGHEZZA/2]=VLN;
  bf[2][CAMPO_LARGHEZZA/2]=VLN;

  bf[CAMPO_ALTEZZA/2][0]=TL;
  bf[CAMPO_ALTEZZA/2][1]=HLN;
  bf[CAMPO_ALTEZZA/2][2]=HLN;

  bf[CAMPO_ALTEZZA/2][CAMPO_LARGHEZZA-1]=TR;
  bf[CAMPO_ALTEZZA/2][CAMPO_LARGHEZZA-2]=HLN;
  bf[CAMPO_ALTEZZA/2][CAMPO_LARGHEZZA-3]=HLN;

  switch (level){
  case 0:
    break;
    
  case 1:
    srand(time(NULL));
    int gg=rand()%3;
    int gh=rand()%2;
    
    aggiungi_ostacolo_quadrato
    (CAMPO_LARGHEZZA/(4)-1+gg,CAMPO_ALTEZZA/4+gh,2);
    
    gg=rand()%3;
    gh=rand()%2;
    
    aggiungi_ostacolo_quadrato
    (CAMPO_LARGHEZZA*(3.0/4.0)+1-gg,CAMPO_ALTEZZA/4-gh,2);
    
    gg=rand()%3;
    gh=rand()%2;
    
    aggiungi_ostacolo_quadrato
    (CAMPO_LARGHEZZA*(3.0/4.0)+1-gg,CAMPO_ALTEZZA*(3.0/4.0)-1-gh,2);
    
    gg=rand()%3;
    gh=rand()%2;
    
    aggiungi_ostacolo_quadrato
    (CAMPO_LARGHEZZA*(1./4.0)-1+gg,CAMPO_ALTEZZA*(3.0/4.0)-1+gh,2);
    
    break;
  }
  
  /* Conta i fiori dopo la preparazione del campo*/
  for(int i=0;i<h;i+=1)
    for(int j=0;j<w;j+=1){
      if(bf[i][j]==FUTF) flowers_number++;
    }
  
  return 0;
}

elemento_campo ** modello_campo()
{
  return bf;
}

elemento_campo modello_elemento_in_campo(direzione d,Giocatore *ap)
{  
  int x,y;
  x = ap->posX;
  y = ap->posY;
  x=x+1*(d==DESTRA);
  x=x-1*(d==SINISTRA);
  y=y-1*(d==ALTO);
  y=y+1*(d==BASSO);

  if(x<0 || x>=CAMPO_LARGHEZZA || y<0 || y>=CAMPO_ALTEZZA)
  {
    return -1;
  }
  return bf[y][x];
}

void  modello_ruota_giocatore(direzione d, Giocatore* player)
{
  switch(d)
    {
    case ALTO:
      {
	player->dir=ALTO;
	break;
      }
    case DESTRA:
      {
	player->dir=DESTRA;
	break;
      }
    case BASSO:
      {
	player->dir=BASSO;
	break;
      }
    case SINISTRA:
      {
	player->dir=SINISTRA;
	break;
      }
    case IN:
      {
	player->dir=IN;
	break;
      }
    }
}

char_type tipo_carattere(elemento_campo code)
{
  if(code==TLC||code==TRC||code==BRC
  ||code==BLC||code==HLN||code==VLN||code==TT||code==TB)
    {
      return WALL;
    }
  
  if(code==0){
    return EMPTY;
  }
  
  if(code==FUTF){
    return FLOWER;
  }
  if(code==GUTF){
    return GIULI;
  }
  if(code==TUTF){
    return TUKI;
  }
  
  if(code==10210||code==9830){
    return ITEM;
  }
  return UNKNOWN;
}


/*___________________________________________________
 *|
 *| Esegue un passo nella direzione indicata
 *| torna 2 se urta le pareti
 *| torna 1 se ha finito i trifogli
 *| toena 0 se non è successo niente
 */
int modello_passo(Giocatore *player, direzione d)
{ 
  int x_old=player->posX;
  int y_old=player->posY;

  char_type ct=UNKNOWN;
  
  switch(d)
    {
    case IN:
      return 0;
      break;
      
    case SINISTRA:
      ct=tipo_carattere(bf[y_old][x_old-1]);
      if(ct!=WALL&&x_old>0)  player->posX--;else return 2;
      break;
    case DESTRA:
      ct=tipo_carattere(bf[y_old][x_old+1]);
      if(ct!=WALL&&x_old<(CAMPO_LARGHEZZA-1)) 
        player->posX++; 
      else return 2;
      break;
    case ALTO:
      ct=tipo_carattere(bf[y_old-1][x_old]);
      if(ct!=WALL&&y_old>0)
       player->posY--;
      else return 2;
    break;
    case BASSO:
      ct=tipo_carattere(bf[y_old+1][x_old]);
      if(ct!=WALL&&y_old<CAMPO_ALTEZZA-1)
       player->posY++;
      else return 2;
      break;
    }
  
  /* Verifica se mangia un fiore*/  
  if(ct==FLOWER){
    player->score+=1;
    flowers_number--;
    bf[player->posY][player->posX]=0;  
  }
  
  if(flowers_number==0)return 1;
  
  return 0;
}

/*________________________________________________________
 *|
 *| Permette al controller di controllare il giocatore
 */
void modello_giocatore
(int x, int y, elemento_campo e, Giocatore *p)
{
  p->posX = x;
  p->posY = y;
  p->UTF = e;
  
  if(bf[y][x] == FUTF)
    {
      flowers_number--;
      bf[y][x] = 0;
    }
}

int modello_numero_fiori()
{
  return flowers_number;
}

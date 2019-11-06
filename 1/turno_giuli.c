    
/*_______________________________________________
 *|
 *| turno_giuli.c
 */

#include "giocatore.h"
#include <stdio.h>
#include <stdlib.h>

#define GUTF 9787
#define TUTF 0040
#define SADGUTF 9785
/*utf del campo*/
#define TLC 9487
#define TRC 9491
#define BRC 9499
#define BLC 9495
#define HLN 9473
#define VLN 9475
#define TT 9531
#define TB 9523
#define TL 9507
#define TR 9523
#define FUTF 9752

/**
 * Algoritmo basato su The Boustrophedon Cellular Decomposition
 * di Choset e Pignon
 * Giuli ad ogni passo crea un grafo collegando le celle di confine
 * alla cella in cui si trova e classificandole come già visitate
 * oppure no e come accedibili o non accedibili
 * Nello svolgimento del gioco il grafo cresce e permette a Giuli
 * di avere una mappa sempre più completa del campo
 * Quando Giuli si trova in un punto in cui tutte le celle vicine
 * sono ostacoli o sono già state visitate, ripercorre il proprio
 * cammino all'indietro fino alla prima cella che presenta almeno 
 * una vicina accedibile e non visitata 
 */

/* tipo di cella */
enum chartype 
{WALL,ITEM,FLOWER,UNKNOWN,TUKI,EMPTY};

typedef enum chartype char_type;

/* attributi della cella */
typedef struct data
{
  int readed;
  int visited;
  char_type cell_type;
  int cont;
} Data;

/* celle del campo */
typedef struct cell {
  Data d;
  struct cell* right;
  struct cell* down;
  struct cell* left;
  struct cell* up;
} Cell;

typedef Cell* cella;

typedef struct node{
  cella pcell;
  struct node * next;
  struct node * prev;
}Node;

typedef Node* cammino;  

/*Prototipi*/
char_type tipo_cella(elemento_campo code);

void crea_cella(cella* p_c);

cella collega_alla_cella(cella cel,Data d,direzione dir);

void crea_cammino(cammino* p);

void aggiungi_al_cammino(cammino* l, cella c);

Cell* cella_vicina(cammino ap,direzione dir);

int nn_g = 0;
int moves_g = 0;

direzione turno_giuli(){
  
  static int init = 0;
  static cella g = NULL;
  static cammino p = NULL;
  static cammino l = NULL;
  moves_g++;
  
  if(!init)
    {
      init = 1;
      
      /*Inizializza il campo*/
      
      crea_cella(&g);      
      cella fn = (cella)malloc(sizeof(Cell));
      g = fn;
      g->d.visited = 1;
      g->d.cell_type = EMPTY;
      g->d.cont = -1;
      
      crea_cammino(&p);
      aggiungi_al_cammino(&p,g);
    }
  
  unsigned char c[4];
  elemento_campo cx;
  
  /*La cella attuale*/
  g = p->pcell;
  if(!g) exit(-1);

  //ALTO
  cx = controller_leggi_elemento_campo(ALTO);
  if(g->up == NULL)
    {
      cella gc = cella_vicina(p,ALTO);
      if(gc)
      {
        g->up = gc;
        gc->down = g;
      }
      else
      {         
        Data d={1,0,tipo_cella(cx)};
        cella tg = collega_alla_cella(g,d,ALTO);
      }
    }
    else
    {
      g->up->d.readed=1;
      g->up->d.cell_type = tipo_cella(cx);
    }
  
  //Basso
  cx = controller_leggi_elemento_campo(BASSO);
  if(g->down == NULL)
    {
      cella gc = cella_vicina(p,BASSO);
      if(gc)
      {
        g->down=gc;
        gc->up=g;
      }
      else
      {
        Data d = {1,0,tipo_cella(cx)};
        cella tg = collega_alla_cella(g,d,BASSO);
      }
    }
  else
    {
      g->down->d.readed = 1;
      g->down->d.cell_type = tipo_cella(cx);
    }

  //Destra
  cx = controller_leggi_elemento_campo(DESTRA);
  if(g->right == NULL)
    {
      cella gc = cella_vicina(p,DESTRA);
      if(gc)
      {
        g->right=gc;
        gc->left=g;
      }
      else
      {
        Data d = {1,0,tipo_cella(cx)};
        cella tg = collega_alla_cella(g,d,DESTRA);
      }
    }
  else
    {
      g->right->d.readed=1;
      g->right->d.cell_type=tipo_cella(cx);
    }
  
  //Sinistra
  cx = controller_leggi_elemento_campo(SINISTRA);
  if(g->left == NULL)
    {
      cella gc = cella_vicina(p,SINISTRA);
      if(gc)
        {
          g->left = gc;
          gc->right = g;
        }
      else
        {
          Data d = {1,0,tipo_cella(cx)};
          cella tg = collega_alla_cella(g,d,SINISTRA);
        }
    }
  else
    {
      g->left->d.readed = 1;
      g->left->d.cell_type = tipo_cella(cx);
    }
     
 	
  if(g->left->d.visited == 0
   && g->left->d.cell_type!= WALL)
    {
      g = g->left;
      g->d.visited = 1;
      aggiungi_al_cammino(&p,g);
      l = p;
      return SINISTRA;
    }

  if(g->up->d.visited == 0
   && g->up->d.cell_type != WALL)
    {
      g = g->up;
      g->d.visited = 1;
      aggiungi_al_cammino(&p,g);
      l = p;
      return ALTO;
    }
      
  if(g->right->d.visited == 0
   && g->right->d.cell_type != WALL)
    {
      g = g->right;
      g->d.visited = 1;
      aggiungi_al_cammino(&p,g);
      l = p;
      return DESTRA;
    }
  
  if(g->down->d.visited == 0
   && g->down->d.cell_type != WALL)
    {
      g = g->down;
      g->d.visited = 1;
      aggiungi_al_cammino(&p,g);
      l = p;
      return BASSO;
    }
 
  l = l->prev;
  if(l == NULL) exit(-1);

  aggiungi_al_cammino(&p,l->pcell);

  direzione nd;
  if(g->right == l->pcell) nd = DESTRA;
  else
  if(g->left == l->pcell) nd = SINISTRA;
  else
  if(g->up == l->pcell) nd = ALTO;
  else
  if(g->down == l->pcell) nd = BASSO;

  return nd;
  
}

Cell* cella_vicina(cammino ap,direzione dir)
{
  int R=0,U=0;
  int found=0;
  
  while(ap->prev){
    if(ap->pcell->right == ap->prev->pcell) R++;
    if(ap->pcell->left == ap->prev->pcell) R--;
    if(ap->pcell->up == ap->prev->pcell) U++;
    if(ap->pcell->down == ap->prev->pcell) U--;
    
   
    if(dir == DESTRA && R == 1 && U == 0)
      {
        return ap->prev->pcell;
      }
    
    if(dir == DESTRA && R == 1 && U == 1)
      {
        return ap->prev->pcell->down;
      }
    
    if(dir == DESTRA && R == 1 && U ==-1)
      {
        return ap->prev->pcell->up;
      }
    
    if(dir == SINISTRA && R ==-1 && U == 0)
      {
        return ap->prev->pcell;
      }
    
    if(dir == SINISTRA && R ==-1 && U == 1)
      {
        return ap->prev->pcell->down;
      }
    
    if(dir == SINISTRA && R ==-1 && U ==-1)
      {
        return ap->prev->pcell->up;
      }

    if(dir == ALTO && R == 0 && U == 1)
      {
        return ap->prev->pcell;
      }
    
    if(dir == ALTO && R==-1 && U ==1)
      {
        return ap->prev->pcell->right;
      }
    
    if(dir == ALTO && R == 1 && U == 1)
      {
        return ap->prev->pcell->left;
      }

    if(dir == BASSO && R == 0 && U ==-1)
      {
        return ap->prev->pcell;
      }
    
    if(dir == BASSO && R==-1 && U ==-1)
      {
        return ap->prev->pcell->right;
      }
    
    if(dir == BASSO && R == 1 && U ==-1)
      {
        return ap->prev->pcell->left;
      }

    ap = ap->prev;
  }
  return NULL;
}

char_type tipo_cella(elemento_campo code)
{
  if(code == TLC || code == TRC || code == BRC 
  || code == BLC ||code == HLN || code == VLN 
  || code == TT || code == TB)
  {
    return WALL;
  }
  
  if(code == 0){
    return EMPTY;
  }
  
  if(code == FUTF){
    return FLOWER;
  }
  if(code == GUTF){
    return TUKI;
  }
  if(code == TUTF){
    return TUKI;
  }
  
  if(code == 10210 || code == 9830){
    return ITEM;
  }
  return UNKNOWN;
}

void crea_cella(cella* g)
{
  *g = NULL;
}

cella collega_alla_cella(cella pcell,Data d,direzione dir)
{
  nn_g++;
  Cell* n = (Cell*)malloc(sizeof(Cell));
  n->d = d;
  n->d.cont = nn_g;
 
  switch(dir)
    {
    case ALTO:
      pcell->up = n;
      n->down = pcell;
      break;
    case BASSO:
      pcell->down = n;
      n->up = pcell;
      break;
    case DESTRA:
      pcell->right = n;
      n->left = pcell;
      break;
    case SINISTRA:
      pcell->left = n;
      n->right = pcell;
      break;
    }
  return n;
}

void crea_cammino(cammino* p)
{
  *p=0;
}
void aggiungi_al_cammino(cammino* l, cella g) 
{
  Node* aux = (Node*)malloc(sizeof(Node));
  
  aux->prev = *l;
  aux->next = NULL;  
  aux->pcell = g;
  if(*l)(*l)->next=aux;
  *l = aux;
}



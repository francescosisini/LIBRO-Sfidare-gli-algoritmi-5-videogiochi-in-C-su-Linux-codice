#include "giocatore.h"
#include <stdio.h>
#include <stdlib.h>

#define GUTF 9787
#define TUTF 0040
#define SADGUTF 9785
/*battlefield utf*/
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

enum chartype {WALL,ITEM,OBSTACLE,DOOR,FOOD,FLOWER,ZORZ,UNKNOWN,Tuki,TUKI,EMPTY};
typedef enum chartype char_type;

/**
 *Battlefield cells graph representation
 */
typedef struct data
{
  int readed;
  int visitata;
  char_type cell_type;
  int cont;
} Data;

typedef struct cell {
  Data d;
  struct cell* destra;
  struct cell* basso;
  struct cell* sinistra;
  struct cell* alto;
} Cell;
typedef Cell* graph;

/**
 *Cell path list
 */
typedef struct node{
  graph pcell;
  struct node * next;
  struct node * prev;
}Node;
typedef Node* path;  



/*Functions*/

char_type get_cell_type2(elemento_campo code);
direzione symmetry2(direzione d);
void new_graph2(graph* g);
graph add_to_graph2(graph pcell,Data d,direzione dir);
void new_path2(path* p);
void add_head2(path* l, graph g);
int decode_char2(unsigned char* r);
Cell* get_neighbor2(path ap,direzione inquired_dir);

FILE *f;


int nn2=0;
int moves2=0;
/**
 * Tuki algorithm based on The Boustrophedon Cellular Decomposition
 * Choset e Pignon
 */
direzione turno_tuki(){
  //FILE * f=fopen("debug.txt","a");
  
  static int init=0;
  static graph g=NULL;
  static path p=NULL;
  static path l=NULL;
  moves2++;
  if(!init)
    {
      init=1;
      /* 1)Start with any cell in the decomposition. 
       * Insert it into the path list. Mark it as visitata*/
      /* The list of visitata cells*/

      /*the battlefield*/
      new_graph2(&g);      
      graph fn=(graph)malloc(sizeof(Cell));
      g=fn;
      g->d.visitata=1;
      g->d.cell_type=EMPTY;
      g->d.cont=-1;
      /*The cell list (path in the battlefield)*/
      new_path2(&p);
      add_head2(&p,g);
    }else
    {
      
    }
  
  /*Assigns the neighbors cells */
  unsigned char c[4];
  elemento_campo cx;
  
  /*This is the cell where Tuki is*/
  g=p->pcell;
  if(!g) exit(-1);

  //Alto
  cx =controller_leggi_elemento_campo(ALTO);
  //fprintf(f,"dir ALTO readed %d ",cx);
  if(g->alto==NULL)
    {
      graph gc=get_neighbor2(p,ALTO);
      if(gc){
	g->alto=gc;
	gc->basso=g;
      }else
	{
          
	  Data d={1,0,get_cell_type2(cx)};
	  graph tg=add_to_graph2(g,d,ALTO);
	}
    }else
    {
      g->alto->d.readed=1;
      g->alto->d.cell_type=get_cell_type2(cx);
    }
  
  //Basso
  cx =controller_leggi_elemento_campo(BASSO);
  //fprintf(f,"dir BASSO readed %d ",cx);
  if(g->basso==NULL)
    {
      graph gc=get_neighbor2(p,BASSO);
      if(gc)
	{
	  g->basso=gc;
	  gc->alto=g;
	}
      else
	{
	  Data d={1,0,get_cell_type2(cx)};
	  graph tg=add_to_graph2(g,d,BASSO);
	}
    }else
    {
      g->basso->d.readed=1;
      g->basso->d.cell_type=get_cell_type2(cx);
    }

  //Destra
  cx =controller_leggi_elemento_campo(DESTRA);
  //fprintf(f,"dir DESTRA readed %d ",cx);
  if(g->destra==NULL)
    {
      /*check if the cell already exists*/
      graph gc=get_neighbor2(p,DESTRA);
      if(gc){
	g->destra=gc;
	gc->sinistra=g;

      }else
	{
	  Data d={1,0,get_cell_type2(cx)};
	  graph tg=add_to_graph2(g,d,DESTRA);

	}
    }else
    {
      g->destra->d.readed=1;
      g->destra->d.cell_type=get_cell_type2(cx);
    }
  
  //Sinistra
  cx =controller_leggi_elemento_campo(SINISTRA);
  //fprintf(f,"dir SINISTRA readed %d ",cx);
  if(g->sinistra==NULL)
    {
      graph gc=get_neighbor2(p,SINISTRA);
      if(gc)
	{
	  g->sinistra=gc;
	  gc->destra=g;
	}else
	{
	  Data d={1,0,get_cell_type2(cx)};
	  graph tg=add_to_graph2(g,d,SINISTRA);
	}
    }else
    {
      g->sinistra->d.readed=1;
      g->sinistra->d.cell_type=get_cell_type2(cx);
    }
    
  /*
    2)Go to the rst unvisitata cell in the neighbor list of
    the current cell (i.e., go to the rst clockwise
    unvisitata cell). Insert this cell into the beginn2ing of
    the path list and mark it as visitata.
   */
 
 //chekcs L
  //if(g->sinistra->d.cell_type==WALL)g->sinistra->d.visitata=1;
  if(g->sinistra->d.visitata==0&&g->sinistra->d.cell_type!=WALL)
    {
      g=g->sinistra;
      g->d.visitata=1;
      add_head2(&p,g);
      l=p;
      return SINISTRA;
    }

  //chekcs U
  //if(g->alto->d.cell_type==WALL)g->alto->d.visitata=1;
  if(g->alto->d.visitata==0&&g->alto->d.cell_type!=WALL)
    {
      g=g->alto;
      g->d.visitata=1;
      add_head2(&p,g);
      l=p;
      return ALTO;
    }
      


 //chekcs R
  //if(g->destra->d.cell_type==WALL)g->destra->d.visitata=1;
  if(g->destra->d.visitata==0&&g->destra->d.cell_type!=WALL)
    {
      g=g->destra;
      g->d.visitata=1;
      add_head2(&p,g);
      l=p;
      return DESTRA;
    }
  //chekcs D
  //if(g->basso->d.cell_type==WALL)g->basso->d.visitata=1;
  if(g->basso->d.visitata==0&&g->basso->d.cell_type!=WALL)
    {
      g=g->basso;
      g->d.visitata=1;
      add_head2(&p,g);
      l=p;
      return BASSO;
    }
 
  

  /*
    3) At this point, back track until a cell with unvis-
    ited neighbors is encountered. This back tracking is
    achieved by walking forward through the path list,
    inserting each element that is visitata to the front
    of the path list, until an element with an unvisied
    neighbor is encountered. Insert this element to the
    front of the path list and repeat the above procedure
    (i.e., goto step 2).
   */
 
  /*back to previous cell*/
  l=l->prev;
if(l==NULL) exit(-1);
  /*Add it to the path list*/
  add_head2(&p,l->pcell);
  /*get the diretion*/
  direzione nd;//next step direzione
  if(g->destra==l->pcell) nd=DESTRA;
  else
  if(g->sinistra==l->pcell) nd=SINISTRA;
  else
  if(g->alto==l->pcell) nd=ALTO;
  else
  if(g->basso==l->pcell) nd=BASSO;

  /*Set as the online cell*/
  //g=l->pcell;
  /*move to the previous one*/
  
  return nd;
  
}

Cell* get_neighbor2(path ap,direzione dir){
  int R=0,U=0;
  int found=0;
  
  while(ap->prev){
    if(ap->pcell->destra==ap->prev->pcell) R++;
    if(ap->pcell->sinistra==ap->prev->pcell) R--;
    if(ap->pcell->alto==ap->prev->pcell) U++;
    if(ap->pcell->basso==ap->prev->pcell) U--;
    /*check neighbor condition*/
    //DESTRA
    if(dir==DESTRA&&R==1&&U==0)
      {
	return ap->prev->pcell;
      }
    //R-U
    if(dir==DESTRA&&(R==1)&&U==+1)
      {
	return ap->prev->pcell->basso;
      }
    //R-D
    if(dir==DESTRA&&(R==1)&&U==-1)
      {
	return ap->prev->pcell->alto;
      }


    if(dir==DESTRA&&R==2&&U==0)
      {
	//return ap->prev->pcell->sinistra;
      }



    
    //L
    if(dir==SINISTRA&&R==-1&&(U==0))
      {
	return ap->prev->pcell;
      }
    //L-U
    if(dir==SINISTRA&&(R==-1)&&U==+1)
      {
	return ap->prev->pcell->basso;
      }
    //L-D
    if(dir==SINISTRA&&(R==-1)&&U==-1)
      {
	return ap->prev->pcell->alto;
      }


    if(dir==SINISTRA&&R==-2&&U==0)
      {
	//return ap->prev->pcell->destra;
      }
    //ALTO
    if(dir==ALTO&&R==0&&U==1)
      {
	return ap->prev->pcell;
      }
    //U-L
    if(dir==ALTO&&(R==-1)&&U==+1)
      {
	return ap->prev->pcell->destra;
      }
    //U-R
    if(dir==ALTO&&(R==1)&&U==+1)
      {
	return ap->prev->pcell->sinistra;
      }

    
    if(dir==ALTO&&R==0&&U==2)
      {
	//return ap->prev->pcell->basso;
      }
    // D
    if(dir==BASSO&&R==0&&U==-1)
      {
	return ap->prev->pcell;
      }
    //D-L
    if(dir==BASSO&&(R==-1)&&U==-1)
      {
	return ap->prev->pcell->destra;
      }
    //D-R
    if(dir==BASSO&&(R==1)&&U==-1)
      {
	return ap->prev->pcell->sinistra;
      }

    if(dir==BASSO&&R==0&&U==-2)
      {

	//return ap->prev->pcell->alto;
      }
    ap=ap->prev;
  }
  return NULL;
}



char_type get_cell_type2(elemento_campo code){
  if(code==TLC||code==TRC||code==BRC||code==BLC||code==HLN||code==VLN||code==TT||code==TB){
    return WALL;
  }
  
  if(code==0){
    return EMPTY;
  }
  
  if(code==FUTF){
    return FLOWER;
  }
  if(code==GUTF){
    return Tuki;
  }
  if(code==TUTF){
    return TUKI;
  }
  
  if(code==10210||code==9830){
    return ITEM;
  }
  return UNKNOWN;
}
direzione symmetry2(direzione d){
  direzione sd=IN;
  switch(d){
  case SINISTRA:
    sd=DESTRA;
    break;
  case DESTRA:
    sd=SINISTRA;
    break;
  case ALTO:
    sd=BASSO;
    break;
   case BASSO:
    sd=ALTO;
    break;
  }
  return sd;
  
}

void new_graph2(graph* g)
{
  *g = NULL;
}


/**
 *Add a cell to the graph after checking that a cell
 *was already present at the same poistion.
 *For each cell of the actual Tuki path is saltoposed
 *to exist another 4 cells at the ajacent direzione
 *This functions.
 *The existence of a possible unlinked cell in the 'd' 
 *direzione is tested scann2ing the path_list untill a
 *cell 
 */
graph add_to_graph2(graph pcell,Data d,direzione dir){
  nn2++;
  Cell* n=(Cell*)malloc(sizeof(Cell));
  n->d=d;
  n->d.cont=nn2;
 
  switch(dir)
    {
    case ALTO:
      pcell->alto=n;
      n->basso=pcell;
      break;
    case BASSO:
      pcell->basso=n;
      n->alto=pcell;
      break;
    case DESTRA:
      pcell->destra=n;
      n->sinistra=pcell;
      break;
    case SINISTRA:
      pcell->sinistra=n;
      n->destra=pcell;
      break;
    }
  return n;
}

void new_path2(path* p){
  *p=0;
}
void add_head2(path* l, graph g) {
  Node* aux = (Node*)malloc(sizeof(Node));
  
  aux->prev = *l;
  aux->next = NULL;  
  aux->pcell = g;
  if(*l)(*l)->next=aux;
  *l = aux;
}
int decode_char2(unsigned char* r){ 
  unsigned int mB1,mB2,mB3,m6bits,m4bits;
  /*creates the masks*/
  m4bits=15;
  m6bits=63;
  int by4=0;
  int by1=r[0]&m4bits;
  int by2=r[1]&m6bits;
  int by3=r[2]&m6bits;
  int code=by4+by3+(by2<<6)+(by1<<12);
  return code;
}




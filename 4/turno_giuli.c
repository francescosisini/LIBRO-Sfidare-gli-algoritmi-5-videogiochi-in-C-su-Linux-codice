   
/*______________________________
 *|
 *| turno_giuli.c
 */
   
#include "giocatore.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>



pacco turno_giuli(lancio l)
{
  static double m=0;
  double x,y;
  
  /* m è il coefficiente angolare della retta di divisione */
  
  double c=0;
  double a;
  if(m  == 0)
    {
      srand(time(0));
      while(c  == 0)
        {
          a=(double)rand()/(double)RAND_MAX*6.28;
          c=cos(a);
        }
      m=sin(a)/c;
    }
 
  x = l.x;
  y = l.y;
  
  /* verifichiamo se x e y sono sopra o sotto la retta */
  double y_r = m*x;

  if(y_r>y)
    return BOMBA;
  else
    return MELA;
}


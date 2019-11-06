   
/*_________________________________________
 *|
 *| turno_giuli.c
 */    

#include "giocatore.h"
#include <stdio.h>
#include <stdlib.h>

#define MYX 20

direzione turno_giuli()
{
  static int x = MYX;
  static direzione dir = SINISTRA;
  static int inc = -1;
  static int marcati = 0;
  static int p1 = 0,p2 = 0;
  static int inside = 0;
  
  int m = marca();
  
  fflush(stdout);
  
  if(m>=0) inside = 1;
  
  if(m<0 && dir==SINISTRA && !inside){
    return dir;  
  }
  
   if(m<0 && dir==SINISTRA && inside){
     dir = DESTRA;
     return dir;
   }
   
   if(m<0 && dir==DESTRA && inside){
     dir = SINISTRA;
     return dir;
   }
   
   if(m>=0){
    marcati++;
    if(marcati==1)
      {
	p1 = altezza_palo();
	p2 = 0;
      }
    if(marcati==2){
      p2 = altezza_palo();
    }
    
    if(marcati==2)
      {
        if(dir==DESTRA){
          if(p2>p1)
            {
              marcati = 0;
              scambia_pali();
              p2 = p1 = 0;
            }
          else
            {
              marcati = 1;
              p1 = p2;
              p2 = 0;
            }
        }
        else
          {
            if(p1>p2)
            {
              marcati = 0;
              scambia_pali();
              p2 = p1 = 0;
            }
            else
            {
              marcati=1;
              p1 = p2;
              p2 = 0;
            }
          }
      }
   }
   x += inc;
   return dir;
}  


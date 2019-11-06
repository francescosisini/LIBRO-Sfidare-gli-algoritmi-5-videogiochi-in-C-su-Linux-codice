   
/*______________________________
*|  
*| gioca_tuki.c
*/
    
#include "tuki5_modello.h"
#include <stdio.h>
#include <unistd.h>

direzione gioca_tuki(posizioni p, oggetto **labx){
  //  static int turno = 0;
  //static direzione ld=SINISTRA;
  
  //return ld;
 
  //turno+=1;

  static direzione dir = FERMO;
  char c;

  int i=p.tuki_y;
  int j=p.tuki_x;

  
  read(STDIN_FILENO, &c, 1);
  
  switch(c)
      {
      case 'j':
        // if(labx[i][j-1]==U||labx[i][j-1]==V||labx[i][j-1]==J)
          dir=SINISTRA;
        break;
        
      case 'l':
        //if(labx[i][j+1]==U||labx[i][j+1]==V||labx[i][j+1]==J)
          dir=DESTRA;
        break;
        
      case 'i':
        //if(labx[i-1][j]==U||labx[i-1][j]==V||labx[i-1][j]==U||J)
          dir = SU;
        break;
        
      case 'm':
        //if(labx[i+1][j]==U||labx[i+1][j]==V||labx[i+1][j]==U||J)
          dir = GIU;
        break;
      default:
        dir = FERMO;
      }
  
  
    return dir;
}

  
   

/*_________________________________________
 *|
 *| turno_interattivo.c
 */
   
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "giocatore.h"


direzione turno_interattivo()
{

  direzione dir = IN;
 
  char c;
  
  read(STDIN_FILENO, &c, 1);
  switch(c)
    {
    case 'j':
      dir=SINISTRA;
       return dir;
      break;
    case 'l':
      dir=DESTRA;
       return dir;
      break;
    case 'i':
      marca();
      return dir;
      break;
    case 32:
      scambia_pali();
      return dir;
      break;
    }
 
  
  return dir;


}
  

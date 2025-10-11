#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

#define TTC_CMDOPT_MAXLEN 32
int main ( int argc, char **ppargv ) {
  int r = -1;
  
  BOOL dump_par = FALSE;
  BOOL dump_ttc = FALSE;
  if( argc > 1 ) {
    /* prints the dump of parsing state: ttcreat -dpar
     * prints the dump of ttc (TimeTable Creation): state ttcreat -dttc
     * prints both dumps of above: ttcreat -dpar -dttc
     */
    assert( ppargv );
    int n = 1;
    do {
      char *popt = ppargv[n];
      assert( popt );
      if( strncmp( popt, "-dpar", TTC_CMDOPT_MAXLEN ) == 0 ) {	
	if( !dump_par )
	  dump_par = TRUE;
	else
	cmdopt_nonsens:
	  printf( "NOTICE: redundant %d th command-line option: %s.\n", n, popt );
      } else if( strncmp( popt, "-dttc", TTC_CMDOPT_MAXLEN ) == 0 ) {
	if( !dump_ttc )	  
	   dump_ttc = TRUE;
	else
	  goto cmdopt_nonsens;
      } else
	printf( "NOTICE: invalid %d th command-line option: %s.\n", n, popt );
      n++;
    } while( n < argc );
  }
  r = ttcreat( NULL, dump_par, dump_ttc );
  return r;
}

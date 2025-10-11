#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

int main ( int argc, char **ppargv ) {
  int r = -1;
  FILE *fp_src = NULL;
  
  char *psrc_fname = NULL;
  BOOL dump_par = FALSE;
  BOOL dump_ttc = FALSE;
  if( argc > 1 ) {
    /* specifies the source-file of timetable to be read from: ttcreat filename.ttb
     * prints out dump of parsing state: ttcreat -vpar
     * prints out dump of ttc (TimeTable Creation): state ttcreat -vttc
     * prints both dumps of above: ttcreat -vpar -vttc
     */
    assert( ppargv[1] );
    int n = 1;
    do {
      assert( ppargv[n] );
      char *popt = ppargv[n];
      if( strncmp( popt, "-vpar", TTC_CMDOPT_MAXLEN ) == 0 ) {
	if( !dump_par )
	  dump_par = TRUE;
	else
	cmdopt_nonsens:
	  printf( "NOTICE: redundant %d th command-line option, omitted.: %s.\n", n, popt );
      } else if( strncmp( popt, "-vttc", TTC_CMDOPT_MAXLEN ) == 0 ) {
	if( !dump_ttc )	  
	   dump_ttc = TRUE;
	else
	  goto cmdopt_nonsens;
      } else {
	char *pext = NULL;
	pext = strrchr( popt, '.' );
	if( pext ) {
	  if( strncmp( pext, TTC_SOURCEFILE_EXT, strlen(TTC_SOURCEFILE_EXT) ) == 0 )
	    psrc_fname = popt;
	} else
	  printf( "FATAL: invalid %d th command-line option: %s.\n", n, popt );
      }	  
      n++;
    } while( n < argc );
  }
  
  if( psrc_fname ) {
    fp_src = fopen( psrc_fname, "r" );
    if( ferror( fp_src ) ) {
      printf( "FATAL: cannot open source file; %s.\n", psrc_fname );
      return r;
    }
  }
  r = ttcreat( fp_src, dump_par, dump_ttc );
  return r;
}

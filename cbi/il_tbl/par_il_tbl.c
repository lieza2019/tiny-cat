#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../../generic.h"

#define break_on_eoferr( fp, r ) {				\
  if( feof( (fp) ) ) { (r) *= -1; break; }			\
  if( ferror( (fp) ) ) { (r) *= -1; break; }			\
  }

int par_csv_iltbl ( char *bufs[], const int nbufs, FILE *fp_src ) {
  assert( bufs );
  assert( nbufs >= 0 );
  assert( fp_src );
  assert( !feof( fp_src ) );
  assert( !ferror( fp_src ) );
  
  int i = -1;
  int c = ',';
  while( c != '\n' ) {
    if( c != ',' ) {
      char *s = bufs[i];
      s[0] = c;
      s[1] = 0;
      fscanf( fp_src, "%[^,]", &s[1] );
      break_on_eoferr( fp_src, i );
      c = fgetc( fp_src );
      break_on_eoferr( fp_src, i );
      assert( c == ',' );
    }
    i++;
    if( i >= nbufs )
      break;
    strcpy( bufs[i], "" );
    c = fgetc( fp_src );
    break_on_eoferr( fp_src, i );
  }
  return i;
}

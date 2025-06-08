#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../../generic.h"

#if 0 // *****
int par_route_iltbl ( int *pseq, char *pro_name, char *pctrl_tr, FILE *fp_src ) {
  assert( pseq );
  assert( pro_name );
  assert( pctrl_tr );
  assert( fp_src );
  int r = 0;
  
  char dc[256 + 1]; // dont cure.
  int n = -1;
  dc[256] = 0;
  *pseq = -1;
  n = fscanf( fp_src, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", pseq, dc, dc, dc, pro_name, dc, dc, dc, dc, pctrl_tr );
  if( n > 0 ) {
    assert( *pseq > -1 );
    int m = 0;
    if( n < 10 ) {
      assert( n == 1 );
      assert( !strcmp( pro_name, "" ) );
      m = fscanf( fp_src, ",,,,,,,,%[^,]", pctrl_tr );
    } else {
      assert( n == 10 );
      assert( strcmp( pro_name, "" ) );
      assert( strcmp( pctrl_tr,  "" ) );
    }
    r = n + m;
  }
  return r;
}
#else
#define break_on_eoferr( fp, r ) {				\
  if( feof( (fp) ) ) { (r) *= -1; break; }			\
  if( ferror( (fp) ) ) { (r) *= -1; break; }			\
  }
int par_route_iltbl ( char *bufs[], const int nbufs, FILE *fp_src ) {
  assert( !feof( fp_src ) );
  assert( !ferror( fp_src ) );
  int i = -1;
  int c = ',';
  while( c != '\n' ) {
    if( c != ',' ) {
      char *s = bufs[i];
      s[0] = c;
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
#endif

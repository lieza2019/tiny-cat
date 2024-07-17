#include "generic.h"
#include "misc.h"
#include "ars.h"
#include "interlock.h"

int foo( void ) {
  SCHEDULED_COMMAND C;
  C.cmd = ARS_SCHEDULED_ROUTESET;
  return 0;
}


BOOL chk_routeconf ( ROUTE_PTR r1, ROUTE_PTR r2) {
  assert( r1 );
  assert( r2 );
  BOOL r = FALSE;
    
  int i;
  for( i = 0; i < r1->trks.num_tracks; i++ ) {
    int j;
    for( j = 0; j < r2->trks.num_tracks; j++ )
      if( r1->trks.tracks[i] == r2->trks.tracks[j] ) {
	r = TRUE;
	goto found;
      }
  }
 found:
  return r;
}

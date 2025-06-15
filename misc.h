#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "generic.h"

#define GEN_INDENT( fp, n, m ) {int i; for(i = 0; i < (n); i++){ int b; for(b = 0; b < (m); b++ ) fprintf((fp), " "); }}

#define ERRORF_OUTSTREAM stdout
#define errorF( fmt, ... ) (				\
  {							\
    assert( fmt );					\
    fprintf( ERRORF_OUTSTREAM, (fmt), __VA_ARGS__ );	\
  }							\
)

extern int nbits_sft ( int m );
  
// below are use for debugging.
extern void phony_raw_recvbuf_traininfo( void *pbuf );  // ***** for debugging.
extern void dump_raw_recvbuf_traininfo( void *pbuf );  // ***** for debugging.

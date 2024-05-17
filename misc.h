#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "generic.h"

#define ERRORF_OUTSTREAM stdout
#define errorF( fmt, ... ) (				\
  {							\
    assert( fmt );					\
    fprintf( ERRORF_OUTSTREAM, (fmt), __VA_ARGS__ );	\
  }							\
)

// below are use for debugging.
extern void phony_raw_recvbuf_traininfo( void *pbuf );  // ***** for debugging.
extern void dump_raw_recvbuf_traininfo( void *pbuf );  // ***** for debugging.

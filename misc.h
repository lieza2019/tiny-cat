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
  


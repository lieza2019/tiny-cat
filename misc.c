#include "generic.h"
#include "misc.h"

#define ERRORF_OUTSTREAM stdout
void errorF (const char *errmsg ) {
  assert( errmsg );
  fprintf( ERRORF_OUTSTREAM, "%s", errmsg );
}


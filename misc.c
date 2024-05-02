#include "generic.h"
#include "misc.h"

void errorF (const char *errmsg ) {
  assert( errmsg );
  printf( "%s", errmsg );
}


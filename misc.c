#include "generic.h"
#include "misc.h"

int nbits_sft ( int m ) {
  assert( m > 0 );
  int n = 0;
  while( m > 1 ) {
    assert( (m % 2) == 0 );
    m /= 2;
    n++;
  }
  return n;
}

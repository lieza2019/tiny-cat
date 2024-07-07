#include "generic.h"
#include "misc.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

typedef struct block {
  BLOCK_ID id;
  char *name;
  int len;
  BOOL occupancy;
} BLOCK, *BLOCK_PTR;
#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_def.h"
#undef BLOCK_ATTRIB_DEFINITION

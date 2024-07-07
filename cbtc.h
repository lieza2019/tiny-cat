#include "generic.h"
#include "misc.h"
#include "sparcs.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

typedef struct block {
  CBTC_BLOCK_ID id;
  char *name;
  int len;
  //BOOL occupancy;
  TINY_TRAIN_STATE_PTR occupancy;
} CBTC_BLOCK, *CBTC_BLOCK_PTR;
#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_def.h"
#undef BLOCK_ATTRIB_DEFINITION

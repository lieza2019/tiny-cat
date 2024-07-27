#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "ars.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

typedef struct block {
  CBTC_BLOCK_ID block_id;
  char *name;
  int len;
  struct {
    CBI_STAT_KIND kind;
    IL_OBJ_INSTANCES track;
  } belonging_tr;
  struct {
    BOOL has_sp;
    SP_ID sp_id;
  } sp;
  TINY_TRAIN_STATE_PTR occupancy; // the link to the train which FRONT_BLOCK-ID bites this block.
} CBTC_BLOCK, *CBTC_BLOCK_PTR;
#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_def.h"
#undef BLOCK_ATTRIB_DEFINITION

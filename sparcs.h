#include "generic.h"
#include "misc.h"
#include "sparcs_def.h"
#include "train_cmd.h"
#include "train_info.h"
#include "train_ctrl.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

#define NUM_OF_SCs 4
typedef enum SC_ID {
  SC801,
  SC802,
  SC803,
  SC804,
  SC805,
  SC806,
  SC807,
  END_OF_SCs
} SC_ID;

typedef struct toward_SC_cmdset {
  TRAIN_COMMAND train_cmd;
} TOWARD_SC_CMDSET, *TOWARD_SC_CMDSET_PTR;

typedef struct from_SC_dataset {
  TRAIN_INFO train_info;
} FROM_SC_DATASET, *FROM_SC_DATASET_PTR;

typedef struct block {
  BLOCK_ID id;
  char *name;
  int len;
  BOOL occupancy;
} BLOCK, *BLOCK_PTR;

extern int alloc_train_cmd_entries( TRAIN_COMMAND_ENTRY_PTR es[], int rakeID, int front_blk, int back_blk );
extern FROM_SC_DATASET_PTR sniff_train_info( SC_ID sc_id );

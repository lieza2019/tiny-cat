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
  SC808,
  SC809,
  END_OF_SCs
} SC_ID;

typedef struct SC_ctrl_cmdset {
  TRAIN_COMMAND train_cmd;
} SC_CTRL_CMDSET, *SC_CTRL_CMDSET_PTR;

typedef struct SC_stat_infoset {
  TRAIN_INFO train_info;
} SC_STAT_INFOSET, *SC_STAT_INFOSET_PTR;

typedef struct block {
  BLOCK_ID id;
  char *name;
  int len;
  BOOL occupancy;
} BLOCK, *BLOCK_PTR;

extern int alloc_train_cmd_entries( TRAIN_COMMAND_ENTRY_PTR es[], int rakeID, int front_blk, int back_blk );
extern SC_STAT_INFOSET_PTR sniff_train_info( SC_ID sc_id );

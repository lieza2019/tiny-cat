#include "generic.h"
#include "misc.h"

#define TRAINID_MAX_LEN 4
typedef struct tiny_train_state {
  int rakeID;
  char trainID[TRAINID_MAX_LEN + 1];
  int dest_blockID;
  BOOL skip_next_stop;
  BOOL ATO_dept_cmd;
  BOOL TH_cmd;
  TRAIN_PERF_REGIME perf_regime_cmd;
  BOOL turnback_siding;
  int dwell_time;
  BOOL train_remove;
  BOOL releasing_emergency_stop;
  BOOL ordering_emergency_stop;
  BOOL ATB_cmd;
  struct {
    unsigned char cmd_id;
    unsigned char cmd_args[OCC_REMOTE_CMD_ARGS_SIZE];
  } occ_remote_cmd;
  TRAIN_COMMAND_ENTRY_PTR pTC[2];
   
  TRAIN_INFO_ENTRY_PTR  pTI;
  BOOL expired;
  struct tiny_train_state *pNext;
} TINY_TRAIN_STATE, *TINY_TRAIN_STATE_PTR;

extern void reveal_train_tracking( void );

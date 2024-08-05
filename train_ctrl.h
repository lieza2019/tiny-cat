#include "generic.h"
#include "misc.h"
#include "cbtc.h"

#define TRAINID_MAX_LEN 4
typedef struct tiny_train_state {
  int rakeID;
  char trainID[TRAINID_MAX_LEN + 1];
  int dest_blockID;
  BOOL skip_next_stop;
  BOOL ATO_dept_cmd;
  BOOL TH_cmd;
  TRAIN_PERF_REGIME perf_regime;
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
  struct {
    CBTC_BLOCK_PTR pblk_forward;
    CBTC_BLOCK_PTR pblk_back;
    struct tiny_train_state *pNext;
  } occupancy;
  TRAIN_INFO_ENTRY_PTR  pTI;
  BOOL updated;
  BOOL omit;
  struct tiny_train_state *pNext;
} TINY_TRAIN_STATE, *TINY_TRAIN_STATE_PTR;

typedef struct standby_train_cmds {
  TINY_TRAIN_STATE_PTR phd;
  TINY_TRAIN_STATE_PTR *pptl;
} STANDBY_TRAIN_CMDS, *STANDBY_TRAIN_CMDS_PTR;

#define MAX_TRAIN_TRACKINGS 1024

extern TINY_TRAIN_STATE trains_tracking[MAX_TRAIN_TRACKINGS];
extern void reveal_train_tracking( TINY_SOCK_PTR pS );
extern BOOL establish_SC_comm ( TINY_SOCK_PTR pS );
extern int load_train_command ( void );
extern void chk_solid_train_cmds ( void );

extern TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
extern TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
extern TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB );
extern TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB );

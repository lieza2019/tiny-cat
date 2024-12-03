#include "generic.h"
#include "misc.h"
#include "cbtc.h"

typedef struct train_ID {
  STOPPING_POINT_CODE dest;
  JOURNEY_ID jid;
} TRAIN_ID;
typedef struct tiny_train_state {
  int rakeID;
  TRAIN_ID train_ID;
  int dest_blockID;
  int crnt_blockID;
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
  STOPPING_POINT_CODE stop_detected;
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
  TRAIN_INFO_ENTRY_PTR pTI;
  TRAIN_INFO_ENTRY TI_last;
  BOOL updated;
  BOOL omit;
  struct tiny_train_state *pNext;
} TINY_TRAIN_STATE, *TINY_TRAIN_STATE_PTR;

typedef struct standby_train_cmds {
  TINY_TRAIN_STATE_PTR phd;
  TINY_TRAIN_STATE_PTR *pptl;
} STANDBY_TRAIN_CMDS, *STANDBY_TRAIN_CMDS_PTR;

#define MAX_TRAIN_TRACKINGS 1024

extern unsigned short TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY Ce, TRAIN_ID trainID );

extern uint8_t sp2_dst_platformID ( STOPPING_POINT_CODE dest_sp );
extern uint8_t journeyID2_serviceID ( JOURNEY_ID journey_ID );

extern uint16_t change_train_state_trainID ( TINY_TRAIN_STATE_PTR pT, const TRAIN_ID train_ID, BOOL mindles );
extern int change_train_state_rakeID ( TINY_TRAIN_STATE_PTR pT, const int rakeID, BOOL mindles );
extern int change_train_state_dest_blockID ( TINY_TRAIN_STATE_PTR pT, const int dest_blockID, BOOL mindles );
extern int change_train_state_crnt_blockID ( TINY_TRAIN_STATE_PTR pT, const int crnt_blockID, BOOL mindles );
extern BOOL change_train_state_skip_next_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL skip_next_stop, BOOL mindles );
extern BOOL change_train_state_ATO_dept_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATO_dept_cmd, BOOL mindles );
extern BOOL change_train_state_TH_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL TH_cmd, BOOL mindles );
extern TRAIN_PERF_REGIME change_train_state_perf_regime ( TINY_TRAIN_STATE_PTR pT, const TRAIN_PERF_REGIME perf_regime, BOOL mindles );
extern BOOL change_train_state_turnback_siding ( TINY_TRAIN_STATE_PTR pT, const BOOL turnback_siding, BOOL mindles );
extern int change_train_state_dwell_time ( TINY_TRAIN_STATE_PTR pT, const int dwell_time, BOOL mindles );
extern BOOL change_train_state_train_remove ( TINY_TRAIN_STATE_PTR pT, const BOOL train_remove, BOOL mindles );
extern BOOL change_train_state_releasing_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL releasing_emergency_stop, BOOL mindles );
extern BOOL change_train_state_ordering_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_emergency_stop, BOOL mindles );
extern BOOL change_train_state_ATB_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATB_cmd, BOOL mindles );

extern TINY_TRAIN_STATE trains_tracking[MAX_TRAIN_TRACKINGS];
extern int establish_SC_comm_infos ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pdescs[], const int ninfos, const int ndescs );
extern int load_train_command ( void );
extern void chk_solid_train_cmds ( void );
extern pthread_mutex_t cbtc_ctrl_cmds_mutex;
extern pthread_mutex_t cbtc_stat_infos_mutex;
extern void *conslt_cbtc_state ( TINY_TRAIN_STATE_PTR pT, const CBTC_CMDS_INFOS kind, void *pstat_prev, void *pstate, const int size );
extern void *pth_emit_cbtc_ctrl_cmds ( void *arg );
extern void *pth_reveal_cbtc_status ( void *arg );

extern TINY_TRAIN_STATE_PTR read_edge_of_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB );
extern TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
extern TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB );
extern TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
extern TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB );

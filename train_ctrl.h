#include "generic.h"
#include "misc.h"
#include "cbtc.h"

typedef struct train_ID {
  STOPPING_POINT_CODE dest;
  JOURNEY_ID jid;
  unsigned short coden;
} TRAIN_ID, *TRAIN_ID_PTR;
typedef const struct train_ID *TRAIN_ID_C_PTR;

typedef struct tiny_train_state {
  int rakeID;
  TRAIN_ID train_ID;
  int dest_blockID;
  int crnt_blockID;
  BOOL keep_door_closed;
  BOOL out_of_service;
  CRNT_ST_PLCODE crnt_station_plcode;
  NEXT_ST_PLCODE next_station_plcode;
  DST_ST_PLCODE dst_station_plcode;
  int destination_number;
  int next_station_number;
  int crnt_station_number;  
  BOOL ATO_dept_cmd;
  BOOL depcond_release;
  BOOL skip_next_stop;
  BOOL origin_station;
  NEXT_ST_DOOROPEN_SIDE next_st_dooropen_side;
  TRAIN_OPERATION_MODE operaton_mode;
  BOOL leave_now;
  TRAIN_PERF_REGIME perf_regime;
  BOOL coasting_cmd;
  BOOL TH_cmd;
  int maximum_speed_cmd;
  BOOL turnback_siding;
  int passenger_address;
  TRAIN_MOVE_DIR dep_dir;
  int regulation_speed;
  int dwell_time;
  BOOL ordering_wakeup;
  BOOL ordering_standby;
  BOOL ATB_cmd;
  BOOL ordering_emergency_stop;
  BOOL releasing_emergency_stop;
  BOOL train_remove;
  SYSTEM_SWITCHING_CMD system_switch_cmd;
  BOOL ordering_reset_onboard;
  BOOL energy_saving;
  REMOTE_DOOR_OPENING remote_door_opening;
  REMOTE_DOOR_CLOSING remote_door_closing;
  BOOL static_test_cmd;
  BOOL dynamic_test_cmd;
  BOOL inching_cmd;
  BOOL back_inching_cmd;
  EMERGE_DOOR_RELEASING em_door_release;
  BOOL back_vrs_reset;
  BOOL forward_vrs_reset;
  int passenger_display_message;
  
  STOPPING_POINT_CODE stop_detected;
  struct {
    unsigned char cmd_id;
    unsigned char cmd_args[OCC_REMOTE_CMD_ARGS_SIZE];
  } occ_remote_cmd;
  TRAIN_COMMAND_ENTRY_PTR pTC[2];
  TRAIN_INFO_ENTRY_PTR pTI;
  TRAIN_INFO_ENTRY TI_last;
  struct {
    unsigned short prev_blk_forward;
    struct {
      CBTC_BLOCK_PTR pblk_forward;
      CBTC_BLOCK_PTR pblk_back;
      struct tiny_train_state *pNext;
    } occupancy;
  } misc;
  BOOL updated;
  BOOL omit;
  struct tiny_train_state *pNext;
} TINY_TRAIN_STATE, *TINY_TRAIN_STATE_PTR;
typedef struct tiny_train_state const *TINY_TRAIN_STATE_C_PTR;

typedef struct standby_train_cmds {
  TINY_TRAIN_STATE_PTR phd;
  TINY_TRAIN_STATE_PTR *pptl;
} STANDBY_TRAIN_CMDS, *STANDBY_TRAIN_CMDS_PTR;

#define MAX_TRAIN_TRACKINGS 1024

extern unsigned short TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY_PTR pCe, TRAIN_ID_C_PTR ptrainID );

extern uint8_t sp2_dst_platformID ( STOPPING_POINT_CODE dest_sp );
extern uint8_t journeyID2_serviceID ( JOURNEY_ID journey_ID );

extern TRAIN_ID_PTR change_train_state_trainID ( TINY_TRAIN_STATE_PTR pT, const TRAIN_ID train_ID, BOOL mindles );
extern int change_train_state_rakeID ( TINY_TRAIN_STATE_PTR pT, const int rakeID, BOOL mindles );
extern int change_train_state_dest_blockID ( TINY_TRAIN_STATE_PTR pT, const int dest_blockID, BOOL mindles );
extern int change_train_state_crnt_blockID ( TINY_TRAIN_STATE_PTR pT, const int crnt_blockID, BOOL mindles );
extern BOOL change_train_state_keep_door_closed ( TINY_TRAIN_STATE_PTR pT, const BOOL keep_door_closed, BOOL mindles );
extern BOOL change_train_state_out_of_service ( TINY_TRAIN_STATE_PTR pT, const BOOL out_of_service, BOOL mindles );
extern CRNT_ST_PLCODE change_train_state_crnt_station_plcode ( TINY_TRAIN_STATE_PTR pT, const CRNT_ST_PLCODE plcode, BOOL mindles );
extern NEXT_ST_PLCODE change_train_state_next_station_plcode ( TINY_TRAIN_STATE_PTR pT, const NEXT_ST_PLCODE plcode, BOOL mindles );
extern DST_ST_PLCODE change_train_state_dst_station_plcode ( TINY_TRAIN_STATE_PTR pT, const DST_ST_PLCODE plcode, BOOL mindles );
extern int change_train_state_destination_number ( TINY_TRAIN_STATE_PTR pT, const int destination_number, BOOL mindles );
extern int change_train_state_next_station_number ( TINY_TRAIN_STATE_PTR pT, const int next_station_number, BOOL mindles );
extern int change_train_state_crnt_station_number ( TINY_TRAIN_STATE_PTR pT, const int crnt_station_number, BOOL mindles );
extern BOOL change_train_state_ATO_dept_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATO_dept_cmd, BOOL mindles );
extern BOOL change_train_state_depcond_release ( TINY_TRAIN_STATE_PTR pT, const BOOL depcond_release, BOOL mindles );
extern BOOL change_train_state_skip_next_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL skip_next_stop, BOOL mindles );
extern BOOL change_train_state_origin_station ( TINY_TRAIN_STATE_PTR pT, const BOOL origin_station, BOOL mindles );
extern NEXT_ST_DOOROPEN_SIDE change_train_state_next_st_dooropen_side ( TINY_TRAIN_STATE_PTR pT, const NEXT_ST_DOOROPEN_SIDE next_st_dooropen_side, BOOL mindles );
extern TRAIN_OPERATION_MODE change_train_state_operation_mode ( TINY_TRAIN_STATE_PTR pT, const TRAIN_OPERATION_MODE operation_mode, BOOL mindles );
extern BOOL change_train_state_leave_now ( TINY_TRAIN_STATE_PTR pT, const BOOL leave_now, BOOL mindles );
extern TRAIN_PERF_REGIME change_train_state_perf_regime ( TINY_TRAIN_STATE_PTR pT, const TRAIN_PERF_REGIME perf_regime, BOOL mindles );
extern BOOL change_train_state_coasting_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL coasting_cmd, BOOL mindles );
extern BOOL change_train_state_TH_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL TH_cmd, BOOL mindles );
extern int change_train_state_maximum_speed_cmd ( TINY_TRAIN_STATE_PTR pT, const int maximum_speed_cmd, BOOL mindles );
extern BOOL change_train_state_turnback_siding ( TINY_TRAIN_STATE_PTR pT, const BOOL turnback_siding, BOOL mindles );
extern int change_train_state_passenger_address ( TINY_TRAIN_STATE_PTR pT, const int passenger_address, BOOL mindles );
extern TRAIN_MOVE_DIR change_train_state_dep_dir ( TINY_TRAIN_STATE_PTR pT, const TRAIN_MOVE_DIR dep_dir, BOOL mindles );
extern int change_train_state_regulation_speed ( TINY_TRAIN_STATE_PTR pT, const int regulation_speed, BOOL mindles );
extern int change_train_state_dwell_time ( TINY_TRAIN_STATE_PTR pT, const int dwell_time, BOOL mindles );
extern BOOL change_train_state_ordering_wakeup ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_wakeup, BOOL mindles );
extern BOOL change_train_state_ordering_standby ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_standby, BOOL mindles );
extern BOOL change_train_state_ATB_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATB_cmd, BOOL mindles );
extern BOOL change_train_state_ordering_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_emergency_stop, BOOL mindles );
extern BOOL change_train_state_releasing_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL releasing_emergency_stop, BOOL mindles );
extern BOOL change_train_state_train_remove ( TINY_TRAIN_STATE_PTR pT, const BOOL train_remove, BOOL mindles );
extern SYSTEM_SWITCHING_CMD change_train_state_system_switch_cmd ( TINY_TRAIN_STATE_PTR pT, const SYSTEM_SWITCHING_CMD system_switch_cmd, BOOL mindles );
extern BOOL change_train_state_ordering_reset_onboard ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_reset_onboard, BOOL mindles );
extern BOOL change_train_state_energy_saving ( TINY_TRAIN_STATE_PTR pT, const BOOL energy_saving, BOOL mindles );
extern REMOTE_DOOR_OPENING change_train_state_remote_door_opening ( TINY_TRAIN_STATE_PTR pT, const REMOTE_DOOR_OPENING remote_door_opening, BOOL mindles );
extern REMOTE_DOOR_CLOSING change_train_state_remote_door_closing ( TINY_TRAIN_STATE_PTR pT, const REMOTE_DOOR_CLOSING remote_door_closing, BOOL mindles );
extern BOOL change_train_state_static_test_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL static_test_cmd, BOOL mindles );
extern BOOL change_train_state_dynamic_test_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL dynamic_test_cmd, BOOL mindles );
extern BOOL change_train_state_inching_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL inching_cmd, BOOL mindles );
extern BOOL change_train_state_back_inching_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL back_inching_cmd, BOOL mindles );
extern EMERGE_DOOR_RELEASING change_train_state_em_door_release ( TINY_TRAIN_STATE_PTR pT, const EMERGE_DOOR_RELEASING em_door_release, BOOL mindles );
extern BOOL change_train_state_back_vrs_reset ( TINY_TRAIN_STATE_PTR pT, const BOOL back_vrs_reset, BOOL mindles );
extern BOOL change_train_state_forward_vrs_reset ( TINY_TRAIN_STATE_PTR pT, const BOOL forward_vrs_reset, BOOL mindles );
extern int change_train_state_passenger_display_message ( TINY_TRAIN_STATE_PTR pT, const int passenger_display_message, BOOL mindles );

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

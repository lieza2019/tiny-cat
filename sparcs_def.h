#if 1
typedef enum cbtc_cmds_infos {
  CBTC_TSR_COMMAND,
  CBTC_NOENTRY_PRESET_CONTROL,
  CBTC_MASTER_CLOCK,
  CBTC_TRAIN_COMMAND,
  CBTC_STATION_COMMAND,
  CBTC_SECTION_COMMAND,
  CBTC_TRAIN_INFORMATION,
  CBTC_TRACK_DATA,
  CBTC_CBI_INFORMATION,
  CBTC_TSR_STATUS,
  CBTC_NOENTRY_PRESET_STATUS,
  CBTC_STATUS_INFORMATION,
  CBTC_STATION_STATUS,  
  CBTC_UNLOCATED_TRAIN_INFORMATION,
  END_OF_CBTC_CMDS_INFOS
} CBTC_CMDS_INFOS;

typedef enum dst_st_plcode {
  DPL_INVALID_TB_SECTION,
  DPL_DNLINE,
  DPL_UPLINE,
  DPL_OTHERS
} DST_ST_PLCODE;

typedef enum next_st_plcode {
  NPL_INVALID_TB_SECTION,
  NPL_DNLINE,
  NPL_UPLINE,
  NPL_OTHERS
} NEXT_ST_PLCODE;

typedef enum crnt_st_plcode {
  CPL_INVALID_TB_SECTION,
  CPL_DNLINE,
  CPL_UPLINE,
  CPL_OTHERS
} CRNT_ST_PLCODE;

typedef enum next_st_dooropen_side {
  NDS_NOT_OPEN,
  NDS_R_SIDE,
  NDS_L_SIDE,
  NDS_BOTH
} NEXT_ST_DOOROPEN_SIDE;

typedef enum remote_door_closing {
  RDC_NO_CLOSING,
  RDC_CLOSING_R_SIDE,
  RDC_CLOSING_L_SIDE,
  RDC_CLOSING_BOTH
} REMOTE_DOOR_CLOSING;

typedef enum remote_door_opening {
  RDC_NO_OPENING,
  RDC_OPENING_R_SIDE,
  RDC_OPENING_L_SIDE,
  RDC_OPENING_BOTH
} REMOTE_DOOR_OPENING;

typedef enum _system_switching_cmd {
  SW_NO_SWITCHING,
  SW_SYS1_MASTER,
  SW_SYS2_MASTER,
  SW_INVALID
} SYSTEM_SWITCHING_CMD;

typedef enum _emerge_door_releasing {
  EMD_NO_CMD,
  EMD_RELEASING_R_SIDE,
  EMD_RELEASING_L_SIDE,
  EMD_RELEASING_BOTH
} EMERGE_DOOR_RELEASING;
#endif

typedef enum operation_mode {
  OM_UNKNOWN_NO_DIRECTIVE,
  OM_UTO,
  OM_ATO,
  OM_ATP,
  OM_RM,
  OM_INVALID_SPARE_5,
  OM_INVALID_SPARE_6,
  OM_INVALID_SPARE_7
} TRAIN_OPERATION_MODE;

typedef enum vehicle_type {
  VT_RESCUE,
  VT_6CARS
} VEHICLE_TYPE;

typedef enum train_detection_type {
  TD_UNKNOWN,
  TD_TRAIN_DETECTION,
  TD_SECTION_PROTECTION,
  TD_COMM_DOWN
} TRAIN_DETECTION_TYPE;

typedef enum initialization_status {
  INI_NOT_INITIALIZING,
  INI_IN_INITIALIZING,
  INI_INITIALIZING_COMPLETED,
  INI_INITIALIZING_TIMEOUT
} INITIALIZATION_STATUS;

typedef enum onboard_reset_result {
  ORR_NOT_RESET,
  ORR_RESET_ONBOARD_OF_SYS1,
  ORR_RESET_ONBOARD_OF_SYS2,
  ORR_SINGLE_SYSTEM_OPERATION
} ONBOARD_RESET_RESULT;

typedef enum train_sleep_mode {
  SM_NOT_IN_SLEEP,
  SM_NOW_IN_TRANSITION,
  SM_IN_SLEEP
} TRAIN_SLEEP_MODE;

typedef enum train_move_dir {
  MD_UNKNOWN,
  MD_DOWN_DIR,
  MD_UP_DIR,
  MD_INVALID
} TRAIN_MOVE_DIR;

typedef enum onboard_ATC_master_standby {
  MS_BOTH_STANDBY,
  MS_SYS1_MASTER,
  MS_SYS2_MASTER,
  MS_BOTH_MASTER,
} ONBOARD_ATC_MASTER_STANDBY;

typedef enum ATO_driving_status {
  DS_RUNNING,
  DS_SHORTSTOP,
  DS_OVERRUN,
  DS_TASC_PATTERN_RUNNING,
  DS_PO_STOPPING,
  DS_P0_OTHER_STOPPING,
  DS_INCHING,
  DS_INVALID
} ATO_DRIVING_STATUS;

typedef enum train_doors_state {
  DR_BOTH_SIDE_NOTALL_CLOSED,
  DR_R_SIDE_ALL_CLOSED,
  DR_L_SIDE_ALL_CLOSED,
  DR_BOTH_SIDE_ALL_CLOSED
} TRAIN_DOORS_STATE;

typedef enum train_perf_regime {
  PR_NO_COMMAND,
  PR_S_MODE,
  PR_N_MODE,
  PR_F_MODE
} TRAIN_PERF_REGIME;

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
  DPL_OTHERS // e.g. 3rd platform
} DST_ST_PLCODE;

typedef enum next_st_plcode {
  NPL_INVALID_TB_SECTION,
  NPL_DNLINE,
  NPL_UPLINE,
  NPL_OTHERS // e.g. 3rd platform
} NEXT_ST_PLCODE;

typedef enum crnt_st_plcode {
  CPL_INVALID_TB_SECTION,
  CPL_DNLINE,
  CPL_UPLINE,
  CPL_OTHERS // e.g. 3rd platform
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
  OM_SPARE_5,
  OM_SPARE_6,
  OM_SPARE_7
} TRAIN_OPERATION_MODE;

typedef enum vehicle_type {
  VT_RESCUE,
  VT_6CARS,
  VT_UNKNOWN
} VEHICLE_TYPE;

typedef enum train_detection_mode {
  TD_UNKNOWN,
  TD_TRAIN_DETECTION,
  TD_SECTION_PROTECTION_ONLY,
  TD_COMM_LOST
} TRAIN_DETECTION_MODE;

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
  SM_IN_SLEEP,
  SM_UNKNOWN
} TRAIN_SLEEP_MODE;

typedef enum train_move_dir {
  MD_UNKNOWN, // no direction
  MD_DOWN_DIR, // =>
  MD_UP_DIR, // <=
  MD_INVALID // invalid
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
  DR_BOTH_SIDE_NOT_ALLCLOSED,
  DR_ONLY_R_SIDE_ALL_CLOSED,
  DR_ONLY_L_SIDE_ALL_CLOSED,
  DR_BOTH_SIDE_ALL_CLOSED
} TRAIN_DOORS_STATE;

typedef enum train_perf_regime {
  PR_NO_COMMAND, // N/A
  PR_S_MODE, // 9% coasting
  PR_N_MODE, // 5% coasting
  PR_F_MODE // maximum
} TRAIN_PERF_REGIME;

typedef enum eb_reason_vobc {
  EB_NORMALITY = 0, // Normal
  EB_EXCEEDING_SPEED_LIMIT = 1, // OverSpeed
  EB_NO_USED = 2, // ORPBaliseDetected ?
  EB_EMERGENCY_STOP_RECEPTION = 4, // EBCommandReceived
  EB_NO_CBTC_COMMAND_RECEIVED_OR_VRS_EQUIPMENT_ERR = 8, // NoCommunication
  EB_DETECTING_OF_TRAIN_INTEGRITY_LOST = 16, // RelayInputErrorFromRS
  EB_ALL_DOOR_CLOSE_IS_LOST_DURING_RUNNING = 32, // DoorOpenWhieMoving
  EB_TRACTION_EQUIPMENT_ABNORMALITY = 64, // AbnormalTractionOrBraking
  EB_OPERATION_STEPS_ABNORMALITY = 128, // AbnormalOperationProcedure
} EB_REASON_VOBC;

typedef enum eb_reason_sc {
  EB_REASON_NOTHING_0 = 0,
  EB_ROUTE_NOT_SET = 1,
  EB_TRAIN_APPROACH = 2,
  EB_CLEARANCE_NOT_KEPT = 3,
  EB_END_OF_TRACK = 4,
  EB_ENTER_NON_AUTHORIZED_ZONE = 5,
  EB_UNIDENTIFIED_TRAIN_AHEAD = 6,
  EB_REASON_NOTHING_7 = 7,
  EB_UNLOCKED_POINT = 8,
  EB_NOENTRY_ZONE = 9,
  EB_EMERGENCY_STOP_ZONE = 10,
  EB_REASON_NOTHING_11 = 11,
  EB_TG_ERROR = 12,
  EB_TG_MISMATCH = 13,
  EB_PSD_OPEN = 14,
  EB_EB_COMMAND = 15,
  EB_TRAIN_UNLOCATED = 16,
  EB_TRAIN_DIRECTION_MISMATCH = 17,
  EB_RADIO_RANGING_MISMATCH = 18, 
  EB_SHORT_OF_WINDOW_NUMBER = 19,
  EB_REMOTE_EB_COMMAND = 20,
  EB_TO_EMMODE_TRANSITION_SECTION = 21,
  EB_EMERGENCY_DOOR_OPEN = 22,
  EB_TRAIN_HEAD_ON = 23,
  EB_DEAD_SECTION = 24,
  EB_TO_EMMODE_PREPARING_SECTION = 25,
  EB_TRAININFO_PROCESS_ERROR = 99,
  EB_UNDEFINED_REASON
} EB_REASON_SC;

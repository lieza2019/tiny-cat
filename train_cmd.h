#include <stdint.h>
#include "generic.h"
#include "misc.h"

#define FLG1_DESTINATION_PLATFORM_CODE 3
#define FLG1_NEXT_STATION_PLATFORM_CODE 12
#define FLG1_CURRENT_STATION_PLATFORM_CODE 48
#define FLG1_OUT_OF_SERVICE 64
#define FLG1_KEEP_DOOR_CLOSED 128

#define FLG2_DIRECTION_OF_OPEANING_DOOR_AT_THE_NEXT_STATION 12
#define FLG2_ORGIN_STATION 16
#define FLG2_SKIP_NEXT_STOP 32
#define FLG2_DEPARTURE_CONDITIONS_RELEASE 64
#define FLG2_ATO_DEPARTURE_COMMAND 128

#define FLG3_TRAIN_HOLD_COMMAND 1
#define FLG3_COASTING_COMMAND 2
#define FLG3_TRAIN_PERFORMANCE_REGIME_COMMAND 12
#define FLG3_LEAVE_NOW 16
#define FLG3_INSTRUCTING_OPERATION_MODE 224

#define PA_THE_PASSENGER_ADDRESSING 127
#define TURNBACK_OR_SIDING 128

#define FLG4_DWELL_TIME 3  // ((FLG4_DWELL_TIME & FLG4) << 8) + dwell_time
#define FLG4_REGURATION_SPEED 60
#define FLG4_DEPARTURE_DIRECTION 192

#define FLG5_TRAIN_REMOVE 2
#define FLG5_ORDERING_RELEASE_FOR_EMERGENCY_STOP 4
#define FLG5_ORDERING_EMERGENCY_STOP 8
#define FLG5_AUTOMATIC_TURN_BACK_COMMAND 32
#define FLG5_ORDERING_STANDBY 64
#define FLG5_ORDERING_WAKEUP 128

#define FLG6_CLOSING_TRAIN_DOOR_REMOTELY 3
#define FLG6_OPENING_TRAIN_DOOR_REMOTELY 12
#define FLG6_ENERGY_SAVING_MODE 16
#define FLG6_ORDERING_RESET_FOR_ONBOARD 32
#define FLG6_SYSTEM_EXCHANGE_COMMAND 192

#define FLG7_VRS_F_RESET 1
#define FLG7_VRS_B_RESET 2
#define FLG7_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND 12
#define FLG7_BACK_INCHING_COMMAND 16
#define FLG7_INCHING_COMMAND 32
#define FLG7_DYNAMIC_TEST_COMMAND 64
#define FLG7_STATIC_TEST_COMMAND 128

#define OCC_REMOTE_CMD_ARGS_SIZE 6
typedef struct train_command_entry {
  uint16_t rakeID;
  uint16_t trainID;
  uint8_t dst_blockID_upper;
  uint8_t dst_blockID_lower;
  uint8_t crnt_blockID_upper;
  uint8_t crnt_blockID_lower;
  uint8_t flgs_1;
  uint8_t destination_number;
  uint8_t next_station_number;
  uint8_t current_station_number;
  uint8_t flgs_2;
  uint8_t flgs_3;
  uint8_t maximum_speed;
  uint8_t PA_and_turnback_or_siding;
  uint8_t flgs4_and_dwell;
  uint8_t dwell_time;
  uint8_t flgs_5;
  uint8_t flgs_6;
  uint8_t flgs_7;
  uint8_t spare_1;
  uint8_t PA_display_msg_h;
  uint8_t PA_display_msg_l;
  struct {
    uint8_t cmd_id;
    uint8_t cmd_args[OCC_REMOTE_CMD_ARGS_SIZE];
  } occ_remote_cmd;
  uint8_t spare2;
} TRAIN_COMMAND_ENTRY, *TRAIN_COMMAND_ENTRY_PTR;

#define TRAIN_COMMAND_ENTRIES_NUM 20
typedef struct train_command {
  TRAIN_COMMAND_ENTRY entries[TRAIN_COMMAND_ENTRIES_NUM];
  int frontier;
} TRAIN_COMMAND, *TRAIN_COMMAND_PTR;

extern unsigned short TRAIN_CMD_RAKEID( TRAIN_COMMAND_ENTRY Ce, unsigned short rakeID );
extern char *TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY Ce, char *pbuf, int size );
extern unsigned short TRAIN_CMD_DESTINATION_BLOCKID( TRAIN_COMMAND_ENTRY Ce, unsigned short dst_blockID );
extern unsigned short TRAIN_CMD_CURRENT_BLOCKID( TRAIN_COMMAND_ENTRY Ce, unsigned short crnt_blockID );
extern DST_ST_PLCODE TRAIN_CMD_DESTINATION_PLATFORM_CODE( TRAIN_COMMAND_ENTRY Ce, DST_ST_PLCODE dst_PLcode );
extern NEXT_ST_PLCODE TRAIN_CMD_NEXT_STATION_PLATFORM_CODE( TRAIN_COMMAND_ENTRY Ce, NEXT_ST_PLCODE next_PLcode );
extern CRNT_ST_PLCODE TRAIN_CMD_CURRENT_STATION_PLATFORM_CODE( TRAIN_COMMAND_ENTRY Ce, CRNT_ST_PLCODE crnt_PLcode );
extern BOOL TRAIN_CMD_OUT_OF_SERVICE( TRAIN_COMMAND_ENTRY Ce, BOOL oos );
extern BOOL TRAIN_CMD_KEEP_DOOR_CLOSED( TRAIN_COMMAND_ENTRY Ce, BOOL kdc );
extern unsigned char TRAIN_CMD_DESTINATION_NUMBER( TRAIN_COMMAND_ENTRY Ce, unsigned char dest_number );
extern unsigned char TRAIN_CMD_NEXT_STATION_NUMBER( TRAIN_COMMAND_ENTRY Ce, unsigned char next_st_number );
extern unsigned char TRAIN_CMD_CURRENT_STATION_NUMBER( TRAIN_COMMAND_ENTRY Ce, unsigned char crnt_st_number );
extern NEXT_ST_DOOROPEN_SIDE TRAIN_CMD_DIRECTION_OF_OPEANING_DOOR_AT_THE_NEXT_STATION( TRAIN_COMMAND_ENTRY Ce, NEXT_ST_DOOROPEN_SIDE next_st_doorside );
extern BOOL TRAIN_CMD_ORGIN_STATION( TRAIN_COMMAND_ENTRY Ce, BOOL origin_st );
extern BOOL TRAIN_CMD_SKIP_NEXT_STOP( TRAIN_COMMAND_ENTRY Ce, BOOL ss );
extern BOOL TRAIN_CMD_DEPARTURE_CONDITIONS_RELEASE( TRAIN_COMMAND_ENTRY Ce, BOOL dept_cond_releasing );
extern BOOL TRAIN_CMD_ATO_DEPARTURE_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL ATO_dept_cmd );
extern BOOL TRAIN_CMD_TRAIN_HOLD_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL th );
extern BOOL TRAIN_CMD_COASTING_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL coasting );
extern TRAIN_PERF_REGIME TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( TRAIN_COMMAND_ENTRY Ce, TRAIN_PERF_REGIME perf_regime );
extern BOOL TRAIN_CMD_LEAVE_NOW( TRAIN_COMMAND_ENTRY Ce, BOOL leave_now );
extern TRAIN_OPERATION_MODE TRAIN_CMD_INSTRUCTING_OPERATION_MODE( TRAIN_COMMAND_ENTRY Ce, TRAIN_OPERATION_MODE ope_mode );
extern unsigned char TRAIN_CMD_MAXIMUM_SPEED( TRAIN_COMMAND_ENTRY Ce, unsigned char maximum_speed );
extern unsigned char TRAIN_CMD_PASSENGER_ADDRESSING( TRAIN_COMMAND_ENTRY Ce, unsigned char pa );
extern BOOL TRAIN_CMD_TURNBACK_OR_SIDING( TRAIN_COMMAND_ENTRY Ce, BOOL turnback_siding );
extern unsigned short TRAIN_CMD_DWELL_TIME( TRAIN_COMMAND_ENTRY Ce, unsigned short dwell_time );
extern unsigned char  TRAIN_CMD_REGURATION_SPEED( TRAIN_COMMAND_ENTRY Ce, unsigned char reg_speed );
extern unsigned char  TRAIN_CMD_DEPARTURE_DIRECTION( TRAIN_COMMAND_ENTRY Ce, unsigned char dep_dir );
extern BOOL TRAIN_CMD_TRAIN_REMOVE( TRAIN_COMMAND_ENTRY Ce, BOOL train_remove );
extern BOOL TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( TRAIN_COMMAND_ENTRY Ce, BOOL releasing_emergency_stop );
extern BOOL TRAIN_CMD_ORDERING_EMERGENCY_STOP( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_emergency_stop );
extern BOOL TRAIN_CMD_AUTOMATIC_TURN_BACK_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL atb );
extern BOOL TRAIN_CMD_ORDERING_STANDBY( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_standby );
extern BOOL TRAIN_CMD_ORDERING_WAKEUP( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_wakeup );
extern REMOTE_DOOR_CLOSING TRAIN_CMD_CLOSING_TRAIN_DOOR_REMOTELY( TRAIN_COMMAND_ENTRY Ce, REMOTE_DOOR_CLOSING closing_doors );
extern REMOTE_DOOR_OPENING TRAIN_CMD_OPENING_TRAIN_DOOR_REMOTELY( TRAIN_COMMAND_ENTRY Ce, BOOL opening_doors );
extern BOOL TRAIN_CMD_ENERGY_SAVING_MODE( TRAIN_COMMAND_ENTRY Ce, BOOL ener_saving );
extern BOOL TRAIN_CMD_ORDERING_RESET_FOR_ONBOARD( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_reset_onboard );
extern SYSTEM_SWITCHING_CMD TRAIN_CMD_SYSTEM_EXCHANGE_COMMAND( TRAIN_COMMAND_ENTRY Ce, SYSTEM_SWITCHING_CMD system_switching_cmd );
extern BOOL TRAIN_CMD_VRS_F_RESET( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_reset_VRS_F );
extern BOOL TRAIN_CMD_VRS_B_RESET( TRAIN_COMMAND_ENTRY Ce, BOOL ordering_reset_VRS_B );
extern EMERGE_DOOR_RELEASING TRAIN_CMD_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND( TRAIN_COMMAND_ENTRY Ce, EMERGE_DOOR_RELEASING emergency_releasing_doors );
extern BOOL TRAIN_CMD_BACK_INCHING_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL back_inching_cmd );
extern BOOL TRAIN_CMD_INCHING_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL inching_cmd );
extern BOOL TRAIN_CMD_DYNAMIC_TEST_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL dynamic_test_cmd );
extern BOOL TRAIN_CMD_STATIC_TEST_COMMAND( TRAIN_COMMAND_ENTRY Ce, BOOL static_test_cmdv);

#define TRAIN_CMD_RAKEID( Ce, _rakeID ) ((unsigned short)((Ce).rakeID = htons( _rakeID )))
#define TRAIN_CMD_DESTINATION_BLOCKID( Ce, _dst_blockID )		\
  ((((Ce).dst_blockID_upper = ((htons( _dst_blockID ) & 0xFF00) >> 8)), ((Ce).dst_blockID_lower = (htons( _dst_blockID ) & 0x00FF))), \
   (unsigned short)(((Ce).dst_blockID_upper << 8) + (Ce).dst_blockID_lower))
#define TRAIN_CMD_CURRENT_BLOCKID( Ce, _crnt_blockID )			\
  ((((Ce).crnt_blockID_upper = (htons( _crnt_blockID ) & 0xFF00)), ((Ce).crnt_blockID_lower = (htons( _crnt_blockID ) & 0x00FF))), \
   (unsigned short)(((Ce).crnt_blockID_upper << 8) + (Ce).crnt_blockID_lower))
#define TRAIN_CMD_DESTINATION_PLATFORM_CODE( Ce, _dst_PLcode )		\
  ((Ce).flgs_1 = (((Ce).flgs_1 & ~FLG1_DESTINATION_PLATFORM_CODE) | (_dst_PLcode & 3)), \
   (DST_ST_PLCODE)((Ce).flgs_1 & FLG1_DESTINATION_PLATFORM_CODE))
#define TRAIN_CMD_NEXT_STATION_PLATFORM_CODE( Ce, _next_PLcode )	\
  (((Ce).flgs_1 = (((Ce).flgs_1 & ~FLG1_NEXT_STATION_PLATFORM_CODE) | ((_next_PLcode & 3) << 2))), \
   (NEXT_ST_PLCODE)(((Ce).flgs_1 & FLG1_NEXT_STATION_PLATFORM_CODE) >> 2))
#define TRAIN_CMD_CURRENT_STATION_PLATFORM_CODE( Ce, _crnt_PLcode )	\
  (((Ce).flgs_1 = (((Ce).flgs_1 & ~FLG1_CURRENT_STATION_PLATFORM_CODE) | ((_crnt_PLcode & 3) << 4))), \
   (CRNT_ST_PLCODE)(((Ce).flgs_1 & FLG1_CURRENT_STATION_PLATFORM_CODE) >> 4))
#define TRAIN_CMD_OUT_OF_SERVICE( Ce, _oos )				\
  (((Ce).flgs_1 = (((Ce).flgs_1 & ~FLG1_OUT_OF_SERVICE) | ((_oos & 1) << 6))), \
   (BOOL)((Ce).flgs_1 & FLG1_OUT_OF_SERVICE))
#define TRAIN_CMD_KEEP_DOOR_CLOSED( Ce, _kdc )				\
  (((Ce).flgs_1 = (((Ce).flgs_1 & ~FLG1_KEEP_DOOR_CLOSED) | ((_kdc & 1) << 7))), \
   (BOOL)((Ce).flgs_1 & FLG1_KEEP_DOOR_CLOSED))
#define TRAIN_CMD_DESTINATION_NUMBER( Ce, _dest_number ) ((unsigned char)((Ce).destination_number = (unsigned char)_dest_number))
#define TRAIN_CMD_NEXT_STATION_NUMBER( Ce, _next_st_number ) ((unsigned char)((Ce).next_station_number = (unsigned char)_next_st_number))
#define TRAIN_CMD_CURRENT_STATION_NUMBER( Ce, _crnt_st_number ) ((unsigned char)((Ce).current_station_number = (unsigned char)_crnt_st_number))
#define TRAIN_CMD_DIRECTION_OF_OPEANING_DOOR_AT_THE_NEXT_STATION( Ce, _next_st_doorside ) \
  (((Ce).flgs_2 = (((Ce).flgs_2 & ~FLG2_DIRECTION_OF_OPEANING_DOOR_AT_THE_NEXT_STATION) | ((_next_st_doorside & 3) << 2))), \
   (NEXT_ST_DOOROPEN_SIDE)(((Ce).flgs_2 & FLG2_DIRECTION_OF_OPEANING_DOOR_AT_THE_NEXT_STATION) >> 2))
#define TRAIN_CMD_ORGIN_STATION( Ce, _origin_st )			\
  (((Ce).flgs_2 = (((Ce).flgs_2 & ~FLG2_ORGIN_STATION) | ((_origin_st & 1) << 4))), \
   (BOOL)((Ce).flgs_2 & FLG2_ORGIN_STATION))
#define TRAIN_CMD_SKIP_NEXT_STOP( Ce, _ss )				\
  (((Ce).flgs_2 = (((Ce).flgs_2 & ~FLG2_SKIP_NEXT_STOP) | ((_ss & 1) << 5))), \
   (BOOL)((Ce).flgs_2 & FLG2_SKIP_NEXT_STOP))
#define TRAIN_CMD_DEPARTURE_CONDITIONS_RELEASE( Ce, _dept_cond_releasing ) \
  (((Ce).flgs_2 = (((Ce).flgs_2 & ~FLG2_DEPARTURE_CONDITIONS_RELEASE) | ((_dept_cond_releasing & 1) << 6))), \
   (BOOL)((Ce).flgs_2 & FLG2_DEPARTURE_CONDITIONS_RELEASE))
#define TRAIN_CMD_ATO_DEPARTURE_COMMAND( Ce, _ATO_dept_cmd )		\
  (((Ce).flgs_2 = (((Ce).flgs_2 & ~FLG2_ATO_DEPARTURE_COMMAND) | ((_ATO_dept_cmd & 1) << 7))), \
   (BOOL)((Ce).flgs_2 & FLG2_ATO_DEPARTURE_COMMAND))
#define TRAIN_CMD_TRAIN_HOLD_COMMAND( Ce, _th )				\
  (((Ce).flgs_3 = (((Ce).flgs_3 & ~FLG3_TRAIN_HOLD_COMMAND) | (_th & 1))), \
   (BOOL)((Ce).flgs_3 & FLG3_TRAIN_HOLD_COMMAND))
#define TRAIN_CMD_COASTING_COMMAND( Ce, _coasting )			\
  (((Ce).flgs_3 = (((Ce).flgs_3 & ~FLG3_COASTING_COMMAND) | ((_coasting & 1) << 1))), \
   (BOOL)((Ce).flgs_3 & FLG3_COASTING_COMMAND))
#define TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( Ce, _perf_regime )	\
  (((Ce).flgs_3 = (((Ce).flgs_3 & ~FLG3_TRAIN_PERFORMANCE_REGIME_COMMAND) | ((_perf_regime & 3) << 2))), \
   (TRAIN_PERF_REGIME)(((Ce).flgs_3 & FLG3_TRAIN_PERFORMANCE_REGIME_COMMAND) >> 2))
#define TRAIN_CMD_LEAVE_NOW( Ce, _leave_now )				\
  (((Ce).flgs_3 = (((Ce).flgs_3 & ~FLG3_LEAVE_NOW) | ((_leave_now & 1) << 4))), \
   (BOOL)((Ce).flgs_3 & FLG3_LEAVE_NOW))
#define TRAIN_CMD_INSTRUCTING_OPERATION_MODE( Ce, _ope_mode )	\
  (((Ce).flgs_3 = (((Ce).flgs_3 & ~FLG3_INSTRUCTING_OPERATION_MODE) | ((_ope_mode & 7) << 5))), \
   (TRAIN_OPERATION_MODE)(((Ce).flgs_3 & FLG3_INSTRUCTING_OPERATION_MODE) >> 5))
#define TRAIN_CMD_MAXIMUM_SPEED( Ce, _maximum_speed ) ((unsigned char)((Ce).maximum_speed = (unsigned char)_maximum_speed))
#define TRAIN_CMD_PASSENGER_ADDRESSING( Ce, _pa ) \
  ((unsigned char)((Ce).PA_and_turnback_or_siding = (((Ce).PA_and_turnback_or_siding & ~PA_THE_PASSENGER_ADDRESSING) | (_pa & PA_THE_PASSENGER_ADDRESSING))))
#define TRAIN_CMD_TURNBACK_OR_SIDING( Ce, _turnback_siding )		\
  (((Ce).PA_and_turnback_or_siding = (((Ce).PA_and_turnback_or_siding & ~TURNBACK_OR_SIDING) | ((_turnback_siding & 1) << 7))), \
   (BOOL)((Ce).PA_and_turnback_or_siding & TURNBACK_OR_SIDING))
#define TRAIN_CMD_DWELL_TIME( Ce, _dwell_time )				\
  (((Ce).flgs4_and_dwell = (((Ce).flgs4_and_dwell & ~FLG4_DWELL_TIME) | ((htons( _dwell_time ) & 0x0300) >> 8))), \
   ((Ce).dwell_time = (unsigned char)(htons( _dwell_time ) & 0x00FF)),	\
   (unsigned short)((((Ce).flgs4_and_dwell & FLG4_DWELL_TIME) << 8) + (Ce).dwell_time))
#define TRAIN_CMD_REGURATION_SPEED( Ce, _reg_speed )			\
  (((Ce).flgs4_and_dwell = (((Ce).flgs4_and_dwell & ~FLG4_REGURATION_SPEED) | ((_reg_speed & 15) << 2))), \
   (unsigned char)(((Ce).flgs4_and_dwell & FLG4_REGURATION_SPEED) >> 2))
#define TRAIN_CMD_DEPARTURE_DIRECTION( Ce, _dep_dir )			\
  (((Ce).flgs4_and_dwell = (((Ce).flgs4_and_dwell & ~FLG4_DEPARTURE_DIRECTION) | ((_dep_dir & 3) << 6))), \
   (unsigned char)(((Ce).flgs4_and_dwell & FLG4_DEPARTURE_DIRECTION) >> 6))
#define TRAIN_CMD_TRAIN_REMOVE( Ce, _train_remove )			\
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_TRAIN_REMOVE) | ((_train_remove & 1) << 1))), \
   (BOOL)((Ce).flgs_5 & FLG5_TRAIN_REMOVE))
#define TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( Ce, _releasing_emergency_stop ) \
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_ORDERING_RELEASE_FOR_EMERGENCY_STOP) | ((_releasing_emergency_stop & 1) << 2)), \
    (BOOL)((Ce).flgs_5 & FLG5_ORDERING_RELEASE_FOR_EMERGENCY_STOP)))
#define TRAIN_CMD_ORDERING_EMERGENCY_STOP( Ce, _ordering_emergency_stop ) \
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_ORDERING_EMERGENCY_STOP) | ((_ordering_emergency_stop & 1) << 3))), \
   (BOOL)((Ce).flgs_5 & FLG5_ORDERING_EMERGENCY_STOP))
#define TRAIN_CMD_AUTOMATIC_TURN_BACK_COMMAND( Ce, _atb ) \
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_AUTOMATIC_TURN_BACK_COMMAND) | ((_atb & 1) << 5))), \
   (BOOL)((Ce).flgs_5 & FLG5_AUTOMATIC_TURN_BACK_COMMAND))
#define TRAIN_CMD_ORDERING_STANDBY( Ce, _ordering_standby )		\
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_ORDERING_STANDBY) | ((_ordering_standby & 1) << 6))), \
   (BOOL)((Ce).flgs_5 & FLG5_ORDERING_STANDBY))
#define TRAIN_CMD_ORDERING_WAKEUP( Ce, _ordering_wakeup )		\
  (((Ce).flgs_5 = (((Ce).flgs_5 & ~FLG5_ORDERING_WAKEUP) | ((_ordering_wakeup & 1) << 7))), \
   (BOOL)((Ce).flgs_5 & FLG5_ORDERING_WAKEUP))
#define TRAIN_CMD_CLOSING_TRAIN_DOOR_REMOTELY( Ce, _closing_doors )	\
  (((Ce).flgs_6 = (((Ce).flgs_6 & ~FLG6_CLOSING_TRAIN_DOOR_REMOTELY) | (_closing_doors & 3))), \
   (REMOTE_DOOR_CLOSING)((Ce).flgs_6 & FLG6_CLOSING_TRAIN_DOOR_REMOTELY))
#define TRAIN_CMD_OPENING_TRAIN_DOOR_REMOTELY( Ce, _opening_doors )	\
  (((Ce).flgs_6 = (((Ce).flgs_6 & ~FLG6_OPENING_TRAIN_DOOR_REMOTELY) | ((_opening_doors & 3) << 2))), \
   (REMOTE_DOOR_OPENING)(((Ce).flgs_6 & FLG6_OPENING_TRAIN_DOOR_REMOTELY) >> 2))
#define TRAIN_CMD_ENERGY_SAVING_MODE( Ce, _ener_saving )		\
  (((Ce).flgs_6 = (((Ce).flgs_6 & ~FLG6_ENERGY_SAVING_MODE) | ((_ener_saving & 1) << 4))), \
   (BOOL)((Ce).flgs_6 & FLG6_ENERGY_SAVING_MODE))
#define TRAIN_CMD_ORDERING_RESET_FOR_ONBOARD( Ce, _ordering_reset_onboard ) \
  (((Ce).flgs_6 = (((Ce).flgs_6 & ~FLG6_ORDERING_RESET_FOR_ONBOARD) | ((_ordering_reset_onboard & 1) << 5))), \
   (BOOL)((Ce).flgs_6 & FLG6_ORDERING_RESET_FOR_ONBOARD))
#define TRAIN_CMD_SYSTEM_EXCHANGE_COMMAND( Ce, _system_switching_cmd )	\
  (((Ce).flgs_6 = (((Ce).flgs_6 & ~FLG6_SYSTEM_EXCHANGE_COMMAND) | ((_system_switching_cmd & 3) << 6))), \
   (SYSTEM_SWITCHING_CMD)(((Ce).flgs_6 & FLG6_SYSTEM_EXCHANGE_COMMAND) >> 6))
#define TRAIN_CMD_VRS_F_RESET( Ce, _ordering_reset_VRS_F )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_VS_F_RESET) | (_ordering_reset_VRS_F & 1))), \
   (BOOL)((Ce).flgs_7 & FLG7_VRS_F_RESET))
#define TRAIN_CMD_VRS_B_RESET( Ce, _ordering_reset_VRS_B )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_VRS_B_RESET) | ((_ordering_reset_VRS_B & 1) << 1))), \
   (BOOL)((Ce).flgs_7 & FLG7_VRS_B_RESET))
#define TRAIN_CMD_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND( Ce, _emergency_releasing_doors )	\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND) | ((_emergency_releasing_doors & 3) << 2))), \
   (EMERGE_DOOR_RELEASING)(((Ce).flgs_7 & FLG7_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND) >> 2))
#define TRAIN_CMD_BACK_INCHING_COMMAND( Ce, _back_inching_cmd )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_BACK_INCHING_COMMAND) | ((_back_inching_cmd & 1) << 4))), \
   (BOOL)((Ce).flgs_7 & FLG7_BACK_INCHING_COMMAND))
#define TRAIN_CMD_INCHING_COMMAND( Ce, _inching_cmd )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_INCHING_COMMAND) | ((_inching_cmd & 1) << 5))), \
   (BOOL)((Ce).flgs_7 & FLG7_INCHING_COMMAND))
#define TRAIN_CMD_DYNAMIC_TEST_COMMAND( Ce, _dynamic_test_cmd )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_DYNAMIC_TEST_COMMAND) | ((_dynamic_test_cmd & 1) << 6))), \
   (BOOL)((Ce).flgs_7 & FLG7_DYNAMIC_TEST_COMMAND))
#define TRAIN_CMD_STATIC_TEST_COMMAND( Ce, _static_test_cmd )		\
  (((Ce).flgs_7 = (((Ce).flgs_7 & ~FLG7_STATIC_TEST_COMMAND) | ((_static_test_cmd & 1) << 7))), \
   (BOOL)((Ce).flgs_7 & FLG7_STATIC_TEST_COMMAND))

#include <stdint.h>
#include "generic.h"
#include "misc.h"

#define FLG1_EFFECTIVENESS_OF_THE_AXLE_COUNTER 64
#define FLG1_SC_STATUS 128

#define FLG2_RAKEID 3
#define FLG2_EMERGENCY_BRAKE_ACTIVE 32
#define FLG2_WASH_MODE_STATUS 64
#define FLG2_DOOR_FAILED_TO_OPEN_CLOSE 128

#define FLG3_NOTICE_OF_TIME 7
#define FLG3_SKIP_NEXT_STOP 8
#define FLG3_PUSH_OF_DEPARTURE_BUTTON 16
#define FLG3_ATB_OK 32
#define FLG3_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE 64
#define FLG3_TRR 128

#define FLG4_VRS_ID_FORWARD 15
#define FLG4_VRS_F_RESET 16
#define FLG4_VOLTAGE_REDUCTION 32
#define FLG4_DYNAMIC_TESTABLE_SECTION 64
#define FLG4_STOP_DETECTION 128

#define FLG5_VRS_ID_BACK 15
#define FLG5_VRS_B_RESET 16
#define FLG5_DOOR_ENABLE 32
#define FLG5_OVERSPEED 64
#define FLG5_PASSING_THROUGH_BALISE 128

#define FLG6_DISTANCE_TO_STOPPING_POINT 7
#define FLG6_REMOTE_EB_RELEASE 8
#define FLG6_EB_RELEASE_ACK 16
#define FLG6_OPERATION_MODE 224

#define FLG7_TYPE_OF_VEHICLE 63
#define FLG7_TRAIN_DETECTION_MODE 192

#define FLG8_INITIALIZATION_STATUS 3
#define FLG8_SLEEP_ACK 4
#define FLG8_WAKEUP_ACK 8
#define FLG8_ONBOARD_ATC_ERROR_INFORMATION 16
#define FLG8_TRAIN_REMOVED 32
#define FLG8_CONDITION_OF_DEPARTURE_DETERRENCE 64
#define FLG8_P0_STOPPED 128

#define FLG9_RESULT_OF_RESET_FOR_ONBOARD 3
#define FLG9_SLEEP_MODE 12
#define FLG9_RUNNING_DIRECTION 48
#define FLG9_MASTER_STANDBY 192

#define FLG10_ATO_DRIVING_STATUS 7
#define FLG10_RESCUE_TRAIN 8
#define FLG10_STATE_OF_TRAIN_DOOR 48
#define FLG10_TRAIN_PERFORMANCE_REGIME 192
 
typedef struct train_info_entry {
  uint8_t flgs_2;
  uint8_t rakeID;
  uint8_t flgs_3;
  uint8_t occ_command_ID_ack;
  uint8_t flgs_4;
  uint8_t vrsID_forward;
  uint8_t flgs_5;
  uint8_t vrsID_back;
  uint16_t forward_train_pos;
  uint16_t forward_train_pos_offset;
  uint16_t back_train_pos;
  uint16_t back_train_pos_offset;
  uint16_t occupied_blk_forward;
  uint16_t occupied_blk_forward_offset;
  uint16_t occupied_blk_back;
  uint16_t occupied_blk_back_offset;
  uint16_t forward_train_pos_segment;
  uint16_t forward_train_pos_offset_segment;
  uint16_t back_train_pos_segment;
  uint16_t back_train_pos_offset_segment;
  uint16_t forward_train_occupied_pos_segment;
  uint16_t forward_train_occupied_pos_offset_segment;
  uint16_t back_train_occupied_segment;
  uint16_t back_train_occupied_pos_offset_segment;
  uint8_t train_speed;
  uint8_t train_max_speed;
  uint8_t flgs_6;
  uint8_t distance_to_SP;
  uint8_t flgs_7;
  uint8_t flgs_8;
  uint8_t flgs_9;
  uint8_t flgs_10;
  struct {
    uint8_t h;
    uint8_t l;
    uint16_t spare;
  } additional_info;
  uint16_t forward_safety_buffer_len;
  uint16_t backward_safety_buffer_len;
  struct {
    uint8_t h;
    uint8_t l;
  } EB_reason;
  struct {
    uint8_t h;
    uint8_t l;
  } EB_factor_of_SC;
  struct {
    uint8_t hh;
    uint8_t hl;
    uint8_t lh;
    uint8_t ll;
  } onboard_ATC_failure_info;
} TRAIN_INFO_ENTRY, *TRAIN_INFO_ENTRY_PTR;

#define TRAIN_INFO_ENTRIES_NUM 20
typedef struct train_info {
  TRAIN_INFO_ENTRY entries[TRAIN_INFO_ENTRIES_NUM];
} TRAIN_INFO, *TRAIN_INFO_PTR;

extern BOOL TRAIN_INFO_EFFECTIVENESS_OF_THE_AXLE_COUNTER( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_SC_STATUS( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_RAKEID( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_EMERGENCY_BRAKE_ACTIVE( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_WASH_MODE_STATUS( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_DOOR_FAILED_TO_OPEN_CLOSE( TRAIN_INFO_ENTRY Ie );
extern unsigned char TRAIN_INFO_NOTICE_OF_TIME( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_SKIP_NEXT_STOP( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_PUSH_OF_DEPARTURE_BUTTON( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_ATB_OK( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_TRR( TRAIN_INFO_ENTRY Ie );
extern unsigned char TRAIN_INFO_OCC_COMMANDID_ACK( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_VRS_ID_FORWARD( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_VRS_F_RESET( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_VOLTAGE_REDUCTION( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_DYNAMIC_TESTABLE_SECTION( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_STOP_DETECTION( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_VRS_ID_BACK( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_VRS_B_RESET( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_DOOR_ENABLE( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_OVERSPEED( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_PASSING_THROUGH_BALISE( TRAIN_INFO_ENTRY Ie );

extern unsigned short TRAIN_INFO_FORWARD_TRAIN_POS( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_POS( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_POS_OFFSET( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_OCCUPIED_BLK_FORWARD( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_OCCUPIED_BLK_FORWARD_OFFSET( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_OCCUPIED_BLK_BACK( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_OCCUPIED_BLK_BACK_OFFSET( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_TRAIN_POS_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_POS_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_POS_OFFSET_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_OCCUPIED_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACK_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT( TRAIN_INFO_ENTRY Ie );
extern unsigned char TRAIN_INFO_TRAIN_SPEED( TRAIN_INFO_ENTRY Ie );
extern unsigned char TRAIN_INFO_TRAIN_MAXSPEED( TRAIN_INFO_ENTRY Ie );

extern unsigned short TRAIN_INFO_DISTANCE_TO_STOPPING_POINT( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_REMOTE_EB_RELEASE( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_EB_RELEASE_ACK( TRAIN_INFO_ENTRY Ie );
extern TRAIN_OPERATION_MODE TRAIN_INFO_OPERATION_MODE( TRAIN_INFO_ENTRY Ie );
extern VEHICLE_TYPE TRAIN_INFO_TYPE_OF_VEHICLE( TRAIN_INFO_ENTRY Ie );
extern TRAIN_DETECTION_MODE TRAIN_INFO_TRAIN_DETECTION_MODE( TRAIN_INFO_ENTRY Ie );
extern INITIALIZATION_STATUS TRAIN_INFO_INITIALIZATION_STATUS( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_SLEEP_ACK( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_WAKEUP_ACK( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_ONBOARD_ATC_ERROR_INFORMATION( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_TRAIN_REMOVED( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_CONDITION_OF_DEPARTURE_DETERRENCE( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_P0_STOPPED( TRAIN_INFO_ENTRY Ie );
extern ONBOARD_RESET_RESULT TRAIN_INFO_RESULT_OF_RESET_FOR_ONBOARD( TRAIN_INFO_ENTRY Ie );
extern TRAIN_SLEEP_MODE TRAIN_INFO_SLEEP_MODE( TRAIN_INFO_ENTRY Ie );
extern TRAIN_MOVE_DIR TRAIN_INFO_RUNNING_DIRECTION( TRAIN_INFO_ENTRY Ie );
extern ONBOARD_ATC_MASTER_STANDBY TRAIN_INFO_MASTER_STANDBY( TRAIN_INFO_ENTRY Ie );
extern ATO_DRIVING_STATUS TRAIN_INFO_ATO_DRIVING_STATUS( TRAIN_INFO_ENTRY Ie );
extern BOOL TRAIN_INFO_RESCUE_TRAIN( TRAIN_INFO_ENTRY Ie );
extern TRAIN_DOORS_STATE TRAIN_INFO_STATE_OF_TRAIN_DOOR( TRAIN_INFO_ENTRY Ie );
extern TRAIN_PERF_REGIME TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_FORWARD_SAFETYBUF_LEN( TRAIN_INFO_ENTRY Ie );
extern unsigned short TRAIN_INFO_BACKWARD_SAFETYBUF_LEN( TRAIN_INFO_ENTRY Ie );
extern EB_REASON_VOBC TRAIN_INFO_EB_REASON( TRAIN_INFO_ENTRY Ie );
extern EB_REASON_SC TRAIN_INFO_FACTOR_IN_EMERGENCY_STOP_SC( TRAIN_INFO_ENTRY Ie );

#define TRAIN_INFO_EFFECTIVENESS_OF_THE_AXLE_COUNTER( Ie ) ((Ie).flgs_1 & FLG1_EFFECTIVENESS_OF_THE_AXLE_COUNTER)
#define TRAIN_INFO_SC_STATUS( Ie ) ((Ie).flgs_1 & FLG1_SC_STATUS)

#define TRAIN_INFO_RAKEID( Ie ) ((((Ie).flgs_2 & FLG2_RAKEID) << 8) + (Ie).rakeID)  // ((FLG2_RAKEID & flgs_2) << 8) + rakeID
#define TRAIN_INFO_EMERGENCY_BRAKE_ACTIVE( Ie ) ((Ie).flgs_2 & FLG2_EMERGENCY_BRAKE_ACTIVE)
#define TRAIN_INFO_WASH_MODE_STATUS( Ie ) ((Ie).flgs_2 & FLG2_WASH_MODE_STATUS)
#define TRAIN_INFO_DOOR_FAILED_TO_OPEN_CLOSE( Ie ) ((Ie).flgs_2 & FLG2_DOOR_FAILED_TO_OPEN_CLOSE)

#define TRAIN_INFO_NOTICE_OF_TIME( Ie ) ((Ie).flgs_3 & FLG3_NOTICE_OF_TIME)
#define TRAIN_INFO_SKIP_NEXT_STOP( Ie ) (((Ie).flgs_3 & FLG3_SKIP_NEXT_STOP) >> 3)
#define TRAIN_INFO_PUSH_OF_DEPARTURE_BUTTON( Ie ) (((Ie).flgs_3 & FLG3_PUSH_OF_DEPARTURE_BUTTON) >> 4)
#define TRAIN_INFO_ATB_OK( Ie ) (((Ie).flgs_3 & FLG3_ATB_OK) >> 5)
#define TRAIN_INFO_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE( Ie ) (((Ie).flgs_3 & FLG3_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE) >> 6)
#define TRAIN_INFO_TRR( Ie ) (((Ie).flgs_3 & FLG3_TRR) >> 7)

#define TRAIN_INFO_OCC_COMMANDID_ACK( Ie ) ((Ie).occ_command_ID_ack)
#define TRAIN_INFO_VRS_ID_FORWARD( Ie ) ((((Ie).flgs_4 & FLG4_VRS_ID_FORWARD) << 8)  + (Ie).vrsID_forward)  // ((FLG4_VRS_ID_FORWARD & flgs_4) << 8) + vrsID_forward
#define TRAIN_INFO_VRS_F_RESET( Ie ) (((Ie).flgs_4 & FLG4_VRS_F_RESET) >> 4)
#define TRAIN_INFO_VOLTAGE_REDUCTION( Ie ) (((Ie).flgs_4 & FLG4_VOLTAGE_REDUCTION) >> 5)
#define TRAIN_INFO_DYNAMIC_TESTABLE_SECTION( Ie ) (((Ie).flgs_4 & FLG4_DYNAMIC_TESTABLE_SECTION) >> 6)
#define TRAIN_INFO_STOP_DETECTION( Ie ) (((Ie).flgs_4 & FLG4_STOP_DETECTION) >> 7)

#define TRAIN_INFO_VRS_ID_BACK( Ie ) ((((Ie).flgs_5 & FLG5_VRS_ID_BACK) << 8) + (Ie).vrsID_back)  // ((FLG5_VRS_ID_BACK & flgs_5) << 8) + vrsID_back
#define TRAIN_INFO_VRS_B_RESET( Ie ) (((Ie).flgs_5 & FLG5_VRS_B_RESET) >> 4)
#define TRAIN_INFO_DOOR_ENABLE( Ie ) (((Ie).flgs_5 & FLG5_DOOR_ENABLE) >> 5)
#define TRAIN_INFO_OVERSPEED( Ie ) (((Ie).flgs_5 & FLG5_OVERSPEED) >> 6)
#define TRAIN_INFO_PASSING_THROUGH_BALISE( Ie ) (((Ie).flgs_5 & FLG5_PASSING_THROUGH_BALISE) >> 7)

#define TRAIN_INFO_FORWARD_TRAIN_POS( Ie ) (ntohs( (Ie).forward_train_pos ))
#define TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET( Ie ) (ntohs( (Ie).forward_train_pos_offset ))
#define TRAIN_INFO_BACK_TRAIN_POS( Ie ) (ntohs( (Ie).back_train_pos ))
#define TRAIN_INFO_BACK_TRAIN_POS_OFFSET( Ie ) (ntohs( (Ie).back_train_pos_offset ))
#define TRAIN_INFO_OCCUPIED_BLK_FORWARD( Ie ) (ntohs( (Ie).occupied_blk_forward ))
#define TRAIN_INFO_OCCUPIED_BLK_FORWARD_OFFSET( Ie ) (ntohs( (Ie).occupied_blk_forward_offset ))
#define TRAIN_INFO_OCCUPIED_BLK_BACK( Ie ) (ntohs( (Ie).occupied_blk_back ))
#define TRAIN_INFO_OCCUPIED_BLK_BACK_OFFSET( Ie ) ntohs( (Ie).occupied_blk_back_offset )
#define TRAIN_INFO_FORWARD_TRAIN_POS_SEGMENT( Ie ) ntohs( (Ie).forward_train_pos_segment )
#define TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET_SEGMENT( Ie ) ntohs( (Ie).forward_train_pos_offset_segment )
#define TRAIN_INFO_BACK_TRAIN_POS_SEGMENT( Ie ) ntohs( (Ie).back_train_pos_segment )
#define TRAIN_INFO_BACK_TRAIN_POS_OFFSET_SEGMENT( Ie ) ntohs( (Ie).back_train_pos_offset_segment )
#define TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_SEGMENT( Ie ) ntohs( (Ie).forward_train_occupied_pos_segment )
#define TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT( Ie ) ntohs( (Ie).forward_train_occupied_pos_offset_segment )
#define TRAIN_INFO_BACK_TRAIN_OCCUPIED_SEGMENT( Ie ) ntohs( (Ie).back_train_occupied_segment )
#define TRAIN_INFO_BACK_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT( Ie ) ntohs( (Ie).back_train_occupied_pos_offset_segment )
#define TRAIN_INFO_TRAIN_SPEED( Ie ) ((Ie).train_speed)
#define TRAIN_INFO_TRAIN_MAXSPEED( Ie ) ((Ie).train_max_speed)

#define TRAIN_INFO_DISTANCE_TO_STOPPING_POINT( Ie ) ((((Ie).flgs_6 & FLG6_DISTANCE_TO_STOPPING_POINT) << 8) + (Ie).distance_to_SP)  // ((FLG6_DISTANCE_TO_STOPPING_POINT & flgs_6) << 8) + distance_to_SP
#define TRAIN_INFO_REMOTE_EB_RELEASE( Ie ) ((Ie).flgs_6 & FLG6_REMOTE_EB_RELEASE)
#define TRAIN_INFO_EB_RELEASE_ACK( Ie ) ((Ie).flgs_6 & FLG6_EB_RELEASE_ACK)
#define TRAIN_INFO_OPERATION_MODE( Ie ) (((Ie).flgs_6 & FLG6_OPERATION_MODE) >> 5)

#define TRAIN_INFO_TYPE_OF_VEHICLE( Ie ) ((Ie).flgs_7 & FLG7_TYPE_OF_VEHICLE)
#define TRAIN_INFO_TRAIN_DETECTION_MODE( Ie ) (((Ie).flgs_7 & FLG7_TRAIN_DETECTION_MODE) >> 6)

#define TRAIN_INFO_INITIALIZATION_STATUS( Ie ) ((Ie).flgs_8 & FLG8_INITIALIZATION_STATUS)
#define TRAIN_INFO_SLEEP_ACK( Ie ) ((Ie).flgs_8 & FLG8_SLEEP_ACK)
#define TRAIN_INFO_WAKEUP_ACK( Ie ) ((Ie).flgs_8 & FLG8_WAKEUP_ACK)
#define TRAIN_INFO_ONBOARD_ATC_ERROR_INFORMATION( Ie ) ((Ie).flgs_8 & FLG8_ONBOARD_ATC_ERROR_INFORMATION)
#define TRAIN_INFO_TRAIN_REMOVED( Ie ) ((Ie).flgs_8 & FLG8_TRAIN_REMOVED)
#define TRAIN_INFO_CONDITION_OF_DEPARTURE_DETERRENCE( Ie ) ((Ie).flgs_8 & FLG8_CONDITION_OF_DEPARTURE_DETERRENCE)
#define TRAIN_INFO_P0_STOPPED( Ie ) ((Ie).flgs_8 & FLG8_P0_STOPPED)

#define TRAIN_INFO_RESULT_OF_RESET_FOR_ONBOARD( Ie ) ((Ie).flgs_9 & FLG9_RESULT_OF_RESET_FOR_ONBOARD)
#define TRAIN_INFO_SLEEP_MODE( Ie ) (((Ie).flgs_9 & FLG9_SLEEP_MODE) >> 2)
#define TRAIN_INFO_RUNNING_DIRECTION( Ie ) (((Ie).flgs_9 & FLG9_RUNNING_DIRECTION) >> 4)
#define TRAIN_INFO_MASTER_STANDBY( Ie ) (((Ie).flgs_9 & FLG9_MASTER_STANDBY) >> 6)

#define TRAIN_INFO_ATO_DRIVING_STATUS( Ie ) ((Ie).flgs_10 & FLG10_ATO_DRIVING_STATUS)
#define TRAIN_INFO_RESCUE_TRAIN( Ie ) ((Ie).flgs_10 & FLG10_RESCUE_TRAIN)
#define TRAIN_INFO_STATE_OF_TRAIN_DOOR( Ie ) (((Ie).flgs_10 & FLG10_STATE_OF_TRAIN_DOOR) >> 4)
#define TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( Ie ) (((Ie).flgs_10 & FLG10_TRAIN_PERFORMANCE_REGIME) >> 6)
#define TRAIN_INFO_FORWARD_SAFETYBUF_LEN( Ie ) (ntohs( (Ie).forward_safety_buffer_len ))
#define TRAIN_INFO_BACKWARD_SAFETYBUF_LEN( Ie ) (ntohs( (Ie).backward_safety_buffer_len ))
#define TRAIN_INFO_EB_REASON( Ie ) ((((uint16_t)((Ie).EB_reason.h)) << 8) + (uint16_t)((Ie).EB_reason.l))
#define TRAIN_INFO_FACTOR_IN_EMERGENCY_STOP_SC( Ie ) ((((uint16_t)((Ie).EB_factor_of_SC.h)) << 8) + (uint16_t)((Ie).EB_factor_of_SC.l))

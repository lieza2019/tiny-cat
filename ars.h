#ifndef ARS_H
#define ARS_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

typedef enum stopping_point_code {
  SP_NONSENS,
    SP_73, // JLA_PL1
    SP_74, // JLA_PL2
  SP_75, // JLA_PL3
  SP_D4, // JLA_TB4
  SP_D9, // JLA_TB3
    SP_76, // KIKJ_PL2
    SP_77, // KIKJ_PL1
    SP_78, // OKBS_PL2
    SP_79, // OKBS_PL1
    SP_80, // BTGD_PL2
    SP_81, // BTGD_PL1
    SP_D5, // BTGD_TB1
    SP_D0, // BTGD_TB2
  END_OF_SPs
} STOPPING_POINT_CODE;

typedef enum ars_events_over_sp {
  ARS_DOCK_DETECTED,
  ARS_LEAVE_DETECTED,
  ARS_SKIP_DETECTED,
  ARS_DETECTS_NONE
} ARS_EVENTS_OVER_SP;
typedef struct ars_event_on_sp {
  STOPPING_POINT_CODE sp;
  ARS_EVENTS_OVER_SP detail;
} ARS_EVENT_ON_SP, *ARS_EVENT_ON_SP_PTR;

typedef enum offset_time_to_fire {
  OFFSET_TO_ROUTESET,
  OFFSET_TO_DEPARTURE,
  END_OF_OFFSET_TIMES
} OFFSET_TIME_TO_FIRE;

typedef enum perfreg_level {
  PERFREG_SLOW,
  PERFREG_NORMAL,
  PERFREG_FAST,
  END_OF_PERFREG
} PERFREG_LEVEL;

typedef enum _crew_id {
  CREW_ID0001,
  CREW_ID0002,
  CREW_ID0003,
  CREW_NO_ID,
  END_OF_CREWIDs
} CREW_ID;

typedef enum ars_scheduled_cmd {
  ARS_SCHEDULED_ROUTESET,
  ARS_SCHEDULED_ROUTEREL,
  ARS_SCHEDULED_ARRIVAL,
  ARS_SCHEDULED_DEPT,
  ARS_SCHEDULED_SKIP,
  END_OF_SCHEDULED_CMDS
} ARS_SCHEDULED_CMD;

typedef int TIME_DIFF;
typedef int JOURNEY_ID;
typedef int DWELL_ID;

typedef struct ars_assoc_time {
  int hour;
  int minute;
  int second;
  int year;
  int month;
  int day;
} ARS_ASSOC_TIME, *ARS_ASSOC_TIME_PTR;
typedef struct scheduled_command {
  ARS_SCHEDULED_CMD cmd;
  union {
    struct { // for ARS_SCHEDULED_ROUTESET
      int nth_routeset;
      IL_OBJ_INSTANCES route_id;
      BOOL is_dept_route;
      ARS_ASSOC_TIME dept_time;
    } sch_routeset;
    struct { // for ARS_SCHEDULED_ROUTEREL
      int nth_routerel;
      IL_OBJ_INSTANCES route_id;
      ARS_ASSOC_TIME dept_time;
    } sch_routerel;
    struct { // for ARS_SCHEDULED_ARRIVAL
      DWELL_ID dw_seq;
      STOPPING_POINT_CODE arr_sp;
      ARS_ASSOC_TIME arr_time;
    } sch_arriv;
    struct { // for ARS_SCHEDULED_DEPT
      DWELL_ID dw_seq;
      TIME_DIFF dwell;
      STOPPING_POINT_CODE dept_sp;
      ARS_ASSOC_TIME dept_time;
      BOOL is_revenue;
      PERFREG_LEVEL perf_lev;
      CREW_ID crew_id;
      struct {
	BOOL L, R;
      } dept_dir;
    } sch_dept;
    struct { // ARS_SCHEDULED_SKIP
      DWELL_ID dw_seq;
      STOPPING_POINT_CODE ss_sp;
      ARS_ASSOC_TIME pass_time;
      STOPPING_POINT_CODE pass_sp;
      BOOL is_revenue;
      PERFREG_LEVEL perf_lev;
      CREW_ID crew_id;
    } sch_skip;
  } attr;
  JOURNEY_ID jid;
  BOOL check;
  struct {
    struct {
      struct scheduled_command *pNext;
      struct scheduled_command *pSucc;
    } journey;
    struct {
      struct scheduled_command *pNext;
      struct scheduled_command *pFellow;
    } sp_sch;
  } ln;
} SCHEDULED_COMMAND, *SCHEDULED_COMMAND_PTR;
typedef const struct scheduled_command *SCHEDULED_COMMAND_C_PTR;

typedef enum ars_reasons {
  ARS_NO_ROUTESET_CMD,
  ARS_NO_TRIGGERED,
  ARS_FOUND_TRAINS_AHEAD,
  ARS_CTRL_TRACKS_ROUTELOCKED,
  ARS_CTRL_TRACKS_DROP,
  ARS_WAITING_ROUTESET_TIME,
  ARS_PRED_DEPTRAINS_FOUND,
  ARS_WAITING_PRED_DEPTRAINS_AT_DST,
  ARS_ROUTE_CONTROLLED_NORMALLY,
  ARS_MUTEX_BLOCKED,
  END_OF_ARS_REASONS
} ARS_REASONS;
extern const char *cnv2str_ars_reasons[];

#define DEFALUT_ROUTESET_OFFSET 30
#define DEFAULT_DEPARTURE_OFFSET 7
typedef struct system_params {
  int routeset_offset;
  int departure_offset; 
} SYSTEM_PARAMS, *SYSTEM_PARAMS_PTR;

extern SYSTEM_PARAMS tiny_system_params;

#endif // ARS_H

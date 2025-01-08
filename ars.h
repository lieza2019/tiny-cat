#ifndef ARS_H
#define ARS_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

typedef int TIME_DIFF;
typedef int JOURNEY_ID;
typedef int DWELL_ID, *DWELL_ID_PTR;

#define DEFALUT_ROUTESET_OFFSET 30
#define DEFAULT_DEPARTURE_OFFSET 7
typedef struct system_params {
  int routeset_offset;
  int departure_offset; 
} SYSTEM_PARAMS, *SYSTEM_PARAMS_PTR;
typedef struct ars_assoc_time {
  int hour;
  int minute;
  int second;
  int year;
  int month;
  int day;
} ARS_ASSOC_TIME, *ARS_ASSOC_TIME_PTR;
typedef struct ars_assoc_time const *ARS_ASSOC_TIME_C_PTR;

typedef enum ars_scheduled_cmd {
  ARS_SCHEDULED_ROUTESET,
  ARS_SCHEDULED_ROUTEREL,
  ARS_SCHEDULED_ARRIVAL,
  ARS_SCHEDULED_DEPT,
  ARS_SCHEDULED_SKIP,
  END_OF_SCHEDULED_CMDS,
  ARS_CMD_DONT_CURE
} ARS_SCHEDULED_CMD;

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
  OFFSET_NOTHING,
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
  ARS_NOMORE_SCHEDULED_CMDS,
  ARS_ILLEGAL_CMD_ROUTESET,
  ARS_ILLEGAL_CMD_ROUTEREL,
  ARS_ILLEGAL_CMD_ARRIV,
  ARS_ILLEGAL_CMD_DEPT,
  ARS_ILLEGAL_CMD_SKIP,
  END_OF_ARS_REASONS
} ARS_REASONS;
extern const char *cnv2str_ars_reasons[];

extern SYSTEM_PARAMS tiny_system_params;

#endif // ARS_H

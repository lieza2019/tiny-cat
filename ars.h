#include "generic.h"
#include "misc.h"
#include "cbi.h"

typedef enum sp_id {
  SP_01,
  SP_02,
  END_OF_SPs
} SP_ID;

typedef enum perfreg_level {
  PERFREG_SLOW,
  PERFREG_NORMAL,
  PERFREG_FAST,
  END_OF_PERFREG
} PERFREG_LEVEL;

typedef enum ars_scheduled_cmd {
  ARS_SCHEDULED_ROUTESET,
  ARS_SCHEDULED_ROUTEREL,
  ARS_SCHEDULED_ARRIVAL,
  ARS_SCHEDULED_DEPT,
  ARS_SCHEDULED_SKIP,
  END_OF_SCHEDULED_CMDS
} ARS_SCHEDULED_CMD;

typedef int JOURNEY_ID;
typedef int DWELL_ID;

typedef struct _scheduled_command {
  ARS_SCHEDULED_CMD cmd;
  union {
    struct {
      struct {
	JOURNEY_ID jid;
	int nth_routeset;
      } seq_in_journey;
      IL_OBJ_INSTANCES route_id;
      BOOL is_dept_route;
      struct {
	int year, month, day, hour, minute, second;
      } dept_time;
    } sch_routeset;
    struct {
      struct {
	JOURNEY_ID jid;
	int nth_routerel;
      } seq_in_journey;
      IL_OBJ_INSTANCES route_id;
      struct {
	int year, month, day, hour, minute, second;
      } dept_time;
    } sch_routerel;
    struct {
      DWELL_ID dw_id;
      SP_ID arr_sp;
      struct {
	int year, month, day, hour, minute, second;
      } arr_time;
    } sch_arriv;
    struct {
      DWELL_ID dw_id;
      SP_ID dept_sp;
      struct {
	int year, month, day, hour, minute, second;
      } dept_time;
      BOOL is_skip;
      PERFREG_LEVEL perf_lev;
      ;
    } sch_dept;
  } attr;
} SCHEDULED_COMMAND, *SCHEDULED_COMMAND_PTR;

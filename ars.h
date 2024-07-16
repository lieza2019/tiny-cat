#include "generic.h"
#include "misc.h"
#include "cbi.h"

typedef enum ars_scheduled_cmd {
  ARS_SCHEDULED_ROUTESET,
  ARS_SCHEDULED_ROUTEREL,
  ARS_SCHEDULED_ARRIVAL,
  ARS_SCHEDULED_DEPT,
  ARS_SCHEDULED_SKIP,
  END_OF_SCHEDULED_CMDS
} ARS_SCHEDULED_CMD;

typedef int JOURNEY_ID;

typedef struct _scheduled_command {
  ARS_SCHEDULED_CMD cmd;
  union {
    struct {
      struct {
	JOURNEY_ID jid;
	int nth;
      } chk_conf;
      IL_OBJ_INSTANCES route_id;
      BOOL is_dept_route;
      struct {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
      } dept_time;
    } sch_routeset;
  } attr;
} SCHEDULED_COMMAND, *SCHEDULED_COMMAND_PTR;

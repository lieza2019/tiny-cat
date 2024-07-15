#include "generic.h"
#include "misc.h"

typedef enum ars_scheduled_cmd {
  ARS_SCHEDULED_ROUTESET,
  ARS_SCHEDULED_ROUTEREL,
  ARS_SCHEDULED_ARRIVAL,
  ARS_SCHEDULED_DEPT,
  ARS_SCHEDULED_SKIP,
  END_OF_SCHEDULED_CMDS
} ARS_SCHEDULED_CMD;

typedef struct _scheduled_command {
  ARS_SCHEDULED_CMD cmd;
  union {
    struct {
      JOURNEY_ID jid;
      int nth;
    } chk_conf;
    IL_OBJ_INSTANCES route_id;
    BOOL is_dept;
    UTC_time dept_time;
  } u;
} SCHEDULED_COMMAND, *SCHEDULED_COMMAND_PTR;

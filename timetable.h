#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbtc.h"

#define MAX_JOURNEYS_IN_TIMETABLE 1024
#define SCHEDULED_COMMANDS_NODEBUF_SIZE 65536

typedef int JOURNEY_ID;
typedef struct journey {
  BOOL valid;
  time_t start_time;
  time_t finish_time;
  JOURNEY_ID jid;
  SCHEDULED_COMMAND_PTR past_commands;
  struct {
    SCHEDULED_COMMAND_PTR pcmds;
    SCHEDULED_COMMAND_PTR pNext;
  } scheduled_commands;
  TINY_TRAIN_STATE_PTR ptrain_ctrl;
} JOURNEY, *JOURNEY_PTR;
typedef const struct journey *JOURNEY_C_PTR;

typedef struct schedule_at_sp {
  SCHEDULED_COMMAND_PTR pFirst;
  SCHEDULED_COMMAND_PTR pNext;
} SCHEDULE_AT_SP, *SCHEDULE_AT_SP_PTR;
struct journeys {
  JOURNEY journey;
  int rake_id;
};
typedef struct timetable {
  struct journeys journeys[MAX_JOURNEYS_IN_TIMETABLE];  
  int num_journeys;
  struct journeys *lkup[MAX_JOURNEYS_IN_TIMETABLE + 1]; // 1 origin, for jid > 0.
  SCHEDULE_AT_SP sp_schedule[END_OF_SPs];
} TIMETABLE, *TIMETABLE_PTR;

#include "timetable_def.h"

extern void cons_sp_schedule ( void );

extern SCHEDULED_COMMAND_PTR sch_cmd_newnode( void );
extern STOPPING_POINT_CODE ars_judge_arriv_dept_skip ( ARS_EVENT_ON_SP_PTR pdetects, TINY_TRAIN_STATE_PTR pT );
extern ARS_REASONS ars_ctrl_route_on_journey ( TIMETABLE_PTR pT, JOURNEY_PTR pJ );
extern SCHEDULED_COMMAND_PTR ars_sch_cmd_ack ( JOURNEY_PTR pJ );

#endif // TIMETABLE_H

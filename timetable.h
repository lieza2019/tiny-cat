#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbtc.h"

#define MAX_JOURNEYS_IN_TIMETABLE 1024

typedef struct journey {
  time_t start_time;
  time_t finish_time;
  SCHEDULED_COMMAND_PTR past_commands;
  struct {
    SCHEDULED_COMMAND_PTR pcmds;
    SCHEDULED_COMMAND_PTR pNext;
  } scheduled_commands;
  TINY_TRAIN_STATE_PTR ptrain_ctrl;
} JOURNEY, *JOURNEY_PTR;

typedef struct schedule_at_sp {
  SCHEDULED_COMMAND_PTR pfirst;
  SCHEDULED_COMMAND_PTR pnext;
} SCHEDULE_AT_SP, *SCHEDULE_AT_SP_PTR;
typedef struct timetable {
  struct {
    JOURNEY journey;
    int rake_id;
  } journeys[MAX_JOURNEYS_IN_TIMETABLE];
  SCHEDULE_AT_SP sp_schedule[END_OF_SPs];
} TIMETABLE, *TIMETABLE_PTR;

extern SCHEDULED_COMMAND_PTR sch_cmd_newnode( void );
extern ARS_REASONS ars_ctrl_route_on_journey ( TIMETABLE_PTR pT, JOURNEY_PTR pJ );

#endif // TIMETABLE_H

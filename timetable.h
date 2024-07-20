#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "ars.h"

typedef struct journey {
  time_t start_time;
  time_t finish_time;
  SCHEDULED_COMMAND_PTR past_commands;
  struct {
    SCHEDULED_COMMAND_PTR pcmds;
    SCHEDULED_COMMAND_PTR pNext;
  } scheduled_commands;
} JOURNEY, *JOURNEY_PTR;

#define MAX_JOURNEYS_IN_TIMETABLE 1024
typedef struct timetable {
  struct {
    JOURNEY journey;
    int rake_id;
  } journeys[MAX_JOURNEYS_IN_TIMETABLE];
} TIMETABLE, *TIMETABLE_PTR;

#endif // TIMETABLE_H

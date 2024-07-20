#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "ars.h"

#define MAX_JOURNEYS_IN_TIMETABLE 1024
typedef struct timetable {
  struct {
    JOURNEY journey;
    int rake_id;
  } journeys[MAX_JOURNEYS_IN_TIMETABLE];
} TIMETABLE, *TIMETABLE_PTR;

#endif // TIMETABLE_H

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbtc.h"
#include "interlock.h"

#define MAX_JOURNEYS_IN_TIMETABLE 1024
#define SCHEDULED_COMMANDS_NODEBUF_SIZE 65536

typedef struct scheduled_command {
  JOURNEY_ID jid;
  ARS_SCHEDULED_CMD cmd;
  union {
    struct { // for ARS_SCHEDULED_ROUTESET
      int nth_routeset;
      IL_SYM route_id;
      BOOL is_dept_route;
      ARS_ASSOC_TIME dept_time;
      ROUTE_PTR proute_prof;
    } sch_roset;
    struct { // for ARS_SCHEDULED_ROUTEREL
      int nth_routerel;
      IL_SYM route_id;
      ARS_ASSOC_TIME dept_time;
    } sch_rorel;
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
  BOOL checked;
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

typedef int JOURNEY_ID;
typedef struct journey {
  BOOL valid;
  JOURNEY_ID jid;
  ARS_ASSOC_TIME start_time;
  struct {
    SCHEDULED_COMMAND_PTR pcmds;
    SCHEDULED_COMMAND_PTR pNext;
  } scheduled_commands;
  ARS_ASSOC_TIME finish_time;
  SCHEDULED_COMMAND_PTR past_commands;
  TINY_TRAIN_STATE_PTR ptrain_ctrl;
} JOURNEY, *JOURNEY_PTR;
typedef const struct journey *JOURNEY_C_PTR;

typedef struct schedule_at_sp {
  int num_events;
  SCHEDULED_COMMAND_PTR pFirst;
  SCHEDULED_COMMAND_PTR pNext;
} SCHEDULE_AT_SP, *SCHEDULE_AT_SP_PTR;
struct journeys {
  JOURNEY journey;
  int rake_id;
};
typedef struct timetable {
  int num_journeys;
  struct journeys journeys[MAX_JOURNEYS_IN_TIMETABLE];
  struct journeys *lkup[MAX_JOURNEYS_IN_TIMETABLE + 1]; // 1 origin, for jid > 0.
  SCHEDULE_AT_SP sp_schedule[END_OF_SPs];
} TIMETABLE, *TIMETABLE_PTR;

#include "timetable_def.h"

extern TIMETABLE online_timetable;

extern void cons_sp_schedule ( void );
extern void makeup_online_timetable ( void );

extern SCHEDULED_COMMAND_PTR sch_cmd_newnode ( void );
extern time_t mktime_of_cmd ( struct tm *pT, ARS_ASSOC_TIME_C_PTR ptime_cmd );
extern STOPPING_POINT_CODE ars_judge_arriv_dept_skip ( ARS_EVENT_ON_SP_PTR pdetects, TINY_TRAIN_STATE_PTR pT );
extern ARS_REASONS ars_ctrl_route_on_journey ( TIMETABLE_PTR pT, JOURNEY_PTR pJ );
extern SCHEDULED_COMMAND_PTR ars_sch_cmd_ack ( JOURNEY_PTR pJ );

#endif // TIMETABLE_H

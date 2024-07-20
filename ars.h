#ifndef ARS_H
#define ARS_H

#include <time.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

typedef enum sp_id {
  SP_73, // JLA_PL1
  SP_74, // JLA_PL2
  SP_76, // KIKJ_PL2
  SP_77, // KIKJ_PL1
  SP_78, // OKBS_PL2
  SP_79, // OKBS_PL1
  SP_80, // BTGD_PL2
  SP_81, // BTGD_PL1
  SP_D4, // KIKJ_TB4
  SP_D9, // KIKJ_TB3
  END_OF_SPs
} SP_ID;

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

typedef struct _scheduled_command {
  ARS_SCHEDULED_CMD cmd;
  union {
    struct { // for ARS_SCHEDULED_ROUTESET
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
    struct { // for ARS_SCHEDULED_ROUTEREL
      struct {
	JOURNEY_ID jid;
	int nth_routerel;
      } seq_in_journey;
      IL_OBJ_INSTANCES route_id;
      struct {
	int year, month, day, hour, minute, second;
      } dept_time;
    } sch_routerel;
    struct { // for ARS_SCHEDULED_ARRIVAL
      DWELL_ID dw_id;
      SP_ID arr_sp;
      struct {
	int year, month, day, hour, minute, second;
      } arr_time;
    } sch_arriv;
    struct { // for ARS_SCHEDULED_DEPT
      DWELL_ID dw_id;
      TIME_DIFF dwell;
      SP_ID dept_sp;
      struct {
	int year, month, day, hour, minute, second;
      } dept_time;
      BOOL is_revenue;
      PERFREG_LEVEL perf_lev;
      CREW_ID crew_id;
      struct {
	BOOL L, R;
      } dept_dir;
    } sch_dept;
    struct { // ARS_SCHEDULED_SKIP
      DWELL_ID dw_id;
      SP_ID ss_sp;
      struct {
	int year, month, day, hour, minute, second;
      } pass_time;
      BOOL is_revenue;
      PERFREG_LEVEL perf_lev;
      CREW_ID crew_id;
    } sch_skip;
  } attr;
  struct _scheduled_command *pNext;
} SCHEDULED_COMMAND, *SCHEDULED_COMMAND_PTR;

extern SCHEDULED_COMMAND_PTR sch_cmd_newnode( void );

#endif // ARS_H

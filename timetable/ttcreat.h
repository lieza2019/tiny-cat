#include "../generic.h"
#include "../cbtc.h"

#define DEFAULT_DWELL_TIME 17 // in sec.
#define DEFAULT_PERFLEVEL PERFREG_NORMAL
#define DEFAULT_REVENUE FALSE
#define DEFAULT_CREWID -1

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_ROUTENAME_LEN 16
#define MAX_SPNAME_LEN 8
#define MAX_TRIP_ROUTES 8
#define MAX_TRIPS 256
#define MAX_RJ_ASGNMENTS 64
#define MAX_JOURNEYS 256

typedef enum kind {
  DATE_SPEC = 1,
  TIME_SPEC,
  ST_PLTB,
  ST_PLTB_PAIR,
  SP,
  SP_PAIR,
  ROUTE,
  ROUTES,
  TRIP,
  TRIPS,
  PERF_REGIME,
  REVENUE_STAT,
  CREWID,
  RJ_ASGN,
  RJ_ASGNS,
  JOURNEY,
  JOURNEYS,
  UNKNOWN
} KIND;
extern char *cnv2str_kind ( char *pstr, KIND kind, const int buflen );

typedef enum ars_sp_cond {
  DWELL = 1,
  SKIP
} ARS_SP_COND;
extern char *cnv2str_sp_cond ( char *pstr, ARS_SP_COND sp_cond, const int buflen );

typedef struct err_stat {
  BOOL err_trip_journey;
  BOOL err_routes;
  BOOL err_trips_decl;
  BOOL err_trip_def;
  BOOL err_rake_journey_asgnmnts_decl;
  BOOL err_rj_asgn;
} ERR_STAT;
extern ERR_STAT err_stat;

typedef struct src_pos {
  int row;
  int col;
} SRC_POS, *SRC_POS_PTR;

typedef struct attr_date {
  KIND kind;
  int year;
  int month;
  int day;
  SRC_POS pos;
} ATTR_DATE, *ATTR_DATE_PTR;
typedef struct attr_time {
  KIND kind;
  int hour;
  int min;
  int sec;
  SRC_POS pos;
} ATTR_TIME, *ATTR_TIME_PTR;

typedef struct attr_sp_pair {
  KIND kind;
  struct {
    char sp_id[MAX_SPNAME_LEN];
    SRC_POS pos;
  } org;
  struct {
    char sp_id[MAX_SPNAME_LEN];
    SRC_POS pos;
  } dst;
} ATTR_SP_PAIR, *ATTR_SP_PAIR_PTR;

typedef struct attr_st_pltb {
  KIND kind;
  struct {
    char name[MAX_STNAME_LEN];
    SRC_POS pos;
  } st;
  struct {
    char id[MAX_PLTB_NAMELEN];
    SRC_POS pos;
  } pltb;
} ATTR_ST_PLTB, *ATTR_ST_PLTB_PTR;
typedef struct attr_st_pltb_pair {
  KIND kind;
  ATTR_ST_PLTB st_pltb_org;
  ATTR_ST_PLTB st_pltb_dst;
} ATTR_ST_PLTB_PAIR, *ATTR_ST_PLTB_PAIR_PTR;

typedef struct attr_route {
  KIND kind;
  char name[MAX_ROUTENAME_LEN];
  SRC_POS pos;
} ATTR_ROUTE, *ATTR_ROUTE_PTR;
typedef struct attr_routes {
  KIND kind;
  int nroutes;
  ATTR_ROUTE route_prof[MAX_TRIP_ROUTES];
} ATTR_ROUTES, *ATTR_ROUTES_PTR;

typedef int DWELL_TIME;
typedef struct attr_trip {
  KIND kind;
  ATTR_ST_PLTB_PAIR attr_st_pltb_orgdst;
  ATTR_SP_PAIR attr_sp_orgdst;
  ATTR_ROUTES attr_route_ctrl;
  // belows are extends.
  struct {
    ARS_SP_COND stop_skip;
    DWELL_TIME dwell_time;
    SRC_POS pos;
  } sp_cond;
  struct {
    struct {
      ATTR_TIME arr_time;
      SRC_POS pos;
    } arriv;
    struct {
      ATTR_TIME dep_time;
      SRC_POS pos;
    } dept;
  } arrdep_time;
  struct {
    PERFREG_LEVEL perfreg_cmd;
    SRC_POS pos;
  } perf_regime;
  struct {
    BOOL stat;
    SRC_POS pos;
  } revenue;
  struct {
    int cid;
    SRC_POS pos;
  } crew_id;
} ATTR_TRIP, *ATTR_TRIP_PTR;
typedef struct attr_trips {
  KIND kind;
  int ntrips;
  ATTR_TRIP trip_prof[MAX_TRIPS];
} ATTR_TRIPS, *ATTR_TRIPS_PTR;

typedef int JOURNEY_ID;
typedef struct attr_journey {
  KIND kind;
  struct {
    JOURNEY_ID jid;
    SRC_POS pos;
  } journey_id;
  ATTR_TRIPS trips;
} ATTR_JOURNEY, *ATTR_JOURNEY_PTR;

typedef struct attr_journeys {
  KIND kind;
  int njourneys;
  ATTR_JOURNEY journey_prof[MAX_JOURNEYS + 1];
} ATTR_JOURNEYS, *ATTR_JOURNEYS_PTR;

typedef int RAKE_ID;
typedef struct attr_rj_asgn {
  KIND kind;
  struct {
    JOURNEY_ID jid;
    SRC_POS pos;
  } journey_id;
  struct {
    RAKE_ID rid;
    SRC_POS pos;
  } rake_id;
} ATTR_RJ_ASGN, *ATTR_RJ_ASGN_PTR;

typedef struct attr_rj_asgns {
  KIND kind;
  int nasgns;
  ATTR_RJ_ASGN rj_asgn[MAX_RJ_ASGNMENTS];
} ATTR_RJ_ASGNS, *ATTR_RJ_ASGNS_PTR;

typedef struct attr_timetable {
  ATTR_TRIPS trips_regtbl;
  ATTR_RJ_ASGNS rj_asgn_regtbl;
  ATTR_JOURNEYS journeys_regtbl;
} ATTR_TIMETABLE, *ATTR_TIMETABLE_PTR;
extern ATTR_TIMETABLE timetable_symtbl;

#include "ttcreat_def.h"

extern BOOL eq_st_pltb ( ATTR_ST_PLTB_PTR p1, ATTR_ST_PLTB_PTR p2 );
extern BOOL eq_st_pltb_pair ( ATTR_ST_PLTB_PAIR_PTR pp1, ATTR_ST_PLTB_PAIR_PTR pp2 );

extern ATTR_TRIP_PTR reg_trip_def ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip );
extern ATTR_RJ_ASGN_PTR reg_rjasgn ( ATTR_RJ_ASGNS_PTR preg_tbl, ATTR_RJ_ASGN_PTR pprev_asgn, ATTR_RJ_ASGN_PTR pasgn );
extern ATTR_TRIP_PTR reg_trip_journey ( ATTR_JOURNEYS_PTR preg_tbl, JOURNEY_ID jid, SRC_POS_PTR ppos, ATTR_TRIP_PTR ptrip );

extern void emit_ars_schcmds( void );

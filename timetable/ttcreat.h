#include "../generic.h"

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_ROUTENAME_LEN 16
#define MAX_SPNAME_LEN 8
#define MAX_TRIP_ROUTES 8
#define MAX_TRIPS 256
#define MAX_RJ_ASGNMENTS 64

typedef enum kind {
  ST_PLTB,
  ST_PLTB_PAIR,
  SP,
  SP_PAIR,
  ROUTE,
  ROUTES,
  TRIP,
  TRIPS,
  RJ_ASGN,
  RJ_ASGNS
} KIND;

typedef struct attr_sp_pair {
  KIND kind;
  char sp_org[MAX_SPNAME_LEN];
  char sp_dst[MAX_SPNAME_LEN];
} ATTR_SP_PAIR, *ATTR_SP_PAIR_PTR;

typedef struct attr_st_pltb {
  KIND kind;
  char st_name[MAX_STNAME_LEN];
  char pltb_name[MAX_PLTB_NAMELEN];
} ATTR_ST_PLTB, *ATTR_ST_PLTB_PTR;
typedef struct attr_st_pltb_pair {
  KIND kind;
  ATTR_ST_PLTB st_pltb_org;
  ATTR_ST_PLTB st_pltb_dst;
} ATTR_ST_PLTB_PAIR, *ATTR_ST_PLTB_PAIR_PTR;

typedef struct attr_route {
  KIND kind;
  char name[MAX_ROUTENAME_LEN];
} ATTR_ROUTE, *ATTR_ROUTE_PTR;
typedef struct attr_routes {
  KIND kind;
  int nroutes;
  ATTR_ROUTE route_prof[MAX_TRIP_ROUTES];
} ATTR_ROUTES, *ATTR_ROUTES_PTR;

typedef struct attr_trip {
  KIND kind;
  ATTR_ST_PLTB_PAIR attr_st_pltb_orgdst;
  ATTR_SP_PAIR attr_sp_orgdst;
  ATTR_ROUTES attr_route_ctrl;
} ATTR_TRIP, *ATTR_TRIP_PTR;
typedef struct attr_trips {
  KIND kind;
  int ntrips;
  ATTR_TRIP trip_prof[MAX_TRIPS];
} ATTR_TRIPS, *ATTR_TRIPS_PTR;

typedef int JOURNEY_ID;
typedef int RAKE_ID;
typedef struct attr_rj_asgn {
  KIND kind;
  JOURNEY_ID jid;
  RAKE_ID rid;
} ATTR_RJ_ASGN, *ATTR_RJ_ASGN_PTR;
typedef struct attr_rj_asgns {
  KIND kind;
  int nasgns;
  ATTR_RJ_ASGN rj_asgn[MAX_RJ_ASGNMENTS];
} ATTR_RJ_ASGNS, *ATTR_RJ_ASGNS_PTR;

typedef struct attr_timetable {
  ATTR_TRIPS trips_regtbl;
  ATTR_RJ_ASGNS rj_asgn_regtbl;
} ATTR_TIMETABLE, *ATTR_TIMETABLE_PTR;

extern BOOL eq_st_pltb ( ATTR_ST_PLTB_PTR p1, ATTR_ST_PLTB_PTR p2 );
extern BOOL eq_st_pltb_pair ( ATTR_ST_PLTB_PAIR_PTR pp1, ATTR_ST_PLTB_PAIR_PTR pp2 );

extern ATTR_TRIP_PTR reg_trip ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip );
extern ATTR_RJ_ASGN_PTR reg_rjasgn ( ATTR_RJ_ASGNS_PTR preg_tbl, ATTR_RJ_ASGN_PTR pprev_asgn, ATTR_RJ_ASGN_PTR pasgn );

extern void emit_ars_schcmds( void );

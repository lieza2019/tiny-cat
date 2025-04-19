#include <stdio.h>
#include <string.h>
#include "../generic.h"
#include "../cbtc.h"
#include "../interlock.h"

#define DEFAULT_DWELL_TIME 17 // in sec.
#define DEFAULT_PERFLEVEL PERFREG_NORMAL
#define DEFAULT_REVENUE FALSE
#define DEFAULT_CREWID -1

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_ROUTENAME_LEN 16
#define MAX_SPNAME_LEN 8
#define MAX_TRIP_ROUTES 8
#define MAX_RJ_ASGNMENTS 64
#define MAX_JOURNEYS 256

#define MAX_TRIPS_DECL 256
#define MAX_JOURNEY_TRIPS 256
#if MAX_TRIPS_DECL > MAX_JOURNEY_TRIPS
#error MAX_JOURNEY_TRIPS must be greater than/equal to MAX_TRIPS_DECL in ttcreat.h.
#endif

#include "ttcreat_def.h"

typedef struct err_stat {
  struct {
    BOOL err_trip_journey;
    BOOL err_routes;
    BOOL err_trips_decl;
    BOOL err_trip_def;
    BOOL err_rake_journey_asgnmnts_decl;
    BOOL err_rj_asgn;
  } par;
  struct {
    struct {
      BOOL route_redef;
      BOOL route_unknown;
    } trips;
  } sem;
} ERR_STAT;
extern ERR_STAT err_stat;

typedef struct rake_journey_asgn {
  int rake_id;
  JOURNEY_ID jid;
} RAKE_JOURNEY_ASGN, *RAKE_JOURNEY_ASGN_PTR;

typedef struct st_pltb_pair  {
  ST_ID st;
  PLTB_ID pltb;
} ST_PLTB_PAIR, *ST_PLTB_PAIR_PTR;
typedef struct st_pltb_orgdst {
  ST_PLTB_PAIR org;
  ST_PLTB_PAIR dst;
} ST_PLTB_ORGDST, *ST_PLTB_ORGDST_PTR;

typedef struct sp_orgdst_pair {
  STOPPING_POINT_CODE sp_org;
  STOPPING_POINT_CODE sp_dst;
} SP_ORGDST_PAIR, *SP_ORGDST_PAIR_PTR;

typedef struct routes_assoc {
  IL_SYM id;
  ROUTE_C_PTR pprof;
} ROUTE_ASSOC, *ROUTE_ASSOC_PTR;
typedef struct trip_desc {
  ST_PLTB_ORGDST st_pltb_orgdst;
  SP_ORGDST_PAIR sp_orgdst;
  int num_routes;
  ROUTE_ASSOC routes[MAX_TRIP_ROUTES];
} TRIP_DESC, *TRIP_DESC_PTR;

typedef struct journey_trip {
  ST_PLTB_ORGDST st_pltb_orgdst;
  int dwell_time;
  struct {
    TINY_TIME_DESC time_arr;
    TINY_TIME_DESC time_dep;
  } time_arrdep;
  PERFREG_LEVEL perfreg;
  BOOL is_revenue;
  CREW_ID crew_id;
} JOURNEY_TRIP, JOURNEY_TRIP_PTR;
typedef struct journey_desc {
  JOURNEY_TRIP trips[MAX_JOURNEY_TRIPS];
  JOURNEY_ID jid;
  int num_trips; 
} JOURNEY_DESC, *JOURNEY_DESC_PTR;

typedef struct timetable_dataset {
  struct {
    TRIP_DESC trips[MAX_TRIPS_DECL];
    int num_trips;    
  } trips_decl;
  RAKE_JOURNEY_ASGN rjasgns[MAX_RJ_ASGNMENTS];
  struct {
    int num_journeys;
    JOURNEY_DESC journeys[MAX_JOURNEYS];
  } j;
} TIMETABLE_DATASET, *TIMETABLE_DATASET_PTR;
extern TIMETABLE_DATASET timetbl_dataset;

#include "ttcreat_par.h"

extern void ttc_print_trips ( TRIP_DESC trips[], int ntrips );
extern int ttcreat ( void );

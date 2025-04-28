#include <stdio.h>
#include <string.h>
#include "../generic.h"
#include "../cbtc.h"
#include "../interlock.h"

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_ROUTENAME_LEN 16
#define MAX_SPNAME_LEN 8
#define MAX_TRIP_ROUTES 8
#define MAX_JR_ASGNMENTS 64
#define MAX_JOURNEYS 256

#define MAX_TRIPS_DECL 256
#define MAX_JOURNEY_TRIPS 256
#if MAX_TRIPS_DECL > MAX_JOURNEY_TRIPS
#error MAX_JOURNEY_TRIPS must be greater than/equal to MAX_TRIPS_DECL in ttcreat.h.
#endif

#define DEFAULT_DWELL_TIME 17 // in sec.
#define DEFAULT_PERFLEVEL PERFREG_NORMAL
#define DEFAULT_REVENUE FALSE
#define DEFAULT_CREWID CREW_NO_ID

#define JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE 1
#define JOURNEY_DEFAULT_ARRTIME_HOUR 5
#define JOURNEY_DEFAULT_ARRTIME_MINUTE 0
#define JOURNEY_DEFAULT_ARRTIME_SECOND 0

#include "ttcreat_def.h"

typedef struct err_stat {
  struct {
    BOOL err_trip_journey;
    BOOL err_routes;
    BOOL err_trips_decl;
    BOOL err_trip_def;
    BOOL err_journey_rake_asgnmnts_decl;
    BOOL err_jr_asgn;
  } par;
  struct {
    BOOL route_redef;
    BOOL unknown_route;
    BOOL unknown_trip;
    BOOL contiguless_trips;
    BOOL inconsistent_arrtime_ovrdn;
    BOOL inconsistent_deptime_ovrdn;
    BOOL invalid_crewid;
  } sem;
} ERR_STAT;
extern ERR_STAT err_stat;
#define TTC_ERRSTAT_PAR( es ) (			\
  (es).par.err_trip_journey ||			\
  (es).par.err_routes ||			\
  (es).par.err_trips_decl ||			\
  (es).par.err_trip_def ||			\
  (es).par.err_journey_rake_asgnmnts_decl ||	\
  (es).par.err_jr_asgn				\
)
#define TTC_ERRSTAT_SEM( es ) (			\
  (es).sem.route_redef ||			\
  (es).sem.unknown_route ||			\
  (es).sem.unknown_trip ||			\
  (es).sem.contiguless_trips ||			\
  (es).sem.inconsistent_arrtime_ovrdn ||	\
  (es).sem.inconsistent_deptime_ovrdn ||	\
  (es).sem.invalid_crewid			\
)

typedef struct journey_rake_asgn {
  int rake_id;
  JOURNEY_ID jid;
} JOURNEY_RAKE_ASGN, *JOURNEY_RAKE_ASGN_PTR;

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

typedef int RUNNING_TIME;
typedef struct trip_desc {
  ST_PLTB_ORGDST st_pltb_orgdst;
  SP_ORGDST_PAIR sp_orgdst;
  RUNNING_TIME running_time;
  int num_routes;
  ROUTE_ASSOC routes[MAX_TRIP_ROUTES];
} TRIP_DESC, *TRIP_DESC_PTR;

typedef enum ars_sp_cond {
  DWELL = 1,
  SKIP
} ARS_SP_COND;
extern char *cnv2str_sp_cond ( char *pstr, ARS_SP_COND sp_cond, const int buflen );

typedef struct time_arrdep {
  TINY_TIME_DESC time_arr;
  TINY_TIME_DESC time_dep;
} TIME_ARRDEP, *TIME_ARRDEP_PTR;

typedef int DWELL_TIME;
typedef struct journey_trip {
  ST_PLTB_ORGDST st_pltb_orgdst;
  TRIP_DESC_PTR ptrip_prof;
  struct {
    ARS_SP_COND stop_skip;
    DWELL_TIME dwell_time;
  } sp_cond;
  TIME_ARRDEP time_arrdep;
  PERFREG_LEVEL perfreg;
  BOOL is_revenue;
  CREW_ID crew_id;
} JOURNEY_TRIP, *JOURNEY_TRIP_PTR;
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
  JOURNEY_RAKE_ASGN jrasgns[MAX_JR_ASGNMENTS];
  struct {
    int num_journeys;
    JOURNEY_DESC journeys[MAX_JOURNEYS];
  } j;
} TIMETABLE_DATASET, *TIMETABLE_DATASET_PTR;
extern TIMETABLE_DATASET timetbl_dataset;

#include "ttcreat_par.h"

extern void ttc_print_trips ( TRIP_DESC trips[], int ntrips );
extern void ttc_print_journeys( JOURNEY_DESC journeys[], int njourneys );

extern TRIP_DESC_PTR lkup_trip ( ST_PLTB_PAIR_PTR porg, ST_PLTB_PAIR_PTR pdst );

extern int ttcreat ( void );

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
#define MAX_TRIPS 256
#define MAX_RJ_ASGNMENTS 64
#define MAX_JOURNEYS 256

#include "ttcreat_def.h"

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
} SP_ORGDST_PAIR;

typedef struct trip_def {
  ST_PLTB_ORGDST st_pltb_orgdst;
  SP_ORGDST_PAIR sp_orgdst;
  struct {
    IL_SYM id;
    ROUTE_PTR pprof;
  } routes[MAX_TRIP_ROUTES];
} TRIP_DESC;

typedef struct {
  ST_PLTB_ORGDST st_pltb_orgdst;
  int dwell_time;
  struct {
    TINY_TIME_DESC time_arr;
    TINY_TIME_DESC time_dep;
  } time_arrdep;
  PERFREG_LEVEL perfreg;
  BOOL is_revenue;
  CREW_ID crew_id;
} JOURNEY_TRIP;

typedef struct t {
  TRIP_DESC trips_decl[MAX_TRIPS];
  RAKE_JOURNEY_ASGN rjasgns[MAX_RJ_ASGNMENTS];
  JOURNEY_TRIP journey_trips[MAX_TRIPS];
} T;
#include "ttcreat_par.h"

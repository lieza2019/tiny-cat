typedef enum par_kind {
  PAR_UNKNOWN = 0,
  PAR_DATE_SPEC = 1,
  PAR_TIME_SPEC,
  PAR_ST_PLTB,
  PAR_ST_PLTB_ORGDST,
  PAR_SP,
  PAR_SP_ASGN,
  PAR_SP_ASGNS,
  PAR_SP_PAIR,
  PAR_ROUTE,
  PAR_ROUTES,
  PAR_TRIP,
  PAR_TRIPS,
  PAR_PERF_REGIME,
  PAR_REVENUE_STAT,
  PAR_CREWID,
  PAR_JR_ASGN,
  PAR_JR_ASGNS,
  PAR_JOURNEY,
  PAR_JOURNEYS,
  END_OF_PAR_KIND
} PAR_KIND;
extern char *cnv2str_kind ( char *pstr, PAR_KIND kind, const int buflen );

typedef struct src_pos {
  int row;
  int col;
} SRC_POS, *SRC_POS_PTR;

typedef struct {
  PAR_KIND kind;
  TINY_TIME_DESC t;
  SRC_POS pos;
} ATTR_TIME, *ATTR_TIME_PTR;

typedef struct attr_st_pltb {
  PAR_KIND kind;
  struct {
    char name[MAX_STNAME_LEN];
    SRC_POS pos;
  } st;
  struct {
    char id[MAX_PLTB_NAMELEN];
    SRC_POS pos;
  } pltb;
} ATTR_ST_PLTB, *ATTR_ST_PLTB_PTR;

typedef struct attr_sp_asgn {
  PAR_KIND kind;
  ATTR_ST_PLTB st_pltb;
  struct {
    PAR_KIND kind;
    char sp_id[MAX_SPNAME_LEN];
    SRC_POS pos;
  } sp;
  SRC_POS pos;
} ATTR_SP_ASGN, *ATTR_SP_ASGN_PTR;
typedef struct attr_sp_asgns {
  PAR_KIND kind;
  int nasgns;
  ATTR_SP_ASGN pltb_sp_asgns[MAX_SP_ASGNMENTS];
} ATTR_SP_ASGNS, *ATTR_SP_ASGNS_PTR;

typedef struct attr_sp_pair {
  PAR_KIND kind;
  STOPPING_POINT_CODE sp_org;
  STOPPING_POINT_CODE sp_dst;
} ATTR_SP_PAIR, *ATTR_SP_PAIR_PTR;

typedef struct attr_st_pltb_pair {
  PAR_KIND kind;
  ATTR_ST_PLTB st_pltb_org;
  ATTR_ST_PLTB st_pltb_dst;
} ATTR_ST_PLTB_ORGDST, *ATTR_ST_PLTB_ORGDST_PTR;

typedef struct attr_route {
  PAR_KIND kind;
  char name[MAX_ROUTENAME_LEN];
  SRC_POS pos;
} ATTR_ROUTE, *ATTR_ROUTE_PTR;
typedef struct attr_routes {
  PAR_KIND kind;
  int nroutes;
  ATTR_ROUTE route_prof[MAX_TRIP_ROUTES];
} ATTR_ROUTES, *ATTR_ROUTES_PTR;

typedef struct attr_trip {
  PAR_KIND kind;
  BOOL deadend;
  ATTR_ST_PLTB_ORGDST attr_st_pltb_orgdst;
  ATTR_SP_PAIR attr_sp_orgdst;
  RUNNING_TIME running_time;
  ATTR_ROUTES attr_route_ctrl;
  // belows are extends.
  struct {
    ARS_SP_COND stop_skip;
    TIME_DIFF dwell_time;
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
  PAR_KIND kind;
  int ntrips;
  ATTR_TRIP trip_prof[MAX_JOURNEY_TRIPS];
} ATTR_TRIPS, *ATTR_TRIPS_PTR;

typedef int JOURNEY_ID;
typedef struct attr_journey {
  PAR_KIND kind;
  struct {
    JOURNEY_ID jid;
    SRC_POS pos;
  } journey_id;
  ATTR_TRIPS trips;
} ATTR_JOURNEY, *ATTR_JOURNEY_PTR;

typedef struct attr_journeys {
  PAR_KIND kind;
  int njourneys;
  ATTR_JOURNEY journey_prof[MAX_JOURNEYS + 1];
} ATTR_JOURNEYS, *ATTR_JOURNEYS_PTR;

typedef int RAKE_ID;
typedef struct attr_jr_asgn {
  PAR_KIND kind;
  struct {
    JOURNEY_ID jid;
    SRC_POS pos;
  } journey_id;
  struct {
    RAKE_ID rid;
    SRC_POS pos;
  } rake_id;
} ATTR_JR_ASGN, *ATTR_JR_ASGN_PTR;
typedef const struct attr_jr_asgn *ATTR_JR_ASGN_C_PTR;

typedef struct attr_jr_asgns {
  PAR_KIND kind;
  int nasgns;
  ATTR_JR_ASGN jr_asgn[MAX_JR_ASGNMENTS];
} ATTR_JR_ASGNS, *ATTR_JR_ASGNS_PTR;

typedef struct attr_timetable {
  ATTR_SP_ASGNS sp_asgn_regtbl;
  ATTR_TRIPS trips_regtbl;
  ATTR_JR_ASGNS jr_asgn_regtbl;
  ATTR_JOURNEYS journeys_regtbl;
} ATTR_TIMETABLE, *ATTR_TIMETABLE_PTR;
extern ATTR_TIMETABLE_PTR timetable_symtbl;

extern BOOL eq_st_pltb ( ATTR_ST_PLTB_PTR p1, ATTR_ST_PLTB_PTR p2 );
extern BOOL eq_st_pltb_pair ( ATTR_ST_PLTB_ORGDST_PTR pp1, ATTR_ST_PLTB_ORGDST_PTR pp2 );

extern ATTR_TRIP_PTR reg_trip_def ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip );
extern ATTR_SP_ASGN_PTR reg_spasgn ( ATTR_SP_ASGNS_PTR preg_tbl, ATTR_SP_ASGN_PTR pprev_asgn, ATTR_SP_ASGN_PTR pasgn );
extern ATTR_JR_ASGN_PTR reg_jrasgn ( ATTR_JR_ASGNS_PTR preg_tbl, ATTR_JR_ASGN_PTR pprev_asgn, ATTR_JR_ASGN_PTR pasgn );
extern ATTR_TRIP_PTR reg_trip_journey ( ATTR_JOURNEYS_PTR preg_tbl, JOURNEY_ID jid, SRC_POS_PTR ppos, ATTR_TRIP_PTR ptrip );

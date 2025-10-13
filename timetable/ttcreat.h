#ifndef TTCREAT_H
#define TTCREAT_H

#include <stdio.h>
#include <string.h>
#include "../generic.h"
#include "../cbtc.h"
#include "../interlock.h"
#include "../timetable.h"

#define TTC_CMDOPT_MAXLEN 32
#define TTC_SOURCEFILE_EXT ".ttb"

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_SPNAME_LEN 8
#define MAX_SP_ASGNMENTS 256
#define MAX_ROUTENAME_LEN 16
#define MAX_TRIP_ROUTES 8
#define MAX_JOURNEYS 256
#define MAX_JR_ASGNMENTS 64

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

typedef struct ttc_ctrl {
  BOOL dump_par_symtbl;
  BOOL dump_ttc_symtbl;
} TTC_CTRL, *TTC_CTRL_PTR;
extern TTC_CTRL ttc_ctrl_flgs;

typedef struct err_stat {
  struct {
    BOOL err_trip_journey;
    BOOL err_routes;
    BOOL err_trips_decl;
    BOOL err_sp_def;
    BOOL err_trip_def;
    BOOL err_journey_rake_asgnmnts;
    BOOL err_jr_asgn;
  } par;
  struct {
    BOOL sp_undefined;
    BOOL route_redef;
    BOOL unknown_route;
    BOOL unknown_trip;
    BOOL contiguless_trips;
    BOOL inconsistent_arrtime_overdn;
    BOOL inconsistent_deptime_overdn;
    BOOL invalid_crewid;
  } sem;
} ERR_STAT;
extern ERR_STAT err_stat;
#define TTC_ERRSTAT_PAR( es ) (			\
  (es).par.err_trip_journey ||			\
  (es).par.err_routes ||			\
  (es).par.err_trips_decl ||			\
  (es).par.err_trip_def ||			\
  (es).par.err_journey_rake_asgnmnts ||		\
  (es).par.err_jr_asgn				\
)
#define TTC_ERRSTAT_SEM( es ) (			\
  (es).sem.route_redef ||			\
  (es).sem.unknown_route ||			\
  (es).sem.unknown_trip ||			\
  (es).sem.contiguless_trips ||			\
  (es).sem.inconsistent_arrtime_overdn ||	\
  (es).sem.inconsistent_deptime_overdn ||	\
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

typedef struct sp_asgn {
  ST_PLTB_PAIR st_pltb;
  STOPPING_POINT_CODE sp;
} SP_ASGN, *SP_ASGN_PTR;

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

typedef struct journey_trip {
  ST_PLTB_ORGDST st_pltb_orgdst;
  BOOL deadend;
  TRIP_DESC_PTR ptrip_prof;
  struct {
    ARS_SP_COND stop_skip;
    TIME_DIFF dwell_time;
  } sp_cond;
  TIME_ARRDEP time_arrdep;
  PERFREG_LEVEL perfreg;
  BOOL is_revenue;
  CREW_ID crew_id;
  struct {
    SCHEDULED_COMMAND_PTR top;
    SCHEDULED_COMMAND_PTR last;
  } pschcmds_trip;
} JOURNEY_TRIP, *JOURNEY_TRIP_PTR;

typedef struct journey_desc {
  JOURNEY_ID jid;
  BOOL valid;
  TINY_TIME_DESC start_time;
  int num_trips;
  JOURNEY_TRIP trips[MAX_JOURNEY_TRIPS];  
  SCHEDULED_COMMAND_PTR pschcmds_journey;
} JOURNEY_DESC, *JOURNEY_DESC_PTR;

typedef struct timetable_dataset {
  struct {
    int num_asgns;
    SP_ASGN spasgns[MAX_SP_ASGNMENTS];    
  } sp_asgns;
  struct {
    TRIP_DESC trips[MAX_TRIPS_DECL];
    int num_trips;
  } trips_decl;
  struct {    
    int num_asgns;
    JOURNEY_RAKE_ASGN jrasgns[MAX_JR_ASGNMENTS];
  } jr_asgns;
  struct {
    int num_journeys;
    JOURNEY_DESC journeys[MAX_JOURNEYS];
  } j;
} TIMETABLE_DATASET, *TIMETABLE_DATASET_PTR;
extern TIMETABLE_DATASET_PTR timetbl_dataset;

#include "ttc_par.h"

static const int nspc_indent = 2;
#define TTC_DIAG_INDENT( n ) {int i; for(i = 0; i < (n); i++){ int b; for(b = 0; b < nspc_indent; b++ ) printf(" "); }}

extern void print_time_desc ( TINY_TIME_DESC_PTR ptd );
extern void ttc_print_trips ( TRIP_DESC trips[], int ntrips );
extern void ttc_print_journeys( JOURNEY_DESC journeys[], int njourneys );
extern int load_online_timetbl ( void );

extern void ttc_print_schcmds ( SCHEDULED_COMMAND_PTR pschcmds, const int nindents );
extern void emit_scheduled_cmds ( void );

extern STOPPING_POINT_CODE lkup_spcode ( ST_PLTB_PAIR_PTR pst_pl );
extern TRIP_DESC_PTR lkup_trip ( ST_PLTB_PAIR_PTR porg, ST_PLTB_PAIR_PTR pdst );

extern void cons_spasgn ( ATTR_SP_ASGNS_PTR pspasgns );
extern void cons_jrasgn ( ATTR_JR_ASGNS_PTR pjrasgns );
extern void cons_trips ( ATTR_TRIPS_PTR ptrips );
extern void cons_journeys ( ATTR_JOURNEYS_PTR pjourneys );

extern int ttcreat ( FILE *fp_src, BOOL dump_par_symtbl, BOOL dump_ttc_symtbl );
extern int cons_online_timetbl ( void );
#endif // TTCREAT_H

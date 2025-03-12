%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ttcreat.h"

#define YYDEBUG 1
#define PRINT_STRBUF_MAXLEN 256

static BOOL dirty;
static struct {
  BOOL err_route;
  BOOL err_trips_decl;
  BOOL err_sp_orgdst_pair;
  BOOL err_st_and_pltb;
  BOOL err_rake_journey_asgnmnts_decl;
  BOOL err_rj_asgn;
} err_ctrl;

static void print_time ( ATTR_TIME_PTR ptime ) {
  assert( ptime );
  printf( "(hour, min, sec): (%02d, %02d, %02d)\n", ptime->hour, ptime->min, ptime->sec );
}

static void print_st_pltb ( ATTR_ST_PLTB_PTR pst_pltb ) {
  assert( pst_pltb );
  char st_pltb_strbuf[(MAX_STNAME_LEN + 1 + MAX_PLTB_NAMELEN) + 1] = "";
  
  strncat( st_pltb_strbuf, pst_pltb->st_name, MAX_STNAME_LEN );
  strncat( st_pltb_strbuf, "_", 2 );
  strncat( st_pltb_strbuf, pst_pltb->pltb_name, MAX_PLTB_NAMELEN );
  printf( "%s", st_pltb_strbuf );
}

static void print_sp_pair ( ATTR_SP_PAIR_PTR psps ) {
  assert( psps );
  printf( "(%s, %s)", psps->sp_org, psps->sp_dst );
}

static void print_routes ( ATTR_ROUTES_PTR proutes ) {
  assert( proutes );
  int i = 0;
  printf( "{" );
  while( i < proutes->nroutes ) {
    printf( "%s", proutes->route_prof[i].name );
    i++;
    if( i < proutes->nroutes )
      printf( ", " );
  }
  printf( "}" );
}

static void print_trip ( ATTR_TRIP_PTR ptrip, BOOL ext ) {
  assert( ptrip );
  char buf[PRINT_STRBUF_MAXLEN + 1] = "";
  
  printf( "(%s, ", cnv2str_kind(buf, ptrip->kind, PRINT_STRBUF_MAXLEN) );
  
  printf( "(" );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_org );
  printf( ", " );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_dst );
  printf( "), " ); 
  
  print_sp_pair( &ptrip->attr_sp_orgdst );
  printf( ", " );
  print_routes( &ptrip->attr_route_ctrl );
  
  if( ext ) {
    printf( ", %s, ", cnv2str_sp_cond(buf, ptrip->sp_cond, PRINT_STRBUF_MAXLEN) );
    printf( "(%02d:%02d:%02d,", ptrip->arrdep_time.arr_time.hour, ptrip->arrdep_time.arr_time.min, ptrip->arrdep_time.arr_time.sec );
    printf( " %02d:%02d:%02d)", ptrip->arrdep_time.dep_time.hour, ptrip->arrdep_time.dep_time.min, ptrip->arrdep_time.dep_time.sec );
    printf( ", %d", ptrip->dwell_time );
    
    printf( ", %s", cnv2str_perf_regime( buf, ptrip->perf_regime, PRINT_STRBUF_MAXLEN ) );
    printf( ", %s", (ptrip->revenue ? "revenue" : "nonreve") );

    if( ptrip->crew_id > -1 ) {
      int i;
      strncpy( buf, "crew_", PRINT_STRBUF_MAXLEN );
      i = strnlen( buf, PRINT_STRBUF_MAXLEN );
      sprintf( &buf[i], "%04d", ptrip->crew_id );
    } else {
      assert( ptrip->crew_id < 0 );
      strncpy( buf, "no_crew_id", PRINT_STRBUF_MAXLEN );
    }
    printf( ", %s", buf );
  }
  printf( ")" );
}

static void print_rjasgn ( ATTR_RJ_ASGN_PTR pasgn ) {
  assert( pasgn );
  char buf[PRINT_STRBUF_MAXLEN + 1] = "";
  
  printf( "(%s, ", cnv2str_kind(buf, pasgn->kind, PRINT_STRBUF_MAXLEN) );
  {
    char str[8];
    sprintf( str, "%d", pasgn->rid );
    printf( "(rake_%s, ", str );
    sprintf( str, "%d", pasgn->jid );
    printf( "J%s)", str );
  }
  printf( ")" );  
}

static ATTR_TRIP_PTR raw_journey_trip ( ATTR_TRIP_PTR ptrip ) {
  assert( ptrip );
  ptrip->kind = UNKNOWN;
  
  ptrip->sp_cond = DWELL;
  ptrip->arrdep_time.arr_time.hour = -1;
  ptrip->arrdep_time.arr_time.min = -1;
  ptrip->arrdep_time.arr_time.sec = -1;
  ptrip->arrdep_time.dep_time.hour = -1;
  ptrip->arrdep_time.dep_time.min = -1;
  ptrip->arrdep_time.dep_time.sec = -1;
  ptrip->dwell_time = DEFAULT_DWELL_TIME;
  
  ptrip->perf_regime = DEFAULT_PERFLEVEL;
  ptrip->revenue = DEFAULT_REVENUE;
  
  ptrip->crew_id = DEFAULT_CREWID;
  return ptrip;
}

static JOURNEY_ID jid_w = -1;
ATTR_TIMETABLE timetable_symtbl = {{TRIPS}, {RJ_ASGNS}, {JOURNEYS}};
%}
%union {
  int nat;
  ATTR_DATE attr_date;
  ATTR_TIME attr_time;  
  char st_name[MAX_STNAME_LEN];
  char pltb_name[MAX_PLTB_NAMELEN];
  ATTR_ST_PLTB attr_st_pltb;
  char sp[MAX_SPNAME_LEN];
  ATTR_SP_PAIR attr_sp_pair;
  ATTR_ROUTE attr_route;
  ATTR_ROUTES attr_routes;  
  ATTR_TRIP attr_trip;
  ATTR_TRIPS_PTR pattr_trips;
  JOURNEY_ID journey_id;
  RAKE_ID rake_id;
  ATTR_RJ_ASGN attr_rj_asgn;
  ATTR_RJ_ASGNS_PTR  pattr_rj_asgns;
  PERFREG_LEVEL perf_regime;
  BOOL revenue;
  int crew_id;
  ATTR_TRIP attr_trip_journey;
  ATTR_TRIPS_PTR pattr_trips_journey;
  ATTR_JOURNEY_PTR pattr_journey;
  ATTR_JOURNEYS_PTR pattr_journeys;
  
  ATTR_TIMETABLE_PTR ptimetable_symtbl;
}
/* %token TK_EOF */
%token <attr_date> TK_DATE
%token <attr_time> TK_TIME
%token <nat> TK_NAT
%token <st_name> TK_STNAME
%token <pltb_name> TK_PLTB_NAME
%type <attr_st_pltb> st_and_pltb
%token <sp> TK_SP
%type <attr_sp_pair> sp_orgdst_pair
%token <attr_route> TK_ROUTE
%type <attr_route> route
%type <attr_routes> routes routes0
%type <attr_trip> trip_def
%token TK_KEY_TRIPS
%type <pattr_trips> trips_definition trips_decl
%token TK_JOURNEYS
%token <journey_id> TK_JOURNEY_ID
%type <journey_id> journey_ident
%token <rake_id> TK_RAKE_ID
%token TK_ASGN
%type <attr_rj_asgn> rj_asgn
%token TK_KEY_ASSIGNMENTS
%type <pattr_rj_asgns> rake_journey_asgnmnts rake_journey_asgnmnts_decl
%type <attr_time> time
%token <perf_regime> TK_PERFREG
%token <revenue> TK_REVENUE
%token <crew_id> TK_CREWID
%type <attr_trip_journey> trip_journey dwell_journey arrdep_time_journey perf_journey revenue_journey crewid_journey
%type <pattr_trips_journey> trips_journey
%type <pattr_journey> journey_definition
%type <pattr_journeys> journeys_decl
%type <ptimetable_symtbl> timetable_decl
 /* %start rake_journey_asgnmnts_decl */
%start timetable_decl
%%
timetable_decl : trips_decl rake_journey_asgnmnts_decl journeys_decl {
#if 1 /* ***** for debugging. */
  {
    printf( "trips:\n" );
    {
      const int nspc_indent = 2;
      assert( $1 );
      ATTR_TRIP_PTR p = $1->trip_prof;
      assert( p );
      int i;
      for( i = 0; i < $1->ntrips; i++ ) {
	{int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
	print_trip( &p[i], FALSE );
	printf( "\n" );
      }
    }
    printf( "\n" );
    printf( "assignments:\n" );
    {
      const int nspc_indent = 2;
      assert( $2 );
      ATTR_RJ_ASGN_PTR p = $2->rj_asgn;
      assert( p );      
      int i;
      for( i = 0; i < $2->nasgns; i++ ) {
	{int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
	print_rjasgn( &p[i] );
	printf( "\n" );
      }
    }
    printf( "\n" );
    printf( "journeys:\n" );    
    {
      const int nspc_indent = 2;
      assert( timetable_symtbl.journeys_regtbl.kind == JOURNEYS );
      int i, j;
      for( i = 0, j = 0;i < timetable_symtbl.journeys_regtbl.njourneys; i++, j++ ) {
	while( j < MAX_JOURNEYS ) {
	  ATTR_JOURNEY_PTR p = &timetable_symtbl.journeys_regtbl.journey_prof[j];
	  assert( p );
	  if( p->jid > 0 ) {
	    assert( p->kind == JOURNEY );
	    int k;
	    {int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
	    printf( "J%02d: \n", p->jid );
	    for( k = 0; k < p->trips.ntrips; k++ ) {
	      assert( p->trips.kind == TRIPS );
	      {int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
	      {int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
	      print_trip( &p->trips.trip_prof[k], TRUE );
	      printf( "\n" );
	    }
	    break;
	  }
	  j++;
	}
      }
    }
  }
#endif
  emit_ars_schcmds();
 }
;

/* Journeys_decl : TK_JOURNEYS : journey_definition*
   journey_definition : TK_JOURNEY_ID : trips_journey
   trips_journey : (trips_journey (';' trip_journey)*)?
   e.g.
   journeys:
     J01:
       ( ((JLA,PL1), (KIKJ, PL1)), 37, (11:22:33, 23:59:59), perfslow, revenue, crew_021 );
       ( ((BTGD,PL2), (OKBS, PL2)), 15, (00:13:51, 21:57:13), perffast, revenue, crew_007 );
*/
journeys_decl : TK_JOURNEYS ':' /* empty journies */ {
  assert( jid_w < 0 );
  assert( timetable_symtbl.journeys_regtbl.kind == JOURNEYS );
  assert( timetable_symtbl.journeys_regtbl.njourneys == 0 );
  $$ = &timetable_symtbl.journeys_regtbl;
}
              | journeys_decl journey_definition {
  assert( jid_w > 0 );
  assert( timetable_symtbl.journeys_regtbl.kind == JOURNEYS );
  timetable_symtbl.journeys_regtbl.njourneys++;
  {
    int n = timetable_symtbl.journeys_regtbl.njourneys;
    assert( n > 0 );
    int i;
    for( i = 0; i < MAX_JOURNEYS; i++ ) {
      assert( n >= 0 );
      if( timetable_symtbl.journeys_regtbl.journey_prof[i].jid > 0 ) {
	assert( timetable_symtbl.journeys_regtbl.journey_prof[i].kind == JOURNEY );
	n--;
      }
    }
    assert( n == 0 );
  }
  jid_w = -1;
  $$ = &timetable_symtbl.journeys_regtbl;  
 }
;
journey_definition : journey_ident ':' trips_journey {
  assert( jid_w == $1 );
  assert( timetable_symtbl.journeys_regtbl.journey_prof[jid_w].jid == jid_w );
  assert( &timetable_symtbl.journeys_regtbl.journey_prof[jid_w].trips == $3 );
  timetable_symtbl.journeys_regtbl.journey_prof[jid_w].kind = JOURNEY;
  $$ = &timetable_symtbl.journeys_regtbl.journey_prof[jid_w];
 }
;
journey_ident : TK_JOURNEY_ID {
  assert( jid_w < 0 );
  assert( $1 > 0 );
  jid_w = $1;
  $$ = jid_w;
 }
;
trips_journey : /* empty trips */ {
  assert( jid_w > -1 );
  $$ = &timetable_symtbl.journeys_regtbl.journey_prof[jid_w].trips;
  assert( $$->ntrips == 0 );
}
              | trips_journey trip_journey ';' {
  assert( jid_w > -1 );
  assert( $1 == &timetable_symtbl.journeys_regtbl.journey_prof[jid_w].trips );  
  ATTR_TRIP_PTR pnxt = &$1->trip_prof[$1->ntrips];  
  ATTR_TRIP_PTR preg = NULL;
  $2.kind = TRIP;
  preg = reg_trip_journey( &timetable_symtbl.journeys_regtbl, jid_w, &$2 );
  assert( pnxt == preg );
  $$ = &timetable_symtbl.journeys_regtbl.journey_prof[jid_w].trips;
 }
;

/* Each trip of journey has following attributes,
    Origin St.& PL/TB
    Destination St.& PL/TB
    S.P.(Stopping Point) condition: DWELLing or SKIPping
    Arrival time of this Stopping Point
    Departure time of this Stopping Point
    Dwell time [sec] of this Stopping Point
    Performance regime, of the departure from this Stopping Point
    Is revenue operation, or not.
    Crew ID, of the crew assigned to the departure from this Stopping Point
*/
/* trip_journey : '(' '('st_and_pltb ',' st_and_pltb')' ',' dwell_journey ',' '('arrtime_journey ',' deptime_journey')' ',' perf_journey ',' revenue_journey ',' crewid_journey ')'
   e.g.
    ( ((JLA,PL1), (KIKJ, PL1)), 37, (11:22:33, 23:59:59), perfslow, revenue, crew_021 )
    ( ((BTGD,PL2), (OKBS, PL2)), 15, (00:13:51, 21:57:13), perffast, revenue, crew_007 )
*/
trip_journey : '(' '('st_and_pltb ',' st_and_pltb')' ')' {
  raw_journey_trip( &$$ );
  $$.kind = TRIP;
  $$.attr_st_pltb_orgdst.kind = ST_PLTB_PAIR;
  $$.attr_st_pltb_orgdst.st_pltb_org = $3;
  $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
#if 0 // ***** for debugging.
  {
    print_trip( &$$, TRUE );
  }
#endif
 }
             | '(' '('st_and_pltb ',' st_and_pltb')' ',' dwell_journey {
  $$ = $8;
  $$.kind = TRIP;
  $$.attr_st_pltb_orgdst.kind = ST_PLTB_PAIR;
  $$.attr_st_pltb_orgdst.st_pltb_org = $3;
  $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
#if 0 // ***** for debugging.
  {
    print_trip( &$$, TRUE );
  }
#endif
 }
;
dwell_journey : TK_NAT ')' {
  raw_journey_trip( &$$ );
  if( $1 == 0 ) {
    $$.sp_cond = SKIP;
  } else {
    assert( $1 > 0 );
    $$.sp_cond = DWELL;
  }
  $$.dwell_time = $1;
 }
              | TK_NAT ',' arrdep_time_journey {
  $$ = $3;
  if( $1 == 0 ) {
    $$.sp_cond = SKIP;
  } else {
    assert( $1 > 0 );
    $$.sp_cond = DWELL;
  }
  $$.dwell_time = $1;  
 }
              | arrdep_time_journey { /* omitted. */
  $$ = $1;
  $$.dwell_time = DEFAULT_DWELL_TIME;
 }
;
arrdep_time_journey : '(' ')' ')' { /* both omitted, form1 */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ')' ',' perf_journey {
  $$ = $4;
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' ')' ')' { /* both omitted, form 2. */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' ')' ',' perf_journey {
  $$ = $5;
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | perf_journey { /* both omitted, form 3. */
  $$ = $1;
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
  #endif
 }
                    | '(' time ')' ')' { /* deparure-time omitted, form 1. */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ')' ',' perf_journey {
  $$ = $5;
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ',' ')' ')' { /* deparure-time omitted, form 2. */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ',' ')' ',' perf_journey {
  $$ = $6;
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = -1;
  $$.arrdep_time.dep_time.min = -1;
  $$.arrdep_time.dep_time.sec = -1;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' time ')' ')' { /* arrival-time omitted. */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = $3.hour;
  $$.arrdep_time.dep_time.min = $3.min;
  $$.arrdep_time.dep_time.sec = $3.sec;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' time ')' ',' perf_journey {
  $$ = $6;
  $$.arrdep_time.arr_time.hour = -1;
  $$.arrdep_time.arr_time.min = -1;
  $$.arrdep_time.arr_time.sec = -1;
  $$.arrdep_time.dep_time.hour = $3.hour;
  $$.arrdep_time.dep_time.min = $3.min;
  $$.arrdep_time.dep_time.sec = $3.sec;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
  
 }
                    | '(' time ',' time ')' ')' {
  raw_journey_trip( &$$ );
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = $4.hour;
  $$.arrdep_time.dep_time.min = $4.min;
  $$.arrdep_time.dep_time.sec = $4.sec;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ',' time ')' ',' perf_journey {
  $$ = $7;
  $$.arrdep_time.arr_time.hour = $2.hour;
  $$.arrdep_time.arr_time.min = $2.min;
  $$.arrdep_time.arr_time.sec = $2.sec;
  $$.arrdep_time.dep_time.hour = $4.hour;
  $$.arrdep_time.dep_time.min = $4.min;
  $$.arrdep_time.dep_time.sec = $4.sec;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
;
perf_journey : TK_PERFREG ')' {
  raw_journey_trip( &$$ );
  $$.perf_regime = $1;
 }
             | TK_PERFREG ',' revenue_journey {
  $$ = $3;
  $$.perf_regime = $1;
 }
             | revenue_journey { /* omitted */
  $$ = $1;
  $$.perf_regime = DEFAULT_PERFLEVEL;
 }
;
revenue_journey : TK_REVENUE ')' {
  raw_journey_trip( &$$ );
  $$.revenue = $1;
 }
                | TK_REVENUE ',' crewid_journey {
  $$ = $3;
  $$.revenue = $1;
 }
                | crewid_journey { /* omitted */
  $$ = $1;
  $$.revenue = DEFAULT_REVENUE;
 }
;
crewid_journey : TK_CREWID ')' {
  raw_journey_trip( &$$ );
  $$.crew_id = $1;
 }
               | ')' {
  raw_journey_trip( &$$ );
  $$.crew_id = -1;
 }
;

time : TK_TIME {
  $$ = $1;
 }
;

/* e.g.
   trips:
     (((JLA,PL1), (KIKJ, PL1)), (SP_73, SP_77), {S803B_S831B});
     (((KIKJ,PL1), (OKBS, PL1)), (SP_77, SP_79), {S831B_S821A});
     (((OKBS,PL1), (BTGD, PL1)), (SP_79, SP_81), {S821A_S801A, S801A_S803A});
     (((BTGD,PL1), (BTGD, TB1)), (SP_81, TB_D5), {S803A_S809A});
     (((BTGD,TB1), (BTGD, PL2)), (SP_D5, TB_80), {S806A_S804A});
     (((BTGD,PL2), (OKBS, PL2)), (SP_80, TB_78), {S804A_S822A});
     (((OKBS,PL2), (KIKJ, PL2)), (SP_78, TB_76), {S822A_S832B});
     (((KIKJ,PL2), (JLA, PL1)), (SP_76, TB_73), {S832B_S802B, S802B_S810B});
*/
/*
  causes following shift/reduce conflicts,
  
  ttcreat.y: warning: 1 shift/reduce conflict [-Wconflicts-sr]
  ttcreat.y: warning: shift/reduce conflict on token error [-Wcounterexamples]
  First example: rake_journey_asgnmnts_decl journeys_decl
  Shift derivation
    timetable_decl
    ↳ 1: rake_journey_asgnmnts_decl journeys_decl
  Second example: error journeys_decl
  Reduce derivation
    timetable_decl
    ↳ 1: error journeys_decl
*/
trips_decl : TK_KEY_TRIPS ':' trips_definition {
  assert( $3 );
  assert( $3->kind == TRIPS );
  $$ = $3;
#if 0 /* ***** for debugging. */
  {
    assert( $$ );
    ATTR_TRIP_PTR p = $$->trip_prof;
    int i;
    for( i = 0; i < $$->ntrips; i++ ) {
      print_trip( &p[i], FALSE );
      printf( "\n" );
    }
  }
#endif
 }
/* reduce/reduce confliction arises with the rule of-> TK_KEY_TRIPS error, as below
           | TK_KEY_TRIPS error trips_definition {
  if( !err_ctrl.err_trips_decl ) {
    printf( "FATAL: syntax-error, missing delimiter in trip declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_trips_decl = TRUE;
  }
  yyerrok;
 } */
           | error {
  if( !err_ctrl.err_trips_decl ) {
    printf( "FATAL: syntax-error, no trip declaration section at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_trips_decl = TRUE;
  }
 }
           | TK_KEY_TRIPS error {
  if( !err_ctrl.err_trips_decl ) {
    printf( "FATAL: syntax-error, incomplete trip declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_trips_decl = TRUE;
  }
 }
           | TK_KEY_TRIPS ':' error {
  if( !err_ctrl.err_trips_decl ) {
    printf( "FATAL: syntax-error, ill-formed trip declaration at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_trips_decl = TRUE;
  }
 }
/*
           | error TK_EOF {
  assert( FALSE );
 }
*/
;
trips_definition : /* empty journies */ {
  $$ = &timetable_symtbl.trips_regtbl;
  assert( $$->ntrips == 0 );
}
                 | trips_definition trip_def {

  if( $2.kind == TRIP ) {
    ATTR_TRIP_PTR p = NULL;
    if( timetable_symtbl.trips_regtbl.ntrips == 0 ) {
      p = reg_trip_def( &timetable_symtbl.trips_regtbl, NULL, &$2 );
      if( p != &$2 ) {
	printf( "FATAL: INTERNAL-error, in trip definiton & registration, giving up.\n" );
	exit( 1 );
      }
    } else {
      ATTR_TRIP dropd = {};
      p = reg_trip_def( &timetable_symtbl.trips_regtbl, &dropd, &$2 );
      if( p != &$2 ) {
	assert( p == &dropd );
	assert( p->kind == TRIP );
	printf( "NOTICE: trip attribute has been overridden with.\n" );
      }
    }
  }
  $$ = &timetable_symtbl.trips_regtbl;
 }
;
/* e.g. (((JLA,PL1), (KIKJ, PL1)), (SP_73, SP_77), {S803B_S831B}) */
trip_def : '(' '('st_and_pltb ',' st_and_pltb')' ',' sp_orgdst_pair ',' '{' routes '}' ')' ';' {
  $$.kind = UNKNOWN;
  $$.attr_st_pltb_orgdst.kind = ST_PLTB_PAIR;
  $$.attr_st_pltb_orgdst.st_pltb_org = $3;
  $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
  $$.attr_sp_orgdst = $8;
  $$.attr_route_ctrl = $11;
  if( ($3.kind == ST_PLTB) && ($5.kind == ST_PLTB) && ($8.kind == SP_PAIR) && ($11.kind == ROUTES) )
    $$.kind = TRIP;
#if 0 /* ***** for debugging. */
  printf( "(kind, st_pltb_pair, sp_orgdst_pair, trip_routes): " );
  print_trip( &$$, FALSE );
  printf( "\n" );
#endif
 }
;
st_and_pltb : '(' TK_STNAME ',' TK_PLTB_NAME ')' {
  $$.kind = ST_PLTB;
  strncpy( $$.st_name, $2, MAX_STNAME_LEN );
  strncpy( $$.pltb_name, $4, MAX_PLTB_NAMELEN );  
  /* print_st_pltb( &$$ ); // ***** for debugging. */
 }
| '(' TK_STNAME TK_PLTB_NAME ')' {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, delimiter in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = ST_PLTB;
  strncpy( $$.st_name, $2, MAX_STNAME_LEN );
  strncpy( $$.pltb_name, $3, MAX_PLTB_NAMELEN );
  yyerrok;
  /* print_st_pltb( &$$ ); // ***** for debugging. */
 }
            | error TK_STNAME ',' TK_PLTB_NAME ')' {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, missing opening parenthesis in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
/* shift/reduce confliction arises with the rule of-> '(' error, as below.
            | '(' error ',' TK_PLTB_NAME ')' {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, no origin station name found in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 } */
/*
  shift/reduce confliction arises with the rule of-> '(' TK_STNAME ',' error, as below.
            | '(' TK_STNAME ',' error ')' {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, no dest platform/turnback found in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 } */
            | '(' TK_STNAME ',' TK_PLTB_NAME error {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @5.first_line, @5.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
            | error {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, no org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
            | '(' error {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, ill-formed org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
            | '(' TK_STNAME error {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, incomplete org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
            | '(' TK_STNAME ',' error {
  if( !err_ctrl.err_st_and_pltb ) {
    printf( "FATAL: syntax-error, incomplete org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_ctrl.err_st_and_pltb = TRUE;
  }
  $$.kind = UNKNOWN;
 }
;
sp_orgdst_pair : '(' TK_SP ',' TK_SP ')' {
  $$.kind = SP_PAIR;
  strncpy( $$.sp_org, $2, MAX_SPNAME_LEN );
  strncpy( $$.sp_dst, $4, MAX_SPNAME_LEN );
  /* print_sp_pair( &$$ ); // ***** for debugging. */
 }
               | '(' TK_SP TK_SP ')' {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, delimiter in org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = SP_PAIR;
  strncpy( $$.sp_org, $2, MAX_SPNAME_LEN );
  strncpy( $$.sp_dst, $3, MAX_SPNAME_LEN );
  yyerrok;
 }
               | error TK_SP ',' TK_SP ')' {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, missing opening parenthesis in org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
/*
  shift/reduce confliction arises with the rule of-> '(' error, as below.
               | '(' error ',' TK_SP ')' {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, no origin found in org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
  } */
               | '(' TK_SP ',' error ')' {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, no dest found in org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
               | '(' TK_SP ',' TK_SP error {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @5.first_line, @5.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
| error {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, no org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
               | '(' error {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, ill-formed org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
               | '(' TK_SP error {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, incomplete org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
               | '(' TK_SP ',' error {
  if( !err_ctrl.err_sp_orgdst_pair ) {
    printf( "FATAL: syntax-error, incomplete org & dst stop-point specifier of trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_ctrl.err_sp_orgdst_pair = TRUE;
  }
  $$.kind = UNKNOWN;
 }
;
routes0 : '{' routes '}' { /* its only for debugging. */
  printf( "routes: " );
  print_routes( &$2 );
  printf( "\n" );
 }
;
routes : route {  
  if( $1.kind == ROUTE ) {
    $$.kind = ROUTES;
    $$.nroutes = 1;
    $$.route_prof[0] = $1;
    /* printf( "route: %s\n", $$.route_prof[0].name ); // ***** for debugging. */
  } else {
    $$.kind = UNKNOWN;
    $$.nroutes = 0;
  }
 }
       | routes ',' route {
  assert( $1.nroutes < MAX_TRIP_ROUTES );  
  if( $3.kind == ROUTE ) {    
    int i;    
    for( i = 0; i < $1.nroutes; i++ )
      $$.route_prof[i] = $1.route_prof[i];
    assert( i == $1.nroutes );
    $$.route_prof[i] = $3;
    $$.nroutes = $1.nroutes + 1;
    $$.kind = ROUTES;
  }
 }
;
route : TK_ROUTE {
  assert( $1.kind == ROUTE );
  $$.kind = $1.kind;
  strncpy( $$.name, $1.name, MAX_ROUTENAME_LEN );
  yyerrok;
  /* printf( "(kind, route): (%d, %s)\n", $$.kind, $$.name ); // ***** for debugging. */
 }
      | error {
  if( !err_ctrl.err_route ) {
    printf( "FATAL: syntax-error, illegal controlled route in trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_route = TRUE;
  }
  $$.kind = UNKNOWN;
 }
;

/* rake_journey_asgnmnts_decl := TK_KEY_ASSIGNMENTS ':' rake_journey_asgnmnts
   rake_journey_asgnmnts := (TK_RAKE_ID ":=" TK_JOURNEY_ID ';')*
   e.g.
   assignments:
     rake_801 := J1; rake_802 := J2; rake_803 := J3; rake_804 := J4;
     rake_811 := J11; rake_812 := J12; rake_813 := J13; rake_814 := J14;
*/
/* rake_journey_asgnmnts_decl : TK_KEY_ASSIGNMENTS ':' rake_journey_asgnmnts { */
rake_journey_asgnmnts_decl : TK_KEY_ASSIGNMENTS ':' rake_journey_asgnmnts {
  assert( $3 );
  assert( $3->kind == RJ_ASGNS );
  $$ = $3;
#if 0 /* ***** for debugging. */
  {
    assert( $$ );
    ATTR_RJ_ASGN_PTR p = $$->rj_asgn;
    int i;
    for( i = 0; i < $$->nasgns; i++ ) {
      print_rjasgn( &p[i] );
      printf( "\n" );
    }
  }
#endif
 }
/* reduce/reduce confliction arises with the rule of-> TK_KEY_ASSIGNMENTS error, as below.
                           | TK_KEY_ASSIGNMENTS error rake_journey_asgnmnts {
  if( !err_ctrl.err_rake_journey_asgnmnts_decl ) {
    printf( "FATAL: syntax-error, missing delimiter in rake-journey assignment declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_rake_journey_asgnmnts_decl = TRUE;
  }
  assert( $3 );
  assert( $3->kind == RJ_ASGNS );
  $$ = $3;
  yyerrok;
 } */
                           | error {
  if( !err_ctrl.err_rake_journey_asgnmnts_decl ) {
    printf( "FATAL: syntax-error, no rake-journey assignments declaration section at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_rake_journey_asgnmnts_decl = TRUE;
  }
  $$ = &timetable_symtbl.rj_asgn_regtbl;
  /* yyclearin; */
 }
                           | TK_KEY_ASSIGNMENTS error {
  if( !err_ctrl.err_rake_journey_asgnmnts_decl ) {
    printf( "FATAL: syntax-error, incomplete rake-journey assignment declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_rake_journey_asgnmnts_decl = TRUE;
  }
  $$ = &timetable_symtbl.rj_asgn_regtbl;
  /* yyclearin; */
 }
/* reduce/reduce confliction arises with the rule-> | error, of rj_asgn as below.
                           | TK_KEY_ASSIGNMENTS ':' error {
  if( !err_ctrl.err_rake_journey_asgnmnts_decl ) {
    printf( "FATAL: syntax-error, ill-formed rake-journey assignments declaration at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_rake_journey_asgnmnts_decl = TRUE;
  }
  $$ = &timetable_symtbl.rj_asgn_regtbl;
  // yyclearin;
 } */
;
rake_journey_asgnmnts : /* empty journies */ {
  $$ = &timetable_symtbl.rj_asgn_regtbl;
  assert( $$->kind == RJ_ASGNS );
  assert( $$->nasgns == 0 );
 }
                      | rake_journey_asgnmnts rj_asgn {
  if( $2.kind == RJ_ASGN ) {
    ATTR_RJ_ASGN_PTR p = NULL;
    if( timetable_symtbl.rj_asgn_regtbl.nasgns == 0 ) {    
      p = reg_rjasgn( &timetable_symtbl.rj_asgn_regtbl, NULL, &$2 );    
      if( p != &$2 ) {
	printf( "FATAL: INTERNAL-error, in rake-journey assignment registration, giving up.\n" );
	exit( 1 );
      }
    } else {
      ATTR_RJ_ASGN dropd = {};
      p = reg_rjasgn( &timetable_symtbl.rj_asgn_regtbl, &dropd, &$2 );
      if( p != &$2 ) {
	assert( p == &dropd );
	assert( p->kind == RJ_ASGN );
	printf( "NOTICE: rake-journey assignment has been overridden with.\n" );
      }
    }
  }
  $$ = &timetable_symtbl.rj_asgn_regtbl;
 }
;
rj_asgn : TK_RAKE_ID TK_ASGN TK_JOURNEY_ID ';' {
  $$.kind = RJ_ASGN;
  $$.rid = $1;
  $$.jid = $3;
  /* print_rjasgn( &$$ ); // ***** for debugging. */
 }
        | error {
  if( !err_ctrl.err_rj_asgn ) {
    printf( "FATAL: syntax-error, in rake-journey assignments at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_ctrl.err_rj_asgn = TRUE;
  }
  $$.kind = UNKNOWN;
  /* yyclearin; */
 }
        | TK_RAKE_ID error TK_JOURNEY_ID ';' {
  if( !err_ctrl.err_rj_asgn ) {
    printf( "FATAL: syntax-error, missing operator in rake-journey assignments at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_rj_asgn = TRUE;
  }
  $$.kind = RJ_ASGN;
  $$.rid = $1;
  $$.jid = $3;
  yyerrok;
 }
        | TK_RAKE_ID error {
  if( !err_ctrl.err_rj_asgn ) {
    printf( "FATAL: syntax-error, incomplete rake-journey assignment, at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_ctrl.err_rj_asgn = TRUE;
  }
  $$.kind = UNKNOWN;
  /* yyclearin; */
 }
        | TK_RAKE_ID TK_ASGN error {
  if( !err_ctrl.err_rj_asgn ) {
    printf( "FATAL: syntax-error, ill-formed rake-journey assignments at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_ctrl.err_rj_asgn = TRUE;
  }
  $$.kind = UNKNOWN;
  /* yyclearin; */
 }
;
%%
int yyerror ( const char *s ) {
  return 1;
}

int main ( void ) {
  extern int yyparse( void );
  extern FILE *yyin;
  
  yyin = stdin;
  if( yyparse() ) {
    return 1;
  }
  return 0;
}

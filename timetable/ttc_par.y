%{
  int yylex();
  int yyerror ( const char *s );
%}
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ttcreat.h"

#define YYDEBUG 1
#define PRINT_STRBUF_MAXLEN 256

static struct {
  JOURNEY_ID jid_w;
  SRC_POS pos;
} journey_id_w = {-1};
static BOOL journey_trip_deadend_acc = FALSE;
ATTR_TIMETABLE_PTR timetable_symtbl = NULL;

static void print_time ( ATTR_TIME_PTR ptime ) {
  assert( ptime );
  printf( "(hour, min, sec): (%02d, %02d, %02d)\n", ptime->t.hour, ptime->t.minute, ptime->t.second );
}

static void print_st_pltb ( ATTR_ST_PLTB_PTR pst_pltb ) {
  assert( pst_pltb );
  char st_pltb_strbuf[(MAX_STNAME_LEN + 1 + MAX_PLTB_NAMELEN) + 1] = "";
  
  strncat( st_pltb_strbuf, pst_pltb->st.name, MAX_STNAME_LEN );
  strncat( st_pltb_strbuf, "_", 2 );
  strncat( st_pltb_strbuf, pst_pltb->pltb.id, MAX_PLTB_NAMELEN );
  printf( "%s", st_pltb_strbuf );
}

static void print_sp_pair ( ATTR_SP_PAIR_PTR psps ) {
  assert( psps );
  const char *sp_org = cnv2str_sp_code( psps->sp_org );
  assert( sp_org );
  const char *sp_dst = cnv2str_sp_code( psps->sp_dst );
  assert( sp_dst );
  
  printf( "(%s, %s)", sp_org, sp_dst );
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

static void print_trip ( ATTR_TRIP_PTR ptrip, BOOL is_journey ) {
  assert( ptrip );
  char buf[PRINT_STRBUF_MAXLEN + 1] = "";
  
  printf( "(%s, ", cnv2str_kind(buf, ptrip->kind, PRINT_STRBUF_MAXLEN) );
  printf( "(" );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_org );
  printf( ", " );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_dst );
  printf( "), " ); 
  if( !is_journey ) {
    print_sp_pair( &ptrip->attr_sp_orgdst );
    printf( ", %d, ", ptrip->running_time );
    print_routes( &ptrip->attr_route_ctrl );
  } else {
    printf( "%d", ptrip->sp_cond.dwell_time );
    printf( ", %s, ", cnv2str_sp_cond(buf, ptrip->sp_cond.stop_skip, PRINT_STRBUF_MAXLEN) );
    printf( "(%02d:%02d:%02d,", ptrip->arrdep_time.arriv.arr_time.t.hour, ptrip->arrdep_time.arriv.arr_time.t.minute, ptrip->arrdep_time.arriv.arr_time.t.second);
    printf( " %02d:%02d:%02d)", ptrip->arrdep_time.dept.dep_time.t.hour, ptrip->arrdep_time.dept.dep_time.t.minute, ptrip->arrdep_time.dept.dep_time.t.second );
    buf[PRINT_STRBUF_MAXLEN - 1] = 0;
    {
      const char *str = cnv2str_perfreg_level[ptrip->perf_regime.perfreg_cmd];      
      printf( ", %s", strncpy( buf, (str ? str : ""), (PRINT_STRBUF_MAXLEN - 1) ) );
    }
    printf( ", %s", (ptrip->revenue.stat ? "revenue" : "nonreve") );

    if( ptrip->crew_id.cid > -1 ) {
      int i;
      strncpy( buf, "crew_", PRINT_STRBUF_MAXLEN );
      i = strnlen( buf, PRINT_STRBUF_MAXLEN );
      sprintf( &buf[i], "%04d", ptrip->crew_id.cid );
    } else {
      assert( ptrip->crew_id.cid < 0 );
      strncpy( buf, "no_crew_id", PRINT_STRBUF_MAXLEN );
    }
    printf( ", %s", buf );
  }
  printf( ")" );
}

static void print_spasgns ( ATTR_SP_ASGN_PTR pasgn ) {
  assert( pasgn );
  printf( "(" );
  print_st_pltb( &pasgn->st_pltb );
  printf( ", %s", pasgn->sp.sp_id );
  printf( ")" );
}

static void print_jrasgn ( ATTR_JR_ASGN_PTR pasgn ) {
  assert( pasgn );
  char buf[PRINT_STRBUF_MAXLEN + 1] = "";
  
  printf( "(%s, ", cnv2str_kind(buf, pasgn->kind, PRINT_STRBUF_MAXLEN) );
  {
    char str[8];    
    sprintf( str, "%d", pasgn->journey_id.jid );
    printf( "(J%s, ", str );
    sprintf( str, "%d", pasgn->rake_id.rid );
    printf( "rake_%s)", str );
  }
  printf( ")" );  
}

static void print_journey ( ATTR_JOURNEY_PTR pjourney ) {
  assert( pjourney );
  
  if( pjourney->journey_id.jid > 0 ) {
    assert( pjourney->kind == PAR_JOURNEY );
    int i;
    {int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
    printf( "J%02d: \n", pjourney->journey_id.jid );
    for( i = 0; i < pjourney->trips.ntrips; i++ ) {
      assert( pjourney->trips.kind == PAR_TRIPS );
      TTC_DIAG_INDENT( 1 );
      TTC_DIAG_INDENT( 1 );
      print_trip( &pjourney->trips.trip_prof[i], TRUE );
      printf( "\n" );
    }
  }
}

static void print_par_symtbl ( ATTR_SP_ASGNS_PTR pspasgns, ATTR_TRIPS_PTR ptrips, ATTR_JR_ASGNS_PTR pjrasgns, ATTR_JOURNEYS_PTR pjourneys ) {
  printf( "sp_asgnments:\n" );
  if( pspasgns ) {    
    ATTR_SP_ASGN_PTR p = pspasgns->pltb_sp_asgns;
    assert( p );
    int i;
    for( i = 0; i < pspasgns->nasgns; i++ ) {
      TTC_DIAG_INDENT( 1 );
      print_spasgns( &p[i] );
      printf( "\n" );
    }
  }
  printf( "\n" );
  
  printf( "trips:\n" );
  if( ptrips ) {
    ATTR_TRIP_PTR p = ptrips->trip_prof;
    assert( p );
    int i;
    for( i = 0; i < ptrips->ntrips; i++ ) {
      TTC_DIAG_INDENT( 1 );
      print_trip( &p[i], FALSE );
      printf( "\n" );
    }
  }
  printf( "\n" );
  
  printf( "assignments:\n" );
  if( pjrasgns ) {
    ATTR_JR_ASGN_PTR p = pjrasgns->jr_asgn;
    assert( p );      
    int i;
    for( i = 0; i < pjrasgns->nasgns; i++ ) {
      {int b; for(b = 0; b < nspc_indent; b++ ) printf(" ");}
      print_jrasgn( &p[i] );
      printf( "\n" );
    }
  }
  printf( "\n" );
  
  printf( "journeys:\n" );
  if( pjourneys ) {
    int cnt = 0;
    int i;
    for( i = 0; (i < MAX_JOURNEYS) && (cnt < pjourneys->njourneys); i++ ) {
      ATTR_JOURNEY_PTR pj = &pjourneys->journey_prof[i];
      assert( pj );
      if( pj->journey_id.jid > 0 ) {
	print_journey( pj );
	cnt++;
      }
    }
    assert( cnt == pjourneys->njourneys );
  }
}

static ATTR_TRIP_PTR raw_journey_trip ( ATTR_TRIP_PTR ptrip ) {
  assert( ptrip );
  ptrip->kind = PAR_UNKNOWN;
  ptrip->deadend = FALSE;
  
  ptrip->sp_cond.stop_skip = DWELL;
  ptrip->arrdep_time.arriv.arr_time.t.hour = -1;
  ptrip->arrdep_time.arriv.arr_time.t.minute = -1;
  ptrip->arrdep_time.arriv.arr_time.t.second = -1;
  ptrip->arrdep_time.dept.dep_time.t.hour = -1;
  ptrip->arrdep_time.dept.dep_time.t.minute = -1;
  ptrip->arrdep_time.dept.dep_time.t.second = -1;
  ptrip->sp_cond.dwell_time = DEFAULT_DWELL_TIME;
  
  ptrip->perf_regime.perfreg_cmd = DEFAULT_PERFLEVEL;
  ptrip->revenue.stat = DEFAULT_REVENUE;
  
  ptrip->crew_id.cid = (int)DEFAULT_CREWID;
  return ptrip;
}

static ATTR_TRIP_PTR reg_trip ( ATTR_TRIP_PTR ptrip ) {
  assert( ptrip );
  ATTR_TRIP_PTR r = NULL;
  
  if( ptrip->kind == PAR_TRIP ) {
    ATTR_TRIP_PTR p = NULL;
    ptrip->attr_sp_orgdst.sp_org = SP_NONSENS;
    ptrip->attr_sp_orgdst.sp_dst = SP_NONSENS;
    if( timetable_symtbl->trips_regtbl.ntrips == 0 ) {
      p = reg_trip_def( &timetable_symtbl->trips_regtbl, NULL, ptrip );
      if( p != ptrip ) {
	printf( "INTERNAL-error: on trip definiton & registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
	exit( 1 );
      }
    } else {
      ATTR_TRIP drop = {};
      p = reg_trip_def( &timetable_symtbl->trips_regtbl, &drop, ptrip );
      if( p != ptrip ) {
	if( p == &drop ) {
	  assert( p->kind == PAR_TRIP );
	  printf( "NOTICE: trip attribute has been overridden with.\n" );
	} else {
	  printf( "INTERNAL-error: on trip definiton & registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
	  exit( 1 );
	}
      }
    }
    r = p;
  }
  return r;
}

static ATTR_SP_ASGN_PTR reg_sp_asgn ( ATTR_SP_ASGN_PTR psp_asgn ) {
  assert( psp_asgn );
  assert( psp_asgn->kind == PAR_SP_ASGN );
  ATTR_SP_ASGN_PTR r = NULL;
  
  ATTR_SP_ASGN_PTR p = NULL;
  if( timetable_symtbl->sp_asgn_regtbl.nasgns == 0 ) {
    p = reg_spasgn( &timetable_symtbl->sp_asgn_regtbl, NULL, psp_asgn );
    if( p != psp_asgn ) {
      printf( "INTERNAL-error: on sp/pl & stopping-point assignment registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
      exit( 1 );
    }
  } else {
    ATTR_SP_ASGN drop = {};
    p = reg_spasgn( &timetable_symtbl->sp_asgn_regtbl, &drop, psp_asgn );
    if( p != psp_asgn ) {
      if( p == &drop ) {
	assert( p->kind == PAR_SP_ASGN );
	printf( "NOTICE: st/pl & stopping-point assignment " );
	print_st_pltb( &p->st_pltb );
	printf( " at (LINE, COL) = (%d, %d), has been overridden with (LINE, COL) = (%d, %d).\n", p->pos.row, p->pos.col, psp_asgn->pos.row, psp_asgn->pos.col );
      } else {
	assert( !p );
	printf( "INTERNAL-error: on sp/pl & stopping-point assignment registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
	exit( 1 );
      }
    }
  }
  r = p;
  return r;
}

static ATTR_JR_ASGN_PTR reg_journey_rake_asgn ( ATTR_JR_ASGN_PTR pjr_asgn ) {
  assert( pjr_asgn );
  assert( pjr_asgn->kind == PAR_JR_ASGN );
  ATTR_JR_ASGN_PTR r = NULL;
  
  ATTR_JR_ASGN_PTR p = NULL;
  if( timetable_symtbl->jr_asgn_regtbl.nasgns == 0 ) {
    p = reg_jrasgn( &timetable_symtbl->jr_asgn_regtbl, NULL, pjr_asgn );
    if( p != pjr_asgn ) {
      printf( "INTERNAL-error: on journey-rake assignment registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
      exit( 1 );
    }
  } else {
    ATTR_JR_ASGN drop = {};
    p = reg_jrasgn( &timetable_symtbl->jr_asgn_regtbl, &drop, pjr_asgn );
    if( p != pjr_asgn ) {
      if( p == &drop ) {
	assert( p->kind == PAR_JR_ASGN );
	printf( "NOTICE: journey-rake assignment J%03d at (LINE, COL) = (%d, %d), has been overridden with (LINE, COL) = (%d, %d).\n", p->journey_id.jid,
		p->journey_id.pos.row, p->journey_id.pos.col, pjr_asgn->journey_id.pos.row, pjr_asgn->journey_id.pos.col );
      } else {
	assert( !p );
	printf( "INTERNAL-error: on journey-rake assignment registration detected in %s:%d, giving up.\n", __FILE__, __LINE__  );
	exit( 1 );
      }
    }
  }
  r = p;
  return r;
}
%}
%union {
  int nat;
  ATTR_TIME attr_time;  
  char st_name[MAX_STNAME_LEN];
  char pltb_name[MAX_PLTB_NAMELEN];
  ATTR_ST_PLTB attr_st_pltb;
  char sp[MAX_SPNAME_LEN];
  ATTR_SP_ASGN attr_sp_asgn;
  ATTR_SP_ASGNS_PTR attr_sp_asgns;
  ATTR_ROUTE attr_route;
  ATTR_ROUTES attr_routes;
  ATTR_TRIP attr_trip;
  ATTR_TRIPS_PTR pattr_trips;
  JOURNEY_ID journey_id;
  RAKE_ID rake_id;
  ATTR_JR_ASGN attr_jr_asgn;
  ATTR_JR_ASGNS_PTR pattr_jr_asgns;
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
%token <attr_time> TK_TIME
%token <nat> TK_NAT
%token <st_name> TK_STNAME
%token <pltb_name> TK_PLTB_NAME
%type <attr_st_pltb> st_and_pltb
%token <sp> TK_SP
%token TK_SP_ASGNS
%type <attr_sp_asgn> stpl_sp_asgn
%type <attr_sp_asgns> stpl_sp_asgns sp_asgns_decl
%token <attr_route> TK_ROUTE
%type <attr_route> route
%type <attr_routes> routes
 /* %type <attr_routes> routes0 // for debugging. */
%type <attr_trip> trip_def
%token TK_KEY_TRIPS
%type <pattr_trips> trips_definition trips_defs trips_decl
%token TK_JOURNEYS
%token <journey_id> TK_JOURNEY_ID
%type <journey_id> journey_ident
%token <rake_id> TK_RAKE_ID
%token TK_ASGN
%type <attr_jr_asgn> jr_asgn
%token TK_KEY_ASSIGNMENTS
%type <pattr_jr_asgns> jr_asgnmnts journey_rake_asgnmnts journey_rake_asgnmnts_decl
%type <attr_time> time
%token <perf_regime> TK_PERFREG
%token <revenue> TK_REVENUE
%token <crew_id> TK_CREWID
%type <attr_trip_journey> trip_journey dwell_journey arrdep_time_journey perf_journey revenue_journey crewid_journey
%type <pattr_trips_journey> trips_journey
%type <pattr_journey> journey_def
%type <pattr_journeys> journey_definitions journeys_declaration
%type <ptimetable_symtbl> timetable_decl
 /* %start journey_rake_asgnmnts */
%start timetable_decl
%%
timetable_decl : sp_asgns_decl trips_decl journey_rake_asgnmnts_decl journeys_declaration {
  if( ttc_ctrl_flgs.dump_par_symtbl ) {
    printf( "\n" );
    print_par_symtbl( $1, $2, $3, $4 );
  }
  $$ = timetable_symtbl;
 }
;

journeys_declaration : journey_definitions {
  cons_journeys( $1 );
  $$ = $1;
}
/* journey_definitions : TK_JOURNEYS : journey_def*
   journey_def : TK_JOURNEY_ID : trips_journey
   trips_journey : (trips_journey (';' trip_journey)*)?
   e.g.
   journeys:
     J01:
       ( ((JLA,PL1), (KIKJ, PL1)), 37, (11:22:33, 23:59:59), perfslow, revenue, crew_021 );
       ( ((BTGD,PL2), (OKBS, PL2)), 15, (00:13:51, 21:57:13), perffast, revenue, crew_007 );
*/
journey_definitions : TK_JOURNEYS ':' /* empty journies */ {
  assert( journey_id_w.jid_w < 0 );
  assert( timetable_symtbl->journeys_regtbl.kind == PAR_UNKNOWN );
  assert( timetable_symtbl->journeys_regtbl.njourneys == 0 );
  timetable_symtbl->journeys_regtbl.kind = PAR_JOURNEYS;
  $$ = &timetable_symtbl->journeys_regtbl;
 }
              | journey_definitions journey_def {
  assert( journey_id_w.jid_w > 0 );
  assert( timetable_symtbl->journeys_regtbl.kind == PAR_JOURNEYS );
  timetable_symtbl->journeys_regtbl.njourneys++;
  {
    int n = timetable_symtbl->journeys_regtbl.njourneys;
    assert( n > 0 );
    int i;
    for( i = 0; i < MAX_JOURNEYS; i++ ) {
      assert( n >= 0 );
      if( timetable_symtbl->journeys_regtbl.journey_prof[i].journey_id.jid > 0 ) {
	assert( timetable_symtbl->journeys_regtbl.journey_prof[i].kind == PAR_JOURNEY );
	n--;
      }
    }
    assert( n == 0 );
  }
  journey_id_w.jid_w = -1;
  $$ = &timetable_symtbl->journeys_regtbl;  
 }
;
journey_def : journey_ident ':' trips_journey {
  assert( journey_id_w.jid_w == $1 );
  assert( &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips == $3 );
  if( timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].journey_id.jid == journey_id_w.jid_w )
    timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].kind = PAR_JOURNEY;
  else
    timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].kind = PAR_UNKNOWN;
  $$ = &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w];
 }
;
journey_ident : TK_JOURNEY_ID {
  assert( journey_id_w.jid_w < 0 );
  assert( $1 > 0 );
  journey_id_w.jid_w = $1;
  journey_id_w.pos.row = @1.first_line;
  journey_id_w.pos.col = @1.first_column;
  $$ = journey_id_w.jid_w;
 }
;
trips_journey : /* empty trips */ {
  assert( journey_id_w.jid_w > -1 );
  $$ = &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips;
  assert( $$->ntrips == 0 );
}
              | trips_journey trip_journey ';' {
  assert( journey_id_w.jid_w > -1 );
  if( $2.kind == PAR_TRIP ) {
    assert( $1 == &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips );
    ATTR_TRIP_PTR pnxt = &$1->trip_prof[$1->ntrips];  
    ATTR_TRIP_PTR preg = NULL;
    $2.kind = PAR_TRIP;    
    if( $2.deadend ) {
      assert( $2.sp_cond.stop_skip == DWELL );
      assert( $2.sp_cond.dwell_time == 0 );
      if( journey_trip_deadend_acc ) {
	if( !err_stat.par.err_trip_journey ) {
	  printf( "FATAL: syntax-error, multiple dead-end trips found in journey definition, at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
	  err_stat.par.err_trip_journey = TRUE;	  
	}
      } else
	journey_trip_deadend_acc = TRUE;
      goto trips_journey_regtrip;
    } else {
      if( journey_trip_deadend_acc ) {
	if( !err_stat.par.err_trip_journey ) {
	  printf( "FATAL: syntax-error, successive trip over the dead-end in journey definition, at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
	  err_stat.par.err_trip_journey = TRUE;
	}
	goto trips_journey_regtrip;
      } else {
      trips_journey_regtrip:
	preg = reg_trip_journey( &timetable_symtbl->journeys_regtbl, journey_id_w.jid_w, &journey_id_w.pos, &$2 );
	assert( pnxt == preg );
      }
    }
  }
  $$ = &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips;
 }
              | trips_journey trip_journey error ';' {
  assert( journey_id_w.jid_w > -1 );
  if( $2.kind == PAR_TRIP ) {
    assert( $1 == &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips );
    ATTR_TRIP_PTR pnxt = &$1->trip_prof[$1->ntrips];  
    ATTR_TRIP_PTR preg = NULL;
    $2.kind = PAR_TRIP;
    if( journey_trip_deadend_acc ) {
      if( !err_stat.par.err_trip_journey ) {
	printf( "FATAL: syntax-error, successive trip over the dead-end in journey definition, at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
	err_stat.par.err_trip_journey = TRUE;
      }
    } else {
      preg = reg_trip_journey( &timetable_symtbl->journeys_regtbl, journey_id_w.jid_w, &journey_id_w.pos, &$2 );
      assert( pnxt == preg );
    }
  }
  $$ = &timetable_symtbl->journeys_regtbl.journey_prof[journey_id_w.jid_w].trips;
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
/* trip_journey: '(' '('st_and_pltb ',' st_and_pltb')' ',' dwell_journey ',' '('arrtime_journey ',' deptime_journey')' ',' perf_journey ',' revenue_journey ',' crewid_journey ')'
   e.g.
    ( ((JLA,PL1), (KIKJ, PL1)), 37, (11:22:33, 23:59:59), perfslow, revenue, crew_021 )
    ( ((BTGD,PL2), (OKBS, PL2)), 15, (00:13:51, 21:57:13), perffast, revenue, crew_007 )
*/
trip_journey : '(' '('st_and_pltb ',' st_and_pltb')' ')' { /* omitted all elements. */
  raw_journey_trip( &$$ );
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($3.kind == PAR_UNKNOWN) || ($5.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
#if 0 // ***** for debugging.
  {
    print_trip( &$$, TRUE );
  }
#endif
 }
             | error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed trip description found in journey definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
| '[' st_and_pltb ']' { /* omitted arrival time on dead-end. */
  raw_journey_trip( &$$ );
  if( $2.kind == PAR_ST_PLTB ) {
    $$.deadend = TRUE;
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $2;
    $$.sp_cond.dwell_time = 0;
    $$.kind = PAR_TRIP;
  } else {
    assert( $2.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
;
             | '[' st_and_pltb ',' time ']' {
  raw_journey_trip( &$$ );
  if( ($2.kind == PAR_ST_PLTB) && ($4.kind == PAR_TIME_SPEC) ) {
    $$.deadend = TRUE;
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $2;
    $$.arrdep_time.arriv.arr_time.t.hour = $4.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $4.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $4.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @4.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @4.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.sp_cond.dwell_time = 0;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind != PAR_ST_PLTB) || ($4.kind != PAR_TIME_SPEC) );
    $$.kind = PAR_UNKNOWN;
  }
 }
             | '[' st_and_pltb time ']' {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing delimiter in terminal platform/turnback section and its arrival time, in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  raw_journey_trip( &$$ );
  if( ($2.kind == PAR_ST_PLTB) && ($3.kind == PAR_TIME_SPEC) ) {
    $$.deadend = TRUE;
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $2;
    $$.arrdep_time.arriv.arr_time.t.hour = $3.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $3.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $3.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @3.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @3.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.sp_cond.dwell_time = 0;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind != PAR_ST_PLTB) || ($3.kind != PAR_TIME_SPEC) );
    $$.kind = PAR_UNKNOWN;
  }
 }
             | '(' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed trip description found in journey definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' '('st_and_pltb ',' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed trip description found in journey definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' '('st_and_pltb st_and_pltb')' ')' {
  /* including the case of -> '(' '(' error. */
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing delimiter in org & dst platform/turnback section specifier in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  raw_journey_trip( &$$ );
  if( ($3.kind == PAR_ST_PLTB) && ($4.kind == PAR_ST_PLTB) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $4;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($3.kind == PAR_UNKNOWN) || ($4.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
 }
             | '(' '('st_and_pltb ',' st_and_pltb ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis of trip description in journey definition at (LINE, COL) = (%d, %d).\n", @6.first_line, @6.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' '('st_and_pltb st_and_pltb')' ',' dwell_journey {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing delimiter in org & dst platform/turnback section specifier in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  if( ($3.kind == PAR_ST_PLTB) && ($4.kind == PAR_ST_PLTB) && ($7.kind == PAR_TRIP) ) {
    $$ = $7;
    $$.kind = PAR_TRIP;
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $4;
  } else {
    assert( ($3.kind == PAR_UNKNOWN) || ($4.kind == PAR_UNKNOWN) || ($7.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
 }
             | '(' '('st_and_pltb ',' st_and_pltb')' ',' dwell_journey {  
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) && ($8.kind == PAR_TRIP) ) {
    $$ = $8;
    $$.kind = PAR_TRIP;
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
  } else {
    assert( ($3.kind == PAR_UNKNOWN) || ($5.kind == PAR_UNKNOWN) || ($8.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
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
    $$.sp_cond.stop_skip = SKIP;
  } else {
    assert( $1 > 0 );
    $$.sp_cond.stop_skip = DWELL;
  }
  $$.sp_cond.dwell_time = $1;
  $$.sp_cond.pos.row = @1.first_line;
  $$.sp_cond.pos.col = @1.first_column;
  $$.kind = PAR_TRIP;
 }
              | TK_NAT ',' arrdep_time_journey {
  if( $3.kind == PAR_TRIP ) {
    $$ = $3;
    if( $1 == 0 ) {
      $$.sp_cond.stop_skip = SKIP;
    } else {
      assert( $1 > 0 );
      $$.sp_cond.stop_skip = DWELL;
    }
    $$.sp_cond.dwell_time = $1;
    $$.sp_cond.pos.row = @1.first_line;
    $$.sp_cond.pos.col = @1.first_column;
    $$.kind = PAR_TRIP;
  } else {
    assert( $3.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
              | arrdep_time_journey { /* omitted. */
  if( $1.kind == PAR_TRIP ) {
    $$ = $1;
    $$.sp_cond.stop_skip = DWELL;
    $$.sp_cond.dwell_time = DEFAULT_DWELL_TIME;
    $$.sp_cond.pos.row = @1.first_line;
    $$.sp_cond.pos.col = @1.first_column;
    $$.kind = PAR_TRIP;
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
;
arrdep_time_journey : '(' ')' ')' { /* both omitted, form1 */
  raw_journey_trip( &$$ );
  $$.arrdep_time.arriv.arr_time.t.hour = -1;
  $$.arrdep_time.arriv.arr_time.t.minute = -1;
  $$.arrdep_time.arriv.arr_time.t.second = -1;
  $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
  $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
  $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
  $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
  $$.arrdep_time.dept.dep_time.t.hour = -1;
  $$.arrdep_time.dept.dep_time.t.minute = -1;
  $$.arrdep_time.dept.dep_time.t.second = -1;
  $$.arrdep_time.dept.dep_time.pos.row = @1.first_line;
  $$.arrdep_time.dept.dep_time.pos.col = @1.first_column;
  $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
  $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
  $$.kind = PAR_TRIP;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed time specifier found in journey definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' ')' ',' perf_journey {
  if( $4.kind == PAR_PERF_REGIME ) {
    $$ = $4;
    $$.arrdep_time.arriv.arr_time.t.hour = -1;
    $$.arrdep_time.arriv.arr_time.t.minute = -1;
    $$.arrdep_time.arriv.arr_time.t.second = -1;
    $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @1.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @1.first_column;    
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $4.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
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
  $$.arrdep_time.arriv.arr_time.t.hour = -1;
  $$.arrdep_time.arriv.arr_time.t.minute = -1;
  $$.arrdep_time.arriv.arr_time.t.second = -1;
  $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
  $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
  $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
  $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
  $$.arrdep_time.dept.dep_time.t.hour = -1;
  $$.arrdep_time.dept.dep_time.t.minute = -1;
  $$.arrdep_time.dept.dep_time.t.second = -1;
  $$.arrdep_time.dept.dep_time.pos.row = @1.first_line;
  $$.arrdep_time.dept.dep_time.pos.col = @1.first_column;
  $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
  $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
  $$.kind = PAR_TRIP;
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed time specifier found in journey definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' ',' ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' ',' ')' ',' perf_journey {
  if( $5.kind == PAR_PERF_REGIME ) {
    $$ = $5;
    $$.arrdep_time.arriv.arr_time.t.hour = -1;
    $$.arrdep_time.arriv.arr_time.t.minute = -1;
    $$.arrdep_time.arriv.arr_time.t.second = -1;
    $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @2.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @2.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $5.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
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
  if( $1.kind == PAR_PERF_REGIME ) {
    $$ = $1;
    $$.arrdep_time.arriv.arr_time.t.hour = -1;
    $$.arrdep_time.arriv.arr_time.t.minute = -1;
    $$.arrdep_time.arriv.arr_time.t.second = -1;
    $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @1.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @1.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
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
  if( $2.kind == PAR_TIME_SPEC ) {
    raw_journey_trip( &$$ );
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $2.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' time ')' ',' perf_journey {
  if( ($2.kind == PAR_TIME_SPEC) && ($5.kind == PAR_PERF_REGIME) ) {
    $$ = $5;
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind == PAR_UNKNOWN) || ($5.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
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
  if( $2.kind == PAR_TIME_SPEC ) {
    raw_journey_trip( &$$ );
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $2.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ',' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed time specifier found in journey definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' time ',' ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' time ',' ')' ',' perf_journey {
  if( ($2.kind == PAR_TIME_SPEC) && ($6.kind == PAR_PERF_REGIME) ) {
    $$ = $6;
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = -1;
    $$.arrdep_time.dept.dep_time.t.minute = -1;
    $$.arrdep_time.dept.dep_time.t.second = -1;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind == PAR_UNKNOWN) || ($6.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
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
  if( $3.kind == PAR_TIME_SPEC ) {
    raw_journey_trip( &$$ );
    $$.arrdep_time.arriv.arr_time.t.hour = -1;
    $$.arrdep_time.arriv.arr_time.t.minute = -1;
    $$.arrdep_time.arriv.arr_time.t.second = -1;
    $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = $3.t.hour;
    $$.arrdep_time.dept.dep_time.t.minute = $3.t.minute;
    $$.arrdep_time.dept.dep_time.t.second = $3.t.second;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( $3.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' ',' time ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' ',' time ')' ',' perf_journey {
  if( ($3.kind == PAR_TIME_SPEC) && ($6.kind == PAR_PERF_REGIME) ) {
    $$ = $6;
    $$.arrdep_time.arriv.arr_time.t.hour = -1;
    $$.arrdep_time.arriv.arr_time.t.minute = -1;
    $$.arrdep_time.arriv.arr_time.t.second = -1;
    $$.arrdep_time.arriv.arr_time.pos.row = @1.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @1.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = $3.t.hour;
    $$.arrdep_time.dept.dep_time.t.minute = $3.t.minute;
    $$.arrdep_time.dept.dep_time.t.second = $3.t.second;
    $$.arrdep_time.dept.dep_time.pos.row = @3.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @3.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($3.kind == PAR_UNKNOWN) || ($6.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
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
  if( ($2.kind == PAR_TIME_SPEC) && ($4.kind == PAR_TIME_SPEC) ) {
    raw_journey_trip( &$$ );
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = $4.t.hour;
    $$.arrdep_time.dept.dep_time.t.minute = $4.t.minute;
    $$.arrdep_time.dept.dep_time.t.second = $4.t.second;
    $$.arrdep_time.dept.dep_time.pos.row = @4.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @4.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind == PAR_UNKNOWN) || ($4.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
#if 0 // ***** for debugging.
  {
    printf( "arr_time: " );
    print_time( &$$.arrdep_time.arr_time );
    printf( "dep_time: " );
    print_time( &$$.arrdep_time.dep_time );
  }
#endif
 }
                    | '(' time ',' time ')' error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in journey definition at (LINE, COL) = (%d, %d).\n", @5.first_line, @5.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
                    | '(' time ',' time ')' ',' perf_journey {
  if( ($2.kind == PAR_TIME_SPEC) && ($4.kind == PAR_TIME_SPEC) && ($7.kind == PAR_PERF_REGIME) ) {
    $$ = $7;
    $$.arrdep_time.arriv.arr_time.t.hour = $2.t.hour;
    $$.arrdep_time.arriv.arr_time.t.minute = $2.t.minute;
    $$.arrdep_time.arriv.arr_time.t.second = $2.t.second;
    $$.arrdep_time.arriv.arr_time.pos.row = @2.first_line;
    $$.arrdep_time.arriv.arr_time.pos.col = @2.first_column;
    $$.arrdep_time.arriv.pos.row = $$.arrdep_time.arriv.arr_time.pos.row;
    $$.arrdep_time.arriv.pos.col = $$.arrdep_time.arriv.arr_time.pos.col;
    $$.arrdep_time.dept.dep_time.t.hour = $4.t.hour;
    $$.arrdep_time.dept.dep_time.t.minute = $4.t.minute;
    $$.arrdep_time.dept.dep_time.t.second = $4.t.second;
    $$.arrdep_time.dept.dep_time.pos.row = @4.first_line;
    $$.arrdep_time.dept.dep_time.pos.col = @4.first_column;
    $$.arrdep_time.dept.pos.row = $$.arrdep_time.dept.dep_time.pos.row;
    $$.arrdep_time.dept.pos.col = $$.arrdep_time.dept.dep_time.pos.col;
    $$.kind = PAR_TRIP;
  } else {
    assert( ($2.kind == PAR_UNKNOWN) || ($4.kind == PAR_UNKNOWN) || ($7.kind == PAR_UNKNOWN) );
    $$.kind = PAR_UNKNOWN;
  }
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
  $$.perf_regime.perfreg_cmd = $1;
  $$.perf_regime.pos.row = @1.first_line;
  $$.perf_regime.pos.col = @1.first_column;
  $$.kind = PAR_PERF_REGIME;
 }
             | TK_PERFREG ',' revenue_journey {
  if( $3.kind == PAR_REVENUE_STAT ) {
    $$ = $3;
    $$.perf_regime.perfreg_cmd = $1;
    $$.perf_regime.pos.row = @1.first_line;
    $$.perf_regime.pos.col = @1.first_column;
    $$.kind = PAR_PERF_REGIME;
  } else {
    assert( $3.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
             | revenue_journey { /* omitted */
  if( $1.kind == PAR_REVENUE_STAT ) {
    $$ = $1;
    $$.perf_regime.perfreg_cmd = DEFAULT_PERFLEVEL;
    $$.perf_regime.pos.row = @1.first_line;
    $$.perf_regime.pos.col = @1.first_column;
    $$.kind = PAR_PERF_REGIME;
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
;
revenue_journey : TK_REVENUE ')' {
  raw_journey_trip( &$$ );
  $$.revenue.stat = $1;
  $$.revenue.pos.row = @1.first_line;
  $$.revenue.pos.col = @1.first_column;
  $$.kind = PAR_REVENUE_STAT;
 }
                | TK_REVENUE ',' crewid_journey {
  if( $3.kind == PAR_CREWID ) {
    $$ = $3;
    $$.revenue.stat = $1;
    $$.revenue.pos.row = @1.first_line;
    $$.revenue.pos.col = @1.first_column;
    $$.kind = PAR_REVENUE_STAT;
  } else {
    assert( $3.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
                | crewid_journey { /* omitted */
  if( $1.kind == PAR_CREWID ) {
    $$ = $1;
    $$.revenue.stat = DEFAULT_REVENUE;
    $$.revenue.pos.row = @1.first_line;
    $$.revenue.pos.col = @1.first_column;
    $$.kind = PAR_REVENUE_STAT;
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    $$.kind = PAR_UNKNOWN;
  }
 }
;
crewid_journey : TK_CREWID ')' {
  raw_journey_trip( &$$ );
  $$.crew_id.cid = $1;
  $$.crew_id.pos.row = @1.first_line;
  $$.crew_id.pos.col = @1.first_column;
  $$.kind = PAR_CREWID;
 }
               | ')' { /* omitted */
  raw_journey_trip( &$$ );
  $$.crew_id.cid = -1;
  $$.crew_id.pos.row = @1.first_line;
  $$.crew_id.pos.col = @1.first_column;
  $$.kind = PAR_CREWID;
 }
               | error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed dwell description found in journey definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
;
time : TK_TIME {
  $$ = $1;
  $$.kind = PAR_TIME_SPEC;
  $$.pos.row = @1.first_line;
  $$.pos.col = @1.first_column;
 }
     | error {
  if( !err_stat.par.err_trip_journey ) {
    printf( "FATAL: syntax-error, ill-formed time specifier found in journey definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_journey = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
;

/* ((JLA,PL1), SP_73);
 */
sp_asgns_decl : stpl_sp_asgns {
  if( $1 )
    cons_spasgn( $1 );
  $$ = $1;
 }
;
stpl_sp_asgns : TK_SP_ASGNS ':' /* empty pltb_sp_asgnments */ {
  timetable_symtbl->sp_asgn_regtbl.kind = PAR_SP_ASGNS;
  timetable_symtbl->sp_asgn_regtbl.nasgns = 0;
  $$ = &timetable_symtbl->sp_asgn_regtbl;
 }
              | stpl_sp_asgns stpl_sp_asgn {
  assert( $1->kind == PAR_SP_ASGNS ); 
  if( $2.kind == PAR_SP_ASGN )
    reg_sp_asgn( &$2 );
  else
    assert( $2.kind == PAR_UNKNOWN );
  $$ = $1;
 }
              | error {
  if( !err_stat.par.err_sp_def ) {
    printf( "FATAL: syntax-error, no stopping-point definition section at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_sp_def = TRUE;
  }
  $$ = NULL;
 }
;
stpl_sp_asgn : '(' st_and_pltb ',' TK_SP ')' ';' {
  $$.kind = PAR_SP_ASGN;
  $$.st_pltb = $2;
  $$.sp.kind = PAR_SP;
  strncpy( $$.sp.sp_id, $4, MAX_SPNAME_LEN );
  $$.sp.pos.row = @4.first_line;
  $$.sp.pos.col = @4.first_column;
  $$.pos = $$.st_pltb.st.pos;
 }
             | '(' error {
  if( !err_stat.par.err_sp_def ) {
    printf( "FATAL: syntax-error, ill-formed st/pl & stopping-point declaration found in sp definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_sp_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' st_and_pltb ',' error {
  if( !err_stat.par.err_sp_def ) {
    printf( "FATAL: syntax-error, ill-formed stopping-point specifier found in sp definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_sp_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' st_and_pltb ',' TK_SP error {
  if( !err_stat.par.err_sp_def ) {
    printf( "FATAL: syntax-error, ill-formed stopping-point specifier found in sp definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_sp_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
             | '(' st_and_pltb ',' TK_SP ')' error {
  if( !err_stat.par.err_sp_def ) {
    printf( "FATAL: syntax-error, missing semicolon in end of sp definition at (LINE, COL) = (%d, %d).\n", @5.first_line, @5.first_column );
    err_stat.par.err_sp_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
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
trips_decl : trips_defs {
  cons_trips( $1 );
  $$ = $1;
}
;
trips_defs : TK_KEY_TRIPS ':' trips_definition {
  assert( $3 );
  assert( $3->kind == PAR_TRIPS );
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
  if( !err_stat.err_trips_decl ) {
    printf( "FATAL: syntax-error, missing delimiter in trip declaration at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.err_trips_decl = TRUE;
  }
  yyerrok;
 } */
           | error {
  if( !err_stat.par.err_trips_decl ) {
    printf( "FATAL: syntax-error, no trip declaration section at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trips_decl = TRUE;
  }
 }
           | TK_KEY_TRIPS error {
  if( !err_stat.par.err_trips_decl ) {
    printf( "FATAL: syntax-error, incomplete trip declaration at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trips_decl = TRUE;
  }
 }
/*
  causes following shift/reduce conflicts, 2025/3/12.
  
  ttcreat.y: warning: 1 shift/reduce conflict [-Wconflicts-sr]
  ttcreat.y: warning: shift/reduce conflict on token error [-Wcounterexamples]
  First example: journey_rake_asgnmnts journey_definitions
  Shift derivation
    timetable_decl
    ↳ 1: journey_rake_asgnmnts journey_definitions
  Second example: error journey_definitions
  Reduce derivation
    timetable_decl
    ↳ 1: error journey_definitions
    
           | TK_KEY_TRIPS ':' error {
  if( !err_stat.err_trips_decl ) {
    printf( "FATAL: syntax-error, ill-formed trip declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.err_trips_decl = TRUE;
  }
 } */
/*
           | error TK_EOF {
  assert( FALSE );
 }
*/
;
trips_definition : /* empty journies */ {
  timetable_symtbl->trips_regtbl.kind = PAR_TRIPS;
  $$ = &timetable_symtbl->trips_regtbl;
  assert( $$->ntrips == 0 );
}
                 | trips_definition trip_def ';'{
  assert( $1->kind == PAR_TRIPS );
  if( $2.kind == PAR_TRIP ) {
    reg_trip( &$2 );
  }
  $$ = &timetable_symtbl->trips_regtbl;
 }
                 | trips_definition trip_def error ';' {
  assert( $1->kind == PAR_TRIPS );
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing semicolon in end of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  if( $2.kind == PAR_TRIP ) {
    reg_trip( &$2 );
  }
  $$ = &timetable_symtbl->trips_regtbl;
 }
;
/* e.g. (((JLA,PL1), (KIKJ, PL1)), (SP_73, SP_77), {S803B_S831B}) */
trip_def : '(' '('st_and_pltb ',' st_and_pltb')' ',' TK_NAT ',' '{' routes '}' ')' {
  $$.kind = PAR_UNKNOWN;
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) && ($11.kind == PAR_ROUTES) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
    $$.running_time = $8;
    $$.attr_route_ctrl = $11;
    $$.kind = PAR_TRIP;
  }
  err_stat.par.err_trip_def = FALSE;
#if 0 /* ***** for debugging. */
  printf( "(kind, st_pltb_pair, trip_routes): " );
  print_trip( &$$, FALSE );
  printf( "\n" );
#endif
 }
         | '(' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed st. & pl/tb specifiers found in trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
         | '(' '(' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed st. & pl/tb specifiers found in trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
/* the follow reduction rule arises reduce/reduce conflicts.
   | '(' '('st_and_pltb error {
     ;
   } */
         | '(' '('st_and_pltb ',' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed st. & pl/tb specifiers found in trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
/* the follow reduction rule arises reduce/reduce conflicts.
   | '(' '('st_and_pltb ',' st_and_pltb error {
     ;
   } */
         | '(' '('st_and_pltb ',' st_and_pltb')' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed org & dst stopping-point specifiers found in trip definition at (LINE, COL) = (%d, %d).\n", @6.first_line, @6.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed found org & dst stopping-point specifiers in trip definition at (LINE, COL) = (%d, %d).\n", @7.first_line, @7.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
         | '(' '('st_and_pltb ',' st_and_pltb')' TK_NAT ',' '{' routes '}' ')' {
  if( !err_stat.par.err_trip_def ) {    
    printf( "FATAL: syntax-error, missing delimiter in trip definition at (LINE, COL) = (%d, %d).\n", @6.first_line, @6.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) && ($10.kind == PAR_ROUTES) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
    $$.running_time = $7;
    $$.attr_route_ctrl = $10;
    $$.kind = PAR_TRIP;
  }
 }
/* the follow reduction rule arises reduce/reduce conflicts.
   | '(' '('st_and_pltb ',' st_and_pltb')' error {
     ;
   } */
/*
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed running-time found in trip definition at (LINE, COL) = (%d, %d).\n", @7.first_line, @7.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
*/
/*
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' sp_orgdst_pair TK_NAT ',' '{' routes '}' ')' {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing delimiter in trip definition at (LINE, COL) = (%d, %d).\n", @8.first_line, @8.first_column );
    err_stat.par.err_trip_def = TRUE;    
  }
  $$.kind = PAR_UNKNOWN;  
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) && ($8.kind == PAR_SP_PAIR) && ($12.kind == PAR_ROUTES) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
    $$.attr_sp_orgdst = $8;
    $$.running_time = $9;
    $$.attr_route_ctrl = $12;
    $$.kind = PAR_TRIP;
  }
 }
*/
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' TK_NAT ',' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed controlled routes specifiers found in trip definition at (LINE, COL) = (%d, %d).\n", @9.first_line, @9.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' TK_NAT '{' routes '}' ')' {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing delimiter in trip definition at (LINE, COL) = (%d, %d).\n", @8.first_line, @8.first_column );
    err_stat.par.err_trip_def = TRUE;    
  }
  $$.kind = PAR_UNKNOWN;  
  if( ($3.kind == PAR_ST_PLTB) && ($5.kind == PAR_ST_PLTB) && ($10.kind == PAR_ROUTES) ) {
    $$.attr_st_pltb_orgdst.kind = PAR_ST_PLTB_ORGDST;
    $$.attr_st_pltb_orgdst.st_pltb_org = $3;
    $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
    $$.running_time = $8;
    $$.attr_route_ctrl = $10;
    $$.kind = PAR_TRIP;
  }
 }
/* the follow reduction rules arise reduce/reduce conflicts.
   | '(' '('st_and_pltb ',' st_and_pltb')' ',' '{' error {
     ;
   }
   | '(' '('st_and_pltb ',' st_and_pltb')' ',' '{' routes error {
     ;
   } */
         | '(' '('st_and_pltb ',' st_and_pltb')' ',' TK_NAT ',' '{' routes '}' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in trip definition at (LINE, COL) = (%d, %d).\n", @12.first_line, @12.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
;
st_and_pltb : '(' TK_STNAME ',' TK_PLTB_NAME ')' {
  $$.kind = PAR_ST_PLTB;
  strncpy( $$.st.name, $2, MAX_STNAME_LEN );
  $$.st.pos.row = @2.first_line;
  $$.st.pos.col = @2.first_column;
  strncpy( $$.pltb.id, $4, MAX_PLTB_NAMELEN );
  $$.pltb.pos.row = @4.first_line;
  $$.pltb.pos.col = @4.first_column;
  /* print_st_pltb( &$$ ); // ***** for debugging. */
 }
            | '(' TK_STNAME TK_PLTB_NAME ')' {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing delimiter in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_ST_PLTB;
  strncpy( $$.st.name, $2, MAX_STNAME_LEN );
  $$.st.pos.row = @2.first_line;
  $$.st.pos.col = @2.first_column; 
  strncpy( $$.pltb.id, $3, MAX_PLTB_NAMELEN );
  $$.pltb.pos.row = @3.first_line;
  $$.pltb.pos.col = @3.first_column; 
  /* print_st_pltb( &$$ ); // ***** for debugging. */
 }
            | error TK_STNAME ',' TK_PLTB_NAME ')' {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing opening parenthesis in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
/* shift/reduce confliction arises with the rule of-> '(' error, as below.
            | '(' error ',' TK_PLTB_NAME ')' {
  if( !err_stat.err_trip_def ) {
    printf( "FATAL: syntax-error, no origin station name found in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.err_trip_def = TRUE;
  }
  $$.kind = UNKNOWN;
 } */
/*
  shift/reduce confliction arises with the rule of-> '(' TK_STNAME ',' error, as below.
            | '(' TK_STNAME ',' error ')' {
  if( !err_stat.err_trip_def ) {
    printf( "FATAL: syntax-error, no dest platform/turnback found in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.err_trip_def = TRUE;
  }
  $$.kind = UNKNOWN;
 } */
            | '(' TK_STNAME ',' TK_PLTB_NAME error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, missing closing parenthesis in org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @4.first_line, @4.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
            | error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, no org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
            | '(' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, ill-formed org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
            | '(' TK_STNAME error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, incomplete org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
            | '(' TK_STNAME ',' error {
  if( !err_stat.par.err_trip_def ) {
    printf( "FATAL: syntax-error, incomplete org & dst platform/turnback section specifier of trip definition at (LINE, COL) = (%d, %d).\n", @3.first_line, @3.first_column );
    err_stat.par.err_trip_def = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
;

/*
routes0 : '{' routes '}' { // its only for debugging.
  printf( "routes: " );
  print_routes( &$2 );
  printf( "\n" );
 }
 ; */
routes : route {  
  if( $1.kind == PAR_ROUTE ) {
    $$.kind = PAR_ROUTES;
    $$.nroutes = 1;
    $$.route_prof[0] = $1;
    /* printf( "route: %s\n", $$.route_prof[0].name ); // ***** for debugging. */
  } else {
    $$.kind = PAR_UNKNOWN;
    $$.nroutes = 0;
  }
 }
       | routes route {
  if( $1.kind == PAR_ROUTES ) {
    assert( ($1.nroutes > 0) && ($1.nroutes < MAX_TRIP_ROUTES) );
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    assert( $1.nroutes == 0 );
  }
  if( $2.kind == PAR_ROUTE ) {
    int i;
    if( $1.nroutes > 0 ) {
      if( !err_stat.par.err_routes ) {
	printf( "FATAL: syntax-error, missing delimiter in ars controlled routes specifier of trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
	err_stat.par.err_routes = TRUE;
      }
    }
    for( i = 0; i < $1.nroutes; i++ )
      $$.route_prof[i] = $1.route_prof[i];
    assert( i == $1.nroutes );
    $$.route_prof[i] = $2;
    $$.nroutes = $1.nroutes + 1;
    $$.kind = PAR_ROUTES;
  }
 }
       | routes ',' route {
  if( $1.kind == PAR_ROUTES ) {
    assert( ($1.nroutes > 0) && ($1.nroutes < MAX_TRIP_ROUTES) );
  } else {
    assert( $1.kind == PAR_UNKNOWN );
    assert( $1.nroutes == 0 );
  }
  if( $3.kind == PAR_ROUTE ) {
    int i;    
    for( i = 0; i < $1.nroutes; i++ )
      $$.route_prof[i] = $1.route_prof[i];
    assert( i == $1.nroutes );
    $$.route_prof[i] = $3;
    $$.nroutes = $1.nroutes + 1;
    $$.kind = PAR_ROUTES;
  }
 }
;
route : TK_ROUTE {
  assert( $1.kind == PAR_ROUTE );
  $$.kind = $1.kind;
  strncpy( $$.name, $1.name, MAX_ROUTENAME_LEN );
  $$.pos.row = @1.first_line;
  $$.pos.col = @1.first_column;
  /* printf( "(kind, route): (%d, %s)\n", $$.kind, $$.name ); // ***** for debugging. */
 }
      | error {
  if( !err_stat.par.err_routes ) {
    printf( "FATAL: syntax-error, illegal ars controlled route in trip definition at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_routes = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
 }
;


/* journey_rake_asgnmnts := TK_KEY_ASSIGNMENTS ':' jr_asgnmnts
   jr_asgnmnts := (TK_RAKE_ID ":=" TK_JOURNEY_ID ';')*
   e.g.
   assignments:
     rake_801 := J1; rake_802 := J2; rake_803 := J3; rake_804 := J4;
     rake_811 := J11; rake_812 := J12; rake_813 := J13; rake_814 := J14;
*/
journey_rake_asgnmnts_decl : journey_rake_asgnmnts {
  cons_jrasgn( $1 );
  $$ = $1;
}
;
/* journey_rake_asgnmnts : TK_KEY_ASSIGNMENTS ':' jr_asgnmnts { */
journey_rake_asgnmnts : TK_KEY_ASSIGNMENTS ':' jr_asgnmnts {
  assert( $3 );
  assert( $3->kind == PAR_JR_ASGNS );
  $$ = $3;
#if 0 /* ***** for debugging. */
  {
    assert( $$ );
    ATTR_JR_ASGN_PTR p = $$->jr_asgn;
    int i;
    for( i = 0; i < $$->nasgns; i++ ) {
      print_jrasgn( &p[i] );
      printf( "\n" );
    }
  }
#endif
 }
/* reduce/reduce confliction arises with the rule of-> TK_KEY_ASSIGNMENTS error, as below.
                           | TK_KEY_ASSIGNMENTS error jr_asgnmnts {
  if( !err_stat.err_journey_rake_asgnmnts ) {
    printf( "FATAL: syntax-error, missing delimiter in journey-rake assignment declaration at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.err_journey_rake_asgnmnts = TRUE;
  }
  assert( $3 );
  assert( $3->kind == JR_ASGNS );
  $$ = $3;
  yyerrok;
 } */
                           | error {
  if( !err_stat.par.err_journey_rake_asgnmnts ) {
    printf( "FATAL: syntax-error, no journey-rake assignments declaration section at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_journey_rake_asgnmnts = TRUE;
  }
  $$ = &timetable_symtbl->jr_asgn_regtbl;
  /* yyclearin; */
 }
                           | TK_KEY_ASSIGNMENTS error {
  if( !err_stat.par.err_journey_rake_asgnmnts ) {
    printf( "FATAL: syntax-error, incomplete journey-rake assignment declaration at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_journey_rake_asgnmnts = TRUE;
  }
  $$ = &timetable_symtbl->jr_asgn_regtbl;
  /* yyclearin; */
 }
/* reduce/reduce confliction arises with the rule-> | error, of jr_asgn as below.
                           | TK_KEY_ASSIGNMENTS ':' error {
  if( !err_stat.err_journey_rake_asgnmnts ) {
    printf( "FATAL: syntax-error, ill-formed journey-rake assignments declaration at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.err_journey_rake_asgnmnts = TRUE;
  }
  $$ = &timetable_symtbl.jr_asgn_regtbl;
  // yyclearin;
 } */
;
jr_asgnmnts : /* empty journies */ {
  timetable_symtbl->jr_asgn_regtbl.kind = PAR_JR_ASGNS;
  $$ = &timetable_symtbl->jr_asgn_regtbl;
  assert( $$->nasgns == 0 );
 }
                      | jr_asgnmnts jr_asgn ';' {
  assert( $1->kind == PAR_JR_ASGNS );
  if( $2.kind == PAR_JR_ASGN )
    reg_journey_rake_asgn( &$2 );
  $$ = &timetable_symtbl->jr_asgn_regtbl;
 }
                      | jr_asgnmnts jr_asgn error ';' {
  assert( $1->kind == PAR_JR_ASGNS );
  if( !err_stat.par.err_jr_asgn ) {
    printf( "FATAL: syntax-error, missing semicolon in end of journey-rake assignments at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_jr_asgn = TRUE;
  }
  if( $2.kind == PAR_JR_ASGN )
    reg_journey_rake_asgn( &$2 );
  $$ = &timetable_symtbl->jr_asgn_regtbl;
 }
;
jr_asgn : TK_JOURNEY_ID TK_ASGN TK_RAKE_ID {
  $$.kind = PAR_JR_ASGN;
  $$.journey_id.jid = $1;
  $$.journey_id.pos.row = @1.first_line;
  $$.journey_id.pos.col = @1.first_column;;
  $$.rake_id.rid = $3;
  $$.rake_id.pos.row = @3.first_line;
  $$.rake_id.pos.col = @3.first_column;;
  /* print_jrasgn( &$$ ); // ***** for debugging. */
 }
        | error {
  if( !err_stat.par.err_jr_asgn ) {
    printf( "FATAL: syntax-error, in journey-rake assignments at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_jr_asgn = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
  // yyclearin;
 }
        | TK_JOURNEY_ID TK_RAKE_ID {
  if( !err_stat.par.err_jr_asgn ) {
    printf( "FATAL: syntax-error, missing operator in journey-rake assignments at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_jr_asgn = TRUE;
  }
  $$.kind = PAR_JR_ASGN;
  $$.journey_id.jid = $1;
  $$.journey_id.pos.row = @1.first_line;
  $$.journey_id.pos.col = @1.first_column;;
  $$.rake_id.rid = $2;
  $$.rake_id.pos.row = @2.first_line;
  $$.rake_id.pos.col = @2.first_column;;
 }
        | TK_JOURNEY_ID error {
  if( !err_stat.par.err_jr_asgn ) {
    printf( "FATAL: syntax-error, incomplete journey-rake assignment, at (LINE, COL) = (%d, %d).\n", @1.first_line, @1.first_column );
    err_stat.par.err_jr_asgn = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
  // yyclearin;
 }
        | TK_JOURNEY_ID TK_ASGN error {
  if( !err_stat.par.err_jr_asgn ) {
    printf( "FATAL: syntax-error, ill-formed journey-rake assignments at (LINE, COL) = (%d, %d).\n", @2.first_line, @2.first_column );
    err_stat.par.err_jr_asgn = TRUE;
  }
  $$.kind = PAR_UNKNOWN;
  // yyclearin;
 }
;
%%
int yyerror ( const char *s ) {
  return 1;
}


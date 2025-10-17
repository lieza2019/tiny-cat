#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

static DWELL_ID dwell_seq( STOPPING_POINT_CODE sp ) {
  static DWELL_ID book[END_OF_SPs];
  assert( sp < END_OF_SPs );
  DWELL_ID r = 0;
  
  assert( book[sp] > -1 );
  if( book[sp] == 0 )
    book[sp] = 1;
  r = book[sp];
  book[sp]++;
  return r;
}

static void print_ARS_SCHEDULED_ARRIVAL ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == ARS_SCHEDULED_ARRIVAL );  
  printf( "%s", "ARS_SCHEDULED_ARRIVAL, " );
  printf( "dwell_id: %d, ", pcmd->attr.sch_arriv.dw_seq ); // for DWELL_ID dw_seq;
  {
    const char *str = cnv2str_sp_code( pcmd->attr.sch_arriv.arr_sp );
    printf( "arr_sp: %s, ", (str ? str : "unknown") ); // for STOPPING_POINT_CODE arr_sp;
  }
  printf( "arr_time: " );
  print_time_desc( &pcmd->attr.sch_arriv.arr_time ); // for ARS_ASSOC_TIME arr_time;
}

static void print_ARS_SCHEDULED_ROUTESET ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == ARS_SCHEDULED_ROUTESET );
  printf( "%s", "ARS_SCHEDULED_ROUTESET, " );  
  printf( "nth_routeset: %d, ", pcmd->attr.sch_roset.nth_routeset ); // for int nth_routeset;
  printf( "route_id: %s, ", cnv2str_il_sym( pcmd->attr.sch_roset.route_id ) ); // for IL_SYM route_id;
  printf( "is_dept: %s, ", (pcmd->attr.sch_roset.is_dept_route ? "TRUE" : "FALSE") ); // for BOOL is_dept_route;
  printf( "dept_time: " );
  print_time_desc( &pcmd->attr.sch_roset.dep_time ); // for ARS_ASSOC_TIME dept_time;
}

static void print_ARS_SCHEDULED_DEPT ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == ARS_SCHEDULED_DEPT );
  printf( "%s", "ARS_SCHEDULED_DEPT, " );
#if 0
  struct { // for ARS_SCHEDULED_DEPT
    struct {
      BOOL L, R;
    } dep_dir;
  } sch_dept;
#endif
  printf( "dwell_id: %d, ", pcmd->attr.sch_dept.dw_seq ); // for DWELL_ID dw_seq;
  printf( "dwell: %d, ", pcmd->attr.sch_dept.dwell ); // for TIME_DIFF dwell;
  {
    const char *str = cnv2str_sp_code( pcmd->attr.sch_dept.dep_sp );
    printf( "dep_sp: %s, ", (str ? str : "unknown") ); // for STOPPING_POINT_CODE dep_sp;
  }
  printf( "dep_time: " );
  print_time_desc( &pcmd->attr.sch_dept.dep_time ); // for ARS_ASSOC_TIME dep_time;
  printf( ", is_revenue: %s, ", (pcmd->attr.sch_dept.is_revenue ? "TRUE" : "FALSE") ); // for BOOL is_revenue;
  {
    const char *str = cnv2str_perfreg_level[pcmd->attr.sch_dept.perf_lev];
    printf( "perf_lev: %s, ", (str ? str : "unknown") ); // for PERFREG_LEVEL perf_lev;
  }
  {
    const char *str = cnv2str_crew_id[pcmd->attr.sch_dept.crew_id];
    printf( "crew_id: %s, ", (str ? str : "unknown") ); // for CREW_ID crew_id;
  }
  printf( "dep_route: %s", cnv2str_il_sym( pcmd->attr.sch_dept.dep_route ) ); // for IL_SYM dep_route;
}
static void print_ARS_SCHEDULED_ROUTEREL ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == ARS_SCHEDULED_ROUTEREL );
  printf( "%s", "ARS_SCHEDULED_ROUTEREL, " );
  printf( "nth_routerel: %d, ", pcmd->attr.sch_rorel.nth_routerel ); // for int nth_routerel;
  printf( "route_id: %s, ", cnv2str_il_sym( pcmd->attr.sch_rorel.route_id ) ); // for IL_SYM route_id;
  printf( "dept_time: " );
  print_time_desc( &pcmd->attr.sch_rorel.dep_time ); // for ARS_ASSOC_TIME dept_time;
}

static void print_ARS_SCHEDULED_SKIP ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == ARS_SCHEDULED_SKIP );
  printf( "%s", "ARS_SCHEDULED_SKIP, " );
  printf( "dwell_id: %d, ", pcmd->attr.sch_skip.dw_seq ); // for DWELL_ID dw_seq;
  {
    const char *str = cnv2str_sp_code( pcmd->attr.sch_skip.pass_sp );
    printf( "pass_sp: %s, ", (str ? str : "unknown") ); // for STOPPING_POINT_CODE pass_sp;
  }
  printf( "pass_time: " );
  print_time_desc( &pcmd->attr.sch_skip.pass_time ); // for ARS_ASSOC_TIME pass_tim;
  printf( ", is_revenue: %s, ", (pcmd->attr.sch_skip.is_revenue ? "TRUE" : "FALSE") ); // for BOOL is_revenue;
  {
    const char *str = cnv2str_perfreg_level[pcmd->attr.sch_skip.perf_lev];
    printf( "perf_lev: %s, ", (str ? str : "unknown") ); // for PERFREG_LEVEL perf_lev;
  }
  {
    const char *str = cnv2str_crew_id[pcmd->attr.sch_skip.crew_id];
    printf( "crew_id: %s", (str ? str : "unknown") ); // for CREW_ID crew_id;
  }
}

static void print_END_OF_SCHEDULED_CMDS ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  assert( pcmd->cmd == END_OF_SCHEDULED_CMDS );
  printf( "%s", "END_OF_SCHEDULED_CMDS" );  
}

void ttc_print_schcmds ( SCHEDULED_COMMAND_PTR pschcmds, const int nindents ) {
  SCHEDULED_COMMAND_PTR pcmd = pschcmds;
  while( pcmd ) {
    TTC_DIAG_INDENT( nindents );
    printf( "(J%03d, ", pcmd->jid );
    switch( pcmd->cmd ) {
    case ARS_SCHEDULED_ARRIVAL:
      print_ARS_SCHEDULED_ARRIVAL( pcmd );
      break;
    case ARS_SCHEDULED_ROUTESET:
      print_ARS_SCHEDULED_ROUTESET( pcmd );
      break;
    case ARS_SCHEDULED_DEPT:
      print_ARS_SCHEDULED_DEPT( pcmd );
      break;
    case ARS_SCHEDULED_ROUTEREL:
      print_ARS_SCHEDULED_ROUTEREL( pcmd );
      break;
    case ARS_SCHEDULED_SKIP:
      print_ARS_SCHEDULED_SKIP( pcmd );
      break;
    case END_OF_SCHEDULED_CMDS:
      print_END_OF_SCHEDULED_CMDS( pcmd );
      break;
    default:
      assert( FALSE );
    }
    printf( ");\n" );
    pcmd = pcmd->ln.journey.planned.pNext;
  }
}

static SCHEDULED_COMMAND_PTR cons_rosetrel_cmds ( JOURNEY_ID jid, JOURNEY_TRIP_PTR pjprof, TRIP_DESC_PTR pjtrip, SCHEDULED_COMMAND_PTR cmd_org, DWELL_ID org_dwid ) {
  assert( pjprof );
  assert( pjtrip );
  assert( pjtrip == lkup_trip( &pjprof->st_pltb_orgdst.org, &pjprof->st_pltb_orgdst.dst ) );
  assert( cmd_org );
  SCHEDULED_COMMAND_PTR r = NULL;
  
  SCHEDULED_COMMAND_PTR psc_roset = NULL;
  SCHEDULED_COMMAND_PTR psc_dep = NULL;
  SCHEDULED_COMMAND_PTR psc_rorel = NULL;
  
  SCHEDULED_COMMAND_PTR roset_org = NULL;
  SCHEDULED_COMMAND_PTR rorel_org = NULL;
  {
    int i;
    assert( pjtrip->num_routes <= MAX_TRIP_ROUTES );
    for( i = 0; i < pjtrip->num_routes; i++ ) {
      psc_roset = newnode_schedulecmd();
      assert( psc_roset );
      psc_roset->ln.journey.planned.pNext = NULL;
      psc_roset->jid = jid;
      psc_roset->cmd = ARS_SCHEDULED_ROUTESET;
      psc_roset->attr.sch_roset.nth_routeset = i + 1;
      assert( pjtrip->routes[i].id == pjtrip->routes[i].pprof->id );
      psc_roset->attr.sch_roset.route_id = pjtrip->routes[i].pprof->id;
      psc_roset->attr.sch_roset.proute_prof = pjtrip->routes[i].pprof;
      psc_roset->attr.sch_roset.is_dept_route = (pjtrip->routes[i].pprof->route_kind == DEP_ROUTE);
      psc_roset->attr.sch_roset.dep_time = pjprof->time_arrdep.time_dep;
      if( psc_rorel )
	psc_rorel->ln.journey.planned.pNext = psc_roset;
      if( !roset_org )
	roset_org = psc_roset;
      if( !psc_dep ) {
	psc_dep = newnode_schedulecmd();
	assert( psc_dep );
	psc_dep->ln.journey.planned.pNext = NULL;
	psc_dep->jid = jid;
	switch( cmd_org->cmd ) {
	case ARS_SCHEDULED_ARRIVAL:
	  psc_dep->cmd = ARS_SCHEDULED_DEPT;
	  psc_dep->attr.sch_dept.dw_seq = org_dwid;
	  psc_dep->attr.sch_dept.dwell = pjprof->sp_cond.dwell_time;
	  assert( pjprof->ptrip_prof );
	  psc_dep->attr.sch_dept.dep_sp = pjprof->ptrip_prof->sp_orgdst.sp_org;
	  psc_dep->attr.sch_dept.dep_time = pjprof->time_arrdep.time_dep;
	  psc_dep->attr.sch_dept.is_revenue = pjprof->is_revenue;
	  psc_dep->attr.sch_dept.perf_lev = pjprof->perfreg;
	  psc_dep->attr.sch_dept.crew_id = pjprof->crew_id;
	  psc_dep->attr.sch_dept.dep_route = psc_roset->attr.sch_roset.route_id;
	  psc_dep->attr.sch_dept.dep_dir.L = FALSE;
	  psc_dep->attr.sch_dept.dep_dir.R = FALSE;
	  if( psc_roset->attr.sch_roset.proute_prof ) {
	    switch( (psc_roset->attr.sch_roset.proute_prof)->bound ) {
	    case ROUTE2_LEFT:
	      psc_dep->attr.sch_dept.dep_dir.L = TRUE;
	      break;
	    case ROUTE2_RIGHT:
	      psc_dep->attr.sch_dept.dep_dir.R = TRUE;
	      break;
	    case ROUTE_ALIGN_UNKNOWN:
	      /* fail thru. */
	    default:
	      break;
	    }	    
	  }
	  psc_dep->attr.sch_dept.proute_prof = psc_roset->attr.sch_roset.proute_prof;
	  assert( psc_dep->attr.sch_dept.proute_prof );
	  //psc_dep->attr.sch_dept.depdir = ;
	  break;
	case ARS_SCHEDULED_SKIP:
	  psc_dep = cmd_org;
	  break;
	default:
	  assert( FALSE );
	}
      }
      psc_rorel = newnode_schedulecmd();
      assert( psc_rorel );
      psc_rorel->ln.journey.planned.pNext = NULL;
      psc_rorel->jid = jid;
      psc_rorel->cmd = ARS_SCHEDULED_ROUTEREL;
      psc_rorel->attr.sch_rorel.nth_routerel = i + 1;
      psc_rorel->attr.sch_rorel.route_id = pjtrip->routes[i].pprof->id;
      psc_rorel->attr.sch_rorel.proute_prof = pjtrip->routes[i].pprof;
      psc_rorel->attr.sch_rorel.dep_time = pjprof->time_arrdep.time_dep;
      assert( psc_roset );
      psc_roset->ln.journey.planned.pNext = psc_rorel;
      if( !rorel_org )
	rorel_org = psc_rorel;
    }
    assert( rorel_org );
  }
  switch( cmd_org->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    assert( cmd_org->attr.sch_arriv.dw_seq == org_dwid );
    cmd_org->ln.journey.planned.pNext = roset_org;
    roset_org->ln.journey.planned.pNext = psc_dep;
    psc_dep->ln.journey.planned.pNext = rorel_org;
    r = cmd_org;
    break;
  case ARS_SCHEDULED_SKIP:
    assert( cmd_org->attr.sch_skip.dw_seq == org_dwid );
    assert( psc_dep == cmd_org );
    roset_org->ln.journey.planned.pNext = psc_dep;
    psc_dep->ln.journey.planned.pNext = rorel_org;
    r = roset_org;
    break;
  default:
    assert( FALSE );
  }
  return r;
}

void emit_scheduled_cmds ( void ) {
  assert( scheduled_cmds.nodes );
  assert( scheduled_cmds.plim );
  int jcnt;
  int i;
  
  for( i = 0, jcnt = 0; (i < MAX_JOURNEYS) && (jcnt < timetbl_dataset->j.num_journeys); i++ ) {
    struct {
      SCHEDULED_COMMAND_PTR phead;
      SCHEDULED_COMMAND_PTR ptail;
    } sch_cmds = { NULL, NULL };
    JOURNEY_DESC_PTR pjd = &timetbl_dataset->j.journeys[i];
    assert( pjd );
    if( pjd->jid > -1 ) {
      int k;
      for( k = 0; (k < MAX_JOURNEY_TRIPS) && (k < pjd->num_trips); k++ ) {
	JOURNEY_TRIP_PTR pjt = &pjd->trips[k];
	assert( pjt );
	TRIP_DESC_PTR pt = NULL;
	SCHEDULED_COMMAND_PTR psc_org = NULL;
	DWELL_ID wid = -1;	
	psc_org = newnode_schedulecmd();
	assert( psc_org );
	psc_org->ln.journey.planned.pNext = NULL;
	psc_org->jid = pjd->jid;
	wid = dwell_seq( 0 );
	assert( wid > -1 );
	if( !pjt->deadend )
	  pt = lkup_trip( &pjt->st_pltb_orgdst.org, &pjt->st_pltb_orgdst.dst );
	if( pjt->sp_cond.stop_skip == DWELL ) {
	  psc_org->cmd = ARS_SCHEDULED_ARRIVAL;
	  psc_org->attr.sch_arriv.dw_seq = wid;
	  if( pjt->deadend )	    
	    psc_org->attr.sch_arriv.arr_sp = lkup_spcode( &pjt->st_pltb_orgdst.org );
	  else
	    psc_org->attr.sch_arriv.arr_sp = (pt ? pt->sp_orgdst.sp_org : SP_NONSENS);
	  psc_org->attr.sch_arriv.arr_time = pjt->time_arrdep.time_arr;
	} else {
	  assert( pjt->sp_cond.stop_skip == SKIP );  
	  psc_org->cmd = ARS_SCHEDULED_SKIP;
	  psc_org->attr.sch_skip.dw_seq = wid;
	  if( pjt->deadend )
	    psc_org->attr.sch_skip.pass_sp = lkup_spcode( &pjt->st_pltb_orgdst.org );
	  else
	    psc_org->attr.sch_skip.pass_sp = (pt ? pt->sp_orgdst.sp_org : SP_NONSENS);
	  assert( CMP_TINYTIME( pjt->time_arrdep.time_arr, pjt->time_arrdep.time_dep ) );
	  psc_org->attr.sch_skip.pass_time = pjt->time_arrdep.time_arr;
	  psc_org->attr.sch_skip.is_revenue = pjt->is_revenue;
	  psc_org->attr.sch_skip.perf_lev = pjt->perfreg;
	  psc_org->attr.sch_skip.crew_id = pjt->crew_id;
	}
	{
	  assert( pjt );
	  assert( psc_org );
	  assert( wid > -1 );
	  SCHEDULED_COMMAND_PTR pcmds = NULL;
	  if( !pjt->deadend && pt) {
	    pcmds = cons_rosetrel_cmds( pjd->jid, pjt, pt, psc_org, wid );
	    assert( pcmds );	  
	    pjt->pschcmds_trip.top = pcmds;
	    {
	      SCHEDULED_COMMAND_PTR p = pcmds;
	      while( p->ln.journey.planned.pNext ) {
		assert( p );
		p = p->ln.journey.planned.pNext;
	      }
	      assert( !p->ln.journey.planned.pNext );
	      pjt->pschcmds_trip.last = p;
	    }
	  } else {
	    assert( psc_org );
	    assert( !psc_org->ln.journey.planned.pNext );
	    pjt->pschcmds_trip.top = psc_org;
	    pjt->pschcmds_trip.last = pjt->pschcmds_trip.top;
	    pcmds = psc_org;
	  }
	  assert( ! (pjt->pschcmds_trip.last)->ln.journey.planned.pNext );       
	  if( sch_cmds.ptail ) {
	    assert( sch_cmds.phead );
	    sch_cmds.ptail->ln.journey.planned.pNext = pcmds;
	  } else {
	    assert( !sch_cmds.phead );
	    sch_cmds.phead = pcmds;
	  }
	  sch_cmds.ptail = pjt->pschcmds_trip.last;
	}
      }
      assert( k == pjd->num_trips );
      pjd->valid = TRUE;
      pjd->pschcmds_journey = sch_cmds.phead;
      if( pjd->num_trips > 0 )
	pjd->start_time = pjd->trips[0].time_arrdep.time_arr;
#if 0
      if( pjd ) {
	assert( sch_cmds.phead && sch_cmds.ptail );
	SCHEDULED_COMMAND_PTR psc_fin = NULL;
	psc_fin = newnode_schedulecmd();
	assert( psc_fin );
	psc_fin->ln.journey.planned.pNext = NULL;
	psc_fin->jid = pjd->jid;
	psc_fin->cmd = END_OF_SCHEDULED_CMDS;
	sch_cmds.ptail->ln.journey.planned.pNext = psc_fin;
	assert( (pjd->pschcmds_journey->cmd == ARS_SCHEDULED_ARRIVAL) || (pjd->pschcmds_journey->cmd == ARS_SCHEDULED_SKIP) );
      }
#else
      {
	assert( sch_cmds.phead && sch_cmds.ptail );
	SCHEDULED_COMMAND_PTR psc_fin = NULL;
	psc_fin = newnode_schedulecmd();
	assert( psc_fin );
	psc_fin->ln.journey.planned.pNext = NULL;
	psc_fin->jid = pjd->jid;
	psc_fin->cmd = END_OF_SCHEDULED_CMDS;
	sch_cmds.ptail->ln.journey.planned.pNext = psc_fin;
	assert( (pjd->pschcmds_journey->cmd == ARS_SCHEDULED_ARRIVAL) || (pjd->pschcmds_journey->cmd == ARS_SCHEDULED_SKIP) );
      }
#endif
      jcnt++;
    }
  }
  assert( jcnt == timetbl_dataset->j.num_journeys );
}

static ARS_ASSOC_TIME_PTR journey_start_time ( SCHEDULED_COMMAND_PTR pcmd ) {
  assert( pcmd );
  ARS_ASSOC_TIME_PTR r = NULL;
  
  switch( pcmd->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    r = &pcmd->attr.sch_arriv.arr_time;
    break;
  case ARS_SCHEDULED_ROUTESET:
    assert( FALSE );
    r = &pcmd->attr.sch_roset.dep_time;
    break;
  case ARS_SCHEDULED_DEPT:
    assert( FALSE );
    r = &pcmd->attr.sch_dept.dep_time;
    break;
  case ARS_SCHEDULED_ROUTEREL:
    //assert( FALSE );
    r = &pcmd->attr.sch_rorel.dep_time;
    break;
  case ARS_SCHEDULED_SKIP:
    r = &pcmd->attr.sch_skip.pass_time;
    break;
  case END_OF_SCHEDULED_CMDS:
    assert( FALSE );
    break;
  default:
    assert( FALSE );
  }
  return r;
}

static int rake_journey_asgn ( JOURNEY_ID jid ) {
  assert( jid > -1 );
  int r = -1;  
  
  int cnt;
  int i;
  for( i = 0, cnt = 0; i < MAX_JR_ASGNMENTS; i++ ) {
    JOURNEY_RAKE_ASGN_PTR pa = &timetbl_dataset->jr_asgns.jrasgns[i];
    assert( pa );
    if( cnt >= timetbl_dataset->jr_asgns.num_asgns ) {
      assert( r < 0 );
      break;
    }
    if( pa->jid > -1 ) {
      if( pa->jid == jid) {
	r = pa->rake_id;
	break;
      }
      cnt++;
    }
  }
  if( r < 0 )
    assert( cnt == timetbl_dataset->jr_asgns.num_asgns );
  return r;
}

int load_online_timetbl ( void ) {
  int r = 0;
  
  int jcnt = 0;
  int i;
  int k;
  for( i = 0, k = 0; (i < MAX_JOURNEYS) && (jcnt < timetbl_dataset->j.num_journeys); i++ ) {
    JOURNEY_DESC_PTR pjd = &timetbl_dataset->j.journeys[i];
    assert( pjd );
    if( pjd->jid > -1 ) {
      assert( pjd->pschcmds_journey );
      JOURNEY_PTR pj = &online_timetbl.journeys[k].journey;
      assert( pj );
      pj->valid = TRUE;
      pj->jid = pjd->jid;
      {
	ARS_ASSOC_TIME_PTR psta = journey_start_time( pjd->pschcmds_journey );
	pj->start_time.hour = 0;
	pj->start_time.minute = 0;
	pj->start_time.second = 0;
	pj->start_time.year = 0;
	pj->start_time.month = 0;
	pj->start_time.day = 0;	  
	pj->finish_time.hour = pj->start_time.hour;
	pj->finish_time.minute = pj->start_time.minute;
	pj->finish_time.second = pj->start_time.second;
	pj->finish_time.year = pj->start_time.year;
	pj->finish_time.month = pj->start_time.month;
	pj->finish_time.day = pj->start_time.day;
	if( psta ) {
	  ARS_ASSOC_TIME_PTR pfin = NULL;
	  pj->start_time = *psta;
	  {
	    SCHEDULED_COMMAND_PTR p = pjd->pschcmds_journey;
	    assert( p );
	    while( p->ln.journey.planned.pNext ) {
	      SCHEDULED_COMMAND_PTR w = p->ln.journey.planned.pNext;
	      assert( w );
	      if( w->cmd == END_OF_SCHEDULED_CMDS )
		break;
	      else
		p = w;
	      assert( p );
	    }
	    assert( p );
	    assert( p->ln.journey.planned.pNext );
	    assert( (p->ln.journey.planned.pNext)->cmd == END_OF_SCHEDULED_CMDS );
	    pfin = journey_start_time( p );
	    if( pfin )
	      pj->finish_time = *pfin;
	    else
	      assert( FALSE );
	  }
	} else
	  assert( FALSE );
      }
      pj->scheduled_commands.pcmds = pjd->pschcmds_journey;
      pj->scheduled_commands.pNext = pj->scheduled_commands.pcmds;
      {
	int r = -1;
	r = rake_journey_asgn( pj->jid );
	online_timetbl.journeys[k].rake_id = (r > -1 ? r : 0);
      }
      k++;
      jcnt++;
    }
  }
  assert( jcnt == timetbl_dataset->j.num_journeys );
  assert( k == jcnt );
  r = jcnt;
  online_timetbl.num_journeys = r;
  return r;
}

static ARS_ASSOC_TIME_PTR make_it_today ( SCHEDULED_COMMAND_PTR pschcmd ) {
  assert( pschcmd );
  ARS_ASSOC_TIME_PTR ptime = NULL;
  
  struct tm *pT_crnt = NULL;
  time_t crnt_time = 0;
  tzset();
  crnt_time = time( NULL );
  pT_crnt = localtime( &crnt_time );
  
  if( pT_crnt ) {
    const int this_year = pT_crnt->tm_year + 1900;
    const int this_month = pT_crnt->tm_mon + 1;
    const int today = pT_crnt->tm_mday;
    switch( pschcmd->cmd ) {
    case ARS_SCHEDULED_ROUTESET:
      pschcmd->attr.sch_roset.dep_time.year = this_year;
      pschcmd->attr.sch_roset.dep_time.month = this_month;
      pschcmd->attr.sch_roset.dep_time.day = today;
      ptime = &pschcmd->attr.sch_roset.dep_time;
      break;
    case ARS_SCHEDULED_ROUTEREL:
      pschcmd->attr.sch_rorel.dep_time.year = this_year;
      pschcmd->attr.sch_rorel.dep_time.month = this_month;
      pschcmd->attr.sch_rorel.dep_time.day = today;
      ptime = &pschcmd->attr.sch_rorel.dep_time;
      break;
    case ARS_SCHEDULED_ARRIVAL:
      pschcmd->attr.sch_arriv.arr_time.year = this_year;
      pschcmd->attr.sch_arriv.arr_time.month = this_month;
      pschcmd->attr.sch_arriv.arr_time.day = today;
      ptime = &pschcmd->attr.sch_arriv.arr_time;
      break;
    case ARS_SCHEDULED_DEPT:
      pschcmd->attr.sch_dept.dep_time.year = this_year;
      pschcmd->attr.sch_dept.dep_time.month = this_month;
      pschcmd->attr.sch_dept.dep_time.day = today;
      ptime = &pschcmd->attr.sch_dept.dep_time;
      break;
    case ARS_SCHEDULED_SKIP:
      pschcmd->attr.sch_skip.pass_time.year = this_year;
      pschcmd->attr.sch_skip.pass_time.month = this_month;
      pschcmd->attr.sch_skip.pass_time.day = today;
      ptime = &pschcmd->attr.sch_skip.pass_time;
      break;
    case END_OF_SCHEDULED_CMDS:
      /* fail thru.*/
    case ARS_CMD_NOP:
      break;
    default:
      assert( FALSE );
      break;
    }
  }
  return ptime;
}
static int make_cmds_today ( ARS_ASSOC_TIME_PTR start_time, SCHEDULED_COMMAND_PTR pschcmds ) {
  assert( start_time );
  assert( pschcmds );
  int cnt = 0;
  
  SCHEDULED_COMMAND_PTR pcmd = pschcmds;
  while( pcmd ) {
    ARS_ASSOC_TIME_PTR ptime = NULL;
    ptime = make_it_today( pcmd );
    if( pcmd == pschcmds )
      *start_time = *ptime;
    cnt++;
    pcmd = pcmd->ln.journey.planned.pNext;
  }
  return cnt;
}

static int asgned_rakeid ( JOURNEY_ID jid ) {
  assert( timetbl_dataset );
  int r = -1;
  
  int i;
  for( i = 0; i < timetbl_dataset->jr_asgns.num_asgns; i++ ) {
    if( timetbl_dataset->jr_asgns.jrasgns[i].jid == jid ) {
      r = timetbl_dataset->jr_asgns.jrasgns[i].rake_id;
      break;
    }
  }
  return r;
}

static struct {
  SCHEDULED_COMMAND_PTR pfrontier;
  SCHEDULED_COMMAND_PTR pfree;
  SCHEDULED_COMMAND nodebuf[SCHEDULED_CMDS_NODEBUF_SIZE * MAX_JOURNEYS_IN_TIMETABLE];
} schcmds_buf = { NULL, NULL };
int cons_online_timetbl ( void ) {
  assert( timetbl_dataset );
  int cnt = 0;
  
  int i = 0;
  schcmds_buf.pfrontier = &schcmds_buf.nodebuf[0];
  online_timetbl.num_journeys = timetbl_dataset->j.num_journeys;
  while( i < MAX_JOURNEYS ) {
    if( timetbl_dataset->j.journeys[i].jid > 0  ) {
      online_timetbl.journeys[cnt].journey.jid = timetbl_dataset->j.journeys[i].jid;
      online_timetbl.journeys[cnt].journey.valid = timetbl_dataset->j.journeys[i].valid;
#if 0
      make_cmds_today( timetbl_dataset->j.journeys[i].pschcmds_journey );
      online_timetbl.journeys[cnt].journey.start_time = (ARS_ASSOC_TIME)timetbl_dataset->j.journeys[i].start_time;
      online_timetbl.journeys[cnt].journey.scheduled_commands.pcmds = timetbl_dataset->j.journeys[i].pschcmds_journey;
      online_timetbl.journeys[cnt].journey.scheduled_commands.pNext = &timetbl_dataset->j.journeys[i].pschcmds_journey[0];
      online_timetbl.journeys[cnt].rake_id = asgned_rakeid( timetbl_dataset->j.journeys[i].jid );
#else
      make_cmds_today( &online_timetbl.journeys[cnt].journey.start_time, timetbl_dataset->j.journeys[i].pschcmds_journey );
      {
	assert( schcmds_buf.pfrontier < &schcmds_buf.nodebuf[SCHEDULED_CMDS_NODEBUF_SIZE * MAX_JOURNEYS_IN_TIMETABLE] );
	SCHEDULED_COMMAND_PTR pnode = schcmds_buf.pfrontier;
	assert( pnode );
	SCHEDULED_COMMAND_PTR pcmd = timetbl_dataset->j.journeys[i].pschcmds_journey;
	while( pcmd ) {
	  assert( pcmd );
	  assert( pnode < &schcmds_buf.nodebuf[SCHEDULED_CMDS_NODEBUF_SIZE * MAX_JOURNEYS_IN_TIMETABLE] );
	  *pnode = *pcmd;
	  pnode++;
	  pcmd = pcmd->ln.journey.planned.pNext;
	}
	online_timetbl.journeys[cnt].journey.scheduled_commands.pcmds = schcmds_buf.pfrontier;
	online_timetbl.journeys[cnt].journey.scheduled_commands.pNext = &schcmds_buf.pfrontier[0];	
	schcmds_buf.pfrontier = pnode;
      }
#endif
      online_timetbl.journeys[cnt].rake_id = asgned_rakeid( timetbl_dataset->j.journeys[i].jid );
      cnt++;
    }
    i++;
  }
  return cnt;
}

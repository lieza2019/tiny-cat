/*
 * handling the setting value for psc_dep->attr.sch_dept.depdir.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

struct scheduled_cmds_nodebuf scheduled_cmds;

ERR_STAT err_stat;
TIMETABLE_DATASET timetbl_dataset;

char *cnv2str_kind ( char *pstr, PAR_KIND kind, const int buflen ) {
  assert( pstr );
  assert( buflen > 0 );
  char *r = NULL;
  
  switch( kind ) {
  case PAR_UNKNOWN:
    strncpy( pstr, "UNKNOWN", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_ST_PLTB:
    strncpy( pstr, "ST_PLTB", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_ST_PLTB_ORGDST:
    strncpy( pstr, "ST_PLTB_PAIR", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_SP:
    strncpy( pstr, "SP", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_SP_PAIR:
    strncpy( pstr, "SP_PAIR", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_ROUTE:
    strncpy( pstr, "ROUTE", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_ROUTES:
    strncpy( pstr, "ROUTES", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_TRIP:
    strncpy( pstr, "TRIP", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_TRIPS:
    strncpy( pstr, "TRIPS", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_JR_ASGN:
    strncpy( pstr, "JR_ASGN", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_JR_ASGNS:
    strncpy( pstr, "JR_ASGNS", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_JOURNEY:
    strncpy( pstr, "JOURNEY", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  default:
    assert( FALSE );
  }
  return r;
}

char *cnv2str_sp_cond ( char *pstr, ARS_SP_COND sp_cond, const int buflen ) {
  assert( pstr );
  assert( buflen > 0 );
  char *r = NULL;
  
  switch( sp_cond ) {
  case DWELL:
    strncpy( pstr, "DWELL", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case SKIP:
    strncpy( pstr, "SKIP", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  default:
    assert( FALSE );
  }
  return r;
}
   
BOOL eq_st_pltb ( ATTR_ST_PLTB_PTR p1, ATTR_ST_PLTB_PTR p2 ) {
  assert( p1 );
  assert( p2 );
  assert( p1->kind == PAR_ST_PLTB );
  assert( p2->kind == PAR_ST_PLTB );
  BOOL r = FALSE;
  
  if( ! strncmp( p1->st.name, p2->st.name, MAX_STNAME_LEN ) )
    if( ! strncmp( p1->pltb.id, p2->pltb.id, MAX_PLTB_NAMELEN ) )
      r = TRUE;
  return r;
}

BOOL eq_st_pltb_pair ( ATTR_ST_PLTB_ORGDST_PTR pp1, ATTR_ST_PLTB_ORGDST_PTR pp2 ) {
  assert( pp1 );
  assert( pp2 );
  assert( pp1->kind == PAR_ST_PLTB_ORGDST );
  assert( pp2->kind == PAR_ST_PLTB_ORGDST );
  BOOL r = FALSE;
  
  if( ! eq_st_pltb( &pp1->st_pltb_org, &pp2->st_pltb_org ) )
    if( ! eq_st_pltb( &pp1->st_pltb_dst, &pp2->st_pltb_dst ) )
      r = TRUE;
  return r;
}

static BOOL ident_trips ( ATTR_ST_PLTB_ORGDST_PTR pt1, ATTR_ST_PLTB_ORGDST_PTR pt2 ) {
  assert( pt1 );
  assert( pt2 );
  assert( pt1->kind == PAR_ST_PLTB_ORGDST );
  assert( pt2->kind == PAR_ST_PLTB_ORGDST );
  
  BOOL r = FALSE;
  r = eq_st_pltb_pair( pt1, pt2 );
  
  return r;
}

ATTR_TRIP_PTR reg_trip_def ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip ) {
  assert( preg_tbl );
  assert( ptrip );
  assert( preg_tbl->kind == PAR_TRIPS );
  assert( ptrip->attr_st_pltb_orgdst.kind == PAR_ST_PLTB_ORGDST );
  BOOL ovw = FALSE;
  ATTR_TRIP_PTR r = NULL;
  
  int i;  
  for( i = 0; i < preg_tbl->ntrips; i++ ) {
    assert( i < preg_tbl->ntrips );
    ATTR_TRIP_PTR pprof = &preg_tbl->trip_prof[i];
    assert( pprof );
    assert( pprof->attr_st_pltb_orgdst.kind == PAR_ST_PLTB_ORGDST );
    if( ! ident_trips( &pprof->attr_st_pltb_orgdst, &ptrip->attr_st_pltb_orgdst ) ) {
      if( pobsolete ) {
	*pobsolete = *pprof;	
	r = pobsolete;
#if 1 // *****	
	preg_tbl->trip_prof[i].attr_sp_orgdst = ptrip->attr_sp_orgdst;
#else	
	{
	  assert( pprof->attr_st_pltb_orgdst.st_pltb_org.st.name );
	  assert( pprof->attr_st_pltb_orgdst.st_pltb_org.pltb.id );
	  ST_PLTB_PAIR stpl_org = { str2_st_id(pprof->attr_st_pltb_orgdst.st_pltb_org.st.name), str2_pltb_id(pprof->attr_st_pltb_orgdst.st_pltb_org.pltb.id) };
	  assert( pprof->attr_st_pltb_orgdst.st_pltb_dst.st.name );
	  assert( pprof->attr_st_pltb_orgdst.st_pltb_dst.pltb.id );
	  ST_PLTB_PAIR stpl_dst = { str2_st_id(pprof->attr_st_pltb_orgdst.st_pltb_dst.st.name), str2_pltb_id(pprof->attr_st_pltb_orgdst.st_pltb_dst.pltb.id) };
	  {
	    const STOPPING_POINT_CODE sp_org = lkup_spcode( &stpl_org );
	    const STOPPING_POINT_CODE sp_dst = lkup_spcode( &stpl_dst );
	    if( sp_org == SP_NONSENS ) {
	      printf( "FATAL: undefined origin st/pl:(%s, %s) stopping-point claimed in trip definition at (LINE, COL) = (%d, %d).\n",
		      pprof->attr_st_pltb_orgdst.st_pltb_org.st.name, pprof->attr_st_pltb_orgdst.st_pltb_org.pltb.id,
		      pprof->attr_st_pltb_orgdst.st_pltb_org.st.pos.row, pprof->attr_st_pltb_orgdst.st_pltb_org.st.pos.col );
	      err_stat.sem.sp_undefined = TRUE;
	    }
	    preg_tbl->trip_prof[i].attr_sp_orgdst.sp_org = sp_org;
	    if( sp_dst == SP_NONSENS ) {
	      printf( "FATAL: undefined destination st/pl:(%s, %s) stopping-point claimed in trip definition at (LINE, COL) = (%d, %d).\n",
		      pprof->attr_st_pltb_orgdst.st_pltb_dst.st.name, pprof->attr_st_pltb_orgdst.st_pltb_dst.pltb.id,
		      pprof->attr_st_pltb_orgdst.st_pltb_dst.st.pos.row, pprof->attr_st_pltb_orgdst.st_pltb_dst.st.pos.col );
	      err_stat.sem.sp_undefined = TRUE;
	    }
	    preg_tbl->trip_prof[i].attr_sp_orgdst.sp_dst = sp_dst;
	  }
	}
#endif
	pprof->attr_route_ctrl = ptrip->attr_route_ctrl;
      } else
	printf( "NOTICE: failed in redefinition the trip of.\n" );
      ovw = TRUE;
    }
  }
  if( !ovw ) {
    assert( i == preg_tbl->ntrips );
    if( i < MAX_TRIPS_DECL ) {
      preg_tbl->trip_prof[i] = *ptrip;
      preg_tbl->ntrips++;
      r = ptrip;
    } else {
      printf( "FATAL: trip definition has exhausted.\n" );
      exit( 1 );
    }
  }
  return r;
}

ATTR_SP_ASGN_PTR reg_spasgn ( ATTR_SP_ASGNS_PTR preg_tbl, ATTR_SP_ASGN_PTR pprev_asgn, ATTR_SP_ASGN_PTR pasgn ) {
  assert( preg_tbl );
  assert( pasgn );
  assert( preg_tbl->kind == PAR_SP_ASGNS );
  assert( pasgn->kind == PAR_SP_ASGN );
  BOOL ovw = FALSE;
  ATTR_SP_ASGN_PTR r = NULL;
  
  int i;
  for( i = 0; i < preg_tbl->nasgns; i++ ) {
    assert( preg_tbl->pltb_sp_asgns[i].kind == PAR_SP_ASGN );
    if( eq_st_pltb( &preg_tbl->pltb_sp_asgns[i].st_pltb, &pasgn->st_pltb ) ) {      
      if( pprev_asgn ) {
	*pprev_asgn = preg_tbl->pltb_sp_asgns[i];
	r = pprev_asgn;
	preg_tbl->pltb_sp_asgns[i] = *pasgn;
      } else
	printf( "NOTICE: failed in redefinition the st/pl & stopping-point assignment of.\n" );
      ovw = TRUE;
    }
  }
  if( !ovw ) {
    assert( i == preg_tbl->nasgns );
    if( i < MAX_SP_ASGNMENTS ) {
      preg_tbl->pltb_sp_asgns[i] = *pasgn;
      preg_tbl->nasgns++;
      r = pasgn;
    } else {
      printf( "FATAL: st/pl & stopping-point assignments has exhausted.\n" );
      exit( 1 );
    }
  }
  return r;
}

ATTR_JR_ASGN_PTR reg_jrasgn ( ATTR_JR_ASGNS_PTR preg_tbl, ATTR_JR_ASGN_PTR pprev_asgn, ATTR_JR_ASGN_PTR pasgn ) {
  assert( preg_tbl );
  assert( pasgn );
  assert( preg_tbl->kind == PAR_JR_ASGNS );
  assert( pasgn->kind == PAR_JR_ASGN );
  BOOL ovw = FALSE;
  ATTR_JR_ASGN_PTR r = NULL;
  
  int i;
  for( i = 0; i < preg_tbl->nasgns; i++ ) {
    assert( preg_tbl->jr_asgn[i].kind == PAR_JR_ASGN );
    if( preg_tbl->jr_asgn[i].journey_id.jid == pasgn->journey_id.jid ) {
      if( pprev_asgn ) {
	*pprev_asgn = preg_tbl->jr_asgn[i];
	r = pprev_asgn;
	preg_tbl->jr_asgn[i] = *pasgn;
      } else
	printf( "NOTICE: failed in redefinition the rake-journey assignment of.\n" );
      ovw = TRUE;
    }
  }
  if( !ovw ) {
    assert( i == preg_tbl->nasgns );
    if( i < MAX_JR_ASGNMENTS ) {
      preg_tbl->jr_asgn[i] = *pasgn;
      preg_tbl->nasgns++;
      r = pasgn;
    } else {
      printf( "FATAL: rake-journey assignments has exhausted.\n" );
      exit( 1 );
    }
  }
  return r;
}

static BOOL next2_pred( ATTR_TRIP_PTR ppred, ATTR_TRIP_PTR psucc ) {
  assert( ppred );
  assert( psucc );
  BOOL r = FALSE;
  ;
  r = TRUE;
  return r;
}

ATTR_TRIP_PTR reg_trip_journey ( ATTR_JOURNEYS_PTR preg_tbl, JOURNEY_ID jid, SRC_POS_PTR ppos, ATTR_TRIP_PTR ptrip ) {
  assert( preg_tbl );
  assert( preg_tbl->kind == PAR_JOURNEYS );
  assert( (jid >= 1) && (jid <= MAX_JOURNEYS) );
  assert( ppos );
  assert( ptrip->kind == PAR_TRIP );
  assert( ptrip );
  assert( ptrip->kind == PAR_TRIP );
  
  ATTR_JOURNEY_PTR pJ = &preg_tbl->journey_prof[jid];
  assert( pJ );
  int nts = pJ->trips.ntrips;  
  if( pJ->kind == PAR_JOURNEY ) {
    assert( pJ->journey_id.jid == jid );
    assert( pJ->trips.kind == PAR_TRIPS );
    assert( nts > 0 );
    if( nts < MAX_JOURNEY_TRIPS ) {
      if( ! next2_pred( &pJ->trips.trip_prof[nts - 1], ptrip ) )
	printf( "NOTICE: Undefined trip found on journey %d.\n", jid );
      pJ->trips.trip_prof[nts] = *ptrip;
      pJ->trips.ntrips++;
    } else {
      printf( "FATAL: trips of the journey %d has exhausted.\n", jid );
      exit( 1 );
    }
  } else {
    assert( ! pJ->kind );
    assert( nts == 0 );
    pJ->kind = PAR_JOURNEY;
    pJ->journey_id.jid = jid;
    pJ->journey_id.pos = *ppos;
    pJ->trips.kind = PAR_TRIPS;
    pJ->trips.trip_prof[nts] = *ptrip;
    pJ->trips.ntrips = 1;
    
  }
  return &pJ->trips.trip_prof[nts];
}

static void print_crewid ( CREW_ID crewid ) {
  assert( ((int)crewid > -1) && ((int)crewid < (int)END_OF_CREWIDs) );
  const char *str = cnv2str_crew_id[crewid];
  printf( "%s", (str ? str : "unknown") );
}

static void print_revenue ( BOOL revenue ) {
  if( revenue )
    printf( "revenue" );
  else
    printf( "nonreve" );
}

static void print_perfreg ( PERFREG_LEVEL perfreg ) {
  switch( perfreg ) {
  case PERFREG_SLOW:
    printf( "perfreg_slow" );
    break;
  case PERFREG_NORMAL:
    printf( "perfreg_norm" );
    break;
  case PERFREG_FAST:
    printf( "perfreg_fast" );
    break;
  case END_OF_PERFREG:
    /* fall thru. */
  default:
    assert( FALSE );
  }
}

static void print_dwell ( ARS_SP_COND spcond, TIME_DIFF dwell ) {
  assert( dwell > -1 );
  switch( spcond ) {
  case DWELL:
    //assert( dwell > 0 );
    printf( "dw:" );
    printf( "%d", dwell );
    break;
  case SKIP:
    //assert( dwell == 0 );
    printf( "ss:" );
    printf( "%d", dwell );
    break;
  default:
    assert( FALSE );
  }
}

static void print_sp_pair ( SP_ORGDST_PAIR_PTR psps ) {
  assert( psps );
  const char *sp_o = cnv2str_sp_code( psps->sp_org );
  const char *sp_d = cnv2str_sp_code( psps->sp_dst );
  printf( "(%s, %s)", (sp_o ? sp_o : "unknown"), (sp_d ? sp_d : "unknown") );
  
}

static void print_st_pltb ( ST_PLTB_PAIR_PTR pst_pltb ) {
  assert( pst_pltb );
  
  char st_pltb_strbuf[(MAX_STNAME_LEN + 1 + MAX_PLTB_NAMELEN) + 1] = "";
  {
    const char *str = cnv2str_st_id( pst_pltb->st );
    strncat( st_pltb_strbuf, (str ? str : "unknown"), MAX_STNAME_LEN );
  }
  
  strncat( st_pltb_strbuf, "_", 2 );
  {
    const char *str = cnv2str_pltb_id( pst_pltb->pltb );
    strncat( st_pltb_strbuf, (str ? str : "unknown"), MAX_PLTB_NAMELEN );
  }
  printf( "%s", st_pltb_strbuf );
}

static void print_routes ( ROUTE_ASSOC routes[], int nroutes ) {
  assert( routes );
  assert( nroutes > -1 );
  int i = 0;
  
  printf( "{" );
  while( i < nroutes ) {
    assert( routes[i].pprof );
    assert( routes[i].id == routes[i].pprof->id );
    printf( "%s", cnv2str_il_sym( routes[i].id ) );
    i++;
    if( i < nroutes )
      printf( ", " );
  }
  printf( "}" );
}

void print_time_desc ( TINY_TIME_DESC_PTR ptd ) {
  assert( ptd );
  printf( "%02d", ptd->hour );
  printf( ":" );
  printf( "%02d", ptd->minute );
  printf( ":" );
  printf( "%02d", ptd->second );
}
static void print_arrdep_time ( TIME_ARRDEP_PTR parrdep_time ) {
  assert( parrdep_time );
  
  printf( "(" );
  print_time_desc ( &parrdep_time->time_arr );
  printf( ", " );
  print_time_desc ( &parrdep_time->time_dep );
  printf( ")" );
}

void ttc_print_spasgns ( SP_ASGN spasgns[], int spasngs ) {
  assert( spasgns );
  int i;
  printf( "spasgns:\n" );
  for( i = 0; i < spasngs; i++ ) {
    TTC_DIAG_INDENT(1);
    printf( "(" );
    print_st_pltb( &spasgns[i].st_pltb );
    {
      const char *sp = cnv2str_sp_code( spasgns[i].sp );
      assert( sp );
      printf( ", %s", sp );
    }
    printf( ")\n" );
  }
}

void ttc_print_jrasgns ( JOURNEY_RAKE_ASGN rjasgns[], int nasgns ) {
  assert( rjasgns );
  int i;
  printf( "jrasgns:\n" );
  for( i = 0; i < nasgns; i++ ) {
    TTC_DIAG_INDENT(1);
    printf( "(J%03d, ", rjasgns[i].jid );
    printf( "rake_%d)\n", rjasgns[i].rake_id );
  }
}

void ttc_print_trips ( TRIP_DESC trips[], int ntrips ) {
  assert( trips );
  assert( ntrips > -1 ); 
  int i;
  
  printf( "trips:\n" );
  for( i = 0; i < ntrips; i++ ) {
    TTC_DIAG_INDENT(1);
    printf( "(" );
    
    printf( "(" );
    print_st_pltb( &trips[i].st_pltb_orgdst.org );
    printf( ", " );
    print_st_pltb( &trips[i].st_pltb_orgdst.dst );
    printf( "), " );
    
    print_sp_pair( &trips[i].sp_orgdst );
    printf( ", " );
    
    print_routes( trips[i].routes, trips[i].num_routes );
    printf( ")\n" );
  }
}

static void ttc_print_jtrip( JOURNEY_TRIP_PTR pjtrip ) {
  assert( pjtrip );
  printf( "(" );
  
  printf( "(" );
  print_st_pltb( &pjtrip->st_pltb_orgdst.org );
  printf( ", " );
  print_st_pltb( &pjtrip->st_pltb_orgdst.dst );
  printf( "), " );
  
  print_dwell( pjtrip->sp_cond.stop_skip, pjtrip->sp_cond.dwell_time );
  printf( ", " );
  
  print_arrdep_time( &pjtrip->time_arrdep );
  printf( ", " );
  
  print_perfreg( pjtrip->perfreg );
  printf( ", " );
  
  print_revenue( pjtrip->is_revenue );
  printf( ", " );
  
  print_crewid( pjtrip->crew_id );
  printf( ");\n" );
}

void ttc_print_journeys( JOURNEY_DESC journeys[], int njourneys ) {
  assert( journeys );
  assert( njourneys > -1 );
  int cnt_j = 0;
  
  int i = 0;
  printf( "journeys:\n" );
  while( i < MAX_JOURNEYS ) {
    if( cnt_j >= njourneys )
      break;
    if( journeys[i].jid > -1 ) {
     int k;
     TTC_DIAG_INDENT(1);
      printf( "J%03d:\n", journeys[i].jid );
      for( k = 0; k < journeys[i].num_trips; k++ ) {
	TTC_DIAG_INDENT(2);
	ttc_print_jtrip( &journeys[i].trips[k] );
      }
      TTC_DIAG_INDENT(1);
      printf( "scheduled commands of J%03d:\n", journeys[i].jid );
      ttc_print_schcmds( journeys[i].pschcmds_journey, 2 );
      cnt_j++;
    }
    i++;
  }
}

static int cons_st_pltb_pair ( ST_PLTB_ORGDST_PTR st_pltb_ref[], const int reftbl_len, const int refs_lim, ATTR_ST_PLTB_ORGDST_PTR pattr_orgdst, const BOOL ovwt ) {
  assert( st_pltb_ref );
  assert( reftbl_len > -1 );
  assert( refs_lim > -1 );
  assert( pattr_orgdst );
  assert( pattr_orgdst->kind == PAR_ST_PLTB_ORGDST ); 
  int r = -1;
  
  assert( pattr_orgdst->st_pltb_org.kind == PAR_ST_PLTB );
  assert( pattr_orgdst->st_pltb_dst.kind == PAR_ST_PLTB );
  ST_ID st_org = END_OF_ST_ID;
  ST_ID st_dst = END_OF_ST_ID;    
  PLTB_ID pltb_org = END_OF_PLTB_ID;
  PLTB_ID pltb_dst = END_OF_PLTB_ID;
  
  st_org = str2_st_id( pattr_orgdst->st_pltb_org.st.name );
  pltb_org = str2_pltb_id( pattr_orgdst->st_pltb_org.pltb.id );
  assert( st_org != END_OF_ST_ID );
  assert( pltb_org != END_OF_PLTB_ID );
  
  st_dst = str2_st_id( pattr_orgdst->st_pltb_dst.st.name );
  pltb_dst = str2_pltb_id( pattr_orgdst->st_pltb_dst.pltb.id );
  assert( st_dst != END_OF_ST_ID );
  assert( pltb_dst != END_OF_PLTB_ID );
  {
    BOOL ovrid = FALSE;
    int i;
    assert( timetbl_dataset.trips_decl.num_trips <= MAX_TRIPS_DECL );
    
    for( i = 0; i < reftbl_len; i++ ) {
      if( i >= refs_lim ) {
	assert( (int)(st_pltb_ref[i]->org.st) == 0 );
	break;
      } else {
	assert( i < refs_lim );
	assert( (int)(st_pltb_ref[i]->org.st) > 0 );
	if( ((st_pltb_ref[i]->org.st == st_org) && (st_pltb_ref[i]->org.pltb == pltb_org)) &&
	    ((st_pltb_ref[i]->dst.st == st_dst) && (st_pltb_ref[i]->dst.pltb == pltb_dst)) ) {
	  ovrid = TRUE;
	  break;
	}
      }
    }
    if( i < reftbl_len ) {
      assert( refs_lim < reftbl_len);
      r = i;
      if( ovrid && ovwt ) {
	assert( r < refs_lim );
	assert( (int)(st_pltb_ref[r]->org.st) > 0 );
	printf( "NOTICE: trip definition overridden at (LINE, COL) = (%d, %d).\n", pattr_orgdst->st_pltb_org.st.pos.row, pattr_orgdst->st_pltb_org.st.pos.col );
      } else {
	assert( r == refs_lim );
	assert( (int)(st_pltb_ref[r]->org.st) == 0 );
      }
      st_pltb_ref[r]->org.st = st_org;
      st_pltb_ref[r]->org.pltb = pltb_org;
      st_pltb_ref[r]->dst.st = st_dst;
      st_pltb_ref[r]->dst.pltb = pltb_dst;
    } else {
      assert( refs_lim == reftbl_len );
      printf( "FATAL: memory exthausted on trip registration.\n" );
      exit( 1 );
    }
  }
  return r;
}

static ROUTE_C_PTR trip_route_prof ( ATTR_ROUTE_PTR pattr_routes ) {
  assert( pattr_routes );
  assert( pattr_routes->kind == PAR_ROUTE );
  ROUTE_C_PTR pprof = NULL;

  assert( pattr_routes->name );
  pprof = conslt_route_prof_s( pattr_routes->name );
  return pprof;
}

static int cons_trip_routes ( ROUTE_ASSOC_PTR ptrip_routes, ATTR_ROUTES_PTR pattr_routes ) {
  assert( ptrip_routes );
  assert( pattr_routes );
  int nroutes = -1;
  
  BOOL err = FALSE;
  int i;
  assert( MAX_TRIP_ROUTES > 0 );
  assert( pattr_routes->nroutes <= MAX_TRIP_ROUTES );
  for( i = 0; (i < MAX_TRIP_ROUTES) && !err; i++ ) {    
    assert( nroutes <= MAX_TRIP_ROUTES );
    if( i >= pattr_routes->nroutes ) {     
      assert( ! ptrip_routes[i].pprof );
      break;
    } else {
      ROUTE_C_PTR pprof = NULL;
      pprof = trip_route_prof( &pattr_routes->route_prof[i] );
      if( pprof ) {
	int j;
	for( j = 0; j < i; j++ )
	  if( ptrip_routes[j].pprof == pprof ) {
	    printf( "FATAL: route redefinition in trip definition at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	    err_stat.sem.route_redef = TRUE;
	    err = TRUE;
	    break;
	  }
	if( !err ) {
	  ptrip_routes[i].id = pprof->id;
	  ptrip_routes[i].pprof = pprof;
	  nroutes = (nroutes < 0) ? 1 : (assert( nroutes >= 1), nroutes + 1);
	}
      } else {
	printf( "FATAL: undefined route found in trip definition at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	err_stat.sem.unknown_route = TRUE;
	err = TRUE;
      }
    }
  }
  if( nroutes > 0 )
    nroutes *= (err ? -1 : 1);
  return nroutes;
}

static SP_ORGDST_PAIR_PTR cons_orgdst_sp_pair ( SP_ORGDST_PAIR_PTR ptrip_sps, ATTR_SP_PAIR_PTR pattr_sps ) {
  assert( ptrip_sps );
  assert( pattr_sps );
  assert( pattr_sps->kind == PAR_SP_PAIR );
  STOPPING_POINT_CODE sp_org = END_OF_SPs;
  STOPPING_POINT_CODE sp_dst = END_OF_SPs;
  
  sp_org = str2_sp_code( pattr_sps->org.sp_id );
  sp_dst = str2_sp_code( pattr_sps->dst.sp_id );
  assert( sp_org != END_OF_SPs );
  assert( sp_dst != END_OF_SPs );
  ptrip_sps->sp_org = sp_org;
  ptrip_sps->sp_dst = sp_dst;
  
  return ptrip_sps;
}

void cons_trips ( ATTR_TRIPS_PTR ptrips ) {
  assert( ptrips );
  assert( ptrips->kind == PAR_TRIPS );
  ST_PLTB_ORGDST_PTR st_pltb_ref[MAX_TRIPS_DECL] = {};
  
  int i;
  assert( timetbl_dataset.trips_decl.num_trips == 0 );
  {
    int j = 0;
    while( j < MAX_TRIPS_DECL ) {
      st_pltb_ref[j] = &timetbl_dataset.trips_decl.trips[j].st_pltb_orgdst;
      assert( (TRIP_DESC_PTR)st_pltb_ref[j] == &timetbl_dataset.trips_decl.trips[j] );
      j++;
    }
    assert( j == MAX_TRIPS_DECL );
  }
  assert( ptrips->ntrips >= 0 );
  for( i = 0; i < ptrips->ntrips; i++ ) {
    assert( ptrips->trip_prof[i].kind == PAR_TRIP );
    int newone = -1;
    newone = cons_st_pltb_pair( st_pltb_ref, MAX_TRIPS_DECL, timetbl_dataset.trips_decl.num_trips, &ptrips->trip_prof[i].attr_st_pltb_orgdst, TRUE );
    if( newone > -1 ) {
      assert( timetbl_dataset.trips_decl.num_trips == newone );
      TRIP_DESC_PTR pT = (TRIP_DESC_PTR)st_pltb_ref[newone];
      assert( pT );      
      cons_orgdst_sp_pair( &pT->sp_orgdst, &ptrips->trip_prof[i].attr_sp_orgdst );
      pT->running_time = ptrips->trip_prof[i].running_time;
      {
	int nroutes = -1;
	pT->num_routes = -1;
	nroutes = cons_trip_routes( pT->routes, &ptrips->trip_prof[i].attr_route_ctrl );
	if( nroutes > -1 )
	  pT->num_routes = nroutes;
      }
      timetbl_dataset.trips_decl.num_trips++;
    }
  }
}

STOPPING_POINT_CODE lkup_spcode ( ST_PLTB_PAIR_PTR pst_pl ) {
  assert( pst_pl );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  int i;
  for( i = 0; i < timetbl_dataset.sp_asgns.num_asgns; i++ ) {
    if( (timetbl_dataset.sp_asgns.spasgns[i].st_pltb.st == pst_pl->st) && (timetbl_dataset.sp_asgns.spasgns[i].st_pltb.pltb == pst_pl->pltb ) ) {
      r = timetbl_dataset.sp_asgns.spasgns[i].sp;
      break;
    }
  }
  return r;
}

TRIP_DESC_PTR lkup_trip ( ST_PLTB_PAIR_PTR porg, ST_PLTB_PAIR_PTR pdst ) {  
  assert( porg );
  assert( pdst );
  TRIP_DESC_PTR r = NULL;
  
  int i;  
  for( i = 0; i< timetbl_dataset.trips_decl.num_trips; i++ ) {
    TRIP_DESC_PTR pT = &timetbl_dataset.trips_decl.trips[i];
    assert( pT );
    if( ((pT->st_pltb_orgdst.org.st == porg->st) && (pT->st_pltb_orgdst.org.pltb == porg->pltb)) &&
	((pT->st_pltb_orgdst.dst.st == pdst->st) && (pT->st_pltb_orgdst.dst.pltb == pdst->pltb)) ) {
      r = pT;
      break;
    }
  }
  return r;
}

void cons_spasgn ( ATTR_SP_ASGNS_PTR pspasgns ) {
  assert( pspasgns );
  assert( pspasgns->kind == PAR_SP_ASGNS );
  int cnt;
  
  int i;
  for( cnt = 0, i = 0; (i < MAX_SP_ASGNMENTS) && (cnt < pspasgns->nasgns); i++ ) {
    assert( cnt < MAX_SP_ASGNMENTS );
    ATTR_SP_ASGN_PTR pa = NULL;
    pa = &pspasgns->pltb_sp_asgns[i];
    assert( pa );
    assert( pa->sp.sp_id );
    if( pa->kind == PAR_SP_ASGN ) {
      int k;
      for( k = 0; k < cnt; k++ ) {
	assert( k < cnt );
	const char *st = cnv2str_st_id( timetbl_dataset.sp_asgns.spasgns[k].st_pltb.st );
	assert( st );
	if( ! strncmp( st, pa->st_pltb.st.name, MAX_STNAME_LEN ) ) {
	  const char *pltb = cnv2str_pltb_id( timetbl_dataset.sp_asgns.spasgns[k].st_pltb.pltb );
	  assert( pltb );
	  if( ! strncmp( pltb, pa->st_pltb.pltb.id, MAX_PLTB_NAMELEN ) ) {
	    printf( "NOTICE: st/pl & sp assignment overridden at (LINE, COL) = (%d, %d).\n", pa->pos.row, pa->pos.col );
	    break;
	  }
	}
      }
      timetbl_dataset.sp_asgns.spasgns[k].st_pltb.st = str2_st_id ( pa->st_pltb.st.name );
      timetbl_dataset.sp_asgns.spasgns[k].st_pltb.pltb = str2_pltb_id( pa->st_pltb.pltb.id );
      timetbl_dataset.sp_asgns.spasgns[k].sp = str2_sp_code( pa->sp.sp_id );
      if( k >= cnt )
	cnt++;
    } else
      assert( pa->kind == PAR_UNKNOWN );
  }
  timetbl_dataset.sp_asgns.num_asgns = cnt;
}

static void cons_jrasgn ( ATTR_JR_ASGNS_PTR pjrasgns ) {
  assert( pjrasgns );
  assert( pjrasgns->kind == PAR_JR_ASGNS );
  int cnt;
  
  int i;
  for( cnt = 0, i = 0; (i < MAX_JR_ASGNMENTS) && (cnt < pjrasgns->nasgns); i++ ) {
    assert( cnt < MAX_JR_ASGNMENTS );
    ATTR_JR_ASGN_PTR pa = NULL;
    pa = &pjrasgns->jr_asgn[i];
    assert( pa );
    if( pa->kind == PAR_JR_ASGN ) {
      int k;
      for( k = 0; k < cnt; k++ ) {
	assert( k < cnt );
	if( timetbl_dataset.jr_asgns.jrasgns[k].jid == pa->journey_id.jid ) {
	  printf( "NOTICE: journey-rake assignment overridden at (LINE, COL) = (%d, %d).\n", pa->journey_id.pos.row, pa->journey_id.pos.col );
	  break;
	}
      }
      timetbl_dataset.jr_asgns.jrasgns[k].jid = pa->journey_id.jid;
      timetbl_dataset.jr_asgns.jrasgns[k].rake_id = pa->rake_id.rid;
      if( k >= cnt )
	cnt++;
    } else
      assert( pa->kind == PAR_UNKNOWN );
  }
  timetbl_dataset.jr_asgns.num_asgns = cnt;
}

static void jtrip_arrdep_time ( JOURNEY_TRIP_PTR pjtrip_prev, JOURNEY_TRIP_PTR pjtrip, ATTR_TRIP_PTR ppar_trip ) {
  assert( pjtrip );
  assert( ppar_trip );
  
  assert( pjtrip->sp_cond.dwell_time == ppar_trip->sp_cond.dwell_time );
  assert( pjtrip->sp_cond.stop_skip == ppar_trip->sp_cond.stop_skip );
  
  struct {
    BOOL dep_ovrid;
    time_t t_arr0;
    TINY_TIME_DESC a;
    TINY_TIME_DESC d;
  } arr_dep = { FALSE };
  
  const time_t t_now = time( NULL );
  struct tm *ptm_now = NULL;
  struct tm tm_arr = {};
  struct tm tm_dep = {};
  time_t t_arr;
  time_t t_dep;
  
  ptm_now = localtime( &t_now );
  assert( ptm_now );
  tm_arr = *ptm_now;
  tm_dep = *ptm_now;
  if( !pjtrip_prev ) {
    assert( !arr_dep.t_arr0 );
    struct tm *ptm_arr = NULL;
    struct tm *ptm_dep = NULL;    
  arrdep_time_calc:
    if( ppar_trip->arrdep_time.arriv.arr_time.t.hour > -1 ) {
      assert( ppar_trip->arrdep_time.arriv.arr_time.t.minute >= 0 );
      assert( ppar_trip->arrdep_time.arriv.arr_time.t.second >= 0 );      
      tm_arr.tm_hour = ppar_trip->arrdep_time.arriv.arr_time.t.hour;
      tm_arr.tm_min = ppar_trip->arrdep_time.arriv.arr_time.t.minute;
      tm_arr.tm_sec = ppar_trip->arrdep_time.arriv.arr_time.t.second;
      t_arr = mktime( &tm_arr );
      if( arr_dep.t_arr0 ) {
	const int diff_arr = (int)t_arr - (int)arr_dep.t_arr0;
	if( abs( diff_arr ) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE ) {
	  printf( "NOTICE: mismatched arrival time with the departure time from its previous station, at (LINE, COL) = (%d, %d).\n",
		  ppar_trip->arrdep_time.arriv.arr_time.pos.row, ppar_trip->arrdep_time.arriv.arr_time.pos.col );
	  if( diff_arr < 0 ) {
	    printf( "FATAL: arrival time overridden by previous stations departure and running time, at (LINE, COL) = (%d, %d).\n",
		    ppar_trip->arrdep_time.arriv.arr_time.pos.row, ppar_trip->arrdep_time.arriv.arr_time.pos.col );
	    assert( ((int)arr_dep.t_arr0 - (int)t_arr) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE );
	    t_arr = arr_dep.t_arr0;
	    err_stat.sem.inconsistent_arrtime_overdn = TRUE;
	  }
	}
      }
      if( ppar_trip->arrdep_time.dept.dep_time.t.hour > -1 ) {
	assert( ppar_trip->arrdep_time.dept.dep_time.t.minute >= 0 );
	assert( ppar_trip->arrdep_time.dept.dep_time.t.second >= 0 );
	tm_dep.tm_hour = ppar_trip->arrdep_time.dept.dep_time.t.hour;
	tm_dep.tm_min = ppar_trip->arrdep_time.dept.dep_time.t.minute;
	tm_dep.tm_sec = ppar_trip->arrdep_time.dept.dep_time.t.second;
	t_dep = mktime( &tm_dep );
	switch( pjtrip->sp_cond.stop_skip ) {
	case DWELL:
	  {
	    const int diff_dw = (int)(t_dep - (t_arr + (time_t)pjtrip->sp_cond.dwell_time));
	    if( abs( diff_dw ) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE ) {
	      printf( "NOTICE: mismatched departure time with its dwell at (LINE, COL) = (%d, %d).\n",
		      ppar_trip->arrdep_time.dept.dep_time.pos.row, ppar_trip->arrdep_time.dept.dep_time.pos.col );
	      if( diff_dw < 0 ) {
		printf( "FATAL: deparure time overridden by its arrival and dwell time, at (LINE, COL) = (%d, %d).\n",
			ppar_trip->arrdep_time.dept.dep_time.pos.row, ppar_trip->arrdep_time.dept.dep_time.pos.col );
		t_dep = t_arr + (time_t)pjtrip->sp_cond.dwell_time;
		err_stat.sem.inconsistent_deptime_overdn = TRUE;
	      }
	    }
	  }	  
	  break;
	case SKIP:
	  if( abs( (int)t_dep - (int)t_arr ) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE ) {
	    printf( "NOTICE: mismatched departure time for skipping, at (LINE, COL) = (%d, %d).\n",
		    ppar_trip->arrdep_time.dept.dep_time.pos.row, ppar_trip->arrdep_time.dept.dep_time.pos.col );
	    t_dep = t_arr;
	    err_stat.sem.inconsistent_deptime_overdn = TRUE;
	  }
	  break;
	default:
	  assert( FALSE );
	}
      } else {
	assert( ppar_trip->arrdep_time.dept.dep_time.t.minute < 0 );
	assert( ppar_trip->arrdep_time.dept.dep_time.t.second < 0 );
	t_dep = t_arr + (time_t)pjtrip->sp_cond.dwell_time;
      }
      ptm_arr = localtime( &t_arr );
      assert( ptm_arr );
      arr_dep.a.hour = ptm_arr->tm_hour;
      arr_dep.a.minute = ptm_arr->tm_min;
      arr_dep.a.second = ptm_arr->tm_sec;
      ptm_dep = localtime( &t_dep );
      assert( ptm_dep );
      arr_dep.d.hour = ptm_dep->tm_hour;
      arr_dep.d.minute = ptm_dep->tm_min;
      arr_dep.d.second = ptm_dep->tm_sec;
    } else {
      assert( ppar_trip->arrdep_time.arriv.arr_time.t.minute < 0 );
      assert( ppar_trip->arrdep_time.arriv.arr_time.t.second < 0 );
      time_t t_arr_dw;      
      if( pjtrip_prev ) {
	assert( arr_dep.t_arr0 );
	t_arr = arr_dep.t_arr0;	
      } else {
	assert( !arr_dep.t_arr0 );
	printf( "FATAL: The first trip of journey must have arrival time to specify journey starting time, at (LINE, COL) = (%d, %d).\n",
		ppar_trip->attr_st_pltb_orgdst.st_pltb_org.pltb.pos.row, ppar_trip->attr_st_pltb_orgdst.st_pltb_org.pltb.pos.col );
	tm_arr.tm_hour = JOURNEY_DEFAULT_ARRTIME_HOUR;
	tm_arr.tm_min = JOURNEY_DEFAULT_ARRTIME_MINUTE;
	tm_arr.tm_sec = JOURNEY_DEFAULT_ARRTIME_SECOND;
	t_arr = mktime( &tm_arr );
	err_stat.sem.inconsistent_arrtime_overdn = TRUE;
      }
      t_arr_dw = t_arr + (time_t)pjtrip->sp_cond.dwell_time;
      if( ppar_trip->arrdep_time.dept.dep_time.t.hour > -1 ) {
	assert( ppar_trip->arrdep_time.dept.dep_time.t.minute >= 0 );
	assert( ppar_trip->arrdep_time.dept.dep_time.t.second >= 0 );
	tm_dep.tm_hour = ppar_trip->arrdep_time.dept.dep_time.t.hour;
	tm_dep.tm_min = ppar_trip->arrdep_time.dept.dep_time.t.minute;
	tm_dep.tm_sec = ppar_trip->arrdep_time.dept.dep_time.t.second;
	t_dep = mktime( &tm_dep );
	switch( pjtrip->sp_cond.stop_skip ) {
	case DWELL:
	  {
	    const int d = (int)t_dep - (int)t_arr_dw;
	    if( (abs( d ) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE) && (d < 0) ) {
	      printf( "FATAL: deparure time overridden with its arrival and dwell time, at (LINE, COL) = (%d, %d).\n",
		      ppar_trip->arrdep_time.dept.dep_time.pos.row, ppar_trip->arrdep_time.dept.dep_time.pos.col );
	      t_dep = t_arr_dw;
	      err_stat.sem.inconsistent_deptime_overdn = TRUE;
	    }	    
	  }
	  break;
	case SKIP:
	  if( abs( (int)t_dep - (int)t_arr_dw ) > JOURNEY_ARRDEP_TIME_ERR_NEGLECTABLE ) {
	    printf( "NOTICE: mismatched departure time for skipping, at (LINE, COL) = (%d, %d).\n",
		    ppar_trip->arrdep_time.dept.dep_time.pos.row, ppar_trip->arrdep_time.dept.dep_time.pos.col );
	    t_dep = t_arr_dw;
	    err_stat.sem.inconsistent_deptime_overdn = TRUE;
	  }
	  break;
	default:
	  assert( FALSE );
	}
      } else {
	assert( ppar_trip->arrdep_time.dept.dep_time.t.minute < 0 );
	assert( ppar_trip->arrdep_time.dept.dep_time.t.second < 0 );
	t_dep = t_arr_dw;
      }
      ptm_arr = localtime( &t_arr );
      assert( ptm_arr );
      arr_dep.a.hour = ptm_arr->tm_hour;
      arr_dep.a.minute = ptm_arr->tm_min;
      arr_dep.a.second = ptm_arr->tm_sec;
      ptm_dep = localtime( &t_dep );
      assert( ptm_dep );
      arr_dep.d.hour = ptm_dep->tm_hour;
      arr_dep.d.minute = ptm_dep->tm_min;
      arr_dep.d.second = ptm_dep->tm_sec;
    }
  } else {
    assert( pjtrip_prev );
    assert( pjtrip_prev->time_arrdep.time_arr.hour >= 0 );
    assert( pjtrip_prev->time_arrdep.time_arr.minute >= 0 );
    assert( pjtrip_prev->time_arrdep.time_arr.second >= 0 );
    
    assert( pjtrip_prev->time_arrdep.time_dep.hour >= 0 );
    assert( pjtrip_prev->time_arrdep.time_dep.minute >= 0 );
    assert( pjtrip_prev->time_arrdep.time_dep.second >= 0 );
    tm_arr.tm_hour = pjtrip_prev->time_arrdep.time_dep.hour;
    tm_arr.tm_min = pjtrip_prev->time_arrdep.time_dep.minute;
    tm_arr.tm_sec = pjtrip_prev->time_arrdep.time_dep.second;    
    t_arr = mktime( &tm_arr );
    if( !pjtrip_prev->deadend ) {
      if( pjtrip_prev->ptrip_prof )
	if( (pjtrip_prev->st_pltb_orgdst.dst.st == pjtrip->st_pltb_orgdst.org.st) && (pjtrip_prev->st_pltb_orgdst.dst.pltb == pjtrip->st_pltb_orgdst.org.pltb) )
	  t_arr += (time_t)(pjtrip_prev->ptrip_prof->running_time);
    }
    arr_dep.t_arr0 = t_arr;
    assert( arr_dep.t_arr0 );
    goto arrdep_time_calc;
  }
  pjtrip->time_arrdep.time_arr = arr_dep.a;
  pjtrip->time_arrdep.time_dep = arr_dep.d;
}

static void cons_journeys ( ATTR_JOURNEYS_PTR pjourneys ) {
  assert( pjourneys );
  assert( pjourneys->kind == PAR_JOURNEYS );
  ST_PLTB_ORGDST_PTR st_pltb_ref[MAX_JOURNEY_TRIPS] = {};
  
  int i = 0;  
  assert( pjourneys->njourneys >= 0 );
  assert( timetbl_dataset.j.num_journeys == 0 );
  while( i < MAX_JOURNEYS ) {
    if( timetbl_dataset.j.num_journeys >= pjourneys->njourneys )
      break;
    timetbl_dataset.j.journeys[i].jid = -1;
    if( pjourneys->journey_prof[i].journey_id.jid > 0 ) {
      timetbl_dataset.j.journeys[i].jid = pjourneys->journey_prof[i].journey_id.jid;
      assert( pjourneys->journey_prof[i].kind == PAR_JOURNEY );
      assert( timetbl_dataset.j.journeys[i].num_trips == 0 );
      {
	int k = 0;
	while( k < MAX_JOURNEY_TRIPS ) {
	  st_pltb_ref[k] = &timetbl_dataset.j.journeys[i].trips[k].st_pltb_orgdst;
	  assert( (JOURNEY_TRIP_PTR)st_pltb_ref[k] == &timetbl_dataset.j.journeys[i].trips[k] );
	  k++;
	}
	assert( k == MAX_JOURNEY_TRIPS );
      }
      {
	ATTR_JOURNEY_PTR pJ_par = &pjourneys->journey_prof[i];
	assert( pJ_par );
	assert( pJ_par->kind == PAR_JOURNEY );
	assert( pJ_par->trips.kind == PAR_TRIPS );
	struct {
	  BOOL deadend_found;;
	  SRC_POS prev_pos;
	} deadend_trip = {};
	JOURNEY_TRIP_PTR pJ_prev = NULL;
	int l;
	assert( !deadend_trip.deadend_found );
	for( l = 0; l < pJ_par->trips.ntrips; l++ ) {
	  assert( pJ_par->trips.trip_prof[l].kind == PAR_TRIP );
	  if( deadend_trip.deadend_found ) {
	    printf( "FATAL: invalid dead-end trip detected at (LINE, COL) = (%d, %d).\n", deadend_trip.prev_pos.row, deadend_trip.prev_pos.col );
	    err_stat.sem.contiguless_trips = TRUE;
	    deadend_trip.deadend_found = FALSE;
	  }
	  if( pJ_par->trips.trip_prof[l].deadend ) {
	    const int newone = timetbl_dataset.j.journeys[i].num_trips;
	    if( newone < MAX_JOURNEY_TRIPS ) {
	      JOURNEY_TRIP_PTR pJ = (JOURNEY_TRIP_PTR)st_pltb_ref[newone];
	      assert( pJ );
	      pJ->st_pltb_orgdst.org.st = str2_st_id( pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.name );
	      pJ->st_pltb_orgdst.org.pltb = str2_pltb_id( pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.pltb.id );
	      if( pJ_prev ) {
		if( !((pJ_prev->st_pltb_orgdst.dst.st == pJ->st_pltb_orgdst.org.st) && (pJ_prev->st_pltb_orgdst.dst.pltb == pJ->st_pltb_orgdst.org.pltb)) ) {
		  printf( "FATAL: contiguous lost in journey trips, at (LINE, COL) = (%d, %d).\n",
			  pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.row, pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.col );
		  err_stat.sem.contiguless_trips = TRUE;
		}
	      }
	      pJ->sp_cond.stop_skip = pJ_par->trips.trip_prof[l].sp_cond.stop_skip;
	      pJ->sp_cond.dwell_time = pJ_par->trips.trip_prof[l].sp_cond.dwell_time;  // settings for pJ->dwell_time;
	      assert( (pJ->sp_cond.stop_skip == DWELL) && (pJ->sp_cond.dwell_time == 0) );
	      pJ->deadend = TRUE;
	      jtrip_arrdep_time( pJ_prev, pJ, &pJ_par->trips.trip_prof[l] ); // settings for pJ->time_arrdep;
	      timetbl_dataset.j.journeys[i].num_trips++;
	      pJ_prev = pJ;
	      deadend_trip.deadend_found = TRUE;
	      deadend_trip.prev_pos = pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos;
	    } else {
	      assert( newone == MAX_JOURNEY_TRIPS );
	      printf( "FATAL: memory exthausted on trip registration.\n" );
	      exit( 1 );
	    }
	  } else {
	    int newone = -1;
	    newone = cons_st_pltb_pair( st_pltb_ref, MAX_JOURNEY_TRIPS, timetbl_dataset.j.journeys[i].num_trips, &pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst, FALSE );
	    if( newone > -1 ) {
	      assert( timetbl_dataset.j.journeys[i].num_trips == newone );
	      JOURNEY_TRIP_PTR pJ = (JOURNEY_TRIP_PTR)st_pltb_ref[newone];
	      assert( pJ );
	      pJ->ptrip_prof = lkup_trip( &pJ->st_pltb_orgdst.org, &pJ->st_pltb_orgdst.dst );
	      if( !pJ->ptrip_prof ) {
		printf( "FATAL: unknwon trip found in journey, at (LINE, COL) = (%d, %d).\n",
			pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.row, pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.col );
		err_stat.sem.unknown_trip = TRUE;
	      }
	      if( pJ_prev )
		if( !((pJ_prev->st_pltb_orgdst.dst.st == pJ->st_pltb_orgdst.org.st) && (pJ_prev->st_pltb_orgdst.dst.pltb == pJ->st_pltb_orgdst.org.pltb)) ) {
		  printf( "FATAL: contiguous lost in journey trips, at (LINE, COL) = (%d, %d).\n",
			  pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.row, pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst.st_pltb_org.st.pos.col );
		  err_stat.sem.contiguless_trips = TRUE;
		}
	      pJ->sp_cond.stop_skip = pJ_par->trips.trip_prof[l].sp_cond.stop_skip;
	      pJ->sp_cond.dwell_time = pJ_par->trips.trip_prof[l].sp_cond.dwell_time;  // settings for pJ->dwell_time;
	      assert( pJ->sp_cond.stop_skip == SKIP ? (pJ->sp_cond.dwell_time == 0) : ((pJ->sp_cond.stop_skip == DWELL) && (pJ->sp_cond.dwell_time > 0)) );
	      jtrip_arrdep_time( pJ_prev, pJ, &pJ_par->trips.trip_prof[l] ); // settings for pJ->time_arrdep;
	      pJ->perfreg = pJ_par->trips.trip_prof[l].perf_regime.perfreg_cmd; // settings for pJ->perfreg;
	      pJ->is_revenue = pJ_par->trips.trip_prof[l].revenue.stat; // settings for pJ->is_revenue;
	      // settings for pJ->crew_id;
	      if( pJ_par->trips.trip_prof[l].crew_id.cid < END_OF_CREWIDs ) {
		pJ->crew_id = (CREW_ID)pJ_par->trips.trip_prof[l].crew_id.cid;
	      } else {
		printf( "FATAL: undefined route found in journey declaration at (LINE, COL) = (%d, %d).\n",
			pJ_par->trips.trip_prof[l].crew_id.pos.row, pJ_par->trips.trip_prof[l].crew_id.pos.col );
		err_stat.sem.invalid_crewid = TRUE;
		pJ->crew_id = CREW_NO_ID;
	      }
	      timetbl_dataset.j.journeys[i].num_trips++;
	      pJ_prev = pJ;
	    }
	  }
	}
      }
      timetbl_dataset.j.num_journeys++;
    }
    i++;
  }
  assert( timetbl_dataset.j.num_journeys == pjourneys->njourneys );
}

int ttcreat ( void ) {
  extern int yyparse( void );
  extern FILE *yyin;
  
  const int err_result = 1;
  int r = 0;
  
  assert( !TTC_ERRSTAT_PAR( err_stat ) );
  yyin = stdin;
  if( yyparse() ) {
    r = err_result;
  } else {
    if( TTC_ERRSTAT_PAR( err_stat ) ) {
      r = err_result;
    }
  }
  
  assert( !TTC_ERRSTAT_SEM( err_stat ) );
  //cons_spasgn( &timetable_symtbl->sp_asgns );
  //cons_trips( &timetable_symtbl->trips_regtbl );
  if( err_stat.sem.route_redef ||
      err_stat.sem.unknown_route ) {
    r = err_result;
  }
  cons_jrasgn( &timetable_symtbl->jr_asgn_regtbl );
  cons_journeys( &timetable_symtbl->journeys_regtbl );
  cons_scheduled_cmds();
  
  printf( "\n" );
  ttc_print_spasgns( timetbl_dataset.sp_asgns.spasgns, timetbl_dataset.sp_asgns.num_asgns );
  printf( "\n" );
  ttc_print_trips( timetbl_dataset.trips_decl.trips, timetbl_dataset.trips_decl.num_trips );
  printf( "\n" );
  ttc_print_jrasgns( timetbl_dataset.jr_asgns.jrasgns, timetbl_dataset.jr_asgns.num_asgns );
  printf( "\n" );
  ttc_print_journeys( timetbl_dataset.j.journeys, timetbl_dataset.j.num_journeys );
  
  load_online_timetbl();
  
  return r;
}

#define SCHEDULED_CMDS_NODEBUFSIZ 1024
int main ( void ) {
  int r = -1;
  BOOL alloc = FALSE;
  
  scheduled_cmds.nodes = (SCHEDULED_COMMAND_PTR)calloc( (sizeof(SCHEDULED_COMMAND) * SCHEDULED_CMDS_NODEBUFSIZ), 1 );
  if( scheduled_cmds.nodes ) {
    scheduled_cmds.plim = &scheduled_cmds.nodes[SCHEDULED_CMDS_NODEBUFSIZ];
    timetable_symtbl = (ATTR_TIMETABLE_PTR)calloc( sizeof(ATTR_TIMETABLE), 1 );
    if( timetable_symtbl ) {
      timetable_symtbl->trips_regtbl.kind = PAR_UNKNOWN;
      timetable_symtbl->jr_asgn_regtbl.kind = PAR_UNKNOWN;
      timetable_symtbl->journeys_regtbl.kind = PAR_UNKNOWN;
      alloc = TRUE;
    }
  }
  if( !alloc ) {
    printf( "memory allocation failed.\n" );
    return r;
  }
  
  {
    int i;
    for( i = 0; i < MAX_JOURNEYS; i++ ) {
      timetable_symtbl->journeys_regtbl.journey_prof[i].journey_id.jid = -1;
    }
  }
  r = ttcreat();
  return r;
}

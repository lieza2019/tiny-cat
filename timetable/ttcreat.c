#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

ERR_STAT err_stat;
TIMETABLE_DATASET timetbl_dataset;

char *cnv2str_kind ( char *pstr, PAR_KIND kind, const int buflen ) {
  assert( pstr );
  assert( buflen > 0 );
  char *r = NULL;
  
  switch( kind ) {
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
  case PAR_RJ_ASGN:
    strncpy( pstr, "RJ_ASGN", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_RJ_ASGNS:
    strncpy( pstr, "RJ_ASGNS", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_JOURNEY:
    strncpy( pstr, "JOURNEY", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case PAR_UNKNOWN:
    strncpy( pstr, "UNKNOWN", (buflen - 1) );
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
    assert( preg_tbl->trip_prof[i].attr_st_pltb_orgdst.kind == PAR_ST_PLTB_ORGDST );
    if( ! ident_trips( &preg_tbl->trip_prof[i].attr_st_pltb_orgdst, &ptrip->attr_st_pltb_orgdst ) ) {
      if( pobsolete ) {
	*pobsolete = preg_tbl->trip_prof[i];
	r = pobsolete;
	preg_tbl->trip_prof[i].attr_sp_orgdst = ptrip->attr_sp_orgdst;
	preg_tbl->trip_prof[i].attr_route_ctrl = ptrip->attr_route_ctrl;
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

ATTR_RJ_ASGN_PTR reg_rjasgn ( ATTR_RJ_ASGNS_PTR preg_tbl, ATTR_RJ_ASGN_PTR pprev_asgn, ATTR_RJ_ASGN_PTR pasgn ) {
  assert( preg_tbl );
  assert( pasgn );
  assert( preg_tbl->kind == PAR_RJ_ASGNS );
  assert( pasgn->kind == PAR_RJ_ASGN );
  BOOL ovw = FALSE;
  ATTR_RJ_ASGN_PTR r = NULL;
  
  int i;
  for( i = 0; i < preg_tbl->nasgns; i++ ) {
    assert( i < preg_tbl->nasgns );
    assert( preg_tbl->rj_asgn[i].kind == PAR_RJ_ASGN );
    if( preg_tbl->rj_asgn[i].journey_id.jid == pasgn->journey_id.jid ) {
      if( pprev_asgn ) {
	*pprev_asgn = preg_tbl->rj_asgn[i];
	r = pprev_asgn;
	preg_tbl->rj_asgn[i] = *pasgn;
      } else
	printf( "NOTICE: failed in redefinition the rake-journey assignment of.\n" );
      ovw = TRUE;
    }
  }
  if( !ovw ) {
    assert( i == preg_tbl->nasgns );
    if( i < MAX_RJ_ASGNMENTS ) {
      preg_tbl->rj_asgn[i] = *pasgn;
      preg_tbl->nasgns++;
      r = pasgn;
    } else {
      printf( "FATAL: rake-journey assignments has exhausted.\n" );
      exit( 1 );
    }
  }
  return r;
}

static BOOL next2_pred ( ATTR_TRIP_PTR ppred, ATTR_TRIP_PTR psucc ) {
  assert( ppred );
  assert( psucc );
  BOOL r = FALSE;
  ;
  r = TRUE;
  return r;
}

#if 0
ATTR_TRIP_PTR reg_trip_journey ( ATTR_JOURNEYS_PTR preg_tbl, JOURNEY_ID jid, ATTR_TRIP_PTR ptrip ) {
  assert( preg_tbl );
  assert( preg_tbl->kind == JOURNEYS );
  assert( (jid >= 1) && (jid <= MAX_JOURNEYS) );
  assert( ptrip->kind == TRIP );
  assert( ptrip );
  assert( ptrip->kind == TRIP );
  
  ATTR_JOURNEY_PTR pJ = &preg_tbl->journey_prof[jid];
  assert( pJ );
  int nts = pJ->trips.ntrips;  
  if( pJ->kind == JOURNEY ) {
    assert( pJ->journey_id.jid == jid );
    assert( pJ->trips.kind == TRIPS );
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
    pJ->kind = JOURNEY;
    pJ->journey_id.jid = jid;
    pJ->trips.kind = TRIPS;
    pJ->trips.trip_prof[nts] = *ptrip;
    pJ->trips.ntrips = 1;
    
  }
  return &pJ->trips.trip_prof[nts];
}
#else
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
#endif

void emit_ars_schcmds( void ) {
  ;
}

#if 0 // *****
static void print_trip ( ATTR_TRIP_PTR ptrip, BOOL ext ) {
  assert( ptrip );
  char buf[PRINT_STRBUF_MAXLEN + 1] = "";
  
  printf( "(%s, ", cnv2str_kind(buf, ptrip->kind, PRINT_STRBUF_MAXLEN) );
  
  
  
  
  if( ext ) {
    printf( ", %s, ", cnv2str_sp_cond(buf, ptrip->sp_cond.stop_skip, PRINT_STRBUF_MAXLEN) );
    printf( "(%02d:%02d:%02d,", ptrip->arrdep_time.arriv.arr_time.t.hour, ptrip->arrdep_time.arriv.arr_time.t.minute, ptrip->arrdep_time.arriv.arr_time.t.second);
    printf( " %02d:%02d:%02d)", ptrip->arrdep_time.dept.dep_time.t.hour, ptrip->arrdep_time.dept.dep_time.t.minute, ptrip->arrdep_time.dept.dep_time.t.second );
    printf( ", %d", ptrip->sp_cond.dwell_time );
#if 0 // *****
    printf( ", %s", cnv2str_perf_regime( buf, ptrip->perf_regime, PRINT_STRBUF_MAXLEN ) );
#else
    buf[PRINT_STRBUF_MAXLEN - 1] = 0;
    printf( ", %s", strncpy( buf, cnv2str_perf_regime[ptrip->perf_regime.perfreg_cmd], (PRINT_STRBUF_MAXLEN - 1) ) );
#endif
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
#endif

static void print_sp_pair ( SP_ORGDST_PAIR_PTR psps ) {
  assert( psps );
  printf( "(%s, %s)", cnv2str_sp_code[psps->sp_org], cnv2str_sp_code[psps->sp_dst] );
}

static void print_st_pltb ( ST_PLTB_PAIR_PTR pst_pltb ) {
  assert( pst_pltb );
  
  char st_pltb_strbuf[(MAX_STNAME_LEN + 1 + MAX_PLTB_NAMELEN) + 1] = "";
  strncat( st_pltb_strbuf, cnv2str_st_id[pst_pltb->st], MAX_STNAME_LEN );
  strncat( st_pltb_strbuf, "_", 2 );
  strncat( st_pltb_strbuf, cnv2str_pltb_id[pst_pltb->pltb], MAX_PLTB_NAMELEN );
  
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

static const int nspc_indent = 2;
#define TTC_DIAG_INDENT( n ) {int i; for(i = 0; i < (n); i++){ int b; for(b = 0; b < nspc_indent; b++ ) printf(" "); }}

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
      printf( "J%03d\n", journeys[i].jid );
      for( k = 0; k < journeys[i].num_trips; k++ ) {
	TTC_DIAG_INDENT(2);
	ttc_print_jtrip( &journeys[i].trips[k] );
      }
      cnt_j++;
    }
    i++;
  }
}

static int cons_st_pltb_pair ( ST_PLTB_ORGDST_PTR st_pltb_ref[], const int reftbl_len, const int refs_lim, ATTR_ST_PLTB_ORGDST_PTR pattr_orgdst, const  BOOL ovwt ) {
  assert( st_pltb_ref );
  assert( reftbl_len > -1 );
  assert( refs_lim > -1 );
  assert( pattr_orgdst );
  assert( pattr_orgdst->kind == PAR_ST_PLTB_ORGDST ); 
  int r = -1; //TRIP_DESC_PTR r = NULL;
  
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
    
    for( i = 0; i < reftbl_len; i++ ) { // for( i = 0; i < MAX_TRIPS_DECL; i++ ) {
      if( i >= refs_lim ) { // if( i >= timetbl_dataset.trips_decl.num_trips ) {	
	assert( (int)(st_pltb_ref[i]->org.st) == 0 ); // assert( (int)(timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st) == 0 );
	break;
      } else {
	assert( i < refs_lim ); // assert( i < timetbl_dataset.trips_decl.num_trips );
	assert( (int)(st_pltb_ref[i]->org.st) > 0 ); // assert( (int)(timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st) > 0 );
	if( ((st_pltb_ref[i]->org.st == st_org) // (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st == st_org)
	     && (st_pltb_ref[i]->org.pltb == pltb_org) // (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.pltb == pltb_org)
	     ) &&
	    ((st_pltb_ref[i]->dst.st == st_dst) // (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.dst.st == st_dst)
	     && (st_pltb_ref[i]->dst.pltb == pltb_dst) // (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.dst.pltb == pltb_dst)
	     ) ) {
	  ovrid = TRUE;
	  break;
	}
      }
    }
    if( i < reftbl_len ) { // if( i < MAX_TRIPS_DECL ) {
      assert( refs_lim < reftbl_len); // assert( timetbl_dataset.trips_decl.num_trips < MAX_TRIPS_DECL );
      r = i; // r = &timetbl_dataset.trips_decl.trips[i];      
      if( ovrid && ovwt ) {
	assert( r < refs_lim ); // assert( i < timetbl_dataset.trips_decl.num_trips );
	assert( (int)(st_pltb_ref[r]->org.st) > 0 ); // assert( (int)(r->st_pltb_orgdst.org.st) > 0 );
	printf( "NOTICE: trip definition overridden at (LINE, COL) = (%d, %d).\n", pattr_orgdst->st_pltb_org.st.pos.row, pattr_orgdst->st_pltb_org.st.pos.col );
      } else {
	assert( r == refs_lim ); // assert( i == timetbl_dataset.trips_decl.num_trips );
	assert( (int)(st_pltb_ref[r]->org.st) == 0 ); // assert( (int)(r->st_pltb_orgdst.org.st) == 0 );
	//timetbl_dataset.trips_decl.num_trips++;
      }
      st_pltb_ref[r]->org.st = st_org;
      st_pltb_ref[r]->org.pltb = pltb_org;
      st_pltb_ref[r]->dst.st = st_dst;
      st_pltb_ref[r]->dst.pltb = pltb_dst;
      // r->num_routes = -1;
    } else {
      assert( refs_lim == reftbl_len ); // assert( timetbl_dataset.trips_decl.num_trips == MAX_TRIPS_DECL );
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
	    printf( "FATAL: route redefinition in trip declaration at at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	    err_stat.sem.trips.route_redef = TRUE;
	    err = TRUE;
	    break;
	  }
	if( !err ) {
	  ptrip_routes[i].id = pprof->id;
	  ptrip_routes[i].pprof = pprof;
	  nroutes = (nroutes < 0) ? 1 : (assert( nroutes >= 1), nroutes + 1);
	}
      } else {
	printf( "FATAL: undefined route found in trip declaration at at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	err_stat.sem.trips.route_unknown = TRUE;
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

static void cons_trips ( ATTR_TRIPS_PTR ptrips ) {
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
      int nroutes = -1;     
      cons_orgdst_sp_pair( &pT->sp_orgdst, &ptrips->trip_prof[i].attr_sp_orgdst );
      pT->num_routes = -1;
      nroutes = cons_trip_routes( pT->routes, &ptrips->trip_prof[i].attr_route_ctrl );
      if( nroutes > -1 )
	pT->num_routes = nroutes;
      timetbl_dataset.trips_decl.num_trips++;
    }
  }
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
	int l;
	for( l = 0; l < pJ_par->trips.ntrips; l++ ) {
	  assert( pJ_par->trips.trip_prof[l].kind == PAR_TRIP );
	  int newone = -1;
	  newone = cons_st_pltb_pair( st_pltb_ref, MAX_JOURNEY_TRIPS, timetbl_dataset.j.journeys[i].num_trips, &pJ_par->trips.trip_prof[l].attr_st_pltb_orgdst, FALSE );
	  if( newone > -1 ) {
	    assert( timetbl_dataset.j.journeys[i].num_trips == newone );
	    JOURNEY_TRIP_PTR pJ = (JOURNEY_TRIP_PTR)st_pltb_ref[newone];
	    assert( pJ );
	    // settings for pJ->dwell_time;
	    // settings for pJ->time_arrdep;
	    // settings for pJ->perfreg;
	    // settings for pJ->is_revenue;
	    timetbl_dataset.j.journeys[i].num_trips++;
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
  
  int r = 0;
  BOOL err = FALSE;
  yyin = stdin;
  if( yyparse() ) {
    err = TRUE;
    r = 1;
  } else {
    if( err_stat.par.err_trip_journey ||
	err_stat.par.err_routes ||
	err_stat.par.err_trips_decl ||
	err_stat.par.err_trip_def ||
	err_stat.par.err_rake_journey_asgnmnts_decl ||
	err_stat.par.err_rj_asgn ) {
      err = TRUE;
      r = 1;
    }
  }
  if( !err ) {
    cons_trips( &timetable_symtbl->trips_regtbl );
    if( err_stat.sem.trips.route_redef ||
	err_stat.sem.trips.route_unknown ) {
      err = TRUE;
      r = 1;
    }
    if( !err ) {
      cons_journeys( &timetable_symtbl->journeys_regtbl );
      printf( "!!!!! !!!!!\n" ); // *****
      //ttc_print_trips( timetbl_dataset.trips_decl.trips, timetbl_dataset.trips_decl.num_trips );
      ttc_print_journeys( timetbl_dataset.j.journeys, timetbl_dataset.j.num_journeys );
      assert( FALSE ); // *****
    }
  }
  return r;
}

int main ( void ) {
  int r = -1;
  
  timetable_symtbl = (ATTR_TIMETABLE_PTR)calloc( sizeof(ATTR_TIMETABLE), 1 );
  if( !timetable_symtbl ) {
    printf( "memory allocation failed.\n" );
    return r;
  }
  timetable_symtbl->trips_regtbl.kind = PAR_UNKNOWN;
  timetable_symtbl->rj_asgn_regtbl.kind = PAR_UNKNOWN;
  timetable_symtbl->journeys_regtbl.kind = PAR_UNKNOWN;
  
  r = ttcreat();
  return r;
}

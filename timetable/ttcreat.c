#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

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
    if( i < MAX_TRIPS ) {      
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
    if( nts < MAX_TRIPS ) {
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
    if( nts < MAX_TRIPS ) {
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

static TRIP_DESC_PTR chk_st_pltb_pair_cons ( ATTR_ST_PLTB_ORGDST_PTR pattr_orgdst ) {
  assert( pattr_orgdst );
  assert( pattr_orgdst->kind == PAR_ST_PLTB_ORGDST );
  TRIP_DESC_PTR r = NULL;
  
  assert( pattr_orgdst->st_pltb_org.kind == PAR_ST_PLTB );
  assert( pattr_orgdst->st_pltb_dst.kind == PAR_ST_PLTB );
  ST_ID st_org = END_OF_ST_ID;
  ST_ID st_dst = END_OF_ST_ID;    
  PLTB_ID pltb_org = END_OF_PLTB_ID;
  PLTB_ID pltb_dst = END_OF_PLTB_ID;
  
  st_org = ttc_cnv2_st_id( pattr_orgdst->st_pltb_org.st.name );
  pltb_org = ttc_cnv2_pltb_id( pattr_orgdst->st_pltb_org.pltb.id );
  assert( st_org != END_OF_ST_ID );
  assert( pltb_org != END_OF_PLTB_ID );
  
  st_dst = ttc_cnv2_st_id( pattr_orgdst->st_pltb_dst.st.name );
  pltb_dst = ttc_cnv2_pltb_id( pattr_orgdst->st_pltb_dst.pltb.id );
  assert( st_dst != END_OF_ST_ID );
  assert( pltb_dst != END_OF_PLTB_ID );
  {
    BOOL ovwt = FALSE;
    int i;
    assert( timetbl_dataset.trips_decl.num_trips <= MAX_TRIPS );
    for( i = 0; i < MAX_TRIPS; i++ ) {
      assert( i < MAX_TRIPS );
      if( i >= timetbl_dataset.trips_decl.num_trips ) {
	assert( (int)(timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st) == 0 );
	break;
      } else {
	assert( i < timetbl_dataset.trips_decl.num_trips );
	assert( (int)(timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st) > 0 );
	if( ((timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.st == st_org) && (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.org.pltb == pltb_org)) &&
	    ((timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.dst.st == st_dst) && (timetbl_dataset.trips_decl.trips[i].st_pltb_orgdst.dst.pltb == pltb_dst)) ) {
	  ovwt = TRUE;
	  break;
	}
      }
    }
    if( i < MAX_TRIPS ) {
      r = &timetbl_dataset.trips_decl.trips[i];
      assert( timetbl_dataset.trips_decl.num_trips < MAX_TRIPS );
      if( ovwt ) {
	assert( i < timetbl_dataset.trips_decl.num_trips );
	assert( (int)(r->st_pltb_orgdst.org.st) > 0 );
	printf( "NOTICE: trip definition overridden at (LINE, COL) = (%d, %d).\n", pattr_orgdst->st_pltb_org.st.pos.row, pattr_orgdst->st_pltb_org.st.pos.col );
      } else {
	assert( i == timetbl_dataset.trips_decl.num_trips );
	assert( (int)(r->st_pltb_orgdst.org.st) == 0 );
	timetbl_dataset.trips_decl.num_trips++;
      }
    } else {
      assert( timetbl_dataset.trips_decl.num_trips == MAX_TRIPS );
      printf( "FATAL: memory exthausted on trip registration.\n" );
    }
  }
  return r;
}

static SP_ORGDST_PAIR_PTR chk_sp_orgdst_cons ( SP_ORGDST_PAIR_PTR ptrip_sps, ATTR_SP_PAIR_PTR pattr_sps ) {
  assert( ptrip_sps );
  assert( pattr_sps );
  assert( pattr_sps->kind == PAR_SP_PAIR );
  STOPPING_POINT_CODE sp_org = END_OF_SPs;
  STOPPING_POINT_CODE sp_dst = END_OF_SPs;
  
  sp_org = ttc_cnv2_sp_code( pattr_sps->org.sp_id );
  sp_dst = ttc_cnv2_sp_code( pattr_sps->dst.sp_id );
  assert( sp_org != END_OF_SPs );
  assert( sp_dst != END_OF_SPs );
  ptrip_sps->sp_org = sp_org;
  ptrip_sps->sp_dst = sp_dst;
  
  return ptrip_sps;
}

static ROUTE_PTR trip_route_prof ( ATTR_ROUTE_PTR pattr_routes ) {
  assert( pattr_routes );
  assert( pattr_routes->kind == PAR_ROUTE );
  ROUTE_PTR pprof = NULL;
  // CBI_STAT_ATTR_PTR conslt_cbi_code_tbl ( const char *ident );
  // ROUTE_C_PTR conslt_route_prof ( IL_SYM route_id );
  
  return pprof;
}

static int chk_trip_routes_cons ( ROUTE_ASSOC_PTR ptrip_routes, ATTR_ROUTES_PTR pattr_routes ) {
  assert( ptrip_routes );
  assert( pattr_routes );
  int nroutes = -1;
  
  BOOL err = FALSE;
  int i;
  assert( pattr_routes->nroutes <= MAX_TRIP_ROUTES );
  for( i = 0; (i < MAX_TRIP_ROUTES) && !err; i++ ) {
    assert( i < MAX_TRIP_ROUTES );
    if( i >= pattr_routes->nroutes ) {
      assert( ! ptrip_routes[i].pprof );   
      break;
    } else {
      ROUTE_PTR pprof = NULL;      
      pprof = trip_route_prof( &pattr_routes->route_prof[i] );
      if( pprof ) {
	int j;
	for( j = 0; j < i; j++ )
	  if( ptrip_routes[j].pprof == pprof ) {
	    printf( "FATAL: route redefinition in trip declaration at at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	    err = TRUE;
	    break;
	  }
	;
	nroutes = (nroutes < 0) ? 1 : (assert( nroutes >= 1), nroutes + 1);
      } else {
	printf( "FATAL: undefined route found in trip declaration at at (LINE, COL) = (%d, %d).\n", pattr_routes->route_prof[i].pos.row, pattr_routes->route_prof[i].pos.col );
	err = TRUE;
      }
    }
  }
  if( nroutes > 0 )
    nroutes *= (err ? -1 : 1);
  return nroutes;
}

static BOOL chk_trips_consist( ATTR_TRIPS_PTR ptrips ) {
  assert( ptrips );
  assert( ptrips->kind == PAR_TRIPS );
  BOOL r = FALSE;
  
  int i;
  assert( ptrips->ntrips >= 0 );
  for( i = 0; i < ptrips->ntrips; i++ ) {
    assert( ptrips->trip_prof[i].kind == PAR_TRIP );
    TRIP_DESC_PTR pT = NULL;
    pT = chk_st_pltb_pair_cons( &ptrips->trip_prof[i].attr_st_pltb_orgdst );
    if( pT ) {
      int nroutes = -1;
      chk_sp_orgdst_cons( &pT->sp_orgdst, &ptrips->trip_prof[i].attr_sp_orgdst );
      nroutes = chk_trip_routes_cons( pT->routes, &ptrips->trip_prof[i].attr_route_ctrl );
      assert( nroutes > -1 );
      pT->num_routes = nroutes;
    }
    ;
  }
  return r;
}

static int ttcreat ( void ) {
  extern int yyparse( void );
  extern FILE *yyin;
  
  int r = 0;
  BOOL err = FALSE;
  yyin = stdin;
  if( yyparse() ) {
    err = TRUE;
    r = 1;
  } else {
    if( err_stat.err_trip_journey ||
	err_stat.err_routes ||
	err_stat.err_trips_decl ||
	err_stat.err_trip_def ||
	err_stat.err_rake_journey_asgnmnts_decl ||
	err_stat.err_rj_asgn ) {
      err = TRUE;
      r = 1;
    }
  }
  if( err ) {
    printf( "terminated with fatal errors.\n" );
    r = 1;
  } else {
    chk_trips_consist( &timetable_symtbl->trips_regtbl );
    ;
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

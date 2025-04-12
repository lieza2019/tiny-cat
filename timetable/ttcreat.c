#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

char *cnv2str_kind ( char *pstr, KIND kind, const int buflen ) {
  assert( pstr );
  assert( buflen > 0 );
  char *r = NULL;
  
  switch( kind ) {
  case ST_PLTB:
    strncpy( pstr, "ST_PLTB", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case ST_PLTB_PAIR:
    strncpy( pstr, "ST_PLTB_PAIR", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case SP:
    strncpy( pstr, "SP", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case SP_PAIR:
    strncpy( pstr, "SP_PAIR", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case ROUTE:
    strncpy( pstr, "ROUTE", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case ROUTES:
    strncpy( pstr, "ROUTES", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case TRIP:
    strncpy( pstr, "TRIP", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case TRIPS:
    strncpy( pstr, "TRIPS", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case RJ_ASGN:
    strncpy( pstr, "RJ_ASGN", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case RJ_ASGNS:
    strncpy( pstr, "RJ_ASGNS", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case JOURNEY:
    strncpy( pstr, "JOURNEY", (buflen - 1) );
    pstr[buflen - 1] = 0;
    r = pstr;
    break;
  case UNKNOWN:
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
  assert( p1->kind == ST_PLTB );
  assert( p2->kind == ST_PLTB );
  BOOL r = FALSE;
  
  if( ! strncmp( p1->st.name, p2->st.name, MAX_STNAME_LEN ) )
    if( ! strncmp( p1->pltb.id, p2->pltb.id, MAX_PLTB_NAMELEN ) )
      r = TRUE;
  return r;
}

BOOL eq_st_pltb_pair ( ATTR_ST_PLTB_PAIR_PTR pp1, ATTR_ST_PLTB_PAIR_PTR pp2 ) {
  assert( pp1 );
  assert( pp2 );
  assert( pp1->kind == ST_PLTB_PAIR );
  assert( pp2->kind == ST_PLTB_PAIR );
  BOOL r = FALSE;
  
  if( ! eq_st_pltb( &pp1->st_pltb_org, &pp2->st_pltb_org ) )
    if( ! eq_st_pltb( &pp1->st_pltb_dst, &pp2->st_pltb_dst ) )
      r = TRUE;
  return r;
}

static BOOL ident_trips ( ATTR_ST_PLTB_PAIR_PTR pt1, ATTR_ST_PLTB_PAIR_PTR pt2 ) {
  assert( pt1 );
  assert( pt2 );
  assert( pt1->kind == ST_PLTB_PAIR );
  assert( pt2->kind == ST_PLTB_PAIR );
  
  BOOL r = FALSE;
  r = eq_st_pltb_pair( pt1, pt2 );
  
  return r;
}

ATTR_TRIP_PTR reg_trip_def ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip ) {
  assert( preg_tbl );
  assert( ptrip );
  assert( preg_tbl->kind == TRIPS );
  assert( ptrip->attr_st_pltb_orgdst.kind == ST_PLTB_PAIR );
  BOOL ovw = FALSE;
  ATTR_TRIP_PTR r = NULL;
  
  int i;  
  for( i = 0; i < preg_tbl->ntrips; i++ ) {
    assert( i < preg_tbl->ntrips );
    assert( preg_tbl->trip_prof[i].attr_st_pltb_orgdst.kind == ST_PLTB_PAIR );
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
  assert( preg_tbl->kind == RJ_ASGNS );
  assert( pasgn->kind == RJ_ASGN );
  BOOL ovw = FALSE;
  ATTR_RJ_ASGN_PTR r = NULL;
  
  int i;
  for( i = 0; i < preg_tbl->nasgns; i++ ) {
    assert( i < preg_tbl->nasgns );
    assert( preg_tbl->rj_asgn[i].kind == RJ_ASGN );
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
  assert( preg_tbl->kind == JOURNEYS );
  assert( (jid >= 1) && (jid <= MAX_JOURNEYS) );
  assert( ppos );
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
    pJ->journey_id.pos = *ppos;
    pJ->trips.kind = TRIPS;
    pJ->trips.trip_prof[nts] = *ptrip;
    pJ->trips.ntrips = 1;
    
  }
  return &pJ->trips.trip_prof[nts];
}
#endif

void emit_ars_schcmds( void ) {
  ;
}

static BOOL chk_st_pltb_pair_cons ( ATTR_ST_PLTB_PAIR_PTR ppair ) {
  assert( ppair );
  assert( ppair->kind == ST_PLTB_PAIR );
  BOOL r = FALSE;
  
  assert( ppair->st_pltb_org.kind == ST_PLTB );
  assert( ppair->st_pltb_dst.kind == ST_PLTB );
  
  return r;
}

static BOOL chk_trips_consist( ATTR_TRIPS_PTR ptrips ) {
  BOOL r = FALSE;
  assert( ptrips );
  assert( ptrips->kind == TRIPS );
  
  int i;
  assert( ptrips->ntrips >= 0 );
  for( i = 0; i < ptrips->ntrips; i++ ) {
    assert( ptrips->trip_prof[i].kind == TRIP );
    chk_st_pltb_pair_cons( &ptrips->trip_prof[i].attr_st_pltb_orgdst );
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
    chk_trips_consist( &timetable_symtbl.trips_regtbl );
    ;
  }
  return r;
}

int main ( void ) {
  int r = -1;
  
  timetable_symtbl1 = (ATTR_TIMETABLE_PTR)calloc( sizeof(ATTR_TIMETABLE), 1 );
  if( !timetable_symtbl1 ) {
    printf( "memory allocation failed.\n" );
    return r;
  }
  r = ttcreat();
  return r;
}

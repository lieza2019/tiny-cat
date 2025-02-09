#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../generic.h"
#include "ttcreat.h"

BOOL eq_st_pltb ( ATTR_ST_PLTB_PTR p1, ATTR_ST_PLTB_PTR p2 ) {
  assert( p1 );
  assert( p2 );
  assert( p1->kind == ST_PLTB );
  assert( p2->kind == ST_PLTB );
  BOOL r = FALSE;
  
  if( ! strncmp( p1->st_name, p2->st_name, MAX_STNAME_LEN ) )
    if( ! strncmp( p1->pltb_name, p2->pltb_name, MAX_PLTB_NAMELEN ) )
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

ATTR_TRIP_PTR reg_trip ( ATTR_TRIPS_PTR preg_tbl, ATTR_TRIP_PTR pobsolete, ATTR_TRIP_PTR ptrip ) {
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
      printf( "FATAL: trip definition has been exhausted.\n" );
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
    if( preg_tbl->rj_asgn[i].jid == pasgn->jid ) {
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
      printf( "FATAL: rake-journey assignments has been exhausted.\n" );
      exit( 1 );
    }
  }
  return r;
}

void emit_ars_schcmds( void ) {
}

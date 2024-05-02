#include "generic.h"
#include "misc.h"
#include "srv.h"

#define INTERLOCK_C
#include "interlock.h"
#undef INTERLOCK_C

static TRACK_PTR ask_track_status( TRACK_ID track_id ) {
  TRACK_PTR r = NULL;
  int i;
  
  i = 0;
  while( track_state[i].id != END_OF_TRACKS ) {
    if( track_state[i].id == track_id ) {
      r = &track_state[i];
      break;
    }
    i++;
  }
  return r;
}

static ROUTE_PTR ask_route_status( ROUTE_ID route_id ) {
  ROUTE_PTR r = NULL;
  int i;
  
  i = 0;
  while( route_state[i].id != END_OF_ROUTES ) {
    if( route_state[i].id == route_id ) {
      r = &route_state[i];
      break;
    }
    i++;
  }
  return r;
}

static BOOL chk_ars_tracks_occupancy( ROUTE_ID route_id ) {
  BOOL r = FALSE;
  ROUTE_PTR pAttrib = NULL;
  
  pAttrib = ask_route_status( route_id );
  assert( pAttrib );
  {
    int i;
    for( i = 0; i < pAttrib->tr.num_tracks; i++ ) {
      TRACK_PTR ptr = NULL;
      ptr = ask_track_status( pAttrib->tr.tracks[i] );
      assert( ptr );
      if( ptr->occupancy ) {
	r = TRUE;
	break;
      } else
	continue;
    }
    assert( i <= pAttrib->tr.num_tracks );
  }
  return r;
}

static BOOL chk_ars_tracks_lock( ROUTE_ID route_id ) {
  BOOL r = FALSE;
  ROUTE_PTR pAttrib = NULL;
  
  pAttrib = ask_route_status( route_id );
  assert( pAttrib );
  {
    int i;
    for( i = 0; i < pAttrib->tr.num_tracks; i++ ) {
      TRACK_PTR ptr = NULL;
      ptr = ask_track_status( pAttrib->tr.tracks[i] );
      assert( ptr );
      if( TLSR_LOCKED( *ptr )
	  || TRSR_LOCKED( *ptr )
	  || sTLSR_LOCKED( *ptr )
	  || sTRSR_LOCKED( *ptr )
	  || eTLSR_LOCKED( *ptr )
	  || eTRSR_LOCKED( *ptr )
	  || kTLSR_LOCKED( *ptr )
	  || kTRSR_LOCKED( *ptr ) ) {
	r = TRUE;
	break;
      }
    }
    assert( i <= pAttrib->tr.num_tracks );
  }
  return r;
}

ARS_REJECTED_REASON chk_ars_condition( ROUTE_ID route_id ) {
  ARS_REJECTED_REASON r = ARS_WELL_CONDITION;
  
  if( chk_ars_tracks_occupancy( route_id ) )
    r = ARS_TRACKS_OCCUPIED;
  else {
    if( chk_ars_tracks_lock( route_id ) )
      r = ARS_TRACKS_LOCKED;
    else
      r = ARS_WELL_CONDITION;
  }
  return r;
}

static BOOL chk_ars_triggered( ARS_ROUTE_PTR pRoute_ars ) {
  assert( pRoute_ars );
  BOOL r = FALSE;
  int i;
  
  for( i = 0; i < pRoute_ars->trg_section.num_blocks; i++ ) {
    TRACK_PTR ptr = NULL;
    ptr = ask_track_status( pRoute_ars->trg_section.trg_blocks[i] );
    assert( ptr );
    if( ptr->occupancy ) {
      r = TRUE;
      break;
    }
  }
  return r;
}

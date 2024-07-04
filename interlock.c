#include "generic.h"
#include "misc.h"
#include "network.h"
#include "cbi.h"
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

static CBI_STAT_INFO_PTR willing_to_recv_OC_stat ( TINY_SOCK_PTR pS, OC2ATS_STAT msg_id ) {
  assert( pS );
  assert( (msg_id >= 0) && (msg_id < END_OF_OC2ATS) );
  CBI_STAT_INFO_PTR r = NULL;
  
  CBI_STAT_INFO_PTR pOC = NULL;
  pOC = &cbi_stat_OC2ATS[msg_id];
  assert( pOC );
  pOC->oc2ats.d_recv_cbi_stat = -1;
  {
    TINY_SOCK_DESC d = -1;
    if( (d = creat_sock_recv( pS, pOC->oc2ats.dst_port )) < 0 ) {
      errorF( "failed to create the socket to receive CBI status information from OC2ATS%d.\n", OC_MSG_ID_CONV_2_INT(msg_id) );
      goto exit;
    }
    pOC->oc2ats.d_recv_cbi_stat = d;
    sock_attach_recv_buf( pS, d, (unsigned char *)&(pOC->oc2ats.recv), (int)sizeof(pOC->oc2ats.recv) );
    r = pOC;
  }
 exit:
  return r;
}

static BOOL establish_OC_stat_recv ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  
  int i = (int)OC2ATS1;
  while( i < (int)END_OF_OC2ATS ) {
    assert( (i >= (int)OC2ATS1) && (i < (int)END_OF_OC2ATS) );
    CBI_STAT_INFO_PTR p = NULL;
    if( !(p = willing_to_recv_OC_stat( pS, (OC2ATS_STAT)i )) )
      goto exit;
    assert( p );
    assert( p->oc2ats.d_recv_cbi_stat > -1 );
    i++;
  }
  r = TRUE;
 exit:
  return r;
}

static CBI_STAT_INFO_PTR willing_to_send_OC_cmd ( TINY_SOCK_PTR pS, ATS2OC_CMD msg_id ) {
  assert( pS );
  assert( (msg_id >= ATS2OC801) && (msg_id < END_OF_ATS2OC) );
  CBI_STAT_INFO_PTR r = NULL;
  
  CBI_STAT_INFO_PTR pOC = NULL;
  pOC = &cbi_stat_ATS2OC[(int)msg_id];
  assert( pOC );
  {
    IP_ADDR_DESC bcast_dst_ipaddr = pOC->oc_ipaddr[(int)msg_id];
    assert( bcast_dst_ipaddr.oct_1st != 0 );
    assert( bcast_dst_ipaddr.oct_2nd != 0 );
    assert( bcast_dst_ipaddr.oct_3rd != 0 );
    assert( bcast_dst_ipaddr.oct_4th != 0 );
    bcast_dst_ipaddr.oct_3rd = 255;
    bcast_dst_ipaddr.oct_4th = 255;
    
    pOC->ats2oc.d_recv_cbi_stat = -1;
    {
      TINY_SOCK_DESC d = -1;
      if( (d = creat_sock_sendnx( pS, pOC->ats2oc.dst_port, TRUE, &bcast_dst_ipaddr )) < 0 ) {
	errorF( "failed to create the socket to send CBI control commands  toward OC%d.\n", OC_ID_CONV_2_INT(msg_id) );
	goto exit;
      }
      pOC->ats2oc.d_recv_cbi_stat = d;
      sock_attach_send_buf( pS, pOC->ats2oc.d_recv_cbi_stat, (unsigned char *)&(pOC->ats2oc.sent), (int)sizeof(pOC->ats2oc.sent) );
    }
    r = pOC;
  }
 exit:
  return r;
}

static BOOL establish_OC_stat_send ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  
  int i = ATS2OC801;
  while( i < (int)END_OF_ATS2OC ) {
    assert( (i >= ATS2OC801) && (i < (int)END_OF_ATS2OC) );
    CBI_STAT_INFO_PTR p = NULL;
    if( !(p = willing_to_send_OC_cmd( pS, (ATS2OC_CMD)i )) )
      goto exit;
    assert( p );
    assert( p->ats2oc.d_recv_cbi_stat > -1 );
    i++;
  }
  r = TRUE;
 exit:
  return r;
}

BOOL establish_CBI_comm ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;

  if( establish_OC_stat_recv( pS ) )
    if( establish_OC_stat_send( pS ) )
      r = TRUE;
  
  return r;
}

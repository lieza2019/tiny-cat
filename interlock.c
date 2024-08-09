#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "cbi.h"
#include "srv.h"

#define INTERLOCK_C
#include "interlock.h"
#undef INTERLOCK_C

const CBI_STAT_KIND ROUTE_KIND2GENERIC[] = {
  _SIGNAL, // MAIN_ROUTE,
  _SIGNAL, // SHUNTING_ROUTE,
  _SIGNAL, // EMERGENCY_ROUTE,
  _CBI_KIND_NONSENS // END_OF_ROUTE_KINDS
};

pthread_mutex_t cbi_stat_info_mutex;

#if 0
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

static ROUTE_PTR ask_route_status( IL_OBJ_INSTANCES route_id ) { 
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

static BOOL chk_ars_tracks_occupancy( IL_OBJ_INSTANCES route_id ) {
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

static BOOL chk_ars_tracks_lock( IL_OBJ_INSTANCES route_id ) {
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

ARS_REJECTED_REASON chk_ars_condition( IL_OBJ_INSTANCES route_id ) {
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
#endif

void cons_track_state ( TRACK_PTR ptrack ) {
  assert( ptrack );
  
  assert( ptrack->kind_cbi == _TRACK );
  assert( ptrack->id < END_OF_IL_OBJ_INSTANCES );
  assert( ptrack->name != NULL );
  int i;
  for( i = 0; i < ptrack->cbtc.num_blocks; i++ ) {
    int found = -1;
    int j = 0;
    while( block_state[j].virt_block_name != END_OF_CBTC_BLOCKs ) {
      assert( block_state[j].block_name > 0 );
      if( block_state[j].virt_block_name == ptrack->cbtc.blocks[i] ) {
	assert( found < 0 );
	ptrack->cbtc.pblocks[i] = &block_state[j];
	assert( ptrack->cbtc.pblocks[i] );
	found = j;
      }
      j++;
    }
    assert( block_state[j].virt_block_name == END_OF_CBTC_BLOCKs );
    assert( found > -1 );
    assert( ptrack->cbtc.pblocks[i] == &block_state[found] );
  }
}

void cons_route_state ( ROUTE_PTR proute ) {
  assert( proute );
  
  assert( (proute->kind_cbi == _ROUTE) && (proute->kind_route < END_OF_ROUTE_KINDS) );
  assert( proute->id < END_OF_IL_OBJ_INSTANCES );
  assert( proute->name != NULL );
  {
    int i;
    for( i = 0; i < proute->body.num_tracks; i++ ) {
      int found = -1;
      int j = 0;
      while( track_state[j].kind_cbi != END_OF_CBI_STAT_KIND ) {
	assert( track_state[j].kind_cbi == _TRACK );
	if( track_state[j].id == proute->body.tracks[i] ) {
	  assert( found < 0 );
	  proute->body.ptracks[i] = &track_state[j];
	  assert( proute->body.ptracks[i] );
	  found = j;
	}
	j++;
      }
      assert( track_state[j].kind_cbi == END_OF_CBI_STAT_KIND );
      assert( found > -1 );
      assert( proute->body.ptracks[i] == &track_state[found] );
    }
  }
  
  if( proute->ars_ctrl.app ) {
    int i;
    for( i = 0; i < proute->ars_ctrl.trg_sect.num_blocks; i++ ) {
      int found = -1;
      int j = 0;
      while( block_state[j].virt_block_name != END_OF_CBTC_BLOCKs ) {
	assert( block_state[j].block_name > 0 );
	if( block_state[j].virt_block_name == proute->ars_ctrl.trg_sect.trg_blks[i] ) {
	  assert( found < 0 );
	  proute->ars_ctrl.trg_sect.ptrg_blks[i] = &block_state[j];
	  assert( proute->ars_ctrl.trg_sect.ptrg_blks[i] );
	  found = j;
	}
	j++;
      }
      assert( block_state[j].virt_block_name == END_OF_CBTC_BLOCKs );
      assert( found > -1 );      
      assert( proute->ars_ctrl.trg_sect.ptrg_blks[i] == &block_state[found] );
    }
    
    {
      int i;
      for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks_occ; i++ ) {
	int found = -1;
	int j = 0;
	while( track_state[j].kind_cbi != END_OF_CBI_STAT_KIND ) {
	  assert( track_state[j].kind_cbi == _TRACK );
	  if( track_state[j].id == proute->ars_ctrl.ctrl_tracks.chk_trks[i] ) {
	    assert( found < 0 );
	    proute->ars_ctrl.ctrl_tracks.pchk_trks[i] = &track_state[j];
	    assert( proute->ars_ctrl.ctrl_tracks.pchk_trks[i] );
	    found = j;
	  }
	  j++;
	}
	assert( track_state[j].kind_cbi == END_OF_CBI_STAT_KIND );
	assert( found > -1 );
	assert( proute->ars_ctrl.ctrl_tracks.pchk_trks[i] == &track_state[found] );
      }
    }
  }
}

void cons_cbtc_block_state ( CBTC_BLOCK_PTR pblock ) {
  assert( pblock );
  
  assert( pblock->block_name > 0 );
  assert( pblock->virt_block_name < END_OF_CBTC_BLOCKs );
  assert( pblock->virt_blkname_str != NULL );
  int found = -1;
  int i = 0;
  while( track_state[i].kind_cbi != END_OF_CBI_STAT_KIND ) {
    assert( track_state[i].kind_cbi == _TRACK );
    if( track_state[i].id == pblock->belonging_tr.track ) {
      assert( found < 0 );
      pblock->belonging_tr.ptrack = &track_state[i];
      assert( pblock->belonging_tr.ptrack );
      {
	int f = -1;
	int j;
	for( j = 0; j < pblock->belonging_tr.ptrack->cbtc.num_blocks; j++ ) {
	  if( pblock == pblock->belonging_tr.ptrack->cbtc.pblocks[j] )
	    f = j;
	}
	assert( j == pblock->belonging_tr.ptrack->cbtc.num_blocks );
	assert( (f > -1) && (f < pblock->belonging_tr.ptrack->cbtc.num_blocks) );
      }
      found = i;
    }
    i++;
  }
  assert( track_state[i].kind_cbi == END_OF_CBI_STAT_KIND );
  assert( found > -1 );
  assert( pblock->belonging_tr.ptrack == &track_state[found] );
}

void cons_il_obj_tables ( void ) {
  int i = 0;
  while( route_state[i].kind_route != END_OF_ROUTE_KINDS ) {
    cons_route_state( &route_state[i] );
    i++;
  }
  assert( route_state[i].kind_route == END_OF_ROUTE_KINDS );
  
  {
    int i = 0;
    while( track_state[i].kind_cbi != END_OF_CBI_STAT_KIND ) {
      cons_track_state( &track_state[i] );
      i++;
    }
    assert( track_state[i].kind_cbi == END_OF_CBI_STAT_KIND );
  }
  
  {
    int i = 0;
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      cons_cbtc_block_state ( &block_state[i] );
      i++;
    }
    assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
    assert( block_state[i].block_name == 0 );
  }
}

#if 1 // for MODULE-TEST
int main( void ) {
  cons_il_obj_tables();
  return 0;
}
#endif
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
      errorF( "failed to create the socket to receive CBI status information from OC2ATS%d.\n", OC_MSG_ID_CONV2INT(msg_id) );
      goto exit;
    }
    pOC->oc2ats.d_recv_cbi_stat = d;
    sock_attach_recv_buf( pS, d, (unsigned char *)&(pOC->oc2ats.recv), (int)sizeof(pOC->oc2ats.recv) );
    r = pOC;
  }
 exit:
  return r;
}

BOOL establish_OC_stat_recv ( TINY_SOCK_PTR pS ) {
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
	errorF( "failed to create the socket to send CBI control commands  toward OC%d.\n", OC_ID_CONV2INT(msg_id) );
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

BOOL establish_OC_stat_send ( TINY_SOCK_PTR pS ) {
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

static RECV_BUF_CBI_STAT_PTR update_cbi_status ( TINY_SOCK_PTR pS, OC2ATS_STAT msg_id ) {
  assert( pS );
  assert( (msg_id >= OC2ATS1) && (msg_id < END_OF_OC2ATS) );
  RECV_BUF_CBI_STAT_PTR r = NULL;
  CBI_STAT_INFO_PTR pOC = NULL;
  
  int r_mutex = -1;
  r_mutex = pthread_mutex_lock( &cbi_stat_info_mutex );
  if( r_mutex ) {
    assert( FALSE );
  }
  pOC = &cbi_stat_OC2ATS[(int)msg_id];
  assert( pOC );
  {
    TINY_SOCK_DESC d = -1;
    int len = -1;
    d = pOC->oc2ats.d_recv_cbi_stat;
    assert( d > -1 );
    assert( sock_recv_socket_attached( pS, d ) > 0 );
    {
      OC_ID oc_id = END_OF_OCs;
      NXNS_HEADER_PTR precv = NULL;
      precv = (NXNS_HEADER_PTR)sock_recv_buf_attached( pS, d, &len );
      assert( precv );
      assert( precv == (NXNS_HEADER_PTR)&(pOC->oc2ats.recv) );
      assert( len > 0 ? len >= sizeof(NXNS_HEADER) : TRUE );
      {
	int i = (int)OC801;
	while( i < (int)END_OF_OCs ) {
	  assert( (i >= OC801) && (i < END_OF_OCs) );
	  if( pOC->LNN[i] == ntohs(precv->nx_hdr.SA_LNN_srcAddrLogicNodeNum) )
	    break;
	  i++;
	}
	assert( (i >= OC801) && (i <= END_OF_OCs) );
	if( i >= END_OF_OCs ) {
	  if( pOC->LNN[i] > 0 ) {
	    errorF( "CBI status info with UNKNOWN LNN of %d received from OC2ATS%d, and ignored.\n",
		    ntohs(precv->nx_hdr.SA_LNN_srcAddrLogicNodeNum), OC_MSG_ID_CONV2INT(msg_id) );
	  }
	  goto exit;
	} else
	  oc_id = (OC_ID)i;
      }
      assert( (oc_id >= OC801) && (oc_id < END_OF_OCs) );
      
      switch( msg_id ) {
      case OC2ATS1:
	if( len < OC_OC2ATS1_MSGSIZE )
	  goto msg_size_err;
	break;
      case OC2ATS2:
	if( len < OC_OC2ATS2_MSGSIZE )
	  goto msg_size_err;
	break;
      case OC2ATS3:
	if( len < OC_OC2ATS3_MSGSIZE )
	  goto msg_size_err;
	break;
      msg_size_err:
	errorF( "illegal sized CBI status info received from OC2ATS%d, and ignored.\n",  OC_MSG_ID_CONV2INT(msg_id) );
	goto exit;
      default:
	assert( FALSE );
      }
      r = (RECV_BUF_CBI_STAT_PTR)memcpy( (void *)&cbi_stat_info[(int)oc_id].msgs[msg_id].buf, precv, len );
      cbi_stat_info[(int)oc_id].msgs[msg_id].updated = TRUE;
    }
  }
 exit:
  r_mutex = -1;
  r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
  if( r_mutex ) {
    assert( FALSE );
  }
  
  return r;
}

static void expire_cbi_status ( void ) {
  int i = -1;
  i = (int)OC801;
  while( i < (int)END_OF_OCs ) {
    assert( (i >= OC801) && (i < END_OF_OCs) );
    int j;
    for( j = 0; j < OC_OC2ATS_MSGS_NUM; j++ )
      cbi_stat_info[i].msgs[j].updated = FALSE;
    i++;
  }
  assert( i == END_OF_OCs );
}

void reveal_il_status ( TINY_SOCK_PTR pS ) {
  assert( pS );
  int i = -1;
  
  expire_cbi_status();
  i = (int)OC2ATS1;
  while( i < (int)END_OF_OC2ATS ) {
    assert( (i >= OC2ATS1) && (i < END_OF_OC2ATS) );
    update_cbi_status( pS, (OC2ATS_STAT)i );
    i++;
  }
  assert( i == END_OF_OC2ATS );
}

void *pth_reveal_il_status ( void *arg ) {
  assert( arg );  
  const useconds_t interval = 1000 * 1000 * 0.01;
  TINY_SOCK_PTR pS = NULL;
  int omits[END_OF_OCs][OC_OC2ATS_MSGS_NUM];
  {
    int i = (int)OC801;
    while( i < (int)END_OF_OCs ) {
      assert( (i >= OC801) && (i < END_OF_OCs) );
      int j;
      for( j = 0; j < OC_OC2ATS_MSGS_NUM; j++ )
	omits[i][j] = 0;
      i++;
    }
    assert( i == END_OF_OCs );
  }
  
  pS = (TINY_SOCK_PTR)arg;
  while( TRUE ) {
    assert( pS );
    if( sock_recv( pS ) < 0 ) {
      errorF( "%s", "error on receiving CBI status information from OCs.\n" );
      continue;
    }
    
    reveal_il_status( pS );
    {
      const int obsolete = 100;
      int j = (int)OC801;
      while( j < (int)END_OF_OCs ) {	
	assert( (j >= OC801) && (j < END_OF_OCs) );
	int k;
	for( k = 0; k < OC_OC2ATS_MSGS_NUM; k++ )
	  if( cbi_stat_info[j].msgs[k].updated ) {
	    if( omits[j][k] < 0 )
	      errorF( "CBI status information of (OC%3d, OC2ATS%d) has started updating, again.\n", OC_ID_CONV2INT(j), OC_MSG_ID_CONV2INT(k) );
	    omits[j][k] = 0;
	  } else {
	    if( omits[j][k] >= obsolete ) {
	      errorF( "CBI status information of (OC%3d, OC2ATS%d) has been obsoleted.\n", OC_ID_CONV2INT(j), OC_MSG_ID_CONV2INT(k) );
	      omits[j][k] = -1;
	    } else
	      if( omits[j][k] > -1 )
		omits[j][k]++;
	  }
	j++;
      }
      assert( j == END_OF_OCs );
    }
    
    {
      int r = 1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

int conslt_il_state ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident ) {
  assert( poc_id );
  assert( pkind );
  assert( ident );
  int r = -1;
  CBI_STAT_ATTR_PTR pA = NULL;
  
  *poc_id = -1;
  *pkind = -1;
  pA = conslt_cbi_code_tbl( ident );
  if( pA ) {
    assert( pA );
    assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
    int r_mutex = -1;
    r_mutex = pthread_mutex_trylock( &cbi_stat_info_mutex );
    if( !r_mutex ) {
      assert( ! strncmp(pA->ident, ident, CBI_STAT_IDENT_LEN) );
      *poc_id = pA->oc_id;
      *pkind = pA->kind;
      {
	RECV_BUF_CBI_STAT_PTR pstat = NULL;
	pstat = &cbi_stat_info[pA->oc_id];
	assert( pstat );
	{ 
	  unsigned char *pgrp = &((unsigned char *)&pstat->msgs[pA->group.msg_id])[pA->group.addr];
	  assert( pgrp );
	  r = pgrp[pA->disp.bytes] & pA->disp.mask;
	}
      }
      r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
      if( r_mutex ) {
	assert( FALSE );
      }
    }
  }
  return r;
}

#if 0
void diag_cbi_stat_attrib ( FILE *fp_out, char *ident ) {
  assert( fp_out );
  assert( ident );
  CBI_STAT_ATTR_PTR pA = NULL;
  
  int r_mutex = -1;
  r_mutex = pthread_mutex_lock( &cbi_stat_info_mutex );
  if( r_mutex ) {
    assert( FALSE);
  }
  
  pA = conslt_cbi_code_tbl( ident );
  if( pA ) {
    assert( pA );
    fprintf( fp_out, "ident: %s\n", pA->ident );
    fprintf( fp_out, "oc_id: OC%3d\n", OC_ID_CONV2INT(pA->oc_id) );
    fprintf( fp_out, "name: %s\n", pA->name );
    fprintf( fp_out, "kind: %s\n", cnv2str_cbi_stat_kind[pA->kind] );
    fprintf( fp_out, "group of (raw, oc_from, addr): (%s, OC2ATS%d, %d)\n",
	     CBI_STAT_GROUP_CONV2STR[pA->group.raw], OC_MSG_ID_CONV2INT(pA->group.msg_id), pA->group.addr );
    {
      char s[] = "CBI_STAT_BIT_x";
      fprintf( fp_out, "disp of (raw, bytes, bits, mask): (%d, %d, %d, %s)\n",
	       pA->disp.raw, pA->disp.bytes, pA->disp.bits, show_cbi_stat_bitmask(s, sizeof(s), pA->disp.mask) );
    }
    {
      assert( pA );
      assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
      RECV_BUF_CBI_STAT_PTR pstat = NULL;

      pstat = &cbi_stat_info[pA->oc_id];
      {
	assert( pstat );
	unsigned char *pgrp = &((unsigned char *)&pstat->msgs[pA->group.msg_id])[pA->group.addr];
	assert( pgrp );
	fprintf( fp_out, "value: %d\n", (pgrp[pA->disp.bytes] & pA->disp.mask) );
      }
    }
  }
  r_mutex = -1;
    r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
  if( r_mutex ) {
    assert( FALSE );
  }
}
#else
void diag_cbi_stat_attrib ( FILE *fp_out, char *ident ) {
  assert( fp_out );
  assert( ident );
  CBI_STAT_ATTR_PTR pA = NULL;
  
  pA = conslt_cbi_code_tbl( ident );
  if( pA ) {
    assert( pA );
    int r_mutex = -1;
    r_mutex = pthread_mutex_trylock( &cbi_stat_info_mutex );
    if( !r_mutex ) {
      fprintf( fp_out, "ident: %s\n", pA->ident );
      fprintf( fp_out, "oc_id: OC%3d\n", OC_ID_CONV2INT(pA->oc_id) );
      fprintf( fp_out, "name: %s\n", pA->name );
      fprintf( fp_out, "kind: %s\n", cnv2str_cbi_stat_kind[pA->kind] );
      fprintf( fp_out, "group of (raw, oc_from, addr): (%s, OC2ATS%d, %d)\n",
	       CBI_STAT_GROUP_CONV2STR[pA->group.raw], OC_MSG_ID_CONV2INT(pA->group.msg_id), pA->group.addr );
      {
	char s[] = "CBI_STAT_BIT_x";
	fprintf( fp_out, "disp of (raw, bytes, bits, mask): (%d, %d, %d, %s)\n",
		 pA->disp.raw, pA->disp.bytes, pA->disp.bits, show_cbi_stat_bitmask(s, sizeof(s), pA->disp.mask) );
      }
      {
	assert( pA );
	assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
	RECV_BUF_CBI_STAT_PTR pstat = NULL;

	pstat = &cbi_stat_info[pA->oc_id];
	{
	  assert( pstat );
	  unsigned char *pgrp = &((unsigned char *)&pstat->msgs[pA->group.msg_id])[pA->group.addr];
	  assert( pgrp );
	  fprintf( fp_out, "value: %d\n", (pgrp[pA->disp.bytes] & pA->disp.mask) );
	}
      }
      r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
      if( r_mutex ) {
	assert( FALSE );
      }
    }
  }
}
#endif

BOOL chk_routeconf ( ROUTE_C_PTR r1, ROUTE_C_PTR r2 ) {
  assert( r1 );
  assert( r2 );
  BOOL r = FALSE;
  
  int i;
  for( i = 0; i < r1->body.num_tracks; i++ ) {
    int j;
    for( j = 0; j < r2->body.num_tracks; j++ )
      if( r1->body.tracks[i] == r2->body.tracks[j] ) {
	r = TRUE;
	goto found;
      }
  }
 found:
  return r;
}

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
pthread_mutex_t cbi_ctrl_sendbuf_mutex;
pthread_mutex_t cbi_ctrl_dispatch_mutex;

static TRACK_C_PTR lkup_track_prof[END_OF_IL_OBJ_INSTANCES];
static void cons_track_prof_lkup_tbl ( void ) {
  int i = 0;
  while( track_state[i].kind_cbi != END_OF_CBI_STAT_KIND ) {
    assert( track_state[i].kind_cbi == _TRACK );
    assert( (track_state[i].id >= 0) && (track_state[i].id < END_OF_IL_OBJ_INSTANCES) );
    lkup_track_prof[track_state[i].id] = &track_state[i];
    assert( lkup_track_prof[track_state[i].id] );
    i++;
  }
  assert( track_state[i].kind_cbi == END_OF_CBI_STAT_KIND );
#ifdef CHK_STRICT_CONSISTENCY
  {
    int cnt = 0;
    int j;
    for( j = 0; j < END_OF_IL_OBJ_INSTANCES; j++ ) {
      if( lkup_track_prof[j] )
	cnt++;
    }
    assert( cnt == i );
    {
      int k = 0;
      while( track_state[k].kind_cbi != END_OF_CBI_STAT_KIND ) {
	int found = -1;
	int l;
	for( l = 0; l < END_OF_IL_OBJ_INSTANCES; l++ ) {
	  if( lkup_track_prof[l] == &track_state[k] ) {
	    assert( found < 0 );
	    found = l;
	  }
	}
	assert( found >= 0 );
	assert( lkup_track_prof[found] == &track_state[k] );
	cnt--;
	k++;
      }
      assert( track_state[k].kind_cbi == END_OF_CBI_STAT_KIND );
      assert( cnt == 0 );
    }
  }
#endif // CHK_STRICT_CONSISTENCY
}

TRACK_C_PTR conslt_track_prof ( IL_OBJ_INSTANCES track_id ) {
  assert( (track_id >= 0) && (track_id < END_OF_IL_OBJ_INSTANCES) );
  return lkup_track_prof[track_id];
}

static ROUTE_C_PTR lkup_route_prof[END_OF_IL_OBJ_INSTANCES];
static void cons_route_prof_lkup_tbl ( void ) {
  int i = 0;
  while( route_state[i].kind_route != END_OF_ROUTE_KINDS ) {
    assert( route_state[i].kind_cbi == _ROUTE );
    assert( (route_state[i].id >= 0) && (route_state[i].id < END_OF_IL_OBJ_INSTANCES) );
    lkup_route_prof[route_state[i].id] = &route_state[i];
    assert( lkup_route_prof[route_state[i].id] );
    i++;
  }
  assert( route_state[i].kind_route == END_OF_ROUTE_KINDS );
  assert( route_state[i].kind_cbi == END_OF_CBI_STAT_KIND );
#ifdef CHK_STRICT_CONSISTENCY
  {
    int cnt = 0;
    int j;
    for( j = 0; j < END_OF_IL_OBJ_INSTANCES; j++ ) {
      if( lkup_route_prof[j] )
	cnt++;
    }
    assert( cnt == i );
    {
      int k = 0;
      while( route_state[k].kind_route != END_OF_ROUTE_KINDS ) {
	int found = -1;
	int l;
	for( l = 0; l < END_OF_IL_OBJ_INSTANCES; l++ ) {
	  if( lkup_route_prof[l] && (lkup_route_prof[l] == &route_state[k]) ) {
	    assert( found < 0 );
	    found = l;
	  }
	}
	assert( found >= 0 );
	assert( lkup_route_prof[found] == &route_state[k] );
	cnt--;
	k++;
      }
      assert( route_state[k].kind_route == END_OF_ROUTE_KINDS );
      assert( route_state[k].kind_cbi == END_OF_CBI_STAT_KIND );
      assert( cnt == 0 );
    }
  }
#endif // CHK_STRICT_CONSISTENCY
}

ROUTE_C_PTR conslt_route_prof ( IL_OBJ_INSTANCES route_id ) {
  assert( (route_id >= 0) && (route_id < END_OF_IL_OBJ_INSTANCES) );
  return lkup_route_prof[route_id];
}

void cons_track_state ( TRACK_PTR ptrack ) {
  assert( ptrack );
  
  assert( ptrack->kind_cbi == _TRACK );
  assert( ptrack->id < END_OF_IL_OBJ_INSTANCES );
  assert( ptrack->name != NULL );
  int i;
  for( i = 0; i < ptrack->cbtc.num_blocks; i++ ) {
    assert( ptrack->cbtc.blocks[i] );
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
      assert( proute->body.tracks[i] );
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
      assert( proute->ars_ctrl.trg_sect.trg_blks[i] );
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
	assert( proute->ars_ctrl.ctrl_tracks.chk_trks[i] );
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
      
      for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_ahead_tracks; i++ ) {
	assert( proute->ars_ctrl.ctrl_tracks.ahead_trks[i] );
	int found = -1;
	int j = 0;
	while( j < MAX_ROUTE_TRACKS ) {
	  if( proute->ars_ctrl.ctrl_tracks.chk_trks[j] == proute->ars_ctrl.ctrl_tracks.ahead_trks[i] ) {
	    assert( found < 0 );
	    proute->ars_ctrl.ctrl_tracks.pahead_trks[i] = proute->ars_ctrl.ctrl_tracks.pchk_trks[j];
	    assert( proute->ars_ctrl.ctrl_tracks.pahead_trks[i] );
	    found = j;
	  }
	  j++;
	}
	assert( found > -1 );
	assert( proute->ars_ctrl.ctrl_tracks.pahead_trks[i] == proute->ars_ctrl.ctrl_tracks.pchk_trks[found] );
      }
    }
    
    {
      int found = -1;
      int i = 0;
      while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
	assert( block_state[i].block_name > 0 );
	if( block_state[i].virt_block_name == proute->ars_ctrl.trip_info.dep.blk ) {
	  assert( found < 0 );
	  proute->ars_ctrl.trip_info.dep.pblk = &block_state[i];
	  assert( proute->ars_ctrl.trip_info.dep.pblk );
	  assert( (proute->ars_ctrl.trip_info.dep.sp >= 0) && (proute->ars_ctrl.trip_info.dep.sp < END_OF_SPs) );
	  if( proute->ars_ctrl.trip_info.dep.sp != SP_NONSENS ) {
	    CBTC_BLOCK_C_PTR p = proute->ars_ctrl.trip_info.dep.pblk;
	    assert( p );
	    assert( p->sp.has_sp );
	    assert( p->sp.sp_code == proute->ars_ctrl.trip_info.dep.sp );
	    assert( p->virt_block_name == proute->ars_ctrl.trip_info.dep.blk );
	  }
	  found = i;
	}
	i++;
      }
      assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
      assert( found > -1 );
      assert( proute->ars_ctrl.trip_info.dep.pblk = &block_state[found] );
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
	  assert( pblock->belonging_tr.ptrack->cbtc.pblocks[j] );
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
  
  if( pblock->sp.has_sp ) {
    assert( (pblock->sp.sp_code > 0) && (pblock->sp.sp_code < END_OF_SPs) );
    assert( (pblock->sp.stop_detect_type >= 0) && (pblock->sp.stop_detect_type < END_OF_STOP_DETECTION_TYPES) );
    pblock->sp.stop_detect_cond.ppaired_blk = NULL;
    
    if( pblock->sp.stop_detect_cond.paired_blk != VB_NONSENS ) {
      int found = -1;
      int i = 0;
      while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
	assert( block_state[i].block_name > 0 );
	if( block_state[i].virt_block_name == pblock->sp.stop_detect_cond.paired_blk ) {
	  assert( found < 0 );
	  pblock->sp.stop_detect_cond.ppaired_blk = &block_state[i];
	  assert( pblock->sp.stop_detect_cond.ppaired_blk );
	  found = i;
	}
	i++;
      }
      assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
      assert( found > -1 );
      assert( pblock->sp.stop_detect_cond.ppaired_blk == &block_state[found] );
    }
  }
}

static void chk_consistency_over_sp_links ( void ) {
  int i = 0;
  while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
    assert( block_state[i].block_name > 0 );
    assert( (block_state[i].virt_block_name >= 0) && (block_state[i].virt_block_name < END_OF_CBTC_BLOCKs) );
    assert( block_state[i].virt_blkname_str );
    if( block_state[i].sp.has_sp ) {
      CBTC_BLOCK_C_PTR pblock = NULL;
      pblock = &block_state[i];
      assert( pblock );
      if( pblock->sp.stop_detect_cond.paired_blk != VB_NONSENS ) {
	CBTC_BLOCK_C_PTR pblk_assoc = pblock->sp.stop_detect_cond.ppaired_blk;
	assert( pblk_assoc );
	assert( pblk_assoc->sp.has_sp );
	assert( pblk_assoc->sp.sp_code == pblock->sp.sp_code );
	assert( pblk_assoc->sp.stop_detect_type == pblock->sp.stop_detect_type );
	
	assert( pblk_assoc->sp.stop_detect_cond.paired_blk == pblock->virt_block_name );
	assert( ! pblk_assoc->sp.stop_detect_cond.ppaired_blk );
	assert( pblk_assoc->sp.stop_detect_cond.ppaired_blk == pblock );
      } else {
	assert( pblock->sp.stop_detect_cond.paired_blk == VB_NONSENS );
	assert( ! pblock->sp.stop_detect_cond.ppaired_blk );
      }
    }
    i++;
  }
  assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
  assert( block_state[i].block_name == 0 );
}

void cons_il_obj_tables ( void ) {
  int i = 0;
  while( track_state[i].kind_cbi != END_OF_CBI_STAT_KIND ) {
    cons_track_state( &track_state[i] );
    i++;
  }
  assert( track_state[i].kind_cbi == END_OF_CBI_STAT_KIND );
  cons_track_prof_lkup_tbl();
  
  {
    int i = 0;
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      cons_cbtc_block_state ( &block_state[i] );
      i++;
    }
    assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
    assert( block_state[i].block_name == 0 );
    chk_consistency_over_sp_links();
  }
  
  {
    int i = 0;
    while( route_state[i].kind_route != END_OF_ROUTE_KINDS ) {
      cons_route_state( &route_state[i] );
      i++;
    }
    assert( route_state[i].kind_route == END_OF_ROUTE_KINDS );
    cons_route_prof_lkup_tbl();
  }
}

#if 0 // for MODULE-TEST
int main ( void ) {  
  cons_il_obj_tables();
  return 0;
}
#endif

static CBI_CTRL_STAT_INFO_PTR willing_2_recv_OC_stat ( TINY_SOCK_PTR pS, OC2ATS_STAT msg_id ) {
  assert( pS );
  assert( (msg_id >= 0) && (msg_id < END_OF_OC2ATS) );
  CBI_CTRL_STAT_INFO_PTR r = NULL;
  
  CBI_CTRL_STAT_INFO_PTR pOC = NULL;
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
    CBI_CTRL_STAT_INFO_PTR p = NULL;
    if( !(p = willing_2_recv_OC_stat( pS, (OC2ATS_STAT)i )) )
      goto exit;
    assert( p );
    assert( p->oc2ats.d_recv_cbi_stat > -1 );
    i++;
  }
  r = TRUE;
 exit:
  return r;
}

static CBI_CTRL_STAT_INFO_PTR willing_2_send_OC_ctrl ( TINY_SOCK_PTR pS, ATS2OC_CMD msg_id ) {
  assert( pS );
  assert( (msg_id >= ATS2OC801) && (msg_id < END_OF_ATS2OC) );
  CBI_CTRL_STAT_INFO_PTR r = NULL;
  
  CBI_CTRL_STAT_INFO_PTR pOC = NULL;
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
    
    pOC->ats2oc.d_sent_cbi_ctrl = -1;
    {
      TINY_SOCK_DESC d = -1;
      if( (d = creat_sock_sendnx( pS, pOC->ats2oc.dst_port, TRUE, &bcast_dst_ipaddr )) < 0 ) {
	errorF( "failed to create the socket to send CBI control commands  toward OC%d.\n", OC_ID_CONV2INT(msg_id) );
	goto exit;
      }
      pOC->ats2oc.d_sent_cbi_ctrl = d;
      sock_attach_send_buf( pS, pOC->ats2oc.d_sent_cbi_ctrl, (unsigned char *)&(pOC->ats2oc.sent.msgs[0].buf), (int)sizeof(pOC->ats2oc.sent.msgs[0].buf) );
    }
    r = pOC;
  }
 exit:
  return r;
}

BOOL establish_OC_ctrl_send ( TINY_SOCK_DESC *pds, const int ndescs, TINY_SOCK_PTR pS ) {
  assert( pds );
  assert( ndescs >= (int)END_OF_ATS2OC );
  assert( pS );
  BOOL r = FALSE;
  int i = (int)ATS2OC801;
  
  assert( i == 0 );
  while( i < (int)END_OF_ATS2OC ) {
    assert( (i >= ATS2OC801) && (i < (int)END_OF_ATS2OC) );
    CBI_CTRL_STAT_INFO_PTR p = NULL;
    if( !(p = willing_2_send_OC_ctrl( pS, (ATS2OC_CMD)i )) )
      goto exit;
    assert( p );
    assert( p->ats2oc.d_sent_cbi_ctrl > -1 );
    pds[i] = p->ats2oc.d_sent_cbi_ctrl;
    i++;
  }
  r = TRUE;
 exit:
  return r;
}

int conslt_il_state ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident ) {
  assert( poc_id );
  assert( pkind );
  assert( ident );
  int r = -1;
  CBI_STAT_ATTR_PTR pA = NULL;
  
  *poc_id = END_OF_OCs;
  *pkind = END_OF_CBI_STAT_KIND;
  pA = conslt_cbi_code_tbl( ident );
  if( pA ) {
    assert( pA );
    assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
    assert( ! strncmp(pA->ident, ident, CBI_STAT_IDENT_LEN) );
    int r_mutex = -1;
    r_mutex = pthread_mutex_trylock( &cbi_stat_info_mutex );
    if( !r_mutex ) {
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
      r_mutex = -1;
      r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
      assert( !r_mutex );
    }
  }
  return r;
}

static RECV_BUF_CBI_STAT_PTR update_cbi_status ( TINY_SOCK_PTR pS, OC2ATS_STAT msg_id ) {
  assert( pS );
  assert( (msg_id >= OC2ATS1) && (msg_id < END_OF_OC2ATS) );
  RECV_BUF_CBI_STAT_PTR r = NULL;
  CBI_CTRL_STAT_INFO_PTR pOC = NULL;
  
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
	if( len < OC2ATS1_MSGSIZE )
	  goto msg_size_err;
	break;
      case OC2ATS2:
	if( len < OC2ATS2_MSGSIZE )
	  goto msg_size_err;
	break;
      case OC2ATS3:
	if( len < OC2ATS3_MSGSIZE )
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
  assert( !r_mutex );
  
  return r;
}

static int expire_cbi_ctrl_bits ( OC_ID oc_id ) {
  assert( (oc_id >= OC801) && (oc_id < END_OF_OCs) );
  int cnt = 0;
  CBI_STAT_ATTR_PTR pA_ctl = NULL;
  
  pA_ctl = cbi_stat_prof[oc_id].pctrl_codes;
  while( pA_ctl ) {
    assert( pA_ctl->attr_ctrl.ctrl_bit );
    if( pA_ctl->attr_ctrl.ctrl_bit ) {
      assert( pA_ctl->attr_ctrl.cmd_id == (ATS2OC_CMD)oc_id );
      int r_mutex = -1;
      r_mutex = pthread_mutex_trylock( &cbi_ctrl_dispatch_mutex );
      if( !r_mutex ) {
	assert( pA_ctl->attr_ctrl.cnt_2_kil >= 0 );
	if( pA_ctl->attr_ctrl.cnt_2_kil > 0 ) {
	  pA_ctl->attr_ctrl.cnt_2_kil--;
	  if( pA_ctl->attr_ctrl.cnt_2_kil <= 0 ) {
	    assert( pA_ctl->attr_ctrl.cnt_2_kil == 0 );
	    pA_ctl->attr_ctrl.cnt_2_kil = 0;
	    assert( !(pA_ctl->dirty) );
	    assert( pA_ctl->attr_ctrl.setval );
	    assert( pA_ctl->pNext_dirt == NULL );
	    pA_ctl->attr_ctrl.setval = FALSE; //pA_ctl->attr_ctrl.setval = TRUE;
	    pA_ctl->dirty = TRUE;
	    pA_ctl->pNext_dirt = cbi_stat_prof[oc_id].pdirty_bits;
#if 1
	    cbi_stat_prof[oc_id].pdirty_bits = pA_ctl;
#endif
	    cnt++;
	  } else
	    assert( pA_ctl->attr_ctrl.cnt_2_kil > 0 );
	} else {
	  assert( pA_ctl->attr_ctrl.cnt_2_kil == 0 );
	  assert( !(pA_ctl->attr_ctrl.setval) );
	}
	r_mutex = pthread_mutex_unlock( &cbi_ctrl_dispatch_mutex );
	assert( !r_mutex );
      }
    }
    pA_ctl = pA_ctl->attr_ctrl.pNext_ctrl;
  }
  return cnt;
}

void *pth_expire_il_ctrl_bits ( void *arg ) {
  assert( !arg );
  const useconds_t interval = 1000 * 1000 * 1;
  int oc_id = (int)END_OF_OCs;
  
  while( TRUE ) {
    for( oc_id = OC801; oc_id < END_OF_OCs; oc_id++ ) {
      expire_cbi_ctrl_bits( oc_id );
    }
    {
      int r = -1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

static int render_il_ctrl_bits ( ATS2OC_CMD cmd_id ) {
  assert( cmd_id < END_OF_ATS2OC );
  int cnt = 0;
  CBI_STAT_ATTR_PTR *pphd = NULL;
  const unsigned char *plim = (unsigned char *)&cbi_stat_ATS2OC[cmd_id].ats2oc.sent.msgs[0].updated;
  unsigned char *pa = cbi_stat_ATS2OC[cmd_id].ats2oc.sent.msgs[0].buf.arena;
  assert( pa );
  
  int r_mutex_dispatch = -1;
  pphd = &cbi_stat_prof[cmd_id].pdirty_bits;
  assert( pphd );
  r_mutex_dispatch = pthread_mutex_trylock( &cbi_ctrl_dispatch_mutex );
  if( !r_mutex_dispatch ) {
    while( *pphd ) {
      assert( pphd );
      CBI_STAT_ATTR_PTR pA = *pphd;
      assert( pA );
      assert( pA->attr_ctrl.ctrl_bit );
      assert( pA->dirty );
      if( pA->attr_ctrl.ctrl_bit && pA->dirty ) {
	assert( pA->attr_ctrl.cnt_2_kil <= 0 );
	int r_mutex_sendbuf = -1;
	r_mutex_sendbuf = pthread_mutex_trylock( &cbi_ctrl_sendbuf_mutex );	
	if( !r_mutex_sendbuf ) {
	  assert( pA->attr_ctrl.cnt_2_kil == 0 );
	  unsigned char *p = &pa[pA->disp.bytes];
#if 1
	  assert( p < plim );
	  if( pA->attr_ctrl.setval ) {
	    *p |= pA->disp.mask;
	    pA->attr_ctrl.cnt_2_kil = CTRL_LIT_SUSTAIN_CNT;
	    printf( "ACTIVATED!\n" );
	  } else {
	    *p &= ~(pA->disp.mask);
	    assert( pA->attr_ctrl.cnt_2_kil == 0 );
	    printf( "INACTIVATED.\n" );
	  }
#endif
	  r_mutex_sendbuf = pthread_mutex_unlock( &cbi_ctrl_sendbuf_mutex );
	  assert( !r_mutex_sendbuf );
	} else
	  break;
      }
      pA->dirty = FALSE;
      *pphd = pA->pNext_dirt;
      pA->pNext_dirt = NULL;
      cnt++;
    }
    assert( pphd );
    if( !(*pphd) )
      cnt *= -1;
    r_mutex_dispatch = pthread_mutex_unlock( &cbi_ctrl_dispatch_mutex );
    assert( !r_mutex_dispatch );
  } else
    printf( "LOCK FAILed.\n" );
  return cnt;
}

void *pth_reveal_il_ctrl_bits ( void *arg ) {
  assert( arg );
  const useconds_t interval = 1000 * 1000 * 0.01;
  int oc_id = (int)END_OF_OCs;
  TINY_SOCK_PTR pS = NULL;
  
  pS = (TINY_SOCK_PTR)arg;
  assert( pS );
  while( TRUE ) {
    BOOL remains;;
    for( remains = FALSE, oc_id = OC801; oc_id < END_OF_OCs; oc_id++ ) {
      if( render_il_ctrl_bits( (ATS2OC_CMD)oc_id ) < 0 )
	remains = TRUE;
    }
    if( remains ) {
      continue;
    }
#if 0
    else {
#if 1
      BOOL inv_all = TRUE;
      int i = 0;
      while( i < (int)END_OF_ATS2OC ) {
	if( pS->send[i].dirty )
	  inv_all = FALSE;
	i++;
      }
      assert( !inv_all );
#endif
      if( sock_send( pS ) < 0 )
	errorF( "%s", "error on sending CBI control information toward OCs.\n" );
      else {
	;
	//printf( "BAR\n" );
      }
    }
#endif
    {
      int r = -1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

static int expire_cbi_status ( void ) {
  int r_mutex = -1;
  int i = -1;
  
  r_mutex = pthread_mutex_trylock( &cbi_stat_info_mutex );
  if( !r_mutex ) {
    i = (int)OC801;
    while( i < (int)END_OF_OCs ) {
      assert( (i >= OC801) && (i < END_OF_OCs) );
      int j;
      for( j = 0; j < OC_OC2ATS_MSGS_NUM; j++ )
	cbi_stat_info[i].msgs[j].updated = FALSE;
      i++;
    }
    assert( i == END_OF_OCs );
    r_mutex = -1;
    r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
    assert( !r_mutex );
  }
  return i;
}

static int reveal_il_status ( TINY_SOCK_PTR pS ) {
  assert( pS );
  int r = -1;
  int i = -1;
  
  r = expire_cbi_status();
  i = (int)OC2ATS1;
  while( i < (int)END_OF_OC2ATS ) {
    assert( (i >= OC2ATS1) && (i < END_OF_OC2ATS) );
    update_cbi_status( pS, (OC2ATS_STAT)i );
    i++;
  }
  assert( i == END_OF_OC2ATS );
  return r;
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
    } else {
      const int obsoleted = 100;
      BOOL clear = FALSE;
      int j = (int)OC801;
      clear = (reveal_il_status(pS) < 0) ? FALSE : TRUE;
      while( j < (int)END_OF_OCs ) {	
	assert( (j >= OC801) && (j < END_OF_OCs) );
	int k;
	for( k = 0; k < OC_OC2ATS_MSGS_NUM; k++ )
	  if( clear && cbi_stat_info[j].msgs[k].updated ) {
	    if( omits[j][k] < 0 )
	      errorF( "CBI status information of (OC%3d, OC2ATS%d) has started updating, again.\n", OC_ID_CONV2INT(j), OC_MSG_ID_CONV2INT(k) );
	    omits[j][k] = 0;
	  } else {
	    if( omits[j][k] >= obsoleted ) {
	      errorF( "CBI status information of (OC%3d, OC2ATS%d) has been obsoleted.\n", OC_ID_CONV2INT(j), OC_MSG_ID_CONV2INT(k) );
	      omits[j][k] = -1;
	    } else
	      if( omits[j][k] > -1 ) {
		assert( omits[j][k] < obsoleted );
		omits[j][k]++;
	      }
	  }
	j++;
      }
      assert( j == END_OF_OCs );
    }
    {
      int r = -1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

int engage_il_ctrl ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident ) {
  assert( poc_id );
  assert( pkind );
  assert( ident );
  int r = -1;
  int oc_id = END_OF_OCs;
  CBI_STAT_ATTR_PTR pA = NULL;
  
  *poc_id = END_OF_OCs;
  *pkind = END_OF_CBI_STAT_KIND;
  pA = conslt_cbi_code_tbl( ident );
  assert( pA );

  if( pA ) {
    assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
    oc_id = pA->oc_id;
    if( pA->attr_ctrl.ctrl_bit ) {
      assert( pA->attr_ctrl.cmd_id == (ATS2OC_CMD)oc_id );
      int r_mutex = -1;
      r_mutex = pthread_mutex_trylock( &cbi_ctrl_dispatch_mutex );
      if( !r_mutex ) {
	int state = -1;
	state = conslt_il_state( poc_id, pkind, ident );

	if( state >= 0 ) {
	  assert( *poc_id == oc_id );
	  assert( *pkind < END_OF_CBI_STAT_KIND );
	  r = 0;
	  if( state < 1 ) {
	    assert( state == 0 );
	    //if( !((pA->attr_ctrl.cnt_2_kil > 0) || (pA->dirty && pA->attr_ctrl.setval)) ) {
	    if( !((pA->attr_ctrl.cnt_2_kil > 0) || pA->dirty) ) {
	      assert( pA->attr_ctrl.cnt_2_kil <= 0 );
	      assert( !(pA->dirty) || !(pA->attr_ctrl.setval) );
	      pA->attr_ctrl.setval = TRUE;
	      pA->dirty = TRUE;
	      pA->pNext_dirt = cbi_stat_prof[oc_id].pdirty_bits;
	      cbi_stat_prof[oc_id].pdirty_bits = pA;
#if CHK_STRICT_CONSISTENCY
	      {
		CBI_STAT_ATTR_PTR p = cbi_stat_prof[oc_id].pdirty_bits;
		assert( p == pA );
		do {
		  assert( p->dirty );
		  p = p->pNext_dirt;
		} while( p );
	      }
#endif // CHK_STRICT_CONSISTENCY
	      r = 1;
	    }
	  } else {
	    assert( state >= 1 );
	    assert( r == 0 );
	  }
	} else {
	  assert( state < 0 );
	  assert( *poc_id == END_OF_OCs );
	  assert( *pkind == END_OF_CBI_STAT_KIND );
	  assert( r < 0 );
	}
	r_mutex = pthread_mutex_unlock( &cbi_ctrl_dispatch_mutex );
	assert( !r_mutex );
      } else {
	assert( *poc_id == END_OF_OCs );
	assert( *pkind == END_OF_CBI_STAT_KIND );
	assert( r < 0 );
      }
    } else {
      assert( *poc_id == END_OF_OCs );
      assert( *pkind == END_OF_CBI_STAT_KIND );
      assert( r < 0 );
    }
  } else {
    assert( *poc_id == END_OF_OCs );
    assert( *pkind == END_OF_CBI_STAT_KIND );
    assert( r < 0 );
  }
  return r;
}

int ungage_il_ctrl ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident ) {
  assert( poc_id );
  assert( pkind );
  assert( ident );
  int r = -1;
  int oc_id = END_OF_OCs;
  CBI_STAT_ATTR_PTR pA = NULL;
  
  *poc_id = END_OF_OCs;
  *pkind = END_OF_CBI_STAT_KIND;
  pA = conslt_cbi_code_tbl( ident );
  assert( pA );
  if( pA ) {
    assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
    oc_id = pA->oc_id;
    if( pA->attr_ctrl.ctrl_bit ) {
      assert( pA->attr_ctrl.cmd_id == (ATS2OC_CMD)oc_id );
      int r_mutex = -1;
      r_mutex = pthread_mutex_trylock( &cbi_ctrl_dispatch_mutex );
      if( !r_mutex ) {
	int state = -1;
	state = conslt_il_state( poc_id, pkind, ident );
	if( state >= 0 ) {
	  assert( *poc_id == oc_id );
	  assert( *pkind < END_OF_CBI_STAT_KIND );
	  r = 0;
	  if( state > 0 ) {
	    assert( state >= 1 );
	    if( !(pA->dirty) || pA->attr_ctrl.setval ) {
	      pA->attr_ctrl.cnt_2_kil = 0;
	      pA->attr_ctrl.setval = FALSE;
	      pA->dirty = TRUE;
	      pA->pNext_dirt = cbi_stat_prof[oc_id].pdirty_bits;
	      cbi_stat_prof[oc_id].pdirty_bits = pA;
#if CHK_STRICT_CONSISTENCY
	      {
		CBI_STAT_ATTR_PTR p = cbi_stat_prof[oc_id].pdirty_bits;
		assert( p == pA );
		do {
		  assert( p->dirty );
		  p = p->pNext_dirt;
		} while( p );
	      }
#endif // CHK_STRICT_CONSISTENCY
	      r = 1;
	    }
	  } else {
	    assert( state == 0 );
	    assert( r == 0 );
	  }
	} else {
	  assert( state < 0 );
	  assert( *poc_id == END_OF_OCs );
	  *pkind = END_OF_CBI_STAT_KIND;
	  assert( r < 0 );
	}
	r_mutex = pthread_mutex_unlock( &cbi_ctrl_dispatch_mutex );
	assert( !r_mutex );
      } else {
	assert( *poc_id == END_OF_OCs );
	*pkind = END_OF_CBI_STAT_KIND;
	assert( r < 0 );
      }
    } else {
      assert( *poc_id == END_OF_OCs );
      *pkind = END_OF_CBI_STAT_KIND;
      assert( r < 0 );
    }
  } else {
    assert( *poc_id == END_OF_OCs );
    *pkind = END_OF_CBI_STAT_KIND;
    assert( r < 0 );
  }
  return r;
}

void diag_cbi_stat_attrib ( FILE *fp_out, char *ident ) {
  assert( fp_out );
  assert( ident );
  CBI_STAT_ATTR_PTR pA = NULL;
  
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
#if 0
    {
      int r_mutex = -1;
      r_mutex = pthread_mutex_trylock( &cbi_stat_info_mutex );
      if( !r_mutex ) {
	assert( pA );
	assert( (pA->oc_id >= OC801) && (pA->oc_id < END_OF_OCs) );
	RECV_BUF_CBI_STAT_PTR pstat = NULL;
	pstat = &cbi_stat_info[pA->oc_id];
	assert( pstat );
	{
	  unsigned char *pgrp = &((unsigned char *)&pstat->msgs[pA->group.msg_id])[pA->group.addr];
	  assert( pgrp );
	  fprintf( fp_out, "value: %d\n", (pgrp[pA->disp.bytes] & pA->disp.mask) );
	}
	r_mutex = -1;
	r_mutex = pthread_mutex_unlock( &cbi_stat_info_mutex );
	assert( !r_mutex );
      }
    }
#else
    {
      OC_ID oc_id = END_OF_OCs;
      CBI_STAT_KIND kind = END_OF_CBI_STAT_KIND;
      int state = -1;
      state = conslt_il_state( &oc_id, &kind, pA->ident );
      if( state > -1 ) {
	assert( oc_id == pA->oc_id );     
	assert( kind == pA->kind );
	fprintf( fp_out, "value: %d\n", state );
      } else
	fprintf( fp_out, "value: %d\n", -1 );
    }
#endif
  }
}

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

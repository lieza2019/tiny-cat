#include <string.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs.h"

TINY_TRAIN_STATE trains_tracking[MAX_TRAIN_TRACKINGS];
static int frontier;

static TINY_TRAIN_STATE_PTR enum_orphant_trains ( void ) {
  TINY_TRAIN_STATE_PTR r = NULL;
  {
    TINY_TRAIN_STATE_PTR *pp = &r;
    int i;
    for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
      if( ! (trains_tracking[i].omit || trains_tracking[i].updated) ) {
	*pp = &trains_tracking[i];
	trains_tracking[i].pNext = NULL;
	pp = &trains_tracking[i].pNext;
	trains_tracking[i].omit = TRUE;
      }
    assert( *pp == NULL );
  }
  return r;
}

static BOOL exam_consistency_with_train_info ( TINY_TRAIN_STATE_PTR pE, TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pE );
  assert( pI );
  return TRUE;
}

static void retrieve_from_train_info ( TINY_TRAIN_STATE_PTR pS, TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pS );
  assert( pI );
  pS->rakeID = TRAIN_INFO_RAKEID( *pI );
  pS->skip_next_stop = TRAIN_INFO_SKIP_NEXT_STOP( *pI );
  pS->perf_regime = TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( *pI );
}

static TINY_TRAIN_STATE_PTR update_train_state( TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pI );
  TINY_TRAIN_STATE_PTR pE = NULL;
  unsigned short rID = 0;
  
  rID = TRAIN_INFO_RAKEID( *pI );
  assert( rID > 0 );
  {
    int i;
    assert( frontier < MAX_TRAIN_TRACKINGS );
    for( i = 0; i < frontier; i++ )
      if( trains_tracking[i].rakeID == rID ) {
	pE = &trains_tracking[i];
	if( ! exam_consistency_with_train_info( pE, pI ) ) {
	  errorF( "%-3d: inconsistency detected in comparing with the train information.\n", trains_tracking[i].rakeID );
	}
	break;
      }
    if( !pE ) {
      assert( frontier < MAX_TRAIN_TRACKINGS );
      assert( i == frontier );
      pE = &trains_tracking[frontier];
      retrieve_from_train_info( pE, pI );
      frontier++;
    }
    assert( pE );
    pE->pTI = pI;
    pE->updated = TRUE;
    pE->omit = FALSE;
  }
  return pE;
}

static void expire_all_train_cmds ( void ) {
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    int j;
    for( j = 0; j < TRAIN_COMMAND_ENTRIES_NUM; j++ )
      SC_ctrl_cmds[i].train_command.expired[j] = TRUE;
  }
}

static void expire_all_train_states ( void ) {
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    trains_tracking[i].updated = FALSE;
}

void reveal_train_tracking( TINY_SOCK_PTR pS ) {
  assert( pS );
  int i;
  
  expire_all_train_states();
  for( i = (int)SC801; i < END_OF_SCs; i++ ) {
    SC_STAT_INFOSET_PTR pSC = NULL;
    pSC = snif_train_info( pS, (SC_ID)i );
    if( pSC ) {
      int j;
      for( j = 0; j < TRAIN_INFO_ENTRIES_NUM; j++ ) {
	TINY_TRAIN_STATE_PTR pE = NULL;
	unsigned short rakeID = TRAIN_INFO_RAKEID(pSC->train_information.recv.train_info.entries[j]);
	if( rakeID != 0 ) {
	  //printf( "received rakeID in the %2d th Train info.: %3d.\n", (i + 1), rakeID );  // ***** for debugging.
	  pE = update_train_state( &pSC->train_information.recv.train_info.entries[j] );
	  assert( pE );
	  pSC->train_information.pTrain_stat[i] = pE;
	}
      }
    }
  }
  
  {
    TINY_TRAIN_STATE_PTR pOph = NULL;
    pOph = enum_orphant_trains();
    while( pOph ) {
      errorF( "%-3d: lost in observing the train information.\n", pOph->rakeID );
      pOph = pOph->pNext;
    }
  }
}

static SC_STAT_INFOSET_PTR willing_to_recv_train_info ( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  assert( (sc_id >= 0) && (sc_id < END_OF_SCs) );
  SC_STAT_INFOSET_PTR r = NULL;
  
  SC_STAT_INFOSET_PTR pSC = NULL;
  pSC = &SC_stat_infos[sc_id];
  assert( pSC );
  {
    TINY_SOCK_DESC d = -1;
    pSC->train_information.d_recv_train_info = -1;
    if( (d = creat_sock_recv( pS, pSC->train_information.dst_port )) < 0 ) {
      errorF( "failed to create the socket to receive Train information from SC%d.\n", SC_ID_CONV_2_INT(sc_id) );
      goto exit;
    }
    pSC->train_information.d_recv_train_info = d;
    sock_attach_recv_buf( pS, d, (unsigned char *)&(pSC->train_information.recv), sizeof(pSC->train_information.recv) );
    r = pSC;
  }
  
 exit:
  return r;
}

static void expire_all_train_state ( void ) {
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    trains_tracking[i].omit = TRUE;
}

static BOOL establish_SC_statinfo_recv ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  {
    int i = (int)SC801;
    while( i < (int)END_OF_SCs ) {
      SC_STAT_INFOSET_PTR p = NULL;
      expire_all_train_state();
      if( ! (p = willing_to_recv_train_info( pS, (SC_ID)i )) )
	goto exit;
      assert( p );
      assert( p->train_information.d_recv_train_info > -1 );
      i++;
    }
    r = TRUE;
  } 
 exit:
  return r;
}

static SC_CTRL_CMDSET_PTR willing_to_send_train_cmd ( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  assert( (sc_id >= 0) && (sc_id < END_OF_SCs) );
  SC_CTRL_CMDSET_PTR r = NULL;
  
  SC_CTRL_CMDSET_PTR pSC = NULL;
  pSC = &SC_ctrl_cmds[sc_id];
  assert( pSC );
  {
    IP_ADDR_DESC bcast_dst_ipaddr = SC_ctrl_cmds[sc_id].sc_ipaddr;    
    bcast_dst_ipaddr.oct_3rd = 255;
    bcast_dst_ipaddr.oct_4th = 255;
    
    pSC->train_command.d_send_train_cmd = -1;
    {
      TINY_SOCK_DESC d = -1;
      if( (d = creat_sock_sendnx( pS, pSC->train_command.dst_port, TRUE, &bcast_dst_ipaddr )) < 0 ) {
	errorF( "failed to create the socket to send Train command toward SC%d.\n", SC_ID_CONV_2_INT(sc_id) );
	goto exit;
      }
      pSC->train_command.d_send_train_cmd = d;
      sock_attach_send_buf( pS, pSC->train_command.d_send_train_cmd, (unsigned char *)&(pSC->train_command.send), (int)sizeof(pSC->train_command.send) );
    }
    r = pSC;
  }
  
 exit:
  return r;
}

static BOOL establish_SC_traincmd_send ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  {
    int i = (int)SC801;
    while( i < (int)END_OF_SCs ) {
      SC_CTRL_CMDSET_PTR p = NULL;
      expire_all_train_cmds();
      if(! (p = willing_to_send_train_cmd( pS, (SC_ID)i )) )
	goto exit;
      assert( p );
      assert( p->train_command.d_send_train_cmd > -1 );
      i++;
    }
    r = TRUE;
  }
 exit:
  return r;
}

BOOL establish_SC_comm ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  
  if( establish_SC_statinfo_recv( pS ) )
    if( establish_SC_traincmd_send( pS ) )
    r = TRUE;
  
  return r;
}

static void cons_train_cmd ( TINY_TRAIN_STATE_PTR pTs ) {
  assert( pTs );
  int i;
  
  for( i = 0; i < 2; i++ ) {
    if( pTs->pTC[i] ) {
      TRAIN_COMMAND_ENTRY_PTR pE = pTs->pTC[i];
      assert( pE );
      assert( TRAIN_INFO_RAKEID(*pE) == pTs->rakeID );
#if 0
      {
	char buf[TRAINID_MAX_LEN + 1];
	buf[TRAINID_MAX_LEN] = 0;
	assert( ! strncmp( pTs->trainID, TRAIN_CMD_TRAINID ( *pE, buf, TRAINID_MAX_LEN ), TRAINID_MAX_LEN ) );
      }
#endif     
      TRAIN_CMD_DESTINATION_BLOCKID( *pE, pTs->dest_blockID );
      TRAIN_CMD_SKIP_NEXT_STOP( *pE, pTs->skip_next_stop );
      TRAIN_CMD_ATO_DEPARTURE_COMMAND( *pE, pTs->ATO_dept_cmd );
      TRAIN_CMD_TRAIN_HOLD_COMMAND( *pE, pTs->TH_cmd );
      TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( *pE, pTs->perf_regime );
      TRAIN_CMD_TURNBACK_OR_SIDING( *pE, pTs->turnback_siding );
      TRAIN_CMD_DWELL_TIME( *pE, pTs->dwell_time );
      TRAIN_CMD_TRAIN_REMOVE( *pE, pTs->train_remove );
      TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( *pE, pTs->releasing_emergency_stop );
      TRAIN_CMD_ORDERING_EMERGENCY_STOP( *pE, pTs->ordering_emergency_stop );
      TRAIN_CMD_AUTOMATIC_TURNBACK_COMMAND( *pE, pTs->ATB_cmd );
    }
  }
}

static void purge_train_cmds ( void ) {
  int i;
  for ( i = 0; i < END_OF_SCs; i++ ) {
    int j;
    for( j = 0; j < TRAIN_COMMAND_ENTRIES_NUM; j++ )
      if( SC_ctrl_cmds[i].train_command.expired[j] ) {
	TRAIN_CMD_RAKEID( SC_ctrl_cmds[i].train_command.send.train_cmd.entries[j], 0 );
      }
  }
}

static void fine_train_cmds ( void ) {
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    SC_CTRL_CMDSET_PTR pSc = &SC_ctrl_cmds[i];
    assert( pSc );
    int maiden = -1;
    int j = 0;
    while( j < TRAIN_COMMAND_ENTRIES_NUM ) {
      if( pSc->train_command.send.train_cmd.entries[j].rakeID == 0 ) {
	assert( pSc->train_command.expired[j] );
	if( maiden < 0 )
	  maiden = j;
	j++;
      } else if( maiden >= 0 ) {
	pSc->train_command.send.train_cmd.entries[maiden] = pSc->train_command.send.train_cmd.entries[maiden + 1];
	j = maiden + 1;
	maiden = -1;
      } else
	j++;
      assert( maiden < TRAIN_COMMAND_ENTRIES_NUM );
    }
    if( maiden >= 0 ) {
      int n = TRAIN_COMMAND_ENTRIES_NUM - maiden;
      memset( &pSc->train_command.send.train_cmd.entries[maiden], 0, ((int)sizeof(TRAIN_COMMAND_ENTRY) * n) );
    }
  }
}

int load_train_command ( void ) {
  int cnt = 0;
  int i;
  
  assert( frontier < MAX_TRAIN_TRACKINGS );
  for( i = 0; i < frontier; i++ ) {
    TINY_TRAIN_STATE_PTR pstat = NULL;
    pstat = &trains_tracking[i];
    assert( pstat );
    if( pstat->rakeID > 0 ) {
      if( pstat->omit )
	continue;
      else {
	TRAIN_COMMAND_ENTRY_PTR es[2] = {NULL, NULL};
	int front_blk = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *(pstat->pTI) );
	int back_blk = TRAIN_INFO_OCCUPIED_BLK_BACK( *(pstat->pTI) );
	int n = alloc_train_cmd_entries( es, pstat, pstat->rakeID, front_blk, back_blk );
	assert( (n > 0) && (n <= 2) );
	{
	  int j;
	  for( j = 0; j < n; j++ )
	    pstat->pTC[j] = es[j];
	  assert( (j > 0) && (j <= 2) );
	  if( j < 2 ) {
	    assert( j == 1 );
	    pstat->pTC[j] = NULL;
	  }
	}
	cons_train_cmd( pstat );
	cnt++;
      }
    }
  }
  purge_train_cmds();
  
  while( standby_train_cmds.phd ) {
    assert( standby_train_cmds.pptl );
    TINY_TRAIN_STATE_PTR pstat = standby_train_cmds.phd;
    assert( pstat );
    assert( pstat->rakeID > 0 );
    assert( ! pstat->omit );
    TRAIN_COMMAND_ENTRY_PTR es[2] = {NULL, NULL};
    int front_blk = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *(pstat->pTI) );
    int back_blk = TRAIN_INFO_OCCUPIED_BLK_BACK( *(pstat->pTI) );
    int n = standup_train_cmd_entries( es, pstat, pstat->rakeID, front_blk, back_blk );
    assert( (n > 0) && (n <= 2) );
    {
      int k;
      for( k = 0; k < n; k++ )
	pstat->pTC[k] = es[k];
      assert( (k > 0) && (k <= 2) );
      if( k < 2 ) {
	assert( k == 1 );
	pstat->pTC[k] = NULL;
      }
    }
    cnt++;
    assert( standby_train_cmds.phd->pNext ? TRUE : standby_train_cmds.pptl == &(standby_train_cmds.phd->pNext) );
    standby_train_cmds.phd = standby_train_cmds.phd->pNext;
  }
  
  fine_train_cmds();
  return cnt;
}

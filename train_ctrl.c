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
  pS->perf_regime_cmd = TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( *pI );
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

static void expire_train_state ( void ) {
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    trains_tracking[i].updated = FALSE;
}

void reveal_train_tracking( TINY_SOCK_PTR pS ) {
  assert( pS );
  int i;
  
  expire_train_state();
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
    if( (d = creat_sock_bcast_recv( pS, pSC->train_information.dst_port )) < 0 ) {
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
  
  int i = (int)SC801;
  while( i < (int)END_OF_SCs ) {
    SC_STAT_INFOSET_PTR p = NULL;
    expire_all_train_state();
    if( ! (p = willing_to_recv_train_info( pS, (SC_ID)i )) )
      goto exit;
    assert( p );
    assert( p->train_information.d_recv_train_info >= 0 );
    i++;
  }
  r = TRUE;
  
 exit:
  return r;
}

BOOL establish_SC_comm ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL r = FALSE;
  
  if( establish_SC_statinfo_recv( pS ) )
    r = TRUE;
  
  return r;
}

static void cons_train_cmd( TINY_TRAIN_STATE_PTR pTs ) {
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
      TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( *pE, pTs->perf_regime_cmd );
      TRAIN_CMD_TURNBACK_OR_SIDING( *pE, pTs->turnback_siding );
      TRAIN_CMD_DWELL_TIME( *pE, pTs->dwell_time );
      TRAIN_CMD_TRAIN_REMOVE( *pE, pTs->train_remove );
      TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( *pE, pTs->releasing_emergency_stop );
      TRAIN_CMD_ORDERING_EMERGENCY_STOP( *pE, pTs->ordering_emergency_stop );
      TRAIN_CMD_AUTOMATIC_TURNBACK_COMMAND( *pE, pTs->ATB_cmd );
    }
  }
}

int charge_train_command( void ) {
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
	int n = alloc_train_cmd_entries( es, pstat->rakeID, front_blk, back_blk );
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
  return cnt;
}

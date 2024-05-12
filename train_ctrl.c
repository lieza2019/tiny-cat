#include <string.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

#define MAX_TRAIN_TRACKINGS 1024

static TINY_TRAIN_STATE tracking_trains[MAX_TRAIN_TRACKINGS];
static int frontier;

static void retrieve_state( TINY_TRAIN_STATE_PTR pE, TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pE );
  assert( pI );
  pE->skip_next_stop = TRAIN_INFO_SKIP_NEXT_STOP( *pI );
  pE->perf_regime_cmd = TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( *pI );
}

static TINY_TRAIN_STATE_PTR update_train_state( TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pI );
  TINY_TRAIN_STATE_PTR pE = NULL;
  unsigned short rID = 0;
  rID = TRAIN_INFO_RAKEID( *pI );
  {
    int i;
    assert( frontier < MAX_TRAIN_TRACKINGS );
    for( i = 0; i < frontier; i++ )
      if( tracking_trains[i].rakeID == rID ) {
	pE = &tracking_trains[i];
	break;
      }
    if( !pE ) {
      assert( i == frontier );
      frontier = i;
      pE = &tracking_trains[frontier];
      retrieve_state( pE, pI );
    }
    pE->expired = TRUE;
  }
  return pE;
}

void update_train_tracking( void ) {
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    SC_STAT_INFOSET_PTR pSC = NULL;
    pSC = sniff_train_info( (SC_ID)i );
    if( pSC ) {
      int j;
      for( j = 0; j < MAX_TRAIN_INFO_ENTRIES; j++ )
	if( pSC->train_info.entries[j].rakeID > 0 )
	  update_train_state( &pSC->train_info.entries[j] );
    }
  }
}

static void construct_cmd( TINY_TRAIN_STATE_PTR pTs ) {
  assert( pTs );
  int i;
  
  for( i = 0; i < 2; i++ ) {
    if( pTs->pTC[i] ) {
      TRAIN_COMMAND_ENTRY_PTR pE = pTs->pTC[i];
      assert( pE );
      assert( pTs ->rakeID == TRAIN_INFO_RAKEID( *pE ) );
      {
	char buf[TRAINID_MAX_LEN + 1];
	buf[TRAINID_MAX_LEN] = 0;
	assert( ! strncmp( pTs->trainID, TRAIN_CMD_TRAINID ( *pE, buf, TRAINID_MAX_LEN ), TRAINID_MAX_LEN ) );
      }
      assert( pTs->dest_blockID == TRAIN_CMD_DESTINATION_BLOCKID( *pE, pTs->dest_blockID ) );
      assert( pTs->skip_next_stop == TRAIN_CMD_SKIP_NEXT_STOP( *pE, pTs->skip_next_stop ) );
      assert( pTs->ATO_dept_cmd == TRAIN_CMD_ATO_DEPARTURE_COMMAND( *pE, pTs->ATO_dept_cmd ) );
      assert( pTs->TH_cmd == TRAIN_CMD_TRAIN_HOLD_COMMAND( *pE, pTs->TH_cmd ) );
      assert( pTs->perf_regime_cmd == TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( *pE, pTs->perf_regime_cmd ) );
      assert( pTs->turnback_siding == TRAIN_CMD_TURNBACK_OR_SIDING( *pE, pTs->turnback_siding ) );
      assert( pTs->dwell_time == TRAIN_CMD_DWELL_TIME( *pE, pTs->dwell_time ) );
      assert( pTs->train_remove == TRAIN_CMD_TRAIN_REMOVE( *pE, pTs->train_remove ) );
      assert( pTs->releasing_emergency_stop == TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( *pE, pTs->releasing_emergency_stop ) );
      assert( TRAIN_CMD_ORDERING_EMERGENCY_STOP( *pE, pTs->ordering_emergency_stop ) );
    }
  }
}

int charge_train_command( void ) {
  int cnt = 0;
  int i;
  
  assert( frontier < MAX_TRAIN_TRACKINGS );
  for( i = 0; i < frontier; i++ ) {
    TINY_TRAIN_STATE_PTR pstat = NULL;
    pstat = &tracking_trains[i];
    assert( pstat );
    if( (pstat->rakeID > 0) && (! pstat->expired) ) {
      TRAIN_COMMAND_ENTRY_PTR es[2] = {NULL, NULL};
      int front_blk = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *(pstat->pTI) );
      int back_blk = TRAIN_INFO_OCCUPIED_BLK_BACK( *(pstat->pTI) );
      int n = alloc_train_cmd_entries( es, pstat->rakeID, front_blk, back_blk );
      assert( (n > 0) && (n <= 2) );
      {
	int j;
	for( j = 0; j < n; j++ )
	  pstat->pTC[j] = es[j];
	assert( j > 0 );
	if( j < 2 )
	  pstat->pTC[j] = NULL;
      }
      construct_cmd( pstat );
      cnt++;
    }
  }
  return cnt;
}

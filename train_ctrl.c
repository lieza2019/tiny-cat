#include <string.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs.h"

TINY_TRAIN_STATE trains_tracking[MAX_TRAIN_TRACKINGS];
static int frontier;

static void elide_block_restrain ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  if( pT->pTI ) {
    assert( pT->rakeID == TRAIN_INFO_RAKEID(*pT->pTI) );
    CBTC_BLOCK_PTR pblk_front = NULL;
    const unsigned short blk_name = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pT->pTI );
    pblk_front = lookup_cbtc_block_prof( blk_name );
    if( pblk_front ) {
      BOOL found_and_elided = FALSE;
      TINY_TRAIN_STATE_PTR *pp = NULL;
      pp = addr_residents_CBTC_BLOCK ( pblk_front );
      assert( pp );
      while( *pp ) {
	assert( *pp );
	assert( pT );
	if( *pp == pT ) {
	  assert( !found_and_elided );
	  *pp = pT->occupancy.front.pNext;
	  pT->occupancy.front.pNext = NULL;
	  found_and_elided = TRUE;
	  continue;
	}
	pp = &(*pp)->occupancy.front.pNext;
	assert( pp );
      }
    } else
      errorF( "%d: unknown cbtc block detected as occupied forward one, of train %3d.\n", blk_name, pT->rakeID );
  }
}

static TINY_TRAIN_STATE_PTR put_block_restrain ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  TINY_TRAIN_STATE_PTR r = NULL;
  
  if( pT->pTI ) {
    assert( pT->rakeID == TRAIN_INFO_RAKEID(*pT->pTI) );
    CBTC_BLOCK_PTR pblk_front = NULL;
    const unsigned short blk_name = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pT->pTI );
    pblk_front = lookup_cbtc_block_prof( blk_name );
    if( pblk_front ) {
      pT->occupancy.front.pNext = read_residents_CBTC_BLOCK( pblk_front );
      r = write_residents_CBTC_BLOCK( pblk_front, pT );
    } else
    errorF( "%d: unknown cbtc block detected as occupied forward one, of train %3d.\n", blk_name, pT->rakeID );  
  }
  return r;
}

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

static TINY_TRAIN_STATE_PTR update_train_state ( TRAIN_INFO_ENTRY_PTR pI ) {
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
    assert( (i >= SC801) && (i < END_OF_SCs) );
    SC_STAT_INFOSET_PTR pSC = NULL;
    pSC = snif_train_info( pS, (SC_ID)i );
    if( pSC ) {
      int j;
      for( j = 0; j < TRAIN_INFO_ENTRIES_NUM; j++ ) {
	TINY_TRAIN_STATE_PTR pE = NULL;
	unsigned short rakeID = TRAIN_INFO_RAKEID(pSC->train_information.recv.train_info.entries[j]);
	if( rakeID != 0 ) {
	  //printf( "received rakeID in the %2d th Train info.: %3d.\n", (i + 1), rakeID );  // ***** for debugging.
	  elide_block_restrain( pE );
	  pE = update_train_state( &pSC->train_information.recv.train_info.entries[j] );
	  assert( pE );
	  put_block_restrain( pE );
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
  pSC = &SC_stat_infos[(int)sc_id];
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
  
  int i = (int)SC801;
  while( i < (int)END_OF_SCs ) {
    assert( (i >= (int)SC801) && (i < (int)END_OF_SCs) );
    SC_STAT_INFOSET_PTR p = NULL;
    expire_all_train_state();
    if( !(p = willing_to_recv_train_info( pS, (SC_ID)i )) )
      goto exit;
    assert( p );
    assert( p->train_information.d_recv_train_info > -1 );
    i++;
  }
  r = TRUE;
 exit:
  return r;
}

static SC_CTRL_CMDSET_PTR willing_to_send_train_cmd ( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  assert( (sc_id >= 0) && (sc_id < END_OF_SCs) );
  SC_CTRL_CMDSET_PTR r = NULL;
  
  SC_CTRL_CMDSET_PTR pSC = NULL;
  pSC = &SC_ctrl_cmds[(int)sc_id];
  assert( pSC );
  {
    IP_ADDR_DESC bcast_dst_ipaddr = SC_ctrl_cmds[(int)sc_id].sc_ipaddr;
    assert( bcast_dst_ipaddr.oct_1st != 0 );
    assert( bcast_dst_ipaddr.oct_2nd != 0 );
    assert( bcast_dst_ipaddr.oct_3rd != 0 );
    assert( bcast_dst_ipaddr.oct_4th != 0 );
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
  
  int i = (int)SC801;
  while( i < (int)END_OF_SCs ) {
    assert( (i >= (int)SC801) && (i < (int)END_OF_SCs) );
    SC_CTRL_CMDSET_PTR p = NULL;
    expire_all_train_cmds();
    if(! (p = willing_to_send_train_cmd( pS, (SC_ID)i )) )
      goto exit;
    assert( p );
    assert( p->train_command.d_send_train_cmd > -1 );
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
      assert( ntohs(pE->rakeID) == pTs->rakeID );
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
	pSc->train_command.send.train_cmd.entries[maiden] = pSc->train_command.send.train_cmd.entries[j];
	{
	  TINY_TRAIN_STATE_PTR pTs = pSc->train_command.pTrain_stat[j];
	  assert( pTs );
	  if( pTs->pTC[0] == &pSc->train_command.send.train_cmd.entries[j] )
	    pTs->pTC[0] = &pSc->train_command.send.train_cmd.entries[maiden];
	  else if( pTs->pTC[1] == &pSc->train_command.send.train_cmd.entries[j] )
	    pTs->pTC[1] = &pSc->train_command.send.train_cmd.entries[maiden];
	  else
	    assert( FALSE );
	  pSc->train_command.pTrain_stat[maiden] = pTs;
	}
	pSc->train_command.expired[maiden] = FALSE;
	
	TRAIN_CMD_RAKEID( pSc->train_command.send.train_cmd.entries[j], 0 );
	pSc->train_command.expired[j] = TRUE;
	j = maiden + 1;
	maiden = -1;
      } else
	j++;
      assert( maiden < TRAIN_COMMAND_ENTRIES_NUM );
    }
    pSc->train_command.frontier = TRAIN_COMMAND_ENTRIES_NUM;
    if( maiden >= 0 ) {
      int n = TRAIN_COMMAND_ENTRIES_NUM - maiden;
      memset( &pSc->train_command.send.train_cmd.entries[maiden], 0, ((int)sizeof(TRAIN_COMMAND_ENTRY) * n) );
      pSc->train_command.frontier = maiden;
    }
    assert( pSc->train_command.frontier <= TRAIN_COMMAND_ENTRIES_NUM );
  }
}

int load_train_command ( void ) {
  int cnt = 0;
  int i;
  
  expire_all_train_cmds();
  for( i = 0; i < frontier; i++ ) {
    assert( frontier < MAX_TRAIN_TRACKINGS );
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

static int enum_alive_rakes ( int (*rakeIDs)[TRAIN_INFO_ENTRIES_NUM + 1] ) {
  struct {
    char sc_name[6];
    SC_ID sc_id;
    int num_of_rakes;
  } lkup[END_OF_SCs] = {
    {"SC801", SC801, 0},
    {"SC802", SC802, 0},
    {"SC803", SC803, 0},
    {"SC804", SC804, 0},
    {"SC805", SC805, 0},
    {"SC806", SC806, 0},
    {"SC807", SC807, 0},
    {"SC808", SC808, 0},
    {"SC809", SC809, 0},
    {"SC810", SC810, 0},
    {"SC811", SC811, 0},
    {"SC812", SC812, 0},
    {"SC813", SC813, 0},
    {"SC814", SC814, 0},
    {"SC815", SC815, 0},
    {"SC816", SC816, 0},
    {"SC817", SC817, 0},
    {"SC818", SC818, 0},
    {"SC819", SC819, 0},
    {"SC820", SC820, 0},
    {"SC821", SC821, 0}
  };
  int cnt = 0;
  int i;
  {
    int sc;
    for( sc = SC801; sc < END_OF_SCs; sc++ )
      rakeIDs[sc][0] = -1;
  }
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
      TRAIN_INFO_ENTRY_PTR pE = trains_tracking[i].pTI;
      assert( pE );
      SC_STAT_INFOSET_PTR pSi = which_SC_from_train_info( pE );
      assert( pSi );
      int j;
      for( j = 0; j < END_OF_SCs; j++ )
	if( ! strncmp( pSi->sc_name, lkup[j].sc_name, strlen("SC8xx") ) )
	  break;
      assert( j < END_OF_SCs );
      rakeIDs[lkup[j].sc_id][lkup[j].num_of_rakes] = (int)TRAIN_INFO_RAKEID( *pE );
      lkup[j].num_of_rakes++;
      rakeIDs[lkup[j].sc_id][lkup[j].num_of_rakes] = -1;
      cnt++;
    }
  return cnt;
}

static void dup_book ( int rakeIDs_dup[], int rakeIDs_org[], int num_of_rakes ) {
  assert( rakeIDs_dup );
  assert( rakeIDs_org );
  assert( (num_of_rakes > -1) && (num_of_rakes <= TRAIN_COMMAND_ENTRIES_NUM) );
  int i;
  for( i = 0; i < num_of_rakes; i++ )
    rakeIDs_dup[i] = rakeIDs_org[i];
  assert( i == num_of_rakes );
  rakeIDs_dup[i] = -1;
}
static void elim_rake( int rakeIDs[], int elim, int num_of_elems ) {
  assert( rakeIDs );
  assert( (num_of_elems > -1) && (num_of_elems <= TRAIN_COMMAND_ENTRIES_NUM) );
  assert( (elim > -1) && (elim < num_of_elems) );
  int i = elim;
  assert( rakeIDs[num_of_elems] < 0 );
  while( i < num_of_elems ) {
    assert( i < num_of_elems );
    rakeIDs[i] = rakeIDs[i + 1];
    i++;
  }
  assert( num_of_elems > 0 ? (rakeIDs[num_of_elems - 1] < 0) : (rakeIDs[0] < 0) );
}

static BOOL chk_consistency ( int rakeIDs[], int num_of_rakes ) {
  assert( rakeIDs );
  assert( num_of_rakes > -1 );
  BOOL r = TRUE;
  if( num_of_rakes > 1 ) {
    int rID = rakeIDs[0];
    int i;
    for( i = 1; i < num_of_rakes; i++ )
      if( rID == rakeIDs[i] ) {
	r = FALSE;
	break;
      }
    if( r )
      r = chk_consistency( &rakeIDs[1], (num_of_rakes - 1) );
  }
  return r;
}

static void chk_massiv_train_cmds_array ( SC_ID sc_id, int rakeIDs[], int num_of_rakes ) {
  assert( sc_id < END_OF_SCs );
  assert( rakeIDs );
  assert( (num_of_rakes > -1) && (num_of_rakes <= TRAIN_COMMAND_ENTRIES_NUM) );
  assert( chk_consistency( rakeIDs, num_of_rakes ) );
  const TRAIN_COMMAND_ENTRY_PTR plim = &SC_ctrl_cmds[sc_id].train_command.send.train_cmd.entries[TRAIN_COMMAND_ENTRIES_NUM];
  TRAIN_COMMAND_ENTRY_PTR pE = &SC_ctrl_cmds[sc_id].train_command.send.train_cmd.entries[0];
  assert( pE );
  {
    int rakeIDs_w[TRAIN_COMMAND_ENTRIES_NUM + 1];
    dup_book( rakeIDs_w, rakeIDs, num_of_rakes );
    while( rakeIDs_w[0] > -1 ) {
      assert( pE < plim );
      BOOL found = FALSE;
      TRAIN_COMMAND_ENTRY_PTR pEp = pE;
      int rID = ntohs( pE->rakeID );
      int j = 0;
      while( rakeIDs_w[j] > -1 ) {
	if( rID == rakeIDs_w[j] ) {
	  int len = 0;
	  while( rakeIDs_w[len] > -1 )
	    len++;
	  elim_rake( rakeIDs_w, j, len );
	  pE++;
	  found = TRUE;
	  break;
	} else
	  j++;
      }
      if( found )
	assert( pE == ++pEp );
      else
	assert( FALSE );
    }
  }
  assert( num_of_rakes < TRAIN_COMMAND_ENTRIES_NUM ? pE < plim : pE == plim );
  
  if( pE < plim ) {
    const int n = (int)((unsigned char *)plim - (unsigned char *)pE);
    assert( n > 0 );
    unsigned char *q = (unsigned char *)pE;
    assert( q );
    int k;
    for( k = 0; k < n; k++ ) {
      assert( *q == 0x00 );
      q++;
    }
  }  
}

void chk_solid_train_cmds ( void ) {
  int rakeIDs[END_OF_SCs][TRAIN_INFO_ENTRIES_NUM + 1];
  int i;
  memset( rakeIDs, 0, sizeof(rakeIDs) );
  enum_alive_rakes( rakeIDs );
  for( i = 0; i < END_OF_SCs; i++ ) {
    int num_of_rakes = -1;
    int j;
    for( j = 0; j < TRAIN_INFO_ENTRIES_NUM; j++ )
      if( rakeIDs[i][j] < 0 ) {
	num_of_rakes = j;
	break;
      }
    assert( j < TRAIN_INFO_ENTRIES_NUM );
    assert( num_of_rakes > -1 );
    chk_massiv_train_cmds_array( i, rakeIDs[i], num_of_rakes );
  }
}

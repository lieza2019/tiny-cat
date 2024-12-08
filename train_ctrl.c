#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs.h"
#include "cbi.h"
#include "srv.h"

TINY_TRAIN_STATE trains_tracking[MAX_TRAIN_TRACKINGS];
static int frontier;

pthread_mutex_t cbtc_ctrl_cmds_mutex;
pthread_mutex_t cbtc_stat_infos_mutex;

uint8_t sp2_dst_platformID ( STOPPING_POINT_CODE dest_sp ) {
  return 0x01;
}
uint8_t journeyID2_serviceID ( JOURNEY_ID journey_ID ) {
  return (uint8_t)journey_ID;
}

static CBTC_BLOCK_PTR update_train_resblock ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  CBTC_BLOCK_PTR crnt_forward_blk = NULL;
  
  if( pT->pTI ) {
    assert( pT->rakeID == TRAIN_INFO_RAKEID(*pT->pTI) );
    const unsigned short blk_name_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pT->pTI );
    const unsigned short blk_name_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pT->pTI );
    CBTC_BLOCK_PTR pblk_forward = NULL;
    CBTC_BLOCK_PTR pblk_back = NULL;
    pblk_forward = lookup_cbtc_block_prof( blk_name_forward );
    if( pblk_forward ) {
      BOOL updated_pblk_back = FALSE;
      BOOL stil_there = FALSE;
      TINY_TRAIN_STATE_PTR p = NULL;
      p = read_residents_CBTC_BLOCK( pblk_forward );
      while( p ) {
	assert( pT );
	if( p == pT ) {
	  assert( pT->occupancy.pblk_forward == pblk_forward );
	  stil_there = TRUE;
	  crnt_forward_blk = pblk_forward;
	  break;
	}
	p = p->occupancy.pNext;
      }
      if( !stil_there ) {
	assert( pT );
	if( pT->occupancy.pblk_forward ) {	  
	  pblk_back = lookup_cbtc_block_prof( blk_name_back );
	  if( pblk_back ) {
	    if( pblk_back == pT->occupancy.pblk_forward ) {
	      TINY_TRAIN_STATE_PTR r = NULL;
	      pT->occupancy.pblk_back = pT->occupancy.pblk_forward;
	      updated_pblk_back = TRUE;
	      r = border_residents_CBTC_BLOCK(pblk_back, pT);
	      if( r != pT )
		goto no_elide;
	    }
	  } else {
	    errorF( "%d: unknown cbtc block detected as back, of train %3d.\n", blk_name_back, pT->rakeID );
	    pT->occupancy.pblk_back = NULL;
	    updated_pblk_back = TRUE;
	  }
	  {
	    BOOL elided = FALSE;
	    TINY_TRAIN_STATE_PTR *pp = NULL;
	    pp = addr_residents_CBTC_BLOCK( pT->occupancy.pblk_forward );
	    assert( pp );
	    while( *pp ) {
	      assert( *pp );
	      assert( pT );
	      if( *pp == pT ) {
		assert( !elided );
		*pp = pT->occupancy.pNext;
		pT->occupancy.pNext = NULL;
		elided = TRUE;
		continue;
	      }
	      pp = &(*pp)->occupancy.pNext;
	      assert( pp );
	    }
	  }
	no_elide:
	  pT->occupancy.pblk_forward = NULL;
	}
	assert( pblk_forward );
	assert( pT );
	pT->occupancy.pNext = read_residents_CBTC_BLOCK( pblk_forward );
	write_residents_CBTC_BLOCK( pblk_forward, pT );
	pT->occupancy.pblk_forward = pblk_forward;
	crnt_forward_blk = pblk_forward;
      }
      assert( pT );
      assert( pblk_forward );
      assert( pT->occupancy.pblk_forward == pblk_forward );

      if( !updated_pblk_back ) {
	if( blk_name_forward == blk_name_back )
	  pT->occupancy.pblk_back = NULL;
	else {
	  assert( blk_name_forward != blk_name_back );
	  pblk_back = lookup_cbtc_block_prof( blk_name_back );
	  if( pblk_back ) {
	    TINY_TRAIN_STATE_PTR r = NULL;
	    pT->occupancy.pblk_back = pblk_back;
	    r = border_residents_CBTC_BLOCK(pblk_back, pT);
	    if( r != pT )
	      ;
	  } else {
	    errorF( "%d: unknown cbtc block detected as back, of train %3d.\n", blk_name_back, pT->rakeID );
	    pT->occupancy.pblk_back = NULL;
	  }
	}
      }
    } else
      errorF( "%d: unknown cbtc block detected as forward, of train %3d.\n", blk_name_forward, pT->rakeID );
  }
  return crnt_forward_blk;
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
	trains_tracking[i].pTI = NULL;
      }
    assert( *pp == NULL );
  }
  return r;
}

static BOOL exam_consistency_with_train_info ( TINY_TRAIN_STATE_PTR pT, TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pT );
  assert( pI );
  return TRUE;
}

static void retrieve_from_train_info ( TINY_TRAIN_STATE_PTR pT, TRAIN_INFO_ENTRY_PTR pI ) {
  assert( pT );
  assert( pI );
  pT->rakeID = TRAIN_INFO_RAKEID( *pI );
  pT->skip_next_stop = TRAIN_INFO_SKIP_NEXT_STOP( *pI );
  pT->perf_regime = TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( *pI );
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
      assert( i == frontier );
      int r_mutex = -1;
      r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
      if( r_mutex ) {
	assert( FALSE );
      } else {
	assert( frontier < MAX_TRAIN_TRACKINGS );
	pE = &trains_tracking[frontier];
	retrieve_from_train_info( pE, pI );
	pE->pTI = NULL;
	frontier++;
	r_mutex = -1;
	r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
	assert( !r_mutex );
      }
    }
    assert( pE );
    if( pE->pTI )
      memcpy( &pE->TI_last, pE->pTI, (int)sizeof(TRAIN_INFO_ENTRY) );
    pE->pTI = pI;
    pE->updated = TRUE;
    pE->omit = FALSE;
  }
  assert( pE );
  assert( pE->pTI );
  return pE;
}

static void expire_all_train_cmds ( void ) {
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    int j;
    for( j = 0; j < TRAIN_COMMAND_ENTRIES_NUM; j++ )
      SC_ctrl_cmds[i].train_command.attribs.u.train_cmd.expired[j] = TRUE;
  }
}

static void expire_all_train_states ( void ) {
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    trains_tracking[i].updated = FALSE;
}

void reveal_train_tracking ( TINY_COMM_PROF_PTR pcomm_prof ) {
  assert( pcomm_prof );
  int r_mutex = -1;
  
  r_mutex = pthread_mutex_lock( &cbtc_stat_infos_mutex );
  if( r_mutex )
    assert( FALSE );
  else {
    int i;  
    expire_all_train_states();
    for( i = (int)SC801; i < END_OF_SCs; i++ ) {
      assert( (i >= SC801) && (i < END_OF_SCs) );
      SC_STAT_INFOSET_PTR pSC = NULL;
      pSC = snif_train_info( &pcomm_prof->cbtc.info.socks, (SC_ID)i );
      if( pSC ) {
	int j;
	for( j = 0; j < TRAIN_INFO_ENTRIES_NUM; j++ ) {
	  TINY_TRAIN_STATE_PTR pE = NULL;
	  unsigned short rakeID = TRAIN_INFO_RAKEID(pSC->train_information.recv.train_info.entries[j]);
	  if( rakeID != 0 ) {
	    //printf( "received rakeID in the %2d th Train info.: %3d.\n", (i + 1), rakeID );  // ***** for debugging.
	    pE = update_train_state( &pSC->train_information.recv.train_info.entries[j] );
	    assert( pE );
	    assert( pE->pTI );
	    update_train_resblock( pE );
#if 0
	    pSC->train_information.pTrain_stat[i] = pE; // ***** BUG? *****
#else
	    pSC->train_information.pTrain_stat[j] = pE;
#endif
	  }
	}
      }
    }
    {
      TINY_TRAIN_STATE_PTR pOph = NULL;
      pOph = enum_orphant_trains();
      while( pOph ) {
	errorF( "%-3d: observation lost on the train information.\n", pOph->rakeID );
	pOph = pOph->pNext;
      }
    }
    r_mutex = -1;
    r_mutex = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
    assert( !r_mutex );
  }
}

static SC_STAT_INFOSET_PTR willing2_recv_train_info ( TINY_SOCK_PTR pS, SC_ID sc_id ) {
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

static int establish_SC_comm_traininfo ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pdescs, const int ndescs ) {
  assert( pS );
  assert( pdescs );
  assert( ndescs > 0 );
  BOOL r = -1;
  
  int i = (int)SC801;
  while( i < (int)END_OF_SCs ) {
    assert( (i >= (int)SC801) && (i < (int)END_OF_SCs) );
    if( i < ndescs ) {
      SC_STAT_INFOSET_PTR p = NULL;
      expire_all_train_state();
      if( !(p = willing2_recv_train_info( pS, (SC_ID)i )) )
	goto exit;
      assert( p );
      assert( p->train_information.d_recv_train_info > -1 );
      assert( pdescs );
      pdescs[i] = p->train_information.d_recv_train_info;
      i++;
    } else
      goto exit;
  }
  assert( i == (int)END_OF_SCs );
  assert( i <= ndescs );
  r = i;
 exit:
  return r;
}

static SC_CTRL_CMDSET_PTR willing2_send_traincmd ( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  assert( (sc_id >= 0) && (sc_id < END_OF_SCs) );
  SC_CTRL_CMDSET_PTR r = NULL;
  
  SC_CTRL_CMDSET_PTR pSC = NULL;
  pSC = &SC_ctrl_cmds[(int)sc_id];
  assert( pSC );
  {
    assert( pSC->train_command.comm_prof.dest_sc_id == sc_id );
    IP_ADDR_DESC bcast_dst_ipaddr = SC_ctrl_cmds[(int)sc_id].sc_ipaddr;
    assert( bcast_dst_ipaddr.oct_1st != 0 );
    assert( bcast_dst_ipaddr.oct_2nd != 0 );
    assert( bcast_dst_ipaddr.oct_3rd != 0 );
    assert( bcast_dst_ipaddr.oct_4th != 0 );
    bcast_dst_ipaddr.oct_3rd = 255;
    bcast_dst_ipaddr.oct_4th = 255;
    pSC->train_command.comm_prof.dst_ipaddr = bcast_dst_ipaddr;
    
    pSC->train_command.comm_prof.d_send_train_cmd = -1;
    {
      TINY_SOCK_DESC d = -1;
      if( (d = creat_sock_sendnx( pS, pSC->train_command.comm_prof.dst_port, TRUE, &pSC->train_command.comm_prof.dst_ipaddr )) < 0 ) {
	errorF( "failed to create the socket to send Train command toward SC%d.\n", SC_ID_CONV_2_INT(sc_id) );
	goto exit;
      }
      pSC->train_command.comm_prof.d_send_train_cmd = d;
      sock_attach_send_buf( pS, pSC->train_command.comm_prof.d_send_train_cmd, (unsigned char *)&(pSC->train_command.comm_prof.send), (int)sizeof(pSC->train_command.comm_prof.send) );
    }
    r = pSC;
  }
  
 exit:
  return r;
}

static int establish_SC_comm_traincmd ( TINY_SOCK_PTR pS, SC_CTRLCMD_COMM_PROF_PTR *pprof_traincmds, const int ndsts ) {
  assert( pS );
  assert( pprof_traincmds );
  assert( ndsts > 0 );
  BOOL r = FALSE;
  
  int i = (int)SC801;
  while( i < (int)END_OF_SCs ) {
    assert( (i >= (int)SC801) && (i < (int)END_OF_SCs) );
    if( i < ndsts ) {
      SC_CTRL_CMDSET_PTR p = NULL;
      expire_all_train_cmds();
      if(! (p = willing2_send_traincmd( pS, (SC_ID)i )) )
	goto exit;
      assert( p );
      assert( p->train_command.comm_prof.d_send_train_cmd > -1 );
      assert( pprof_traincmds );
      pprof_traincmds[i] = &p->train_command.comm_prof;
      i++;
    } else
      goto exit;
  }
  assert( i == END_OF_SCs );
  assert( i <= ndsts );
  r = i;
 exit:
  return r;
}

int establish_SC_comm_infos ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pdescs[], const int ninfos, const int ndescs ) {
  assert( pS );
  assert( pdescs );
  assert( ninfos >= (int)END_OF_CBTC_CMDS_INFOS );
  assert( ndescs > 0 );
  int r = 1;
  
  assert( (int)CBTC_TRAIN_INFORMATION < ninfos );
  if( establish_SC_comm_traininfo( pS, pdescs[CBTC_TRAIN_INFORMATION], ndescs ) < 0 ) {
    r *= -1;
    goto exit;
  }
 exit:
  return r;
}

int establish_SC_comm_cmds ( TINY_SOCK_PTR pS, SC_CTRLCMD_COMM_PROF_PTR *pprofs[], const int nprofs, const int ndsts ) {
  assert( pS );
  assert( pprofs );
  assert( nprofs >= (int)END_OF_CBTC_CMDS_INFOS ); 
  assert( ndsts > 0 );
  int r = 1;
  
  assert( (int)CBTC_TRAIN_COMMAND < nprofs );
  if( establish_SC_comm_traincmd( pS, pprofs[CBTC_TRAIN_COMMAND], ndsts ) < 0 ) {
    r *= -1;
    goto exit;
  }
 exit:
  return r;
}

static void cons_train_cmd ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  int i;
  
  for( i = 0; i < 2; i++ ) {
    TRAIN_COMMAND_ENTRY_PTR pE = pT->pTC[i];
    if( pE ) {
      assert( ntohs(pE->rakeID) == pT->rakeID );
      pT->train_ID.coden = TRAIN_CMD_TRAINID( pE, &pT->train_ID );
#ifdef CHK_STRICT_CONSISTENCY
      {
	assert( pT );
	const uint16_t train_id = (uint16_t)((sp2_dst_platformID(pT->train_ID.dest) << 8) + journeyID2_serviceID(pT->train_ID.jid));
	assert( pT->train_ID.coden == (unsigned short)train_id );
      }
#endif // CHK_STRICT_CONSISTENCY
      TRAIN_CMD_DESTINATION_BLOCKID( *pE, pT->dest_blockID );
      TRAIN_CMD_CURRENT_BLOCKID( *pE, pT->crnt_blockID );
      TRAIN_CMD_KEEP_DOOR_CLOSED( *pE, pT->keep_door_closed );
      TRAIN_CMD_OUT_OF_SERVICE( *pE, pT->out_of_service );
      TRAIN_CMD_CURRENT_STATION_PLATFORM_CODE( *pE, pT->crnt_station_plcode );
      TRAIN_CMD_NEXT_STATION_PLATFORM_CODE( *pE, pT->next_station_plcode );
      TRAIN_CMD_DESTINATION_PLATFORM_CODE( *pE, pT->dst_station_plcode );
      TRAIN_CMD_DESTINATION_NUMBER( *pE, pT->destination_number );
      TRAIN_CMD_NEXT_STATION_NUMBER( *pE, pT->next_station_number );
      TRAIN_CMD_CURRENT_STATION_NUMBER( *pE, pT->crnt_station_number );
      TRAIN_CMD_ATO_DEPARTURE_COMMAND( *pE, pT->ATO_dept_cmd );
      TRAIN_CMD_DEPARTURE_CONDITIONS_RELEASE( *pE, pT->depcond_release );
      TRAIN_CMD_SKIP_NEXT_STOP( *pE, pT->skip_next_stop );
      TRAIN_CMD_ORIGIN_STATION( *pE, pT->origin_station );
      TRAIN_CMD_DIRECTION_OF_OPENING_DOOR_AT_THE_NEXT_STATION( *pE, pT->next_st_dooropen_side );
      TRAIN_CMD_INSTRUCTING_OPERATION_MODE( *pE, pT->operaton_mode );
      TRAIN_CMD_LEAVE_NOW( *pE, pT->leave_now );
      TRAIN_CMD_TRAIN_PERFORMANCE_REGIME_COMMAND( *pE, pT->perf_regime );
      TRAIN_CMD_COASTING_COMMAND( *pE, pT->coasting_cmd );
      TRAIN_CMD_TRAIN_HOLD_COMMAND( *pE, pT->TH_cmd );
      TRAIN_CMD_MAXIMUM_SPEED( *pE, pT->maximum_speed_cmd );
      TRAIN_CMD_TURNBACK_OR_SIDING( *pE, pT->turnback_siding );
      TRAIN_CMD_PASSENGER_ADDRESSING( *pE, pT->passenger_address );
      TRAIN_CMD_DEPARTURE_DIRECTION( *pE, pT->dep_dir );
      TRAIN_CMD_REGULATION_SPEED( *pE, pT->regulation_speed );
      TRAIN_CMD_DWELL_TIME( *pE, pT->dwell_time );
      TRAIN_CMD_ORDERING_WAKEUP( *pE, pT->ordering_wakeup );
      TRAIN_CMD_ORDERING_STANDBY( *pE, pT->ordering_standby );
      TRAIN_CMD_AUTOMATIC_TURNBACK_COMMAND( *pE, pT->ATB_cmd );
      TRAIN_CMD_ORDERING_EMERGENCY_STOP( *pE, pT->ordering_emergency_stop );
      TRAIN_CMD_ORDERING_RELEASE_FOR_EMERGENCY_STOP( *pE, pT->releasing_emergency_stop );
      TRAIN_CMD_TRAIN_REMOVE( *pE, pT->train_remove );
      TRAIN_CMD_SYSTEM_EXCHANGE_COMMAND( *pE, pT->system_switch_cmd );
      TRAIN_CMD_ORDERING_RESET_FOR_ONBOARD( *pE, pT->ordering_reset_onboard );
      TRAIN_CMD_ENERGY_SAVING_MODE( *pE, pT->energy_saving );
      TRAIN_CMD_OPENING_TRAIN_DOOR_REMOTELY( *pE, pT->remote_door_opening );
      TRAIN_CMD_CLOSING_TRAIN_DOOR_REMOTELY( *pE, pT->remote_door_closing );
      TRAIN_CMD_STATIC_TEST_COMMAND( *pE, pT->static_test_cmd );
      TRAIN_CMD_DYNAMIC_TEST_COMMAND( *pE, pT->dynamic_test_cmd );
      TRAIN_CMD_INCHING_COMMAND( *pE, pT->inching_cmd );
      TRAIN_CMD_BACK_INCHING_COMMAND( *pE, pT->back_inching_cmd );
      TRAIN_CMD_EMERGENCY_TRAIN_DOOR_RELEASING_COMMAND( *pE, pT->em_door_release );
      TRAIN_CMD_VRS_B_RESET( *pE, pT->back_vrs_reset );
      TRAIN_CMD_VRS_F_RESET( *pE, pT->forward_vrs_reset );
    }
  }
}

static void purge_train_cmds ( void ) {
  int i;
  for ( i = 0; i < END_OF_SCs; i++ ) {
    int j;
    for( j = 0; j < TRAIN_COMMAND_ENTRIES_NUM; j++ )
      if( SC_ctrl_cmds[i].train_command.attribs.u.train_cmd.expired[j] ) {
	TRAIN_CMD_RAKEID( SC_ctrl_cmds[i].train_command.comm_prof.send.u.train_cmd.spol.entries[j], 0 );
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
      if( pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[j].rakeID == 0 ) {
	assert( pSc->train_command.attribs.u.train_cmd.expired[j] );
	if( maiden < 0 )
	  maiden = j;
	j++;
      } else if( maiden >= 0 ) {
	pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[maiden] = pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[j];
	{
	  TINY_TRAIN_STATE_PTR pTs = pSc->train_command.attribs.u.train_cmd.pTrain_stat[j];
	  assert( pTs );
	  if( pTs->pTC[0] == &pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[j] )
	    pTs->pTC[0] = &pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[maiden];
	  else if( pTs->pTC[1] == &pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[j] )
	    pTs->pTC[1] = &pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[maiden];
	  else
	    assert( FALSE );
	  pSc->train_command.attribs.u.train_cmd.pTrain_stat[maiden] = pTs;
	}
	pSc->train_command.attribs.u.train_cmd.expired[maiden] = FALSE;
	
	TRAIN_CMD_RAKEID( pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[j], 0 );
	pSc->train_command.attribs.u.train_cmd.expired[j] = TRUE;
	j = maiden + 1;
	maiden = -1;
      } else
	j++;
      assert( maiden < TRAIN_COMMAND_ENTRIES_NUM );
    }
    pSc->train_command.attribs.u.train_cmd.frontier = TRAIN_COMMAND_ENTRIES_NUM;
    if( maiden >= 0 ) {
      int n = TRAIN_COMMAND_ENTRIES_NUM - maiden;
      memset( &pSc->train_command.comm_prof.send.u.train_cmd.spol.entries[maiden], 0, ((int)sizeof(TRAIN_COMMAND_ENTRY) * n) );
      pSc->train_command.attribs.u.train_cmd.frontier = maiden;
    }
    assert( pSc->train_command.attribs.u.train_cmd.frontier <= TRAIN_COMMAND_ENTRIES_NUM );
  }
}

int load_train_command ( void ) {
  int cnt = 0;
  
  int r_mutex_ctrl = -1;
  r_mutex_ctrl = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
  if( r_mutex_ctrl ) {
    assert( FALSE );
  } else {
    int r_mutex_stat = -1;
    r_mutex_stat = pthread_mutex_lock( &cbtc_stat_infos_mutex );
    if( r_mutex_stat ) {
      assert( FALSE );
    } else {
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
	    int n = -1;
	    n = alloc_train_cmd_entries( es, pstat, pstat->rakeID, front_blk, back_blk );
	    if( n > 0 ) {
	      assert( n <= 2 );
	      int j;
	      for( j = 0; j < n; j++ )
		pstat->pTC[j] = es[j];
	      assert( (j > 0) && (j <= 2) );
	      if( j < 2 ) {
		assert( j == 1 );
		pstat->pTC[j] = NULL;
	      }
	      cons_train_cmd( pstat );
	      cnt++;
	    }
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
	int n = -1;
	n = standup_train_cmd_entries( es, pstat, pstat->rakeID, front_blk, back_blk );
	if( n > 0 ) {
	  assert( n <= 2 );
	  int k;
	  for( k = 0; k < n; k++ )
	    pstat->pTC[k] = es[k];
	  assert( (k > 0) && (k <= 2) );
	  if( k < 2 ) {
	    assert( k == 1 );
	    pstat->pTC[k] = NULL;
	  }
	  cnt++;
	  assert( standby_train_cmds.phd->pNext ? TRUE : standby_train_cmds.pptl == &(standby_train_cmds.phd->pNext) );
	  standby_train_cmds.phd = standby_train_cmds.phd->pNext;
	}
      }
      fine_train_cmds();
    
      r_mutex_stat = -1;
      r_mutex_stat = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
      assert( !r_mutex_stat );
    }
    r_mutex_ctrl = -1;
    r_mutex_ctrl = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
    assert( !r_mutex_ctrl );
  }
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
  const TRAIN_COMMAND_ENTRY_PTR plim = &SC_ctrl_cmds[sc_id].train_command.comm_prof.send.u.train_cmd.spol.entries[TRAIN_COMMAND_ENTRIES_NUM];
  TRAIN_COMMAND_ENTRY_PTR pE = &SC_ctrl_cmds[sc_id].train_command.comm_prof.send.u.train_cmd.spol.entries[0];
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
	assert( rID > 0 );
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
      if( found ) {
	assert( pE == ++pEp );
      }
      else {
	//printf( "(sc_id, rakeID) = (%d, %d)\n", SC_ID_CONV_2_INT(sc_id), rID ); // ***** for debugging.
	assert( FALSE );
      }
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
  
  int r_mutex_ctrl = -1;
  r_mutex_ctrl = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
  if( r_mutex_ctrl ) {
    assert( FALSE );
  } else {
    int r_mutex_stat = -1;
    r_mutex_stat = pthread_mutex_lock( &cbtc_stat_infos_mutex );
    if( r_mutex_stat ) {
      assert( FALSE );
    } else {
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
      r_mutex_stat = -1;
      r_mutex_stat = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
      assert( !r_mutex_stat );
    }
    r_mutex_ctrl = -1;
    r_mutex_ctrl = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
    assert( !r_mutex_ctrl );
  }
}

void *conslt_cbtc_state ( TINY_TRAIN_STATE_PTR pT, const CBTC_CMDS_INFOS kind, void *pstat_prev, void *pstate, const int size ) {
  assert( pT );
  assert( pstate );
  assert( size > 0 );
  void *r = NULL;
  
  switch( kind ) {
  case CBTC_TRAIN_INFORMATION:
    {
      assert( size >= sizeof( TRAIN_INFO_ENTRY ) );
      void *pdst = NULL;
      TRAIN_INFO_ENTRY_PTR pI = (TRAIN_INFO_ENTRY_PTR)pstate;
      assert( pI );      
      int r_mutex = -1;
      r_mutex = pthread_mutex_trylock( &cbtc_stat_infos_mutex );
      if( !r_mutex ) {
	void *psrc = NULL;
	if( pT->pTI )
	  psrc = pT->pTI;
	else
	  psrc = &pT->TI_last;
	pdst = memcpy( pstate, psrc, sizeof(TRAIN_INFO_ENTRY) );
	assert( pdst == pstate );
	if( pstat_prev ) {
	  pdst = memcpy( pstat_prev, &pT->TI_last, sizeof(TRAIN_INFO_ENTRY) );
	  assert( pdst == pstat_prev );
	}
	r = pstate;
	r_mutex = -1;
	r_mutex = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
	assert( !r_mutex );
      } else {
	if( pstat_prev ) {
	  pdst = memcpy( pstat_prev, &pT->TI_last, sizeof(TRAIN_INFO_ENTRY) );
	  assert( pdst == pstat_prev );
	}
      }
    }
    break;
  case CBTC_TRACK_DATA:
    /* fall thru. */
  case CBTC_CBI_INFORMATION:
    /* fall thru. */
  case CBTC_TSR_STATUS:
    /* fall thru. */
  case CBTC_NOENTRY_PRESET_STATUS:
    /* fall thru. */
  case CBTC_STATUS_INFORMATION:
    /* fall thru. */
  case CBTC_STATION_STATUS:
    /* fall thru. */
  case CBTC_UNLOCATED_TRAIN_INFORMATION:
    assert( FALSE );
  default:
    assert( FALSE );
  }
  return r;
}

void *pth_emit_cbtc_ctrl_cmds ( void *arg ) {  
  assert( arg );
  const useconds_t interval = 1000 * 1000 * 0.1;
  TINY_COMM_PROF_PTR pcomm_threads_prof = (TINY_COMM_PROF_PTR)arg;
  assert( pcomm_threads_prof );
  TINY_SOCK_PTR psocks_cbtc_cmds = &pcomm_threads_prof->cbtc.cmd.socks;
  assert( psocks_cbtc_cmds );
  
  while( TRUE ) {
    assert( pcomm_threads_prof );
    assert( psocks_cbtc_cmds );
    int r_mutex = -1;
    r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
    if( r_mutex ) {
      assert( FALSE );
    } else {
      int i = (int)SC801;
      while( i < END_OF_SCs ) {
	// for Train commands.
	SC_CTRLCMD_COMM_PROF_PTR pprof_traincmd = pcomm_threads_prof->cbtc.cmd.train_cmd.pprofs[i];
	if( pprof_traincmd ) {
	  assert( pcomm_threads_prof->cbtc.cmd.train_cmd.pprofs[i] );
	  const TINY_SOCK_DESC sd_train_info = pprof_traincmd->d_send_train_cmd;
	  if( sd_train_info > -1 ) {
	    const int sendsiz_traincmd = sizeof( pprof_traincmd->send );
#ifdef CHK_STRICT_CONSISTENCY
	    {
	      unsigned char *pmsg_buf = NULL;
	      int siz = -1;
	      pmsg_buf = sock_send_buf_attached( psocks_cbtc_cmds, sd_train_info, &siz );
	      assert( pmsg_buf == (unsigned char *)&pprof_traincmd->send );
	      assert( siz >= sendsiz_traincmd );
	    }
#endif // CHK_STRICT_CONSISTENCY
	    {
	      const uint8_t dst_sc_id = 101 + i;
	      NXNS_HEADER_PTR phdr = NULL;
	      assert( (OC_ID)((int)dst_sc_id - 101) == (OC_ID)i );
	      if( !pprof_traincmd->nx.emission_start )
		pprof_traincmd->nx.emission_start = time( NULL );
	      phdr = &pprof_traincmd->send.header;
	      assert( phdr );
	      {
		const uint8_t msgType = 43; // #43: Train command (1-20)
		uint32_t seq = pprof_traincmd->nx.seq;
		seq++;
		seq = (seq %= NX_SEQNUM_MAX_PLUS1) ? seq : 1;
		mk_nxns_header( phdr, pprof_traincmd->nx.emission_start, msgType, dst_sc_id, seq );
		pprof_traincmd->nx.seq = seq;
	      }
	      {
		int n = -1;
		n = sock_send_ready( psocks_cbtc_cmds, sd_train_info, sendsiz_traincmd );
		assert( n == sendsiz_traincmd );
	      }
	    }
	  }
	}
	// end of Train commands.
	i++;
      }
      if( sock_send( psocks_cbtc_cmds ) < 0 ) {
	errorF( "%s", "failed to send cbtc control commands toward the SCs.\n" );
      }
      r_mutex = -1;
      r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
      assert( !r_mutex );
    }
    {
      int r = -1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

void *pth_reveal_cbtc_status ( void *arg ) {
  assert( arg );
  const useconds_t interval = 1000 * 1000 * 0.1;
  TINY_COMM_PROF_PTR pcomm_prof = (TINY_COMM_PROF_PTR)arg;
  assert( pcomm_prof );
  
  while( TRUE ) {
    assert( pcomm_prof );
    int r_mutex = -1;
    r_mutex = pthread_mutex_lock( &cbtc_stat_infos_mutex );
    if( r_mutex ) {
      assert( FALSE );
    } else {
      int nrecv = -1;
      if( (nrecv = sock_recv( &pcomm_prof->cbtc.info.socks )) < 0 ) {
	errorF( "%s", "error on receiving CBTC status information from SC.\n" );
      }
      r_mutex = -1;
      r_mutex = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
      assert( !r_mutex );
    }
    {
      int r = -1;
      r = usleep( interval );
      assert( !r );
    }
  }
  return NULL;
}

#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

SC_CTRL_CMDSET SC_ctrl_cmds[END_OF_SCs] = {
  {"SC801", {172, 21,  1, 1}, {{SC801, UDP_BCAST_RECV_PORT_SC801_Train_command, -1}}},
  {"SC802", {172, 21,  2, 1}, {{SC802, UDP_BCAST_RECV_PORT_SC802_Train_command, -1}}},
  {"SC803", {172, 21,  3, 1}, {{SC803, UDP_BCAST_RECV_PORT_SC803_Train_command, -1}}},
  {"SC804", {172, 21,  4, 1}, {{SC804, UDP_BCAST_RECV_PORT_SC804_Train_command, -1}}},
  {"SC805", {172, 21,  5, 1}, {{SC805, UDP_BCAST_RECV_PORT_SC805_Train_command, -1}}},
  {"SC806", {172, 21,  6, 1}, {{SC806, UDP_BCAST_RECV_PORT_SC806_Train_command, -1}}},
  {"SC807", {172, 21,  7, 1}, {{SC807, UDP_BCAST_RECV_PORT_SC807_Train_command, -1}}},
  {"SC808", {172, 21,  8, 1}, {{SC808, UDP_BCAST_RECV_PORT_SC808_Train_command, -1}}},
  {"SC809", {172, 21,  9, 1}, {{SC809, UDP_BCAST_RECV_PORT_SC809_Train_command, -1}}},
  {"SC810", {172, 21, 10, 1}, {{SC810, UDP_BCAST_RECV_PORT_SC810_Train_command, -1}}},
  {"SC811", {172, 21, 11, 1}, {{SC811, UDP_BCAST_RECV_PORT_SC811_Train_command, -1}}},
  {"SC812", {172, 21, 12, 1}, {{SC812, UDP_BCAST_RECV_PORT_SC812_Train_command, -1}}},
  {"SC813", {172, 21, 13, 1}, {{SC813, UDP_BCAST_RECV_PORT_SC813_Train_command, -1}}},
  {"SC814", {172, 21, 14, 1}, {{SC814, UDP_BCAST_RECV_PORT_SC814_Train_command, -1}}},
  {"SC815", {172, 21, 15, 1}, {{SC815, UDP_BCAST_RECV_PORT_SC815_Train_command, -1}}},
  {"SC816", {172, 21, 16, 1}, {{SC816, UDP_BCAST_RECV_PORT_SC816_Train_command, -1}}},
  {"SC817", {172, 21, 17, 1}, {{SC817, UDP_BCAST_RECV_PORT_SC817_Train_command, -1}}},
  {"SC818", {172, 21, 18, 1}, {{SC818, UDP_BCAST_RECV_PORT_SC818_Train_command, -1}}},
  {"SC819", {172, 21, 19, 1}, {{SC819, UDP_BCAST_RECV_PORT_SC819_Train_command, -1}}},
  {"SC820", {172, 21, 20, 1}, {{SC820, UDP_BCAST_RECV_PORT_SC820_Train_command, -1}}},
  {"SC821", {172, 21, 21, 1}, {{SC821, UDP_BCAST_RECV_PORT_SC821_Train_command, -1}}}
};
SC_STAT_INFOSET SC_stat_infos[END_OF_SCs] = {
  {"SC801", {172, 21,  1, 1}, {UDP_BCAST_RECV_PORT_SC801_Train_information, -1}},
  {"SC802", {172, 21,  2, 1}, {UDP_BCAST_RECV_PORT_SC802_Train_information, -1}},
  {"SC803", {172, 21,  3, 1}, {UDP_BCAST_RECV_PORT_SC803_Train_information, -1}},
  {"SC804", {172, 21,  4, 1}, {UDP_BCAST_RECV_PORT_SC804_Train_information, -1}},
  {"SC805", {172, 21,  5, 1}, {UDP_BCAST_RECV_PORT_SC805_Train_information, -1}},
  {"SC806", {172, 21,  6, 1}, {UDP_BCAST_RECV_PORT_SC806_Train_information, -1}},
  {"SC807", {172, 21,  7, 1}, {UDP_BCAST_RECV_PORT_SC807_Train_information, -1}},
  {"SC808", {172, 21,  8, 1}, {UDP_BCAST_RECV_PORT_SC808_Train_information, -1}},
  {"SC809", {172, 21,  9, 1}, {UDP_BCAST_RECV_PORT_SC809_Train_information, -1}},
  {"SC810", {172, 21, 10, 1}, {UDP_BCAST_RECV_PORT_SC810_Train_information, -1}},
  {"SC811", {172, 21, 11, 1}, {UDP_BCAST_RECV_PORT_SC811_Train_information, -1}},
  {"SC812", {172, 21, 12, 1}, {UDP_BCAST_RECV_PORT_SC812_Train_information, -1}},
  {"SC813", {172, 21, 13, 1}, {UDP_BCAST_RECV_PORT_SC813_Train_information, -1}},
  {"SC814", {172, 21, 14, 1}, {UDP_BCAST_RECV_PORT_SC814_Train_information, -1}},
  {"SC815", {172, 21, 15, 1}, {UDP_BCAST_RECV_PORT_SC815_Train_information, -1}},
  {"SC816", {172, 21, 16, 1}, {UDP_BCAST_RECV_PORT_SC816_Train_information, -1}},
  {"SC817", {172, 21, 17, 1}, {UDP_BCAST_RECV_PORT_SC817_Train_information, -1}},
  {"SC818", {172, 21, 18, 1}, {UDP_BCAST_RECV_PORT_SC818_Train_information, -1}},
  {"SC819", {172, 21, 19, 1}, {UDP_BCAST_RECV_PORT_SC819_Train_information, -1}},
  {"SC820", {172, 21, 20, 1}, {UDP_BCAST_RECV_PORT_SC820_Train_information, -1}},
  {"SC821", {172, 21, 21, 1}, {UDP_BCAST_RECV_PORT_SC821_Train_information, -1}}
};

STANDBY_TRAIN_CMDS standby_train_cmds;

SC_CTRL_CMDSET_PTR which_SC_from_train_cmd ( TRAIN_COMMAND_ENTRY_PTR pTc ) {
  assert( pTc );
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    void *lim_inf = (void *)&SC_ctrl_cmds[i];
    void *lim_sup = lim_inf + sizeof(SC_CTRL_CMDSET);
    if( ((void *)pTc > lim_inf) && ((void *)pTc < lim_sup) )
      break;
  }
  assert( i < END_OF_SCs );
  return &SC_ctrl_cmds[i];
}

#if 1 // *****
SC_STAT_INFOSET_PTR which_SC_from_train_info ( TRAIN_INFO_ENTRY_PTR pTi ) {
  assert( pTi );
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    void *lim_inf = (void *)&SC_stat_infos[i];
    void *lim_sup = lim_inf + sizeof(SC_STAT_INFOSET);
    if( ((void *)pTi > lim_inf) && ((void *)pTi < lim_sup) )
      break;
  }
  assert( i < END_OF_SCs );
  return &SC_stat_infos[i];
}
#else
int which_SC_from_train_info ( SC_STAT_INFOSET_PTR pstats[], TRAIN_INFO_ENTRY_PTR pTi ) {
  assert( pstats );
  assert( pTi );
  int r = 0;
  
  int i;
  for( i = 0; i < END_OF_SCs; i++ ) {
    const void *lim_inf = (void *)&SC_stat_infos[i];
    const void *lim_sup = lim_inf + sizeof(SC_STAT_INFOSET);
    if( ((void *)pTi > lim_inf) && ((void *)pTi < lim_sup) ) {
      pstats[i] = &SC_stat_infos[i];
      r++;
    }
  }
  return r;
}
#endif

static int which_SC_zones ( SC_ID zones[], int front_blk, int back_blk ) {  
  assert( zones );
  int r = -1;
  
  CBTC_BLOCK_PTR pBf = lookup_cbtc_block_prof( front_blk );
  CBTC_BLOCK_PTR pBb = lookup_cbtc_block_prof( back_blk );
  zones[0] = END_OF_SCs;
  zones[1] = END_OF_SCs;
  if( pBf ) {
    if( pBb ) {
      zones[0] = pBf->zone;
      zones[1] = pBb->zone;
      r = (zones[0] != zones[1]) ? 2 : 1;
    } else {
      zones[0] = pBf->zone;
      r = 1;
    }
  } else if( pBb ) {
    zones[0] = pBb->zone;
    r = 1;
  } else {
    r = 0;
  }
  printf( "(zone_front, zone_rear):(%d, %d)\n", SCID_CNV2_INT(zones[0]), SCID_CNV2_INT(zones[1]) );
  return r;
}

static TRAIN_COMMAND_ENTRY_PTR lkup_train_cmd ( TINY_TRAIN_STATE_PTR pTs, SC_CTRL_CMDSET_PTR pCs, int rakeID ) {
  assert( pTs );
  assert( pCs );
  assert( rakeID > 0 );
  TRAIN_COMMAND_ENTRY_PTR pE = NULL;
  int i;
  
  for( i = 0; i < TRAIN_COMMAND_ENTRIES_NUM; i++ )
    if( (int)ntohs(pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[i].rakeID) == rakeID ) {
      pE = &pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[i];
      pCs->train_command.attribs.u.train_cmd.pTrain_stat[i] = pTs;
      pCs->train_command.attribs.u.train_cmd.expired[i] = FALSE;
      break;
    }
  if( pE ) {
    assert( i < TRAIN_COMMAND_ENTRIES_NUM );
    assert( (int)ntohs(pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[i].rakeID) == rakeID );
    assert( pCs->train_command.attribs.u.train_cmd.pTrain_stat[i] );
    assert( pCs->train_command.attribs.u.train_cmd.pTrain_stat[i] == pTs );
    pCs->train_command.attribs.u.train_cmd.expired[i] = FALSE;   
  } else {
    int j;
    for( j = 0; j < pCs->train_command.attribs.u.train_cmd.frontier; j++ )
      if( pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[j].rakeID == 0 ) {
	assert( pCs->train_command.attribs.u.train_cmd.expired[j] );
	pE = &pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[j];
	pCs->train_command.attribs.u.train_cmd.pTrain_stat[j] = pTs;
	pCs->train_command.attribs.u.train_cmd.expired[j] = FALSE;
	break;
      }
    if( !pE ) {
      assert( j == pCs->train_command.attribs.u.train_cmd.frontier );
      if( pCs->train_command.attribs.u.train_cmd.frontier < TRAIN_COMMAND_ENTRIES_NUM ) {
	int f = pCs->train_command.attribs.u.train_cmd.frontier;
	assert( pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[f].rakeID == 0 );
	assert( pCs->train_command.attribs.u.train_cmd.expired[f] );
	pE = &pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[f];
	pCs->train_command.attribs.u.train_cmd.pTrain_stat[f] = pTs;
	pCs->train_command.attribs.u.train_cmd.expired[f] = FALSE;
	pCs->train_command.attribs.u.train_cmd.frontier++;
      } else {
	if( standby_train_cmds.pptl )
	  *standby_train_cmds.pptl = pTs;
	standby_train_cmds.pptl = &(pTs->pNext);
	*standby_train_cmds.pptl = NULL;
      }
    }
  }
  if( pE )
    TRAIN_CMD_RAKEID( *pE, rakeID ); 
  return pE;
}

int alloc_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk ) {
  assert( es );
  assert( pTs );
  assert( rakeID > 0 );
  SC_ID zones[2] = { END_OF_SCs, END_OF_SCs };
  int r = 0;
  
  int n = -1;
  n = which_SC_zones( zones, front_blk, back_blk );
  if( n > 0 ) {
    assert( n <= 2 );
    int i;
    for( i = 0; i < n; i++ ) {
      assert( zones[i] != END_OF_SCs );
      es[i] = lkup_train_cmd( pTs, &SC_ctrl_cmds[zones[i]], rakeID );
      if( !es[i] ) {
	assert( standby_train_cmds.pptl );
	if( !standby_train_cmds.phd ) {
	  standby_train_cmds.phd = pTs;
	  assert( standby_train_cmds.pptl == &(standby_train_cmds.phd->pNext) );
	}
	assert( !(*standby_train_cmds.pptl) );
      } else
	r++;
    }
  } else {
    errorF( "%s", "failed to allocate train command entry to the train:%03d, for invalid Occupied Block(forward/back).\n" );
    r = -1;
  }
  return r;
}

int standup_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk ) {
  assert( es );
  assert( pTs );
  assert( rakeID > 0 );
  SC_ID zones[2] = { END_OF_SCs, END_OF_SCs };
  int r = 0;

  int n = -1;
  n = which_SC_zones( zones, front_blk, back_blk );
  if( n > 0 ) {
    assert(n <= 2);
    int i;
    for( i = 0; i < n; i++ ) {
      assert( zones[i] != END_OF_SCs );
      SC_CTRL_CMDSET_PTR pCs = &SC_ctrl_cmds[zones[i]];
      assert( pCs );
      int j;
      for( j = 0; j < TRAIN_COMMAND_ENTRIES_NUM; j++ ) {
	if( pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[j].rakeID == 0 ) {
	  assert( pCs->train_command.attribs.u.train_cmd.expired[j] );
	  TRAIN_CMD_RAKEID( pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[j], rakeID );
	  pCs->train_command.attribs.u.train_cmd.pTrain_stat[j] = pTs;
	  pCs->train_command.attribs.u.train_cmd.expired[j] = FALSE;
	  es[i] = &pCs->train_command.comm_prof.send.u.train_cmd.spol.entries[j];
	  if( j > pCs->train_command.attribs.u.train_cmd.frontier )
	    pCs->train_command.attribs.u.train_cmd.frontier = j;
	  assert( pCs->train_command.attribs.u.train_cmd.frontier < TRAIN_COMMAND_ENTRIES_NUM );
	  r++;
	}
      }
    }
    assert( r == n );
  } else {
    errorF( "%s", "failed to allocate train command entry to the standing-by train:%03d, for invalid Occupied Block(forward/back).\n" );
    r = -1;
  }
  return r;
}

SC_CTRL_CMDSET_PTR emit_train_cmd( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  return NULL;
}

SC_STAT_INFOSET_PTR snif_train_info( TINY_SOCK_PTR pS, SC_ID sc_id ) {
  assert( pS );
  assert( (sc_id >= SC801) && (sc_id < END_OF_SCs) );
  SC_STAT_INFOSET_PTR pSC = NULL;

  pSC = &SC_stat_infos[sc_id];
  assert( pSC );
  {
    int i;
    for( i = 0; i < TRAIN_INFO_ENTRIES_NUM; i++ )
      pSC->train_information.pTrain_stat[i] = NULL;
  }
  
  {
    TINY_SOCK_DESC d = -1;
    int len = -1;
    d = pSC->train_information.d_recv_train_info;
    assert( d > -1 );
    assert( sock_recv_socket_attached( pS, d ) > 0 );
    {
      NXNS_HEADER_PTR p = NULL;
      p = (NXNS_HEADER_PTR)sock_recv_buf_attached( pS, d, &len );
      //printf( "len is %d.\n", len );  // ***** for debugging.
      assert( p );
      assert( p == (NXNS_HEADER_PTR)&(pSC->train_information.recv) );
    }
    assert( len > 0 ? len >= sizeof(NXNS_HEADER) : TRUE );
  }
  return pSC;
}

void phony_raw_recvbuf_traininfo( void *pbuf ) {  // ***** for debugging.
  int i;
  for( i = 0; i < TRAIN_INFO_ENTRIES_NUM; i++ )
    ((struct recv_buf_traininfo *)pbuf)->train_info.entries[i].rakeID = (uint8_t)(i + 20);
}  
void dump_raw_recvbuf_traininfo( void *pbuf ) {  // ***** for debugging.
  assert( pbuf );
  int i;
  for( i = 0; i < TRAIN_INFO_ENTRIES_NUM; i++ ) {
    unsigned short rakeID =  TRAIN_INFO_RAKEID( ((struct recv_buf_traininfo *)pbuf)->train_info.entries[i] );
    if( rakeID > 0 )
      printf( "received rakeID: %03d.\n", rakeID );
  }
}

static void print_TRAIN_OPERATION_MODE ( FILE *fp_out, const TRAIN_OPERATION_MODE mode ) {
  assert( fp_out );
  switch( mode ) {
  case OM_UNKNOWN_NO_DIRECTIVE:
    fprintf( fp_out, "OM_UNKNOWN_NO_DIRECTIVE" );
    break;
  case OM_UTO:
    fprintf( fp_out, "OM_UTO" );
    break;
  case OM_ATO:
    fprintf( fp_out, "OM_ATO" );
    break;
  case OM_ATP:
    fprintf( fp_out, "OM_ATP" );
    break;
  case OM_RM:
    fprintf( fp_out, "OM_RM" );
    break;
  case OM_SPARE_5:
    fprintf( fp_out, "OM_INVALID_SPARE_5" );
    break;
  case OM_SPARE_6:
    fprintf( fp_out, "OM_INVALID_SPARE_6" );
    break;
  case OM_SPARE_7:
    fprintf( fp_out, "OM_INVALID_SPARE_7" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_TRAIN_DETECTION_MODE ( FILE *fp_out, const TRAIN_DETECTION_MODE mode ) {
  assert( fp_out );
  switch( mode ) {
  case TD_UNKNOWN:
    fprintf( fp_out, "TD_UNKNOWN" );
    break;
  case TD_TRAIN_DETECTION:
    fprintf( fp_out, "TD_TRAIN_DETECTION" );
    break;
  case TD_SECTION_PROTECTION_ONLY:
    fprintf( fp_out, "TD_SECTION_PROTECTION_ONLY" );
    break;
  case TD_COMM_LOST:
    fprintf( fp_out, "TD_COMM_LOST" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_TYPE_OF_VEHICLE ( FILE *fp_out, const VEHICLE_TYPE mode ) {
  assert( fp_out );
  switch( mode ) {
  case VT_RESCUE:
    fprintf( fp_out, "VT_RESCUE" );
    break;
  case VT_6CARS:
    fprintf( fp_out, "VT_6CARS" );
    break;
  case VT_UNKNOWN:
    /* fall thru. */
  default:
    fprintf( fp_out, "VT_UNKNOWN" );
  }
}

static void print_INITIALIZATION_STATUS ( FILE *fp_out, const INITIALIZATION_STATUS stat ) {
  assert( fp_out );
  switch( stat ) {
  case INI_NOT_INITIALIZING:
    fprintf( fp_out, "INI_NOT_INITIALIZING" );
    break;
  case INI_IN_INITIALIZING:
    fprintf( fp_out, "INI_IN_INITIALIZING" );
    break;
  case INI_INITIALIZING_COMPLETED:
    fprintf( fp_out, "INI_INITIALIZING_COMPLETED" );
    break;
  case INI_INITIALIZING_TIMEOUT:
    fprintf( fp_out, "INI_INITIALIZING_TIMEOUT" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_MASTER_STANDBY ( FILE *fp_out, const ONBOARD_ATC_MASTER_STANDBY mode ) {
  assert( fp_out );
  switch( mode ) {
  case MS_BOTH_STANDBY:
    fprintf( fp_out, "MS_BOTH_STANDBY" );
    break;
  case MS_SYS1_MASTER:
    fprintf( fp_out, "MS_SYS1_MASTER" );
    break;
  case MS_SYS2_MASTER:
    fprintf( fp_out, "MS_SYS2_MASTER" );
    break;
  case MS_BOTH_MASTER:
    fprintf( fp_out, "MS_BOTH_MASTER" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_RUNNING_DIRECTION ( FILE *fp_out, const TRAIN_MOVE_DIR dir ) {
  assert( fp_out );
  switch( dir ) {
  case MD_UNKNOWN:
    fprintf( fp_out, "MD_UNKNOWN" );
    break;
  case MD_DOWN_DIR:
    fprintf( fp_out, "MD_DOWN_DIR" );
    break;
  case MD_UP_DIR:
    fprintf( fp_out, "MD_UP_DIR" );
    break;
  case MD_INVALID:
    fprintf( fp_out, "MD_INVALID" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_SLEEP_MODE ( FILE *fp_out, const TRAIN_SLEEP_MODE mode ) {
  assert( fp_out );
  switch( mode ) {
  case SM_NOT_IN_SLEEP:
    fprintf( fp_out, "SM_NOT_IN_SLEEP" );
    break;
  case SM_NOW_IN_TRANSITION:
    fprintf( fp_out, "SM_NOW_IN_TRANSITION" );
    break;
  case SM_IN_SLEEP:
    fprintf( fp_out, "SM_IN_SLEEP" );
    break;
  case SM_UNKNOWN:
    fprintf( fp_out, "SM_UNKNOWN" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_TRAIN_PERFORMANCE_REGIME ( FILE *fp_out, const TRAIN_PERF_REGIME perf ) {
  assert( fp_out );
  switch( perf ) {
  case PR_NO_COMMAND:
    fprintf( fp_out, "PR_NO_COMMAND" );
    break;
  case PR_S_MODE:
    fprintf( fp_out, "PR_S_MODE" );
    break;
  case PR_N_MODE:
    fprintf( fp_out, "PR_N_MODE" );
    break;
  case PR_F_MODE:
    fprintf( fp_out, "PR_F_MODE" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_RESULT_OF_RESET_FOR_ONBOARD ( FILE *fp_out, const ONBOARD_RESET_RESULT res ) {
  assert( fp_out );
  switch( res ) {
  case ORR_NOT_RESET:
    fprintf( fp_out, "ORR_NOT_RESET" );
    break;
  case ORR_RESET_ONBOARD_OF_SYS1:
    fprintf( fp_out, "ORR_RESET_ONBOARD_OF_SYS1" );
    break;
  case ORR_RESET_ONBOARD_OF_SYS2:
    fprintf( fp_out, "ORR_RESET_ONBOARD_OF_SYS2" );
    break;
  case ORR_SINGLE_SYSTEM_OPERATION:
    fprintf( fp_out, "ORR_SINGLE_SYSTEM_OPERATION" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_STATE_OF_TRAIN_DOOR ( FILE *fp_out, const TRAIN_DOORS_STATE stat ) {
  assert( fp_out );
  switch( stat ) {
  case DR_BOTH_SIDE_NOT_ALLCLOSED:
    fprintf( fp_out, "DR_BOTH_SIDE_NOT_ALLCLOSED" );
    break;
  case DR_ONLY_R_SIDE_ALL_CLOSED:
    fprintf( fp_out, "DR_ONLY_R_SIDE_ALL_CLOSED" );
    break;
  case DR_ONLY_L_SIDE_ALL_CLOSED:
    fprintf( fp_out, "DR_ONLY_L_SIDE_ALL_CLOSED" );
    break;
  case DR_BOTH_SIDE_ALL_CLOSED:
    fprintf( fp_out, "DR_BOTH_SIDE_ALL_CLOSED" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_ATO_DRIVING_STATUS ( FILE *fp_out, const ATO_DRIVING_STATUS stat ) {
  assert( fp_out );
  switch( stat ) {
  case DS_RUNNING:
    fprintf( fp_out, "DS_RUNNING" );
    break;
  case DS_SHORTSTOP:
    fprintf( fp_out, "DS_SHORTSTOP" );
    break;
  case DS_OVERRUN:
    fprintf( fp_out, "DS_OVERRUN" );
    break;
  case DS_TASC_PATTERN_RUNNING:
    fprintf( fp_out, "DS_TASC_PATTERN_RUNNING" );
    break;
  case DS_PO_STOPPING:
    fprintf( fp_out, "DS_PO_STOPPING" );
    break;
  case DS_P0_OTHER_STOPPING:
    fprintf( fp_out, "DS_P0_OTHER_STOPPING" );
    break;
  case DS_INCHING:
    fprintf( fp_out, "DS_INCHING" );
    break;
  case DS_INVALID:
    fprintf( fp_out, "DS_INVALID" );
    break;
  default:
    assert( FALSE );
  }
}

static void print_EB_REASON ( FILE *fp_out, const EB_REASON_VOBC reason ) {
  assert( fp_out );
  BOOL cont = FALSE;
  
  if( reason & (unsigned short)EB_NORMALITY ) { // Normal
    fprintf( fp_out, "EB_NORMALITY" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_EXCEEDING_SPEED_LIMIT ) { // OverSpeed
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_EXCEEDING_SPEED_LIMIT" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_NO_USED ) { // ORPBaliseDetected ?
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_NO_USED" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_EMERGENCY_STOP_RECEPTION ) { // EBCommandReceived
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_EMERGENCY_STOP_RECEPTION" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_NO_CBTC_COMMAND_RECEIVED_OR_VRS_EQUIPMENT_ERR ) { // NoCommunication
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_NO_CBTC_COMMAND_RECEIVED_OR_VRS_EQUIPMENT_ERR" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_DETECTING_OF_TRAIN_INTEGRITY_LOST ) { // RelayInputErrorFromRS
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_DETECTING_OF_TRAIN_INTEGRITY_LOST" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_ALL_DOOR_CLOSE_IS_LOST_DURING_RUNNING ) { // DoorOpenWhieMoving
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_ALL_DOOR_CLOSE_IS_LOST_DURING_RUNNING" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_TRACTION_EQUIPMENT_ABNORMALITY ) { // AbnormalTractionOrBraking
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_TRACTION_EQUIPMENT_ABNORMALITY" );
    cont = TRUE;
  }
  if( reason & (unsigned short)EB_OPERATION_STEPS_ABNORMALITY ) { // AbnormalOperationProcedure
    if( cont )
      fprintf( fp_out, ", " );
    fprintf( fp_out, "EB_OPERATION_STEPS_ABNORMALITY" );
    cont = TRUE;
  }
  if( !cont )
    fprintf( fp_out, "EB_NORMALITY" );
  fprintf( fp_out, "\n" );
}

static void print_FACTOR_IN_EMERGENCY_STOP_SC ( FILE *fp_out, const EB_REASON_SC reason ) {
  assert( fp_out );
  switch( reason ) {
  case EB_REASON_NOTHING_0:
    fprintf( fp_out, "EB_REASON_NOTHING_0" );
    break;
  case EB_ROUTE_NOT_SET:
    fprintf( fp_out, "EB_ROUTE_NOT_SET" );
    break;
  case EB_TRAIN_APPROACH:
    fprintf( fp_out, "EB_TRAIN_APPROACH" );
    break;
  case EB_CLEARANCE_NOT_KEPT:
    fprintf( fp_out, "EB_CLEARANCE_NOT_KEPT" );
    break;
  case EB_END_OF_TRACK:
    fprintf( fp_out, "EB_END_OF_TRACK" );
    break;
  case EB_ENTER_NON_AUTHORIZED_ZONE:
    fprintf( fp_out, "EB_ENTER_NON_AUTHORIZED_ZONE" );
    break;
  case EB_UNIDENTIFIED_TRAIN_AHEAD:
    fprintf( fp_out, "EB_UNIDENTIFIED_TRAIN_AHEAD" );
    break;
  case EB_REASON_NOTHING_7:
    fprintf( fp_out, "EB_REASON_NOTHING_7" );
    break;
  case EB_UNLOCKED_POINT:
    fprintf( fp_out, "EB_UNLOCKED_POINT" );
    break;
  case EB_NOENTRY_ZONE:
    fprintf( fp_out, "EB_NOENTRY_ZONE" );
    break;
  case EB_EMERGENCY_STOP_ZONE:
    fprintf( fp_out, "EB_EMERGENCY_STOP_ZONE" );
    break;
  case EB_REASON_NOTHING_11:
    fprintf( fp_out, "EB_REASON_NOTHING_11" );
    break;
  case EB_TG_ERROR:
    fprintf( fp_out, "EB_TG_ERROR" );
    break;
  case EB_TG_MISMATCH:
    fprintf( fp_out, "EB_TG_MISMATCH" );
    break;
  case EB_PSD_OPEN:
    fprintf( fp_out, "EB_PSD_OPEN" );
    break;
  case EB_EB_COMMAND:
    fprintf( fp_out, "EB_EB_COMMAND" );
    break;
  case EB_TRAIN_UNLOCATED:
    fprintf( fp_out, "EB_TRAIN_UNLOCATED" );
    break;
  case EB_TRAIN_DIRECTION_MISMATCH:
    fprintf( fp_out, "EB_TRAIN_DIRECTION_MISMATCH" );
    break;
  case EB_RADIO_RANGING_MISMATCH:
    fprintf( fp_out, "EB_RADIO_RANGING_MISMATCH" );
    break;
  case EB_SHORT_OF_WINDOW_NUMBER:
    fprintf( fp_out, "EB_SHORT_OF_WINDOW_NUMBER" );
    break;
  case EB_REMOTE_EB_COMMAND:
    fprintf( fp_out, "EB_REMOTE_EB_COMMAND" );
    break;
  case EB_TO_EMMODE_TRANSITION_SECTION:
    fprintf( fp_out, "EB_TO_EMMODE_TRANSITION_SECTION" );
    break;
  case EB_EMERGENCY_DOOR_OPEN:
    fprintf( fp_out, "EB_EMERGENCY_DOOR_OPEN" );
    break;
  case EB_TRAIN_HEAD_ON:
    fprintf( fp_out, "EB_TRAIN_HEAD_ON" );
    break;
  case EB_DEAD_SECTION:
    fprintf( fp_out, "EB_DEAD_SECTION" );
    break;
  case EB_TO_EMMODE_PREPARING_SECTION:
    fprintf( fp_out, "EB_TO_EMMODE_PREPARING_SECTION" );
    break;
  case EB_TRAININFO_PROCESS_ERROR:
    fprintf( fp_out, "EB_TRAININFO_PROCESS_ERROR" );
    break;
  case EB_UNDEFINED_REASON:
    /* fall thru. */
  default:
    fprintf( fp_out, "EB_UNDEFINED_REASON" );
  }
}

#define NOTICE_OF_TIME_NO_TIME_UPDATE 0x00
#define NOTICE_OF_TIME_VRS_TIME_UPDATE 0x01
#define NOTICE_OF_TIME_DMI_TIME_UPDATE 0x02
#define NOTICE_OF_TIME_TCMS_TIME_UPDATE 0x04
void diag_train_info ( FILE *fp_out, const TRAIN_INFO_ENTRY_PTR pE ) {
  assert( fp_out );
  assert( pE );
  
  fprintf( fp_out, "rakeID: %03d\n", TRAIN_INFO_RAKEID(*pE) );
  fprintf( fp_out, "TRR: %d\n", TRAIN_INFO_TRR(*pE) );
  fprintf( fp_out, "Acknowledge of energy saving mode: %d\n", TRAIN_INFO_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE(*pE) );
  fprintf( fp_out, "ATB OK: %d\n", TRAIN_INFO_ATB_OK(*pE) );
  fprintf( fp_out, "Push of departure button: %d\n", TRAIN_INFO_PUSH_OF_DEPARTURE_BUTTON(*pE) );
  fprintf( fp_out, "Skip Next Stop: %d\n", TRAIN_INFO_SKIP_NEXT_STOP(*pE) );
  fprintf( fp_out, "Notice of time: " );
  {
    const unsigned char v = TRAIN_INFO_NOTICE_OF_TIME(*pE);
    BOOL cont = FALSE;
    if( v & NOTICE_OF_TIME_VRS_TIME_UPDATE ) {
      fprintf( fp_out, "VRS_time_update" );
      cont = TRUE;
    }
    if( v & NOTICE_OF_TIME_DMI_TIME_UPDATE ) {
      if( cont )
	fprintf( fp_out, ", " );
      fprintf( fp_out, "DMI_time_update" );
      cont = TRUE;
    }
    if( v & NOTICE_OF_TIME_TCMS_TIME_UPDATE ) {
      if( cont )
	fprintf( fp_out, ", " );
      fprintf( fp_out, "TCMS_time_update" );
      cont = TRUE;
    }
    if( !cont )
      fprintf( fp_out, "NO_time_update" );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "OCC Command ID Ack: %d\n", (int)TRAIN_INFO_OCC_COMMANDID_ACK(*pE) );
  fprintf( fp_out, "Stop detection: %d\n", TRAIN_INFO_STOP_DETECTION(*pE) );
  fprintf( fp_out, "Dynamic testable section: %d\n", TRAIN_INFO_DYNAMIC_TESTABLE_SECTION(*pE) );
  fprintf( fp_out, "Voltage reduction: %d\n", TRAIN_INFO_VOLTAGE_REDUCTION(*pE) );
  fprintf( fp_out, "VRS(F) reset: %d\n", TRAIN_INFO_VRS_F_RESET(*pE) );
  fprintf( fp_out, "VRS ID(forward): %d\n", TRAIN_INFO_VRS_ID_FORWARD(*pE) );
  fprintf( fp_out, "Passing through balise: %d\n", TRAIN_INFO_PASSING_THROUGH_BALISE(*pE) );
  fprintf( fp_out, "overspeed: %d\n", TRAIN_INFO_OVERSPEED(*pE) );
  fprintf( fp_out, "Door Enable: %d\n", TRAIN_INFO_DOOR_ENABLE(*pE) );
  fprintf( fp_out, "VRS(B) reset: %d\n", TRAIN_INFO_VRS_B_RESET(*pE) );
  fprintf( fp_out, "VRS ID(back): %d\n", TRAIN_INFO_VRS_ID_BACK(*pE) );
  fprintf( fp_out, "Forward train position: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS(*pE) );
  fprintf( fp_out, "Forward train position offset: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET(*pE) );
  fprintf( fp_out, "Back train position: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS(*pE) );
  fprintf( fp_out, "Back train position offset: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_OFFSET(*pE) );
  fprintf( fp_out, "Occupied Block(forward): %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_FORWARD(*pE) );
  fprintf( fp_out, "Occupied Block(forward) offset: %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_FORWARD_OFFSET(*pE) );
  fprintf( fp_out, "Occupied Block(back): %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_BACK(*pE) );
  fprintf( fp_out, "Occupied Block(back) offset: %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_BACK_OFFSET(*pE) );
  fprintf( fp_out, "Forward train position segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train position offset segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Back train position segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Back train position offset segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train occupied position segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train occupied position offset segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Back train occupied segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_OCCUPIED_SEGMENT(*pE) );
  fprintf( fp_out, "Back train occupied position offset segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Train speed: %d\n", (int)TRAIN_INFO_TRAIN_SPEED(*pE) );
  fprintf( fp_out, "Train max speed: %d\n", (int)TRAIN_INFO_TRAIN_MAXSPEED(*pE) );
  {
    assert( pE );
    const TRAIN_OPERATION_MODE m = TRAIN_INFO_OPERATION_MODE( *pE );
    fprintf( fp_out, "Operation mode: " );
    print_TRAIN_OPERATION_MODE( fp_out, m );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "EB release ack: %d\n", (int)TRAIN_INFO_EB_RELEASE_ACK(*pE) );
  fprintf( fp_out, "Remote EB release: %d\n", (int)TRAIN_INFO_REMOTE_EB_RELEASE(*pE) );
  fprintf( fp_out, "distance to stopping point: %d\n", (int)TRAIN_INFO_DISTANCE_TO_STOPPING_POINT(*pE) );
  {
    assert( pE );
    const TRAIN_DETECTION_MODE m = TRAIN_INFO_TRAIN_DETECTION_MODE( *pE );
    fprintf( fp_out, "Train detection mode: " );
    print_TRAIN_DETECTION_MODE( fp_out, m );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const VEHICLE_TYPE v = TRAIN_INFO_TYPE_OF_VEHICLE( *pE );
    fprintf( fp_out, "Type of vehicle: " );
    print_TYPE_OF_VEHICLE( fp_out, v );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "P0 stopped: %d\n", (int)TRAIN_INFO_P0_STOPPED(*pE) );
  fprintf( fp_out, "Condition of Departure deterrence: %d\n", (int)TRAIN_INFO_CONDITION_OF_DEPARTURE_DETERRENCE(*pE) );
  fprintf( fp_out, "Train Removed: %d\n", (int)TRAIN_INFO_TRAIN_REMOVED(*pE) );
  fprintf( fp_out, "On-board ATC error information: %d\n", (int)TRAIN_INFO_ONBOARD_ATC_ERROR_INFORMATION(*pE) );
  fprintf( fp_out, "Wake-up Ack: %d\n", (int)TRAIN_INFO_WAKEUP_ACK(*pE) );
  fprintf( fp_out, "Sleep Ack: %d\n", (int)TRAIN_INFO_SLEEP_ACK(*pE) );
  {
    assert( pE );
    const INITIALIZATION_STATUS s = TRAIN_INFO_INITIALIZATION_STATUS(*pE);
    fprintf( fp_out, "Initialization status: " );
    print_INITIALIZATION_STATUS( fp_out, s );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const ONBOARD_ATC_MASTER_STANDBY m = TRAIN_INFO_MASTER_STANDBY( *pE );
    fprintf( fp_out, "Master/standby: " );
    print_MASTER_STANDBY( fp_out, m );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const TRAIN_MOVE_DIR d = TRAIN_INFO_RUNNING_DIRECTION( *pE );
    fprintf( fp_out, "Running Direction: " );
    print_RUNNING_DIRECTION( fp_out, d );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "Door failed to open_close: %d\n", (int)TRAIN_INFO_DOOR_FAILED_TO_OPEN_CLOSE(*pE) );
  fprintf( fp_out, "Wash Mode Status: %d\n", (int)TRAIN_INFO_WASH_MODE_STATUS(*pE) );
  {
    assert( pE );
    const TRAIN_SLEEP_MODE m = TRAIN_INFO_SLEEP_MODE( *pE );
    fprintf( fp_out, "Sleep mode: " );
    print_SLEEP_MODE( fp_out, m );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const ONBOARD_RESET_RESULT r = TRAIN_INFO_RESULT_OF_RESET_FOR_ONBOARD( *pE );
    fprintf( fp_out, "Result of reset for on-board: " );
    print_RESULT_OF_RESET_FOR_ONBOARD( fp_out, r );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const TRAIN_PERF_REGIME p = TRAIN_INFO_TRAIN_PERFORMANCE_REGIME( *pE );
    fprintf( fp_out, "Train performance regime: " );
    print_TRAIN_PERFORMANCE_REGIME( fp_out, p );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const TRAIN_DOORS_STATE s = TRAIN_INFO_STATE_OF_TRAIN_DOOR( *pE );
    fprintf( fp_out, "State of train door: " );
    print_STATE_OF_TRAIN_DOOR( fp_out, s );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "Rescue train: %d\n", (int)TRAIN_INFO_RESCUE_TRAIN(*pE) );
  {
    assert( pE );
    const ATO_DRIVING_STATUS s = TRAIN_INFO_ATO_DRIVING_STATUS( *pE );
    fprintf( fp_out, "ATO driving status: " );
    print_ATO_DRIVING_STATUS( fp_out, s );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "Forward safety buffer length: %d\n", (int)TRAIN_INFO_FORWARD_SAFETYBUF_LEN(*pE) );
  fprintf( fp_out, "Backward safety buffer length: %d\n", (int)TRAIN_INFO_BACKWARD_SAFETYBUF_LEN(*pE) );
  {
    assert( pE );
    const EB_REASON_VOBC r = TRAIN_INFO_EB_REASON( *pE );
    fprintf( fp_out, "EB reason: " );
    print_EB_REASON( fp_out, r );
    fprintf( fp_out, "\n" );
  }
  {
    assert( pE );
    const EB_REASON_SC r = TRAIN_INFO_FACTOR_IN_EMERGENCY_STOP_SC( *pE );
    fprintf( fp_out, "Factor in emergency stop(SC): " );
    print_FACTOR_IN_EMERGENCY_STOP_SC( fp_out, r );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "Emergency Brake Active: %d\n", (int)TRAIN_INFO_EMERGENCY_BRAKE_ACTIVE(*pE) );
}

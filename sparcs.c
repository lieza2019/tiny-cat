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

static int which_SC_zones( SC_ID zones[], int front_blk, int back_blk ) {  
  assert( zones );
  //assert( front_blk > 0 );
  //assert( back_blk > 0 );
  int r = -1;
  
  //zones[0] = SC817; zones[1] = SC818;
  zones[0] = SC801;
  zones[1] = SC801;
  r = 1;
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
      if( !(es[i] = lkup_train_cmd( pTs, &SC_ctrl_cmds[zones[i]], rakeID )) ) {
	assert( standby_train_cmds.pptl );
	if( !standby_train_cmds.phd ) {
	  standby_train_cmds.phd = pTs;
	  assert( standby_train_cmds.pptl == &(standby_train_cmds.phd->pNext) );
	}
	assert( !(*standby_train_cmds.pptl) );
      } else
	r++;
    }
    assert( r == n );
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

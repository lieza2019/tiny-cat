#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

SC_CTRL_CMDSET SC_ctrl_cmds[END_OF_SCs];
SC_STAT_INFOSET SC_stat_infos[END_OF_SCs] = {
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC801_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC802_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC803_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC804_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC805_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC806_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC807_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC808_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC809_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC810_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC811_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC812_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC813_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC814_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC815_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC816_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC817_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC818_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC819_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC820_Train_information}},
  {{0, 0, 0, 0}, {UDP_BCAST_RECV_PORT_SC821_Train_information}}
};

static int which_SC_zones( SC_ID zones[], int front_blk, int back_blk ) {  
  assert( zones );
  assert( front_blk > 0 );
  assert( back_blk > 0 );
  
  zones[0] = SC801;
  zones[1] = SC802;
  return( 2 );
}

static TRAIN_COMMAND_ENTRY_PTR lkup_train_cmd( SC_CTRL_CMDSET_PTR pCs, int rakeID ) {
  assert( pCs );
  assert( rakeID > 0 );
  TRAIN_COMMAND_ENTRY_PTR pE = NULL;
  
  {
    int i;
    for( i = 0; i < TRAIN_COMMAND_ENTRIES_NUM; i++ )
      if( pCs->train_cmd.entries[i].rakeID == rakeID ) {
	pE = &pCs->train_cmd.entries[i];
	break;
      }
  }
  if( !pE ) {
    int j;
    for( j = 0 ; j < pCs->train_cmd.frontier; j++ )
      if( pCs->train_cmd.entries[j].rakeID == 0 ) {
	pE = &pCs->train_cmd.entries[j];
	break;
      }
    if( !pE ) {
      assert( j == pCs->train_cmd.frontier );
      if( pCs->train_cmd.frontier < TRAIN_COMMAND_ENTRIES_NUM ) {
	int f = pCs->train_cmd.frontier;
	assert( pCs->train_cmd.entries[f].rakeID == 0 );
	pE = &pCs->train_cmd.entries[f];
	pCs->train_cmd.frontier++;
      }
    } else
      assert( FALSE );
  }
  return pE;
}

int alloc_train_cmd_entries( TRAIN_COMMAND_ENTRY_PTR es[], int rakeID, int front_blk, int back_blk ) {
  assert( es );
  assert( rakeID > 0 );
  assert( front_blk > 0 );
  assert( back_blk > 0 );
  SC_ID zones[2] = { END_OF_SCs, END_OF_SCs };
  int r = 0;
  
  int n = which_SC_zones( zones, front_blk, back_blk );
  assert( (n > 0) && (n <= 2) );
  {
    int i;
    for( i = 0; i < n; i++ ) {
      assert( zones[i] != END_OF_SCs );
      es[i] = lkup_train_cmd( &SC_ctrl_cmds[zones[i]], rakeID );
    }
    assert( i == n );
    r = i;
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
    for( i = 0; i < MAX_TRAIN_INFO_ENTRIES; i++ )
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
      assert( p == (NXNS_HEADER_PTR)(&pSC->train_information.recv) );
    }
    assert( len > 0 ? len >= sizeof(NXNS_HEADER) : len > -1 );
  }
  return pSC;
}

void phony_raw_recvbuf_traininfo( void *pbuf ) {  // ***** for debugging.
  int i;
  for( i = 0; i < MAX_TRAIN_INFO_ENTRIES; i++ )
    ((struct recv_buf_traininfo *)pbuf)->train_info.entries[i].rakeID = (uint8_t)(i + 20);
}  
void dump_raw_recvbuf_traininfo( void *pbuf ) {  // ***** for debugging.
  assert( pbuf );
  int i;
  for( i = 0; i < MAX_TRAIN_INFO_ENTRIES; i++ ) {
    unsigned short rakeID =  TRAIN_INFO_RAKEID( ((struct recv_buf_traininfo *)pbuf)->train_info.entries[i] );
    if( rakeID > 0 )
      printf( "received rakeID: %03d.\n", rakeID );
  }
}

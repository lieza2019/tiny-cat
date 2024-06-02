#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "srv.h"
#include "interlock.h"

#if 1
static SC_CTRL_CMDSET_PTR which_SC_from_train_cmd ( TRAIN_COMMAND_ENTRY_PTR pTc ) {
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

static SC_STAT_INFOSET_PTR which_SC_from_train_info ( TRAIN_INFO_ENTRY_PTR pTi ) {
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

static int enum_alive_rakes ( int rakeIDs[END_OF_SCs][TRAIN_INFO_ENTRIES_NUM + 1] ) {
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
  const TRAIN_COMMAND_ENTRY_PTR plim = &SC_ctrl_cmds[sc_id].train_command.send.train_cmd.entries[TRAIN_COMMAND_ENTRIES_NUM];
  TRAIN_COMMAND_ENTRY_PTR pE = &SC_ctrl_cmds[sc_id].train_command.send.train_cmd.entries[0];
  assert( pE );
  assert( chk_consistency( rakeIDs, num_of_rakes ) );
  int i;
  for( i = 0; i < num_of_rakes; i++ ) {
    assert( pE < plim );
    BOOL found = FALSE;
    TRAIN_COMMAND_ENTRY_PTR p = pE;
    assert( p );
    TRAIN_COMMAND_ENTRY_PTR pEp = pE;
    int rID = ntohs( p->rakeID );
    while( rID > 0 ) {
      assert( p < plim );
      if( rID == rakeIDs[i] ) {
	pE++;
	found = TRUE;
	break;
      } else {
	p++;
	if( p < plim )
	  rID = ntohs( p->rakeID );
	else
	  break;
      }
    }
    if( found )
      assert( pE == pEp++ );
    else
      assert( FALSE );
  }
  
  assert( num_of_rakes < TRAIN_COMMAND_ENTRIES_NUM ? pE < plim : pE == plim );
  if( pE < plim ) {
    const int n = (int)((unsigned char *)plim - (unsigned char *)pE);
    assert( n > 0 );
    unsigned char *q = (unsigned char *)pE;
    assert( q );
    int k;
    for( k = 0; k < n; i++ ) {
      assert( *q == 0x00 );
      q++;
    }
  }
}

static void chk_solid_train_cmds ( void ) {
  int rakeIDs[END_OF_SCs][TRAIN_INFO_ENTRIES_NUM + 1];
  int i;
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

static int diag_tracking_train_cmd ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;

  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
      int j = 0;
      do {
	TRAIN_COMMAND_ENTRY_PTR pE = trains_tracking[i].pTC[j];
	assert( j < 1 ? pE : (TRAIN_COMMAND_ENTRY_PTR)TRUE );
	int rakeID = -1;
	rakeID = (int)ntohs( pE->rakeID );
	assert( rakeID > 0 );
	assert( rakeID == trains_tracking[i].rakeID );

	fprintf( fp_out, "%s;\n", (which_SC_from_train_cmd(pE))->sc_name );
	fprintf( fp_out, "rakeID: %-3d\n", rakeID );
	j++;
      } while( (j < 2) && trains_tracking[i].pTC[j] );
      fprintf( fp_out, "\n" );
      r++;
    }
  return r;
}

static int diag_tracking_train_stat ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
      TRAIN_INFO_ENTRY_PTR pE = trains_tracking[i].pTI;
      assert( pE );
      fprintf( fp_out, "%s;\n", (which_SC_from_train_info(pE))->sc_name );
      
      assert( trains_tracking[i].rakeID == (int)TRAIN_INFO_RAKEID(*trains_tracking[i].pTI) );
      fprintf( fp_out, "rakeID: %-3d\n", trains_tracking[i].rakeID );
      
      fprintf( fp_out, "\n" );
      r++;
    }
  return r;
}

#define BUFSIZ_msgServerStatus MAX_SEND_BUFSIZ
#define BUFSIZ_msgServerHeartbeat MAX_SEND_BUFSIZ
static unsigned char buf_msgServerStatus[BUFSIZ_msgServerStatus];
static unsigned char buf_msgServerHeartbeat[BUFSIZ_msgServerStatus];
BOOL launch_msg_srv_stat ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pd_beat, TINY_SOCK_DESC *pd_stat ) {
  assert( pS );
  assert( pd_beat );
  assert( pd_stat );
  IP_ADDR_DESC srvstat_dstip = { 0, 0, 0, 0 };  
  BOOL r = FALSE;
  
  srvstat_dstip.oct_1st = BROADCAST_DSTIP_1stO;
  srvstat_dstip.oct_2nd = BROADCAST_DSTIP_2ndO;
  srvstat_dstip.oct_3rd = BROADCAST_DSTIP_3rdO;
  srvstat_dstip.oct_4th = BROADCAST_DSTIP_4thO;
  if( (*pd_beat = creat_sock_send( pS, UDP_BCAST_SEND_PORT_MsgServerHeartbeat, TRUE, &srvstat_dstip )) >= 0 ) {
    sock_attach_send_buf( pS, *pd_beat, buf_msgServerHeartbeat, sizeof(buf_msgServerHeartbeat) );
    if( (*pd_stat = creat_sock_send( pS, UDP_BCAST_SEND_PORT_msgServerStatus, TRUE, &srvstat_dstip )) >= 0 ) {
      sock_attach_send_buf( pS, *pd_stat, buf_msgServerStatus, sizeof(buf_msgServerStatus) );
      r = TRUE;
    }
  }
  
  return r;;
}

int main ( void ) {
  TINY_SOCK_DESC sd_send_srvbeat = -1;
  TINY_SOCK_DESC sd_send_srvstat = -1;
  TINY_SOCK_DESC sd_recv_srvstat = -1;
  TINY_SOCK socks;
  TINY_SOCK_CREAT( socks );
#if 0
  printf( "sizeof TRAIN_INFO_ENTRY: %d.\n", (int)sizeof(TRAIN_INFO_ENTRY) );
  printf( "sizeof TRAIN_INFO: %d.\n", (int)sizeof(TRAIN_INFO) );
  {
    struct _recv_buf_train_info {
      NXNS_HEADER header;
      uint8_t flgs_1;
      uint8_t spare_1;
      uint8_t spare_2;
      uint8_t spare_3;
      TRAIN_INFO train_info;
    };
    printf( "sizof RECV_BUF_TRAININFO: %d.\n", (int)sizeof(struct _recv_buf_train_info) );
  }
  printf( "sizeof MSG_TINY_SERVER_STATUS: %d.\n", (int)sizeof(MSG_TINY_SERVER_STATUS) );
  {
    struct _tny_srv_stat_NXNS {
      NXNS_HEADER hdr;
      MSG_TINY_SERVER_STATUS load;
    };
    printf( "sizeof TinyMsgServerStatus on NX-NS header: %d.\n", (int)sizeof(struct _tny_srv_stat_NXNS) );
  }
  exit( 0 );
#endif
  
  if( ! launch_msg_srv_stat( &socks, &sd_send_srvbeat, &sd_send_srvstat ) ) {
    errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  }
  assert( sd_send_srvstat > -1 );
  assert( sd_send_srvbeat > -1 );
  
  if( (sd_recv_srvstat = creat_sock_recv( &socks, UDP_BCAST_SEND_PORT_msgServerStatus )) < 0 ) {
    errorF( "%s", "failed to create the socket to self-receive msgServerStatus.\n" );
    exit( 1 );
  } else {
    assert( sd_recv_srvstat > -1 );
    sock_attach_recv_buf( &socks, sd_recv_srvstat, buf_msgServerStatus, sizeof(buf_msgServerStatus) );
  }
  
  if( ! establish_SC_comm( &socks ) ) {
    errorF("%s", "failed to create the recv/send UDP ports for Train information and Train command respectively.\n");
    exit( 1 );
  }
  
  {
    const useconds_t interval = 1000 * 1000 * 0.1;
    int nrecv = -1;
    int cnt = 0;
    static MSG_TINY_HEARTBEAT msg_srv_beat;
    static MSG_TINY_SERVER_STATUS msg_srv_stat;
    TINY_SRVBEAT_HEARTBEAT_SERVERID( msg_srv_beat, 1 );
    
    TINY_SRVSTAT_MSG_SERVERID( msg_srv_stat, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_JPW,  1, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_IGDA, 2, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_RKPM, 3, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_IWNR, 4, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_JLA,  5, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_BCGN, 6, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_KIKD, 7, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_RKAM, 8, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_MKPR, 9, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_NPPR, 10, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_DPCK, 11, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_PAGI, 12, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_MKPD, 13, 1 );
    TINY_SRVSTAT_REGURATION_MODE( msg_srv_stat, 1 );
    TINY_SRVSTAT_MSG_COMM_PA( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_TRAINRADIO( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_TVS( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_SCADA( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_LOGGER1( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_LOGGER2( msg_srv_stat, TRUE );
    
    while( TRUE ) {
      errorF( "%s", "waken up!\n" );
      if( (nrecv = sock_recv( &socks )) < 0 ) {
	errorF( "%s", "error on receiving CBTC status information from SCs.\n" );
	continue;
      }
      
      reveal_train_tracking( &socks );
#if 0
      if( diag_tracking_train_stat( stdout ) > 0 )
	fprintf( stdout, "\n" );
#endif
      
      {
	unsigned char *pmsg_buf = NULL;
	int msglen = -1;
	pmsg_buf = sock_recv_buf_attached( &socks, sd_recv_srvstat, &msglen );
	assert( pmsg_buf );
	assert( pmsg_buf == buf_msgServerStatus );
	assert( (msglen > 0) ? (msglen == sizeof(MSG_TINY_SERVER_STATUS)) : TRUE );
      }
      
      {
	unsigned char *pmsg_buf = NULL;
	int size = -1;
	pmsg_buf = sock_send_buf_attached( &socks, sd_send_srvbeat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_HEARTBEAT) );
	memcpy( pmsg_buf, &msg_srv_beat, sizeof(msg_srv_beat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks, sd_send_srvbeat, sizeof(msg_srv_beat) );
	  assert( n == sizeof(MSG_TINY_HEARTBEAT) );
	}
	
	pmsg_buf = NULL;
	size = -1;
	pmsg_buf = sock_send_buf_attached( &socks, sd_send_srvstat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_SERVER_STATUS) );
	msg_srv_stat.n = cnt;
	memcpy( pmsg_buf, &msg_srv_stat, sizeof(msg_srv_stat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks, sd_send_srvstat, sizeof(msg_srv_stat) );
	  assert( n == sizeof(MSG_TINY_SERVER_STATUS) );
	}
      }
      
      load_train_command();
#ifdef CHK_STRICT_CONSISTENCY
      chk_solid_train_cmds();
#endif // CHK_STRICT_CONSISTENCY
      if( diag_tracking_train_cmd( stdout ) > 0 )
	fprintf( stdout, "\n" );
#if 0
      if( sock_send(&socks) < 1 ) {
	errorF( "%s", "failed to send msgServerStatus.\n" );
	exit( 1 );
      }
#endif
      cnt++;
      assert( ! usleep( interval ) );
    }
  }
  return 0;
}
#endif

#if 0
#define RECV_BUFSIZ_msgServerStatus MAX_RECV_BUFSIZ // must be greater than / equal to MAX_RECV_BUFSIZ
#define SEND_BUFSIZ_msgServerStatus MAX_SEND_BUFSIZ // must be greater than / equal to MAX_SEND_BUFSIZ

static unsigned char recv_buf_msgServerStatus[RECV_BUFSIZ_msgServerStatus];
static unsigned char send_buf_msgServerStatus[SEND_BUFSIZ_msgServerStatus];

int main (void) {
  TINY_SOCK_DESC sd_recv, sd_send;
  TINY_SOCK socks;
  
  // msgServerStatusを、UDPにて送受信できるようになる。
  TINY_SOCK_CREAT( socks );
  if( (sd_recv = creat_sock_recv ( &socks, UDP_BCAST_SEND_PORT_msgServerStatus )) < 0 ) {
    errorF( "%s", "failed to create the socket to receive msgServerStatus.\n" );
    exit( 1 );
  } else
    sock_attach_recv_buf( &socks, sd_recv, recv_buf_msgServerStatus, sizeof(recv_buf_msgServerStatus) );
  {
    IP_ADDR_DESC dst_ipaddr = LOOPBACK_IPADDR;
    if( (sd_send = creat_sock_sendnx ( &socks, UDP_BCAST_SEND_PORT_msgServerStatus, TRUE, &dst_ipaddr )) < 0 ) {
      errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
      exit( 1 );
    } else
      sock_attach_send_buf( &socks, sd_send, send_buf_msgServerStatus, sizeof(send_buf_msgServerStatus) );
  }
  
  // Workstation commandを、TCPにて受信できるように、なる。
  ;
  
  { 
    const useconds_t interval = 1000 * 1000 * 1;
    MSG_SERVER_STATUS msg_srv_stat;
    
    msg_srv_stat.n = 1;
    while( TRUE ) {
      NXNS_HEADER_PTR pRecv_msg_srv_stat = NULL;
      assert( recv_buf_msgServerStatus == (unsigned char *)memset( recv_buf_msgServerStatus, 0, RECV_BUFSIZ_msgServerStatus ) );
      if( sock_recv( &socks ) < 0 ) {
	errorF( "%s", "error on receiving msgServerStatus, detected.\n" );
	exit( 1 );
      } else {
	int len = -1;
	pRecv_msg_srv_stat = (NXNS_HEADER_PTR)sock_recv_buf_attached( &socks, sd_recv, &len );
	assert( pRecv_msg_srv_stat );
	if( len < (sizeof(NXNS_HEADER) + sizeof(MSG_SERVER_STATUS)) )
	  errorF( "%s", "failed to receive msgServerStatus.\n" );
	else {
	  int m = ((MSG_SERVER_STATUS_PTR)NXNS_USR_DATA( pRecv_msg_srv_stat ))->n;
	  printf( "msgServerStatus.n = %d.\n", m );
	  msg_srv_stat.n = m + 1;
	}
      }
      
      {
	unsigned char *pbuf = NULL;
	int size = -1;
	pbuf = sock_send_buf_attached( &socks, sd_send, &size );
	assert( pbuf );
	assert( size >= (sizeof(NXNS_HEADER) + sizeof(MSG_SERVER_STATUS)) );
	memcpy( pbuf, &msg_srv_stat, sizeof(msg_srv_stat) );
	assert( sock_send_ready( &socks, sd_send, sizeof(msg_srv_stat) ) == sizeof(msg_srv_stat) );
	if( sock_send( &socks ) < 1 ) {
	  errorF( "%s", "failed to send msgServerStatus.\n" );
	  exit( 1 );
	}
      }
      assert( ! usleep( interval ) );
    } /* while( TRUE ) */
  }
  
  return 0;
}
#endif

#if 0
int main ( void ) {
  printf( "sizeof NXNS_HEADER: %d.\n", (int)sizeof(NXNS_HEADER) );
  printf( "sizeof TRAIN_INFO_ENTRY: %d.\n", (int)sizeof(TRAIN_INFO_ENTRY) );
  {
    SC_STAT_INFOSET S;
    printf( "sizeof recv-buffer of SC_STAT_INFOSET: %d.\n", (int)sizeof(S.train_information.recv) );
    {
      int i;
      printf( "sizeof header of SC_STAT_INFOSET: %d.\n", (int)sizeof(S.train_information.recv.header) );
      for( i = 0; i < MAX_TRAIN_INFO_ENTRIES; i++ )
	printf( "sizeof %2d th entry of Train information in SC_STAT_INFOSET: %d.\n", (i + 1), (int)sizeof(S.train_information.recv.train_info.entries[i]) );
    }
  }
  return 0;
}
#endif

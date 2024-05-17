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

static int diag_tracking_trains ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( trains_tracking[i].rakeID > 0 ) {
      assert( trains_tracking[i].rakeID == (int)(trains_tracking[i].pTI->rakeID) );
      fprintf( fp_out, "%-3d: rakeID\n", trains_tracking[i].rakeID );
      r++;
    }
  return r;
}

#define BROADCAST_DSTIPADDR LOOPBACK_IPADDR
MSG_SERVER_STATUS send_buf_MsgServerStatus;
TINY_SOCK_DESC launch_msg_srv_stat ( TINY_SOCK_PTR pS ) {
  assert( pS );
  TINY_SOCK_DESC d = -1;
  
  if( (d = creat_sock_bcast_send ( pS, UDP_BCAST_RECV_PORT_msgServerStatus, BROADCAST_DSTIPADDR )) >= 0 )
    sock_attach_send_buf( pS, d, (unsigned char *)&send_buf_MsgServerStatus, sizeof(send_buf_MsgServerStatus) );
  return d;
}

int main ( void ) {
  TINY_SOCK_DESC sd_msg_srv_stat = -1;
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
  exit( 0 );
#endif
  
  if( ! establish_SC_comm( &socks ) ) {
    errorF("%s", "failed to create the recv ports for Train information.\n");
    exit( 1 );
  }
  if( (sd_msg_srv_stat = launch_msg_srv_stat( &socks )) < 0 ) {
    errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  }
  
  {
    const useconds_t interval = 1000 * 1000 * 3;
    int nrecv = -1;
    while( TRUE ) {
      errorF( "%s", "waken up.\n" );
      if( (nrecv = recv_bcast( &socks )) < 0 ) {
	errorF( "%s", "error on receiving CBTC status information from SCs.\n" );
	continue;
      }
      reveal_train_tracking( &socks );
      if( diag_tracking_trains( stdout ) > 0 )
	fprintf( stdout, "\n" );
#if 0
      {
	MSG_SERVER_STATUS_PTR pmsg_buf = NULL;
	int size = -1;
	int cnt = 0;
	pmsg_buf = (MSG_SERVER_STATUS_PTR)sock_send_buf_attached( &socks, sd_msg_srv_stat, &size );
	assert( pmsg_buf );
	pmsg_buf->n = cnt;
	assert( sock_send_buf_ready( &socks, sd_msg_srv_stat, sizeof(MSG_SERVER_STATUS) ) == sizeof(send_buf_MsgServerStatus) );
	if( send_bcast( &socks) < 1 ) {
	  errorF( "%s", "failed to send msgServerStatus.\n" );
	  exit( 1 );
	}
	cnt++;
      }
#endif 
      assert( ! usleep( interval ) );
    }
  }
  return 0;
}

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
  if( (sd_recv = creat_sock_bcast_recv ( &socks, UDP_BCAST_RECV_PORT_msgServerStatus )) < 0 ) {
    errorF( "%s", "failed to create the socket to receive msgServerStatus.\n" );
    exit( 1 );
  } else
    sock_attach_recv_buf( &socks, sd_recv, recv_buf_msgServerStatus, sizeof(recv_buf_msgServerStatus) );
  
  if( (sd_send = creat_sock_bcast_send ( &socks, UDP_BCAST_RECV_PORT_msgServerStatus, LOOPBACK_IPADDR )) < 0 ) {
    errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  } else
    sock_attach_send_buf( &socks, sd_send, send_buf_msgServerStatus, sizeof(send_buf_msgServerStatus) );
  
  // Workstation commandを、TCPにて受信できるように、なる。
  ;

  { 
    const useconds_t interval = 1000 * 1000 * 3;
    MSG_SERVER_STATUS msg_srv_stat;
    
    msg_srv_stat.n = 1;
    while( TRUE ) {
      NXNS_HEADER_PTR pRecv_msg_srv_stat = NULL;
      assert( recv_buf_msgServerStatus == (unsigned char *)memset( recv_buf_msgServerStatus, 0, RECV_BUFSIZ_msgServerStatus ) );
      if( recv_bcast( &socks ) < 0 ) {
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
	int size = -1
	pbuf = sock_send_buf_attached( &socks, sd_send, &size );
	assert( pbuf );
	assert( size >= (sizeof(NXNS_HEADER) + sizeof(MSG_SERVER_STATUS)) );
	memcpy( pbuf, &msg_srv_stat, sizeof(msg_srv_stat) );
	assert( sock_send_buf_ready( &socks, sd_send, sizeof(msg_srv_stat) ) == sizeof(msg_srv_stat) );
	if( send_bcast( &socks ) < 1 ) {
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

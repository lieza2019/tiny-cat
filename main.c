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

int main (void ) {
  TINY_SOCK socks;
  TINY_SOCK_CREAT( socks );
  
  if( ! establish_SC_comm( &socks ) ) {
    errorF("%s", "failed to create the recv ports for Train information.\n");
    exit( 1 );
  }
  {
    const useconds_t interval = 1000 * 1000 * 3;
    while( TRUE ) {
      //errorF( "%s", "waken up.\n" );
      if( diag_tracking_trains( stdout ) > 0 )
	fprintf( stdout, "\n" );
      
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
	int size = 0;
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


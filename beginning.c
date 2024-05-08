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

#define RECV_BUFSIZ_msgServerStatus  1024
#define SEND_BUFSIZ_msgServerStatus  1024

static unsigned char recv_buf_msgServerStatus[RECV_BUFSIZ_msgServerStatus];
static unsigned char send_buf_msgServerStatus[SEND_BUFSIZ_msgServerStatus];
int main (void) {
  // Workstation commandを、TCPにて受信できるように、なる。
  
  // msgServerStatusを、UDPにて送信できるように、なる。
  TINY_SOCK_DESC sd_recv, sd_send;
  TINY_SOCK1 socks;
  
  TINY_SOCK_CREAT( socks );
  if( (sd_recv = creat_sock_bcast_recv1 ( &socks, UDP_BCAST_RECV_PORT_msgServerStatus )) < 0 ) {
    errorF( "failed to create the socket to receive msgServerStatus.\n" );
    exit( 1 );
  } else
    sock_attach_recv_buf( &socks, sd_recv, recv_buf_msgServerStatus, sizeof(recv_buf_msgServerStatus) );
  
  if( (sd_send = creat_sock_bcast_send1 ( &socks, UDP_BCAST_RECV_PORT_msgServerStatus, LOOPBACK_IPADDR )) < 0 ) {
    errorF( "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  } else
    sock_attach_send_buf( &socks, sd_send, send_buf_msgServerStatus, sizeof(send_buf_msgServerStatus) );
  
  { 
    const useconds_t interval = 1000 * 1000 * 3;
    MSG_SERVER_STATUS msg_srv_stat;
    
    msg_srv_stat.n = 1;
    while( TRUE ) {
      MSG_SERVER_STATUS_PTR pRecv_msg_srv_stat = NULL;
      assert( recv_buf_msgServerStatus == (unsigned char *)memset( recv_buf_msgServerStatus, 0, RECV_BUFSIZ_msgServerStatus ) );
      if( recv_bcast1( &socks ) < 0 ) {
	errorF( "error on receiving msgServerStatus, detected.\n" );
	exit( 1 );
      } else {
	int len = -1;
	pRecv_msg_srv_stat = (MSG_SERVER_STATUS_PTR)sock_recv_buf_attached( &socks, sd_recv, &len );
	assert( pRecv_msg_srv_stat );
	if( len < sizeof(msg_srv_stat) )
	  errorF( "failed to receive msgServerStatus.\n" );
	else {
	  printf( "msgServerStatus.n = %d.\n", pRecv_msg_srv_stat->n );
	  msg_srv_stat.n = pRecv_msg_srv_stat->n + 1;
	}
      }
      
      {
	unsigned char *pbuf = NULL;
	int size = 0;
	pbuf = sock_send_buf_attached( &socks, sd_send, &size );
	assert( pbuf );
	assert( size >= sizeof(msg_srv_stat) );
	memcpy( pbuf, &msg_srv_stat, sizeof(msg_srv_stat) );
	assert( sock_send_buf_ready( &socks, sd_send, sizeof(msg_srv_stat) ) == sizeof(msg_srv_stat) );
	if( send_bcast2( &socks ) < 1 ) {
	  errorF( "failed to send msgServerStatus.\n" );
	  exit( 1 );
	}
      }
      assert( ! usleep( interval ) );
    } /* while( TRUE ) */
  }
  
  return 0;
}

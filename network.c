#include <stdio.h>
#include <stdlib.h>
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

int TINY_SOCK_AVAIL ( struct tiny_sock_entry es[], int max_entries ) {
  assert( es );
  int r = -1;
  int i;
  for( i = 0; i < max_entries; i++ )
    if( es[i].sock < 0 ) {
      r = i;
      break;
    }
  return r;
}

static unsigned char *sock_attach_buf( struct tiny_sock_entry es[], TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( es );
  assert( td >= 0 );
  unsigned char *pPrev = NULL;
  
  pPrev = es[td].pbuf;
  es[td].pbuf = pbuf;
  es[td].size = size;
  
  return pPrev;
}

static unsigned char *sock_buf_attached( struct tiny_sock_entry es[], TINY_SOCK_DESC td, int *psize, int *plen ) {
  assert( es );
  assert( td >= 0 );

  if( psize )
    *psize = es[td].size;
  if( plen )
    *plen = es[td].len;
  
  return es[td].pbuf;
}

unsigned char *sock_recv_attach_buf( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( pS );
  assert( td >= 0 );

  return sock_attach_buf( pS->recv, td, pbuf, size );
}

unsigned char *sock_recv_buf_attached( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, int *plen ) {
  assert( pS );
  assert( td >= 0 );
  assert( plen );
  
  return sock_buf_attached( pS->recv, td, NULL, plen );
}

unsigned char *sock_send_attach_buf( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( pS );
  assert( td >= 0 );

  return sock_attach_buf( pS->send, td, pbuf, size );
}

unsigned char *sock_send_buf_attached( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, int *psize ) {
  assert( pS );
  assert( td >= 0 );
  assert( psize );
  
  return sock_buf_attached( pS->send, td, psize, NULL );
}

#if 1
int creat_sock_bcast_recv ( TINY_SOCK_PTR pS, unsigned short udp_bcast_recv_port ) {
  assert( pS );
  int r = -1;
  
  pS->addr.sin_family = AF_INET;
  pS->addr.sin_port = htons( udp_bcast_recv_port );
  pS->addr.sin_addr.s_addr = INADDR_ANY;
  
  pS->sock = socket( AF_INET, SOCK_DGRAM, 0 );
  if( pS->sock > 0 )
    r = bind( pS->sock, (struct sockaddr *)&(pS->addr), sizeof(pS->addr) );
  return r;
}
#endif
int creat_sock_bcast_recv1 ( TINY_SOCK_PTR1 pS, unsigned short udp_bcast_recv_port ) {
  assert( pS );
  int r = 1;
  
  r = TINY_SOCK_RECV_AVAIL( pS );
  if( r > 0 ) {
    pS->recv[r].addr.sin_family = AF_INET;
    pS->recv[r].addr.sin_port = htons( udp_bcast_recv_port );
    pS->recv[r].addr.sin_addr.s_addr = INADDR_ANY;
    {
      int s = -1;
      s = socket( AF_INET, SOCK_DGRAM, 0 );
      if( s < 0 )
	r = -1;
      else
	if( bind( s, (struct sockaddr *)&(pS->recv[r].addr), sizeof(pS->recv[r].addr) ) < 0 ) {
	  close( s );
	  r = -1;
	} else
	  pS->recv[r].sock = s;
      pS->recv[r].pbuf = NULL;
      pS->recv[r].size = -1;
    }
  }
  return r;
}

#if 1
int creat_sock_bcast_send ( TINY_SOCK_PTR pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr ) {
  assert( pS );
  assert( dest_host_ipaddr );
  
  pS->addr.sin_family = AF_INET;
  pS->addr.sin_port = htons( udp_bcast_dest_port );
  pS->addr.sin_addr.s_addr = inet_addr( dest_host_ipaddr );
  pS->sock = socket( AF_INET, SOCK_DGRAM, 0 );
  return( pS->sock );
}
#endif
int creat_sock_bcast_send1 ( TINY_SOCK_PTR1 pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr ) {
  assert( pS );
  assert( dest_host_ipaddr );
  int r = -1;
  
  r = TINY_SOCK_SEND_AVAIL( pS );
  if( r > 0 ) {
    pS->send[r].addr.sin_family = AF_INET;
    pS->send[r].addr.sin_port = htons( udp_bcast_dest_port );
    pS->send[r].addr.sin_addr.s_addr = inet_addr( dest_host_ipaddr );
    {
      int s = -1;
      s = socket( AF_INET, SOCK_DGRAM, 0 );
      if( s < 0 )
	r = -1;
      else
	pS->send[r].sock = s;
    }
    pS->send[r].pbuf = NULL;
    pS->send[r].size = -1;
  }
  return r;
}

#if 1
int recv_bcast ( TINY_SOCK_PTR pS, unsigned char *pbuf, int size ) {
  assert( pS );
  assert( pbuf );
  int n = -1;
  n = recv( pS->sock, pbuf, size, 0 );
  return n;
}
#endif
int recv_bcast1 ( TINY_SOCK_PTR1 pS ) {
  assert( pS );
  int r = -1;
  
  struct candidate {
    BOOL trigg;
    TINY_SOCK_DESC sd;
    int sock;
    unsigned char *pbuf;
    int buf_size;
    int wrote_len;
  };
  struct candidate valid[MAX_RECV_SOCK_NUM];
  int num_valids = 0;
  
  int i, j;
  for( i = 0, j = 0; i < MAX_RECV_SOCK_NUM; i++ )
    if( pS->recv[i].sock > 0 ) {
      valid[j].trigg = FALSE;
      valid[j].sd = i;
      valid[j].sock = pS->recv[i].sock;
      valid[j].pbuf = pS->recv[i].pbuf;
      valid[j].buf_size = pS->recv[i].size;
      valid[j].wrote_len = 0;
      j++;
    }
  assert( j <= i );
  num_valids = j;

  r = 0;
  {
    fd_set readfds;
    int nfds = 0;
    struct timeval timeout;
    int n;
    
    int k;
    FD_ZERO( &readfds );
    for( k = 0; k < num_valids; k++ ) {
      FD_SET( valid[k].sock, &readfds );
      if( nfds < valid[k].sock )
	nfds = valid[k].sock;
    }
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    n = select( (nfds + 1), &readfds, NULL, NULL, &timeout );
    if( n < 0 )
      r = -1;
    else if( n > 0 ) { 
      for( k = 0; k < num_valids; k++ )
	if( FD_ISSET( valid[k].sock, &readfds ) )
	  valid[k].trigg = TRUE;
      {
	BOOL err = FALSE;
	k = 0;
	while( (k < num_valids) && valid[k].trigg ) {
	  int m = -1;
	  m = recv( valid[k].sock, valid[k].pbuf, valid[k].buf_size, 0 );
	  if( m < 0 )
	    err = TRUE;
	  else {
	    valid[k].wrote_len = m;
	    r++;
	  }
	  k++;
	}
	assert( k <= num_valids );
	r = err ? (r * -1) : r;
      }
      
      k = 0;
      while( (k < num_valids) && valid[k].trigg ) {
	int sd = valid[k].sd;
	pS->recv[sd].len = valid[k].wrote_len;
	pS->recv[sd].dirty = TRUE;
	k++;
      }
      assert( k <= num_valids );
    } else
      assert( n == 0 );
  }
  return r;
}

#if 1
int send_bcast ( TINY_SOCK_PTR pS, unsigned char *pbuf, int len ) {
  assert( pS );
  assert( pbuf );
  int n = -1;
  n = sendto( pS->sock, pbuf, len, 0, (struct sockaddr *)&(pS->addr), sizeof(pS->addr) );
  return n;
}
#endif
int send_bcast1 ( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC sd ) {
  assert( pS );
  assert( sd >= 0 );
  int n = -1;
  n = sendto( pS->send[sd].sock, pS->send[sd].pbuf, pS->send[sd].len, 0, (struct sockaddr *)&(pS->send[sd].addr), sizeof(pS->send[sd].addr) );
  return n;
}

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

int  sock_recv_socket_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td ) {
  assert( pS );
  assert( td >= 0 );
  int s = -1;
  
  s = pS->recv[td].sock;
  assert( s > 0 );
  return s;
}

int  sock_send_socket_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td ) {
  assert( pS );
  assert( td >= 0 );
  int s = -1;
  
  s = pS->send[td].sock;
  assert( s > 0 );
  return s;
}

static unsigned char *sock_attach_buf( struct tiny_sock_entry es[], TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( es );
  assert( td >= 0 );
  unsigned char *pPrev = NULL;
  
  pPrev = es[td].pbuf;
  es[td].pbuf = pbuf;
  es[td].buf_siz = size;
  
  return pPrev;
}

static unsigned char *sock_buf_attached( struct tiny_sock_entry es[], TINY_SOCK_DESC td, int *psize, int *plen ) {
  assert( es );
  assert( td >= 0 );

  if( psize )
    *psize = es[td].buf_siz;
  if( plen )
    *plen = es[td].wrote_len;
  
  return es[td].pbuf;
}

unsigned char *sock_attach_recv_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( pS );
  assert( td >= 0 );
  
  return sock_attach_buf( pS->recv, td, pbuf, size );
}

unsigned char *sock_recv_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *plen ) {
  assert( pS );
  assert( td >= 0 );
  assert( plen );
  
  return sock_buf_attached( pS->recv, td, NULL, plen );
}

unsigned char *sock_attach_send_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size ) {
  assert( pS );
  assert( td >= 0 );

  return sock_attach_buf( pS->send, td, pbuf, size );
}

unsigned char *sock_send_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *psize ) {
  assert( pS );
  assert( td >= 0 );
  assert( psize );
  
  return sock_buf_attached( pS->send, td, psize, NULL );
}

int sock_send_buf_ready ( TINY_SOCK_PTR pS, TINY_SOCK_DESC sd, int len ) {
  assert( pS );
  assert( sd >= 0 );
  assert( pS->send[sd].sock > 0 );
  assert( pS->send[sd].pbuf );
  assert( (len >= 0) && (len <= pS->send[sd].buf_siz) );
  
  pS->send[sd].wrote_len = len;
  pS->send[sd].dirty = TRUE;
  return pS->send[sd].wrote_len;
}

int creat_sock_bcast_recv ( TINY_SOCK_PTR pS, unsigned short udp_bcast_recv_port ) {
  assert( pS );
  int r = 1;
  
  r = TINY_SOCK_RECV_AVAIL( pS );
  if( r > -1 ) {
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
      pS->recv[r].buf_siz = -1;
    }
  }
  return r;
}

int creat_sock_bcast_send ( TINY_SOCK_PTR pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr ) {
  assert( pS );
  assert( dest_host_ipaddr );
  int r = -1;
  
  r = TINY_SOCK_SEND_AVAIL( pS );
  if( r > -1 ) {
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
    pS->send[r].buf_siz = -1;
  }
  return r;
}

int recv_bcast ( TINY_SOCK_PTR pS ) {
  assert( pS );
  int r = -1;
  
  struct candidate {
    BOOL trigg;
    TINY_SOCK_DESC sd;
    int sock;
    unsigned char *pbuf;
    int buf_siz;
    int wrote_len;
  } valid[MAX_RECV_SOCK_NUM];
  int num_valids = 0;
  
  int i, j;
  for( i = 0, j = 0; i < MAX_RECV_SOCK_NUM; i++ )
    if( pS->recv[i].sock > 0 ) {
      valid[j].trigg = FALSE;
      valid[j].sd = i;
      valid[j].sock = pS->recv[i].sock;
      valid[j].pbuf = pS->recv[i].pbuf;
      valid[j].buf_siz = pS->recv[i].buf_siz;
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
      int cnt_trigg = 0;
      int l;
      for( l = 0; l < num_valids; l++ )
	if( FD_ISSET( valid[l].sock, &readfds ) ) {
	  valid[l].trigg = TRUE;
	  cnt_trigg++;
	}
      //printf( "triggered sockets: %d.\n", cnt_trigg );  // ***** for debugging.
      {
	BOOL err = FALSE;
	l = 0;
	while( l < num_valids ) {
	  struct sockaddr_in from;
	  socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);
	  int m = -1;
	  if( ! valid[l].trigg ) {
	    l++;
	    continue;
	  }
	  //phony_raw_recvbuf_traininfo( valid[l].pbuf );  // ***** for debugging.
	  m = recvfrom( valid[l].sock, valid[l].pbuf, valid[l].buf_siz, MSG_WAITALL, (struct sockaddr *)&from, &sockaddr_in_size );
	  //dump_raw_recvbuf_traininfo( valid[l].pbuf );  // ***** for debugging.
	  if( m < 0 ) {
	    err = TRUE;
	    assert( FALSE );  // ***** for debugging.
	  } else {
	    //printf( "received data length: %d.\n", m );  // ***** for debugging.
	    valid[l].wrote_len = m;
	    r++;
	  }
	  l++;
	}
	assert( l == num_valids );
	r = err ? (r * -1) : r;
      }
      
      l = 0;
      while( l < num_valids ) {
	if( ! valid[l].trigg ) {
	  l++;
	  continue;
	} else {
	  int sd = valid[l].sd;
	  pS->recv[sd].wrote_len = valid[l].wrote_len;
	  pS->recv[sd].dirty = TRUE;
	  l++;
	}
      }
      assert( l == num_valids );
    } else {
      assert( n == 0 );  // for the case of TIMEOUT, and do nothing.
      //printf( "no received, in this turn in recv_bcast.\n" );  // ***** for debugging.
    }
  }
  //printf( "result of recv_bcast r: %d.\n", r );  // ***** for debugging.
  return r;
}

static size_t envelop_with_the_header ( NXNS_HEADER_PTR phdr, unsigned char *prawdata, int rawdata_len, int sendto_bufsiz ) {
  assert( phdr );
  assert( prawdata );
  assert( rawdata_len >= 0 );
  assert( sendto_bufsiz >= sizeof(NXNS_HEADER) );
  assert( (sizeof(NXNS_HEADER) + rawdata_len) <= sendto_bufsiz );
  
  unsigned char buf[MAX_SEND_BUFSIZ];
  unsigned char *pbuf = NULL;
  int n = 0;
  
  assert( pbuf = memcpy( buf, phdr, sizeof(NXNS_HEADER) ) );
  n += sizeof( NXNS_HEADER );
  if( rawdata_len > 0 ) {
    assert( n == sizeof(NXNS_HEADER) );
    assert( pbuf == buf );
    assert( pbuf = memcpy( (pbuf + n), prawdata, rawdata_len ) );
    n += rawdata_len;
  }
  
  assert( prawdata == memcpy( prawdata, buf, n ) );
#if 1
    assert( NEXUS_HDR(*(NXNS_HEADER_PTR)prawdata).H_TYPE_headerType[0] == 'N' );
    assert( NEXUS_HDR(*(NXNS_HEADER_PTR)prawdata).H_TYPE_headerType[1] == 'U' );
    assert( NEXUS_HDR(*(NXNS_HEADER_PTR)prawdata).H_TYPE_headerType[2] == 'X' );
    assert( NEXUS_HDR(*(NXNS_HEADER_PTR)prawdata).H_TYPE_headerType[3] == 'M' );
#endif
  return n;
}

int send_bcast ( TINY_SOCK_PTR pS ) {
  assert( pS );
  BOOL err = FALSE;
  int r = 0;
  int i;
  
  for( i = 0; i < MAX_SEND_SOCK_NUM; i++ ) {
    if( pS->send[i].sock > 0 )
      if( pS->send[i].dirty ) {
	NXNS_HEADER nx_ns_hdr;
	NX_HEADER_CREAT( NEXUS_HDR( nx_ns_hdr ) );
#if 1
	assert( NEXUS_HDR(nx_ns_hdr).H_TYPE_headerType[0] == 'N' );
	assert( NEXUS_HDR(nx_ns_hdr).H_TYPE_headerType[1] == 'U' );
	assert( NEXUS_HDR(nx_ns_hdr).H_TYPE_headerType[2] == 'X' );
	assert( NEXUS_HDR(nx_ns_hdr).H_TYPE_headerType[3] == 'M' );
#endif
	{
	  int n = -1;
	  int wrote_with_hdr_len = -1;
	  wrote_with_hdr_len = envelop_with_the_header( &nx_ns_hdr, pS->send[i].pbuf, pS->send[i].wrote_len, MAX_SEND_BUFSIZ );
	  assert( wrote_with_hdr_len == (sizeof(NXNS_HEADER) + pS->send[i].wrote_len) );
#if 1
	  assert( NEXUS_HDR( *(NXNS_HEADER_PTR)(pS->send[i].pbuf) ).H_TYPE_headerType[0] == 'N' );
	  assert( NEXUS_HDR( *(NXNS_HEADER_PTR)(pS->send[i].pbuf) ).H_TYPE_headerType[1] == 'U' );
	  assert( NEXUS_HDR( *(NXNS_HEADER_PTR)(pS->send[i].pbuf) ).H_TYPE_headerType[2] == 'X' );
	  assert( NEXUS_HDR( *(NXNS_HEADER_PTR)(pS->send[i].pbuf) ).H_TYPE_headerType[3] == 'M' );
#endif	  
	  n = sendto( pS->send[i].sock, pS->send[i].pbuf, wrote_with_hdr_len, 0, (struct sockaddr *)&(pS->send[i].addr), sizeof(pS->send[i].addr) );
	  if( n < 0 )
	    err = FALSE;
	  else
	    r++;
	}
	pS->send[i].dirty = FALSE;
      }
  }
  r = err ? (r * -1) : r;
  return r;
}

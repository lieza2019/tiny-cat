typedef int TINY_SOCK_DESC;
typedef struct tiny_sock {
  int sock;
  struct sockaddr_in addr;
} TINY_SOCK, *TINY_SOCK_PTR;

#define MAX_RECV_SOCK_NUM 8
#define MAX_SEND_SOCK_NUM 8
struct tiny_sock_entry {
  int sock;
  struct sockaddr_in addr;
  unsigned char *pbuf;
  int size;
  int len;
  BOOL dirty;
};
typedef struct _tiny_sock {
  struct tiny_sock_entry recv[MAX_RECV_SOCK_NUM];
  struct tiny_sock_entry send[MAX_SEND_SOCK_NUM];
} TINY_SOCK1, *TINY_SOCK_PTR1;

#define TINY_SOCK_CREAT( S ) (				\
  {							\
    int i;						\
    for( i = 0; i < MAX_RECV_SOCK_NUM; i++ ) {		\
      (S).recv[i].sock = -1;				\
    }							\
    for( i = 0; i < MAX_SEND_SOCK_NUM; i++ ) {		\
      (S).send[i].sock = -1;				\
    }							\
  }							\
)

#if 0
int TINY_SOCK_RECV_AVAIL ( TINY_SOCK_PTR1 pS ) {
  assert( pS );
  int r = -1;
  int i;
  for( i = 0; i < MAX_RECV_SOCK_NUM; i++ )
    if( pS->recv[i].sock < 0 ) {
      r = i;
      break;
    }
  return r;
}
#else
extern int TINY_SOCK_AVAIL ( struct tiny_sock_entry es[], int max_entries );
#endif
#define TINY_SOCK_RECV_AVAIL( pS ) ( (assert( (pS) )), TINY_SOCK_AVAIL( (pS)->recv,  MAX_RECV_SOCK_NUM ) )
#define TINY_SOCK_SEND_AVAIL( pS ) ( (assert( (pS) )), TINY_SOCK_AVAIL( (pS)->send,  MAX_SEND_SOCK_NUM ) )


extern unsigned char *sock_recv_attach_buf( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_recv_buf_attached( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, int *psize );
extern unsigned char *sock_send_attach_buf( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_send_buf_attached( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC td, int *psize );

extern int creat_sock_bcast_recv ( TINY_SOCK_PTR pS, unsigned short udp_bcast_recv_port );
extern int creat_sock_bcast_recv1 ( TINY_SOCK_PTR1 pS, unsigned short udp_bcast_recv_port );
  
extern int creat_sock_bcast_send ( TINY_SOCK_PTR pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr );
extern int creat_sock_bcast_send1 ( TINY_SOCK_PTR1 pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr );

extern int recv_bcast ( TINY_SOCK_PTR pS, unsigned char *pbuf, int size );
extern int recv_bcast1 ( TINY_SOCK_PTR1 pS );

extern int send_bcast ( TINY_SOCK_PTR pS, unsigned char *pbuf, int len );
extern int send_bcast1 ( TINY_SOCK_PTR1 pS, TINY_SOCK_DESC sd );

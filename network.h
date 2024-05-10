#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "generic.h"

#define MAX_RECV_BUFSIZ (16 * 1024) // in bytes
#define MAX_SEND_BUFSIZ (16 * 1024) // in bytes
struct tiny_sock_entry {
  int sock;
  struct sockaddr_in addr;
  unsigned char *pbuf;
  int buf_siz;
  int wrote_len;
  BOOL dirty;
};

#if 0
typedef struct tiny_sock {
  int sock;
  struct sockaddr_in addr;
} TINY_SOCK, *TINY_SOCK_PTR;
#else
#define MAX_RECV_SOCK_NUM 8
#define MAX_SEND_SOCK_NUM 8
typedef struct _tiny_sock {
  struct tiny_sock_entry recv[MAX_RECV_SOCK_NUM];
  struct tiny_sock_entry send[MAX_SEND_SOCK_NUM];
} TINY_SOCK, *TINY_SOCK_PTR;
#endif

typedef int TINY_SOCK_DESC;

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

extern int TINY_SOCK_AVAIL ( struct tiny_sock_entry es[], int max_entries );
#define TINY_SOCK_RECV_AVAIL( pS ) ( (assert( (pS) )), TINY_SOCK_AVAIL( (pS)->recv,  MAX_RECV_SOCK_NUM ) )
#define TINY_SOCK_SEND_AVAIL( pS ) ( (assert( (pS) )), TINY_SOCK_AVAIL( (pS)->send,  MAX_SEND_SOCK_NUM ) )

extern unsigned char *sock_attach_recv_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_recv_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *psize );
extern unsigned char *sock_attach_send_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_send_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *psize );
extern int sock_send_buf_ready( TINY_SOCK_PTR pS, TINY_SOCK_DESC sd, int len );

extern int creat_sock_bcast_recv ( TINY_SOCK_PTR pS, unsigned short udp_bcast_recv_port );
extern int creat_sock_bcast_send ( TINY_SOCK_PTR pS, unsigned short udp_bcast_dest_port, const char *dest_host_ipaddr );

extern int recv_bcast ( TINY_SOCK_PTR pS );
extern int send_bcast ( TINY_SOCK_PTR pS );

#define NS_NXHDR_FLG1_M_CTL_MLT 1
#define NS_NXHDR_FLG1_M_CTL_ONE 2
#define NS_NXHDR_FLG1_M_CTL_INQ 4
#define NS_NXHDR_FLG1_M_CTL_RPL 8

typedef struct NEXUS_header {
  char H_TYPE_headerType[4]; // = "NUXM";
  uint32_t ML_messageLength; //= 200;
  
  uint8_t SA_DMN_srcAddrDomainNum; // = 104;
  uint8_t SA_DFN_srcAddrDataFieldNum; // = 105;
  uint16_t SA_LNN_srcAddrLogicNodeNum; // = 106;
  
  uint8_t DA_DMN_dstAdderDomainNum; // = 106;
  uint8_t DA_DFN_dstAdderDataFieldNum; // = 107;
  uint16_t DA_MGN_dstAdderMulticastGroupNum; // = 109;
  
  uint32_t V_SEQ_versionSequenceNum; // = 100;
  uint32_t SEQ_sequenceNum; // = 200;
  
  uint8_t M_CTL_flgs_1; // appRPL = True, appINQ = True, appONE = True, appMLT = True
  uint8_t M_CTL_flgs_2; // constantly, 0 filled.
  uint8_t M_CTL_flgs_3; // constantly, 0 filled.
  uint8_t M_CTL_flgs_4; // constantly, 0 filled.
  
  unsigned char INQ_ID[12]; // constantly, 0 filled.
  uint16_t TCD_transactionCode; // = 100;
  uint16_t VER_versionNum; // = 200;
  unsigned char GTID_gtID[8]; // constantly, 0 filled.
  uint16_t MODE; // = 104;
  uint8_t PVER_nxProtocolVersion; // = 105;
  uint8_t PRI_messagePriorityLevel; // = 106;
  uint8_t CBN_currentFlagmentBlockNumber; // constantly, 1 fixed.
  uint8_t TBN_totalFlagmentBlockNumber; // constantly, 1 fixed.
  uint16_t BSIZE_blockSize; // = 100;
  uint32_t FUI_forFuture; // constantly, 0 filled.
} NX_HEADER, *NX_HEADER_PTR;

#define NEXUS_HEADER_CREAT( N ) (				\
  {								\
    (N).H_TYPE_headerType[0] = 'N';				\
    (N).H_TYPE_headerType[1] = 'U';				\
    (N).H_TYPE_headerType[2] = 'X';				\
    (N).H_TYPE_headerType[3] = 'M';				\
  }								\
)

#define NS_USRHDR_FLG1_SYSTEM_EXCHANGE_CMD 3
#define NS_USRHDR_FLG1_RESETCMD 4

#define NS_USRHDR_FLG2_NETWORK_TIME 1
#define NS_USRHDR_FLG2_GPS_TM_STATUS 2
#define NS_USRHDR_FLG2_TM_CHANGE_CMD 4
#define NS_USRHDR_NETWORK_TIME( H ) ((((H).flgs_2 & NS_USRHDR_FLG2_NETWORK_TIME) << 24) + ((H).networkTime[0] << 16) + ((H).networkTime[1] << 8) + (H).networkTime[2])

typedef struct NS_user_header {
  uint8_t srcID; // = 200;
  uint8_t srcStatus; // = 103;
  uint8_t msgType; // = 0;n
  uint8_t dstID; // = 104;
  uint8_t flgs_1;
  uint8_t ackEM; // = 105;
  uint16_t dataLength; // = 106;
  uint8_t flgs_2;
  uint8_t networkTime[3];
} NS_USRHDR, *NS_USRHDR_PTR;

#define NEXUS_HDR( H ) ((H).nx_hdr)
#define NS_USR_HDR( H ) ((H).ns_usr_hdr)

typedef struct NX_NS_header {
  NX_HEADER nx_hdr;
  NS_USRHDR ns_usr_hdr;
} NX_NS_HEADER, *NX_NS_HEADER_PTR;

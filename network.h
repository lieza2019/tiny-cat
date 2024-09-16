#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "generic.h"

#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED
typedef struct ip_addr_desc {
  unsigned char oct_1st;
  unsigned char oct_2nd;
  unsigned char oct_3rd;
  unsigned char oct_4th;
} IP_ADDR_DESC, *IP_ADDR_DESC_PTR;

#define MAX_RECV_BUFSIZ (16 * 1024) // in bytes
#define MAX_SEND_BUFSIZ (16 * 1024) // in bytes
struct tiny_sock_entry {
  int sock;
  struct sockaddr_in addr;
  BOOL is_nx;
  unsigned char *pbuf;
  int buf_siz;
  int wrote_len;
  BOOL dirty;
};
#define MAX_RECV_SOCK_NUM 256
#define MAX_SEND_SOCK_NUM 256
typedef struct _tiny_sock {
  struct tiny_sock_entry recv[MAX_RECV_SOCK_NUM];
  struct tiny_sock_entry send[MAX_SEND_SOCK_NUM];
} TINY_SOCK, *TINY_SOCK_PTR;

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

extern int sock_recv_socket_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td );
extern int sock_send_socket_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td );

extern unsigned char *sock_attach_recv_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_recv_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *psize );
extern unsigned char *sock_attach_send_buf( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, unsigned char *pbuf, int size );
extern unsigned char *sock_send_buf_attached( TINY_SOCK_PTR pS, TINY_SOCK_DESC td, int *psize );
extern int sock_send_ready ( TINY_SOCK_PTR pS, TINY_SOCK_DESC sd, int len );

extern int creat_sock_recv ( TINY_SOCK_PTR pS, unsigned short udp_bcast_recv_port );
extern TINY_SOCK_DESC creat_sock_send ( TINY_SOCK_PTR pS, unsigned short udp_dst_port, BOOL bcast, const IP_ADDR_DESC_PTR pIPdesc );
extern TINY_SOCK_DESC creat_sock_sendnx ( TINY_SOCK_PTR pS, unsigned short udp_dst_port, BOOL bcast, const IP_ADDR_DESC_PTR pIPdesc );

extern int sock_recv ( TINY_SOCK_PTR pS );
extern int sock_send ( TINY_SOCK_PTR pS );

typedef struct NEXUS_header {
  char H_TYPE_headerType[4]; // set the NEXUS signature constant of "NUXM".
  uint32_t ML_messageLength;
  
  uint8_t SA_DMN_srcAddrDomainNum;
  uint8_t SA_DFN_srcAddrDataFieldNum;
  uint16_t SA_LNN_srcAddrLogicNodeNum;
  
  uint8_t DA_DMN_dstAdderDomainNum;
  uint8_t DA_DFN_dstAdderDataFieldNum;
  uint16_t DA_MGN_dstAdderMulticastGroupNum;
  
  uint32_t V_SEQ_versionSequenceNum;
  uint32_t SEQ_sequenceNum;
  
  uint8_t M_CTL_flgs_1; // constantly, 0x80 as only MLT is set.
  uint8_t M_CTL_flgs_2; // constantly, 0 filled.
  uint8_t M_CTL_flgs_3; // constantly, 0 filled.
  uint8_t M_CTL_flgs_4; // constantly, 0 filled.
  
  unsigned char INQ_ID[12]; // constantly, 0 filled.
  uint16_t TCD_transactionCode;
  uint16_t VER_versionNum; // constantly, 0 filled.
  unsigned char GTID_gtID[8]; // constantly, 0 filled.
  uint16_t MODE;
  uint8_t PVER_nxProtocolVersion; // constantly, 0 filled.
  uint8_t PRI_messagePriorityLevel; // constantly, 0 filled.
  uint8_t CBN_currentFlagmentBlockNumber; // constantly, 0 filled.
  uint8_t TBN_totalFlagmentBlockNumber; // constantly, 0 filled.
  uint16_t BSIZE_blockSize; // constantly, 0 filled.
  uint8_t FUI_forFuture[4]; // constantly, 0 filled.
} NX_HEADER, *NX_HEADER_PTR;

extern BOOL NX_HEADER_M_CTL_RPL( NX_HEADER NX_hdr );
extern BOOL NX_HEADER_M_CTL_INQ( NX_HEADER NX_hdr );
extern BOOL NX_HEADER_M_CTL_ONE( NX_HEADER NX_hdr );
extern BOOL NX_HEADER_M_CTL_MLT( NX_HEADER NX_hdr );
extern void NX_HEADER_CREAT(NX_HEADER NX_hdr );

#define NX_HEADER_FLG1_M_CTL_RPL 16
#define NX_HEADER_FLG1_M_CTL_INQ 32
#define NX_HEADER_FLG1_M_CTL_ONE 64
#define NX_HEADER_FLG1_M_CTL_MLT 128
#define NX_HEADER_M_CTL_RPL( H ) (((H).M_CTL_flgs_1 & NX_HEADER_FLG1_M_CTL_RPL) >> 4)
#define NX_HEADER_M_CTL_INQ( H ) (((H).M_CTL_flgs_1 & NX_HEADER_FLG1_M_CTL_INQ) >> 5)
#define NX_HEADER_M_CTL_ONE( H ) (((H).M_CTL_flgs_1 & NX_HEADER_FLG1_M_CTL_ONE) >> 6)
#define NX_HEADER_M_CTL_MLT( H ) (((H).M_CTL_flgs_1 & NX_HEADER_FLG1_M_CTL_MLT) >> 7)

#define NX_HEADER_CREAT( N ) (					\
  {								\
    (N).H_TYPE_headerType[0] = 'N';				\
    (N).H_TYPE_headerType[1] = 'U';				\
    (N).H_TYPE_headerType[2] = 'X';				\
    (N).H_TYPE_headerType[3] = 'M';				\
    (N).M_CTL_flgs_1 = 0;					\
    (N).M_CTL_flgs_1 |= (uint8_t)NX_HEADER_FLG1_M_CTL_MLT;	\
    (N).M_CTL_flgs_2 = 0;					\
    (N).M_CTL_flgs_3 = 0;					\
    (N).M_CTL_flgs_4 = 0;					\
    (N).INQ_ID[0] = 0;						\
    (N).INQ_ID[1] = 0;						\
    (N).INQ_ID[2] = 0;						\
    (N).INQ_ID[3] = 0;						\
    (N).INQ_ID[4] = 0;						\
    (N).INQ_ID[5] = 0;						\
    (N).INQ_ID[6] = 0;						\
    (N).INQ_ID[7] = 0;						\
    (N).INQ_ID[8] = 0;						\
    (N).INQ_ID[9] = 0;						\
    (N).INQ_ID[10] = 0;						\
    (N).INQ_ID[11] = 0;						\
    (N).VER_versionNum = 0;					\
    (N).GTID_gtID[0] = 0;					\
    (N).GTID_gtID[1] = 0;					\
    (N).GTID_gtID[2] = 0;					\
    (N).GTID_gtID[3] = 0;					\
    (N).GTID_gtID[4] = 0;					\
    (N).GTID_gtID[5] = 0;					\
    (N).GTID_gtID[6] = 0;					\
    (N).GTID_gtID[7] = 0;					\
    (N).PVER_nxProtocolVersion = 0;				\
    (N).PRI_messagePriorityLevel = 0;				\
    (N).CBN_currentFlagmentBlockNumber = 0;			\
    (N).TBN_totalFlagmentBlockNumber = 0;			\
    (N).BSIZE_blockSize = 0;					\
    (N).FUI_forFuture[0] = 0;					\
    (N).FUI_forFuture[1] = 0;					\
    (N).FUI_forFuture[2] = 0;					\
    (N).FUI_forFuture[3] = 0;					\
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
  uint8_t msgType; // = 0;
  uint8_t dstID; // = 104;
  uint8_t flgs_1;
  uint8_t ackEM; // = 105;
  uint16_t dataLength; // = 106;
  uint8_t flgs_2; // constantly, 0 filled.
  uint8_t networkTime[3]; // constantly, 0 filled.
} NS_USRHDR, *NS_USRHDR_PTR;

typedef struct NX_NS_header {
  NX_HEADER nx_hdr;
  NS_USRHDR ns_usr_hdr;
} NXNS_HEADER, *NXNS_HEADER_PTR;

#define NEXUS_HDR( H ) ((H).nx_hdr)
#define NS_USRHDR( H ) ((H).ns_usr_hdr)
#define NXNS_PAYLOAD( pG ) ((unsigned char *)(pG) + (int)sizeof(NXNS_HEADER))

#define NS_USRHDR_CREAT( H ) (					\
  {								\
    (H).srcID = 200;						\
    (H).srcStatus = 103;					\
    (H).msgType = 0;						\
    (H).dstID = 0;						\
    (H).flgs_1 = 0;						\
    (H).ackEM = 105;						\
    (H).dataLength = 0;						\
    (H).flgs_2 = 0;						\
    (H).networkTime[0] = 0;					\
    (H).networkTime[1] = 0;					\
    (H).networkTime[2] = 0;					\
  }								\
)
#endif // NETWORK_H_INCLUDED

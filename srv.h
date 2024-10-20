#ifndef SRV_H_INCLUDED
#define SRV_H_INCLUDED

#include <stdint.h>
#include "network.h"
#include "sparcs.h"
#include "cbi.h"

typedef struct tiny_comm_prof {
  struct {
    struct {
      TINY_SOCK socks;
      TINY_SOCK_DESC descs[END_OF_ATS2OC];
    } ctrl;
    struct {
      TINY_SOCK socks;
      TINY_SOCK_DESC descs[END_OF_OC2ATS];
    } stat;
  } cbi;
  struct {
    struct {
      TINY_SOCK socks;
      struct {
	SC_CTRLCMD_COMM_PROF_PTR pprofs[END_OF_SCs];
      } train_cmd;
    } cmd; // currently, for only Train command.
    struct {
      TINY_SOCK socks;
      struct {
	TINY_SOCK_DESC descs[END_OF_SCs];
	BOOL ready;
      } train_info;
    } info; // currently, for only Train information.
  } cbtc;
} TINY_COMM_PROF, *TINY_COMM_PROF_PTR;

typedef enum _ars_rejected_reason {
  ARS_TRACKS_OCCUPIED,
  ARS_TRACKS_LOCKED,
  ARS_WELL_CONDITION
} ARS_REJECTED_REASON;

//#define UDP_BCAST_SEND_PORT_MsgServerHeartbeat 60000
#define UDP_BCAST_SEND_PORT_MsgServerHeartbeat 61000
#define UDP_BCAST_SEND_PORT_msgServerStatus 61001
//#define BROADCAST_DST_IPADDR "255.255.255.255"
#define BROADCAST_DSTIP_1stO 172
#define BROADCAST_DSTIP_2ndO 21
#define BROADCAST_DSTIP_3rdO 255
#define BROADCAST_DSTIP_4thO 255
#define LOOPBACK_IPADDR {127, 0, 0, 1}

typedef enum tiny_regulation_mode {
  TNY_CUTOFF,
  TNY_TIMETABLE_REGULATION,
  TNY_HEADWAY_REGULATION
} TINY_REGULATION_MODE;

typedef enum IXL_of_Line8 {
  ACR_JPW,
  ACR_IGDA,
  ACR_RKPM,
  ACR_IWNR,
  ACR_JLA,
  ACR_BCGN,
  ACR_KIKD,
  ACR_RKAM,
  ACR_MKPR,
  ACR_NPPR,
  ACR_DPCK,
  ACR_PAGI,
  ACR_MKPD,
#if 1
  END_OF_ACRs
#else
  ACR_GAGR, // not implemented yet.
  ACR_KPEN, // not implemented yet.
  ACR_JSTB, // not implemented yet.
  END_OF_ACRs
#endif
} IXL_of_Line8;

struct tny_acr_of_IXL {
  uint8_t user_id;
  uint8_t ws_id;
};

typedef struct ats_version {
  struct {
    uint8_t n1, n2, n3, n4;
  } version;
  uint32_t hash;
  uint32_t git_hash;
  uint8_t flgs_1; // atsGitHashIsDirty :: BOOL at MSB.
} ATS_VERSION, *ATS_VERSION_PTR;

typedef struct msgTinyServerHeartbeat {
  uint8_t tny_heartbeatServerID;
  ATS_VERSION tny_ats_version;
  ;
  int n;
  //uint8_t padding[8];
} MSG_TINY_HEARTBEAT, *MSG_TINY_HEARTBEAT_PTR;

extern int TINY_SRVBEAT_HEARTBEAT_SERVERID( MSG_TINY_HEARTBEAT B, int msgServerID );


#define TINY_SRVBEAT_HEARTBEAT_SERVERID( B, _msgServerID ) ((B).tny_heartbeatServerID = (_msgServerID))
#define TINY_SRVBEAT_HEARTBEAT_ATSVER_ATSVERSION
#define TINY_SRVBEAT_HEARTBEAT_ATSVER_ATSHASH
#define TINY_SRVBEAT_HEARTBEAT_ATSVER_ATSGITHASH
#define TINY_SRVBEAT_HEARTBEAT_ATSVER_ATSGITHASH_ISDIRTY

#define MAX_UDP_PAYLOAD_SIZ 1472
typedef struct msgTinyServerStatus {
  uint8_t tny_msgServerID;
  struct tny_acr_of_IXL currentACR[END_OF_ACRs];
  uint8_t tny_RegulationMode;
  uint8_t flgs_1; // msgCommPA, msgCommTrainRadio, msgCommTVS, msgCommSCADA, msgCommLogger_1, msgCommLogger_2, N/A, N/A.
  ;
  uint8_t n;
  uint8_t padding[76];
  //uint8_t padding[MAX_UDP_PAYLOAD_SIZ - 106];
} MSG_TINY_SERVER_STATUS, *MSG_TINY_SERVER_STATUS_PTR;

#define FLG1_TINY_MSG_COMM_PA 128
#define FLG1_TINY_MSG_COMM_TRAINRADIO 64
#define FLG1_TINY_MSG_COMM_TVS 32
#define FLG1_TINY_MSG_COMM_SCADA 16
#define FLG1_TINY_MSG_COMM_LOGGER1 8
#define FLG1_TINY_MSG_COMM_LOGGER2 4

extern int TINY_SRVSTAT_MSG_SERVERID( MSG_TINY_SERVER_STATUS S, int msgServerID );

extern int TINY_SRVSTAT_CURRENT_ACR_USERID( MSG_TINY_SERVER_STATUS S, IXL_of_Line8 IXL, int user_id );
extern int TINY_SRVSTAT_CURRENT_ACR_WSID( MSG_TINY_SERVER_STATUS S, IXL_of_Line8 IXL, int ws_id );
extern void TINY_SRVSTAT_CURRENT_ACR( MSG_TINY_SERVER_STATUS S, IXL_of_Line8 IXL, int user_id, int ws_id );

extern TINY_REGULATION_MODE TINY_SRVSTAT_REGURATION_MODE( MSG_TINY_SERVER_STATUS S, TINY_REGULATION_MODE regulationMode );
extern BOOL TINY_SRVSTAT_MSG_COMM_PA( MSG_TINY_SERVER_STATUS S, BOOL comm_PA );
extern BOOL TINY_SRVSTAT_MSG_COMM_TRAINRADIO( MSG_TINY_SERVER_STATUS S, BOOL commTrainRadio );
extern BOOL TINY_SRVSTAT_MSG_COMM_TVS( MSG_TINY_SERVER_STATUS S, BOOL commTVS );
extern BOOL TINY_SRVSTAT_MSG_COMM_SCADA( MSG_TINY_SERVER_STATUS S, BOOL commSCADA );
extern BOOL TINY_SRVSTAT_MSG_COMM_LOGGER1( MSG_TINY_SERVER_STATUS S, BOOL commLogger_1 );
extern BOOL TINY_SRVSTAT_MSG_COMM_LOGGER2( MSG_TINY_SERVER_STATUS S, BOOL commLogger_2 );
  
//#define TINY_SRVSTAT_MSG_SERVERID( S, _msgServerID ) (((S).tny_msgServerID = (_msgSererID)), ((S).tny_msgServerID))
#define TINY_SRVSTAT_MSG_SERVERID( S, _msgServerID ) ((S).tny_msgServerID = (_msgServerID))

#define TINY_SRVSTAT_CURRENT_ACR_USERID( S, _IXL, _user_id ) ((S).currentACR[(_IXL)].user_id = (_user_id))
#define TINY_SRVSTAT_CURRENT_ACR_WSID( S, _IXL, _ws_id ) ((S).currentACR[(_IXL)].ws_id = (_ws_id))
#define TINY_SRVSTAT_CURRENT_ACR( S, _IXL, _user_id, _ws_id ) (		\
  {									\
    assert( (_user_id) == TINY_SRVSTAT_CURRENT_ACR_USERID((S), (_IXL), (_user_id)) ); \
    assert( (_ws_id) == TINY_SRVSTAT_CURRENT_ACR_WSID((S), (_IXL), (_ws_id)) ); \
  }									\
)
#define TINY_SRVSTAT_REGURATION_MODE( S, _RegulationMode ) ((TINY_REGULATION_MODE)((S).tny_RegulationMode = (_RegulationMode)))
#define TINY_SRVSTAT_MSG_COMM_PA( S, _comm_PA )				\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_PA) | ((_comm_PA) << nbits_sft((FLG1_TINY_MSG_COMM_PA))))),	\
   (BOOL)((S).flgs_1 & FLG1_TINY_MSG_COMM_PA))
#define TINY_SRVSTAT_MSG_COMM_TRAINRADIO( S, _commTrainRadio )		\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_TRAINRADIO) | (_commTrainRadio << nbits_sft((FLG1_TINY_MSG_COMM_TRAINRADIO))))), \
   ((S).flgs_1 & FLG1_TINY_MSG_COMM_TRAINRADIO))
#define TINY_SRVSTAT_MSG_COMM_TVS( S, _commTVS )			\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_TVS) | (_commTVS << nbits_sft(FLG1_TINY_MSG_COMM_TVS)))), \
   ((S).flgs_1 & FLG1_TINY_MSG_COMM_TVS))
#define TINY_SRVSTAT_MSG_COMM_SCADA( S, _commSCADA )			\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_SCADA) | (_commSCADA << nbits_sft(FLG1_TINY_MSG_COMM_SCADA)))), \
   ((S).flgs_1 & FLG1_TINY_MSG_COMM_SCADA))
#define TINY_SRVSTAT_MSG_COMM_LOGGER1( S, _commLogger_1 )		\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_LOGGER1) | (_commLogger_1 << nbits_sft(FLG1_TINY_MSG_COMM_LOGGER1)))), \
   ((S).flgs_1 & FLG1_TINY_MSG_COMM_LOGGER1))
#define TINY_SRVSTAT_MSG_COMM_LOGGER2( S, _commLogger_2 )		\
  (((S).flgs_1 = (((S).flgs_1 & ~FLG1_TINY_MSG_COMM_LOGGER2) | (_commLogger_2 << nbits_sft(FLG1_TINY_MSG_COMM_LOGGER2)))), \
   ((S).flgs_1 & FLG1_TINY_MSG_COMM_LOGGER2))

extern void reveal_train_tracking ( TINY_COMM_PROF_PTR pcomm_prof );
#endif //SRV_H_INCLUDED

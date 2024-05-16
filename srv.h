#include <stdint.h>

typedef enum _ars_rejected_reason {
  ARS_TRACKS_OCCUPIED,
  ARS_TRACKS_LOCKED,
  ARS_WELL_CONDITION
} ARS_REJECTED_REASON;

#define UDP_BCAST_RECV_PORT_msgServerStatus 60004
#define LOOPBACK_IPADDR "127.0.0.1"

typedef struct msgServerStatus {
  uint16_t msgServerID;
  struct {
    uint8_t nReqs;
    uint16_t OC_ID;
    struct {
      uint16_t UserID;
      uint16_t WorkstationID;
    } ACR_request;
    uint8_t msgSrvCurrentRegulationMode;
  } msgSrvCurrentControlStatus;
  ;
  int n;
} MSG_SERVER_STATUS, *MSG_SERVER_STATUS_PTR;

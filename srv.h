 typedef enum _ars_rejected_reason {
  ARS_TRACKS_OCCUPIED,
  ARS_TRACKS_LOCKED,
  ARS_WELL_CONDITION
} ARS_REJECTED_REASON;

#define UDP_BCAST_RECV_PORT_msgServerStatus 60004
#define LOOPBACK_IPADDR "127.0.0.1"

typedef struct msgServerStatus {
  int n;
} MSG_SERVER_STATUS, *MSG_SERVER_STATUS_PTR;


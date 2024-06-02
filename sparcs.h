#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs_def.h"
#include "train_cmd.h"
#include "train_info.h"
#include "train_ctrl.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

#define NUM_OF_SCs 4n
typedef enum SC_ID {
  SC801,
  SC802,
  SC803,
  SC804,
  SC805,
  SC806,
  SC807,
  SC808,
  SC809,
  SC810,
  SC811,
  SC812,
  SC813,
  SC814,
  SC815,
  SC816,
  SC817,
  SC818,
  SC819,
  SC820,
  SC821,
  END_OF_SCs
} SC_ID;
#define SC_ID_CONV_2_INT( S ) ((S) + 801)

#define UDP_BCAST_RECV_PORT_SC801_Train_command 55143
#define UDP_BCAST_RECV_PORT_SC802_Train_command 55243
#define UDP_BCAST_RECV_PORT_SC803_Train_command 55343
#define UDP_BCAST_RECV_PORT_SC804_Train_command 55443
#define UDP_BCAST_RECV_PORT_SC805_Train_command 55543
#define UDP_BCAST_RECV_PORT_SC806_Train_command 55643
#define UDP_BCAST_RECV_PORT_SC807_Train_command 55743
#define UDP_BCAST_RECV_PORT_SC808_Train_command 55843
#define UDP_BCAST_RECV_PORT_SC809_Train_command 55943
#define UDP_BCAST_RECV_PORT_SC810_Train_command 56043
#define UDP_BCAST_RECV_PORT_SC811_Train_command 56143
#define UDP_BCAST_RECV_PORT_SC812_Train_command 56243
#define UDP_BCAST_RECV_PORT_SC813_Train_command 56343
#define UDP_BCAST_RECV_PORT_SC814_Train_command 56443
#define UDP_BCAST_RECV_PORT_SC815_Train_command 56543
#define UDP_BCAST_RECV_PORT_SC816_Train_command 56643
#define UDP_BCAST_RECV_PORT_SC817_Train_command 56743
#define UDP_BCAST_RECV_PORT_SC818_Train_command 56843
#define UDP_BCAST_RECV_PORT_SC819_Train_command 56943
#define UDP_BCAST_RECV_PORT_SC820_Train_command 57043
#define UDP_BCAST_RECV_PORT_SC821_Train_command 57143

struct send_buf_traincmd {
  NXNS_HEADER header;
  uint8_t flgs_1;
  uint8_t spare_1;
  uint8_t spare_2;
  uint8_t spare_3;
  TRAIN_COMMAND train_cmd;
};
typedef struct SC_ctrl_cmdset {
  char sc_name[6];
  IP_ADDR_DESC sc_ipaddr;
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_send_train_cmd;
    struct send_buf_traincmd send;
    BOOL expired[TRAIN_COMMAND_ENTRIES_NUM];
    TINY_TRAIN_STATE_PTR pTrain_stat[TRAIN_COMMAND_ENTRIES_NUM];
    int frontier;
  } train_command;
} SC_CTRL_CMDSET, *SC_CTRL_CMDSET_PTR;

#define UDP_BCAST_RECV_PORT_SC801_Train_information 55135
#define UDP_BCAST_RECV_PORT_SC802_Train_information 55235
#define UDP_BCAST_RECV_PORT_SC803_Train_information 55335
#define UDP_BCAST_RECV_PORT_SC804_Train_information 55435
#define UDP_BCAST_RECV_PORT_SC805_Train_information 55535
#define UDP_BCAST_RECV_PORT_SC806_Train_information 55635
#define UDP_BCAST_RECV_PORT_SC807_Train_information 55735
#define UDP_BCAST_RECV_PORT_SC808_Train_information 55835
#define UDP_BCAST_RECV_PORT_SC809_Train_information 55935
#define UDP_BCAST_RECV_PORT_SC810_Train_information 56035
#define UDP_BCAST_RECV_PORT_SC811_Train_information 56135
#define UDP_BCAST_RECV_PORT_SC812_Train_information 56235
#define UDP_BCAST_RECV_PORT_SC813_Train_information 56335
#define UDP_BCAST_RECV_PORT_SC814_Train_information 56435
#define UDP_BCAST_RECV_PORT_SC815_Train_information 56535
#define UDP_BCAST_RECV_PORT_SC816_Train_information 56635
#define UDP_BCAST_RECV_PORT_SC817_Train_information 56735
#define UDP_BCAST_RECV_PORT_SC818_Train_information 56835
#define UDP_BCAST_RECV_PORT_SC819_Train_information 56935
#define UDP_BCAST_RECV_PORT_SC820_Train_information 57035
#define UDP_BCAST_RECV_PORT_SC821_Train_information 57135

struct recv_buf_traininfo {
  NXNS_HEADER header;
  uint8_t flgs_1;
  uint8_t spare_1;
  uint8_t spare_2;
  uint8_t spare_3;
  TRAIN_INFO train_info;
};
typedef struct SC_stat_infoset {
  char sc_name[6];
  IP_ADDR_DESC sc_ipaddr;
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_recv_train_info;
    struct recv_buf_traininfo recv;
    TINY_TRAIN_STATE_PTR pTrain_stat[TRAIN_INFO_ENTRIES_NUM];
  } train_information;
} SC_STAT_INFOSET, *SC_STAT_INFOSET_PTR;

typedef struct block {
  BLOCK_ID id;
  char *name;
  int len;
  BOOL occupancy;
} BLOCK, *BLOCK_PTR;

extern SC_CTRL_CMDSET SC_ctrl_cmds[END_OF_SCs];
extern SC_STAT_INFOSET SC_stat_infos[END_OF_SCs];
extern STANDBY_TRAIN_CMDS standby_train_cmds;

extern int alloc_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk );
extern int standup_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk );
extern SC_CTRL_CMDSET_PTR emit_train_cmd( TINY_SOCK_PTR pS, SC_ID sc_id );
extern SC_STAT_INFOSET_PTR snif_train_info( TINY_SOCK_PTR pS, SC_ID sc_id );

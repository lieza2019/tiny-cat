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

#define NUM_OF_SCs 4
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
  END_OF_SCs
} SC_ID;
#define SC_ID_CONV_2_INT( S ) ((S) + 801)

typedef struct SC_ctrl_cmdset {
  TRAIN_COMMAND train_cmd;
} SC_CTRL_CMDSET, *SC_CTRL_CMDSET_PTR;

#define UDP_BCAST_RECV_PORT_SC801_Train_information 63001
#define UDP_BCAST_RECV_PORT_SC802_Train_information 63002
#define UDP_BCAST_RECV_PORT_SC803_Train_information 63003
#define UDP_BCAST_RECV_PORT_SC804_Train_information 63004
#define UDP_BCAST_RECV_PORT_SC805_Train_information 63005
#define UDP_BCAST_RECV_PORT_SC806_Train_information 63006
#define UDP_BCAST_RECV_PORT_SC807_Train_information 63007
#define UDP_BCAST_RECV_PORT_SC808_Train_information 63008
#define UDP_BCAST_RECV_PORT_SC809_Train_information 63009

typedef struct SC_stat_infoset {
  struct {
    unsigned char oct_1st;
    unsigned char oct_2nd;
    unsigned char oct_3rd;
    unsigned char oct_4th;
  } sc_ipaddr;
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_recv_train_info;
    TRAIN_INFO train_info;
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

extern int alloc_train_cmd_entries( TRAIN_COMMAND_ENTRY_PTR es[], int rakeID, int front_blk, int back_blk );
extern SC_STAT_INFOSET_PTR sniff_train_info( SC_ID sc_id );

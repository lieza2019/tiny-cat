#ifndef SPARCS_H_INCLUDED
#define SPARCS_H_INCLUDED

#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs_def.h"
#include "train_cmd.h"
#include "train_info.h"

#include "train_ctrl.h"

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
  union {
    struct {
      //uint8_t flgs_1;
      //uint8_t spare_1;
      //uint8_t spare_2;
      //uint8_t spare_3;
      TRAIN_COMMAND spol;
    } train_cmd;
  } u;
};
typedef struct sc_ctrlcmd_comm_prof {
  SC_ID dest_sc_id;
  const unsigned short dst_port;
  TINY_SOCK_DESC d_send_train_cmd;
  IP_ADDR_DESC dst_ipaddr;
  struct {
    time_t emission_start;
    uint32_t seq;
  } nx;
  struct send_buf_traincmd send;
} SC_CTRLCMD_COMM_PROF, *SC_CTRLCMD_COMM_PROF_PTR;
typedef struct sc_ctrl_cmdprof {
  SC_CTRLCMD_COMM_PROF comm_prof;
  struct {
    union {
      struct {
	int frontier;
	BOOL expired[TRAIN_COMMAND_ENTRIES_NUM];
	TINY_TRAIN_STATE_PTR pTrain_stat[TRAIN_COMMAND_ENTRIES_NUM];
      } train_cmd;
    } u;
  } attribs;
} SC_CTRL_CMDPROF, *SC_CTRL_CMDPROF_PTR;

typedef struct SC_ctrl_cmdset {
  char sc_name[6];
  IP_ADDR_DESC sc_ipaddr;
  SC_CTRL_CMDPROF train_command;
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

extern SC_CTRL_CMDSET SC_ctrl_cmds[END_OF_SCs];
extern SC_STAT_INFOSET SC_stat_infos[END_OF_SCs];
extern STANDBY_TRAIN_CMDS standby_train_cmds;

extern SC_CTRL_CMDSET_PTR which_SC_from_train_cmd ( TRAIN_COMMAND_ENTRY_PTR pTc );
extern SC_STAT_INFOSET_PTR which_SC_from_train_info ( TRAIN_INFO_ENTRY_PTR pTi );
extern int alloc_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk );
extern int standup_train_cmd_entries ( TRAIN_COMMAND_ENTRY_PTR es[], TINY_TRAIN_STATE_PTR pTs, int rakeID, int front_blk, int back_blk );
extern SC_CTRL_CMDSET_PTR emit_train_cmd( TINY_SOCK_PTR pS, SC_ID sc_id );
extern SC_STAT_INFOSET_PTR snif_train_info( TINY_SOCK_PTR pS, SC_ID sc_id );

extern int establish_SC_comm_cmds ( TINY_SOCK_PTR pS, SC_CTRLCMD_COMM_PROF_PTR *pprofs[], const int nprofs, const int ndsts );

extern void diag_train_info ( FILE *fp_out, const TRAIN_INFO_ENTRY_PTR pE );
#endif //SPARCS_H_INCLUDED

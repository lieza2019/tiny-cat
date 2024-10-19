#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "generic.h"
#include "misc.h"
#include "network.h"
#include "sparcs.h"
#include "cbi.h"
#include "interlock.h"
#include "srv.h"

#if 1
static int diag_tracking_train_cmd ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
      int j = 0;
      do {
	TRAIN_COMMAND_ENTRY_PTR pE = trains_tracking[i].pTC[j];
	assert( j < 1 ? pE : (TRAIN_COMMAND_ENTRY_PTR)TRUE );
	int rakeID = -1;
	rakeID = (int)ntohs( pE->rakeID );
	assert( rakeID > 0 );
	assert( rakeID == trains_tracking[i].rakeID );
	
	fprintf( fp_out, "%s;\n", (which_SC_from_train_cmd(pE))->sc_name );
	fprintf( fp_out, "rakeID: %-3d\n", rakeID );
	j++;
      } while( (j < 2) && trains_tracking[i].pTC[j] );
      fprintf( fp_out, "\n" );
      r++;
    }
  return r;
}

#define NOTICE_OF_TIME_NO_TIME_UPDATE 0x00
#define NOTICE_OF_TIME_VRS_TIME_UPDATE 0x01
#define NOTICE_OF_TIME_DMI_TIME_UPDATE 0x02
#define NOTICE_OF_TIME_TCMS_TIME_UPDATE 0x04
static void diag_train_stat ( FILE *fp_out, const TRAIN_INFO_ENTRY_PTR pE ) {
  assert( fp_out );
  assert( pE );
  
  fprintf( fp_out, "rakeID: %03d\n", TRAIN_INFO_RAKEID(*pE) );
  fprintf( fp_out, "TRR: %d\n", TRAIN_INFO_TRR(*pE) );
  fprintf( fp_out, "Acknowledge of energy saving mode: %d\n", TRAIN_INFO_ACKNOWLEDGE_OF_ENERGY_SAVING_MODE(*pE) );
  fprintf( fp_out, "ATB OK: %d\n", TRAIN_INFO_ATB_OK(*pE) );
  fprintf( fp_out, "Push of departure button: %d\n", TRAIN_INFO_PUSH_OF_DEPARTURE_BUTTON(*pE) );
  fprintf( fp_out, "Skip Next Stop: %d\n", TRAIN_INFO_SKIP_NEXT_STOP(*pE) );
  fprintf( fp_out, "Notice of time: " );
  {
    const unsigned char v = TRAIN_INFO_NOTICE_OF_TIME(*pE);
    BOOL cont = FALSE;
    if( v & NOTICE_OF_TIME_VRS_TIME_UPDATE ) {
      fprintf( fp_out, "VRS_time_update" );
      cont = TRUE;
    }
    if( v & NOTICE_OF_TIME_DMI_TIME_UPDATE ) {
      if( cont )
	fprintf( fp_out, ", " );
      fprintf( fp_out, "DMI_time_update" );
      cont = TRUE;
    }
    if( v & NOTICE_OF_TIME_TCMS_TIME_UPDATE ) {
      if( cont )
	fprintf( fp_out, ", " );
      fprintf( fp_out, "TCMS_time_update" );
      cont = TRUE;
    }
    if( !cont )
      fprintf( fp_out, "NO_time_update" );
    fprintf( fp_out, "\n" );
  }
  fprintf( fp_out, "OCC Command ID Ack: %d\n", (int)TRAIN_INFO_OCC_COMMANDID_ACK(*pE) );
  fprintf( fp_out, "Stop detection: %d\n", TRAIN_INFO_STOP_DETECTION(*pE) );
  fprintf( fp_out, "Dynamic testable section: %d\n", TRAIN_INFO_DYNAMIC_TESTABLE_SECTION(*pE) );
  fprintf( fp_out, "Voltage reduction: %d\n", TRAIN_INFO_VOLTAGE_REDUCTION(*pE) );
  fprintf( fp_out, "VRS(F) reset: %d\n", TRAIN_INFO_VRS_F_RESET(*pE) );
  fprintf( fp_out, "VRS ID(forward): %d\n", TRAIN_INFO_VRS_ID_FORWARD(*pE) );
  fprintf( fp_out, "Passing through balise: %d\n", TRAIN_INFO_PASSING_THROUGH_BALISE(*pE) );
  fprintf( fp_out, "overspeed: %d\n", TRAIN_INFO_OVERSPEED(*pE) );
  fprintf( fp_out, "Door Enable: %d\n", TRAIN_INFO_DOOR_ENABLE(*pE) );
  fprintf( fp_out, "VRS(B) reset: %d\n", TRAIN_INFO_VRS_B_RESET(*pE) );
  fprintf( fp_out, "VRS ID(back): %d\n", TRAIN_INFO_VRS_ID_BACK(*pE) );
  fprintf( fp_out, "Forward train position: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS(*pE) );
  fprintf( fp_out, "Forward train position offset: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET(*pE) );
  fprintf( fp_out, "Back train position: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS(*pE) );
  fprintf( fp_out, "Back train position offset: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_OFFSET(*pE) );
  fprintf( fp_out, "Occupied Block(forward): %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_FORWARD(*pE) );
  fprintf( fp_out, "Occupied Block(forward) offset: %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_FORWARD_OFFSET(*pE) );
  fprintf( fp_out, "Occupied Block(back): %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_BACK(*pE) );
  fprintf( fp_out, "Occupied Block(back) offset: %d\n", (int)TRAIN_INFO_OCCUPIED_BLK_BACK_OFFSET(*pE) );
  fprintf( fp_out, "Forward train position segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train position offset segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Back train position segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Back train position offset segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train occupied position segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_SEGMENT(*pE) );
  fprintf( fp_out, "Forward train occupied position offset segment: %d\n", (int)TRAIN_INFO_FORWARD_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Back train occupied segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_OCCUPIED_SEGMENT(*pE) );
  fprintf( fp_out, "Back train occupied position offset segment: %d\n", (int)TRAIN_INFO_BACK_TRAIN_OCCUPIED_POS_OFFSET_SEGMENT(*pE) );
  fprintf( fp_out, "Train speed: %d\n", (int)TRAIN_INFO_TRAIN_SPEED(*pE) );
  fprintf( fp_out, "Train max speed: %d\n", (int)TRAIN_INFO_TRAIN_MAXSPEED(*pE) );
}
static int show_tracking_train_stat ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ ) {
    if( trains_tracking[i].rakeID > 0 ){
      if( ! trains_tracking[i].omit ) {
	TRAIN_INFO_ENTRY_PTR pTI = NULL;
	TRAIN_INFO_ENTRY TI;
	memset( &TI, 0, sizeof(TRAIN_INFO_ENTRY) );
	pTI = (TRAIN_INFO_ENTRY_PTR)conslt_cbtc_state( &trains_tracking[i], CBTC_TRAIN_INFORMATION, NULL, (void *)&TI, sizeof(TRAIN_INFO_ENTRY) );
	if( pTI ) {
	  assert( pTI == &TI );
	  assert( trains_tracking[i].rakeID != (int)TRAIN_INFO_RAKEID(TI) );
	  fprintf( fp_out, "%s:\n", (which_SC_from_train_info(trains_tracking[i].pTI))->sc_name );
	  diag_train_stat( fp_out, &TI );
	  fprintf( fp_out, "\n" );
	  r++;
	}
      } else
	fprintf( fp_out, "rakeID:%03d, has no Train information now.\n", trains_tracking[i].rakeID );
    }
  }
  return r;
}

static void creat_comm_threads ( TINY_COMM_PROF_PTR pcomm_threads_prof ) {
  assert( pcomm_threads_prof );
  pthread_t P_cbtc_stat;
  pthread_t P_cbtc_ctrl;
  pthread_t P_il_stat;
  pthread_t P_il_ctrl_emit;
  pthread_t P_il_ctrl_kron;
  
  pthread_mutex_init( &cbtc_stat_infos_mutex, NULL );
  pthread_mutex_init( &cbtc_ctrl_cmds_mutex, NULL );
  pthread_mutex_init( &cbi_ctrl_sendbuf_mutex, NULL );    
  pthread_mutex_init( &cbi_ctrl_dispatch_mutex, NULL );
  
  if( pthread_create( &P_cbtc_stat, NULL, pth_reveal_cbtc_status, (void *)pcomm_threads_prof ) ) {
    errorF( "%s", "failed to invoke the CBTC status gathering thread.\n" );
    exit( 1 );
  }
  if( pthread_create( &P_cbtc_ctrl, NULL, pth_emit_cbtc_ctrl_cmds, (void *)&pcomm_threads_prof->cbtc.cmd.socks ) ) {
    errorF( "%s", "failed to invoke the CBTC control commands emitter thread.\n" );
    exit( 1 );
  }
  
  if( pthread_create( &P_il_ctrl_emit, NULL, pth_revise_il_ctrl_bits, (void *)&pcomm_threads_prof->cbi.ctrl.socks ) ) {
    errorF( "%s", "failed to invoke the CBI control emission thread.\n" );
    exit( 1 );
  }
  pthread_mutex_init( &cbi_stat_info_mutex, NULL );
  if( pthread_create( &P_il_stat, NULL, pth_reveal_il_status, (void *)&pcomm_threads_prof->cbi.stat.socks ) ) {
    errorF( "%s", "failed to invoke the CBI status gathering thread.\n" );
    exit( 1 );
  }
  if( pthread_create( &P_il_ctrl_kron, NULL, pth_expire_il_ctrl_bits, NULL ) ) {
    errorF( "%s", "failed to invoke the CBI control elimination thread.\n" );
    exit( 1 );
  }
}

static void _establish_SC_comm ( TINY_COMM_PROF_PTR pcomm_prof ) {
  assert( pcomm_prof );
  TINY_SOCK_DESC *pdescs[END_OF_CBTC_CMDS_INFOS] = {};
  {
    int i;
    pdescs[CBTC_TRAIN_COMMAND] = pcomm_prof->cbtc.cmd.train_cmd.descs;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) { pcomm_prof->cbtc.cmd.train_cmd.descs[i] = -1; }
    
    pdescs[CBTC_TRAIN_INFORMATION] = pcomm_prof->cbtc.info.train_info.descs;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) { pcomm_prof->cbtc.info.train_info.descs[i] = -1; }
  }
  
  TINY_SOCK_CREAT( pcomm_prof->cbtc.info.socks );
  if( establish_SC_comm_infos( &pcomm_prof->cbtc.info.socks, pdescs, (int)END_OF_CBTC_CMDS_INFOS, (int)END_OF_SCs ) < 0 ) {
    errorF("%s", "failed to create the recv UDP ports for CBTC/SC status informations.\n" );
    exit( 1 );
  }
#ifdef CHK_STRICT_CONSISTENCY
  {
    int i;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) {
      assert( pcomm_prof->cbtc.info.train_info.descs[i] > -1 );
    }
  }  
#endif // CHK_STRICT_CONSISTENCY
  
  TINY_SOCK_CREAT( pcomm_prof->cbtc.cmd.socks );
  if( establish_SC_comm_cmds( &pcomm_prof->cbtc.cmd.socks, pdescs, (int)END_OF_CBTC_CMDS_INFOS, (int)END_OF_SCs ) < 0 ) {
    errorF("%s", "failed to create the send UDP ports for CBTC/SC control commands.\n");
    exit( 1 );
  }
#ifdef CHK_STRICT_CONSISTENCY
  {
    int i;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) {
      assert( pcomm_prof->cbtc.cmd.train_cmd.descs[i] > -1 );
    }
  }
#endif // CHK_STRICT_CONSISTENCY
}

static void load_il_status_geometry ( void ) {
  int cnt = 0;
  int oc_id = (int)OC801;
  while( oc_id < (int)END_OF_OCs ) {
    if( ! il_status_geometry_resources[oc_id].csv_fname ) {
      oc_id++;
      continue;
    }
    assert( il_status_geometry_resources[oc_id].csv_fname );
    assert( il_status_geometry_resources[oc_id].oc_id == oc_id );
    {
      int n = -1;
      char fname[512];
      fname[511] = 0;
#ifndef IN_CBI_RESOURCEDIR
      assert( (int)sizeof(fname) > (strlen("./cbi/") + strlen(il_status_geometry_resources[oc_id].csv_fname)) );
      strcat( strcpy(fname, "./cbi/"), il_status_geometry_resources[oc_id].csv_fname );
#else
      strcpy( fname, il_status_geometry_resources[oc_id].csv_fname );
#endif // IN_CBI_RESOURCEDRI
      n = load_cbi_code_tbl ( il_status_geometry_resources[oc_id].oc_id, fname );
      if( n < 0 ) {
	errorF( "failed to open the CBI status csv file of %s.\n", fname );
	exit( 1 );
      }
      errorF( "read %d entries from raw csv file of %s.\n", n, il_status_geometry_resources[oc_id].csv_fname );
      cnt += n;
    }
    oc_id++;
  }
  printf( "read %d entries on, from raw csv files.\n", cnt );
  
  {
    int m = -1;
    m = revise_cbi_code_tbl( NULL );
    assert( m > -1 );
    errorF( "revised %d entries of CSV status.\n", m );
  }
}

static void establish_OC_comm ( TINY_COMM_PROF_PTR pcomm_prof ) {
  assert( pcomm_prof );
  int nsocks_ctrl = -1;
  int nsocks_stat = -1;
  
  int i;
  for( i = 0; i < END_OF_ATS2OC; i++ )
    pcomm_prof->cbi.ctrl.descs[i] = -1;
  TINY_SOCK_CREAT( pcomm_prof->cbi.ctrl.socks );
  nsocks_ctrl = establish_OC_comm_ctrl( &pcomm_prof->cbi.ctrl.socks, pcomm_prof->cbi.ctrl.descs, (int)END_OF_ATS2OC );
  if( nsocks_ctrl > 0 ) {
    assert( nsocks_ctrl == END_OF_ATS2OC );
    int j;
#ifdef CHK_STRICT_CONSISTENCY
    for( j = 0; j < nsocks_ctrl; j++ )
      assert( pcomm_prof->cbi.ctrl.descs[j] > -1 );
#endif // CHK_STRICT_CONSISTENCY
    for( j = 0; j < END_OF_OC2ATS; j++ )
      pcomm_prof->cbi.stat.descs[j] = -1;
    TINY_SOCK_CREAT( pcomm_prof->cbi.stat.socks );
    nsocks_stat = establish_OC_comm_stat( &pcomm_prof->cbi.stat.socks, pcomm_prof->cbi.stat.descs, (int)END_OF_OC2ATS );
    if( nsocks_stat > 0 ) {
      assert( nsocks_stat == END_OF_OC2ATS );
#ifdef CHK_STRICT_CONSISTENCY
      int k;
      for( k = 0; k < nsocks_stat; k++ )
	assert( pcomm_prof->cbi.stat.descs[k] > -1 );
#endif // CHK_STRICT_CONSISTENCY
      load_il_status_geometry();
    } else
      goto err_OC_sockets;
  } else {
  err_OC_sockets:
    errorF("%s", "failed to create the send/recv UDP ports for CBI control & state information, respectively.\n");
    exit( 1 );
  }
}

#define BUFSIZ_msgServerStatus MAX_SEND_BUFSIZ
#define BUFSIZ_msgServerHeartbeat MAX_SEND_BUFSIZ
static unsigned char buf_msgServerStatus[BUFSIZ_msgServerStatus];
static unsigned char buf_msgServerHeartbeat[BUFSIZ_msgServerStatus];
BOOL launch_msg_srv_stat ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pd_beat, TINY_SOCK_DESC *pd_stat ) {
  assert( pS );
  assert( pd_beat );
  assert( pd_stat );
  IP_ADDR_DESC srvstat_dstip = { 0, 0, 0, 0 };  
  BOOL r = FALSE;
  
  srvstat_dstip.oct_1st = BROADCAST_DSTIP_1stO;
  srvstat_dstip.oct_2nd = BROADCAST_DSTIP_2ndO;
  srvstat_dstip.oct_3rd = BROADCAST_DSTIP_3rdO;
  srvstat_dstip.oct_4th = BROADCAST_DSTIP_4thO;
  if( (*pd_beat = creat_sock_send( pS, UDP_BCAST_SEND_PORT_MsgServerHeartbeat, TRUE, &srvstat_dstip )) >= 0 ) {
    sock_attach_send_buf( pS, *pd_beat, buf_msgServerHeartbeat, sizeof(buf_msgServerHeartbeat) );
    if( (*pd_stat = creat_sock_send( pS, UDP_BCAST_SEND_PORT_msgServerStatus, TRUE, &srvstat_dstip )) >= 0 ) {
      sock_attach_send_buf( pS, *pd_stat, buf_msgServerStatus, sizeof(buf_msgServerStatus) );
      r = TRUE;
    }
  }
  return r;
}

int main ( void ) {
  TINY_SOCK socks_srvstat;
  TINY_SOCK_DESC sd_send_srvbeat = -1;
  TINY_SOCK_DESC sd_send_srvstat = -1;
  TINY_SOCK_DESC sd_recv_srvstat = -1;
  
  TINY_COMM_PROF comm_threads_prof;
  memset( &comm_threads_prof, 0, sizeof(comm_threads_prof) );
  
  tzset();
  cons_il_obj_tables();

  TINY_SOCK_CREAT( socks_srvstat );  
#if 0
  if( ! establish_SC_comm( &socks_srvstat ) ) {
    errorF("%s", "failed to create the recv/send UDP ports for Train information and Train command respectively.\n");
    exit( 1 );
  }
#else
  _establish_SC_comm( &comm_threads_prof );
#endif
  establish_OC_comm( &comm_threads_prof );
  
#if 0
  printf( "sizeof TRAIN_INFO_ENTRY: %d.\n", (int)sizeof(TRAIN_INFO_ENTRY) );
  printf( "sizeof TRAIN_INFO: %d.\n", (int)sizeof(TRAIN_INFO) );
  {
    struct _recv_buf_train_info {
      NXNS_HEADER header;
      uint8_t flgs_1;
      uint8_t spare_1;
      uint8_t spare_2;
      uint8_t spare_3;
      TRAIN_INFO train_info;
    };
    printf( "sizeof RECV_BUF_TRAININFO: %d.\n", (int)sizeof(struct _recv_buf_train_info) );
  }
  printf( "sizeof MSG_TINY_SERVER_STATUS: %d.\n", (int)sizeof(MSG_TINY_SERVER_STATUS) );
  {
    struct _tny_srv_stat_NXNS {
      NXNS_HEADER hdr;
      MSG_TINY_SERVER_STATUS load;
    };
    printf( "sizeof TinyMsgServerStatus on NX-NS header: %d.\n", (int)sizeof(struct _tny_srv_stat_NXNS) );
  }
  exit( 0 );
#endif
  
  if( ! launch_msg_srv_stat( &socks_srvstat, &sd_send_srvbeat, &sd_send_srvstat ) ) {
    errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  }
  assert( sd_send_srvstat > -1 );
  assert( sd_send_srvbeat > -1 );
  if( (sd_recv_srvstat = creat_sock_recv( &socks_srvstat, UDP_BCAST_SEND_PORT_msgServerStatus )) < 0 ) {
    errorF( "%s", "failed to create the socket to self-receive msgServerStatus.\n" );
    exit( 1 );
  } else {
    assert( sd_recv_srvstat > -1 );
    sock_attach_recv_buf( &socks_srvstat, sd_recv_srvstat, buf_msgServerStatus, sizeof(buf_msgServerStatus) );
  }
  
  {
    const useconds_t interval = 1000 * 1000 * 0.1;
    int cnt = 0;
    
    static MSG_TINY_HEARTBEAT msg_srv_beat;
    static MSG_TINY_SERVER_STATUS msg_srv_stat;
    TINY_SRVBEAT_HEARTBEAT_SERVERID( msg_srv_beat, 1 );
    
    TINY_SRVSTAT_MSG_SERVERID( msg_srv_stat, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_JPW,  1, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_IGDA, 2, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_RKPM, 3, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_IWNR, 4, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_JLA,  5, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_BCGN, 6, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_KIKD, 7, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_RKAM, 8, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_MKPR, 9, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_NPPR, 10, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_DPCK, 11, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_PAGI, 12, 1 );
    TINY_SRVSTAT_CURRENT_ACR( msg_srv_stat, ACR_MKPD, 13, 1 );
    TINY_SRVSTAT_REGURATION_MODE( msg_srv_stat, 1 );
    TINY_SRVSTAT_MSG_COMM_PA( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_TRAINRADIO( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_TVS( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_SCADA( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_LOGGER1( msg_srv_stat, TRUE );
    TINY_SRVSTAT_MSG_COMM_LOGGER2( msg_srv_stat, TRUE );
    
    creat_comm_threads( &comm_threads_prof );
    while( TRUE ) {
      errorF( "%s", "waken up!\n" );    
#if 1
      show_tracking_train_stat( stdout );
#endif
#if 0
      diag_cbi_stat_attrib( stdout, "S803A_S811A" );
      {
	OC_ID oc_id = END_OF_OCs;
	CBI_STAT_KIND kind = END_OF_CBI_STAT_KIND;
	const char ctl_bit_ident[] = "P_S821A_S801A";
	engage_il_ctrl( &oc_id, &kind, ctl_bit_ident );
	//errorF( "(oc_id): (%d)\n", OC_ID_CONV2INT(oc_id) ); // ***** for debugging.
      }
#endif
      ready_on_emit_OC_ctrl( &comm_threads_prof.cbi.ctrl.socks, comm_threads_prof.cbi.ctrl.descs, END_OF_ATS2OC );
      reveal_train_tracking( &comm_threads_prof );
      purge_block_restrains();
      
      {
	unsigned char *pmsg_buf = NULL;
	int msglen = -1;
	pmsg_buf = sock_recv_buf_attached( &socks_srvstat, sd_recv_srvstat, &msglen );
	assert( pmsg_buf == buf_msgServerStatus );
	assert( (msglen > 0) ? (msglen == sizeof(MSG_TINY_SERVER_STATUS)) : TRUE );
      }
      {
	unsigned char *pmsg_buf = NULL;
	int size = -1;
	pmsg_buf = sock_send_buf_attached( &socks_srvstat, sd_send_srvbeat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_HEARTBEAT) );
	memcpy( pmsg_buf, &msg_srv_beat, sizeof(msg_srv_beat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks_srvstat, sd_send_srvbeat, sizeof(msg_srv_beat) );
	  assert( n == sizeof(MSG_TINY_HEARTBEAT) );
	}
	
	pmsg_buf = NULL;
	size = -1;
	pmsg_buf = sock_send_buf_attached( &socks_srvstat, sd_send_srvstat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_SERVER_STATUS) );
	msg_srv_stat.n = cnt;
	memcpy( pmsg_buf, &msg_srv_stat, sizeof(msg_srv_stat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks_srvstat, sd_send_srvstat, sizeof(msg_srv_stat) );
	  assert( n == sizeof(MSG_TINY_SERVER_STATUS) );
	}
      }
      
#if 0
      {
	int i;
	for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ ) {
	  if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
	    TINY_TRAIN_STATE_PTR pT = &trains_tracking[i];
	    assert( pT );
	    if( pT->rakeID == 9 ) {
	      change_train_state_skip_next_stop( pT, TRUE, FALSE );
	    }
	  }
	}
      }
#endif
      load_train_command();
#ifdef CHK_STRICT_CONSISTENCY
      chk_solid_train_cmds();
#endif // CHK_STRICT_CONSISTENCY
      if( diag_tracking_train_cmd( stdout ) > 0 )
	fprintf( stdout, "\n" );
#if 0
      if( sock_send0(&socks_srvstat, NULL ) < 1 ) {
	errorF( "%s", "failed to send msgServerStatus.\n" );
	exit( 1 );
      }
#endif
      cnt++;
      {
	int r = -1;
	r = usleep( interval );
	assert( !r );
      }
    }
  }
  return 0;
}
#endif

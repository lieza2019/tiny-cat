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

static int diag_tracking_train_stat ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  
  int i;
  for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ )
    if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
      TRAIN_INFO_ENTRY_PTR pE = trains_tracking[i].pTI;
      assert( pE );
      fprintf( fp_out, "%s;\n", (which_SC_from_train_info(pE))->sc_name );
      
      assert( trains_tracking[i].rakeID == (int)TRAIN_INFO_RAKEID(*trains_tracking[i].pTI) );
      fprintf( fp_out, "rakeID: %-3d\n", trains_tracking[i].rakeID );
      
      fprintf( fp_out, "\n" );
      r++;
    }
  return r;
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

static void load_il_status_geometry ( void ) {
  int oc_id = -1;
  oc_id = (int)OC801;
  while( oc_id < (int)END_OF_OCs ) {
    assert( (oc_id >= OC801) && (oc_id < END_OF_OCs) );
    if( ! il_status_geometry_resources[oc_id].csv_fname ) {
      oc_id++;
      continue;
    }
    assert( il_status_geometry_resources[oc_id].csv_fname );
    assert( il_status_geometry_resources[oc_id].oc_id == oc_id );
    {
      int n = -1;
      char fname[256];
      assert( (int)sizeof(fname) > strlen(il_status_geometry_resources[oc_id].csv_fname) );
#ifndef IN_CBI_RESOURCEDIR
      assert( (strlen("./cbi/") + (int)sizeof(fname)) > strlen(il_status_geometry_resources[oc_id].csv_fname) );
      strcat( strcpy(fname, "./cbi/"), il_status_geometry_resources[oc_id].csv_fname );      
#else
      strcpy( fname, il_status_geometry_resources[oc_id].csv_fname );
#endif // IN_CBI_RESOURCEDRI
      n = load_cbi_code_tbl ( il_status_geometry_resources[oc_id].oc_id, fname );
      if( n < 0 ) {
	errorF( "failed to open the CBI status csv file of %s.\n", fname );
	exit( 1 );
      } else {
	int m = -1;
	errorF( "read %d entries from raw csv file of %s.\n", n, il_status_geometry_resources[oc_id].csv_fname );
	m = revise_cbi_code_tbl( NULL );
	assert( m > -1 );
	errorF( "revised %d entries over the ones from the file of %s.\n", m, il_status_geometry_resources[oc_id].csv_fname );
      }
    }
    oc_id++;
  }
}

int main ( void ) {
  TINY_SOCK socks_less_1;
  TINY_SOCK socks_cbi_stat;
  TINY_SOCK_DESC sd_send_srvbeat = -1;
  TINY_SOCK_DESC sd_send_srvstat = -1;
  TINY_SOCK_DESC sd_recv_srvstat = -1;
  
  tzset();
  cons_il_obj_tables();
  
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
  
  TINY_SOCK_CREAT( socks_less_1 );  
  if( ! launch_msg_srv_stat( &socks_less_1, &sd_send_srvbeat, &sd_send_srvstat ) ) {
    errorF( "%s", "failed to create the socket to send msgServerStatus.\n" );
    exit( 1 );
  }
  assert( sd_send_srvstat > -1 );
  assert( sd_send_srvbeat > -1 );
  
  if( (sd_recv_srvstat = creat_sock_recv( &socks_less_1, UDP_BCAST_SEND_PORT_msgServerStatus )) < 0 ) {
    errorF( "%s", "failed to create the socket to self-receive msgServerStatus.\n" );
    exit( 1 );
  } else {
    assert( sd_recv_srvstat > -1 );
    sock_attach_recv_buf( &socks_less_1, sd_recv_srvstat, buf_msgServerStatus, sizeof(buf_msgServerStatus) );
  }
  
  if( ! establish_SC_comm( &socks_less_1 ) ) {
    errorF("%s", "failed to create the recv/send UDP ports for Train information and Train command respectively.\n");
    exit( 1 );
  }
  
  if( establish_OC_ctrl_send( &socks_less_1 ) ) {
    TINY_SOCK_CREAT( socks_cbi_stat );
    if( establish_OC_stat_recv( &socks_cbi_stat ) )
      load_il_status_geometry();
    else
      goto err_OC_sockets;
  } else {
  err_OC_sockets:
      errorF("%s", "failed to create the recv/send UDP ports for CBI state information and control command respectively.\n");
      exit( 1 );
  }
  
  {
    const useconds_t interval = 1000 * 1000 * 0.1;
    int nrecv = -1;
    int cnt = 0;
    pthread_t P_il_stat;
    pthread_t P_il_ctrl_dispat, P_il_ctrl_chrono;
    
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
#if 0
    pthread_mutex_init( &cbi_ctrl_sendbuf_mutex, NULL );    
    pthread_mutex_init( &cbi_ctrl_dispatch_mutex, NULL );
    if( pthread_create( &P_il_ctrl_dispat, NULL, pth_reveal_il_ctrl_bits, NULL ) ) {
      errorF( "%s", "failed to invoke the CBI control emission thread.\n" );
      exit( 1 );
    }
    if( pthread_create( &P_il_ctrl_chrono, NULL, pth_expire_il_ctrl_bits, NULL ) ) {
      errorF( "%s", "failed to invoke the CBI control elimination thread.\n" );
      exit( 1 );
    }
#endif
    pthread_mutex_init( &cbi_stat_info_mutex, NULL );
    if( pthread_create( &P_il_stat, NULL, pth_reveal_il_status, (void *)&socks_cbi_stat ) ) {
      errorF( "%s", "failed to invoke the CBI status gathering thread.\n" );
      exit( 1 );
    }
    
    while( TRUE ) {
      errorF( "%s", "waken up!\n" );
      if( (nrecv = sock_recv( &socks_less_1 )) < 0 ) {
	errorF( "%s", "error on receiving CBTC/CBI status information from SC/OCs.\n" );
	continue;
      }
      
      diag_cbi_stat_attrib( stdout, "S821B_S801B" );
      
      reveal_train_tracking( &socks_less_1 );
      purge_block_restrains();
#if 0
      if( diag_tracking_train_stat( stdout ) > 0 )
	fprintf( stdout, "\n" );
#endif
      
      {
	unsigned char *pmsg_buf = NULL;
	int msglen = -1;
	pmsg_buf = sock_recv_buf_attached( &socks_less_1, sd_recv_srvstat, &msglen );
	assert( pmsg_buf );
	assert( pmsg_buf == buf_msgServerStatus );
	assert( (msglen > 0) ? (msglen == sizeof(MSG_TINY_SERVER_STATUS)) : TRUE );
      }
      
      {
	unsigned char *pmsg_buf = NULL;
	int size = -1;
	pmsg_buf = sock_send_buf_attached( &socks_less_1, sd_send_srvbeat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_HEARTBEAT) );
	memcpy( pmsg_buf, &msg_srv_beat, sizeof(msg_srv_beat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks_less_1, sd_send_srvbeat, sizeof(msg_srv_beat) );
	  assert( n == sizeof(MSG_TINY_HEARTBEAT) );
	}
	
	pmsg_buf = NULL;
	size = -1;
	pmsg_buf = sock_send_buf_attached( &socks_less_1, sd_send_srvstat, &size );
	assert( pmsg_buf );
	assert( size >= sizeof(MSG_TINY_SERVER_STATUS) );
	msg_srv_stat.n = cnt;
	memcpy( pmsg_buf, &msg_srv_stat, sizeof(msg_srv_stat) );
	{
	  int n = -1;
	  n = sock_send_ready( &socks_less_1, sd_send_srvstat, sizeof(msg_srv_stat) );
	  assert( n == sizeof(MSG_TINY_SERVER_STATUS) );
	}
      }
      
      load_train_command();
#ifdef CHK_STRICT_CONSISTENCY
      chk_solid_train_cmds();
#endif // CHK_STRICT_CONSISTENCY
      if( diag_tracking_train_cmd( stdout ) > 0 )
	fprintf( stdout, "\n" );
#if 0
      if( sock_send(&socks) < 1 ) {
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
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
#include "surveill.h"
#include "timetable.h"
#include "./timetable/ttcreat.h"
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

#if 0
static void show_train_residents_consistency ( FILE *fp_out, TINY_TRAIN_STATE_PTR pT ) {
  assert( fp_out );
  assert( pT );
  CBTC_BLOCK_C_PTR pblk_forw = pT->misc.occupancy.pblk_forward;
  CBTC_BLOCK_C_PTR pblk_back = pT->misc.occupancy.pblk_back;
  
  if( pblk_forw ) {
    TINY_TRAIN_STATE_C_PTR p = NULL;
    p = read_residents_CBTC_BLOCK( pblk_forw );
    if( pblk_back ) {
      TINY_TRAIN_STATE_C_PTR e = NULL;
      if( pblk_forw == pblk_back ) {
	int i;
	for( i = 0; i < MAX_OPAQUE_BLKS; i++ )
	  assert( ! pT->misc.occupancy.opaque_blks[i] );
	{
	  int j;
	  for( j = 0; j < MAX_ADJACENT_BLKS; j++ ) {
	    TINY_TRAIN_STATE_C_PTR q = NULL;
	    q = read_edge_of_residents_CBTC_BLOCK1( pblk_forw, j );
	    assert( q != pT );
	  }
	}
      } else {
	int found = -1;
	int i;
	for( i = 0; i < MAX_ADJACENT_BLKS; i++ ) {
	  TINY_TRAIN_STATE_C_PTR q = NULL;
	  q = read_edge_of_residents_CBTC_BLOCK1( pblk_forw, i );
	  if( q == pT ) {
	    assert( found < 0 );
	    found = i;
	  }
	}
	assert( found > -1 );
	e = read_edge_of_residents_CBTC_BLOCK1( pblk_forw, found );
	assert( e == pT );
	{
	  int j;
	  found = -1;
	  for( j = 0; j < MAX_ADJACENT_BLKS; j++ ) {
	    TINY_TRAIN_STATE_C_PTR r = NULL;
	    r = read_edge_of_residents_CBTC_BLOCK1( pblk_back, j );
	    if( r == pT ) {
	      found = TRUE;
	      break;
	    }
	  }
	  assert( found > -1 );
	}
      }
      if( e ) {
	assert( pT->misc.occupancy.pblk_forward != pT->misc.occupancy.pblk_back );
	while( p ) {
	  if( p == pT )
	    break;
	  else
	    p = p->misc.occupancy.pNext;
	}
	assert( !p );
	p = e;
	{
	  TINY_TRAIN_STATE_C_PTR q = NULL;
	  q = read_residents_CBTC_BLOCK( pblk_back );
	  while( q ) {
	    if( q == pT )
	      break;
	    else
	      q = q->misc.occupancy.pNext;
	  }
	  assert( !q );
	}
      } else {
      lk4_forward:
	assert( p );
	do {
	  if( p == pT )
	    break;
	  else
	    p = p->misc.occupancy.pNext;
	} while( p );
	assert( p );
      }
      assert( p == pT );
      fprintf( fp_out, "***** Occupied Block (forward): %d\n", p->misc.occupancy.pblk_forward->block_name );
      if( pblk_back )
	fprintf( fp_out, "***** Occupied Block (back): %d\n", p->misc.occupancy.pblk_back->block_name );
    } else
      goto lk4_forward;;
  } else {
    assert( !pblk_forw );
    assert( !pblk_back );
  }
}
#else
static void show_train_residents_consistency ( FILE *fp_out, TINY_TRAIN_STATE_PTR pT ) {
  assert( fp_out );
  assert( pT );

  if( pT->misc.occupancy.pblk_forward )
    fprintf( fp_out, "***** Occupied Block (forward): %d\n", pT->misc.occupancy.pblk_forward->block_name );
  if( pT->misc.occupancy.pblk_back )
    fprintf( fp_out, "***** Occupied Block (back): %d\n", pT->misc.occupancy.pblk_back->block_name );
}
#endif

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
	  fprintf( fp_out, "%s:\n", (which_SC_from_train_info(trains_tracking[i].pTI))->sc_name );
	  diag_train_info( fp_out, &TI );
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
  if( pthread_create( &P_cbtc_ctrl, NULL, pth_emit_cbtc_ctrl_cmds, (void *)pcomm_threads_prof ) ) {
    errorF( "%s", "failed to invoke the CBTC control commands emitter thread.\n" );
    exit( 1 );
  }
  
  if( pthread_create( &P_il_ctrl_emit, NULL, pth_revise_il_ctrl_bits, (void *)pcomm_threads_prof  ) ) {
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

static void establish_SC_comm ( TINY_COMM_PROF_PTR pcomm_prof ) {
  assert( pcomm_prof );
  TINY_SOCK_DESC *pdescs[END_OF_CBTC_CMDS_INFOS] = {};
  {
    int i;
    pdescs[CBTC_TRAIN_INFORMATION] = pcomm_prof->cbtc.info.train_info.descs;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) { pcomm_prof->cbtc.info.train_info.descs[i] = -1; }
  }
  SC_CTRLCMD_COMM_PROF_PTR *pprofs[END_OF_CBTC_CMDS_INFOS] = {};
  pprofs[CBTC_TRAIN_COMMAND] = pcomm_prof->cbtc.cmd.train_cmd.pprofs;
  
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
  if( establish_SC_comm_cmds( &pcomm_prof->cbtc.cmd.socks, pprofs, (int)END_OF_CBTC_CMDS_INFOS, (int)END_OF_SCs ) < 0 ) {
    errorF("%s", "failed to create the send UDP ports for CBTC/SC control commands.\n");
    exit( 1 );
  }
#ifdef CHK_STRICT_CONSISTENCY
  {
    int i;
    for( i = (int)SC801; i < END_OF_SCs; i++ ) {
      const SC_CTRLCMD_COMM_PROF_PTR p = pcomm_prof->cbtc.cmd.train_cmd.pprofs[i];
      assert( p );
      assert( p->d_send_train_cmd > -1  );
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
      char fname[CBI_CODE_FILENAME_MAXLEN];
      fname[511] = 0;
#ifndef IN_CBI_RESOURCEDIR
      assert( (int)sizeof(fname) > (strlen("./cbi/") + strlen(il_status_geometry_resources[oc_id].csv_fname)) );
      strcat( strcpy(fname, "./cbi/"), il_status_geometry_resources[oc_id].csv_fname );
#else
      strcpy( fname, il_status_geometry_resources[oc_id].csv_fname );
#endif // IN_CBI_RESOURCEDIR
      n = load_cbi_code( il_status_geometry_resources[oc_id].oc_id, fname );
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
#if 0 // *****
  {
    int m = -1;
    m = revise_cbi_codetbl( NULL );
    assert( m > -1 );
    errorF( "revised %d entries of CSV status.\n", m );
  }
#endif
}

static void establish_OC_comm ( TINY_COMM_PROF_PTR pcomm_prof ) {
  assert( pcomm_prof );
  int nsocks_ctrl = -1;
  int nsocks_stat = -1;
  
  TINY_SOCK_CREAT( pcomm_prof->cbi.ctrl.socks );
  nsocks_ctrl = establish_OC_comm_ctrl( &pcomm_prof->cbi.ctrl.socks, pcomm_prof->cbi.ctrl.pprofs, END_OF_ATS2OC, (int)END_OF_ATS2OC );
  if( nsocks_ctrl > 0 ) {
    assert( nsocks_ctrl == END_OF_ATS2OC );
    int j;
#ifdef CHK_STRICT_CONSISTENCY
    for( j = 0; j < nsocks_ctrl; j++ )
      assert( pcomm_prof->cbi.ctrl.pprofs[j]->d_sent_cbi_ctrl > -1 );
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
#if 0
  {
    extern int ars_chk_depschedule ( SCHEDULE_AT_SP sch_dep[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC );
    extern int ars_chk_dstschedule ( SCHEDULE_AT_SP sch_dst[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC_dst, SCHEDULED_COMMAND_C_PTR pC_lok );
    const DWELL_ID did = 7;
    const JOURNEY_ID jid = 2;
    int r = -1;
    SCHEDULED_COMMAND_PTR pC = NULL;    
    online_timetbl = trial_timetable;
    makeup_online_timetable();
    pC = online_timetbl.lkup[jid]->journey.scheduled_commands.pNext;
    {
      while( pC ) {
	assert( pC );
	//if( pC->cmd == ARS_SCHEDULED_SKIP )
	if( (pC->cmd == ARS_SCHEDULED_ARRIVAL) && (pC->attr.sch_dept.dw_seq == did) )
	  break;
	pC = pC->ln.journey.pNext;
      }
    }    
    //r = ars_chk_depschedule( online_timetbl.sp_schedule, pC );
    SCHEDULED_COMMAND_PTR pC_dst = pC;
    SCHEDULED_COMMAND_PTR pC_lok = pC_dst;
    {
      while( pC_lok ) {
	if( pC_lok->cmd == ARS_SCHEDULED_DEPT )
	  break;
	pC_lok = pC_lok->ln.journey.pNext;
      }
    }
    r = ars_chk_dstschedule( online_timetbl.sp_schedule, pC_dst, pC_lok );
    assert( FALSE );
  }
#endif
#if 1
  {
    online_timetbl = trial_timetable;
    makeup_online_timetable();
  }
#endif
  TINY_SOCK socks_srvstat;
  TINY_SOCK_DESC sd_send_srvbeat = -1;
  TINY_SOCK_DESC sd_send_srvstat = -1;
  TINY_SOCK_DESC sd_recv_srvstat = -1;
  
  TINY_COMM_PROF comm_threads_prof;
  memset( &comm_threads_prof, 0, sizeof(comm_threads_prof) );
  
  tzset();
  cons_il_obj_table();
  cons_lkuptbl_cbtc_block_prof();
  cons_lkuptbl_sp2_block();
  
  establish_SC_comm( &comm_threads_prof );
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
  
  TINY_SOCK_CREAT( socks_srvstat );
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
    comm_threads_prof.cbi.ctrl.ready = TRUE;
    while( TRUE ) {
      errorF( "%s", "waken up!\n" );
#if 0
      show_tracking_train_stat( stdout );
#endif
#if 0
      //diag_cbi_stat_attrib( stdout, "T801A_TLSR" );
      //diag_cbi_stat_attrib( stdout, "Lo_CY801A" );
      //diag_cbi_stat_attrib( stdout, "ESP801A" );
      //diag_cbi_stat_attrib( stdout, "EM877_D@JLA" ); // diag_cbi_stat_attrib( stdout, "EM877_D@BGCN" );
      //diag_cbi_stat_attrib( stdout, "EM878_D@JLA" ); // diag_cbi_stat_attrib( stdout, "EM878_D@BGCN" ); 
      diag_cbi_stat_attrib( stdout, "EM878_D@JLA" );
#endif
#if 0
      {
	extern int ars_chk_trgtime ( OFFSET_TIME_TO_FIRE offset_kind, double *pdif, int hour, int minute, int second );
	ars_chk_trgtime( OFFSET_NOTHING, NULL, 11, 19, 0 );
      }
      {
	OC_ID oc_id;
	CBI_STAT_KIND kind;
	int tr_stat = -1;
	const IL_SYM tr = T802A_TR;
	tr_stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym( tr ) );
	printf( "%s : %d\n", cnv2str_il_sym( tr ), tr_stat );
	assert( TRUE );
      }
      {
	extern int ars_chk_trackcirc ( ROUTE_C_PTR proute );
	const IL_SYM rid = S807B_S831B;
	ROUTE_C_PTR pR = conslt_route_prof( rid );
	int r_ctrl = -1;
	r_ctrl  = ars_chk_trackcirc( pR );
	printf( "%s : %d\n", cnv2str_il_sym( rid ), r_ctrl );
	assert( TRUE );
      }
      {
	OC_ID oc_id;
	CBI_STAT_KIND kind;
	int sr_stat = -1;
	const IL_SYM sr = T827B_eTRSR;
	sr_stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym( sr ) );
	printf( "%s : %d\n", cnv2str_il_sym( sr ), sr_stat );
	assert( TRUE );
      }
      {
	extern int ars_chk_routelok ( ROUTE_C_PTR proute );
	const IL_SYM rid = S803A_S811A;
	ROUTE_C_PTR pR = conslt_route_prof( rid );
	int r_ctrl= -1;
	r_ctrl  = ars_chk_routelok( pR );
	printf( "%s : %d\n", cnv2str_il_sym( rid ), r_ctrl );
	assert( TRUE );
      }      
#endif
#if 0
      {
	OC_ID oc_id = END_OF_OCs;
	CBI_STAT_KIND kind = END_OF_CBI_STAT_KIND;
	//const char ctl_bit_ident[] = "P_S807B_S831B";
	//const char ctl_bit_ident[] = "P_S801A_S803A";
	const char ctl_bit_ident[] = "P_S801A_S803A_CAN";
	//const char ctl_bit_ident[] = "P_CY807A_OFF";  // e.g. const char ctl_bit_ident[] = "P_S821A_S801A";
	engage_il_ctrl( &oc_id, &kind, ctl_bit_ident );
	//errorF( "(oc_id): (%d)\n", OC_ID_CONV2INT(oc_id) ); // ***** for debugging.
      }
#endif
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
#if 1
      {
	const JOURNEY_ID jid = 1;
	const int target_rake = 1;
	int i;
	for( i = 0; i < MAX_TRAIN_TRACKINGS; i++ ) {
	  if( (! trains_tracking[i].omit) && (trains_tracking[i].rakeID > 0) ) {
	    TINY_TRAIN_STATE_PTR pT = &trains_tracking[i];
	    assert( pT );
	    if( pT->rakeID == target_rake ) {
	      show_train_residents_consistency( stdout, pT );
#if 0
	      {
		assert( pT );
		extern CBTC_BLOCK_C_PTR any_trains_ahead ( ROUTE_C_PTR proute, int ahead_blk, TINY_TRAIN_STATE_PTR ptrain_ctl );
		const IL_SYM rid = S831B_S821A;
		ROUTE_C_PTR pR = conslt_route_prof( rid );
		if( pT->misc.occupancy.pblk_forward ) {
		  const unsigned short blk_res = (pT->misc.occupancy.pblk_forward)->block_name;
		  int i;
		  for( i = 0; i < pR->ars_ctrl.trg_sect.num_blocks; i++ ) {
		    CBTC_BLOCK_PTR pprof = conslt_cbtc_block_prof( pR->ars_ctrl.trg_sect.trg_blks[i] );
		    assert( pprof );		    
		    if( pprof->block_name == blk_res ) {		      
		      break;
		    }
		  }
		  if( ++i < pR->ars_ctrl.trg_sect.num_blocks ) {
		    CBTC_BLOCK_C_PTR pB = NULL;
		    pB = any_trains_ahead( pR, i, pT );
		    if( pB )
		      printf( "%s : ahead train detected on %d\n", cnv2str_il_sym( rid ), pB->block_name );
		  }
		}
		assert( TRUE );
	      }
	      {
		assert( pT );
		extern CBTC_BLOCK_C_PTR ars_chk_hit_trgsection1 ( ROUTE_C_PTR proute, TINY_TRAIN_STATE_PTR ptrain_ctrl, int blk_specified );
		const IL_SYM rid = S807B_S831B;
		ROUTE_C_PTR pR = conslt_route_prof( rid );
		CBTC_BLOCK_C_PTR pB = ars_chk_hit_trgsection1( pR, pT, -1 );
		if( pB )
		  printf( "%s : %d\n", cnv2str_il_sym( rid ), pB->block_name );
	      }
#endif
#if 1
	      {
		ARS_REASONS r = END_OF_ARS_REASONS;
		JOURNEY_PTR pJ = &online_timetbl.lkup[jid]->journey;
		ARS_REASONS res = END_OF_ARS_REASONS;
		ARS_EVENT_ON_SP ev;
		pJ->ptrain_ctrl = pT;
		ars_judge_arriv_dept_skip( &ev, pT );		
		r = ars_atodept_on_journey( &online_timetbl, pJ, &ev );
		//assert( r != ARS_ROUTE_CONTROLLED_NORMALLY ); // *****
		r = ars_routectl_on_journey( &online_timetbl, pJ );
		//assert( r != ARS_ROUTE_CONTROLLED_NORMALLY ); // *****
		ars_schcmd_ack( &res, pJ, &ev );
		{
		  char cmd_name[6] = "";
		  char *s = NULL;
		  s = cnv2abb_ars_command( cmd_name, pJ->scheduled_commands.pNext->cmd );
		  if( !s ) {
		    strncpy( cmd_name, "???", 5 );
		    s = cmd_name;
		  }
		  printf( "(jid, next_cmd, past_cmds): (%d, %s, ", pJ->jid, s );
		  printf( "{" );
		  {
		    BOOL first = TRUE;
		    SCHEDULED_COMMAND_PTR p = pJ->past_commands.phead;
		    while( p ) {
		      assert( p );
		      if( !first )
			printf( ", " );
		      s = cnv2abb_ars_command(cmd_name,p->cmd);
		      if( !s ) {
			strncpy( cmd_name, "???", 5 );
			s = cmd_name;
		      }
		      printf( "%s", s );
		      p = p->ln.journey.past.pNext;
		      first = FALSE;
		    }
		  }
		  printf( "})\n" );
		  //assert( FALSE ); // *****
		}
	      }
#endif
#if 0
	      int r_mutex = -1;
	      r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
	      if( r_mutex ) {
		assert( FALSE );
	      } else {
		//fst grp.
		TRAIN_ID tid = {};
		tid.jid = 37;
		//change_train_state_trainID( pT, tid, TRUE );
		//change_train_state_skip_next_stop( pT, TRUE, TRUE );
		//change_train_state_ATO_dept_cmd( pT, TRUE, TRUE );
		//change_train_state_TH_cmd( pT, TRUE, TRUE );
		//change_train_state_ordering_emergency_stop( pT, TRUE, TRUE );
		//change_train_state_releasing_emergency_stop( pT, TRUE, TRUE );
		//change_train_state_perf_regime( pT, PR_NO_COMMAND, TRUE );
		//change_train_state_turnback_siding( pT, TRUE, TRUE );
		//change_train_state_ATB_cmd( pT, TRUE, TRUE );
		//change_train_state_train_remove( pT, TRUE, TRUE );
		//change_train_state_dwell_time( pT, 1025, TRUE );
		//change_train_state_dest_blockID( pT, 1416, TRUE );
		//change_train_state_crnt_blockID( pT, 1416, TRUE );
		// 2nd grp.
		//change_train_state_crnt_station_plcode( pT, CPL_INVALID_TB_SECTION, TRUE );
		//change_train_state_next_station_plcode( pT, NPL_OTHERS, TRUE );
		//change_train_state_dst_station_plcode( pT, DPL_INVALID_TB_SECTION, TRUE );
		//change_train_state_destination_number( pT, 54, TRUE );
		//change_train_state_next_station_number( pT, 8, TRUE );
		//change_train_state_crnt_station_number( pT, 11, TRUE );
		//change_train_state_keep_door_closed( pT, TRUE, TRUE );
		//change_train_state_out_of_service( pT, TRUE, TRUE );
		//change_train_state_leave_now( pT, TRUE, TRUE );
		//change_train_state_ordering_wakeup( pT, TRUE, TRUE );
		//change_train_state_ordering_standby( pT, TRUE, TRUE );
		//change_train_state_ordering_reset_onboard( pT, TRUE, TRUE );
		//change_train_state_static_test_cmd( pT, TRUE, TRUE );
		//change_train_state_depcond_release( pT, TRUE, TRUE );
		//change_train_state_origin_station( pT, TRUE, TRUE );
		//change_train_state_next_st_dooropen_side( pT, NDS_NOT_OPEN, TRUE );
		//change_train_state_operation_mode( pT, OM_UNKNOWN_NO_DIRECTIVE, TRUE );
		//change_train_state_coasting_cmd( pT, FALSE, TRUE );
		//change_train_state_energy_saving( pT, TRUE, TRUE );
		//change_train_state_system_switch_cmd( pT, SW_NO_SWITCHING, TRUE );
		//change_train_state_remote_door_opening( pT, RDC_NO_OPENING, TRUE );
		//change_train_state_remote_door_closing( pT, RDC_NO_CLOSING, TRUE );
		//change_train_state_dynamic_test_cmd( pT, TRUE, TRUE );
		//change_train_state_inching_cmd( pT, TRUE, TRUE );
		//change_train_state_back_inching_cmd( pT, TRUE, TRUE );
		//change_train_state_em_door_release( pT, EMD_NO_CMD, TRUE );
		//change_train_state_back_vrs_reset( pT, TRUE, TRUE );
		//change_train_state_forward_vrs_reset( pT, TRUE, TRUE );
		//change_train_state_maximum_speed_cmd( pT, 61, TRUE );
		//change_train_state_passenger_address( pT, 79, TRUE );
		change_train_state_dep_dir( pT, MD_UNKNOWN, TRUE );
		//change_train_state_regulation_speed( pT, 18, TRUE );
		//change_train_state_passenger_display_message( pT, 65539, TRUE );
		r_mutex = -1;
		r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
		assert( !r_mutex );
	      }
#endif
#if 0
	      char str[255 + 1] = "detect_train_docked @";
	      ARS_EVENT_ON_SP ev;
	      STOPPING_POINT_CODE sp = SP_NONSENS;
	      //pT->stop_detected = SP_81; // *****
	      sp = ars_judge_arriv_dept_skip( &ev, pT );
	      {
		char buf[255 + 1];
		buf[255] = 0;
		switch( sp ) {
		case SP_NONSENS:
		  strcpy( buf, "" );
		  break;
		case SP_73: // JLA_PL1
		  strcpy( buf, "JLA_PL1" );
		  break;
		case SP_74: // JLA_PL2
		  strcpy( buf, "JLA_PL2" );
		  break;
		case SP_75: // JLA_PL3
		  strcpy( buf, "JLA_PL3" );
		  break;
		case SP_D4: // JLA_TB4
		  strcpy( buf, "JLA_TB4" );
		  break;
		case SP_D9: // JLA_TB3
		  strcpy( buf, "JLA_TB3" );
		  break;
		case SP_76: // KIKJ_PL2
		  strcpy( buf, "KIKJ_PL2" );
		  break;
		case SP_77: // KIKJ_PL1
		  strcpy( buf, "KIKJ_PL1" );
		  break;
		case SP_78: // OKBS_PL2
		  strcpy( buf, "OKBS_PL2" );
		  break;
		case SP_79: // OKBS_PL1
		  strcpy( buf, "OKBS_PL1" );
		  break;
		case SP_80: // BTGD_PL2
		  strcpy( buf, "BTGD_PL2" );
		  break;
		case SP_81: // BTGD_PL1
		  strcpy( buf, "BTGD_PL1" );
		  break;
		case SP_D5: // BTGD_TB1
		  strcpy( buf, "BTGD_TB1" );
		  break;
		case SP_D0: // BTGD_TB2
		  strcpy( buf, "BTGD_TB2" );
		  break;
		case END_OF_SPs:
		  assert( FALSE );
		}
		str[255] = 0;
		switch( ev.detail ) {
		case ARS_DOCK_DETECTED:
		  strncpy( str, "ARS_DOCK_DETECTED @", 255 );
		  break;
		case ARS_LEAVE_DETECTED:
		  strncpy( str, "ARS_LEAVE_DETECTED @", 255 );
		  break;
		case ARS_SKIP_DETECTED:
		  strncpy( str, "ARS_SKIP_DETECTED @", 255 );
		  break;
		case ARS_DETECTS_NONE:
		  assert( ev.sp == SP_NONSENS );
		  assert( strnlen( buf, 255 ) == 0 );
		  strncpy( str, "ARS_DETECTS_NONE", 255 );
		  break;
		default:
		  assert( FALSE );
		}
		strncat( str, buf, 255 );
		printf( "%s.\n", str );
	      }
#endif
	    }
	  }
	}
      }
#endif
      
      load_train_command();
#ifdef CHK_STRICT_CONSISTENCY
      chk_solid_train_cmds();
#endif // CHK_STRICT_CONSISTENCY
#if 0 // *****
      if( diag_tracking_train_cmd( stdout ) > 0 )
	fprintf( stdout, "\n" );
#endif
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

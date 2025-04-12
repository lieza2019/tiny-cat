#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "ars.h"
#include "surveill.h"
#include "timetable.h"

const char *cnv2str_perf_regime[] = {
  "PERFREG_SLOW",
  "PERFREG_NORMAL",
  "PERFREG_FAST",
  "END_OF_PERFREG",
  NULL
};

char *cnv2abb_ars_command ( char *abb, ARS_SCHEDULED_CMD cmd ) {
  const int nchrs = 5;
  char *r = NULL;
  int i;
  const struct {
    ARS_SCHEDULED_CMD cmd;
    char *abb;
  } lkup[] = {
    {ARS_SCHEDULED_ROUTESET, "Set"},
    {ARS_SCHEDULED_ROUTEREL, "Rel"},
    {ARS_SCHEDULED_ARRIVAL, "Arr"},
    {ARS_SCHEDULED_DEPT, "Dep"},
    {ARS_SCHEDULED_SKIP, "SS"},
    {END_OF_SCHEDULED_CMDS, "Fin"},
    {ARS_CMD_NOP, "Nop"},    
  };
  for( i = 0; i <= ARS_CMD_NOP; i++ ) {
    if( cmd == lkup[i].cmd ) {
      strncpy( abb, lkup[i].abb, nchrs );
      r = abb;
      break;
    }
  }
  return r;
}

const char *cnv2str_ars_reasons[] = {
  "ARS_NO_RAKE_ASGNED",
  "ARS_NO_ROUTESET_CMD",
  "ARS_NO_TRIGGERED",
  "ARS_FOUND_TRAINS_AHEAD",
  "ARS_CTRL_TRACKS_ROUTELOCKED",
  "ARS_CTRL_TRACKS_DROP",
  "ARS_WAITING_ROUTESET_TIME",
  "ARS_WAITING_PRED_DEPTRAINS",
  "ARS_FOUND_PRED_ARRIVDEP_AT_DST",
  "ARS_NO_ROUTE_OPEN",
  "ARS_NOW_ROUTE_CONTROLLING",
  "ARS_NOW_ATODEPT_EMISSION",
  "ARS_ROUTE_CONTROLLED_NORMALLY",
  "ARS_MUTEX_BLOCKED",
  "ARS_NO_SCHEDULED_CMDS",
  "ARS_ILLEGAL_CMD_ROUTESET",
  "ARS_ILLEGAL_CMD_ROUTEREL",
  "ARS_ILLEGAL_CMD_ARRIV",
  "ARS_ILLEGAL_CMD_DEPT",
  "ARS_ILLEGAL_CMD_SKIP",
  "ARS_INCONSISTENT_ROUTESET",
  "ARS_INCONSISTENT_ROUTEREL",
  "ARS_INCONSISTENT_ARRIVAL",
  "ARS_INCONSISTENT_DEPT",
  "ARS_INCONSISTENT_SKIP",
  "ARS_MISSING_ROUTEREL",
  "ARS_NO_DEADEND_CMD",
  "ARS_WRONG_CMD_ATTRIB",
  "ARS_FOUND_ERROROUS_INTERLOCKDEF",
  NULL
};

SYSTEM_PARAMS tiny_system_params = {
  DEFALUT_ROUTESET_OFFSET,
  DEFAULT_DEPARTURE_OFFSET
};

static SCHEDULED_COMMAND sch_cmd_nodes[SCHEDULED_COMMANDS_NODEBUF_SIZE];
static int frontier;

SCHEDULED_COMMAND_PTR sch_cmd_newnode ( void ) {
  SCHEDULED_COMMAND_PTR r = NULL;
  if( frontier < SCHEDULED_COMMANDS_NODEBUF_SIZE )
    r = &sch_cmd_nodes[++frontier];
  else
    errorF( "%s", "nodes exhausted, to create a scheduled command.\n" );
  return r;
};

time_t mktime_of_cmd ( struct tm *pT, ARS_ASSOC_TIME_C_PTR ptime_cmd ) {
  assert( pT );
  assert( ptime_cmd );
  time_t r;
  
  pT->tm_hour = ptime_cmd->hour;
  pT->tm_min = ptime_cmd->minute;
  pT->tm_sec = ptime_cmd->second;
  pT->tm_year = ptime_cmd->year;
  pT->tm_mon = ptime_cmd->month;
  pT->tm_mday = ptime_cmd->day;
  r = mktime( pT );
  return r;
}

STOPPING_POINT_CODE ars_judge_arriv_dept_skip ( ARS_EVENT_ON_SP_PTR pdetects, TINY_TRAIN_STATE_PTR pT ) {
  assert( pdetects );
  assert( pT );
  STOPPING_POINT_CODE hit_sp = SP_NONSENS;
  
  pdetects->sp = SP_NONSENS;
  pdetects->situation = ARS_DETECTS_NONE;
  if( pT->stop_detected != SP_NONSENS ) {
    pdetects->sp = pT->stop_detected;
    pdetects->situation = ARS_DOCK_DETECTED;
    hit_sp = detect_train_docked( &pdetects->situation, DOCK_DETECT_MINOR, pT );
    if( hit_sp == SP_NONSENS ) {      
      /* Accounting on the irregular case, detect_train_docked() may return SP_NONSENS, with the condition that
	 both occupied blocks of front & back has been 0, in the situation the train been in the platform and
	 still with ARS_DOCK_DETECTED.
      */
      //assert( pdetects->situation != ARS_DOCK_DETECTED ); // this assertion seems insufficient, as below.      
      if( pdetects->situation != ARS_DOCK_DETECTED ) {
	pdetects->sp = SP_NONSENS;
	pT->stop_detected = SP_NONSENS;
      }
    } else {
      if( pdetects->situation == ARS_LEAVE_DETECTED ) {
	pdetects->sp = hit_sp;
	pT->stop_detected = SP_NONSENS;
      } else {
	assert( pdetects->sp == pT->stop_detected );
	assert( pdetects->situation == ARS_DOCK_DETECTED );
      }
    }
  } else {
    assert( pT->stop_detected == SP_NONSENS );
    hit_sp = detect_train_docked( &pdetects->situation, DOCK_DETECT_MAJOR, pT );
    if( hit_sp != SP_NONSENS ) {
      assert( pdetects->situation == ARS_DOCK_DETECTED );
      pdetects->sp = hit_sp;
      pT->stop_detected = hit_sp;
    } else {
      hit_sp = detect_train_skip( &pdetects->situation, pT );
      if( hit_sp != SP_NONSENS ) {
	assert( pT->stop_detected == SP_NONSENS );
	assert( pdetects->situation == ARS_SKIP_DETECTED );
	pdetects->sp = hit_sp;
      }
    }
  }
  return pdetects->sp;
}

static int ars_chk_trgtime ( OFFSET_TIME_TO_FIRE offset_kind, double *pdif, int hour, int minute, int second ) { // well tested, 2025/01/02
  assert( (offset_kind >= 0) && (offset_kind < END_OF_OFFSET_TIMES) );
  assert( (hour >= 0) && (hour < 24) );
  assert( (minute >= 0) && (minute < 60) );
  assert( (second >= 0) && (second < 60) );
  int r = -1;
  
  struct tm *pT_crnt = NULL;
  time_t crnt_time = 0;
  tzset();
  
  crnt_time = time( NULL );
  pT_crnt = localtime( &crnt_time );
#if 0 // ***** for debugging.
  printf( "crnt_localtime: yyyy/mm/dd, HH:MM:SS => %d/%02d/%02d, %02d:%02d:%02d\n",
	  (pT_crnt->tm_year + 1900), (pT_crnt->tm_mon + 1), pT_crnt->tm_mday, pT_crnt->tm_hour, pT_crnt->tm_min, pT_crnt->tm_sec );
#endif
  if( pT_crnt ) {
    struct tm T_trg = {};
    T_trg.tm_year = pT_crnt->tm_year;
    T_trg.tm_mon = pT_crnt->tm_mon;
    T_trg.tm_mday = pT_crnt->tm_mday;
    T_trg.tm_hour = hour;
    T_trg.tm_min = minute;
    T_trg.tm_sec = second;
    {
      struct tm *pT_trg = NULL;
      time_t crnt_local = 0;
      time_t trg_time = 0;
      crnt_local = mktime( pT_crnt );
      trg_time = mktime( &T_trg );
      switch( offset_kind ) {
      case OFFSET_TO_ROUTESET:
	{
	  time_t w = trg_time - tiny_system_params.routeset_offset;
	  pT_trg = localtime( &w );
	  if( pT_trg )
	    trg_time = mktime( pT_trg );
	}
	break;
      case OFFSET_TO_DEPARTURE:
	{
	  time_t w = trg_time - tiny_system_params.departure_offset;
	  pT_trg = localtime( &w );
	  if( pT_trg )
	    trg_time = mktime( pT_trg );
	}
	break;
      case OFFSET_NOTHING:
	goto no_offset;
      default:
	assert( FALSE );
      no_offset:
	pT_trg = localtime( &trg_time );
	if( pT_trg )
	   trg_time = mktime( pT_trg );
	/* fall thru. */
      }
      if( pT_trg ) {
	double diff = -1;
	diff = difftime( trg_time, crnt_local );
	if( pdif )
	  *pdif = diff;
	r = (diff < (double)0) ? 1 : 0;
#if 0 // ***** for debugging.
	printf( "trig_localtime: yyyy/mm/dd, HH:MM:SS => %d/%02d/%02d, %02d:%02d:%02d\n",
		(pT_trg->tm_year + 1900), (pT_trg->tm_mon + 1), pT_trg->tm_mday, pT_trg->tm_hour, pT_trg->tm_min, pT_trg->tm_sec );
	printf( "judgement: %s\n", (r ? "TRIGGERED!" : "STANDING-BY") );
#endif
      }
    }
  }
  return r;
}

static SCHEDULED_COMMAND_C_PTR next_arrcmd ( ARS_REASONS *pres, BOOL *pss, SCHEDULED_COMMAND_C_PTR pC ) {
  assert( pres );
  assert( pC );
  assert( pC->cmd == ARS_SCHEDULED_DEPT );
  const STOPPING_POINT_CODE org_sp = pC->attr.sch_dept.dep_sp;
  
  SCHEDULED_COMMAND_C_PTR p = pC->ln.journey.planned.pNext;
  *pres = END_OF_ARS_REASONS;
  *pss = FALSE;
  while( p ) {
    if( p->cmd == END_OF_SCHEDULED_CMDS )
      break;
    else if( p->cmd == ARS_SCHEDULED_ARRIVAL ) {
      if( p->attr.sch_arriv.arr_sp == org_sp )
	*pres = ARS_INCONSISTENT_ARRIVAL;
      break;
    } else if( p->cmd == ARS_SCHEDULED_DEPT ) {
      *pres = ARS_INCONSISTENT_DEPT;
      break;
    } else {
      if( p->cmd == ARS_SCHEDULED_SKIP )
	*pss = TRUE;
      p = p->ln.journey.planned.pNext; // includng the case of ARS_SCHEDULED_SKIP.
    }
  }
  if( !p ) {
    *pres = ARS_NO_DEADEND_CMD;
    p = pC;
  }
  return p;
}

ARS_REASONS ars_atodept_on_journey ( TIMETABLE_PTR pTT, JOURNEY_PTR pJ, ARS_EVENT_ON_SP *pev_sp ) {
  assert( pTT );
  assert( pJ );
  assert( pev_sp );
  ARS_REASONS r = END_OF_ARS_REASONS;
  TINY_TRAIN_STATE_PTR pT = NULL;
  
  pT = pJ->ptrain_ctrl;
  if( pT ) {
    if( pev_sp->sp != SP_NONSENS ) {
      if( pev_sp->situation == ARS_DOCK_DETECTED ) {
	SCHEDULED_COMMAND_PTR pC = NULL;
	pC = pJ->scheduled_commands.pNext;
	if( pC->cmd == ARS_SCHEDULED_DEPT ) {	  
	  if( pev_sp->sp == pC->attr.sch_dept.dep_sp ) {
	    ROUTE_C_PTR pR = NULL;
	    pR = conslt_route_prof( pC->attr.sch_dept.dep_route );
	    if( pR ) {
	      if( pR->ars_ctrl.app ) {
		ARS_REASONS res_next_arr = END_OF_ARS_REASONS;
		BOOL ss = FALSE;
		CBTC_BLOCK_C_PTR pdst = NULL;
		SCHEDULED_COMMAND_C_PTR parr_next = NULL;
		int r_mutex = -1;
		r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
		if( r_mutex ) {
		  assert( FALSE );
		} else {  
		  // this fragment for CBTC control emission, should be moved to the functions dedicated to manage CBTC command controlling.      
		  parr_next = next_arrcmd( &res_next_arr, &ss, pC );
		  assert( parr_next );
		  //printf( "result of next_arrcmd: %s\n", (res_next_arr != END_OF_ARS_REASONS ? cnv2str_ars_reasons[res_next_arr] : "no_claims") ); // *****
		  r = res_next_arr;
		  if( (parr_next->cmd == ARS_SCHEDULED_ARRIVAL) && (res_next_arr == END_OF_ARS_REASONS) )
		    pdst = lookup_block_of_sp( parr_next->attr.sch_arriv.arr_sp );
		  else
		    pdst = pR->ars_ctrl.trip_info.dst.pblk;
		  if( pdst ) {
		    //pT->skip_next_stop = ss; // *****
		    change_train_state_skip_next_stop( pT, TRUE, TRUE );
		    //pT->dest_blockID = pdst->block_name; // *****
		    change_train_state_dest_blockID( pT, pdst->block_name, TRUE );
		    {
		      char *route_SoHR = NULL;
		      char raw[CBI_STAT_IDENT_LEN + 1];
		      int stat = -1;
		      strncpy( raw, cnv2str_il_sym( pR->sig_pair.org.sig ), CBI_STAT_IDENT_LEN );
		      route_SoHR = mangl2_So_Sxxxy_HyR( raw );
		      {
			OC_ID oc_id;
			CBI_STAT_KIND kind;
			stat = conslt_il_state( &oc_id, &kind, route_SoHR );
		      }
		      if( stat > 0) {
			if( pC->attr.sch_dept.dep_dir.L )
			  //pT->dep_dir = MD_UP_DIR; // *****
			  change_train_state_dep_dir( pT, MD_UP_DIR, TRUE );
			else if( pC->attr.sch_dept.dep_dir.R )
			  //pT->dep_dir = MD_DOWN_DIR; // *****
			  change_train_state_dep_dir( pT, MD_DOWN_DIR, TRUE );
			else
			  //pT->dep_dir = MD_UNKNOWN; // *****
			  change_train_state_dep_dir( pT, MD_UNKNOWN, TRUE );
			//pT->ATO_dept_cmd = TRUE; // *****
			change_train_state_ATO_dept_cmd( pT, TRUE, TRUE );
			r = ARS_NOW_ATODEPT_EMISSION;;
		      } else {
			assert( stat <= 0 );
			if( stat == 0 )
			  r = ARS_NO_ROUTE_OPEN;
			else {
			  assert( stat < 0 );
			  switch( stat ) {
			  case -1:
			    r = ARS_MUTEX_BLOCKED;
			    break;
			  case -2:
			    r = ARS_FOUND_ERROROUS_INTERLOCKDEF;
			    break;
			  default:
			    assert( FALSE );
			  }
			}
		      }
		    }
		  } else {
		    //pT->dest_blockID = 0; // *****
		    change_train_state_dest_blockID( pT, 0, TRUE );
		    r = ARS_INCONSISTENT_DEPT;
		  }
		  r_mutex = -1;
		  r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
		  assert( !r_mutex );
		}
	      } else
		r = ARS_ILLEGAL_CMD_DEPT;
	    } else
	      r = ARS_WRONG_CMD_ATTRIB;
	  } else
	    r = ARS_NO_TRIGGERED;
	} else
	  r = ARS_NO_TRIGGERED;
      } else
	r = ARS_NO_TRIGGERED;
    } else
      r = ARS_NO_TRIGGERED;
  } else
    r = ARS_NO_RAKE_ASGNED;
  assert( r != END_OF_ARS_REASONS );  
  return r;
}

static int ars_chk_routelok ( ROUTE_C_PTR proute ) { // well tested, 2025/01/01
  assert( proute );
  assert( proute->ars_ctrl.app );
  int r = -1;
  
  int i = -1;  
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks_lok; i++ ) {
    OC_ID oc_id;
    CBI_STAT_KIND kind;
    TRACK_C_PTR ptr = NULL;
    ptr = proute->ars_ctrl.ctrl_tracks.pchk_trks[i];
    assert( ptr );
    assert( ptr->kind == _TRACK );
    {
      int stat = -1;
      if( ptr->lock.TLSR.app ) {
	assert( ptr->lock.TLSR.kind == _TLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.TLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _TLSR );
	    printf( "TLSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.TLSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.TLSR.id), __FILE__, __LINE__ );
	  }
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _TLSR );
      }
      if( ptr->lock.TRSR.app ) {
	assert( ptr->lock.TRSR.kind == _TRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.TRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _TRSR );
	    printf( "TRSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.TRSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.TRSR.id), __FILE__, __LINE__ );
	  }  
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _TRSR );
      }
      if( ptr->lock.sTLSR.app ) {
	assert( ptr->lock.sTLSR.kind == _sTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.sTLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _sTLSR );
	    printf( "sTLSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.sTLSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.sTLSR.id), __FILE__, __LINE__ );
	  }
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _sTLSR );
      }
      if( ptr->lock.sTRSR.app ) {
	assert( ptr->lock.sTRSR.kind == _sTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.sTRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _sTRSR );
	    printf( "sTRSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.sTRSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.sTRSR.id), __FILE__, __LINE__ );
	  }
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _sTRSR );
      }
      if( ptr->lock.eTLSR.app ) {
	assert( ptr->lock.eTLSR.kind == _eTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.eTLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _eTLSR );
	    printf( "eTLSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.eTLSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.eTLSR.id), __FILE__, __LINE__ );
	  }
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _eTLSR );
      }
      if( ptr->lock.eTRSR.app ) {
	assert( ptr->lock.eTRSR.kind == _eTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->lock.eTRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 ) {
	    assert( kind == _eTRSR );
	    printf( "eTRSR locked, detected on %s\n", cnv2str_il_sym(ptr->lock.eTRSR.id) ); // ***** for debugging.
	  } else {
	    assert( stat < 0 );
	    if( stat == -2 )
	      errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->lock.eTRSR.id), __FILE__, __LINE__ );
	  }
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _eTRSR );
      }
      r = stat;
    }
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_lok) ? (r >= 1) : (r <= 0) );  
  return r;
}

static int ars_chk_trackcirc ( ROUTE_C_PTR proute ) { // well tested, 2025/01/01
  assert( proute );
  int r = -1;
  
  int i;
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks_occ; i++ ) {
    OC_ID oc_id;
    CBI_STAT_KIND kind;
    int stat = -1;
    
    TRACK_C_PTR ptr = NULL;
    ptr = proute->ars_ctrl.ctrl_tracks.pchk_trks[i];
    assert( ptr );
    assert( ptr->kind == _TRACK );
    stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(ptr->id) );
    if( stat <= 0 ) {
      if( stat == 0 ) {
	assert( kind == _TRACK );
      } else {
	assert( stat < 0 );
	if( stat == -2 )
	  errorF( "FATAL! %s, no such cbi status, in %s:%d\n", cnv2str_il_sym(ptr->id), __FILE__, __LINE__ );
      }
      r = stat;
      break;
    }
    assert( stat >= 1 );
    assert( kind == _TRACK );
    r = stat;
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_occ) ? (r >= 1) : (r <= 0) );
  return r;
}

static SCHEDULED_COMMAND_PTR fetch_routeset_cmd ( JOURNEY_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR r = NULL;
  
  SCHEDULED_COMMAND_PTR *ppC = NULL;
  ppC = &pJ->scheduled_commands.pNext;
  assert( ppC );  
  while( *ppC ) {
    assert( *ppC );
    if( ! (*ppC)->checked ) {
      if( (*ppC)->cmd == ARS_SCHEDULED_ROUTESET ) {
	r = *ppC;
	break;
      }
#if 0
      else {
	if( ((*ppC)->cmd == ARS_SCHEDULED_DEPT) || ((*ppC)->cmd == ARS_SCHEDULED_SKIP) )
	  break;
      }
#endif
    }
    ppC = &(*ppC)->ln.journey.planned.pNext;
  }
  assert( ppC );
  return r;
}

static CBTC_BLOCK_C_PTR any_trains_ahead ( ROUTE_C_PTR proute, int ahead_blk, TINY_TRAIN_STATE_PTR ptrain_ctl ) {
  assert( proute );
  assert( (ahead_blk > 0) && (ahead_blk <= proute->ars_ctrl.trg_sect.num_blocks) );
  assert( ptrain_ctl );
  CBTC_BLOCK_C_PTR r = NULL;
  BOOL found = FALSE;
  
  int i;
  for( i = ahead_blk; i < proute->ars_ctrl.trg_sect.num_blocks; i++ ) {
    CBTC_BLOCK_C_PTR pB = proute->ars_ctrl.trg_sect.ptrg_blks[i];
    assert( pB );
    TINY_TRAIN_STATE_PTR pT = NULL;
    pT = read_residents_CBTC_BLOCK( proute->ars_ctrl.trg_sect.ptrg_blks[i] );
    if( pT ) {
      TINY_TRAIN_STATE_PTR p = pT;
      while( p ) {
	assert( p != ptrain_ctl );
	p = p->misc.occupancy.pNext;
      }
      found = TRUE;
      goto exam_border_consistency;
    } else {
      assert( !pT );
      int i;
    exam_border_consistency:      
      for( i = 0; i < MAX_ADJACENT_BLKS; i++ ) {
	TINY_TRAIN_STATE_PTR p = read_edge_of_residents_CBTC_BLOCK1( pB, i );
	if( p ) {
	  assert( p != ptrain_ctl );
	  if( !found ) {
	    pT = p;
	    found = TRUE;
	  }
	}
      }
    }
    assert( pB );
    if( found ) {
      assert( pT );
      r = pB;
      break;
    }
  }
  return r;
}

static TINY_TRAIN_STATE_PTR on_the_edge ( CBTC_BLOCK_C_PTR pB, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  assert( pT );
  TINY_TRAIN_STATE_PTR r = NULL;
  
  int i;
  for( i = 0; i < MAX_ADJACENT_BLKS; i++ ) {
    TINY_TRAIN_STATE_PTR p = read_edge_of_residents_CBTC_BLOCK1( pB, i );
    if( p == pT ) {
      r = p;
      break;
    }
  }
  return r;
}

static int ars_chk_hit_trgsection ( ROUTE_C_PTR proute, CBTC_BLOCK_C_PTR *ppblk_hit, TINY_TRAIN_STATE_PTR ptrain_ctl, int blk_specified ) { // well tested, 2025/01/02
  assert( proute );
  assert( ppblk_hit );
  assert( proute->ars_ctrl.app );
  assert( ptrain_ctl );
  int r = -1;
  
  int i;
  r = 0;
  *ppblk_hit = NULL;
  for( i = 0; i < proute->ars_ctrl.trg_sect.num_blocks; i++ ) {    
    CBTC_BLOCK_C_PTR pB = proute->ars_ctrl.trg_sect.ptrg_blks[i];
    assert( pB );
    TINY_TRAIN_STATE_PTR pT = NULL;
    if( blk_specified >= 0 )
      if( pB->block_name != blk_specified )
	continue;
    pT = read_residents_CBTC_BLOCK( pB );
    while( pT ) {
      if( pT == ptrain_ctl ) {
	*ppblk_hit = pB;
	break;
      } else {
	assert( !(*ppblk_hit) );
	pT = pT->misc.occupancy.pNext;
      }
    }
    if( !(*ppblk_hit) ) {
      pT = on_the_edge( pB, ptrain_ctl );
      if( pT == ptrain_ctl ) {	
	*ppblk_hit = pB;
	break;
      }
    } else
      break;
  }
  if( *ppblk_hit ) {
    r = 2;
    if( ++i < proute->ars_ctrl.trg_sect.num_blocks ) {
      CBTC_BLOCK_C_PTR pB_pred = NULL;
      pB_pred = any_trains_ahead( proute, i, ptrain_ctl );
      if( pB_pred )
	r = 1;
    }
  }
  return r;
}

static SCHEDULED_COMMAND_C_PTR is_fellow ( SCHEDULED_COMMAND_C_PTR pcmd, SCHEDULED_COMMAND_C_PTR pC, const int sentinel ) {
  assert( pcmd );
  assert( pC );
  assert( sentinel > -1 );
  SCHEDULED_COMMAND_C_PTR r = NULL;

  assert( ! pcmd->checked );
  assert( ! pC->checked );
  SCHEDULED_COMMAND_C_PTR p = pC->ln.sp_sch.pFellow;
  
  int i = 0;
  while( p ) {
    assert( !r );
    if( i >= sentinel ) {
      assert( FALSE );
      break;
    } else if( p == pC ) {
      break;
    } else if( p->checked ) {
      p = p->ln.sp_sch.pFellow;
      continue;
    } else
      if( p == pcmd ) {
	r = p;
	break;
      }
    p = p->ln.sp_sch.pFellow;
    i++;
  }
  return r;
}

static int ars_chk_depschedule ( SCHEDULE_AT_SP sch_dep[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC ) { // well tested, 2025/01/05
  assert( sch_dep );
  assert( pC );
  assert( ! pC->checked );
  assert( (pC->cmd == ARS_SCHEDULED_DEPT) || (pC->cmd == ARS_SCHEDULED_SKIP) );  
  int r = -1;

  STOPPING_POINT_CODE dep_sp = SP_NONSENS;
  switch( pC->cmd ) {
  case ARS_SCHEDULED_DEPT:
    dep_sp = pC->attr.sch_dept.dep_sp;
    break;
  case ARS_SCHEDULED_SKIP:
    dep_sp = pC->attr.sch_skip.pass_sp;
    break;
  default:
    assert( FALSE );
  }
  assert( dep_sp != SP_NONSENS );  
  {
    const int num_cmds = sch_dep[dep_sp].num_events;
    SCHEDULED_COMMAND_C_PTR pcmd_next = sch_dep[dep_sp].pNext;
    while( pcmd_next ) {
      assert( pcmd_next );
      assert( (pcmd_next->cmd == ARS_SCHEDULED_ARRIVAL) || (pcmd_next->cmd == ARS_SCHEDULED_DEPT) || (pcmd_next->cmd == ARS_SCHEDULED_SKIP) );
      if( pcmd_next == pC ) {
	assert( ! pcmd_next->checked );
	assert( pcmd_next->jid == pC->jid );
	break;
      } else if( pcmd_next->checked ) {
	assert( pcmd_next != pC );
	pcmd_next = pcmd_next->ln.sp_sch.pNext;
	continue;
      } else if( is_fellow( pcmd_next, pC, num_cmds ) ) {
	break;
      } else {
	switch( pcmd_next->cmd ) {
	case ARS_SCHEDULED_ARRIVAL:	  
#ifdef CHK_STRICT_CONSISTENCY
	  assert( pcmd_next->attr.sch_arriv.arr_sp == dep_sp );
#endif // CHK_STRICT_CONSISTENCY
	  r = 1;
	  break;
	case ARS_SCHEDULED_DEPT:
#ifdef CHK_STRICT_CONSISTENCY	
	  assert( pcmd_next->attr.sch_dept.dep_sp == dep_sp );
	  assert( pcmd_next != pC );
#endif // CHK_STRICT_CONSISTENCY
	  if( pC->cmd == ARS_SCHEDULED_DEPT )
	    r = (pcmd_next->jid == pC->jid);
	  else
	    r = 0;
	  break;
	case ARS_SCHEDULED_SKIP:
#ifdef CHK_STRICT_CONSISTENCY
	  assert( pcmd_next->attr.sch_skip.pass_sp == dep_sp );
	  assert( pcmd_next != pC );
#endif // CHK_STRICT_CONSISTENCY
	  if( pC->cmd == ARS_SCHEDULED_SKIP )
	    r = (pcmd_next->jid == pC->jid);
	  else
	    r = 0;
	  break;
	case ARS_CMD_NOP:
#if 0 // ***** for debugging.
	  r = 1;
	  break;
#else
	  assert( FALSE );
#endif
	default:
	  assert( FALSE );
	}
	assert( r > -1 );
      }
      if( !r )
	break;
      pcmd_next = pcmd_next->ln.sp_sch.pNext;
    }
  }
  return ((r < 0) ? (r * -1) : r);
}

static int ars_chk_dstschedule ( SCHEDULE_AT_SP sch_dst[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC_dst, SCHEDULED_COMMAND_C_PTR pC_lok ) { // well tested, 2025/01/12
  assert( sch_dst );
  assert( pC_dst );
  assert( ! pC_dst->checked );
  assert( (pC_dst->cmd == ARS_SCHEDULED_ARRIVAL) || (pC_dst->cmd == ARS_SCHEDULED_SKIP) );
  int r = -1;
  
  STOPPING_POINT_CODE dst_sp = SP_NONSENS;
  switch( pC_dst->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    dst_sp = pC_dst->attr.sch_arriv.arr_sp;
    break;
  case ARS_SCHEDULED_SKIP:
    dst_sp = pC_dst->attr.sch_skip.pass_sp;
    break;
  default:
    assert( FALSE );
  }
  assert( dst_sp != SP_NONSENS );
  {
    const int num_cmds = sch_dst[dst_sp].num_events;
    SCHEDULED_COMMAND_C_PTR pcmd_next = sch_dst[dst_sp].pNext;
    BOOL next_arrival = FALSE;
    BOOL judged = FALSE;
    while( pcmd_next ) {
      assert( pcmd_next );
      assert( (pcmd_next->cmd == ARS_SCHEDULED_ARRIVAL) || (pcmd_next->cmd == ARS_SCHEDULED_DEPT) || (pcmd_next->cmd == ARS_SCHEDULED_SKIP) );
      if( pcmd_next->checked ) {
	assert( pcmd_next != pC_dst );
	pcmd_next = pcmd_next->ln.sp_sch.pNext;
	continue;
      }
      if( next_arrival || (pcmd_next != pC_dst) ) {
	SCHEDULED_COMMAND_C_PTR p = NULL;
	if( next_arrival ) {
	  if( pcmd_next != pC_lok ) { // including the case of (pC_lok == NULL).
	    if( !pC_lok ) {
	      assert( pC_dst->cmd == ARS_SCHEDULED_ARRIVAL );
	      break;
	    } else {
	      assert( !pC_lok->checked );
	      assert( pC_lok->cmd == ARS_SCHEDULED_DEPT );
	      p = is_fellow( pcmd_next, pC_lok, num_cmds );
	      if( p )
		pcmd_next = pC_lok;
	      else {
		pcmd_next = pcmd_next->ln.sp_sch.pNext;
		if( pcmd_next->checked )
		  continue;
	      }
	    }
	  }
	} else {
	  p = is_fellow( pcmd_next, pC_dst, num_cmds );
	  if( p )
	    pcmd_next = pC_dst;
	}
      }
      assert( pcmd_next );
      assert( pC_dst );
      switch( pcmd_next->cmd ) {
      case ARS_SCHEDULED_ARRIVAL:
	if( ! next_arrival ) {
	  assert( r <= 0 );
	  r = 0;
	  if( (pC_dst->cmd == ARS_SCHEDULED_ARRIVAL) && (pcmd_next->jid == pC_dst->jid ) ) {
	    next_arrival = TRUE;
	    r = 1;
	    break;
	  }
	} else {
	  assert( r );
	  r = 0;
	}
	break;
      case ARS_SCHEDULED_DEPT:
	if( next_arrival ) {
	  assert( pC_dst->cmd == ARS_SCHEDULED_ARRIVAL );
	  assert( r );
	  assert( pC_lok );
	  if( pC_lok->cmd == ARS_SCHEDULED_DEPT )
	    r = (pcmd_next->jid == pC_lok->jid);
	  else
	    r = 0;
	  judged = TRUE;
	} else {
	  assert( r <= 0 );
	  r = 0;
	}
	break;
      case ARS_SCHEDULED_SKIP:
	if( ! next_arrival ) {
	  assert( r <= 0 );
	  r = 0;
	  if( pC_dst->cmd == ARS_SCHEDULED_SKIP )
	    r = (pcmd_next->jid == pC_dst->jid);
	  judged = TRUE;
	} else {
	  assert( r );
	  r = 0;
	}
	break;
      case ARS_CMD_NOP:
#if 0 // ***** for debugging.
	pcmd_next = pcmd_next->ln.sp_sch.pNext;
	continue;
#else
	assert( FALSE );
#endif
      default:
	assert( FALSE );
      }
      assert( r > -1 );
      if( (r == 0) || judged )
	break;
    }
  }
  return ((r < 0) ? (r * -1) : r);
}

static BOOL pick_depcmd ( ARS_REASONS *pres, SCHEDULED_COMMAND_PTR *ppC_dep, SCHEDULED_COMMAND_PTR pC_roset ) {
  assert( pres );
  assert( ppC_dep );
  assert( pC_roset );
  assert( pC_roset->cmd == ARS_SCHEDULED_ROUTESET );  
  BOOL r = FALSE;
  
  SCHEDULED_COMMAND_PTR p = pC_roset->ln.journey.planned.pNext;
 next_cmd:
  if( p ) {
    switch( p->cmd ) {
    case ARS_SCHEDULED_ROUTESET:
      // pC_roset maybe problematic, for successive commands of ARS_SCHEDULED_ROUTESET should be ARS_SCHEDULED_DEPT/ARS_SCHEDULED_SKIP or ARS_SCHEDULED_ROUTEREL.
      *pres = ARS_INCONSISTENT_ROUTESET;
      break;
    case ARS_SCHEDULED_ROUTEREL:
      *ppC_dep = NULL;
      r = TRUE;
      break;
    case ARS_SCHEDULED_ARRIVAL:      
      *pres = ARS_INCONSISTENT_ROUTESET; // see the above case of ARS_SCHEDULED_ROUTESET.
      break;
    case ARS_SCHEDULED_DEPT:
      *ppC_dep = p;
      r = TRUE;
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL, is missing.
	*pres = ARS_MISSING_ROUTEREL;
      } else {
	if( p->cmd != ARS_SCHEDULED_ROUTEREL )
	  // pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL.
	  *pres =  ARS_INCONSISTENT_DEPT;
      }
      break;
    case ARS_SCHEDULED_SKIP:
      *ppC_dep = p;
      r = TRUE;
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL, is missing.
	*pres = ARS_MISSING_ROUTEREL;
      } else {
	if( p->cmd != ARS_SCHEDULED_ROUTEREL )
	  // pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL.
	  *pres =  ARS_INCONSISTENT_SKIP;
      }
      break;
    case END_OF_SCHEDULED_CMDS:
      *pres = ARS_INCONSISTENT_ROUTESET; // see the above case of ARS_SCHEDULED_ROUTESET.
      break;
    case ARS_CMD_NOP:
#if 0 // ***** for debugging.
      p = p->ln.journey.planned.pNext;
      goto next_cmd;
#else
      assert( FALSE );
#endif
    }
  } else {
    // pC_roset maybe problematic, for successive commands should be ARS_SCHEDULED_DEPT/ARS_SCHEDULED_SKIP or ARS_SCHEDULED_ROUTEREL (or both), are missing.
    *pres = ARS_MISSING_ROUTEREL;
  }
  return r;
}

static BOOL pick_dstcmd ( ARS_REASONS *pres, SCHEDULED_COMMAND_PTR *ppC_dst, SCHEDULED_COMMAND_PTR *ppC_lok, SCHEDULED_COMMAND_PTR pC_roset ) {
  assert( pres );
  assert( ppC_dst );
  assert( ppC_lok );
  assert( pC_roset );
  assert( (pC_roset->cmd == ARS_SCHEDULED_ROUTESET) && (! pC_roset->attr.sch_roset.is_dept_route) );
  assert( pC_roset->attr.sch_roset.proute_prof );
  assert( pC_roset->attr.sch_roset.route_id == (pC_roset->attr.sch_roset.proute_prof)->id );
  BOOL r = FALSE;
  
  SCHEDULED_COMMAND_PTR p = pC_roset;  
  *ppC_dst = NULL;
  *ppC_lok = NULL;
  while( p && !r ) {
    switch( p->cmd ) {
    case ARS_SCHEDULED_ROUTESET:
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, for successive commands should be ARS_SCHEDULED_DEPT/ARS_SCHEDULED_SKIP or ARS_SCHEDULED_ROUTEREL (or both), are missing.
	*pres = ARS_MISSING_ROUTEREL;
	return FALSE;
      } else {
	if( !((p->cmd == ARS_SCHEDULED_DEPT) || (p->cmd == ARS_SCHEDULED_SKIP) || (p->cmd == ARS_SCHEDULED_ROUTEREL)) ) {
	  // pC_roset maybe problematic, for successive commands of ARS_SCHEDULED_ROUTESET should be ARS_SCHEDULED_DEPT/ARS_SCHEDULED_SKIP or ARS_SCHEDULED_ROUTEREL.
	  *pres = ARS_INCONSISTENT_ROUTESET;
	  return FALSE;
	}
      }
      assert( p );
      assert( (p->cmd == ARS_SCHEDULED_DEPT) || (p->cmd == ARS_SCHEDULED_SKIP) || (p->cmd == ARS_SCHEDULED_ROUTEREL) );
      break;
    case ARS_SCHEDULED_ROUTEREL:
      p = p->ln.journey.planned.pNext;
      break;
    case ARS_SCHEDULED_ARRIVAL:
      *ppC_dst = p;
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, any journey must have END_OF_SCHEDULED_CMDS on its dead-end.
	*pres = ARS_NO_DEADEND_CMD;
	r = TRUE;
      } else {
	if( p->cmd == END_OF_SCHEDULED_CMDS ) {
	  r = TRUE;
	} else if( p->cmd == ARS_SCHEDULED_DEPT ) {
	  continue;
	} else if( p->cmd == ARS_SCHEDULED_ROUTESET ) {
	  continue;
	} else {
	  // pC_roset maybe problematic, for successive commands of ARS_SCHEDULED_ARRIVAL cannot be the cmds excepting the ones above.
	  *pres = ARS_INCONSISTENT_ARRIVAL;
	  return FALSE;
	}
      }
      break;
    case ARS_SCHEDULED_DEPT:
      if( *ppC_dst ) {
	*ppC_lok = p;
	r = TRUE;
      }
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL, is missing.
	*pres = ARS_MISSING_ROUTEREL;
	if( ! *ppC_dst )
	  return FALSE;
      } else {
	if( p->cmd != ARS_SCHEDULED_ROUTEREL ) {
	  // pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL.
	  *pres =  ARS_INCONSISTENT_DEPT;
	  if( ! *ppC_dst )
	    return FALSE;
	}
      }
      break;
    case ARS_SCHEDULED_SKIP:
      if( p != pC_roset->ln.journey.planned.pNext ) {
	*ppC_dst = p;
	r = TRUE;
      }
      p = p->ln.journey.planned.pNext;
      if( !p ) {
	// pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL, is missing.
	*pres = ARS_MISSING_ROUTEREL;
	if( ! *ppC_dst )
	  return FALSE;
      } else {
	if( p->cmd != ARS_SCHEDULED_ROUTEREL ) {
	  // pC_roset maybe problematic, for successive command should be ARS_SCHEDULED_ROUTEREL.
	  *pres =  ARS_INCONSISTENT_SKIP;
	  if( ! *ppC_dst )
	    return FALSE;
	}
      }
      break;
    case END_OF_SCHEDULED_CMDS:
      assert( ! p->ln.journey.planned.pNext );
      assert( !r );
      break;
    case ARS_CMD_NOP:
#if 0 // ***** for debugging.
      p = p->ln.journey.planned.pNext;
      break;
#else
      assert( FALSE );
#endif
    }
  }
  assert( p );
  
  return r;
}

static SCHEDULED_COMMAND_PTR make_it_past ( JOURNEY_PTR pJ, SCHEDULED_COMMAND_PTR pC ) { // well tested, 2025/01/21
  assert( pJ );
  assert( pC );
  assert( pC->checked );
  
  if( ! pJ->past_commands.phead ) {
    assert( ! pJ->past_commands.plast );
    pC->ln.journey.past.pPrev = NULL;
    pJ->past_commands.phead = pC;
    pJ->past_commands.plast = pJ->past_commands.phead;    
  } else {
    assert( pJ->past_commands.plast );
    pC->ln.journey.past.pPrev = pJ->past_commands.plast;
    (pJ->past_commands.plast)->ln.journey.past.pNext = pC;
    pJ->past_commands.plast = pC;
  }
  pC->ln.journey.past.pNext = NULL;
  
  if( pJ->scheduled_commands.pNext == pC )
    pJ->scheduled_commands.pNext = pC->ln.journey.planned.pNext;  
  return ( pJ->scheduled_commands.pNext );
}

ARS_REASONS ars_routectl_on_journey ( TIMETABLE_PTR pTT, JOURNEY_PTR pJ ) {
  assert( pTT );
  assert( pJ );
  assert( pJ->ptrain_ctrl );
  ARS_REASONS r = END_OF_ARS_REASONS;
  
  TINY_TRAIN_STATE_PTR pT = NULL;
  pT = pJ->ptrain_ctrl;
  if( pT ) {
    SCHEDULED_COMMAND_PTR pC = NULL;
    pC = fetch_routeset_cmd( pJ );
    if( pC ) {
      assert( pC );
      assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
      assert( ! pC->checked );
      if( pC->cmd == ARS_SCHEDULED_ROUTESET ) {
	assert( whats_kind_of_il_sym( pC->attr.sch_roset.route_id ) == _ROUTE );
	ROUTE_C_PTR pR = NULL;
	pR = conslt_route_prof( pC->attr.sch_roset.route_id );
	assert( pR );
	if( pR ) {
	  assert( pR->kind == _ROUTE );
	  assert( (pR->route_kind < END_OF_ROUTE_KINDS) && (pR->route_kind != EMERGE_ROUTE) );
	  assert( pR->ars_ctrl.app );
	  if( pR->ars_ctrl.app ){
	    assert( pT );
	    CBTC_BLOCK_C_PTR pB = NULL;
	    int cond = -1;
	    cond = ars_chk_hit_trgsection( pR, &pB, pT, -1 );
	    if( cond <= 0 ) {
	      if( cond < 0 )
		r = ARS_MUTEX_BLOCKED;
	      else {
		assert( cond == 0 );
		r = ARS_NO_TRIGGERED;
	      }
	    } else if( cond == 1 ) {
	      r = ARS_FOUND_TRAINS_AHEAD;
	    } else {
	      assert( cond >= 2 );
	      char *route_R = NULL;
	      char raw[CBI_STAT_IDENT_LEN + 1];
	      OC_ID oc_id;
	      CBI_STAT_KIND kind;
	      int stat = -1;
	      strncpy( raw, cnv2str_il_sym( pC->attr.sch_roset.route_id ), CBI_STAT_IDENT_LEN );
	      route_R = mangl2_Sxxxy_Sxxxy_R( raw );
	      assert( route_R );
	      stat = conslt_il_state( &oc_id, &kind, route_R );
	      if( stat <= 0 ) {
		if( stat < 0 ) {
		  switch( stat ) {
		  case -1:
		    r = ARS_MUTEX_BLOCKED;
		    break;
		  case -2:
		    r = ARS_FOUND_ERROROUS_INTERLOCKDEF;
		    break;
		  default:
		    assert( FALSE );
		  }	
		} else {
		  assert( stat == 0 );
		  cond = ars_chk_trackcirc( pR );
		  if( cond <= 0 ) {
		    if( cond < 0 )
		      r = ARS_MUTEX_BLOCKED;
		    else {
		      assert( cond == 0 );
		      r = ARS_CTRL_TRACKS_DROP;
		    }
		  } else {
		    assert( cond > 0 );
		    assert( pC );
		    const int hour_to_set = pC->attr.sch_roset.dept_time.hour;
		    const int minute_to_set = pC->attr.sch_roset.dept_time.minute;
		    const int second_to_set = pC->attr.sch_roset.dept_time.second;
		    assert( (hour_to_set >= 0) && (hour_to_set < 24) );
		    assert( (minute_to_set >= 0) && (minute_to_set < 60) );
		    assert( (second_to_set >= 0) && (second_to_set < 60) );
		    if( pC->attr.sch_roset.is_dept_route ) {
		      goto is_the_time_now;
		    } else {
		      cond = ars_chk_routelok( pR );
		      if( cond <= 0 ) {
			if( cond < 0 )
			  r = ARS_MUTEX_BLOCKED;
			else {
			  assert( cond == 0 );
			  r = ARS_CTRL_TRACKS_ROUTELOCKED;
			}
		      } else {
			assert( cond > 0 );
		      is_the_time_now:
			cond = ars_chk_trgtime( OFFSET_TO_ROUTESET, NULL, hour_to_set, minute_to_set, second_to_set );
			if( cond <= 0 ) {
			  if( cond < 0 )
			    r = ARS_MUTEX_BLOCKED;
			  else {
			    assert( cond == 0 );
			    r = ARS_WAITING_ROUTESET_TIME;
			  }
			} else {
			  assert( cond > 0 );
			  assert( pTT );
			  assert( pC );
			  //SCHEDULED_COMMAND_PTR pC_dep = pC->ln.journey.pNext;
			  //if( pC_dep && (pC_dep->cmd == ARS_SCHEDULED_DEPT) && (pC_dep->attr.sch_dept.dept_sp == pR->ars_ctrl.trip_info.dep.sp) ) {
			  ARS_REASONS res_dep = END_OF_ARS_REASONS;
			  SCHEDULED_COMMAND_PTR pC_dep = NULL;
			  if( pick_depcmd( &res_dep, &pC_dep, pC ) ) {
			    //printf( "result of pick_depcmd: %s\n", (res_dep != END_OF_ARS_REASONS ? cnv2str_ars_reasons[res_dep] : "no_claims") ); // *****
			    //assert( FALSE ); // *****
			    if( !pC_dep ) {
			      assert( ! pC->attr.sch_roset.is_dept_route );
			      goto chk_dst_cond;
			    } else {
			      cond = ars_chk_depschedule( pTT->sp_schedule, pC_dep );
			      if( cond <= 0 ) {
				if( cond < 0 )
				  r = ARS_MUTEX_BLOCKED;
				else {
				  assert( cond == 0 );
				  r = ARS_WAITING_PRED_DEPTRAINS;
				}
			      } else {
				assert( cond > 0 );
			      chk_dst_cond:
				assert( pTT );			    
				ARS_REASONS res_dst = END_OF_ARS_REASONS;
				if( pC->attr.sch_roset.is_dept_route ) {
				  goto ready_on_fire;
				} else {
				  SCHEDULED_COMMAND_PTR pC_dst = NULL;
				  SCHEDULED_COMMAND_PTR pC_lok = NULL;
				  pC->attr.sch_roset.proute_prof = pR; // *****
				  if( pick_dstcmd( &res_dst, &pC_dst, &pC_lok, pC ) ) {
				    //printf( "result of pick_dstcmd: %s\n", (res_dst != END_OF_ARS_REASONS ? cnv2str_ars_reasons[res_dst] : "no_claims") ); // *****
				    cond = ars_chk_dstschedule( pTT->sp_schedule, pC_dst, pC_lok );
				    if( cond <= 0 ) {
				      if( cond < 0 )
					r = ARS_MUTEX_BLOCKED;
				      else {
					assert( cond == 0 );
					r = ARS_FOUND_PRED_ARRIVDEP_AT_DST;
				      }
				    } else {
				    ready_on_fire:
				      //printf( "cmd of pC_dst: %d\n", pC_dst->cmd );
				      //assert( FALSE ); // *****
				      assert( pC );
				      assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
				      char *P_route = NULL;
				      strncpy( raw, cnv2str_il_sym( pC->attr.sch_roset.route_id ), CBI_STAT_IDENT_LEN );
				      P_route = mangl2_P_Sxxxy_Sxxxy( raw );
				      assert( P_route );
				      engage_il_ctrl( &oc_id, &kind, P_route );
				      r = (res_dst != END_OF_ARS_REASONS) ? res_dst : ARS_NOW_ROUTE_CONTROLLING;
				    }
				  } else {
				    assert( res_dst != END_OF_ARS_REASONS );
				    //printf( "result of pick_dstcmd: %s\n", (res_dst != END_OF_ARS_REASONS ? cnv2str_ars_reasons[res_dst] : "no_claims") ); // *****
				    //assert( FALSE ); // *****
				    r = res_dst;
				  }
				}
			      }
			    }
			  } else {
			    assert( res_dep != END_OF_ARS_REASONS );
			    //printf( "result of pick_depcmd: %s\n", (res_dep != END_OF_ARS_REASONS ? cnv2str_ars_reasons[res_dep] : "no_claims") ); // *****
			    //assert( FALSE ); // *****
			    r = res_dep;
			  }
			}
		      }
		    }
		  }
		}
	      } else {
		assert( stat > 0 );
		assert( ! pC->checked );
		char *P_route = NULL;
		pC->checked = TRUE;
		make_it_past( pJ, pC );
		strncpy( raw, cnv2str_il_sym( pC->attr.sch_roset.route_id ), CBI_STAT_IDENT_LEN );
		P_route = mangl2_P_Sxxxy_Sxxxy( raw );
		ungage_il_ctrl( &oc_id, &kind, P_route );	    
		r = ARS_ROUTE_CONTROLLED_NORMALLY;
		//printf( "cmd of pC: %d\n", pC->cmd ); // *****
		//printf( "jid of pJ: %d\n", pJ->jid ); // *****
		//printf( "cmd of pJ->scheduled_commands.pNext: %d\n", pJ->scheduled_commands.pNext->cmd ); // *****
		//assert( FALSE ); // *****   
	      }
	    }
	  } else
	    r = ARS_ILLEGAL_CMD_ROUTESET;
	} else
	  r = ARS_WRONG_CMD_ATTRIB;
      } else
	r = ARS_NO_ROUTESET_CMD;
    } else
      r = ARS_NO_SCHEDULED_CMDS;
  } else
    r = ARS_NO_RAKE_ASGNED;
  assert( r != END_OF_ARS_REASONS );
  return r;
}

static ARS_ASSOC_TIME_PTR timestamp ( ARS_ASSOC_TIME_PTR pstamp ) {
  assert( pstamp );
  ARS_ASSOC_TIME_PTR r = NULL;
  
  struct tm *pT_crnt = NULL;
  time_t crnt_time = 0;
  crnt_time = time( NULL );
  pT_crnt = localtime( &crnt_time );
  if( pT_crnt ) {
    pstamp->hour = pT_crnt->tm_hour;
    pstamp->minute = pT_crnt->tm_min;
    pstamp->second = pT_crnt->tm_sec;
    pstamp->year = pT_crnt->tm_year;
    pstamp->month = pT_crnt->tm_mon;
    pstamp->day = pT_crnt->tm_mday;
    r = pstamp;
  }
  return r;
}

SCHEDULED_COMMAND_PTR ars_schcmd_ack ( ARS_REASONS *pres, JOURNEY_PTR pJ, ARS_EVENT_ON_SP *pev_sp ) {
  assert( pres );
  assert( pJ );
  assert( pev_sp );
  SCHEDULED_COMMAND_PTR r = NULL;
  
  TINY_TRAIN_STATE_PTR pT = NULL;
  pT = pJ->ptrain_ctrl;
  if( pT ) {
    SCHEDULED_COMMAND_PTR pC = NULL;
    pC = pJ->scheduled_commands.pNext;
    while( pC ) {
      assert( pC );
      if( ! pC->checked ) {
	//printf( "cmd of pC: %d\n", pC->cmd ); // *****
	//assert( FALSE ); // *****
	r = pC;
	switch ( pC->cmd ) {
	case ARS_SCHEDULED_ROUTESET:
	  assert( pC );
	  assert( !pC->checked );
	  assert( pC == r );
	  break;
	case ARS_SCHEDULED_ROUTEREL:
	  assert( pJ );
	  assert( pC );
	  {
	    ROUTE_C_PTR pR = NULL;
	    pR = conslt_route_prof( pC->attr.sch_rorel.route_id );
	    assert( pR );
	    assert( pR->kind == _ROUTE );
	    assert( (pR->route_kind < END_OF_ROUTE_KINDS) && (pR->route_kind != EMERGE_ROUTE) );
	    assert( pR->ars_ctrl.app );
	    if( pR->ars_ctrl.ctrl_tracks.num_ahead_tracks > 0 ) {
	      TRACK_C_PTR pahead_trk = pR->ars_ctrl.ctrl_tracks.pahead_trks[0];
	      assert( pahead_trk );
	      OC_ID oc_id;
	      CBI_STAT_KIND kind;
	      int stat = -1;
	      stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(pahead_trk->id) );
	      if( stat == 0 ) {
		timestamp( &pC->attr.sch_rorel.dept_time );
	      acc_and_chk_rorel:
		pC->checked = TRUE;
		make_it_past( pJ, pC );
		r = pC->ln.journey.planned.pNext;
	      }
	    } else {
	      SCHEDULED_COMMAND_PTR p = pJ->past_commands.plast;
	    try_again:
	      if( p ) {
		assert( pJ->past_commands.phead );
		assert( p );
		switch( p->cmd ) {
		case ARS_SCHEDULED_DEPT:
		case ARS_SCHEDULED_SKIP:
		  goto acc_and_chk_rorel;
		case ARS_SCHEDULED_ROUTESET:
		case ARS_SCHEDULED_ROUTEREL:
		case ARS_SCHEDULED_ARRIVAL:
		case END_OF_SCHEDULED_CMDS:
		  *pres = ARS_INCONSISTENT_ROUTEREL;
		  break;
		case ARS_CMD_NOP:
#if 0 // ***** for debugging.
		  //*pres = ARS_INCONSISTENT_ROUTEREL;
		  p = p->ln.journey.past.pPrev;
		  goto try_again;
#else
		  // fall thru.
#endif
		default:
		  assert( FALSE );
		}
	      } else
		*pres = ARS_INCONSISTENT_ROUTEREL;
	    }
	  }
	  break;
	case ARS_SCHEDULED_ARRIVAL:
	  assert( pC );
	  assert( pT );
	  //printf( "cmd of pC: %d\n", pC->cmd ); // *****
	  //assert( FALSE ); // *****
	  if( pev_sp->sp != SP_NONSENS ) {
	    if( pev_sp->sp == pC->attr.sch_arriv.arr_sp ) {
	      if( pev_sp->situation == ARS_DOCK_DETECTED ) {
		timestamp( &pC->attr.sch_arriv.arr_time );
		pC->checked = TRUE;
		make_it_past( pJ, pC );
		r = pC->ln.journey.planned.pNext;
	      } else {
		if( pev_sp->situation == ARS_SKIP_DETECTED ) {
		  SCHEDULED_COMMAND_PTR pC_next = NULL;
		  timestamp( &pC->attr.sch_arriv.arr_time );
		  pC->checked = TRUE;
		  make_it_past( pJ, pC );
		  r = pC->ln.journey.planned.pNext;
		  pC_next = pC->ln.journey.planned.pNext;
		  if( pC_next ) {
		    if( pC_next->cmd == ARS_SCHEDULED_ROUTESET ) {
		      if( pC_next->checked ) {
			pC_next = pC_next->ln.journey.planned.pNext;
			if( pC_next->cmd == ARS_SCHEDULED_DEPT ) {
			  pC = pC_next;
			  goto acc_with_dept;
			} else {
			  if( pC_next->cmd != ARS_SCHEDULED_ROUTEREL )
			    *pres = ARS_INCONSISTENT_ROUTESET;
			}
		      }
		    } else
		      // we assume the successor cmd of ARS_SCHEDULED_ARRIVAL is only ARS_SCHEDULED_ROUTESET, lacks the consistency with pick_dstcmd().
		      if( pC_next->cmd != END_OF_SCHEDULED_CMDS )
			*pres = ARS_INCONSISTENT_ARRIVAL;
		  } else
		    *pres = ARS_NO_DEADEND_CMD;
		}
	      }
	    }
	  }
	  break;
	case ARS_SCHEDULED_DEPT:
	  assert( pC );
	  assert( pT );
	acc_with_dept:
	  if( pev_sp->sp != SP_NONSENS ) {
	    if( pev_sp->sp == pC->attr.sch_dept.dep_sp ) {
	      if( (pev_sp->situation == ARS_LEAVE_DETECTED) || (pev_sp->situation == ARS_SKIP_DETECTED) ) {
		timestamp( &pC->attr.sch_dept.dep_time );
		pC->checked = TRUE;
		make_it_past( pJ, pC );
		if( pev_sp->situation == ARS_LEAVE_DETECTED )
		  //pT->ATO_dept_cmd = FALSE; // *****
		  change_train_state_ATO_dept_cmd( pT, FALSE, FALSE );
		r = pC->ln.journey.planned.pNext;
	      }
	    }
	  }
	  break;
	case ARS_SCHEDULED_SKIP:
	  assert( pC );
	  assert( pT );
	  if( pev_sp->sp != SP_NONSENS ) {
	    if( pev_sp->sp == pC->attr.sch_skip.pass_sp ) {
	      if( (pev_sp->situation == ARS_SKIP_DETECTED) || (pev_sp->situation == ARS_LEAVE_DETECTED) ) {
		timestamp( &pC->attr.sch_skip.pass_time );
		pC->checked = TRUE;
		make_it_past( pJ, pC );
		r = pC->ln.journey.planned.pNext;
	      }
	    }
	  }
	  break;
	case END_OF_SCHEDULED_CMDS:
	  pJ->scheduled_commands.pNext = pC;
	  return pC;
	case ARS_CMD_NOP:
#if 0 // ***** for debugging.
	  r = pC->ln.journey.planned.pNext;
	  break;
#else
	  assert( FALSE );
#endif
	default:
	  assert( FALSE );
	}
      } else {
	assert( pC->checked );
	r = pC->ln.journey.planned.pNext;
      }
      if( pC == r )
	break;
      pC = r;
    }
    pJ->scheduled_commands.pNext = r;
  }
  r = pJ->scheduled_commands.pNext;
  return r;
}

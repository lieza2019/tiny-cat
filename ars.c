#include <stdio.h>
#include <string.h>
#include <time.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "ars.h"
#include "surveill.h"
#include "timetable.h"

const char *cnv2str_ars_reasons[] = {
  "ARS_NO_ROUTESET_CMD",
  "ARS_NO_TRIGGERED",
  "ARS_FOUND_TRAINS_AHEAD",
  "ARS_CTRL_TRACKS_ROUTELOCKED",
  "ARS_CTRL_TRACKS_DROP",
  "ARS_WAITING_ROUTESET_TIME",
  "ARS_PRED_DEPTRAINS_FOUND",
  "ARS_WAITING_PRED_DEPTRAINS_AT_DST",
  "ARS_ROUTE_CONTROLLED_NORMALLY",
  "ARS_MUTEX_BLOCKED",
  "ARS_NOMORE_SCHEDULED_CMDS",
  "ARS_ILLEGAL_CMD_ROUTESET",
  "ARS_ILLEGAL_CMD_ROUTEREL",
  "ARS_ILLEGAL_CMD_ARRIV",
  "ARS_ILLEGAL_CMD_DEPT",
  "ARS_ILLEGAL_CMD_SKIP",
  NULL,
};

SYSTEM_PARAMS tiny_system_params = {
  DEFALUT_ROUTESET_OFFSET,
  DEFAULT_DEPARTURE_OFFSET
};

static SCHEDULED_COMMAND sch_cmd_nodes[SCHEDULED_COMMANDS_NODEBUF_SIZE];
static int frontier;

SCHEDULED_COMMAND_PTR sch_cmd_newnode( void ) {
  SCHEDULED_COMMAND_PTR r = NULL;
  if( frontier < SCHEDULED_COMMANDS_NODEBUF_SIZE )
    r = &sch_cmd_nodes[++frontier];
  else
    errorF( "%s", "nodes exhausted, to create a scheduled command.\n" );
  return r;
};

STOPPING_POINT_CODE ars_judge_arriv_dept_skip ( ARS_EVENT_ON_SP_PTR pdetects, TINY_TRAIN_STATE_PTR pT ) {
  assert( pdetects );
  assert( pT );
  STOPPING_POINT_CODE hit_sp = SP_NONSENS;
  
  pdetects->sp = SP_NONSENS;
  pdetects->detail = ARS_DETECTS_NONE;
  
  if( pT->stop_detected == SP_NONSENS ) {
    hit_sp = detect_train_docked( DOCK_DETECT_MAJOR, pT );
    if( hit_sp != SP_NONSENS ) {
      pT->stop_detected = hit_sp;
      pdetects->sp = hit_sp;
      pdetects->detail = ARS_DOCK_DETECTED;
    }
  }
  hit_sp = SP_NONSENS;
  hit_sp = detect_train_docked( DOCK_DETECT_MINOR, pT );
  if( hit_sp != SP_NONSENS ) {
    pT->stop_detected = hit_sp;
    pdetects->sp = hit_sp;
    pdetects->detail = ARS_DOCK_DETECTED;
  }
  
  hit_sp = SP_NONSENS;
  hit_sp = detect_train_skip( pT );
  if( hit_sp != SP_NONSENS ) {
    assert( pT->stop_detected == SP_NONSENS );
    pdetects->sp = hit_sp;
    pdetects->detail = ARS_SKIP_DETECTED;
  }
  
  hit_sp = SP_NONSENS;
  hit_sp = detect_train_leave( pT );
  if( hit_sp != SP_NONSENS ) {
    pT->stop_detected = SP_NONSENS;
    pdetects->sp = hit_sp;
    pdetects->detail = ARS_LEAVE_DETECTED;
  }
  
  return pdetects->sp;
}

static int ars_chk_trgtime ( OFFSET_TIME_TO_FIRE offset_kind, int hour, int minute, int second ) {
  assert( (offset_kind >= 0) && (offset_kind < END_OF_OFFSET_TIMES) );
  assert( (hour >= 0) && (hour < 24) );
  assert( (minute >= 0) && (minute < 60) );
  assert( (second >= 0) && (second < 60) );
  
  int r = -1;
  struct tm *pT_crnt = NULL;
  time_t crnt_time = 0;
  
  crnt_time = time( NULL );
  pT_crnt = localtime( &crnt_time );
#if 0 // ***** for debugging.
  tzset();
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
      default:
	assert( FALSE );
	/* fall thru. */
      }
      if( pT_trg ) {
#if 0 // ***** for debugging.
	tzset();
	printf( "trig_localtime: yyyy/mm/dd, HH:MM:SS => %d/%02d/%02d, %02d:%02d:%02d\n",
		(pT_trg->tm_year + 1900), (pT_trg->tm_mon + 1), pT_trg->tm_mday, pT_trg->tm_hour, pT_trg->tm_min, pT_trg->tm_sec );
	printf( "judgement: %s\n", (r ? "TRIGGERED!" : "STANDING-BY") );
#endif
	r = (difftime( trg_time, crnt_local ) < (double)0) ? 1 : 0;
      }
    }
  }
  return r;
}

static int ars_chk_cond_routelok ( ROUTE_C_PTR proute ) {
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
	  if( stat == 0 )
	    assert( kind == _TLSR );
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
	  if( stat == 0 )
	    assert( kind == _TRSR );
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
	  if( stat == 0 )
	    assert( kind == _sTLSR );
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
	  if( stat == 0 )
	    assert( kind == _sTRSR );
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
	  if( stat == 0 )
	    assert( kind == _eTLSR );
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
	  if( stat == 0 )
	    assert( kind == _eTRSR );
	  r = stat;
	  break;
	}
	assert( stat >= 1 );
	assert( kind == _eTRSR );
      }
      r = stat;
    }
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_lok) ? (r == 1) : (r <= 0) );  
  return r;
}

static int ars_chk_cond_trackcirc ( ROUTE_C_PTR proute ) {
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
      if( stat == 0 )
	assert( kind == _TRACK );
      r = stat;
      break;
    }
    assert( stat >= 1 );
    assert( kind == _TRACK );
    r = stat;
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_occ) ? (r == 1) : (r <= 0) );
  return r;
}

static SCHEDULED_COMMAND_PTR fetch_routeset_cmd1 ( JOURNEY_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR r = NULL;
  
  SCHEDULED_COMMAND_PTR *ppC = NULL;
  ppC = &pJ->scheduled_commands.pNext;
  assert( ppC );
  while( *ppC ) {
    assert( ppC );
    assert( *ppC );
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
    ppC = &(*ppC)->ln.journey.pNext;
  }
  assert( ppC );
  return r;
}

static BOOL no_trains_ahead ( ROUTE_C_PTR proute, int ahead_blk ) {
  assert( proute );
  assert( (ahead_blk > 0) && (ahead_blk <= proute->ars_ctrl.trg_sect.num_blocks) );
  BOOL r = TRUE;
  int i;
  
  for( i = ahead_blk; i < proute->ars_ctrl.trg_sect.num_blocks; i++ ) {
    if( read_residents_CBTC_BLOCK( proute->ars_ctrl.trg_sect.ptrg_blks[i] ) ||
	read_edge_of_residents_CBTC_BLOCK( proute->ars_ctrl.trg_sect.ptrg_blks[i] ) ) {
      r = FALSE;
      break;
    }
  }
  return r;
}

static int ars_chk_hit_trgsection ( ROUTE_C_PTR proute, TINY_TRAIN_STATE_PTR ptrain_ctrl) {
  assert( proute );
  assert( proute->ars_ctrl.app );
  assert( ptrain_ctrl );
  int r = -1;
  int i;
  
  r = 0;
  for( i = 0; i < proute->ars_ctrl.trg_sect.num_blocks; i++ ) {
    TINY_TRAIN_STATE_PTR pT = NULL;
    pT = read_residents_CBTC_BLOCK( proute->ars_ctrl.trg_sect.ptrg_blks[i] );
    while( pT ) {
      if( pT == ptrain_ctrl ) {
	r = 1;
	if( !(pT->occupancy.pNext || read_edge_of_residents_CBTC_BLOCK( proute->ars_ctrl.trg_sect.ptrg_blks[i] )) ) {
	  if( no_trains_ahead( proute, (i + 1) ) )
	    r = 2;
	}
	break;
      }
      pT = pT->occupancy.pNext;
    }
  }
  return r;
}

static int ars_chk_dstschedule ( SCHEDULE_AT_SP sch_dst[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC ) {
  assert( sch_dst );
  assert( pC );
  assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
  assert( whats_kind_of_il_sym( pC->attr.sch_roset.route_id ) == _ROUTE );
  int r = -1;
  
  ROUTE_C_PTR pR = NULL;
  pR = conslt_route_prof( pC->attr.sch_roset.route_id );
  assert( pR );
  assert( pR->kind == _ROUTE );
  assert( pR->ars_ctrl.app );
  assert( pR->ars_ctrl.trip_info.dst.pblk );
  if( pC->attr.sch_roset.is_dept_route ) {
    assert( pR->route_kind == DEP_ROUTE );
    r = 1;
  } else {
    assert( ! pC->attr.sch_roset.is_dept_route );
    assert( (pR->route_kind == ENT_ROUTE) || (pR->route_kind == SHUNT_ROUTE) );
    CBTC_BLOCK_C_PTR pdst_blk = pR->ars_ctrl.trip_info.dst.pblk;
    assert( pdst_blk );
    assert( pdst_blk->sp.has_sp );
    assert( (pdst_blk->sp.sp_code > 0) && (pdst_blk->sp.sp_code < END_OF_SPs) );
    SCHEDULED_COMMAND_C_PTR pdst_next_sch = sch_dst[pdst_blk->sp.sp_code].pNext;
    
    BOOL judged = FALSE;
    while( pdst_next_sch ) {
      assert( pdst_next_sch );
      assert( (pdst_next_sch->cmd == ARS_SCHEDULED_ARRIVAL) || (pdst_next_sch->cmd == ARS_SCHEDULED_DEPT) || (pdst_next_sch->cmd == ARS_SCHEDULED_SKIP) );
      switch( pdst_next_sch->cmd ) {
      case ARS_SCHEDULED_ARRIVAL:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdst_next_sch->attr.sch_arriv.arr_sp == pdst_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	if( pdst_next_sch->jid == pC->jid ) {
	  const DWELL_ID dw_seq_arriv = pdst_next_sch->attr.sch_arriv.dw_seq;
	  SCHEDULED_COMMAND_C_PTR pdst_next_next = pdst_next_sch->ln.sp_sch.pNext;
	  if( pdst_next_next ) {
	    r = 0;
	    do { 
	      assert( pdst_next_next );
	      assert( (pdst_next_next->cmd == ARS_SCHEDULED_ARRIVAL) || (pdst_next_next->cmd == ARS_SCHEDULED_DEPT) || (pdst_next_next->cmd == ARS_SCHEDULED_SKIP) );
	      if( pdst_next_next->cmd == ARS_SCHEDULED_DEPT ) {
#ifdef CHK_STRICT_CONSISTENCY
		assert( pdst_next_next->attr.sch_dept.dept_sp == pdst_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
		if( pdst_next_next->jid == pC->jid ) {
		  const DWELL_ID dw_seq_dept = pdst_next_next->attr.sch_dept.dw_seq;
		  if( dw_seq_dept == (dw_seq_arriv + 1) ) {
		    r = 1;	  
		    break;
		  }
		}
	      }
	      pdst_next_next = pdst_next_next->ln.sp_sch.pFellow;
	    } while( pdst_next_next );
	  } else
	    r = 1;
	  judged = TRUE;
	} else
	  r = 0;
	break;
      case ARS_SCHEDULED_DEPT:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdst_next_sch->attr.sch_dept.dept_sp == pdst_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	r = 0;
	break;
      case ARS_SCHEDULED_SKIP:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdst_next_sch->attr.sch_skip.ss_sp == pdst_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	if( pdst_next_sch->jid == pC->jid ) {
	  r = 1;
	  judged = TRUE;
	} else
	  r = 0;
	break;
      default:
	assert( FALSE );
      }
      if( judged )
	break;
      else
	pdst_next_sch = pdst_next_sch->ln.sp_sch.pFellow;
    }
  }
  return r;
}

static int ars_chk_depschedule ( SCHEDULE_AT_SP sch_dep[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC ) {
  assert( sch_dep );
  assert( pC );
  assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
  assert( whats_kind_of_il_sym( pC->attr.sch_roset.route_id ) == _ROUTE );
  int r = -1;
  
  ROUTE_C_PTR pR = NULL;
  pR = conslt_route_prof( pC->attr.sch_roset.route_id );
  assert( pR );
  assert( pR->kind == _ROUTE );
  assert( pR->ars_ctrl.app );
  assert( pR->ars_ctrl.trip_info.dep.pblk );
  assert( (pR->route_kind == DEP_ROUTE) || (pR->route_kind == ENT_ROUTE) || (pR->route_kind == SHUNT_ROUTE) );
  {
    CBTC_BLOCK_C_PTR pdep_blk = pR->ars_ctrl.trip_info.dep.pblk;
    assert( pdep_blk );
    assert( pdep_blk->sp.has_sp );
    assert( (pdep_blk->sp.sp_code > 0) && (pdep_blk->sp.sp_code < END_OF_SPs) );
    SCHEDULED_COMMAND_C_PTR pdep_next_sch = sch_dep[pdep_blk->sp.sp_code].pNext;
    
    BOOL judged = FALSE;
    while( pdep_next_sch ) {
      assert( pdep_next_sch );
      assert( (pdep_next_sch->cmd == ARS_SCHEDULED_ARRIVAL) || (pdep_next_sch->cmd == ARS_SCHEDULED_DEPT) || (pdep_next_sch->cmd == ARS_SCHEDULED_SKIP) );
      switch( pdep_next_sch->cmd ) {
      case ARS_SCHEDULED_ARRIVAL:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdep_next_sch->attr.sch_arriv.arr_sp == pdep_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	r = 0;
	break;
      case ARS_SCHEDULED_DEPT:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdep_next_sch->attr.sch_dept.dept_sp == pdep_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	if( pdep_next_sch->jid != pC->jid ) {
	  r = 1;
	  judged = TRUE;
	} else
	  r = 0;
	break;
      case ARS_SCHEDULED_SKIP:
#ifdef CHK_STRICT_CONSISTENCY
	assert( pdep_next_sch->attr.sch_skip.ss_sp == pdep_blk->sp.sp_code );
#endif // CHK_STRICT_CONSISTENCY
	if( pdep_next_sch->jid == pC->jid ) {
	  r = 1;
	  judged = TRUE;
	} else
	  r = 0;
	break;
      default:
	assert( FALSE );	
      }
      if( judged )
	break;
      else
	pdep_next_sch = pdep_next_sch->ln.sp_sch.pFellow;
    }
  }
  return r;
}

static SCHEDULED_COMMAND_PTR make_it_past ( JOURNEY_PTR pJ, SCHEDULED_COMMAND_PTR pC ) {
  assert( pJ );
  assert( pC );
  assert( pJ->scheduled_commands.pNext == pC );
  
  SCHEDULED_COMMAND_PTR *pp = NULL;
  pp = &pJ->past_commands;
  assert( pp );
  while( *pp ) {
    assert( *pp );
    pp = &(*pp)->ln.journey.pNext;
    assert( pp );
  }
  assert( pp );
  assert( ! *pp );
  *pp = pC;
  pJ->scheduled_commands.pNext = pC->ln.journey.pNext;
  pC->ln.journey.pNext = NULL;
  
  return ( pJ->scheduled_commands.pNext );
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

ARS_REASONS ars_ctrl_route_on_journey ( TIMETABLE_PTR pTT, JOURNEY_PTR pJ ) {
  assert( pTT );
  assert( pJ );
  assert( pJ->ptrain_ctrl );
  ARS_REASONS r = END_OF_ARS_REASONS;
  
  SCHEDULED_COMMAND_PTR pC = NULL;
  pC = fetch_routeset_cmd1( pJ );
  if( pC ) {
    assert( pC );
    assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
    if( pC->cmd == ARS_SCHEDULED_ROUTESET ) {
      assert( whats_kind_of_il_sym( pC->attr.sch_roset.route_id ) == _ROUTE );
      ROUTE_C_PTR pR = NULL;
      pR = conslt_route_prof( pC->attr.sch_roset.route_id );
      assert( pR );
      assert( pR->kind == _ROUTE );
      assert( (pR->route_kind < END_OF_ROUTE_KINDS) && (pR->route_kind != EMERGE_ROUTE) );
      assert( pR->ars_ctrl.app );
      if( pR->ars_ctrl.app ){
	int cond = -1;
	cond = ars_chk_hit_trgsection( pR, pJ->ptrain_ctrl );
	if( cond <= 0 ) {
	  if( cond < 0 )
	    r = ARS_MUTEX_BLOCKED;
	  else {
	    assert( cond == 0 );
	    r = ARS_NO_TRIGGERED;
	  }
	} else if( cond == 1 )
	  r = ARS_FOUND_TRAINS_AHEAD;
	else {
	  assert( cond >= 2 );
	  cond = ars_chk_cond_trackcirc( pR );
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
	    if( pC->attr.sch_roset.is_dept_route )
	      goto is_the_time_to_go;
	    else {
	      cond = ars_chk_cond_routelok( pR );
	      if( cond <= 0 ) {
		if( cond < 0 )
		  r = ARS_MUTEX_BLOCKED;
		else {
		  assert( cond == 0 );
		  r = ARS_CTRL_TRACKS_ROUTELOCKED;
		}
	      } else {
		assert( cond > 0 );
	      is_the_time_to_go:
		cond = ars_chk_trgtime( OFFSET_TO_ROUTESET, hour_to_set, minute_to_set, second_to_set );
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
		  cond = ars_chk_depschedule( pTT->sp_schedule, pC );
		  if( cond <= 0 ) {
		    if( cond < 0 )
		      r = ARS_MUTEX_BLOCKED;
		    else {
		      assert( cond == 0 );
		      r = ARS_PRED_DEPTRAINS_FOUND;
		    }
		  } else {
		    assert( cond > 0 );
		    cond = ars_chk_dstschedule( pTT->sp_schedule, pC );
		    if( cond <= 0 ) {
		      if( cond < 0 )
			r = ARS_MUTEX_BLOCKED;
		      else {
			assert( cond == 0 );
			r = ARS_WAITING_PRED_DEPTRAINS_AT_DST;
		      }
		    } else {
		      assert( pR->ars_ctrl.trip_info.dst.blk != VB_NONSENS );
		      assert( pR->ars_ctrl.trip_info.dst.pblk );
		      CBTC_BLOCK_C_PTR pdst = pR->ars_ctrl.trip_info.dst.pblk;
		      if( pdst ) {
			TINY_TRAIN_STATE_PTR pT = pJ->ptrain_ctrl;
			assert( pT );
			pT->dest_blockID = pdst->block_name;
		      }
#if 0 // NOW, UNDER CONSTRUCTION
		      {
			OC_ID oc_id;
			CBI_STAT_KIND kind;
			conslt_il_state( &oc_id, &kind, cnv2str_il_sym(pC->attr.sch_roset.route_id) );
		      }
#endif
		      assert( pC );
		      assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
		      make_it_past( pJ, pC );
		      r = ARS_ROUTE_CONTROLLED_NORMALLY;
		    }
		  }
		}
	      }
	    }
	  }
	}
      } else
	r = ARS_ILLEGAL_CMD_ROUTESET;
    } else
      r = ARS_NO_ROUTESET_CMD;
  } else
    r = ARS_NOMORE_SCHEDULED_CMDS;
  assert( r != END_OF_ARS_REASONS );
  return r;
}

SCHEDULED_COMMAND_PTR ars_sch_cmd_ack ( JOURNEY_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR r = NULL;
  
  TINY_TRAIN_STATE_PTR pT = NULL;
  pT = pJ->ptrain_ctrl;
  if( pT ) {
    SCHEDULED_COMMAND_PTR pC = NULL;
    pC = pJ->scheduled_commands.pNext;
    while( pC ) {
      switch ( pC->cmd ) {
      case ARS_SCHEDULED_ROUTESET:
	assert( pC );
	{
	  ROUTE_C_PTR pR = NULL;
	  pR = conslt_route_prof( pC->attr.sch_roset.route_id );
	  assert( pR );
	  assert( pR->kind == _ROUTE );
	  assert( (pR->route_kind < END_OF_ROUTE_KINDS) && (pR->route_kind != EMERGE_ROUTE) );
	  assert( pR->ars_ctrl.app );
	  {
	    int cond = -1;
	    cond = ars_chk_hit_trgsection( pR, pJ->ptrain_ctrl );
	    if( cond >= 2 ) {
	      OC_ID oc_id;
	      CBI_STAT_KIND kind;
	      int stat = -1;
	      stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(pC->attr.sch_roset.route_id) );
	      if( stat > 0 ) {
		timestamp( &pC->attr.sch_roset.dept_time );
		make_it_past( pJ, pC );
	      }
	    }
	  }
	}
	break;
      case ARS_SCHEDULED_ROUTEREL:
	assert( pC );
	{
	  ROUTE_C_PTR pR = NULL;
	  pR = conslt_route_prof( pC->attr.sch_rorel.route_id );
	  assert( pR );
	  assert( pR->kind == _ROUTE );
	  assert( (pR->route_kind < END_OF_ROUTE_KINDS) && (pR->route_kind != EMERGE_ROUTE) );
	  assert( pR->ars_ctrl.app );
	  if( pR->ars_ctrl.ctrl_tracks.pahead_trks[0] ) {
	    TRACK_C_PTR pahead_trk = pR->ars_ctrl.ctrl_tracks.pahead_trks[0];
	    OC_ID oc_id;
	    CBI_STAT_KIND kind;
	    int stat = -1;
	    assert( pahead_trk );
	    stat = conslt_il_state( &oc_id, &kind, cnv2str_il_sym(pahead_trk->id) );
	    if( stat == 0 ) {
	      timestamp( &pC->attr.sch_rorel.dept_time );
	      make_it_past( pJ, pC );
	    }
	  }
	}
	break;
      case ARS_SCHEDULED_ARRIVAL:
	assert( pC );
	assert( pT );
	{ 
	  ARS_EVENT_ON_SP detects = { SP_NONSENS, ARS_DETECTS_NONE };
	  ars_judge_arriv_dept_skip( &detects, pT );
	  if( detects.sp != SP_NONSENS ) {
	    if( detects.sp == pC->attr.sch_arriv.arr_sp ) {
	      if( detects.detail == ARS_DOCK_DETECTED ) {
		timestamp( &pC->attr.sch_arriv.arr_time );
		make_it_past( pJ, pC );
	      } else {
		if( detects.detail == ARS_SKIP_DETECTED ) {
		  SCHEDULED_COMMAND_PTR pC_next = NULL;
		  timestamp( &pC->attr.sch_arriv.arr_time );
		  make_it_past( pJ, pC );
		  pC_next = pJ->scheduled_commands.pNext;
		  if( pC_next )
		    if( pC_next->cmd == ARS_SCHEDULED_DEPT )
		      if( pC_next->attr.sch_dept.dept_sp == detects.sp ) {
			pC = pC_next;
			goto chk_and_acc_as_dept;
		      }
		}
	      }
	    }
	  }
	}
	break;
      case ARS_SCHEDULED_DEPT:
	assert( pC );
	assert( pT );
	{
	  ARS_EVENT_ON_SP detects = { SP_NONSENS, ARS_DETECTS_NONE };
	  ars_judge_arriv_dept_skip( &detects, pT );
	  if( detects.sp != SP_NONSENS ) {
	    if( detects.sp == pC->attr.sch_dept.dept_sp ) {
	      if( detects.detail == ARS_LEAVE_DETECTED )
		goto chk_and_acc_as_dept;
	      else {
		if( detects.detail == ARS_SKIP_DETECTED ) {
		chk_and_acc_as_dept:
		  assert( pC == pJ->scheduled_commands.pNext );
		  timestamp( &pC->attr.sch_dept.dept_time );
		  make_it_past( pJ, pC );
		}
	      }
	    }
	  }
	}
	break;
      case ARS_SCHEDULED_SKIP:
	assert( pC );
	assert( pT );
	{
	  ARS_EVENT_ON_SP detects = { SP_NONSENS, ARS_DETECTS_NONE };
	  ars_judge_arriv_dept_skip( &detects, pT );
	  if( detects.sp != SP_NONSENS ) {
	    if( detects.sp == pC->attr.sch_skip.ss_sp ) {
	      if( detects.detail == ARS_SKIP_DETECTED )
		goto chk_and_acc_as_skip;
	      else
		if( detects.detail == ARS_LEAVE_DETECTED ) {
		chk_and_acc_as_skip:
		  timestamp( &pC->attr.sch_skip.pass_time );
		  make_it_past( pJ, pC );
		}
	    }
	  }
	}
	break;
      case END_OF_SCHEDULED_CMDS:
	return pC;
      default:
	assert( FALSE );
      }
      pC = pC->ln.journey.pNext;
    }
    r = pC;
  }
  return r;
}

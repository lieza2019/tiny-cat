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
  pdetects->detail = ARS_DETECTS_NONE;
  if( pT->stop_detected != SP_NONSENS ) {
    pdetects->sp = pT->stop_detected;
    pdetects->detail = ARS_DOCK_DETECTED;
    hit_sp = detect_train_docked( &pdetects->detail, DOCK_DETECT_MINOR, pT );
    if( hit_sp == SP_NONSENS ) {
      assert( pdetects->detail != ARS_DOCK_DETECTED );
      pdetects->sp = SP_NONSENS;
      pT->stop_detected = SP_NONSENS;
    } else {
      if( pdetects->detail == ARS_LEAVE_DETECTED ) {
	pdetects->sp = hit_sp;
	pT->stop_detected = SP_NONSENS;
      } else {
	assert( pdetects->sp == pT->stop_detected );
	assert( pdetects->detail == ARS_DOCK_DETECTED );
      }
    }
  } else {
    assert( pT->stop_detected == SP_NONSENS );
    hit_sp = detect_train_docked( &pdetects->detail, DOCK_DETECT_MAJOR, pT );
    if( hit_sp != SP_NONSENS ) {
      assert( pdetects->detail == ARS_DOCK_DETECTED );
      pdetects->sp = hit_sp;
      pT->stop_detected = hit_sp;
    } else {
      hit_sp = detect_train_skip( &pdetects->detail, pT );
      if( hit_sp != SP_NONSENS ) {
	assert( pT->stop_detected == SP_NONSENS );
	assert( pdetects->detail == ARS_SKIP_DETECTED );
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
      if( stat == 0 )
	assert( kind == _TRACK );
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
  assert( pC->cmd == ARS_SCHEDULED_DEPT );
  int r = -1;
  
  assert( (pC->attr.sch_dept.dept_sp > 0) && (pC->attr.sch_dept.dept_sp < END_OF_SPs) );
  const int num_cmds = sch_dep[pC->attr.sch_dept.dept_sp].num_events;
  SCHEDULED_COMMAND_C_PTR pcmd_next = sch_dep[pC->attr.sch_dept.dept_sp].pNext;
  
  struct tm T = {};
  time_t time_dep = 0;
  time_dep = mktime_of_cmd( &T, &pC->attr.sch_dept.dept_time );
  
  while( pcmd_next ) {
    assert( pcmd_next );
    assert( (pcmd_next->cmd == ARS_SCHEDULED_ARRIVAL) || (pcmd_next->cmd == ARS_SCHEDULED_DEPT) || (pcmd_next->cmd == ARS_SCHEDULED_SKIP) );
    if( pcmd_next == pC ) {
      assert( ! pC->checked );
      break;
    } else if( pcmd_next->checked ) {
      assert( pcmd_next != pC );
      pcmd_next = pcmd_next->ln.sp_sch.pNext;
      continue;
    } else if( is_fellow( pcmd_next, pC, num_cmds ) ) {
      break;
    } else {
      time_t time_cmd = 0;
      switch( pcmd_next->cmd ) {
      case ARS_SCHEDULED_ARRIVAL:
	time_cmd = mktime_of_cmd( &T, &pcmd_next->attr.sch_arriv.arr_time );
#ifdef CHK_STRICT_CONSISTENCY
	assert( pcmd_next->attr.sch_arriv.arr_sp == pC->attr.sch_dept.dept_sp );
#endif // CHK_STRICT_CONSISTENCY
	r = 1;
	break;
      case ARS_SCHEDULED_DEPT:
	time_cmd = mktime_of_cmd( &T, &pcmd_next->attr.sch_dept.dept_time );
#ifdef CHK_STRICT_CONSISTENCY	
	assert( pcmd_next->attr.sch_dept.dept_sp == pC->attr.sch_dept.dept_sp );
	assert( pcmd_next != pC );
#endif // CHK_STRICT_CONSISTENCY
	r = 0;
	break;
      case ARS_SCHEDULED_SKIP:
	time_cmd = mktime_of_cmd( &T, &pcmd_next->attr.sch_skip.pass_time );
#ifdef CHK_STRICT_CONSISTENCY
	assert( pcmd_next->attr.sch_skip.ss_sp == pC->attr.sch_dept.dept_sp );
#endif // CHK_STRICT_CONSISTENCY
	r = 0;
	break;
      default:
	assert( FALSE );
      }
      assert( r > -1 );
      assert( time_cmd );
#ifdef CHK_STRICT_CONSISTENCY
      assert( difftime( time_cmd, time_dep ) < 0 );
#endif // CHK_STRICT_CONSISTENCY
    }
    if( !r )
      break;
    pcmd_next = pcmd_next->ln.sp_sch.pNext;
  }
  return ((r < 0) ? (r * -1) : r);
}

static int ars_chk_dstschedule ( SCHEDULE_AT_SP sch_dst[END_OF_SPs], SCHEDULED_COMMAND_C_PTR pC_dst, SCHEDULED_COMMAND_C_PTR pC_lok ) { // well tested, 2025/01/12
  assert( sch_dst );
  assert( pC_dst );
  assert( (pC_dst->cmd == ARS_SCHEDULED_ARRIVAL) || (pC_dst->cmd == ARS_SCHEDULED_SKIP) );
  assert( pC_lok ? ((pC_lok->cmd == ARS_SCHEDULED_DEPT) && (pC_dst->cmd == ARS_SCHEDULED_ARRIVAL)) : (pC_dst->cmd == ARS_SCHEDULED_SKIP) );
  int r = -1;
  
  STOPPING_POINT_CODE dst_sp = SP_NONSENS;
  switch( pC_dst->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    dst_sp = pC_dst->attr.sch_arriv.arr_sp;
    break;
  case ARS_SCHEDULED_SKIP:
    dst_sp = pC_dst->attr.sch_skip.ss_sp;
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
	  if( pcmd_next != pC_lok ) {
	    p = is_fellow( pcmd_next, pC_lok, num_cmds );
	    if( p ) {
	      assert( pC_lok->cmd == ARS_SCHEDULED_DEPT );
	      pcmd_next = pC_lok;
	    } else {
	      pcmd_next = pcmd_next->ln.sp_sch.pNext;
	      if( pcmd_next->checked )
		continue;
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
#if 1 // *****
      case ARS_CMD_DONT_CURE:
	pcmd_next = pcmd_next->ln.sp_sch.pNext;
	continue;
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

static char *mangling2_P_Sxxxy_Sxxxy ( char *praw ) {
  assert( praw );
  char *r = NULL;
  int i = 0;
  while( praw[i] ) {
    if( i >= CBI_STAT_IDENT_LEN )
      break;
    i++;
  }
  if( i < CBI_STAT_IDENT_LEN ) {
    assert( praw[i] == 0 );
    if( (i + 2) < CBI_STAT_IDENT_LEN ) {
      int j = i;
      while( j > -1 ) {
	praw[j + 2] = praw[j];
	j--;
      }
      assert( j == -1 );
      praw[0] = 'P';
      praw[1] = '_';
    }
    assert( strlen( praw ) == (i + 2) );
    r = praw;
  }
  return r;
}

static char *mangling2_Sxxxy_Sxxxy_R ( char *praw ) {
  assert( praw );
  char *r = NULL;
  int i = 0;
  while( praw[i] ) {
    if( i >= CBI_STAT_IDENT_LEN ) {
      praw[i] = 0;
      break;
    }
    i++;
  }
  if( i < CBI_STAT_IDENT_LEN ) {
    if( (i + 2) < CBI_STAT_IDENT_LEN ) {
      praw[i++] = '_';
      praw[i++] = 'R';
      praw[i] = 0;
      r = praw;
    }
    assert( i <= CBI_STAT_IDENT_LEN );
  }
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

ARS_REASONS ars_ctrl_route_on_journey ( TIMETABLE_PTR pTT, JOURNEY_PTR pJ ) {
  assert( pTT );
  assert( pJ );
  assert( pJ->ptrain_ctrl );
  ARS_REASONS r = END_OF_ARS_REASONS;
  
  SCHEDULED_COMMAND_PTR pC = NULL;
  pC = fetch_routeset_cmd( pJ );
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
	CBTC_BLOCK_C_PTR pB = NULL;
	int cond = -1;	
	cond = ars_chk_hit_trgsection( pR, &pB, pJ->ptrain_ctrl, -1 ); //cond = ars_chk_hit_trgsection( pR, pJ->ptrain_ctrl );
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
	      goto is_the_time;
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
	      is_the_time:
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
		  if( pC->cmd == ARS_SCHEDULED_ROUTESET ) {
		    SCHEDULED_COMMAND_PTR pC_dep = pC->ln.journey.pNext;
		    if( pC_dep && (pC_dep->cmd == ARS_SCHEDULED_DEPT) && (pC_dep->attr.sch_dept.dept_sp == pR->ars_ctrl.trip_info.dep.sp) ) {
		      cond = ars_chk_depschedule( pTT->sp_schedule, pC_dep );
		      if( cond <= 0 ) {
			if( cond < 0 )
			  r = ARS_MUTEX_BLOCKED;
			else {
			  assert( cond == 0 );
			  r = ARS_PRED_DEPTRAINS_FOUND;
			}
		      } else {
			assert( cond > 0 );
			assert( pTT );
			assert( pC_dep );
			if( pC->attr.sch_roset.is_dept_route ) {
			  goto ready_on_fire;
			} else {
			  SCHEDULED_COMMAND_PTR pC_dst = pC->ln.journey.pNext;
			  SCHEDULED_COMMAND_PTR pC_lok = NULL;
			  cond = ars_chk_dstschedule( pTT->sp_schedule, pC, pC ); // ***** !!!!!
			  if( cond <= 0 ) {
			    if( cond < 0 )
			      r = ARS_MUTEX_BLOCKED;
			    else {
			      assert( cond == 0 );
			      r = ARS_WAITING_PRED_DEPTRAINS_AT_DST;
			    }
			  } else {
			  ready_on_fire:
			    assert( pR->ars_ctrl.trip_info.dst.blk != VB_NONSENS );
			    assert( pR->ars_ctrl.trip_info.dst.pblk );
			    CBTC_BLOCK_C_PTR pdst = pR->ars_ctrl.trip_info.dst.pblk;
			    if( pdst ) {
			      TINY_TRAIN_STATE_PTR pT = pJ->ptrain_ctrl;
			      assert( pT );
			      pT->dest_blockID = pdst->block_name;
			    }
#if 1 // NOW, UNDER CONSTRUCTION
			    {
			      assert( pC );
			      assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
			      char *route_R = NULL;
			      char raw[CBI_STAT_IDENT_LEN + 1];
			      OC_ID oc_id;
			      CBI_STAT_KIND kind;
			      int stat = -1;
			      strncpy( raw, cnv2str_il_sym( pC->attr.sch_roset.route_id ), CBI_STAT_IDENT_LEN );
			      route_R = mangling2_Sxxxy_Sxxxy_R( raw );
			      assert( route_R );
			      stat = conslt_il_state( &oc_id, &kind, route_R );
			      if( stat == 0 ) {
				char *P_route = NULL;
				strncpy( raw, cnv2str_il_sym( pC->attr.sch_roset.route_id ), CBI_STAT_IDENT_LEN );
				P_route = mangling2_P_Sxxxy_Sxxxy( raw );
				assert( P_route );-
				engage_il_ctrl( &oc_id, &kind, P_route );
				//printf( "stat. of conslt_il_state on %s: %d\n", P_route, -1 ); // *****
				//assert( FALSE ); // *****
				r = ARS_ROUTE_CONTROLLED_NORMALLY;
				//make_it_past( pJ, pC );
			      } else {
				if( stat > 0 )
				  r = ARS_ROUTE_ALREADY_CONTROLLED;
			      }
			    }
#endif
			  }
			}
		      }
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
  assert( r != END_OF_ARS_REASONS );  // occasionally, tiny-cat process is falling down by this assertion, need to be fixed!
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
	    CBTC_BLOCK_C_PTR pB = NULL;
	    int cond = -1;
	    cond = ars_chk_hit_trgsection( pR, &pB, pJ->ptrain_ctrl, -1 );
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

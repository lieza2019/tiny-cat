#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "timetable.h"

TIMETABLE online_timetable;

static SCHEDULED_COMMAND_PTR events_at_sp[END_OF_SPs];
static SCHEDULED_COMMAND_PTR sortbuf_at_sp[SCHEDULED_COMMANDS_NODEBUF_SIZE];

static void scattr_over_sp_schedule ( JOURNEY_C_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR pcmd = pJ->scheduled_commands.pcmds;
  while( pcmd ) {
    STOPPING_POINT_CODE sp = END_OF_SPs;
    switch( pcmd->cmd ) {
    case ARS_SCHEDULED_ARRIVAL:
      sp = pcmd->attr.sch_arriv.arr_sp;
      assert( (sp >= 0) && (sp < END_OF_SPs) );
      pcmd->ln.sp_sch.pNext =  events_at_sp[sp];
      events_at_sp[sp] = pcmd;
      break;
    case ARS_SCHEDULED_DEPT:
      sp = pcmd->attr.sch_dept.dept_sp;
      assert( (sp >= 0) && (sp < END_OF_SPs) );
      pcmd->ln.sp_sch.pNext = events_at_sp[sp];
      events_at_sp[sp] = pcmd;
      break;
    case ARS_SCHEDULED_SKIP:
      sp = pcmd->attr.sch_skip.ss_sp;
      assert( (sp >= 0) && (sp < END_OF_SPs) );
      pcmd->ln.sp_sch.pNext = events_at_sp[sp];
      events_at_sp[sp] = pcmd;
      break;
    case ARS_SCHEDULED_ROUTESET:
      /* fall thru. */
    case ARS_SCHEDULED_ROUTEREL:
      break;
    case END_OF_SCHEDULED_CMDS:
      /* fall thru. */
    default:
      assert( FALSE );
      break;
    }
    pcmd = pcmd->ln.journey.pNext;
  }
}

static time_t mktime_of_cmd ( struct tm *pT, ARS_ASSOC_TIME_PTR ptime_cmd ) {
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

static int cmp_with_cmd2 ( time_t time_c1, DWELL_ID *pdw_id2, SCHEDULED_COMMAND_PTR pC2 ) {
  assert( pC2 );
  assert( pdw_id2 );
  int r = 0;
  struct tm T2 = {};
  time_t time_c2 = 0;
  
  switch( pC2->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    *pdw_id2 = pC2->attr.sch_arriv.dw_id;
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_arriv.arr_time );
    break;
  case ARS_SCHEDULED_DEPT:
    *pdw_id2 = pC2->attr.sch_dept.dw_id;
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
    *pdw_id2 = pC2->attr.sch_skip.dw_id;
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_skip.pass_time );
    break;
  case ARS_SCHEDULED_ROUTESET:
    /* fall thru. */
  case ARS_SCHEDULED_ROUTEREL:
    /* fall thru. */
  case END_OF_SCHEDULED_CMDS:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  assert( pdw_id2 );
  assert( *pdw_id2 > -1 );
  
  {
    double d;      
    d = difftime( time_c1, time_c2 );
    if( d < 0 )
      r = -1;
    else if( d > 0 )
      r = 1;
    else {
      assert( d == 0 );
      r = 0;
    }
  }
  return r;
}
static int cmp_over_sp_cmds ( const void *pC1, const void *pC2 ) {
  assert( pC1 );
  assert( pC2 );
  int r = 0;
  DWELL_ID dw_id1 = -1;
  DWELL_ID dw_id2 = -1;
  
  struct tm T1 = {};
  time_t t1 = 0;
  
  switch( ((SCHEDULED_COMMAND_PTR)pC1)->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    dw_id1 = ((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_arriv.dw_id;
    t1 = mktime_of_cmd( &T1, &((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_arriv.arr_time );   
    break;
  case ARS_SCHEDULED_DEPT:
    dw_id1 = ((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_dept.dw_id;
    t1 = mktime_of_cmd( &T1, &((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
    dw_id1 = ((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_skip.dw_id;
    t1 = mktime_of_cmd( &T1, &((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_skip.pass_time );
    break;
  case ARS_SCHEDULED_ROUTESET:
    /* fall thru. */
  case ARS_SCHEDULED_ROUTEREL:
    /* fall thru. */
  case END_OF_SCHEDULED_CMDS:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  assert( dw_id1 > -1 );
  
  r = cmp_with_cmd2( t1, &dw_id2, (SCHEDULED_COMMAND_PTR)pC2 );
  assert( dw_id2 > -1 );
  if( r == 0 ) {
    JOURNEY_ID jid1 = ((SCHEDULED_COMMAND_PTR)pC1)->jid;
    JOURNEY_ID jid2 = ((SCHEDULED_COMMAND_PTR)pC2)->jid;
    if( jid1 < jid2 )
      r = -1;
    else if( jid1 > jid2 )
      r = 1;
    else {
      assert( jid1 == jid2 );
      if( dw_id1 < dw_id2 )
	r = -1;
      else if( dw_id1 > dw_id2 )
	r = 1;
      else {
	assert( dw_id1 == dw_id2 );
	r = 0;
#ifdef CHK_STRICT_CONSISTENCY
	assert( FALSE );
#endif // CHK_STRICT_CONSISTENCY
      }
    }
  }
  return r;
}

static int cmp_over_journeys ( const void *pJ1, const void *pJ2 ) {
  assert( pJ1 );
  assert( pJ2 );
  int r = 0;
  if( ! ((JOURNEY_C_PTR)pJ1)->valid )
    r = ((JOURNEY_C_PTR)pJ2)->valid ? -1 : 0;
  else if( ! ((JOURNEY_C_PTR)pJ2)->valid )
    r = 1;
  else {
    assert( ((JOURNEY_C_PTR)pJ1)->valid && ((JOURNEY_C_PTR)pJ2)->valid );
    double d;
    d = difftime( ((JOURNEY_C_PTR)pJ1)->start_time, ((JOURNEY_C_PTR)pJ2)->start_time );
    if( d < 0 )
      r = -1;
    else if( d > 0 )
      r = 1;
    else {
      JOURNEY_ID jid1 = ((JOURNEY_C_PTR)pJ1)->jid;
      JOURNEY_ID jid2 = ((JOURNEY_C_PTR)pJ2)->jid;
      if( jid1 < jid2 )
	r = -1;
      else if( jid1 > jid2 )
	r = 1;
      else {
	assert( jid1 == jid2 );
	r = 0;
#ifdef CHK_STRICT_CONSISTENCY
	assert( FALSE );
#endif // CHK_STRICT_CONSISTENCY
      }
    }
  }
  return r;
}


BOOL is_simultaneous ( ARS_ASSOC_TIME_PTR pT1, ARS_ASSOC_TIME_PTR pT2 ) {
  assert( pT1 );
  assert( pT2 );
  BOOL r = FALSE;
  
  r = (pT1->year == pT2->year) && (pT1->month == pT2->month) && (pT1->day == pT2->day) &&
    (pT1->hour == pT2->hour) && (pT1->minute == pT2->minute) && (pT1->second == pT2->second);
  return r;
}

void cons_sp_schedule ( void ) {
  int i;
  
  assert( (online_timetable.num_journeys >= 0) && (online_timetable.num_journeys < MAX_JOURNEYS_IN_TIMETABLE) );
  for( i = 0; i < online_timetable.num_journeys; i++ ) {
    scattr_over_sp_schedule( (JOURNEY_C_PTR)&online_timetable.journeys[i] );
  }
#ifdef CHK_STRICT_CONSISTENCY
  assert( (i >= 0) && (i < MAX_JOURNEYS_IN_TIMETABLE) );
  while( i < MAX_JOURNEYS_IN_TIMETABLE ) {
    assert( ! online_timetable.journeys[i].journey.valid );
    i++;
  }
  assert( i == MAX_JOURNEYS_IN_TIMETABLE );
#endif // CHK_STRICT_CONSISTENCY
    
  for( i = 0; i < END_OF_SPs; i++ ) {
    int cnt = 0;
    SCHEDULED_COMMAND_PTR pC_sp = events_at_sp[i];   
    while( pC_sp ) {
      assert( cnt < SCHEDULED_COMMANDS_NODEBUF_SIZE );
      sortbuf_at_sp[cnt++] = pC_sp;
      pC_sp = pC_sp->ln.sp_sch.pNext;
    }
    assert( (cnt >= 0) && (cnt < SCHEDULED_COMMANDS_NODEBUF_SIZE) );
#ifdef CHK_STRICT_CONSISTENCY
    {
      int k = cnt;
      while( k < SCHEDULED_COMMANDS_NODEBUF_SIZE ) {
	sortbuf_at_sp[k] = NULL;
	k++;
      }
      assert( k == SCHEDULED_COMMANDS_NODEBUF_SIZE );
    }
#endif // CHK_STRICT_CONSISTENCY
    
    qsort( sortbuf_at_sp, cnt, sizeof(SCHEDULED_COMMAND_PTR), cmp_over_sp_cmds );
#ifdef CHK_STRICT_CONSISTENCY
    { 
      int k = 0;
      while( k < cnt ) {	
	assert( sortbuf_at_sp[k] );
	k++;
      }
      assert( k == cnt );
      while( k < SCHEDULED_COMMANDS_NODEBUF_SIZE ) {
	assert( ! sortbuf_at_sp[k] );
	k++;
      }
    }
#endif // CHK_STRICT_CONSISTENCY
    {
      ARS_ASSOC_TIME_PTR psenior_fellow_t = NULL;
      int k;
      for( pC_sp = NULL, k = 0; k < cnt; k++ ) {
	if( psenior_fellow_t ) {
	  assert( pC_sp );
	  ARS_ASSOC_TIME_PTR pjunior_fellow_t = NULL;
	  switch( sortbuf_at_sp[k]->cmd ) {
	  case ARS_SCHEDULED_ARRIVAL:
	    pjunior_fellow_t = &sortbuf_at_sp[k]->attr.sch_arriv.arr_time;
	    break;
	  case ARS_SCHEDULED_DEPT:
	    pjunior_fellow_t = &sortbuf_at_sp[k]->attr.sch_dept.dept_time;
	    break;
	  case ARS_SCHEDULED_SKIP:
	    pjunior_fellow_t = &sortbuf_at_sp[k]->attr.sch_skip.pass_time;
	    break;
	  case ARS_SCHEDULED_ROUTESET:
	    /* fall thru. */
	  case ARS_SCHEDULED_ROUTEREL:
	    /* fall thru. */
	  case END_OF_SCHEDULED_CMDS:
	    /* fall thru. */
	  default:
	    assert( FALSE );
	  }
	  if( pjunior_fellow_t ) {
	    if( is_simultaneous(psenior_fellow_t, pjunior_fellow_t) )
	      pC_sp->ln.sp_sch.pFellow = sortbuf_at_sp[k];
	  }
	  psenior_fellow_t = pjunior_fellow_t;
	} else
	  switch( sortbuf_at_sp[k]->cmd ) {
	  case ARS_SCHEDULED_ARRIVAL:
	    psenior_fellow_t = &sortbuf_at_sp[k]->attr.sch_arriv.arr_time;
	    break;
	  case ARS_SCHEDULED_DEPT:
	    psenior_fellow_t = &sortbuf_at_sp[k]->attr.sch_dept.dept_time;
	    break;
	  case ARS_SCHEDULED_SKIP:
	    psenior_fellow_t = &sortbuf_at_sp[k]->attr.sch_skip.pass_time;
	    break;
	  case ARS_SCHEDULED_ROUTESET:
	    /* fall thru. */
	  case ARS_SCHEDULED_ROUTEREL:
	    /* fall thru. */
	  case END_OF_SCHEDULED_CMDS:
	    /* fall thru. */
	  default:
	    assert( FALSE );
	  }
	pC_sp = sortbuf_at_sp[k];
	assert( pC_sp );
	pC_sp->ln.sp_sch.pFellow = NULL;
	pC_sp->ln.sp_sch.pNext = (k + 1) < cnt ? sortbuf_at_sp[k + 1] : NULL;
      }
      assert( k == cnt );
#ifdef CHK_STRICT_CONSISTENCY
      {
	SCHEDULED_COMMAND_PTR p = sortbuf_at_sp[0];
	while( p ) {
	  k--;
	  p = p->ln.sp_sch.pNext;
	}
	assert( k == 0 );
      }
#endif // CHK_STRICT_CONSISTENCY
    }
    events_at_sp[i] = sortbuf_at_sp[0];
  }
  
  for( i = 0; i < END_OF_SPs; i++ ) {
    SCHEDULED_COMMAND_PTR p = NULL;
    online_timetable.sp_schedule[i].pFirst = events_at_sp[i];
    p = online_timetable.sp_schedule[i].pFirst;
    while( p ) {
      assert( p );
      if( ! p->check ) {
	online_timetable.sp_schedule[i].pNext = p;
	break;
      }
      p = p->ln.sp_sch.pNext;
    }
#ifdef CHK_STRICT_CONSISTENCY
    while( p ) {
      assert( ! p->check );
      p = p->ln.sp_sch.pNext;
    }
#endif // CHK_STRICT_CONSISTENCY
  }
  assert( i == END_OF_SPs );
}

void makeup_online_timetable ( void ) {
  qsort( online_timetable.journeys, MAX_JOURNEYS_IN_TIMETABLE, sizeof(struct journeys), cmp_over_journeys );
  {
    int cnt = 0;
    int i;
    for( i = 1; i <= MAX_JOURNEYS_IN_TIMETABLE; i++ )
      online_timetable.lkup[i] = NULL;
    
    for( i = 0; i < MAX_JOURNEYS_IN_TIMETABLE; i++ ) {
      JOURNEY_ID jid = online_timetable.journeys[i].journey.jid;
      if( ! online_timetable.journeys[i].journey.valid )
	break;
      assert( (jid > 0) && (jid <= MAX_JOURNEYS_IN_TIMETABLE) );
      online_timetable.lkup[jid] = &online_timetable.journeys[i];
      cnt++;
    }
    online_timetable.num_journeys = cnt;
#ifdef CHK_STRICT_CONSISTENCY
    assert( (i >= 0) && (i < MAX_JOURNEYS_IN_TIMETABLE) );
    while( i < MAX_JOURNEYS_IN_TIMETABLE ) {
      assert( ! online_timetable.journeys[i].journey.valid );
      i++;
    }
    assert( i == MAX_JOURNEYS_IN_TIMETABLE );
#endif // CHK_STRICT_CONSISTENCY
  }
  
  cons_sp_schedule();
}

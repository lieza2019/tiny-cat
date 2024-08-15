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
      pcmd->ln.pNext_sp =  events_at_sp[sp];
      events_at_sp[sp] = pcmd;
      break;
    case ARS_SCHEDULED_DEPT:
      sp = pcmd->attr.sch_dept.dept_sp;
      assert( (sp >= 0) && (sp < END_OF_SPs) );
      pcmd->ln.pNext_sp = events_at_sp[sp];
      events_at_sp[sp] = pcmd;
      break;
    case ARS_SCHEDULED_SKIP:
      sp = pcmd->attr.sch_skip.ss_sp;
      assert( (sp >= 0) && (sp < END_OF_SPs) );
      pcmd->ln.pNext_sp = events_at_sp[sp];
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
    pcmd = pcmd->ln.pNext_cmd;
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

static int cmp_with_cmd2 ( time_t time_c1, SCHEDULED_COMMAND_PTR pC2 ) {
  assert( pC2 );
  int r = 0;
  struct tm T2 = {};
  time_t time_c2 = 0;
  
  switch( pC2->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_arriv.arr_time );
    break;
  case ARS_SCHEDULED_DEPT:
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
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
  struct tm T1 = {};
  time_t t1 = 0;
  
  switch( ((SCHEDULED_COMMAND_PTR)pC1)->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    t1 = mktime_of_cmd( &T1, &((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_arriv.arr_time );
    break;
  case ARS_SCHEDULED_DEPT:
    t1 = mktime_of_cmd( &T1, &((SCHEDULED_COMMAND_PTR)pC1)->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
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
  
  r = cmp_with_cmd2( t1, (SCHEDULED_COMMAND_PTR)pC2 );
  if( r == 0 ) {
    JOURNEY_ID jid1 = ((SCHEDULED_COMMAND_PTR)pC1)->jid;
    JOURNEY_ID jid2 = ((SCHEDULED_COMMAND_PTR)pC2)->jid;
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
  return r;
}

static int cmp_over_journeys ( const void *pJ1, const void *pJ2 ) {
  assert( pJ1 );
  assert( pJ2 );
  int r = 0;
  if( ! ((JOURNEY_C_PTR)pJ1)->valid )
    r = ((JOURNEY_C_PTR)pJ2)->valid ? -1 : 0;
  else if( ! ((JOURNEY_C_PTR)pJ2)->valid )
    r = ((JOURNEY_C_PTR)pJ1)->valid ? 1 : 0;
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

void shuffl_online_timetable ( void ) {
  qsort( online_timetable.journeys, MAX_JOURNEYS_IN_TIMETABLE, sizeof(struct journeys), cmp_over_journeys );
  {
    int cnt = 0;
    int i;
    for( i = 0; i < MAX_JOURNEYS_IN_TIMETABLE; i++ ) {
      if( ! online_timetable.journeys[i].journey.valid )
	break;
      cnt++;
    }
#ifdef CHK_STRICT_CONSISTENCY
    assert( (i >= 0) && (i < MAX_JOURNEYS_IN_TIMETABLE) );
    while( i < MAX_JOURNEYS_IN_TIMETABLE ) {
      assert( ! online_timetable.journeys[i].journey.valid );
      i++;
    }
    assert( i == MAX_JOURNEYS_IN_TIMETABLE );
#endif // CHK_STRICT_CONSISTENCY
    online_timetable.num_journeys = cnt;
  }
  
  assert( (online_timetable.num_journeys >= 0) && (online_timetable.num_journeys < MAX_JOURNEYS_IN_TIMETABLE) );
  { 
    int i;
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
      SCHEDULED_COMMAND_PTR pC_sp = events_at_sp[i];
      int cnt = 0;
      while( pC_sp ) {
	assert( cnt < SCHEDULED_COMMANDS_NODEBUF_SIZE );
	sortbuf_at_sp[cnt++] = pC_sp;
	pC_sp = pC_sp->ln.pNext_sp;
      }
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
	assert( (cnt >= 0) && (cnt < SCHEDULED_COMMANDS_NODEBUF_SIZE) );
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
	int k;
	for( k = 0; k < cnt; k++ ) {
	  pC_sp = sortbuf_at_sp[k];
	  assert( pC_sp );	
	  pC_sp->ln.pNext_sp = (k + 1) < cnt ? sortbuf_at_sp[k + 1] : NULL;
	}
	assert( k == cnt );
#ifdef CHK_STRICT_CONSISTENCY
	{
	  SCHEDULED_COMMAND_PTR p = sortbuf_at_sp[0];
	  while( p ) {
	    k--;
	    p = p->ln.pNext_sp;
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
	;
	p = p->ln.pNext_sp;
      }
    }
    assert( i == END_OF_SPs );
  }
}

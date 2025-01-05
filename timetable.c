#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"

#define TIMETABLE_C
#include "timetable.h"
#undef TIMETABLE_C

TIMETABLE online_timetable;

static SCHEDULED_COMMAND_PTR events_at_sp[END_OF_SPs];
static SCHEDULED_COMMAND_PTR sortbuf_at_sp[SCHEDULED_COMMANDS_NODEBUF_SIZE];

static void scattr_over_sp_schedule ( JOURNEY_C_PTR pJ ) { // well tested, 2025/01/04
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
      /* fall thru. */
    case END_OF_SCHEDULED_CMDS:
      break;
    default:
      assert( FALSE );
      break;
    }
    pcmd = pcmd->ln.journey.pNext;
  }
}

static int cmp_with_cmd2 ( time_t time_c1, DWELL_ID_PTR pdw_seq2, SCHEDULED_COMMAND_PTR pC2 ) {
  assert( pC2 );
  int r = 0;
  struct tm T2 = {};
  time_t time_c2 = 0;
  
  switch( pC2->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    if( pdw_seq2 )
      *pdw_seq2 = pC2->attr.sch_arriv.dw_seq;
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_arriv.arr_time );
    break;
  case ARS_SCHEDULED_DEPT:
    if( pdw_seq2 )
      *pdw_seq2 = pC2->attr.sch_dept.dw_seq;
    time_c2 = mktime_of_cmd( &T2, &pC2->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
    if( pdw_seq2 )
      *pdw_seq2 = pC2->attr.sch_skip.dw_seq;
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
  if( pdw_seq2 ) {
    assert( *pdw_seq2 > -1 );
    double d;
    d = difftime( time_c1, time_c2 );
    if( d < 0 )
      r = -1;
    else if( d > 0 )
      r = 1;
    else {
      r = 0;
    }
  }
  return r;
}
static int cmp_over_sp_cmds ( const void *pC1, const void *pC2 ) {
  assert( pC1 );
  assert( pC2 );
  int r = 0;  
  DWELL_ID dw_seq1 = -1;
  DWELL_ID dw_seq2 = -1;
  DWELL_ID_PTR pdw_seq2 = &dw_seq2;
			 
  struct tm T1 = {};
  time_t t1 = 0;
  
  switch( (*(SCHEDULED_COMMAND_PTR *)pC1)->cmd ) {
  case ARS_SCHEDULED_ARRIVAL:
    dw_seq1 = (*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_arriv.dw_seq;
    t1 = mktime_of_cmd( &T1, &(*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_arriv.arr_time );
    break;
  case ARS_SCHEDULED_DEPT:
    dw_seq1 = (*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_dept.dw_seq;
    t1 = mktime_of_cmd( &T1, &(*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_dept.dept_time );
    break;
  case ARS_SCHEDULED_SKIP:
    dw_seq1 = (*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_skip.dw_seq;
    t1 = mktime_of_cmd( &T1, &(*(SCHEDULED_COMMAND_PTR *)pC1)->attr.sch_skip.pass_time );
    break;
  case END_OF_SCHEDULED_CMDS:
    assert( pdw_seq2 );
    r = ((*(SCHEDULED_COMMAND_PTR *)pC2)->cmd == END_OF_SCHEDULED_CMDS) ? 0 : -1;
    pdw_seq2 = NULL;
    break;
  case ARS_SCHEDULED_ROUTESET:
    assert( FALSE );
    /* fall thru. */
  case ARS_SCHEDULED_ROUTEREL:
    assert( FALSE );
    /* fall thru. */
  default:
    assert( FALSE );
  }
  
  r = cmp_with_cmd2( t1, pdw_seq2, *(SCHEDULED_COMMAND_PTR *)pC2 );
  if( pdw_seq2 ) {
    assert( dw_seq2 > -1 );
    if( r == 0 ) {
      JOURNEY_ID jid1 = (*(SCHEDULED_COMMAND_PTR *)pC1)->jid;
      JOURNEY_ID jid2 = (*(SCHEDULED_COMMAND_PTR *)pC2)->jid;
      if( jid1 < jid2 )
	r = -1;
      else if( jid1 > jid2 )
	r = 1;
      else {
	assert( jid1 == jid2 );
	if( dw_seq1 < dw_seq2 )
	  r = -1;
	else if( dw_seq1 > dw_seq2 )
	  r = 1;
	else {
	  assert( dw_seq1 == dw_seq2 );
	  r = 0;
	}
      }
    }
  } else {
    assert( (*(SCHEDULED_COMMAND_PTR *)pC1)->cmd == END_OF_SCHEDULED_CMDS );
    if( (*(SCHEDULED_COMMAND_PTR *)pC2)->cmd == END_OF_SCHEDULED_CMDS )
      assert( r == 0 );
    else
      assert( r < 0 );
  }
  return r;
}

static int cmp_over_journeys ( const void *pJ1, const void *pJ2 ) {
  assert( pJ1 );
  assert( pJ2 );
  int r = 0;
  if( ! ((JOURNEY_C_PTR)pJ1)->valid ) {
    if( ((JOURNEY_C_PTR)pJ2)->valid )
      r = 1;
    else {
      assert( !((JOURNEY_C_PTR)pJ1)->valid && !((JOURNEY_C_PTR)pJ2)->valid );
      goto try_again;
    }
  } else if( ! ((JOURNEY_C_PTR)pJ2)->valid )
    r = -1;
  else {
    assert( ((JOURNEY_C_PTR)pJ1)->valid && ((JOURNEY_C_PTR)pJ2)->valid );
    struct tm tm = {};
    double d;
  try_again:
    d = difftime( mktime_of_cmd(&tm, &((JOURNEY_PTR)pJ1)->start_time), mktime_of_cmd(&tm, &((JOURNEY_PTR)pJ2)->start_time) );
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
      }
    }
  }
  return r;
}

static BOOL coincide ( ARS_ASSOC_TIME_PTR pT1, ARS_ASSOC_TIME_PTR pT2 ) {
  assert( pT1 );
  assert( pT2 );
  BOOL r = FALSE;
  
  r = (pT1->year == pT2->year) && (pT1->month == pT2->month) && (pT1->day == pT2->day) &&
    (pT1->hour == pT2->hour) && (pT1->minute == pT2->minute) && (pT1->second == pT2->second);
  return r;
}

void cons_sp_schedule ( void ) { // well tested, 2025/01/04
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
      SCHEDULED_COMMAND_PTR psenior_fellow = NULL;      
      ARS_ASSOC_TIME_PTR psenior_fellow_t = NULL;
      int k;
      for( pC_sp = NULL, k = 0; k < cnt; k++ ) {
	if( psenior_fellow ) {
	  assert( psenior_fellow_t );
	  ARS_ASSOC_TIME_PTR pjunior_fellow_t = NULL;
	  SCHEDULED_COMMAND_PTR pjunior_fellow = sortbuf_at_sp[k];
	  assert( pjunior_fellow );
	  assert( pC_sp );
	  switch( sortbuf_at_sp[k]->cmd ) {
	  case ARS_SCHEDULED_ARRIVAL:
	    pjunior_fellow_t = &pjunior_fellow->attr.sch_arriv.arr_time;
	    break;
	  case ARS_SCHEDULED_DEPT:
	    pjunior_fellow_t = &pjunior_fellow->attr.sch_dept.dept_time;
	    break;
	  case ARS_SCHEDULED_SKIP:
	    pjunior_fellow_t = &pjunior_fellow->attr.sch_skip.pass_time;
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
	  assert( pjunior_fellow );
	  assert( pjunior_fellow_t );
	  if( coincide( psenior_fellow_t, pjunior_fellow_t ) ) {
	    pC_sp->ln.sp_sch.pFellow = pjunior_fellow;
	  } else {
	    if( pC_sp != psenior_fellow )
	      pC_sp->ln.sp_sch.pFellow = psenior_fellow;
	    else
	      assert( ! pC_sp->ln.sp_sch.pFellow );
	    psenior_fellow = pjunior_fellow;
	    psenior_fellow_t = pjunior_fellow_t;
	  }
	} else {
	  assert( k == 0 );
	  assert( !pC_sp );
	  assert( !psenior_fellow_t );
	  psenior_fellow = sortbuf_at_sp[k];	  
	  switch( psenior_fellow->cmd ) {
	  case ARS_SCHEDULED_ARRIVAL:	    
	    psenior_fellow_t = &psenior_fellow->attr.sch_arriv.arr_time;
	    break;
	  case ARS_SCHEDULED_DEPT:
	    psenior_fellow_t = &psenior_fellow->attr.sch_dept.dept_time;
	    break;
	  case ARS_SCHEDULED_SKIP:
	    psenior_fellow_t = &psenior_fellow->attr.sch_skip.pass_time;
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
	  assert( psenior_fellow );
	  assert( psenior_fellow_t );
	}
	pC_sp = sortbuf_at_sp[k];
	assert( pC_sp );
	pC_sp->ln.sp_sch.pFellow = NULL;
	pC_sp->ln.sp_sch.pNext = (k + 1) < cnt ? sortbuf_at_sp[k + 1] : NULL;
      }
      assert( k == cnt );
      if( cnt > 0 ) {	
	assert( pC_sp == sortbuf_at_sp[cnt - 1] );
	assert( psenior_fellow );
	assert( psenior_fellow_t );
	if( pC_sp != psenior_fellow )
	  pC_sp->ln.sp_sch.pFellow = psenior_fellow;
	else
	  assert( ! pC_sp->ln.sp_sch.pFellow );
      }
#ifdef CHK_STRICT_CONSISTENCY
      {
	SCHEDULED_COMMAND_PTR p = sortbuf_at_sp[0];
	while( p ) {
	  assert( --k >= 0 );
	  p = p->ln.sp_sch.pNext;
	}
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
      if( ! p->checked ) {
	online_timetable.sp_schedule[i].pNext = p;
	break;
      }
      p = p->ln.sp_sch.pNext;
    }
#ifdef CHK_STRICT_CONSISTENCY
    while( p ) {
      assert( ! p->checked );
      p = p->ln.sp_sch.pNext;
    }
#endif // CHK_STRICT_CONSISTENCY
  }
  assert( i == END_OF_SPs );
}

void makeup_online_timetable ( void ) { // well tested, 2025/01/04
  int cnt = 0;
  
  qsort( online_timetable.journeys, online_timetable.num_journeys, sizeof(struct journeys), cmp_over_journeys );
  {
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
  {
    assert( online_timetable.num_journeys == cnt );
    int j;
    for( j = 0; j < online_timetable.num_journeys; j++ ) {
      SCHEDULED_COMMAND_PTR pcmd = online_timetable.journeys[j].journey.scheduled_commands.pcmds;
      while( pcmd ) {
	assert( pcmd );
	pcmd->ln.journey.pNext = NULL;
	if( pcmd->cmd == END_OF_SCHEDULED_CMDS )
	  break;
	else
	  pcmd->ln.journey.pNext = (pcmd + 1);	  
	pcmd++;
      }
      assert( pcmd );
      assert( pcmd->cmd == END_OF_SCHEDULED_CMDS );
    }
  }
  cons_sp_schedule();
}

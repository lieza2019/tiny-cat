#include <string.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"
#include "interlock.h"
#include "ars.h"
#include "timetable.h"

const char *cnv2str_ars_reasons[] = {
  "ARS_OTHER_TRAINS_AHEAD",
  "ARS_CTRL_TRACKS_OCCUPIED",
  "ARS_CTRL_TRACKS_ROUTELOCKED",
  "ARS_BLOCKED",
  "ARS_CONTROLLED_NORMALLY",
  NULL
};

#define SCHEDULED_COMMANDS_NODEBUF_SIZE 65536
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

static int ars_chk_cond_routelok ( ROUTE_PTR proute ) {
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
    assert( ptr->kind_cbi == _TRACK );
    {
      int stat = -1;
      if( ptr->lock.TLSR.app ) {
	assert( ptr->lock.TLSR.kind == _TLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.TLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _TLSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _TLSR );
      }
      if( ptr->lock.TRSR.app ) {
	assert( ptr->lock.TRSR.kind == _TRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.TRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _TRSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _TRSR );
      }
      if( ptr->lock.sTLSR.app ) {
	assert( ptr->lock.sTLSR.kind == _sTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.sTLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _sTLSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _sTLSR );
      }
      if( ptr->lock.sTRSR.app ) {
	assert( ptr->lock.sTRSR.kind == _sTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.sTRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _sTRSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _sTRSR );
      }
      if( ptr->lock.eTLSR.app ) {
	assert( ptr->lock.eTLSR.kind == _eTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.eTLSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _eTLSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _eTLSR );
      }
      if( ptr->lock.eTRSR.app ) {
	assert( ptr->lock.eTRSR.kind == _eTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(ptr->lock.eTRSR.id) );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _eTRSR );
	  r = stat;
	  break;
	}
	assert( stat == 1 );
	assert( kind == _eTRSR );
      }
      r = stat;
    }
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_lok) ? (r == 1) : (r <= 0) );
  
  return r;
}

static int ars_chk_cond_trackcirc ( ROUTE_PTR proute ) {
  assert( proute );
  int r = -1;
  
  int i = -1;
  //for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks; i++ ) {
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks_ctrl; i++ ) {
    OC_ID oc_id;
    CBI_STAT_KIND kind;
    int stat = -1;
    
    TRACK_C_PTR ptr = NULL;
    ptr = proute->ars_ctrl.ctrl_tracks.pchk_trks[i];
    assert( ptr );
    assert( ptr->kind_cbi == _TRACK );
    assert( ! strncmp(ptr->name, cnv2str_il_obj(ptr->id), CBI_STAT_IDENT_LEN) );
    stat = conslt_il_state( &oc_id, &kind, ptr->name );
    if( stat <= 0 ) {
      if( stat == 0 )
	assert( kind == _TRACK );
      r = stat;
      break;
    }
    assert( stat == 1 );
    assert( kind == _TRACK );
    r = stat;
  }
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_ctrl) ? (r == 1) : (r <= 0) );
  
  return r;
}

static SCHEDULED_COMMAND_PTR *fetch_cmd_routeset ( SCHEDULED_COMMAND_PTR *ppNext_cmd ) {
  assert( ppNext_cmd );
  SCHEDULED_COMMAND_PTR *r = NULL;
  
  SCHEDULED_COMMAND_PTR *ppC = NULL;
  ppC = ppNext_cmd;
  assert( ppC );
  while( *ppC ) {
    assert( ppC );
    assert( *ppC );
    if( (*ppC)->cmd == ARS_SCHEDULED_ROUTESET ) {
      r = ppC;
      break;
    } else {
      if( ((*ppC)->cmd == ARS_SCHEDULED_DEPT) || ((*ppC)->cmd == ARS_SCHEDULED_SKIP) )
	break;
    }
    ppC = &(*ppC)->pNext;
  }
  assert( ppC );
  assert( (r != NULL) ? ((r == ppC) && ((*ppC)->cmd == ARS_SCHEDULED_ROUTESET)) : (*ppC ? (((*ppC)->cmd == ARS_SCHEDULED_DEPT) || ((*ppC)->cmd == ARS_SCHEDULED_SKIP)) : (r == NULL)) );
  return r;
}

ARS_REASONS ars_ctrl_route_on_journey ( JOURNEY_PTR pJ ) {
  assert( pJ );
  ARS_REASONS r = END_OF_ARS_REASONS;
  SCHEDULED_COMMAND_PTR *ppC = NULL;
  
  ppC = fetch_cmd_routeset( &pJ->scheduled_commands.pNext );
  if( ppC ) {
    assert( (*ppC)->cmd == ARS_SCHEDULED_ROUTESET );
    SCHEDULED_COMMAND_PTR pC = *ppC;
    assert( pC );
    assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
    {
      assert( whats_kind_of_il_obj( pC->attr.sch_routeset.route_id ) == _ROUTE );
      ROUTE_PTR pR = NULL;
      pR = &route_state[pC->attr.sch_routeset.route_id];
      assert( pR );
      assert( (pR->kind_cbi == _ROUTE) && (pR->kind_route < END_OF_ROUTE_KINDS) );
      {
	int cond = -1;
	cond = ars_chk_cond_trackcirc( pR );
	if( cond <= 0 ) {
	  if( cond < 0 )
	    r = ARS_MUTEX_BLOCKED;
	  else
	    r = ARS_CTRL_TRACKS_OCCUPIED;
	} else {
	  if( pC->attr.sch_routeset.is_dept_route )
	    r = ARS_CONTROLLED_NORMALLY;
	  else {
	    cond = ars_chk_cond_routelok( pR );
	    if( cond <= 0 ) {
	      if( cond < 0 )
		r = ARS_MUTEX_BLOCKED;
	      else
		r = ARS_CTRL_TRACKS_ROUTELOCKED;
	    } else {
	      r = ARS_CONTROLLED_NORMALLY;
	      ;
	    }
	  }
	}
      }
    }
  } else
    r = ARS_NO_ROUTESET_CMD;
  
  assert( r != END_OF_ARS_REASONS );
  return r;
}

void ars_sch_cmd_ack ( JOURNEY_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR pC = NULL;
  pC = pJ->scheduled_commands.pNext;
  while( pC ) {
    switch ( pC->cmd ) {
    case ARS_SCHEDULED_ROUTESET:
      //pC->attr.sch_routeset.route_id
      break;
    case ARS_SCHEDULED_ROUTEREL:
      break;
    case ARS_SCHEDULED_ARRIVAL:
      break;
    case ARS_SCHEDULED_DEPT:
      break;
    case ARS_SCHEDULED_SKIP:
      break;
    case END_OF_SCHEDULED_CMDS:
      break;
    default:
      assert( FALSE );
    }
    pC = pC->pNext;
  }
  pJ->scheduled_commands.pNext = pC;
}

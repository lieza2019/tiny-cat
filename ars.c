#include <string.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"
#include "interlock.h"
#include "ars.h"
#include "timetable.h"

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

static BOOL ars_chk_cond_routelok ( ROUTE_PTR proute ) {
  assert( proute );
  assert( proute->ars_ctrl.app );
  BOOL r = FALSE;
  
  int i = -1;  
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks; i++ ) {
    OC_ID oc_id;
    CBI_STAT_KIND kind;   
    TRACK_PTR ptr = NULL;
    ptr = proute->ars_ctrl.ctrl_tracks.pchk_trks[i];
    assert( ptr );
    assert( ptr->kind_cbi == _TRACK );
    { 
      int stat = -1;
      if( ptr->lock.TLSR.app ) {
	assert( ptr->lock.TLSR.kind == _TLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.TLSR.id] );
	if( stat <= 0 )
	  break;
	assert( stat == 1 );
	assert( kind == _TLSR );
      }
      if( ptr->lock.TRSR.app ) {
	assert( ptr->lock.TRSR.kind == _TRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.TRSR.id] );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _TRSR );
	  break;
	}
	assert( stat == 1 );
	assert( kind == _TRSR );
      }
      if( ptr->lock.sTLSR.app ) {
	assert( ptr->lock.sTLSR.kind == _sTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.sTLSR.id] );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _sTLSR );
	  break;
	}
	assert( stat == 1 );
	assert( kind == _sTLSR );
      }
      if( ptr->lock.sTRSR.app ) {
	assert( ptr->lock.sTRSR.kind == _sTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.sTRSR.id] );	
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _sTRSR );
	  break;
	}
	assert( stat == 1 );
	assert( kind == _sTRSR );
      }
      if( ptr->lock.eTLSR.app ) {
	assert( ptr->lock.eTLSR.kind == _eTLSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.eTLSR.id] );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _eTLSR );
	  break;
	}
	assert( stat == 1 );
	assert( kind == _eTLSR );
      }
      if( ptr->lock.eTRSR.app ) {
	assert( ptr->lock.eTRSR.kind == _eTRSR );
	stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->lock.eTRSR.id] );
	if( stat <= 0 ) {
	  if( stat == 0 )
	    assert( kind == _eTRSR );
	  break;
	}
	assert( stat == 1 );
	assert( kind == _eTRSR );
      }
    }
  }
  if( i >= proute->ars_ctrl.ctrl_tracks.num_tracks ) {
    assert( i == proute->ars_ctrl.ctrl_tracks.num_tracks );
    r = TRUE;
  }
  
  return r;
}

static BOOL ars_chk_cond_trackcirc ( ROUTE_PTR proute ) {
  assert( proute );
  BOOL r = FALSE;
  
  int i = -1;
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks; i++ ) {
    OC_ID oc_id;
    CBI_STAT_KIND kind;
    int stat = -1;
    
    TRACK_PTR ptr = NULL;
    ptr = proute->ars_ctrl.ctrl_tracks.pchk_trks[i];
    assert( ptr );
    assert( ptr->kind_cbi == _TRACK );
    assert( ! strncmp(cnv2str_il_obj_instances[ptr->id], ptr->name, CBI_STAT_IDENT_LEN) );
    stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj_instances[ptr->id] );
    if( stat <= 0 ) {
      if( stat == 0 )
	assert( kind == _TRACK );
      break;
    }
    assert( stat == 1 );
    assert( kind == _TRACK );
  }
  if( i >= proute->ars_ctrl.ctrl_tracks.num_tracks ) {
    assert( i == proute->ars_ctrl.ctrl_tracks.num_tracks );
    r = TRUE;
  }
  
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

void ars_ctrl_route_on_journey ( JOURNEY_PTR pJ ) {
  assert( pJ );
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
      ars_chk_cond_trackcirc( pR );
    }
  }
}

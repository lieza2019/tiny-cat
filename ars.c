#include <string.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"
#include "ars.h"
#include "interlock.h"

BOOL chk_routeconf ( ROUTE_PTR r1, ROUTE_PTR r2) {
  assert( r1 );
  assert( r2 );
  BOOL r = FALSE;
  
  int i;
  for( i = 0; i < r1->trks.num_tracks; i++ ) {
    int j;
    for( j = 0; j < r2->trks.num_tracks; j++ )
      if( r1->trks.tracks[i] == r2->trks.tracks[j] ) {
	r = TRUE;
	goto found;
      }
  }
 found:
  return r;
}

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

BOOL chk_ars_cond_routelock ( ROUTE_PTR proute ) {
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
    assert( ptr->kind == _TRACK );
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

BOOL chk_ars_cond_trackstat ( ROUTE_PTR proute ) {
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
    assert( ptr->kind == _TRACK );
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

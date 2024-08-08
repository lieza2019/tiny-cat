#include <string.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
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
  for( i = 0; i < proute->ars_ctrl.ctrl_tracks.num_tracks_occ; i++ ) {
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
  assert( (i >= proute->ars_ctrl.ctrl_tracks.num_tracks_occ) ? (r == 1) : (r <= 0) );
  
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

static BOOL no_trains_ahead ( ROUTE_PTR proute, int ahead_blk ) {
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

static int ars_chk_hit_trgsection ( ROUTE_PTR proute, TINY_TRAIN_STATE_PTR ptrain_ctrl) {
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

ARS_REASONS ars_ctrl_route_on_journey ( JOURNEY_PTR pJ ) {
  assert( pJ );
  assert( pJ->ptrain_ctrl );
  ARS_REASONS r = END_OF_ARS_REASONS;
  SCHEDULED_COMMAND_PTR *ppC = NULL;
  
  ppC = fetch_cmd_routeset( &pJ->scheduled_commands.pNext );
  if( ppC ) {
    assert( (*ppC)->cmd == ARS_SCHEDULED_ROUTESET );
    SCHEDULED_COMMAND_PTR pC = *ppC;
    assert( pC );    
    {
      assert( pC->cmd == ARS_SCHEDULED_ROUTESET );
      assert( whats_kind_of_il_obj( pC->attr.sch_routeset.route_id ) == _ROUTE );
      ROUTE_PTR pR = NULL;
      pR = &route_state[pC->attr.sch_routeset.route_id];
      assert( pR );
      assert( (pR->kind_cbi == _ROUTE) && (pR->kind_route < END_OF_ROUTE_KINDS) );
      assert( pR->ars_ctrl.app );
      {
	int cond = -1;
	cond = ars_chk_hit_trgsection( pR, pJ->ptrain_ctrl );
	if( cond <= 0 ) {
	  if( cond < 0 )
	    r = ARS_MUTEX_BLOCKED;
	  else
	    r = ARS_NO_TRIGGERED;
	} else if( cond == 1 )
	  r = ARS_OTHER_TRAINS_AHEAD;
	else {
	  assert( cond >= 2 );
	  cond = ars_chk_cond_trackcirc( pR );
	  if( cond <= 0 ) {
	    if( cond < 0 )
	      r = ARS_MUTEX_BLOCKED;
	    else
	      r = ARS_CTRL_TRACKS_OCCUPIED;
	  } else {
	    if( pC->attr.sch_routeset.is_dept_route )
	      r = ARS_ROUTE_CONTROLLED_NORMALLY;
	    else {
	      cond = ars_chk_cond_routelok( pR );
	      if( cond <= 0 ) {
		if( cond < 0 )
		  r = ARS_MUTEX_BLOCKED;
		else
		  r = ARS_CTRL_TRACKS_ROUTELOCKED;
	      } else {
		;
		r = ARS_ROUTE_CONTROLLED_NORMALLY;
	      }
	    }
	  }
	}
      }
      ;
    }
  } else
    r = ARS_NO_ROUTESET_CMD;
  
  assert( r != END_OF_ARS_REASONS );
  return r;
}

BOOL ars_judge_arriv ( TINY_TRAIN_STATE_PTR pT, SP_ID sp ) {
  assert( pT );
  assert( sp < END_OF_SPs );
  BOOL r = FALSE;
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  pI = pT->pTI;
  assert( pI );
  {
    unsigned short blk_occ_front = 0;
    unsigned short blk_occ_rear = 0;
    blk_occ_front = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
    blk_occ_rear = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
    assert( blk_occ_front > 0 );
    assert( blk_occ_rear > 0 );
  }
  return r;
}

void ars_sch_cmd_ack ( JOURNEY_PTR pJ ) {
  assert( pJ );
  SCHEDULED_COMMAND_PTR pC = NULL;
  pC = pJ->scheduled_commands.pNext;
  while( pC ) {
    OC_ID oc_id;    CBI_STAT_KIND kind;
    int stat = -1;
    switch ( pC->cmd ) {
    case ARS_SCHEDULED_ROUTESET:
      stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(pC->attr.sch_routeset.route_id) );
      assert( stat >= 0 );
      if( stat > 0 ) {
	SCHEDULED_COMMAND_PTR *pp = NULL;
	pp = &pJ->past_commands;
	assert( pp );
	while( *pp ) {
	  assert( *pp );
	  pp = &(*pp)->pNext;
	  assert( pp );
	}
	assert( pp );
	assert( ! *pp );
	*pp = pC;
	pJ->scheduled_commands.pNext = pC->pNext;
	pC->pNext = NULL;
      }
      break;
    case ARS_SCHEDULED_ROUTEREL:
      stat = conslt_il_state( &oc_id, &kind, cnv2str_il_obj(pC->attr.sch_routeset.route_id) );
      assert( stat >= 0 );
      if( stat < 1 ) {
	assert( stat == 0 );
	SCHEDULED_COMMAND_PTR *pp = NULL;
	pp = &pJ->past_commands;
	assert( pp );
	while( *pp ) {
	  pp = &(*pp)->pNext;
	  assert( pp );
	}
	assert( pp );
	assert( ! *pp );
	*pp = pC;
	pJ->scheduled_commands.pNext = pC->pNext;
	pC->pNext = NULL;
      }
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

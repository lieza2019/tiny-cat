#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "interlock.h"

typedef enum dock_detect_directive {
  DOCK_DETECT_MAJOR,
  DOCK_DETECT_MINOR
} DOCK_DETECT_DIRECTIVE;

STOPPING_POINT_CODE judge_train_docked ( DOCK_DETECT_DIRECTIVE mode, TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  pI = pT->pTI;
  assert( pI );
  if( (mode == DOCK_DETECT_MINOR) || (TRAIN_INFO_STOP_DETECTION(*pI)) ) {
    const unsigned short blk_occ_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
    const unsigned short blk_occ_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
    
    assert( blk_occ_forward > 0 );
    assert( blk_occ_back > 0 );
    CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( blk_occ_forward );
    CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( blk_occ_back );
    if( (pB_forward != NULL) && (pB_back != NULL) ) {
      if( pB_forward->sp.has_sp || pB_back->sp.has_sp ) {
	if( ! pB_back->sp.has_sp ) {
	  assert( pB_forward->sp.has_sp );
	  if( pB_forward->sp.stop_detect_cond.paired_blk == VB_NONSENS )
	    r = pB_forward->sp.sp_code;
	} else if( ! pB_forward->sp.has_sp ) {
	  assert( pB_back->sp.has_sp );
	  if( pB_back->sp.stop_detect_cond.paired_blk == VB_NONSENS )
	    r = pB_back->sp.has_sp;
	} else {
	  assert( pB_forward->sp.has_sp && pB_back->sp.has_sp );
	  if( pB_forward == pB_back ) {
	    assert( pB_forward->sp.sp_code == pB_back->sp.sp_code );
	    r = pB_forward->sp.sp_code;
	  } else {
	    assert( pB_forward != pB_back );
	    if( pB_forward->sp.sp_code == pB_back->sp.sp_code ) {
	      CBTC_BLOCK_C_PTR pforward_pairblk = pB_forward->sp.stop_detect_cond.ppaired_blk;
	      CBTC_BLOCK_C_PTR pback_pairblk = pB_back->sp.stop_detect_cond.ppaired_blk;
	      if( (pforward_pairblk != NULL) && (pback_pairblk != NULL) )
		if( ((pforward_pairblk == pB_back) && (pforward_pairblk->virt_block_name == pB_back->virt_block_name)) &&
		    ((pback_pairblk == pB_forward) && (pback_pairblk->virt_block_name == pB_forward->virt_block_name)) ) {
		  assert( pB_forward->sp.sp_code == pB_back->sp.sp_code );
		  r = pB_forward->sp.sp_code;
		}
	    }
	  }	
	}
      }
    } else {
      INVALID_OCCUPIED_BLOCK_COND cond_invalid;
      if( !pB_forward ) {
	if( !pB_back ) {
	  assert( !pB_forward );
	  assert( !pB_back );
	  cond_invalid = INVALID_FORWARD_BACK_BLOCK;
	} else {
	  assert( !pB_forward );
	  assert( pB_back );
	  cond_invalid = INVALID_FORWARD_BLOCK;
	}
      } else {
	assert( pB_forward );
	assert( !pB_back );
	cond_invalid = INVALID_BACK_BLOCK;
      }
      ;
    }     
    if( r != SP_NONSENS ) {
      assert( pB_forward );
      assert( pB_back );
      if( pB_forward->sp.has_sp || pB_back->sp.has_sp ) {
	assert( pI );
	switch( mode ) {
	case DOCK_DETECT_MAJOR:
	  switch( TRAIN_INFO_OPERATION_MODE( *pI ) ) {
	  case OM_UTO:
	    /* same as the case of ATO, fall thru. */
	  case OM_ATO:
	    if( TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) ) {
	      if( TRAIN_INFO_ATO_DRIVING_STATUS(*pI) == DS_PO_STOPPING )
		break;
	    }
	    r = SP_NONSENS;
	    break;
	  case OM_ATP:
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI)) )
	      r = SP_NONSENS;
	    break;
	  case OM_RM:
	    /* fail thru. */
	  default:
	    break;
	  }
	  break;
	case DOCK_DETECT_MINOR:
	  switch( TRAIN_INFO_OPERATION_MODE( *pI ) ) {
	  case OM_UTO:
	    /* same as the case of ATO, fall thru. */
	  case OM_ATO:
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) || TRAIN_INFO_STOP_DETECTION(*pI)) )
	      r = SP_NONSENS;
	    break;
	  case OM_ATP:
	    /* fail thru. */
	  case OM_RM:
	    /* fail thru. */
	  default:
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) || TRAIN_INFO_STOP_DETECTION(*pI)) )
	      if( !((pB_forward->sp.sp_code == pT->stop_detected) && (pB_back->sp.sp_code == pT->stop_detected)) )
		r = SP_NONSENS;
	    break;
	  }
	  break;
	default:
	  assert( FALSE );
	}
      }
    }
  }
  return r;
}

#include <string.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "interlock.h"
#include "surveill.h"

#if 0 // now obsolete
static STOPPING_POINT_CODE sp_with_p0 ( CBTC_BLOCK_C_PTR pblk_forward, CBTC_BLOCK_C_PTR pblk_back ) {
  assert( pblk_forward );
  assert( pblk_back );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  if( pblk_forward->sp.has_sp && !pblk_back->sp.has_sp )
    r = (pblk_forward->sp.stop_detect_type == P0_COUPLING) ? pblk_forward->sp.sp_code : SP_NONSENS;
  else if( !pblk_forward->sp.has_sp && pblk_back->sp.has_sp )    
    r = (pblk_back->sp.stop_detect_type == P0_COUPLING) ? pblk_back->sp.sp_code : SP_NONSENS;
  else if( pblk_forward->sp.has_sp && pblk_back->sp.has_sp ) {
    if( pblk_forward == pblk_back ) {
      assert( pblk_forward->sp.sp_code == pblk_back->sp.sp_code );
      if( pblk_forward->sp.stop_detect_type == P0_COUPLING ) {
	assert( pblk_back->sp.stop_detect_type == P0_COUPLING );
	r = pblk_forward->sp.sp_code;
      } else {
	assert( pblk_back->sp.stop_detect_type != P0_COUPLING );
	r = SP_NONSENS;
      }
    } else {
      if( pblk_forward->sp.sp_code == pblk_back->sp.sp_code )	  
	if( (pblk_forward->sp.stop_detect_type == P0_COUPLING) || (pblk_back->sp.stop_detect_type == P0_COUPLING) )
	  r = pblk_forward->sp.sp_code;
	else {
	  assert( (pblk_forward->sp.stop_detect_type != P0_COUPLING) && (pblk_back->sp.stop_detect_type != P0_COUPLING) );
	  r = SP_NONSENS;
	}
      else
	r = SP_NONSENS;
    }
  } else {
    assert( !pblk_forward->sp.has_sp && !pblk_back->sp.has_sp );
    r = SP_NONSENS;
  }
  return r;
}

STOPPING_POINT_CODE detect_train_leave ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  TRAIN_INFO_ENTRY TI;
  
  memset( &TI, 0, sizeof(TRAIN_INFO_ENTRY) );
  pI = (TRAIN_INFO_ENTRY_PTR)conslt_cbtc_state( pT, CBTC_TRAIN_INFORMATION, NULL, &TI, sizeof(TRAIN_INFO_ENTRY) );
  if( pI ) {
    assert( pI == &TI );
    if( pT->stop_detected != SP_NONSENS ) {
      if( !TRAIN_INFO_STOP_DETECTION(*pI) ) {
	const unsigned short blk_occ_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
	const unsigned short blk_occ_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
	assert( blk_occ_forward > 0 );
	assert( blk_occ_back > 0 );
	CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( blk_occ_forward );
	CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( blk_occ_back );
	if( (pB_forward != NULL) && (pB_back != NULL) ) {
	  if( pB_forward->sp.has_sp || pB_back->sp.has_sp ) {
	    STOPPING_POINT_CODE sp_now = SP_NONSENS;
	    sp_now = sp_with_p0( pB_forward, pB_back );
	    if( sp_now == pT->stop_detected ) {
	      assert( pB_forward->sp.has_sp || pB_back->sp.has_sp );
	      switch( TRAIN_INFO_OPERATION_MODE( *pI ) ) {
	      case OM_UTO:
		/* same as the case of ATO, fall thru. */
	      case OM_ATO:
		if( !TRAIN_INFO_P0_STOPPED(*pI) && !TRAIN_INFO_DOOR_ENABLE(*pI) ) {
		  r = sp_now;
		  break;
		}
		break;
	      case OM_ATP:
		/* fail thru. */
	      case OM_RM:
		/* fail thru. */
	      default:
		if( !TRAIN_INFO_P0_STOPPED(*pI) && !TRAIN_INFO_DOOR_ENABLE(*pI) )
		  goto chk_front_blk_occupancy;
		break;
	      }
	    } else {
	    chk_front_blk_occupancy:
	      if( !pB_forward->sp.has_sp && pB_back->sp.has_sp )
		r = (pB_back->sp.sp_code == pT->stop_detected) ? pT->stop_detected : SP_NONSENS;
	    }
	  }
	} else {
	  assert( !(pB_forward && pB_back) );
	  if( !pB_forward ) {
	    if( !pB_back ) {
	      assert( !pB_forward );
	      assert( !pB_back );
	      errorF( "invalid occupied block (forward): %d, detected in leaving-detector.\n", blk_occ_forward );
	      errorF( "invalid occupied block (back): %d, detected in leaving-detector.\n", blk_occ_back );
	    } else {
	      assert( !pB_forward );
	      assert( pB_back );
	      errorF( "invalid occupied block (forward): %d, detected in leaving-detector.\n", blk_occ_forward );	  
	    }
	  } else {
	    assert( pB_forward );
	    assert( !pB_back );
	    errorF( "invalid occupied block (back): %d, detected in leaving-detector.\n", blk_occ_back );
	  }
	}    
      }
    }
  }
  assert( (r != SP_NONSENS) ? (r == pT->stop_detected) : TRUE );
  return r;
}
#endif

STOPPING_POINT_CODE detect_train_docked ( ARS_EVENT_ON_SP_PTR pev_sp, DOCK_DETECT_DIRECTIVE mode, TINY_TRAIN_STATE_PTR pT ) {
  assert( pev_sp );
  assert( (mode == DOCK_DETECT_MAJOR) || (mode == DOCK_DETECT_MINOR) );
  assert( pT );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  TRAIN_INFO_ENTRY TI;
  
  memset( &TI, 0, sizeof(TRAIN_INFO_ENTRY) );
  pI = (TRAIN_INFO_ENTRY_PTR)conslt_cbtc_state( pT, CBTC_TRAIN_INFORMATION, NULL, &TI, sizeof(TRAIN_INFO_ENTRY) );
  if( pI ) {
    assert( pI == &TI );
    if( (mode == DOCK_DETECT_MINOR) || (TRAIN_INFO_STOP_DETECTION(*pI)) ) {
      const unsigned short blk_occ_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
      const unsigned short blk_occ_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
      assert( blk_occ_forward > 0 );
      assert( blk_occ_back > 0 );
      CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( blk_occ_forward );
      CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( blk_occ_back );
      if( mode == DOCK_DETECT_MINOR ) {
	assert( pT->stop_detected != SP_NONSENS );
	r = pT->stop_detected;
	goto exam_for_bak_blk;
      } else {
	assert( mode == DOCK_DETECT_MAJOR );
	assert( TRAIN_INFO_STOP_DETECTION( *pI ) );
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
	  assert( !(pB_forward && pB_back) );
	exam_for_bak_blk:
	  if( !pB_forward ) {
	    if( !pB_back ) {
	      assert( !pB_forward );
	      assert( !pB_back );
	      errorF( "invalid occupied block (forward): %d, detected in docking-detector..\n", blk_occ_forward );
	      errorF( "invalid occupied block (back): %d, detected in docking-detector.\n", blk_occ_back );
	    } else {
	      assert( !pB_forward );
	      assert( pB_back );
	      errorF( "invalid occupied block (forward): %d, detected in docking-detector.\n", blk_occ_forward );	  
	    }
	  } else {
	    assert( pB_forward );
	    if( !pB_back )
	      errorF( "invalid occupied block (back): %d, detected in docking-detector.\n", blk_occ_back );
	    else
	      assert( pB_forward && pB_back );
	  }
	}
      }
      if( r != SP_NONSENS ) {
	assert( pB_forward );
	assert( pB_back );
	assert( pB_forward->sp.has_sp || pB_back->sp.has_sp );
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
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) || TRAIN_INFO_STOP_DETECTION(*pI)) ) {
	      if( pB_forward && pB_back )
		if( !((pB_forward->sp.sp_code == pT->stop_detected) && (pB_back->sp.sp_code == pT->stop_detected)) )
		  r = SP_NONSENS;
	    } else {
	      if( pB_forward && pB_back )
		if( (pB_forward->sp.sp_code != pT->stop_detected) && (pB_back->sp.sp_code != pT->stop_detected) )
		  r = SP_NONSENS;
	    }
	    break;
	  }
	  break;
	default:
	  assert( FALSE );
	}
      }
    }
  } else {
    assert( pT );
    r = pT->stop_detected;
  }
  return r;
}

STOPPING_POINT_CODE detect_train_skip ( TINY_TRAIN_STATE_PTR pT ) {
  assert( pT );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  TRAIN_INFO_ENTRY_PTR pI_prev = NULL;
  TRAIN_INFO_ENTRY_PTR pI_now = NULL;
  
  TRAIN_INFO_ENTRY TI_prev;
  TRAIN_INFO_ENTRY TI_now;
  memset( &TI_prev, 0, sizeof(TRAIN_INFO_ENTRY) );
  memset( &TI_now, 0, sizeof(TRAIN_INFO_ENTRY) );
  
  pI_now = (TRAIN_INFO_ENTRY_PTR)conslt_cbtc_state( pT, CBTC_TRAIN_INFORMATION, &TI_prev, &TI_now, sizeof(TRAIN_INFO_ENTRY) );
  if( pI_now ) {
    assert( pI_now == &TI_now );
    pI_prev = &TI_prev;
    if( pT->stop_detected == SP_NONSENS ) {
      assert( pI_prev );
      const unsigned short blk_occ_forward_prev = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI_prev );
      const unsigned short blk_occ_back_prev = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI_prev );
      if( (blk_occ_forward_prev > 0) && (blk_occ_back_prev > 0) ) {
	CBTC_BLOCK_C_PTR pB_forward_prev = lookup_cbtc_block_prof( blk_occ_forward_prev );
	CBTC_BLOCK_C_PTR pB_back_prev = lookup_cbtc_block_prof( blk_occ_back_prev );
	if( (pB_forward_prev != NULL) && (pB_back_prev != NULL) ) {
	  if( pB_forward_prev->sp.has_sp ) {
	    const STOPPING_POINT_CODE sp_prev = pB_forward_prev->sp.sp_code;
	    const unsigned short blk_occ_forward_now = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI_now );
	    const unsigned short blk_occ_back_now = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI_now );	
	    assert( (blk_occ_forward_prev > 0) && (blk_occ_forward_now > 0) );
	    assert( (blk_occ_back_prev > 0) && (blk_occ_back_now > 0) );
	    CBTC_BLOCK_C_PTR pB_forward_now = lookup_cbtc_block_prof( blk_occ_forward_now );
	    CBTC_BLOCK_C_PTR pB_back_now = lookup_cbtc_block_prof( blk_occ_back_now );
	    if( (pB_forward_now != NULL) && (pB_back_now != NULL) ) {
	      if( pB_back_now->sp.has_sp && (pB_back_now->sp.sp_code == sp_prev) )
		if( !pB_forward_now->sp.has_sp ) {
		  assert( pI_now );
		  if ( TRAIN_INFO_SKIP_NEXT_STOP( *pI_now ) )
		    r = pB_back_now->sp.sp_code;
		}
	    } else {
	      assert( !(pB_forward_now && pB_back_now) );
	      if( !pB_forward_now ) {
		if( !pB_back_now ) {
		  assert( !pB_forward_now );
		  assert( !pB_back_now );
		  errorF( "invalid occupied block (forward): %d, detected in skipping-detector.\n", blk_occ_forward_now );
		  errorF( "invalid occupied block (back): %d, detected in skipping-detector.\n", blk_occ_back_now );
		} else {
		  assert( !pB_forward_now );
		  assert( pB_back_now );
		  errorF( "invalid occupied block (forward): %d, detected in skipping-detector.\n", blk_occ_forward_now );
		}
	      } else {
		assert( pB_forward_now );
		assert( !pB_back_now );
		errorF( "invalid occupied block (back): %d, detected in skipping-detector.\n", blk_occ_back_now );
	      }
	    }	
	  }
	}
      }
    }
  }
  return r;
}

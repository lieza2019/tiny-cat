#include <string.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "interlock.h"
#include "surveill.h"

#if 0 // now obsolete
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

STOPPING_POINT_CODE detect_train_docked ( ARS_EVENTS_OVER_SP *pev_sp, DOCK_DETECT_DIRECTIVE mode, TINY_TRAIN_STATE_PTR pT ) {
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
    const unsigned short occblk_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
    const unsigned short occblk_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
    assert( occblk_forward > 0 );
    assert( occblk_back > 0 );
    CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( occblk_forward );
    CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( occblk_back );
    if( mode == DOCK_DETECT_MINOR ) {
      assert( pT->stop_detected != SP_NONSENS );
      r = pT->stop_detected;
      goto exam_for_bak_blk;
    } else {
      assert( mode == DOCK_DETECT_MAJOR );
      if( (pB_forward != NULL) && (pB_back != NULL) ) {
	if( pB_forward->sp.has_sp || pB_back->sp.has_sp ) {
	  if( ! pB_back->sp.has_sp ) {
	    assert( pB_forward->sp.has_sp );
	    assert( pT );
	    pT->misc.prev_blk_forward = pB_forward->block_name;
	    if( pB_forward->sp.stop_detect_cond.paired_blk == VB_NONSENS )
	      r = pB_forward->sp.sp_code;
	  } else if( ! pB_forward->sp.has_sp ) {
	    assert( pB_back->sp.has_sp );
	    if( pB_back->sp.stop_detect_cond.paired_blk == VB_NONSENS )
	      r = pB_back->sp.sp_code;
	  } else {
	    assert( pB_forward->sp.has_sp && pB_back->sp.has_sp );
	    if( pB_forward == pB_back ) {
	      assert( pB_forward->sp.sp_code == pB_back->sp.sp_code );
	      assert( pT );
	      pT->misc.prev_blk_forward = pB_forward->block_name;
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
	    errorF( "invalid occupied block (forward): %d, detected in docking-detector..\n", occblk_forward );
	    errorF( "invalid occupied block (back): %d, detected in docking-detector.\n", occblk_back );
	  } else {
	    assert( !pB_forward );
	    assert( pB_back );
	    errorF( "invalid occupied block (forward): %d, detected in docking-detector.\n", occblk_forward );	  
	  }
	} else {
	  assert( pB_forward );
	  if( !pB_back )
	    errorF( "invalid occupied block (back): %d, detected in docking-detector.\n", occblk_back );
	  else
	    assert( pB_forward && pB_back );
	}
      }
    }
    if( r != SP_NONSENS ) {
      assert( pI );
      assert( pB_forward );
      assert( pB_back );
      assert( pB_forward->sp.has_sp || pB_back->sp.has_sp );
      STOPPING_POINT_CODE sp_detected = SP_NONSENS;
      if( pB_forward->sp.has_sp && !(pB_back->sp.has_sp) )
	sp_detected = pB_forward->sp.sp_code;
      else if( !(pB_forward->sp.has_sp) && pB_back->sp.has_sp )
	sp_detected = pB_back->sp.sp_code;
      else {
	assert( pB_forward->sp.sp_code == pB_back->sp.sp_code );
	sp_detected = pB_forward->sp.sp_code;
      }
      switch( mode ) {
      case DOCK_DETECT_MAJOR:
	if( TRAIN_INFO_STOP_DETECTION( *pI ) ) {
	  switch( TRAIN_INFO_OPERATION_MODE( *pI ) ) {
	  case OM_UTO:
	    /* same as the case of ATO, fall thru. */
	  case OM_ATO:
	    if( sp_detected == sp_with_p0( pB_forward, pB_back ) ) {
	      if( TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) ) {
		if( TRAIN_INFO_ATO_DRIVING_STATUS(*pI) == DS_PO_STOPPING )
		  goto ATO_docked;
	      }
	    } else {
	    ATO_docked:
	      *pev_sp = ARS_DOCK_DETECTED;
	      pT->misc.prev_blk_forward = 0;
	      break;
	    }
	    r = SP_NONSENS;
	    break;
	  case OM_ATP:
	    if( sp_detected == sp_with_p0( pB_forward, pB_back ) ) {
	      if( TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) )
		goto ATP_docked;
	    } else {
	    ATP_docked:
	      *pev_sp = ARS_DOCK_DETECTED;
	      pT->misc.prev_blk_forward = 0;
	      break;
	    }
	    r = SP_NONSENS;
	    break;
	  case OM_RM:
	    /* fail thru. */
	  default:
	    *pev_sp = ARS_DOCK_DETECTED;
	    pT->misc.prev_blk_forward = 0;
	    break;
	  }
	  break;
	} else {
	  r = SP_NONSENS;
	  break;
	}
      case DOCK_DETECT_MINOR:
	if( pB_forward && pB_back ) {
	  if( (pB_forward->sp.sp_code != pT->stop_detected) && (pB_back->sp.sp_code != pT->stop_detected) ) {
	    r = SP_NONSENS;
	    *pev_sp = ARS_DETECTS_NONE;
	    pT->misc.prev_blk_forward = 0;
	    break;
	  }
	}
	switch( TRAIN_INFO_OPERATION_MODE( *pI ) ) {
	case OM_UTO:
	  /* same as the case of ATO, fall thru. */
	case OM_ATO:
	  if( sp_detected == sp_with_p0( pB_forward, pB_back ) ) {
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) || TRAIN_INFO_STOP_DETECTION(*pI)) )
	      goto ATO_leavin;
	  } else {
	  ATO_leavin:
	    *pev_sp = ARS_LEAVE_DETECTED;
	  }
	  break;
	case OM_ATP:
	  /* fail thru. */
	case OM_RM:
	  /* fail thru. */
	default:
	  if( sp_detected == sp_with_p0( pB_forward, pB_back ) ) {
	    if( !(TRAIN_INFO_P0_STOPPED(*pI) || TRAIN_INFO_DOOR_ENABLE(*pI) || TRAIN_INFO_STOP_DETECTION(*pI)) )
	      goto ATP_RM_etc_leavin;
	  } else {
	  ATP_RM_etc_leavin:
	    if( pB_forward && pB_back )
	      if( !((pB_forward->sp.sp_code == pT->stop_detected) && (pB_back->sp.sp_code == pT->stop_detected)) )
		*pev_sp = ARS_LEAVE_DETECTED;
	  }
	  break;
	}
	break;
      default:
	assert( FALSE );
      }
    }
  } else {
    assert( pT );
    r = pT->stop_detected;
  }
  return r;
}

STOPPING_POINT_CODE detect_train_skip ( ARS_EVENTS_OVER_SP *pev_sp, TINY_TRAIN_STATE_PTR pT ) {
  assert( pev_sp );
  assert( pT );
  STOPPING_POINT_CODE r = SP_NONSENS;
  
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  TRAIN_INFO_ENTRY TI;
  memset( &TI, 0, sizeof(TRAIN_INFO_ENTRY) );
  
  pI = (TRAIN_INFO_ENTRY_PTR)conslt_cbtc_state( pT, CBTC_TRAIN_INFORMATION, NULL, &TI, sizeof(TRAIN_INFO_ENTRY) );
  if( pI ) {
    assert( pI == &TI );
    if( pT->stop_detected == SP_NONSENS ) {
      if( pT->misc.prev_blk_forward > 0 ) {
	CBTC_BLOCK_C_PTR pB_forward_prev = lookup_cbtc_block_prof( pT->misc.prev_blk_forward );
	assert( pB_forward_prev->sp.has_sp );
	if( pB_forward_prev->sp.has_sp ) {
	  const STOPPING_POINT_CODE sp_prev = pB_forward_prev->sp.sp_code;
	  const unsigned short occblk_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
	  const unsigned short occblk_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );	
	  assert( occblk_forward > 0 );
	  assert( occblk_back > 0 );
	  CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( occblk_forward );
	  CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( occblk_back );
	  if( (pB_forward != NULL) && (pB_back != NULL) ) {
	    if( pB_back->sp.has_sp && (pB_back->sp.sp_code == sp_prev) ) {
	      if( (pB_forward_prev->block_name != pB_forward->block_name) && !(pB_forward->sp.has_sp) ) {
		//printf( "(forward, back) = (%d, %d)\n", pB_forward->block_name, pB_back->block_name ); // ***** for debugging.
		assert( pI );
		if ( TRAIN_INFO_SKIP_NEXT_STOP( *pI ) ) {
		  r = pB_back->sp.sp_code;
		  *pev_sp = ARS_SKIP_DETECTED;
		}
		pT->misc.prev_blk_forward = 0;
	      }
	    }
	  } else {
	    assert( !(pB_forward && pB_back) );
	    if( !pB_forward ) {
	      if( !pB_back ) {
		assert( !pB_forward );
		assert( !pB_back );
		errorF( "invalid occupied block (forward): %d, detected in skipping-detector.\n", occblk_forward );
		errorF( "invalid occupied block (back): %d, detected in skipping-detector.\n", occblk_back );
	      } else {
		assert( !pB_forward );
		assert( pB_back );
		errorF( "invalid occupied block (forward): %d, detected in skipping-detector.\n", occblk_forward );
	      }
	    } else {
	      assert( pB_forward );
	      assert( !pB_back );
	      errorF( "invalid occupied block (back): %d, detected in skipping-detector.\n", occblk_back );
	    }
	  }
	}
      }
    }
  }
  return r;
}

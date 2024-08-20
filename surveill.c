#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "cbi.h"
#include "interlock.h"

STOPPING_POINT_CODE judge_train_arriv ( TINY_TRAIN_STATE_PTR pT, STOPPING_POINT_CODE sp ) {
  assert( pT );
  assert( (sp > 0) && (sp < END_OF_SPs) );
  STOPPING_POINT_CODE r = FALSE;
  
  TRAIN_INFO_ENTRY_PTR pI = NULL;
  pI = pT->pTI;
  assert( pI );
  if( TRAIN_INFO_PO_STOPPED(*pI) ) {
    const unsigned short blk_occ_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pI );
    const unsigned short blk_occ_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pI );
    assert( blk_occ_forward > 0 );
    assert( blk_occ_back > 0 );
    {
      CBTC_BLOCK_C_PTR pB_forward = lookup_cbtc_block_prof( blk_occ_forward );
      CBTC_BLOCK_C_PTR pB_back = lookup_cbtc_block_prof( blk_occ_back );
      if( (pB_forward != NULL) && (pB_back != NULL) ) {
	if( pB_forward->sp.has_sp || pB_back->sp.has_sp ) {
	  if( ! pB_back->sp.has_sp ) {
	    assert( pB_forward->sp.has_sp );
	    r = pB_forward->sp.sp_code;
	  } else if( ! pB_forward->sp.has_sp ) {
	    assert( pB_back->sp.has_sp );
	    r = pB_back->sp.has_sp;
	  } else {
	    assert( pB_forward->sp.has_sp && pB_back->sp.has_sp );
	    if( pB_forward == pB_back ) {
	      assert( pB_forward->sp.sp_code == pB_back->sp.sp_code );
	      r = pB_forward->sp.sp_code;
	    } else {
	      assert( pB_forward != pB_back );
	      if( (pB_forward->sp.stop_detect_cond.pforward == NULL) && (pB_forward->sp.stop_detect_cond.pback != NULL) ) {
		;
	      } else if( (pB_forward->sp.stop_detect_cond.pforward != NULL) && (pB_forward->sp.stop_detect_cond.pback == NULL) ) {
		;
	      } else {
		assert( pB_forward->sp.has_sp && pB_back->sp.has_sp );
		assert( pB_forward != pB_back );
		if( (pB_forward->sp.stop_detect_cond.pforward == NULL) && (pB_forward->sp.stop_detect_cond.pback == NULL) )
		  r = pB_forward->sp.sp_code;
		else {
		  assert( pB_forward->sp.stop_detect_cond.pforward && pB_forward->sp.stop_detect_cond.pback );
		  assert( FALSE );
		}
	      }
	    }	
	  }
	}
	
      } else {
	if( !pB_forward ) {
	  ;
	} else {
	  assert( !pB_back );
	  ;
	}
      }
    }
  }
  return r;
}

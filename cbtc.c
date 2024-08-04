#include "generic.h"
#include "misc.h"

#define CBTC_C
#include "cbtc.h"
#undef CBTC_C

#include "sparcs.h"

TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB ) {
  assert( pB );
  return (TINY_TRAIN_STATE_PTR)pB->residents;
}

TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  pB->residents = (void *)pT;
  return (TINY_TRAIN_STATE_PTR)pB->residents;
}

TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB ) {
  assert( pB );
  return (TINY_TRAIN_STATE_PTR *)(&pB->residents);
}

static CBTC_BLOCK_PTR blkname2_cbtc_block_prof[65536];
static CBTC_BLOCK_PTR virtblk2_cbtc_block_prof[65536];

void cons_lkuptbl_cbtc_block_prof ( void ) {
  const CBTC_BLOCK_PTR plim_sup = (CBTC_BLOCK_PTR)((unsigned char *)block_state + (int)sizeof(block_state));
  
  // for the lookup-table of blkname2_cbtc_block_prof: block_name -> block_prof
  {
    int i = 0;
    assert( i == 0 );
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      CBTC_BLOCK_PTR p = &block_state[i];
      assert( (p >= block_state) && (p < plim_sup) );
      int idx = p->block_name;
      assert( (idx > 0) && (idx < 65536) );
      assert( blkname2_cbtc_block_prof[idx] == NULL );
      blkname2_cbtc_block_prof[idx] = p;
      assert( blkname2_cbtc_block_prof[idx] );
      assert( blkname2_cbtc_block_prof[idx]->block_name == block_state[i].block_name );
      i++;
    }
    assert( (i >= 0) && (i < 65536) );
  }
  
  // for the lookup-table of virtblk2_cbtc_block_prof: virtual_block_name -> block_prof
  {
    int i = 0;
    assert( i == 0 );
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      CBTC_BLOCK_PTR p = &block_state[i];
      assert( (p >= block_state) && (p < plim_sup) );
      int idx = (int)p->virt_block_name;
      assert( (idx >= 0) && (idx < 65536) );
      assert( virtblk2_cbtc_block_prof[idx] == NULL );
      virtblk2_cbtc_block_prof[idx] = p;
      assert( virtblk2_cbtc_block_prof[idx] );
      assert( virtblk2_cbtc_block_prof[idx]->virt_block_name == block_state[i].virt_block_name );
      i++;
    }
    assert( (i >= 0) && (i < 65536) );
  }
}

CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name ) {
  return blkname2_cbtc_block_prof[block_name];
}

CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname ) {
  return virtblk2_cbtc_block_prof[virt_blkname];
}

void purge_block_restrains ( void ) {
  int i = 0;
  while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
    CBTC_BLOCK_PTR pB = &block_state[i];
    assert( pB );
    assert( (pB->block_name > 0) && (pB->virt_block_name != END_OF_CBTC_BLOCKs) );
    TINY_TRAIN_STATE_PTR *pp = NULL;
    pp = addr_residents_CBTC_BLOCK( pB );
    assert( pp );
    while( *pp ) {
      assert( *pp );
      if( (*pp)->omit ) {
	TINY_TRAIN_STATE_PTR w = NULL;
	w = *pp;
	assert( w );
	*pp = w->occupancy.front.pNext;
	w->occupancy.front.pNext = NULL;
	continue;
      }
      pp = &(*pp)->occupancy.front.pNext;
      assert( pp );
    }
    i++;
  }
}	

#if 0 // for MODULE-TEST
int main( void ) {
  const CBTC_BLOCK_PTR plim_sup = (CBTC_BLOCK_PTR)((unsigned char *)block_state + (int)sizeof(block_state));
  
  cons_lkuptbl_cbtc_block_prof();
  // test for the method of lookup_cbtc_block_prof().
  {
    int i;
    for( i = 0; i < 65536; i++ ) {
      CBTC_BLOCK_PTR p = NULL;
      p = lookup_cbtc_block_prof( (unsigned short)i );
      if( p ) {
	assert( p->block_name == (unsigned short)i );
	assert( !(p->misc.msc_flg1) );
	p->misc.msc_flg1 = TRUE;
      } else {
	int j = 0;
	assert( j == 0 );
	while( block_state[j].virt_block_name != END_OF_CBTC_BLOCKs ) {
	  CBTC_BLOCK_PTR q = &block_state[j];
	  assert( (q >= block_state) && (q < plim_sup) );
	  assert( q->block_name != (unsigned short)i );
	  j++;
	}
      }
    }
    {
      int k = 0;
      assert( k == 0 );
      while( block_state[k].virt_block_name != END_OF_CBTC_BLOCKs ) {
	assert( block_state[k].misc.msc_flg1 );
	k++;
      }
      assert( block_state[k].virt_block_name == END_OF_CBTC_BLOCKs );
    }
  }
  // test for the method of conslt_cbtc_block_prof().
  {
    int i;
    for( i = 0; i < 65536; i++ ) {
      CBTC_BLOCK_PTR p = NULL;
      p = conslt_cbtc_block_prof( (CBTC_BLOCK_ID)i );
      if( p ) {
	assert( p->virt_block_name == (CBTC_BLOCK_ID)i );
	assert( !(p->misc.msc_flg2) );
	p->misc.msc_flg2 = TRUE;
      } else {
	int j = 0;
	assert( j == 0 );
	while( block_state[j].virt_block_name != END_OF_CBTC_BLOCKs ) {
	  CBTC_BLOCK_PTR q = &block_state[j];
	  assert( (q >= block_state) && (q < plim_sup) );
	  assert( q->virt_block_name != (CBTC_BLOCK_ID)i );
	  j++;
	}
      }
    }
    {
      int k = 0;
      assert( k == 0 );
      while( block_state[k].virt_block_name != END_OF_CBTC_BLOCKs ) {
	assert( block_state[k].misc.msc_flg2 );
	k++;
      }
      assert( block_state[k].virt_block_name == END_OF_CBTC_BLOCKs );
    }
  }
  
  return 0;
}
#endif

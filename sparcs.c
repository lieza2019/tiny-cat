#include "generic.h"
#include "misc.h"
#include "sparcs.h"

TOWARD_SC_CMDSET toward_SC[END_OF_SCs];
FROM_SC_DATASET from_SC[END_OF_SCs];

static int which_SC_zones( SC_ID zones[], int front_blk, int back_blk ) {  
  assert( zones );
  zones[0] = SC801;
  zones[1] = SC802;
  return( 2 );
}

static TRAIN_COMMAND_ENTRY_PTR lkup_train_cmd( int rakeID, TOWARD_SC_CMDSET_PTR pCs ) {
  assert( rakeID > 0 );
  assert( pCs );
  TRAIN_COMMAND_ENTRY_PTR pE = NULL;
  
  {
    int i;
    for( i = 0; i < TRAIN_COMMAND_ENTRIES_NUM; i++ )
      if( pCs->train_cmd.entries[i].rakeID == rakeID ) {
	pE = &pCs->train_cmd.entries[i];
	break;
      }
  }
  if( !pE ) {
    int j;
    for( j = 0 ; j < pCs->train_cmd.frontier; j++ )
      if( pCs->train_cmd.entries[j].rakeID == 0 ) {
	pE = &pCs->train_cmd.entries[j];
	break;
      }
    if( !pE ) {
      assert( j == pCs->train_cmd.frontier );
      if( pCs->train_cmd.frontier < TRAIN_COMMAND_ENTRIES_NUM ) {
	int f = pCs->train_cmd.frontier;
	assert( pCs->train_cmd.entries[f].rakeID == 0 );
	pE = &pCs->train_cmd.entries[f];
	pCs->train_cmd.frontier++;
      }
    } else
      assert( FALSE );
  }
  return pE;
}

int alloc_train_cmd_entries( TRAIN_COMMAND_ENTRY_PTR es[], int rakeID, int front_blk, int back_blk ) {
  assert( es );
  assert( front_blk > 0 );
  assert( back_blk > 0 );
  SC_ID zones[2] = { END_OF_SCs, END_OF_SCs };
  int r = 0;
  
  int n = which_SC_zones( zones, front_blk, back_blk );
  assert( (n > 0) && (n <= 2) );
  {
    int i;
    for( i = 0; i < n; i++ ) {
      assert( zones[i] != END_OF_SCs );
      es[i] = lkup_train_cmd( rakeID, &toward_SC[zones[i]] );
    }
    assert( i == n );
    r = i;
  }
  return r;
}

TOWARD_SC_CMDSET_PTR emit_train_cmd( SC_ID sc_id ) {
  return NULL;
}

FROM_SC_DATASET_PTR sniff_train_info( SC_ID sc_id ) {
  return NULL;
}

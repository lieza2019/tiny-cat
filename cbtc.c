#include <string.h>
#include <pthread.h>
#include "generic.h"
#include "misc.h"

#define CBTC_C
#include "cbtc.h"
#undef CBTC_C
#include "interlock.h"
#include "sparcs.h"

const char *cnv2str_lkup ( const char *id2str_tbl[], int id ) {
  assert( id2str_tbl );
  assert( id > -1 );
  const char *r = NULL;
  
  int i = 0;
  while( id2str_tbl[i] ) {
    assert( id2str_tbl[i] );
    if( i == id ) {
      r = id2str_tbl[id];
      break;
    }
    i++;
  }
  assert( id <= i );
  return r;
}

#if 0 // *****
static const char *spcode_2_str[] = {
  "SP_NONSENS",
  "SP_73", // JLA_PL1
  "SP_74", // JLA_PL2
  "SP_75", // JLA_PL3
  "SP_D4", // JLA_TB4
  "SP_D9", // JLA_TB3
  "SP_76", // KIKJ_PL2
  "SP_77", // KIKJ_PL1
  "SP_78", // OKBS_PL2
  "SP_79", // OKBS_PL1
  "SP_80", // BTGD_PL2
  "SP_81", // BTGD_PL1
  "SP_D5", // BTGD_TB1
  "SP_D0", // BTGD_TB2
  NULL
};
const char *cnv2str_sp_code ( STOPPING_POINT_CODE sp_code ) {
  return cnv2str_lkup( spcode_2_str, sp_code );
}

STOPPING_POINT_CODE str2_sp_code ( const char *sp_str ) {
  assert( sp_str );
  STOPPING_POINT_CODE r = END_OF_SPs;
  
  const int cmpmax = 16;
  if( ! strncmp( sp_str, "SP_NONSENS", cmpmax ) )
    r = SP_NONSENS;
  else if( ! strncmp( sp_str, "SP_73", cmpmax ) ) // JLA_PL1
    r = SP_73;
  else if( ! strncmp( sp_str, "SP_74", cmpmax ) ) // JLA_PL2
    r = SP_74;
  else if( ! strncmp( sp_str, "SP_75", cmpmax ) ) // JLA_PL3
    r = SP_75;
  else if( ! strncmp( sp_str, "SP_D4", cmpmax ) ) // JLA_TB4
    r = SP_D4;
  else if( ! strncmp( sp_str, "SP_D9", cmpmax ) ) // JLA_TB3
    r = SP_D9;
  else if( ! strncmp( sp_str, "SP_76", cmpmax ) ) // KIKJ_PL2
    r = SP_76;
  else if( ! strncmp( sp_str, "SP_77", cmpmax ) ) // KIKJ_PL1
    r = SP_77;
  else if( ! strncmp( sp_str, "SP_78", cmpmax ) ) // OKBS_PL2
    r = SP_78;
  else if( ! strncmp( sp_str, "SP_79", cmpmax ) ) // OKBS_PL1
    r = SP_79;
  else if( ! strncmp( sp_str, "SP_80", cmpmax ) ) // BTGD_PL2
    r = SP_80;
  else if( ! strncmp( sp_str, "SP_81", cmpmax ) ) // BTGD_PL1
    r = SP_81;
  else if( ! strncmp( sp_str, "SP_D5", cmpmax ) ) // BTGD_TB1
    r = SP_D5;
  else if( ! strncmp( sp_str, "SP_D0", cmpmax ) ) // BTGD_TB2
    r = SP_D0;
  else
    assert( FALSE );
  
  return r;
}

static const char *pltb_2_str[] = {
  "", // see below line.
  "PL1", // PL1 = 1
  "PL2",
  "PL3",
  "TB1",
  "TB2",
  "TB3",
  "TB4",
  NULL
};
const char *cnv2str_pltb_id ( PLTB_ID pltb_id ) {
  return cnv2str_lkup( pltb_2_str, pltb_id );
}

PLTB_ID str2_pltb_id ( const char *pltb_str ) {
  assert( pltb_str );
  PLTB_ID r = END_OF_PLTB_ID;
  
  const int cmpmax = 8;
  if( ! strncmp( pltb_str, "PL1", cmpmax ) )
    r = PL1;
  else if( ! strncmp( pltb_str, "PL2", cmpmax ) )
    r = PL2;
  else if( ! strncmp( pltb_str, "PL3", cmpmax ) )
    r = PL3;
  else if( ! strncmp( pltb_str, "TB1", cmpmax ) )
    r = TB1;
  else if( ! strncmp( pltb_str, "TB2", cmpmax ) )
    r = TB2;
  else if( ! strncmp( pltb_str, "TB3", cmpmax ) )
    r = TB3;
  else if( ! strncmp( pltb_str, "TB4", cmpmax ) )
    r = TB4;
  else
    assert( FALSE );
  
  return r;
}

static const char *stid_2_str[] = {
  "", // see below line.
  "JLA", // JLA = 1
  "KIKJ",
  "OKBS",
  "BTGD",
  NULL
};
const char *cnv2str_st_id ( ST_ID st_id ) {
  return cnv2str_lkup( stid_2_str, st_id );
}

ST_ID str2_st_id ( const char *st_str ) {
  assert( st_str );
  ST_ID r = END_OF_ST_ID;
  
  const int cmpmax = 8;
  if( ! strncmp( st_str, "JLA", cmpmax ) )
    r = JLA;
  else if( ! strncmp( st_str, "KIKJ", cmpmax ) )
    r = KIKJ;
  else if( ! strncmp( st_str, "OKBS", cmpmax ) )
    r = OKBS;
  else if( ! strncmp( st_str, "BTGD", cmpmax ) )
    r = BTGD;
  else
    assert( FALSE );
  
  return r;
}
#endif

TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB ) {
  assert( pB );
  return (TINY_TRAIN_STATE_PTR)pB->residents.ptrains;
}

TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  pB->residents.ptrains = (void *)pT;
  return read_residents_CBTC_BLOCK( pB );
}

TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB ) {
  assert( pB );
  return (TINY_TRAIN_STATE_PTR *)&(pB->residents.ptrains);
}

#if 1 // ***** for ONLY the service for ars.c
TINY_TRAIN_STATE_PTR read_edge_of_residents_CBTC_BLOCK ( CBTC_BLOCK_C_PTR pB ) {
  assert( pB );
  return (TINY_TRAIN_STATE_PTR)pB->residents.edges;
}
#endif
TINY_TRAIN_STATE_PTR read_edge_of_residents_CBTC_BLOCK1 ( CBTC_BLOCK_C_PTR pB, const int which ) {
  assert( pB );
  assert( (-1 < which) && (which < MAX_ADJACENT_BLKS) );
  return (TINY_TRAIN_STATE_PTR)pB->residents.edges[which];
}

#if 0 // *****
TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  pB->residents.edges = (void *)pT;
  if( pT ) {
    assert( pT->misc.occupancy.pblk_forward == pB );
    BOOL elided = FALSE;
    TINY_TRAIN_STATE_PTR *pp = NULL;
    pp = addr_residents_CBTC_BLOCK( pT->misc.occupancy.pblk_forward );
    assert( pp );
    while( *pp ) {
      assert( *pp );
      assert( pT );
      if( *pp == pT ) {
	assert( !elided );
	*pp = pT->misc.occupancy.pNext;
	pT->misc.occupancy.pNext = NULL;
	elided = TRUE;
	continue;
      }
      pp = &(*pp)->misc.occupancy.pNext;
      assert( pp );
      assert( pT->misc.occupancy.pblk_forward == pB );
    }
  }
  return read_edge_of_residents_CBTC_BLOCK( pB );
}
#else
TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, const int which, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  assert( (-1 < which) && (which < MAX_ADJACENT_BLKS) );
  pB->residents.edges[which] = (void *)pT;
  if( pT ) {
    BOOL elided = FALSE;
    TINY_TRAIN_STATE_PTR *pp = NULL;
    if( pB == pT->misc.occupancy.pblk_forward )
      pp = addr_residents_CBTC_BLOCK( pT->misc.occupancy.pblk_forward );
    else {
      assert( pB == pT->misc.occupancy.pblk_back );
      pp = addr_residents_CBTC_BLOCK( pT->misc.occupancy.pblk_back );
    }
    assert( pp );
    while( *pp ) {
      assert( *pp );
      assert( pT );
      if( *pp == pT ) {
	assert( !elided );
	*pp = pT->misc.occupancy.pNext;
	pT->misc.occupancy.pNext = NULL;
	elided = TRUE;
	continue;
      }
      pp = &(*pp)->misc.occupancy.pNext;
      assert( pp );
    }
  }
  return read_edge_of_residents_CBTC_BLOCK1( pB, which );
}
#endif

static CBTC_BLOCK_PTR blkname2_cbtc_block_prof[65536];
static CBTC_BLOCK_PTR virtblk2_cbtc_block_prof[65536];
void cons_lkuptbl_cbtc_block_prof ( void ) {
  const CBTC_BLOCK_PTR plim_sup = (CBTC_BLOCK_PTR)((unsigned char *)block_state + (int)sizeof(block_state));
 
  // for the lookup-table of blkname2_cbtc_block_prof: block_name -> block_prof
  {
    int i = 0;
    assert( i == 0 );
    while( block_state[i].virt_block_name < END_OF_CBTC_BLOCKs ) {
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
    assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
  }
  
  // for the lookup-table of virtblk2_cbtc_block_prof: virtual_block_name -> block_prof
  {
    int i = 0;
    assert( i == 0 );
    while( block_state[i].virt_block_name < END_OF_CBTC_BLOCKs ) {
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
    assert( block_state[i].virt_block_name == END_OF_CBTC_BLOCKs );
  }
}

CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name ) {
  return blkname2_cbtc_block_prof[block_name];
}

CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname ) {
  return virtblk2_cbtc_block_prof[virt_blkname];
}

static CBTC_BLOCK_C_PTR sp2_block[END_OF_SPs];
void cons_lkuptbl_sp2_block ( void ) {
  int i = 0;
  while( block_state[i].virt_block_name < END_OF_CBTC_BLOCKs ) {
    if( block_state[i].sp.has_sp ) {
      switch( block_state[i].sp.stop_detect_type ) {
      case P0_COUPLING:
	sp2_block[block_state[i].sp.sp_code] = &block_state[i];
	assert( block_state[i].sp.stop_detect_cond.paired_blk == VB_NONSENS );
	assert( ! block_state[i].sp.stop_detect_cond.ppaired_blk );
	{
	  int j = i + 1;
	  while( block_state[j].virt_block_name < END_OF_CBTC_BLOCKs ) {
	    if( block_state[j].sp.has_sp )
	      assert( block_state[j].sp.sp_code != block_state[i].sp.sp_code );
	    j++;
	  }
	}
	break;
      case VIRTUAL_P0:	
	break;
      case END_OF_STOP_DETECTION_TYPES:
	/* fall thru. */
      default:
	assert( FALSE );
      }
    }
    i++;
  }
}

CBTC_BLOCK_C_PTR lookup_block_of_sp ( STOPPING_POINT_CODE sp ) {
  assert( (sp >= 0) && (sp < END_OF_SPs) );
  return sp2_block[sp];
}

static BOOL creteria_2_elide ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT ) {
  assert( pB );
  assert( pB->block_name > 0 );
  assert( pB->virt_block_name < END_OF_CBTC_BLOCKs );
  assert( pT );
  BOOL r = FALSE;
  
  if( pT->omit )
    r = TRUE;
  else {
    if( ! pT->pTI )
      r = TRUE;
    else {
      const unsigned short blk_name_forward = TRAIN_INFO_OCCUPIED_BLK_FORWARD( *pT->pTI );
      const unsigned short blk_name_back = TRAIN_INFO_OCCUPIED_BLK_BACK( *pT->pTI );
      CBTC_BLOCK_PTR pblk_forward = NULL;
      CBTC_BLOCK_PTR pblk_back = NULL;
      pblk_forward = lookup_cbtc_block_prof( blk_name_forward );
      if( pblk_forward ) {
	pblk_back = lookup_cbtc_block_prof( blk_name_back );
	if( pblk_back ) {
	  assert( pblk_forward );
	  assert( pblk_back );
	  assert( pB );
	  r = !((pB->block_name == pblk_forward->block_name) || (pB->block_name == pblk_back->block_name));
	} else {
	  errorF( "%d: unknown cbtc block detected as back, of train %3d.\n", blk_name_back, pT->rakeID );
	  r = pB->block_name != pblk_forward->block_name;
	}
      } else {
	errorF( "%d: nknown cbtc block detected as forward, of train %3d.\n", blk_name_forward, pT->rakeID );
	r = TRUE;
      }
    }
  }
  
  return r;
}

void purge_block_restrains ( void ) {  
  int r_mutex = -1;
  
  r_mutex = pthread_mutex_lock( &cbtc_stat_infos_mutex );
  if( r_mutex ) {
    assert( FALSE );
  } else {
    int i = 0;
    while( block_state[i].virt_block_name < END_OF_CBTC_BLOCKs ) {
      CBTC_BLOCK_PTR pB = &block_state[i];
      assert( pB );
      assert( (pB->block_name > 0) && (pB->virt_block_name < END_OF_CBTC_BLOCKs) );
      TINY_TRAIN_STATE_PTR *pp = NULL;
      pp = addr_residents_CBTC_BLOCK( pB );
      assert( pp );
      while( *pp ) {
	assert( *pp );
	if( creteria_2_elide( pB, *pp ) ) {
	  TINY_TRAIN_STATE_PTR w = NULL;
	  w = *pp;
	  assert( w );
	  *pp = w->misc.occupancy.pNext;
	  w->misc.occupancy.pNext = NULL;
	  continue;
	}
	pp = &(*pp)->misc.occupancy.pNext;
	assert( pp );
      }
      {
	assert( pB );
	int i;
	for( i = 0; i < MAX_ADJACENT_BLKS; i++ ) {
	  TINY_TRAIN_STATE_PTR p = read_edge_of_residents_CBTC_BLOCK1( pB, i );
	  if( p ) {
	    if( creteria_2_elide( pB, p ) )
	      border_residents_CBTC_BLOCK( pB, i, NULL );
	  }
	}
      }
      if( pB->residents.overwhelmed ) {
	TINY_TRAIN_STATE_PTR p = (TINY_TRAIN_STATE_PTR)(pB->residents.overwhelmed);
	BOOL found = FALSE;
	int i;
	for( i = 0; i < MAX_OPAQUE_BLKS; i++ )
	  if( p->misc.occupancy.opaque_blks[i] == pB ) {
	    found = TRUE;
	    break;
	  }
	if( !found )
	  pB->residents.overwhelmed = NULL;
      }
      i++;
    }
    r_mutex = -1;
    r_mutex = pthread_mutex_unlock( &cbtc_stat_infos_mutex );
    assert( !r_mutex );
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
	while( block_state[j].virt_block_name < END_OF_CBTC_BLOCKs ) {
	  CBTC_BLOCK_PTR q = &block_state[j];
	  assert( (q >= block_state) && (q < plim_sup) );
	  assert( q->block_name != (unsigned short)i );
	  j++;
	}
	assert( block_state[j].virt_block_name == END_OF_CBTC_BLOCKs );
      }
    }
    {
      int k = 0;
      assert( k == 0 );
      while( block_state[k].virt_block_name < END_OF_CBTC_BLOCKs ) {
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
	while( block_state[j].virt_block_name < END_OF_CBTC_BLOCKs ) {
	  CBTC_BLOCK_PTR q = &block_state[j];
	  assert( (q >= block_state) && (q < plim_sup) );
	  assert( q->virt_block_name != (CBTC_BLOCK_ID)i );
	  j++;
	}
	assert( block_state[j].virt_block_name == END_OF_CBTC_BLOCKs );
      }
    }
    {
      int k = 0;
      assert( k == 0 );
      while( block_state[k].virt_block_name < END_OF_CBTC_BLOCKs ) {
	assert( block_state[k].misc.msc_flg2 );
	k++;
      }
      assert( block_state[k].virt_block_name == END_OF_CBTC_BLOCKs );
    }
  }
  
  return 0;
}
#endif

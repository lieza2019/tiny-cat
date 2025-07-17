/*
 * establish database for track profile.
 * construct parsing-platfrom for route_rel table.
 * construct database for route profile.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../generic.h"
#include "../../misc.h"
#define CBTC_C
#include "../../cbtc.h"
#undef CBTC_C
#include "../../interlock.h"

#define ERR_FAILED_ALLOC_WORKMEM 3
#define ERR_FAILED_CONS_ILSYM_DATABASE 1
#define ERR_FAILED_OPEN_ILTBL_TRACKS 2

#define TRACK_PROF_DECL_MAXNUM 1024
#define ROUTE_PROF_DECL_MAXNUM 256

#define TRACK_NAME_MAXLEN 16
#define TRACK_BOUNDALIGN_MAXLEN 8

#define ROUTE_NAME_MAXLEN 32
#define ROUTE_MAX_CTRLTRACKS 32
#define ROUTE_MAX_APPTRACKS 32

#define ILCOND_IDENT_MAXLEN 256

extern int par_csv_iltbl ( char *bufs[], const int nbufs, FILE *fp_src );

typedef enum track_bound {
  BOUND_DOWN = 1,
  BOUND_UP,
  BOUND_UNKNOWN
} TRACK_BOUND;
static const char *trbound2_str[] = {
  "HAS_NO_BOUND",
  "BOUND_DOWN",
  "BOUND_UP",
  "BOUND_UNKNOWN",
  NULL
};
const char *cnv2str_trbound ( TRACK_BOUND bound ) {
  return cnv2str_lkup( trbound2_str, bound );
}

struct fixed_pos {
  CBTC_BLOCK_PTR pprof;
  int npos;
  BLK_LINKAGE_PTR pos[MAX_ADJACENT_BLKS];
};
struct track_sr {
  BOOL defined;
  char sr_name[CBI_STAT_IDENT_LEN + 1];
  CBI_STAT_ATTR_PTR psr_attr;
};
typedef struct track_prof {
  char track_name[CBI_STAT_IDENT_LEN + 1];
  CBI_STAT_ATTR_PTR ptr_attr;
  struct {
    int nblks;
    CBTC_BLOCK_PTR pblk_profs[MAX_TRACK_BLOCKS];
  } consists_blks;
  struct {
    int nblks;
    struct fixed_pos pblk_fixes[MAX_TRACK_BLOCKS];
  } hardbonds;
  struct {
    struct track_sr TLSR, TRSR;
    struct track_sr sTLSR, sTRSR;
    struct track_sr eTLSR, eTRSR;
    struct track_sr kTLSR, kTRSR;
  } sr;
  TRACK_BOUND tr_bound;
  struct track_prof *pNext;
} TRACK_PROF, *TRACK_PROF_PTR;

struct route_tr {
  char tr_name[CBI_STAT_IDENT_LEN + 1];
  TRACK_PROF_PTR tr_prof;
};
typedef struct route_prof {
  char route_name[CBI_STAT_IDENT_LEN + 1];
  struct {
    int ntrs;
    struct route_tr tr[ROUTE_MAX_APPTRACKS];
    struct route_tr *porigin;
  } apps;
  //struct route_tr origin;
  struct {
    int ntrs;
    struct route_tr tr[ROUTE_MAX_CTRLTRACKS];
    struct route_tr ahead, *pahead;
  } ctrls;
} ROUTE_PROF, *ROUTE_PROF_PTR;

static struct {
  struct {
    TRACK_PROF_PTR track_profs;
    TRACK_PROF_PTR pavail;
    TRACK_PROF_PTR pprof_sets[END_OF_ST_ID];
  } tracks;
  struct {
    ROUTE_PROF_PTR route_profs;
    ROUTE_PROF_PTR pavail;
  } routes;
} tracks_routes_prof = {};

static TRACK_PROF_PTR lkup_track_prof ( const char *ptr_name ) {
  assert( ptr_name );
  TRACK_PROF_PTR r = NULL;
  
  TRACK_PROF_PTR pprof = tracks_routes_prof.tracks.track_profs;
  while( pprof < tracks_routes_prof.tracks.pavail ) {
    assert( pprof );
    assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
    if( strncmp( pprof->track_name, ptr_name, TRACK_NAME_MAXLEN ) == 0 ) {
      r = pprof;
      break;
    }
    pprof++;
  }
  return r;
}

static ROUTE_PROF_PTR lkup_route_prof ( const char *pro_name ) {
  assert( pro_name );
  ROUTE_PROF_PTR r = NULL;
  
  ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.route_profs;
  while( pprof < tracks_routes_prof.routes.pavail ) {
    assert( pprof );
    assert( ROUTE_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
    if( strncmp( pprof->route_name, pro_name, ROUTE_NAME_MAXLEN ) == 0 ) {
      r = pprof;
      break;
    }
    pprof++;
  }
  return r;
}

static void skip_chr ( FILE *fp_src ) {
  assert( fp_src );
  if( !ferror( fp_src ) ) {
    while( !feof( fp_src ) ) {
      int c;
      c = fgetc( fp_src );
      if( c == '\n' )
	break;
      if( ferror( fp_src ) )
	break;
    }
  }
}

static BOOL track_prof_sr ( FILE *fp_out, struct track_sr *psr, char *ptr_name, const char * psfx_sr ) {
  assert( psr );
  assert( ptr_name );
  assert( psfx_sr );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  BOOL r = FALSE;
  
  char idstr[ILCOND_IDENT_MAXLEN + 1];
  strncpy( idstr, ptr_name, ILCOND_IDENT_MAXLEN );
  strcat( idstr, psfx_sr );
  idstr[ILCOND_IDENT_MAXLEN] = 0;
  {
    CBI_STAT_ATTR_PTR pattr = NULL;
    pattr = conslt_cbi_code_tbl( idstr );
    if( pattr ) {
      assert( !strncmp( cnv2str_il_sym( pattr->id ), pattr->ident, ILCOND_IDENT_MAXLEN ) ); // *****
      if( !strncmp( pattr->ident, idstr, ILCOND_IDENT_MAXLEN ) ) {	
	psr->defined = TRUE;
	strncpy( psr->sr_name, idstr, CBI_STAT_IDENT_LEN );
	psr->psr_attr = pattr;
	fprintf( fp_out, "TRUE, " );
	fprintf( fp_out, "%s, %s", psfx_sr, psr->sr_name );
	r = TRUE;
      }
    }
  }
  if( !r ) {
    psr->defined = FALSE;
    psr->psr_attr = NULL;
    fprintf( fp_out, "FALSE" );
  }
  return r;
}

static int linking ( struct fixed_pos blks[], int nblks, LINX_BONDAGE_KIND bind ) {
  assert( blks );
  assert( nblks <= MAX_TRACK_BLOCKS );
  int cnt = 0;
  if( nblks > 1 ) {
    struct fixed_pos *pblk = &blks[0];
    int i;
    assert( pblk );
    for( i = 0; i < pblk->npos; i++ ) {
      int j;
      if( ! pblk->pos[i] )
	continue;
      assert( pblk->pos[i]->pmorph );
      assert( pblk->pos[i]->pmorph->pblock );
      for( j = 1; j < nblks; j++ ) {
	struct fixed_pos *pb = &blks[j];
	BOOL found = FALSE;	
	int k;
	assert( pb );
	for( k = 0; k < pb->npos; k++ ) {
	  if( ! pb->pos[k] )
	    continue;
	  assert( pb->pos[k]->pmorph );
	  assert( pb->pos[k]->pmorph->pblock );
	  if( (pblk->pos[i]->neigh_blk == pb->pos[k]->pmorph->pblock->block_name) &&
	      (pb->pos[k]->neigh_blk == pblk->pos[i]->pmorph->pblock->block_name) ) {
	    BLK_LINKAGE_PTR pl = pblk->pos[i];
	    do {
	      assert( pl );
	      pl->bond.pln_neigh = pb->pos[k];
	      pl->bond.kind = bind;
	      pl = pl->pNext;
	    } while( pl != pblk->pos[i] );
	    pl = pb->pos[k];
	    do {
	      pl->bond.pln_neigh = pblk->pos[i];
	      pl->bond.kind = bind;
	      pl = pl->pNext;
	    } while( pl != pb->pos[k] );
	    pblk->pos[i] = NULL;
	    pb->pos[k] = NULL;
	    cnt++;
	    found = TRUE;
	    break;
	  }
	}
	if( found )
	  break;
      }
    }
    cnt += linking( &blks[1], (nblks - 1), bind );
  }
  return cnt;
}
static int link_internal_blks ( CBTC_BLOCK_PTR profs[], struct fixed_pos fixes[], const int nblks ) {
  assert( profs );
  assert( fixes );
  assert( nblks <= MAX_TRACK_BLOCKS );
  int cnt = 0;
  
  int i;
  for( i = 0; i < nblks; i++ ) {
    assert( i < MAX_TRACK_BLOCKS );
    int n = -1;
    n = enum_fixed_branches( profs[i], fixes[cnt].pos, MAX_ADJACENT_BLKS );    
    if( n > 0 ) {
      fixes[cnt].pprof = profs[i];
      fixes[cnt].npos = n;
      cnt++;
    }
  }
  linking( fixes, cnt, LINK_HARD );
  return cnt;
}

static struct route_tr *trylnk_orgahd ( struct route_tr app_trs[], const int napp_trs, struct fixed_pos pahd_blks[], const int nahd_blks ) {
  assert( app_trs );
  assert( napp_trs <= ROUTE_MAX_APPTRACKS );
  assert( pahd_blks );
  assert( nahd_blks <= MAX_TRACK_BLOCKS );
  struct fixed_pos fixes[2] = {};
  
  int i;
  for( i = 0; i < napp_trs; i++ ) {
    assert( strnlen( app_trs[i].tr_name, CBI_STAT_IDENT_LEN ) > 0 );
    assert( app_trs[i].tr_prof );
    int j;
    for( j = 0; j < app_trs[i].tr_prof->hardbonds.nblks; j++ ) {
      int k;
      fixes[0] = app_trs[i].tr_prof->hardbonds.pblk_fixes[j];
      for( k = 0; k < nahd_blks; k++ ) {
	int n = -1;
	fixes[1] = pahd_blks[k];
	n = linking( fixes, 2, LINK_SEMIHARD );
	if( n > 0 ) {
	  if( n == 1 ) {
	    BOOL found = FALSE;
	    int l;	      
	    for( l = 0; l < app_trs[i].tr_prof->consists_blks.nblks; l++ ) {
	      if( app_trs[i].tr_prof->consists_blks.pblk_profs[l] == fixes[0].pprof ) {
		found = TRUE;
		break;
	      }
	    }
	    assert( found );
	  } else
#if 1 // *****	  
	    assert( FALSE );
#endif
	  return &app_trs[i];
	}
      }
    }
  }
  return NULL;
}

static struct route_tr *link_orgahd_blks ( struct route_tr app_trs[], const int napps, TRACK_PROF_PTR pahd_tr ) {
  assert( app_trs );
  assert( napps <= ROUTE_MAX_APPTRACKS );
  assert( pahd_tr );
  struct route_tr *porg_tr = NULL;
  
  porg_tr = trylnk_orgahd( app_trs, napps, pahd_tr->hardbonds.pblk_fixes, pahd_tr->hardbonds.nblks );
  if( porg_tr ) {
    struct fixed_pos *pahd_added = &pahd_tr->hardbonds.pblk_fixes[pahd_tr->hardbonds.nblks];
    assert( pahd_added < &pahd_tr->hardbonds.pblk_fixes[MAX_TRACK_BLOCKS] );
    int i;
    for( i = 0; i < pahd_tr->consists_blks.nblks; i++ ) {
      CBTC_BLOCK_PTR pblk = pahd_tr->consists_blks.pblk_profs[i];
      assert( pblk );
      int j;
      for( j = 0; j < pblk->shape.num_morphs; j++ ) {
	BLK_MORPH_PTR pmor = &pblk->shape.morphs[j];
	assert( pmor );
	int k;
	for( k = 0; k < pmor->num_links; k++ ) {
	  BLK_LINKAGE_PTR plnk = &pmor->linkages[k];
	  assert( plnk );
	  if( plnk->bond.pln_neigh ) {
	    assert( plnk->bond.kind != LINK_NONE );
	    continue;
	  }
	  assert( plnk->bond.kind == LINK_NONE );
	  pahd_added->pprof = pblk;
	  pahd_added->pos[0] = plnk;
	  pahd_added->npos = 1;
	  porg_tr = trylnk_orgahd( app_trs, napps, pahd_added, 1 );
	  if( porg_tr ) {
	    BOOL found = FALSE;
	    int l;
	    for( l = 0; l < pahd_tr->hardbonds.nblks; l++ ) {
	      assert( pblk );
	      struct fixed_pos *p = &pahd_tr->hardbonds.pblk_fixes[l];
	      assert( p->pprof );
	      if( p->pprof == pblk ) {
		assert( p->npos < MAX_ADJACENT_BLKS );
		p->pos[p->npos] = pahd_added->pos[0];
		p->npos++;
		found = TRUE;
		break;
	      }
	      if( !found ) {
		assert( pahd_tr->hardbonds.nblks < MAX_TRACK_BLOCKS );
		pahd_tr->hardbonds.nblks++;
	      }
	    }
	  }
	}
      }
    }
  }
  return porg_tr;
}

static int track_prof_blks ( CBTC_BLOCK_PTR *pphead, char *ptr_name ) {
  assert( pphead );
  assert( ptr_name );
  int cnt = 0;  
  CBI_STAT_ATTR_PTR pattr = NULL;
  
  char idstr[ILCOND_IDENT_MAXLEN + 1];
  strncpy( idstr, ptr_name, ILCOND_IDENT_MAXLEN );
  strcat( idstr, "_TR" );
  pattr = conslt_cbi_code_tbl( idstr );
  if( pattr ) {
    CBTC_BLOCK_PTR ptail = NULL;
    int i = 0;
    *pphead = NULL;
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      if( block_state[i].belonging_tr.track == pattr->id ) {
	if( *pphead ) {
	  assert( ptail );
	  ptail->belonging_tr.pNext = &block_state[i];	    
	} else {
	  assert( !ptail );
	  *pphead = &block_state[i];
	}
	block_state[i].belonging_tr.pNext = NULL;
	ptail = &block_state[i];
	cnt++;
      }
      i++;
    }
    assert( block_state[i].block_name == 0 );
  }
  return cnt;
}

static TRACK_PROF_PTR emit_track_prof ( FILE *fp_out, TRACK_PROF_PTR pprof, char *ptr_name, char *pbounds ) {
  assert( pprof );
  assert( ptr_name );
  assert( pbounds );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  fprintf( fp_out, "{ _TRACK, " );
  snprintf( pprof->track_name, CBI_STAT_IDENT_LEN, "%s_TR", ptr_name );
  pprof->ptr_attr = conslt_cbi_code_tbl( pprof->track_name );
  fprintf( fp_out, "\"%s\", ", pprof->track_name );
  fprintf( fp_out, "%s, ", pprof->track_name );
  // cbtc
  fprintf( fp_out, "{" );
  {
    int nblks = -1;
    CBTC_BLOCK_PTR pblk_prof = NULL;
    nblks = track_prof_blks( &pblk_prof, ptr_name );
    assert( nblks > -1 );
    pprof->consists_blks.nblks = nblks;
    fprintf( fp_out, "%d", pprof->consists_blks.nblks );
    if( nblks > 0 ) {
      assert( pblk_prof );
      int n = nblks;
      int i = 0;
      fprintf( fp_out, ", {" );
      do {
	assert( n > 0 );
	if( n < nblks )
	  fprintf( fp_out, ", " );
	n--;
	pprof->consists_blks.pblk_profs[i] = pblk_prof;
	fprintf( fp_out, "%s", pprof->consists_blks.pblk_profs[i]->virt_blkname_str );
	i++;
	pblk_prof = pblk_prof->belonging_tr.pNext;
      } while( pblk_prof );
      assert( n == 0 );
      assert( i == nblks );
      fprintf( fp_out, "}" );
      
      pprof->hardbonds.nblks = link_internal_blks( pprof->consists_blks.pblk_profs, pprof->hardbonds.pblk_fixes, pprof->consists_blks.nblks );
    }    
  }
  fprintf( fp_out, "}, " );
  // lock
  fprintf( fp_out, "{" );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.TLSR, ptr_name, "_TLSR" ); // TLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.TRSR, ptr_name, "_TRSR" ); // TRSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.sTLSR, ptr_name, "_sTLSR" ); // sTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.sTRSR, ptr_name, "_sTRSR" ); // sTRSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.eTLSR, ptr_name, "_eTLSR" ); // eTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.eTRSR, ptr_name, "_eTRSR" ); // eTRSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.kTLSR, ptr_name, "_kTLSR" ); // kTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_sr( fp_out, &pprof->sr.kTRSR, ptr_name, "_kTRSR" ); // kTRSR
  fprintf( fp_out, "}" );
  fprintf( fp_out, "}" );
  fprintf( fp_out, "},\n" );
  
  if( !strcmp( pbounds, "Down" ) )
    pprof->tr_bound = BOUND_DOWN;
  else if( !strcmp( pbounds, "Up" ) )
    pprof->tr_bound = BOUND_UP;
  else
    pprof->tr_bound = BOUND_UNKNOWN;
  return pprof;
}

static int emit_track_dataset ( TRACK_PROF_PTR *pprofs, FILE *fp_out, FILE *fp_src ) {
  assert( pprofs );
  assert( fp_out );
  assert( fp_src );
  TRACK_PROF_PTR pprev = NULL;
  int cnt = 0;
  
  assert( !ferror( fp_out ) );
  assert( !ferror( fp_src ) );
  while( !feof(fp_src) ) {
    assert( cnt < TRACK_PROF_DECL_MAXNUM );
    int n = -1;
    char seq[5 + 1];
    char tr_name[TRACK_NAME_MAXLEN + 1];
    char bounds[TRACK_BOUNDALIGN_MAXLEN + 1];
    seq[5] = 0;
    tr_name[TRACK_NAME_MAXLEN] = 0;
    bounds[TRACK_BOUNDALIGN_MAXLEN] = 0;
    strcpy( seq, "" );
    strcpy( tr_name, "T" );
    strcpy( bounds, "" );
    {
      char *strs[5];
      char dc[256 + 1]; // dont cure.
      strs[0] = seq;
      strs[1] = &tr_name[1];
      strs[2] = dc;
      strs[3] = dc;
      strs[4] = bounds;
      n = par_csv_iltbl( strs, 5, fp_src );
      if( ferror( fp_src ) )
	break;
    }
    if( n >= 5 ) {
      assert( tr_name[0] == 'T' );
      if( strnlen( tr_name, TRACK_NAME_MAXLEN ) > 1 ) {
	TRACK_PROF_PTR pprof = NULL;
	GEN_INDENT( fp_out, 1, 2 );
	assert( tracks_routes_prof.tracks.pavail < &tracks_routes_prof.tracks.track_profs[TRACK_PROF_DECL_MAXNUM] );
	pprof = emit_track_prof( fp_out, tracks_routes_prof.tracks.pavail, tr_name, bounds );
	assert( pprof == tracks_routes_prof.tracks.pavail );	
	if( ferror( fp_out ) )
	  break;
	cnt++;
#if 1 // *****
	{
	  printf( "(seq, tr_name, bound, [consist_blks]): (%s, %s, %s, [", seq, pprof->track_name, cnv2str_trbound(pprof->tr_bound) );
	  int i;
	  for( i = 0; i < pprof->consists_blks.nblks; i++ ) {
	    CBTC_BLOCK_PTR pblk = pprof->consists_blks.pblk_profs[i];
	    assert( pblk );
	    if( i > 0 )
	      printf( ", " );
	    assert( pblk->virt_blkname_str );
	    printf( "%s", pblk->virt_blkname_str );
	  }
	  printf( "])\n" );
	  for( i = 0; i < pprof->consists_blks.nblks; i++ ) {
	    CBTC_BLOCK_PTR pblk = pprof->consists_blks.pblk_profs[i];
	    assert( pblk );
	    print_block_prof( stdout, pblk );
	  }
	}
#endif
	if( cnt == 1 ) {
	  assert( !pprev );
	  *pprofs = tracks_routes_prof.tracks.pavail;
	} else {
	  assert( pprev );
	  assert( !pprev->pNext );
	  pprev->pNext = tracks_routes_prof.tracks.pavail;
	}
	(tracks_routes_prof.tracks.pavail)->pNext = NULL;
	pprev = tracks_routes_prof.tracks.pavail;
	tracks_routes_prof.tracks.pavail++;
      }
    }
    skip_chr( fp_src );
  }
  return cnt;
}

static void emit_track_dataset_prolog ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  fprintf( fp_out, "#ifdef TRACK_ATTRIB_DEFINITION\n" );
  fprintf( fp_out, "#ifdef INTERLOCK_C\n" );
  fprintf( fp_out, "TRACK track_dataset_def[] = {\n" );
}
static void emit_track_dataset_epilog ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  GEN_INDENT( fp_out, 1, 2 );
  fprintf( fp_out, "{ END_OF_CBI_STAT_KIND }\n" );
  fprintf( fp_out, "};\n" );
  fprintf( fp_out, "#else\n" );
  fprintf( fp_out, "extern TRACK track_dataset_def[];\n" );
  fprintf( fp_out, "#endif\n" );
  fprintf( fp_out, "#endif // TRACK_ATTRIB_DEFINITION\n" );
}

#if 0 // *****
static BLK_LINKAGE_PTR _linking ( BLK_LINKAGE_PTR pbra, CBTC_BLOCK_PTR pblks, const int nblks ) {
  assert( pbra );
  assert( pblks );
  assert( nblks >= 0 );
  int i;
  
  pbra->pln_neigh = NULL;
  for( i = 0; i < nblks; i++ ) {    
    int j;
    assert( pblks[i].shape.num_morphs < 3 );
    for( j = 0; j < pblks[i].shape.num_morphs; j++ ) {
      BLK_MORPH_PTR pmor = &pblks[i].shape.morphs[j];      
      int k;
      assert( pmor );
      assert( pmor->num_links < 3 );
      for( k = 0; k < pmor->num_links; k++ ) {
	BLK_LINKAGE_PTR plnk = &pmor->linkages[k];
	assert( plnk );
	if( pbra->neigh_blk == plnk->neigh_blk ) {
	  pbra->pln_neigh = plnk;
	  return pbra->pln_neigh;
	}
      }
    }
  }
  assert( !pbra->pln_neigh );
  return NULL;
}
static void cons_block_linkages ( TRACK_PROF_PTR pprofs ) {
  assert( pprofs );
  TRACK_PROF_PTR pprof = pprofs;
  
  while( pprof ) {
    assert( pprof );
    int i;
    for( i = 0; i < (pprof->consists_blks.nblks - 1); i++ ) {
      CBTC_BLOCK_PTR pblk = pprof->consists_blks.pblk_profs[i];
      int j;
      assert( pblk );
      for( j = 0; j < pblk->shape.num_morphs; j++ ) {
	BLK_MORPH_PTR pmor = &pblk->shape.morphs[j];	
	int k;
	assert( pmor );
	assert( pmor->num_links < 3 );
	for( k = 0; k < pmor->num_links; k++ ) {	  
	  _linking( &pmor->linkages[k], pprof->consists_blks.pblk_profs[i + 1], ((pprof->consists_blks.nblks - 1) - i) );
	  ;
	}	
      }
    }
    pprof = pprof->pNext;
  }
}
#endif

static int gen_track_dataset ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  int r = ERR_FAILED_OPEN_ILTBL_TRACKS;
  FILE *fp_src = NULL;
  
  emit_track_dataset_prolog( fp_out );
  fp_src = fopen( "BCGN_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {      
      r = emit_track_dataset( &tracks_routes_prof.tracks.pprof_sets[BTGD], fp_out, fp_src );     
    }
  }
  r = ERR_FAILED_OPEN_ILTBL_TRACKS;
  fp_src = fopen( "JLA_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {
      r = emit_track_dataset( &tracks_routes_prof.tracks.pprof_sets[JLA], fp_out, fp_src );
    }
  }
  emit_track_dataset_epilog( fp_out );
  return r;
}

static void emit_route_dataset_prolog ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  fprintf( fp_out, "#ifdef ROUTE_ATTRIB_DEFINITION\n" );
  fprintf( fp_out, "#ifdef INTERLOCK_C\n" );
  fprintf( fp_out, "ROUTE route_dataset_def[] = {\n" );
}
static void emit_route_dataset_epilog ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  GEN_INDENT( fp_out, 1, 2 );
  fprintf( fp_out, "{ END_OF_CBI_STAT_KIND, END_OF_ROUTE_KINDS }\n" );
  fprintf( fp_out, "};\n" );
  fprintf( fp_out, "#else\n" );
  fprintf( fp_out, "extern ROUTE route_dataset_def[];\n" );
  fprintf( fp_out, "#endif\n" );
  fprintf( fp_out, "#endif // ROUTE_ATTRIB_DEFINITION\n" );
}

static int read_iltbl_signal ( FILE *fp_out, FILE *fp_src ) {
  assert( fp_out );
  assert( fp_src );
  ROUTE_PROF_PTR pprof = NULL;
  int cnt = 0;

  assert( !ferror( fp_out ) );
  assert( !ferror( fp_src ) );
  while( !feof( fp_src ) ) {
    int n = -1;
    char seq[5 + 1];
    char ro_name[ROUTE_NAME_MAXLEN + 1];
    char ctrl_tr[TRACK_NAME_MAXLEN + 1];
    seq[5] = 0;
    ro_name[ROUTE_NAME_MAXLEN] = 0;
    ctrl_tr[TRACK_NAME_MAXLEN] = 0;
    strcpy( seq, "" );
    strcpy( ro_name, "" );
    strcpy( ctrl_tr, "T" );
    {
      char *strs[11];
      char dc[256 + 1]; // dont cure.
      dc[256] = 0;
      strs[0] = seq;;
      strs[1] = dc;
      strs[2] = dc;
      strs[3] = dc;
      strs[4] = dc;
      strs[5] = ro_name;
      strs[6] = dc;
      strs[7] = dc;
      strs[8] = dc;
      strs[9] = dc;
      strs[10] = &ctrl_tr[1];
      n = par_csv_iltbl( strs, 11, fp_src );
      assert( ctrl_tr[0] == 'T' );
    }
    if( n > 1 ) {
      if( strncmp( ro_name, "", ROUTE_NAME_MAXLEN ) ) {
	if( cnt > 0 )
	  tracks_routes_prof.routes.pavail++;
	assert( tracks_routes_prof.routes.pavail < &tracks_routes_prof.routes.route_profs[ROUTE_PROF_DECL_MAXNUM] );
	pprof = tracks_routes_prof.routes.pavail;
	assert( ROUTE_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->route_name, ro_name, ROUTE_NAME_MAXLEN );
	pprof->ctrls.ntrs = 0;
	strcpy( pprof->ctrls.ahead.tr_name, "" );
	pprof->ctrls.ahead.tr_prof = NULL;
	pprof->ctrls.pahead = NULL;
	cnt++;
      }
      assert( pprof );
      if( strnlen( ctrl_tr, TRACK_NAME_MAXLEN ) > 1 ) {
	const int i = pprof->ctrls.ntrs;
	assert( strnlen( ctrl_tr, TRACK_NAME_MAXLEN ) < (TRACK_NAME_MAXLEN - strlen("_TR")) );
	strncat( ctrl_tr, "_TR", TRACK_NAME_MAXLEN );
	assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->ctrls.tr[i].tr_name, ctrl_tr, TRACK_NAME_MAXLEN );
	pprof->ctrls.tr[i].tr_prof = lkup_track_prof( pprof->ctrls.tr[i].tr_name );
#if 1 // *****
	assert( pprof->ctrls.tr[i].tr_prof );
#endif
	pprof->ctrls.ntrs++;
      }
#if 0 // *****
      printf( "(seq, route, [ctrl_tracks]): (%s, %s, [", seq, pprof->route_name );
      {	
	int i = 0;
	while( i < pprof->ctrls.ntrs ) {
	  if( i > 0 )
	    printf( ", " );
	  printf( "%s", pprof->ctrls.tr[i].tr_name );
	  i++;
	}
      }
      printf( "])\n" );
#endif
    }
    skip_chr( fp_src );
  }
  if( cnt > 0 )
    tracks_routes_prof.routes.pavail++;
  return cnt;
}

static int read_iltbl_routerel ( FILE *fp_out, FILE *fp_src ) {
  assert( fp_out );
  assert( fp_src );
  extern int par_csv_iltbl ( char *bufs[], const int nbufs, FILE *fp_src );
  
  ROUTE_PROF_PTR pprof = NULL;
  int cnt = -1;
  
  assert( !ferror( fp_out ) );
  assert( !ferror( fp_src ) );
  while( !feof( fp_src ) ) {
    int n = -1;
    char seq[5 + 1];
    char ro_name[ROUTE_NAME_MAXLEN + 1];
    char app_tr[TRACK_NAME_MAXLEN + 1];
#if 1 // *****
    char ahd_tr[TRACK_NAME_MAXLEN + 1];
#endif
    seq[5] = 0;
    ro_name[ROUTE_NAME_MAXLEN] = 0;
    app_tr[TRACK_NAME_MAXLEN] = 0;
    strcpy( seq, "" );
    strcpy( ro_name, "" );
    strcpy( app_tr, "T" );
    strcpy( ahd_tr, "T" );
    {
#if 0 // *****
      char *strs[5]; // *****
#else
      char *strs[14];
#endif
      char dc[256 + 1]; // dont cure.
      dc[256] = 0;
      strs[0] = seq;
      strs[1] = dc;
      strs[2] = ro_name;
      strs[3] = dc;
      strs[4] = &app_tr[1];
#if 1 // *****
      strs[5] = dc;
      strs[6] = dc;
      strs[7] = dc;
      strs[8] = dc;
      strs[9] = dc;
      strs[10] = dc;
      strs[11] = dc;
      strs[12] = dc;
      strs[13] = &ahd_tr[1];
#endif
      assert( app_tr[0] == 'T' );
      assert( ahd_tr[0] == 'T' );
#if 0 // *****
      n = par_csv_iltbl( strs, 5, fp_src );
#else
      n = par_csv_iltbl( strs, 14, fp_src );
#endif
    }
#if 0 // *****
    if( n > 1 ) {
      if( strncmp( ro_name, "", ROUTE_NAME_MAXLEN ) ) {	
	cnt++;
	pprof = lkup_route_prof( ro_name );
	if( pprof )
	  pprof->apps.ntrs = 0;
      }
      if( pprof ) {
	if( (strnlen(app_tr, TRACK_NAME_MAXLEN ) > 1) && strncmp(&app_tr[1], "Nil", TRACK_NAME_MAXLEN) ) {
	  assert( strnlen( app_tr, TRACK_NAME_MAXLEN ) < (TRACK_NAME_MAXLEN - strlen("_TR")) );
	  strncat( app_tr, "_TR", TRACK_NAME_MAXLEN );
	  assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	  strncpy( pprof->apps.tr[pprof->apps.ntrs].tr_name, app_tr, TRACK_NAME_MAXLEN );
	  pprof->apps.ntrs++;
	}
      }
    }
#else
    if( n > 1 ) {
      if( strncmp( ro_name, "", ROUTE_NAME_MAXLEN ) ) {	
	cnt++;
	pprof = lkup_route_prof( ro_name );
	assert( pprof->apps.ntrs == 0 );
      }
      if( pprof ) {
	assert( cnt > -1 );
	if( (strnlen(app_tr, TRACK_NAME_MAXLEN ) > 1) && strncmp(&app_tr[1], "Nil", TRACK_NAME_MAXLEN) ) {
	  assert( strnlen(app_tr, TRACK_NAME_MAXLEN) < (TRACK_NAME_MAXLEN - strlen("_TR")) );
	  strncat( app_tr, "_TR", TRACK_NAME_MAXLEN );
	  assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	  strncpy( pprof->apps.tr[pprof->apps.ntrs].tr_name, app_tr, TRACK_NAME_MAXLEN );
	  pprof->apps.tr[pprof->apps.ntrs].tr_prof = lkup_track_prof( pprof->apps.tr[pprof->apps.ntrs].tr_name );
#if 1 // *****
	  assert( pprof->apps.tr[pprof->apps.ntrs].tr_prof );
#endif
	  pprof->apps.ntrs++;
	}
	if( (strnlen(ahd_tr, TRACK_NAME_MAXLEN ) > 1) && strncmp(&ahd_tr[1], "Nil", TRACK_NAME_MAXLEN) ) {
	  assert( strnlen(ahd_tr, TRACK_NAME_MAXLEN) < (TRACK_NAME_MAXLEN - strlen("_TR")) );
	  strncat( ahd_tr, "_TR", TRACK_NAME_MAXLEN );
	  assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	  strncpy( pprof->ctrls.ahead.tr_name, ahd_tr, TRACK_NAME_MAXLEN );
	  pprof->ctrls.ahead.tr_prof = lkup_track_prof( pprof->ctrls.ahead.tr_name );
#if 1 // *****
	  assert( pprof->ctrls.ahead.tr_prof );
#endif	  
	}
      }
    }
#endif
    skip_chr( fp_src );
  }
  return (cnt + 1);
}

#if 0
static int cons_ctrl_tracks ( ROUTE_PROF_PTR pprof ) {
  assert( pprof );
  int i;
  for( i = 0; i < pprof->ctrls.ntrs; i++ ) {
    TRACK_PROF_PTR ptr = pprof->tr[i].tr_prof;
    assert( ptr );
    int j;
    for( j = 0; j < ptr->consists_blks.nblks; j++ ) {
      CBTC_BLOCK_PTR pblk = ptr->consists_blks.pblk_profs[j];
      assert( pblk );
      ;
    }
  }
  return 0;
}
#endif

static TRACK_PROF_PTR pick_ahead_track ( ROUTE_PROF_PTR pro_prof ) {
  assert( pro_prof );
  assert( ! pro_prof->ctrls.pahead );
  TRACK_PROF_PTR r = NULL;
  
  if( (strnlen(pro_prof->ctrls.ahead.tr_name, CBI_STAT_IDENT_LEN) > 1) && pro_prof->ctrls.ahead.tr_prof ) {
    int i;
    assert( pro_prof->ctrls.ntrs >= 0 );
    for( i = 0; i < pro_prof->ctrls.ntrs; i++ ) {
      if( !strncmp( pro_prof->ctrls.tr[i].tr_name, pro_prof->ctrls.ahead.tr_name, CBI_STAT_IDENT_LEN ) ) {
	assert( pro_prof->ctrls.tr[i].tr_prof == pro_prof->ctrls.ahead.tr_prof );
	pro_prof->ctrls.pahead = &pro_prof->ctrls.tr[i];
	r = pro_prof->ctrls.ahead.tr_prof;
	break;
      }
    }
#if 1 // *****
    assert( pro_prof->ctrls.pahead );
#endif
  }
  return r;
}
static int emit_route_dataset ( FILE *fp_out, FILE *fp_src_sig,  FILE *fp_src_rel ) {
  assert( fp_out );
  assert( fp_src_sig );
  assert( fp_src_rel );
  
  read_iltbl_signal( fp_out, fp_src_sig );
  read_iltbl_routerel( fp_out, fp_src_rel );
#if 0 // *****
  {
    ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.route_profs;
    assert( pprof );
    while( pprof < tracks_routes_prof.routes.pavail ) {
      assert( pprof );
      cons_ctrl_tracks( pprof );
      pprof++;
    }
  }
#endif
  assert( tracks_routes_prof.routes.route_profs );
  ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.route_profs;
  while( pprof < tracks_routes_prof.routes.pavail ) {
    assert( pprof );
    TRACK_PROF_PTR pahd_tr = NULL;
    pahd_tr = pick_ahead_track( pprof );
#if 0 // *****
    if( pahd_tr ) {
      assert( pprof->ctrls.pahead );
    }
#endif
    pprof->apps.porigin = link_orgahd_blks( pprof->apps.tr, pprof->apps.ntrs, pahd_tr );
#if 1 // *****
    {
      assert( strlen( pprof->route_name ) > 1 );
      int i;
      printf( "(route, [app_tracks], (origin_track, (ahead_track, [ctrl_tracks]))): (%s, [", pprof->route_name );
      for( i = 0; i < pprof->apps.ntrs; i++ ) {
	if( i > 0 )
	  printf( ", " );
	printf( "%s", pprof->apps.tr[i].tr_name );
      }
#if 0 // *****
      printf( "], [" );
#else
      printf( "], (%s, ", pprof->apps.porigin ? pprof->apps.porigin->tr_name : "none_origin" );
      printf( "(%s, [", pprof->ctrls.ahead.tr_name );
#endif
      for( i = 0; i < pprof->ctrls.ntrs; i++ ) {
	if( i > 0 )
	  printf( ", " );
	printf( "%s", pprof->ctrls.tr[i].tr_name );
      }
      printf( "])))\n" );
    }
#endif
    pprof++;
  }
  return 0;
}

static int gen_route_dataset ( FILE *fp_out ) {
  assert( fp_out );
  int r = 0;
  FILE *fp_src_sig = NULL;
  FILE *fp_src_rel = NULL;
  
  emit_route_dataset_prolog( fp_out );
  fp_src_sig = fopen( "BCGN_SIGNAL.csv", "r" );
  if( fp_src_sig ) {
    if( !ferror( fp_src_sig ) ) {
      fp_src_rel = fopen( "BCGN_ROUTEREL.csv", "r" );
      if( fp_src_rel ) {
	if( !ferror( fp_src_rel ) ) {
	  r = emit_route_dataset( fp_out, fp_src_sig, fp_src_rel );
	}
      }
    }
  }
  emit_route_dataset_epilog( fp_out );
  return r;
}

static int init_gen_il_dataset ( void ) {
  int r = ERR_FAILED_ALLOC_WORKMEM;
  
  tracks_routes_prof.tracks.track_profs = calloc( TRACK_PROF_DECL_MAXNUM, sizeof(TRACK_PROF) );
  if( !tracks_routes_prof.tracks.track_profs )
    return r;
  else
    tracks_routes_prof.tracks.pavail = tracks_routes_prof.tracks.track_profs;
  tracks_routes_prof.routes.route_profs = calloc( ROUTE_PROF_DECL_MAXNUM, sizeof(ROUTE_PROF) );
  if( !tracks_routes_prof.routes.route_profs )
    return r;
  else
    tracks_routes_prof.routes.pavail = tracks_routes_prof.routes.route_profs;
  r = ERR_FAILED_CONS_ILSYM_DATABASE;
  {
    int n = -1;
    n = load_cbi_code( OC801, "../memmap/BOTANICAL_GARDEN.csv" );
    if( n > 0 ) {
      int cnt = n;
      n = load_cbi_code( OC802, "../memmap/JASOLA_VIHAR.csv" );
      if( n > 0 )
	r = cnt + n;
    }
  }
  return r;
}

int main ( void ) {
  FILE *fp_out = NULL;
  int r = -1;
  cons_block_state();
  
  init_gen_il_dataset();
  fp_out = fopen( "interlock_dataset0.h", "w" );
  if( fp_out ) {
    if( !ferror( fp_out ) ) {
      r = gen_track_dataset( fp_out );
      fprintf( fp_out, "\n" );
      r = gen_route_dataset( fp_out );
    }
  }
  return r;
}

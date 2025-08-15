/*
 * construct database for route profile.
 *
 * -1) splitting route name into src & dst ones, and regist them together.
 * -2) identify the origin track of the route, which has its destination signal as its ORIGIN one.
 * -3) For all relevant routes claim above condition, we should examine all ones have same track as its ORIGIN.
 * -4) then We can judge the track as the DESTINATION track of the route we are now looking for its dest.
 * -read_iltbl_track, read_iltbl_point
 * verification the registration of POINT name over the cbi codetable, in read_iltbl_point.
 * how to tame the case of that both NKR & RKR are TRUE, its obvious ilegal condition.
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

#define POINT_NAME_NAXLEN 32
#define TURNOUT_MAX_POINTS 16
#define ROUTE_MAX_POINTS 16

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
#if 0 // *****
  BLK_LINKAGE_PTR pos[MAX_ADJACENT_BLKS];
#else
  struct {
    BLK_LINKAGE_PTR plnk;
    BOOL bond;
  } pos[MAX_ADJACENT_BLKS];
#endif
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
  struct {
    int npts;
    struct {
      char pt_name[CBI_STAT_IDENT_LEN + 1];
      CBI_STAT_ATTR_PTR ppt_attr;
    } point[TURNOUT_MAX_POINTS];
  } turnout;
  TRACK_BOUND tr_bound;
  struct track_prof *pNext;
} TRACK_PROF, *TRACK_PROF_PTR;

struct route_tr {
  char tr_name[CBI_STAT_IDENT_LEN + 1];
  TRACK_PROF_PTR tr_prof;
};
struct route_sw {
  char pt_name[CBI_STAT_IDENT_LEN + 1];
  struct {
    BOOL normal;
    BOOL reverse;
  } stat;
};
typedef struct route_prof {
  char route_name[CBI_STAT_IDENT_LEN + 1];
  struct {
    struct {
      char signame_org[CBI_STAT_IDENT_LEN + 1];
      struct route_tr *porg_tr;
    } org;
    struct {
      char signame_dst[CBI_STAT_IDENT_LEN + 1];
      struct route_tr *pdst_tr;
      struct route_prof *pNext;
    } dst;
  } orgdst;
  struct {
    int ntrs;
    struct route_tr tr[ROUTE_MAX_APPTRACKS];
  } apps;
  struct {
    int ntrs;
    struct route_tr tr[ROUTE_MAX_CTRLTRACKS];
    struct route_tr ahead, *pahead;
    int npts;
    struct route_sw pt[ROUTE_MAX_POINTS];
  } body;
} ROUTE_PROF, *ROUTE_PROF_PTR;

static struct {
  struct {
    TRACK_PROF_PTR track_profs;
    TRACK_PROF_PTR pavail;
    TRACK_PROF_PTR pprof_sets[END_OF_ST_ID];
  } tracks;
  struct {
    struct {
      ROUTE_PROF_PTR pwhole;
      int nixls;
      ROUTE_PROF_PTR pprevs[END_OF_OCs];
      ROUTE_PROF_PTR pcrnt_ixl;
    } profs;
    ROUTE_PROF_PTR pavail;
    struct {
      ROUTE_PROF_PTR pdestin;
    } bkpatches;
  } routes;
} tracks_routes_prof = {};

static void print_track_prof ( TRACK_PROF_PTR ptr_prof ) {
  assert( ptr_prof );
  
  printf( "(tr_name, bound, {points}, {route_locks}, [consist_blks]): (%s, %s, {", ptr_prof->track_name, cnv2str_trbound(ptr_prof->tr_bound) );
  {
    int i;
    for( i = 0; i < ptr_prof->turnout.npts; i++ ) {
      if( i > 0 )
	printf( ", " );
      printf( "%s", ptr_prof->turnout.point[i].pt_name );
    }
  }
  printf( "}, {" );
  
  {
    BOOL sr = FALSE;
    // TLSR
    if( ptr_prof->sr.TLSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.TLSR.sr_name );
      sr = TRUE;
    }
    // TRSR
    if( ptr_prof->sr.TRSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.TRSR.sr_name );
      sr = TRUE;
    }    
    // sTLSR
    if( ptr_prof->sr.sTLSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.sTLSR.sr_name );
      sr = TRUE;
    }
    // sTRRS
    if( ptr_prof->sr.sTRSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.sTRSR.sr_name );
      sr = TRUE;
    }
    // eTLSR
    if( ptr_prof->sr.eTLSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.eTLSR.sr_name );
      sr = TRUE;
    }
    // eTRSR
    if( ptr_prof->sr.eTRSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.eTRSR.sr_name );
      sr = TRUE;
    }
    // kTLSR
    if( ptr_prof->sr.kTLSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.kTLSR.sr_name );
      sr = TRUE;
    }
    // kTRSR
    if( ptr_prof->sr.kTRSR.defined ) {
      if( sr )
	printf( ", " );
      printf( "%s", ptr_prof->sr.kTRSR.sr_name );
      sr = TRUE;
    }
  }
  printf( "}, " );
  
  {
    int i;
    for( i = 0; i < ptr_prof->consists_blks.nblks; i++ ) {
      CBTC_BLOCK_PTR pblk = ptr_prof->consists_blks.pblk_profs[i];
      assert( pblk );
      if( i > 0 )
	printf( ", " );
#if 0 // *****
      print_block_prof( stdout, pblk );
#else
      assert( pblk->virt_blkname_str );
      printf( "%s", pblk->virt_blkname_str );
#endif
    }
    printf( "])\n" );
  }
}

static BOOL prn_rtprof_lv1 = FALSE;
static void prn_route_prof_lv0 ( ROUTE_PROF_PTR pro_prof ) {
  assert( pro_prof );
  assert( strlen( pro_prof->route_name ) > 1 );
  int i;
  printf( "(route, (org_sig, dst_sig), [app_tracks], (" );
  if( prn_rtprof_lv1 )
    printf( "(" );
  printf( "origin_track, " );
  if( prn_rtprof_lv1 )
    printf( "destin_track), " );
  else
    printf( "origin_track, " );
  printf( "points, " );
  printf( "(ahead_track, [ctrl_tracks]))): " );
  printf( "(%s (%s, %s), [", pro_prof->route_name, pro_prof->orgdst.org.signame_org, pro_prof->orgdst.dst.signame_dst );
  for( i = 0; i < pro_prof->apps.ntrs; i++ ) {
    if( i > 0 )
      printf( ", " );
    printf( "%s", pro_prof->apps.tr[i].tr_name );
  }  
  printf( "], (" );
  if( prn_rtprof_lv1 )
    printf( "(" );
  printf( "%s, ", pro_prof->orgdst.org.porg_tr ? pro_prof->orgdst.org.porg_tr->tr_name : "none_origin" );  
  if( prn_rtprof_lv1 )
    printf( "%s), ", pro_prof->orgdst.dst.pdst_tr ? pro_prof->orgdst.dst.pdst_tr->tr_name : "none_destin" );
  {
    int i;
    for( i = 0; i < pro_prof->body.npts; i++ ) {
      if( i > 0 )
	printf( ", " );
      printf( "%s:", pro_prof->body.pt[i].pt_name );
      if( pro_prof->body.pt[i].stat.normal )
	printf( "NKR" );
      if( pro_prof->body.pt[i].stat.reverse ) {
	if( pro_prof->body.pt[i].stat.normal )
	  printf( "&" );
	printf( "RKR" );
      }
      if( !(pro_prof->body.pt[i].stat.normal || pro_prof->body.pt[i].stat.reverse) )
	printf( "none" );
    }
    printf( ", " );
  }
  printf( "(%s, [", pro_prof->body.ahead.tr_name );
  for( i = 0; i < pro_prof->body.ntrs; i++ ) {
    if( i > 0 )
      printf( ", " );
    printf( "%s", pro_prof->body.tr[i].tr_name );
  }
  printf( "])))\n" );
  prn_rtprof_lv1 = FALSE;
}
static void prn_route_prof_lv1 ( ROUTE_PROF_PTR pro_prof ) {
  assert( pro_prof );
  prn_rtprof_lv1 = TRUE;
  prn_route_prof_lv0( pro_prof );
  assert( !prn_rtprof_lv1 );
}

static int print_route_prof ( ROUTE_PROF_PTR pro_prof ) {
  assert( pro_prof );
  int n = 0;
  
  //ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.profs.pcrnt_ixl;
  while( pro_prof < tracks_routes_prof.routes.pavail ) {
    prn_route_prof_lv1( pro_prof );
    n++;
    pro_prof++;
  }
  return n;
}

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
  
  ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.profs.pcrnt_ixl;
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

static int linking_core ( struct fixed_pos *pblks[], int nblks, LINX_BONDAGE_KIND bind ) {
  assert( pblks );
  assert( nblks <= MAX_TRACK_BLOCKS );
  int cnt = 0;
  
  if( nblks > 1 ) {
    struct fixed_pos *pblk = pblks[0];
    int i;
    for( i = 0; i < pblk->npos; i++ ) {
      assert( pblk->pos[i].plnk );
      int j;
      if( pblk->pos[i].bond )
	continue;
#if 0 // *****
      if( (pblk->pos[i].plnk)->bond.pln_neigh ) {
	assert( (pblk->pos[i].plnk)->bond.kind != LINK_NONE );
	continue;
      }
#endif
      assert( (pblk->pos[i].plnk)->pmorph );
      assert( (pblk->pos[i].plnk)->pmorph->pblock );
      for( j = 1; j < nblks; j++ ) {
	struct fixed_pos *pb = pblks[j];
	BOOL found = FALSE;	
	int k;	
	for( k = 0; k < pb->npos; k++ ) {
	  assert( pb->pos[k].plnk );
	  if( pb->pos[k].bond )
	    continue;
#if 0 // *****
	  if( (pb->pos[k].plnk)->bond.pln_neigh ) {
	    assert( (pb->pos[k].plnk)->bond.kind != LINK_NONE );
	    continue;
	  }
#endif
	  assert( (pb->pos[k].plnk)->pmorph );
	  assert( (pb->pos[k].plnk)->pmorph->pblock );
	  if( ((pblk->pos[i].plnk)->neigh_blk == (pb->pos[k].plnk)->pmorph->pblock->block_name) &&
	      ((pb->pos[k].plnk)->neigh_blk == (pblk->pos[i].plnk)->pmorph->pblock->block_name) ) {
	    BLK_LINKAGE_PTR pl = pblk->pos[i].plnk;
	    do {
	      assert( pl );
	      pl->bond.pln_neigh = pb->pos[k].plnk;
	      pl->bond.kind = bind;	      
	      pl = pl->pNext;
	    } while( pl != pblk->pos[i].plnk );
	    pl = pb->pos[k].plnk;
	    do {
	      pl->bond.pln_neigh = pblk->pos[i].plnk;
	      pl->bond.kind = bind;
	      pl = pl->pNext;
	    } while( pl != pb->pos[k].plnk );
	    pblk->pos[i].bond = TRUE;
	    pb->pos[k].bond = TRUE;
	    cnt++;
	    found = TRUE;
	    break;
	  }
	}
	if( found )
	  break;
      }
    }
    cnt += linking_core( &pblks[1], (nblks - 1), bind );
  }
  return cnt;
}
static int linking_blks ( struct fixed_pos *pblks[], int nblks, LINX_BONDAGE_KIND bind ) {
  assert( pblks );
  assert( nblks <= MAX_TRACK_BLOCKS );
  int r = -1;
  
  int i;
  for( i = 0; i < nblks; i++ ) {
    int k;
    for( k = 0; k < MAX_ADJACENT_BLKS; k++ ) {
      pblks[i]->pos[k].bond = FALSE;
    }
  }
  r = linking_core( pblks, nblks, bind );
  return r;
}

static int link_internal_blks ( CBTC_BLOCK_PTR profs[], struct fixed_pos fixes[], const int nblks ) {
  assert( profs );
  assert( fixes );
  assert( nblks <= MAX_TRACK_BLOCKS );
  struct fixed_pos *pps[MAX_TRACK_BLOCKS] = {};
  int cnt = 0;
  
  int i;
  for( i = 0; i < nblks; i++ ) {
    assert( i < MAX_TRACK_BLOCKS );
    BLK_LINKAGE_PTR plnks[MAX_ADJACENT_BLKS] = {};
    int n = -1;
    n = enum_fixed_branches( profs[i], plnks, MAX_ADJACENT_BLKS );
    if( n > 0 ) {
      int j;
      fixes[cnt].pprof = profs[i];
      fixes[cnt].npos = n;
      for( j = 0; j < fixes[cnt].npos; j++ ) {
	assert( plnks[j] );
	fixes[cnt].pos[j].plnk = plnks[j];	
      }
      cnt++;
    }
  }
  for( i = 0; i < cnt; i++ )
    pps[i] = &fixes[i];
  linking_blks( pps, cnt, LINK_HARD );
  return cnt;
}

static struct route_tr *trylnk_orgahd ( struct route_tr app_trs[], const int napp_trs, struct fixed_pos pahd_blks[], const int nahd_blks ) {
  assert( app_trs );
  assert( napp_trs <= ROUTE_MAX_APPTRACKS );
  assert( pahd_blks );
  assert( nahd_blks <= MAX_TRACK_BLOCKS );
  struct fixed_pos *pps[2] = {};
  
  int i;
  for( i = 0; i < napp_trs; i++ ) {
    assert( strnlen( app_trs[i].tr_name, CBI_STAT_IDENT_LEN ) > 0 );
    int j;
    if( ! app_trs[i].tr_prof )
      continue;
    for( j = 0; j < app_trs[i].tr_prof->hardbonds.nblks; j++ ) {
      int k;
      pps[0] = &app_trs[i].tr_prof->hardbonds.pblk_fixes[j];
      for( k = 0; k < nahd_blks; k++ ) {
	int n = -1;
	pps[1] = &pahd_blks[k];
	n = linking_blks( pps, 2, LINK_SEMIHARD );
	if( n > 0 ) {
	  if( n == 1 ) {
	    BOOL found = FALSE;
	    int l;	      
	    for( l = 0; l < app_trs[i].tr_prof->consists_blks.nblks; l++ ) {
	      if( app_trs[i].tr_prof->consists_blks.pblk_profs[l] == pps[0]->pprof ) {
		found = TRUE;
		break;
	      }
	    }
	    assert( found );
	    return &app_trs[i];
	  } else {
	    printf( "fatal: ill-formed linking detected on the blocks of %s and %s.\n",
		    ((pps[0]->pprof) ? (pps[0]->pprof)->virt_blkname_str : "UNKNOWN"),
		    ((pps[1]->pprof) ? (pps[1]->pprof)->virt_blkname_str : "UNKNOWN") );
	  }	  
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
  if( !porg_tr ) {
    assert( pahd_tr->hardbonds.nblks < MAX_TRACK_BLOCKS );
    struct fixed_pos *pahd_added = &pahd_tr->hardbonds.pblk_fixes[pahd_tr->hardbonds.nblks];    
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
	  pahd_added->pos[0].plnk = plnk;
	  pahd_added->npos = 1;
	  porg_tr = trylnk_orgahd( app_trs, napps, pahd_added, 1 );
	  if( porg_tr ) {
	    assert( pahd_added->pprof == pblk );
	    assert( pahd_added->pos[0].plnk );
	    assert( pahd_added->pos[0].bond );
	    BOOL found = FALSE;
	    int l;
	    for( l = 0; l < pahd_tr->hardbonds.nblks; l++ ) {
	      struct fixed_pos *p = &pahd_tr->hardbonds.pblk_fixes[l];
	      assert( p->pprof );
	      if( p->pprof == pahd_added->pprof ) {
		{
		  int m;
		  for( m = 0; m < p->npos; m++ ) {
		    assert( p->pos[m].plnk );
		    assert( p->pos[m].plnk != pahd_added->pos[0].plnk );
		  }		 
		}		
		assert( p->npos < MAX_ADJACENT_BLKS );
		p->pos[p->npos].plnk = pahd_added->pos[0].plnk;;
		p->npos++;
		found = TRUE;
		break;
	      }
	    }
	    if( !found ) 
	      pahd_tr->hardbonds.nblks++;
	    return porg_tr;
	  }
	}
      }
    }
    assert( !porg_tr );
  }
  return porg_tr;
}

static TRACK_PROF_PTR emit_track_prof ( FILE *fp_out, TRACK_PROF_PTR pprof ) {
  assert( pprof );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  char *ptr_name = pprof->track_name;
  
  fprintf( fp_out, "{ _TRACK, " );
  if( pprof->ptr_attr ) {
    fprintf( fp_out, "\"%s\", ", (pprof->ptr_attr)->ident );
    fprintf( fp_out, "%s, ", (pprof->ptr_attr)->ident );
  } else {
    fprintf( fp_out, "\"%s\", ", ptr_name );
    fprintf( fp_out, "%s, ", ptr_name );
  }
  
  // cbtc
  fprintf( fp_out, "{" );
  {
    int i;
    fprintf( fp_out, "%d", pprof->consists_blks.nblks );
    fprintf( fp_out, ", {" );
    for( i = 0; i < pprof->consists_blks.nblks; i++ ) {
      if( i > 0 )
	fprintf( fp_out, ", " );
      fprintf( fp_out, "%s", pprof->consists_blks.pblk_profs[i]->virt_blkname_str );
    }
    fprintf( fp_out, "}" );
  }
  fprintf( fp_out, "}, " );
  
  // lock, TLSR
  fprintf( fp_out, "{" );
  fprintf( fp_out, "{" );
  if( pprof->sr.TLSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_TLSR", pprof->sr.TLSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // TRSR
  if( pprof->sr.TRSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_TRSR", pprof->sr.TRSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // sTLSR
  if( pprof->sr.sTLSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_sTLSR", pprof->sr.sTLSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // sTRRS
  if( pprof->sr.sTRSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_sTRSR", pprof->sr.sTRSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // eTLSR
  if( pprof->sr.eTLSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_eTLSR", pprof->sr.eTLSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // eTRSR
  if( pprof->sr.eTRSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_eTRSR", pprof->sr.eTRSR.sr_name );
  } else
   fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // kTLSR
  if( pprof->sr.kTLSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_kTLSR", pprof->sr.kTLSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  // kTRSR
  if( pprof->sr.kTRSR.defined ) {
    fprintf( fp_out, "TRUE, " );
    fprintf( fp_out, "%s, %s", "_kTRSR", pprof->sr.kTRSR.sr_name );
  } else
    fprintf( fp_out, "FALSE" );
  fprintf( fp_out, "}" );
  fprintf( fp_out, "}" );
  
  fprintf( fp_out, "},\n" );
  return pprof;
}

static int read_iltbl_track ( TRACK_PROF_PTR *pprofs, FILE *fp_src ) {
  assert( pprofs);
  assert( fp_src );
  TRACK_PROF_PTR pprev = NULL;
  int cnt = 0;
  
  while( !feof(fp_src) ) {
    assert( !ferror( fp_src ) );
    assert( cnt < TRACK_PROF_DECL_MAXNUM );
    int n = -1;
    char seq[5 + 1] = "";
    char tr_name[TRACK_NAME_MAXLEN + 1] = "T";
    char bounds[TRACK_BOUNDALIGN_MAXLEN + 1] = "";
    seq[5] = 0;
    tr_name[TRACK_NAME_MAXLEN] = 0;
    bounds[TRACK_BOUNDALIGN_MAXLEN] = 0;
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
    if( n > 1 ) {
      assert( tr_name[0] == 'T' );
      if( strnlen( &tr_name[1], (TRACK_NAME_MAXLEN - 1) ) > 1 ) {
	assert( tracks_routes_prof.tracks.pavail < &tracks_routes_prof.tracks.track_profs[TRACK_PROF_DECL_MAXNUM] );
	TRACK_PROF_PTR pprof = tracks_routes_prof.tracks.pavail;
	snprintf( pprof->track_name, CBI_STAT_IDENT_LEN, "%s_TR", tr_name );
	if( !strcmp( bounds, "Down" ) )
	  pprof->tr_bound = BOUND_DOWN;
	else if( !strcmp( bounds, "Up" ) )
	  pprof->tr_bound = BOUND_UP;
	else
	  pprof->tr_bound = BOUND_UNKNOWN;
	cnt++;
	assert( pprof == tracks_routes_prof.tracks.pavail );
	if( cnt == 1 ) {
	  assert( !pprev );
	  *pprofs = pprof;
	} else {
	  assert( pprev );
	  assert( !pprev->pNext );
	  pprev->pNext = pprof;
	}
	pprof->pNext = NULL;
	pprev = pprof;
	tracks_routes_prof.tracks.pavail++;
      }
    }
    skip_chr( fp_src );
  }
  return cnt;
}

static int read_iltbl_point ( FILE *fp_src ) {
  assert( fp_src );
  assert( !ferror( fp_src ) );
  int cnt = 0;
  
  while( !feof( fp_src ) ) {
    int n = -1;
    char seq[5 + 1] = "";    
    char sw_name[POINT_NAME_NAXLEN + 1] = "P";
    char tr_name[TRACK_NAME_MAXLEN + 1] = "T";
    seq[5] = 0;
    sw_name[POINT_NAME_NAXLEN] = 0;
    tr_name[TRACK_NAME_MAXLEN] = 0;
    {
      char *strs[11];
      char dc[256 + 1]; // dont cure.
      dc[256] = 0;
      strs[0] = seq;
      strs[1] = dc;
      strs[2] = &sw_name[1];
      strs[3] = &tr_name[1];
      strs[4] = dc;      
      strs[5] = dc;
      strs[6] = dc;
      strs[7] = dc;
      n = par_csv_iltbl( strs, 8, fp_src );
    }
    if( n > 3 ) {
      if( strncmp( sw_name, "", POINT_NAME_NAXLEN ) ) {
	assert( sw_name[0] == 'P' );
	TRACK_PROF_PTR ptr_prof = NULL;
	if( strncmp( tr_name, "", TRACK_NAME_MAXLEN ) ) {
	  assert( tr_name[0] == 'T' );
	  if( strnlen( tr_name, TRACK_NAME_MAXLEN ) > 1 ) {
	    strncat( tr_name, "_TR", TRACK_NAME_MAXLEN );
	    ptr_prof = lkup_track_prof( tr_name );
	    if( ptr_prof ) {
	      const int idx = ptr_prof->turnout.npts;
	      assert( POINT_NAME_NAXLEN <= CBI_STAT_IDENT_LEN );
	      strncpy( ptr_prof->turnout.point[idx].pt_name, sw_name, POINT_NAME_NAXLEN );
	      ptr_prof->turnout.npts++;
	      cnt++;
	    } else {
	      cnt *= -1;
	      printf( "warning: unknown turnout track %s with %s detected, not found in TRACK of interlock table.\n", tr_name, sw_name );
	    }
	  }
	}
      }
    }
    skip_chr( fp_src );
  }
  return cnt;
}

static char *stem_track ( char *stem, const char *tr_name, const int stem_len ) {
  assert( stem );
  BOOL trimmed = FALSE;
  
  const char *p = tr_name;
  while( *p ) {
    assert( *p );
    if( strncmp( p, "_TR", strlen("_TR") ) == 0 ) {
      const int slen = (p - tr_name) < stem_len ? (p - tr_name) : stem_len;
      strncpy( stem, tr_name, slen );
      if( slen < stem_len )
	stem[slen] = 0;
      else
	assert( slen == stem_len );
      trimmed = TRUE;
      break;
    }
    p++;
  }
  return (trimmed ? stem : NULL );
}
static BOOL identify_sr ( struct track_sr *psr, char *tr_name, const char *psfx_sr ) {
  assert( psr );
  assert( tr_name );
  assert( psfx_sr );
  BOOL r = FALSE;
  
  char idstr[CBI_STAT_IDENT_LEN + 1];
  {
    char *p = NULL;
    p = stem_track( idstr, tr_name, CBI_STAT_IDENT_LEN );
    assert( p == idstr );
  }
  strcat( idstr, psfx_sr );
  idstr[CBI_STAT_IDENT_LEN] = 0;
  {
    CBI_STAT_ATTR_PTR pattr = NULL;
    pattr = conslt_cbi_code_tbl( idstr );
    if( pattr ) {
      assert( !strncmp( cnv2str_il_sym(pattr->id), pattr->ident, CBI_STAT_IDENT_LEN ) );
      psr->defined = TRUE;
      strncpy( psr->sr_name, pattr->ident, CBI_STAT_IDENT_LEN );
      psr->psr_attr = pattr;
      r = TRUE;
    } else {
      psr->defined = FALSE;
      psr->psr_attr = NULL;
    }
  }
  return r;
}
static void track_prof_sr ( TRACK_PROF_PTR ptr_prof ) {
  assert( ptr_prof );
  assert( strnlen(ptr_prof->track_name, CBI_STAT_IDENT_LEN) > 0 );
  
  identify_sr( &ptr_prof->sr.TLSR, ptr_prof->track_name, "_TLSR" ); // TLSR
  identify_sr( &ptr_prof->sr.TRSR, ptr_prof->track_name, "_TRSR" ); // TRSR
  identify_sr( &ptr_prof->sr.sTLSR, ptr_prof->track_name, "_sTLSR" ); // sTLSR
  identify_sr( &ptr_prof->sr.sTRSR, ptr_prof->track_name, "_sTRSR" ); // sTRSR
  identify_sr( &ptr_prof->sr.eTLSR, ptr_prof->track_name, "_eTLSR" ); // eTLSR
  identify_sr( &ptr_prof->sr.eTRSR, ptr_prof->track_name, "_eTRSR" ); // eTRSR
  identify_sr( &ptr_prof->sr.kTLSR, ptr_prof->track_name, "_kTLSR" ); // kTLSR
  identify_sr( &ptr_prof->sr.kTRSR, ptr_prof->track_name, "_kTRSR" ); // kTRSR
}

static int track_prof_blks ( CBTC_BLOCK_PTR *pphead, char *ptr_name ) {
  assert( pphead );
  assert( ptr_name );
  int cnt = 0;  
  CBI_STAT_ATTR_PTR pattr = NULL;
  
  char idstr[CBI_STAT_IDENT_LEN + 1];
  strncpy( idstr, ptr_name, CBI_STAT_IDENT_LEN );
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
static void consist_blks_and_linking ( TRACK_PROF_PTR ptr_prof ) {
  assert( ptr_prof );
  
  CBTC_BLOCK_PTR pblk_prof = NULL;
  {
    int nblks = -1;
    if( ptr_prof->ptr_attr )
      nblks = track_prof_blks( &pblk_prof, (ptr_prof->ptr_attr)->ident );
    else
      nblks = track_prof_blks( &pblk_prof, ptr_prof->track_name );
    ptr_prof->consists_blks.nblks = nblks;
  }
  if( ptr_prof->consists_blks.nblks > 0 ) {
    assert( pblk_prof );
    int n = ptr_prof->consists_blks.nblks;
    int i = 0;
    do {
      assert( n > 0 );
      ptr_prof->consists_blks.pblk_profs[i++] = pblk_prof;
      n--;
      pblk_prof = pblk_prof->belonging_tr.pNext;
    } while( pblk_prof );
    assert( n == 0 );
    assert( i == ptr_prof->consists_blks.nblks );
    ptr_prof->hardbonds.nblks = link_internal_blks( ptr_prof->consists_blks.pblk_profs, ptr_prof->hardbonds.pblk_fixes, ptr_prof->consists_blks.nblks );
  } else {
    printf( "warning: track %s has no blocks.\n", (ptr_prof->ptr_attr ? (ptr_prof->ptr_attr)->ident : ptr_prof->track_name) );
  }
}

static int emit_track_dataset ( TRACK_PROF_PTR *pprofs, FILE *fp_out, FILE *fp_src_tr, FILE *fp_src_sw ) {
  assert( pprofs );
  assert( fp_out );
  assert( fp_src_tr );
  assert( fp_src_sw );
  int cnt = 0;
  
  if( !ferror( fp_src_tr ) ) {
    if( read_iltbl_track( pprofs, fp_src_tr ) > 1 ) {
      TRACK_PROF_PTR prof = *pprofs;
      if( !ferror( fp_src_sw ) )
	read_iltbl_point( fp_src_sw );
      while( prof ) {
	if( !ferror( fp_out ) ) {
	  GEN_INDENT( fp_out, 1, 2 );
	  if( !ferror( fp_out ) ) {	    
	    track_prof_sr( prof );
	    prof->ptr_attr = conslt_cbi_code_tbl( prof->track_name );
	    if( ! prof->ptr_attr ) {
	      printf( "warning: track %s has no registration in cbi codetable.\n", prof->track_name );
	    }
	    consist_blks_and_linking( prof );
	    print_track_prof( prof );
	    {
	      TRACK_PROF_PTR p = NULL;
	      p = emit_track_prof( fp_out, prof );
	      assert( p == prof );
	    }
	    cnt++;
	  } else {
	    cnt *= -1;
	    break;
	  }
	} else {
	  cnt *= -1;
	  break;
	}	
	prof = prof->pNext;
      }
    }
  }
  return cnt;
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

static int gen_track_dataset ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  int r = ERR_FAILED_OPEN_ILTBL_TRACKS;
  
  FILE *fp_src_tr = NULL;
  FILE *fp_src_sw = NULL;
  emit_track_dataset_prolog( fp_out );
  fp_src_tr = fopen( "BCGN_TRACK.csv", "r" );
  if( fp_src_tr ) {
    if( !ferror( fp_src_tr ) ) {
      fp_src_sw = fopen( "BCGN_POINT.csv", "r" );
      if( fp_src_sw ) {
	if( !ferror( fp_src_sw ) ) {
	  r = emit_track_dataset( &tracks_routes_prof.tracks.pprof_sets[BTGD], fp_out, fp_src_tr, fp_src_sw );	  
	}
      }
    }
  }
  r = ERR_FAILED_OPEN_ILTBL_TRACKS;
  fp_src_tr = fopen( "JLA_TRACK.csv", "r" );
  if( fp_src_tr ) {
    if( !ferror( fp_src_tr ) ) {
      fp_src_sw = fopen( "JLA_POINT.csv", "r" );
      if( fp_src_sw ) {
	if( !ferror( fp_src_sw ) ) {
	  r = emit_track_dataset( &tracks_routes_prof.tracks.pprof_sets[JLA], fp_out, fp_src_tr, fp_src_sw );
	}
      }
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

static int split_orgdst_sigs( const char *route_name, const int name_len ) {
  assert( route_name );
  assert( name_len >= 0 );
  int cnt = 0;
  while( cnt < name_len ) {
    assert( cnt < name_len );
    char c = route_name[cnt];
    if( (c == 0) || (c == '_') )
      break;
    cnt++;
  }
  assert( (cnt >= name_len) ? route_name[cnt] == 0 : TRUE );
  return cnt;
}

static int read_iltbl_signal ( FILE *fp_src ) {
  assert( fp_src );
  ROUTE_PROF_PTR pprof = NULL;
  int cnt = 0;
  
  assert( !ferror( fp_src ) );
  while( !feof( fp_src ) ) {
    int n = -1;
    char seq[5 + 1];
    char ro_name[ROUTE_NAME_MAXLEN + 1];
    char nor_sw[POINT_NAME_NAXLEN + 1] = "P";
    char rev_sw[POINT_NAME_NAXLEN + 1] = "P";
    char ctrl_tr[TRACK_NAME_MAXLEN + 1];
    seq[5] = 0;
    ro_name[ROUTE_NAME_MAXLEN] = 0;
    nor_sw[POINT_NAME_NAXLEN] = 0;
    rev_sw[POINT_NAME_NAXLEN] = 0;
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
      strs[6] = &nor_sw[1];
      strs[7] = &rev_sw[1];
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
	assert( tracks_routes_prof.routes.pavail < &tracks_routes_prof.routes.profs.pwhole[ROUTE_PROF_DECL_MAXNUM] );
	pprof = tracks_routes_prof.routes.pavail;
	assert( ROUTE_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->route_name, ro_name, ROUTE_NAME_MAXLEN );
	{
	  const int whole_len = strnlen( pprof->route_name, ROUTE_NAME_MAXLEN );
	  assert( pprof->route_name[whole_len] == 0 );
	  int sep = split_orgdst_sigs( pprof->route_name, whole_len );
	  assert( sep <= whole_len );
	  strncpy( pprof->orgdst.org.signame_org, pprof->route_name, sep );
	  if( pprof->route_name[sep] == '_' )
	    strncpy( pprof->orgdst.dst.signame_dst, &pprof->route_name[sep+1], ROUTE_NAME_MAXLEN );
	}
	pprof->body.ntrs = 0;
	strcpy( pprof->body.ahead.tr_name, "" );
	pprof->body.ahead.tr_prof = NULL;
	pprof->body.pahead = NULL;
	pprof->body.npts = 0;
	cnt++;
      }
      assert( pprof );
      if( strnlen( ctrl_tr, TRACK_NAME_MAXLEN ) > 1 ) {
	const int i = pprof->body.ntrs;
	assert( strnlen( ctrl_tr, TRACK_NAME_MAXLEN ) < (TRACK_NAME_MAXLEN - strlen("_TR")) );
	strncat( ctrl_tr, "_TR", TRACK_NAME_MAXLEN );
	assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->body.tr[i].tr_name, ctrl_tr, TRACK_NAME_MAXLEN );
	pprof->body.tr[i].tr_prof = lkup_track_prof( pprof->body.tr[i].tr_name );
	if( ! pprof->body.tr[i].tr_prof ) {
	  printf( "warning: unknown control-track %s of %s detected, not found in TRACK of interlock table.\n", pprof->body.tr[i].tr_name, pprof->route_name );
	}
	pprof->body.ntrs++;
      }
      assert( nor_sw[0] == 'P' );
      if( (strnlen(&nor_sw[1], (POINT_NAME_NAXLEN - 1)) > 1) && strncmp(&nor_sw[1], "Nil", (POINT_NAME_NAXLEN - 1)) ) {
	assert( POINT_NAME_NAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->body.pt[pprof->body.npts].pt_name, nor_sw, POINT_NAME_NAXLEN );
	pprof->body.pt[pprof->body.npts].stat.normal = TRUE;
	pprof->body.npts++;
      }
      assert( rev_sw[0] == 'P' );
      if( (strnlen(&rev_sw[1], (POINT_NAME_NAXLEN - 1)) > 1) && strncmp(&rev_sw[1], "Nil", (POINT_NAME_NAXLEN - 1)) ) {
	assert( POINT_NAME_NAXLEN <= CBI_STAT_IDENT_LEN );
	strncpy( pprof->body.pt[pprof->body.npts].pt_name, rev_sw, POINT_NAME_NAXLEN );
	pprof->body.pt[pprof->body.npts].stat.reverse = TRUE;
	pprof->body.npts++;
      }
    }
    skip_chr( fp_src );
  }
  if( cnt > 0 )
    tracks_routes_prof.routes.pavail++;
  return cnt;
}

static int read_iltbl_routerel ( FILE *fp_src ) {
  assert( fp_src );
  extern int par_csv_iltbl ( char *bufs[], const int nbufs, FILE *fp_src );
  
  ROUTE_PROF_PTR pprof = NULL;
  int cnt = -1;
  
  assert( !ferror( fp_src ) );
  while( !feof( fp_src ) ) {
    int n = -1;
    char seq[5 + 1];
    char ro_name[ROUTE_NAME_MAXLEN + 1];
    char app_tr[TRACK_NAME_MAXLEN + 1];
    char ahd_tr[TRACK_NAME_MAXLEN + 1];
    seq[5] = 0;
    ro_name[ROUTE_NAME_MAXLEN] = 0;
    app_tr[TRACK_NAME_MAXLEN] = 0;
    strcpy( seq, "" );
    strcpy( ro_name, "" );
    strcpy( app_tr, "T" );
    strcpy( ahd_tr, "T" );
    {
      char *strs[14];
      char dc[256 + 1]; // dont cure.
      dc[256] = 0;
      strs[0] = seq;
      strs[1] = dc;
      strs[2] = ro_name;
      strs[3] = dc;
      strs[4] = &app_tr[1];
      strs[5] = dc;
      strs[6] = dc;
      strs[7] = dc;
      strs[8] = dc;
      strs[9] = dc;
      strs[10] = dc;
      strs[11] = dc;
      strs[12] = dc;
      strs[13] = &ahd_tr[1];
      n = par_csv_iltbl( strs, 14, fp_src );
    }
    if( n > 1 ) {
      if( strncmp( ro_name, "", ROUTE_NAME_MAXLEN ) ) {	
	cnt++;
	pprof = lkup_route_prof( ro_name );
	assert( pprof->apps.ntrs == 0 );
      }
      if( pprof ) {
	assert( cnt > -1 );
	assert( app_tr[0] == 'T' );
	if( (strnlen(&app_tr[1], (TRACK_NAME_MAXLEN - 1)) > 1) && strncmp(&app_tr[1], "Nil", TRACK_NAME_MAXLEN) ) {
	  assert( strnlen(&app_tr[1], (TRACK_NAME_MAXLEN - 1)) < ((TRACK_NAME_MAXLEN - 1) - strlen("_TR")) );
	  strncat( app_tr, "_TR", TRACK_NAME_MAXLEN );
	  assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	  strncpy( pprof->apps.tr[pprof->apps.ntrs].tr_name, app_tr, TRACK_NAME_MAXLEN );
	  pprof->apps.tr[pprof->apps.ntrs].tr_prof = lkup_track_prof( pprof->apps.tr[pprof->apps.ntrs].tr_name );
	  if( !pprof->apps.tr[pprof->apps.ntrs].tr_prof ) {
	    printf( "warning: unknown approach-track %s of %s detected, not found in TRACK of interlock table.\n", pprof->apps.tr[pprof->apps.ntrs].tr_name, pprof->route_name );
	  }
	  pprof->apps.ntrs++;
	}
	assert( ahd_tr[0] == 'T' );
	if( (strnlen(&ahd_tr[1], (TRACK_NAME_MAXLEN - 1)) > 1) && strncmp(&ahd_tr[1], "Nil", TRACK_NAME_MAXLEN) ) {
	  assert( strnlen(&ahd_tr[1], (TRACK_NAME_MAXLEN - 1)) < ((TRACK_NAME_MAXLEN - 1) - strlen("_TR")) );
	  strncat( ahd_tr, "_TR", TRACK_NAME_MAXLEN );
	  assert( TRACK_NAME_MAXLEN <= CBI_STAT_IDENT_LEN );
	  strncpy( pprof->body.ahead.tr_name, ahd_tr, TRACK_NAME_MAXLEN );
	  pprof->body.ahead.tr_prof = lkup_track_prof( pprof->body.ahead.tr_name );
	  if( !pprof->body.ahead.tr_prof ) {
	    printf( "warning: unknown ahead-track %s of %s detected, not found in TRACK of interlock table.\n", pprof->body.ahead.tr_name, pprof->route_name );
	  }
	}
      }
    }
    skip_chr( fp_src );
  }
  return (cnt + 1);
}

static TRACK_PROF_PTR pick_ahead_track ( ROUTE_PROF_PTR pro_prof ) {
  assert( pro_prof );
  assert( ! pro_prof->body.pahead );
  TRACK_PROF_PTR r = NULL;
  
  if( (strnlen(pro_prof->body.ahead.tr_name, CBI_STAT_IDENT_LEN) > 1) && pro_prof->body.ahead.tr_prof ) {
    int i;
    assert( pro_prof->body.ntrs >= 0 );
    for( i = 0; i < pro_prof->body.ntrs; i++ ) {
      if( !strncmp( pro_prof->body.tr[i].tr_name, pro_prof->body.ahead.tr_name, CBI_STAT_IDENT_LEN ) ) {
	assert( pro_prof->body.tr[i].tr_prof == pro_prof->body.ahead.tr_prof );
	pro_prof->body.pahead = &pro_prof->body.tr[i];
	r = pro_prof->body.ahead.tr_prof;
	break;
      }
    }
    if( !pro_prof->body.pahead ) {
      printf( "warning: ahead-track %s is not found in control-ones of %s.\n", pro_prof->body.ahead.tr_name, pro_prof->route_name );
    }
  }
  return r;
}

static struct route_tr *pick_dest_track ( const char *sig_dst, const int kickdn ) {
  assert( sig_dst );
  assert( kickdn > -2 );
  struct {
    struct route_tr *ptr_org;
    ROUTE_PROF_PTR pro_prof;
  } ro_org = {};
  
  if( kickdn >= 0 ) {
    ROUTE_PROF_PTR pr_prof = (kickdn == tracks_routes_prof.routes.profs.nixls) ? tracks_routes_prof.routes.profs.pcrnt_ixl : tracks_routes_prof.routes.profs.pprevs[kickdn];
    while( pr_prof < tracks_routes_prof.routes.pavail ) {
      assert( pr_prof );
      if( strncmp( pr_prof->orgdst.org.signame_org, sig_dst, CBI_STAT_IDENT_LEN ) == 0 ) {
	if( pr_prof->orgdst.org.porg_tr ) {
	  if( ro_org.ptr_org ) {
	    assert( ro_org.pro_prof );
	    TRACK_PROF_PTR pt_prof = pr_prof->orgdst.org.porg_tr->tr_prof;
	    if( !((strncmp(ro_org.ptr_org->tr_name, pr_prof->orgdst.org.porg_tr->tr_name, CBI_STAT_IDENT_LEN) != 0) ||
		  (ro_org.ptr_org->tr_prof ? (pt_prof ? ro_org.ptr_org->tr_prof != pt_prof : FALSE) : FALSE)) ) {
	      if( pt_prof ) {
		ro_org.ptr_org = pr_prof->orgdst.org.porg_tr;
		ro_org.pro_prof = pr_prof;
	      }
	    } else {
	      printf( "warning: inconsitency found on the origin tracks of %s and %s, for %s and %s respectively.\n", ro_org.ptr_org->tr_name, pr_prof->orgdst.org.porg_tr->tr_name,
		      (ro_org.pro_prof)->route_name, pr_prof->route_name );
	    }
	  } else {
	    ro_org.ptr_org = pr_prof->orgdst.org.porg_tr;
	    ro_org.pro_prof = pr_prof;
	  }
	}
      }
      pr_prof++;
    }
    if( !ro_org.ptr_org )
      ro_org.ptr_org = pick_dest_track( sig_dst, (kickdn - 1) );
  }
  return ro_org.ptr_org;
}

static int read_route_iltbls ( FILE *fp_src_sig,  FILE *fp_src_rel ) {
  assert( fp_src_sig );
  assert( fp_src_rel );
  int n = 0;
  
  read_iltbl_signal( fp_src_sig );
  read_iltbl_routerel( fp_src_rel );
  if( tracks_routes_prof.routes.profs.pcrnt_ixl ) {
    ROUTE_PROF_PTR pr_prof = tracks_routes_prof.routes.profs.pcrnt_ixl;
    while( pr_prof < tracks_routes_prof.routes.pavail ) {
      assert( pr_prof );
#if 0 // *****
      if(  strcmp( pr_prof->route_name, "S801A_S807A" ) == 0 ) {
	printf( "HIT" );
      }
#endif
      TRACK_PROF_PTR pahd_tr = NULL;
      pahd_tr = pick_ahead_track( pr_prof );
      if( pahd_tr ) {
	pr_prof->orgdst.org.porg_tr = link_orgahd_blks( pr_prof->apps.tr, pr_prof->apps.ntrs, pahd_tr );
      } else {	
	printf( "warning: missing ahead-track of %s.\n", pr_prof->route_name );
      }
#if 0 // *****
      prn_route_prof_lv0( pr_prof );
#endif
      pr_prof++;
      n++;
    }
  }
  return n;
}

static int fill_dest_tracks ( void ) {
  int n = 0;
  
  ROUTE_PROF_PTR pbkp_dst = tracks_routes_prof.routes.bkpatches.pdestin;
  ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.profs.pcrnt_ixl;
  assert( pprof );
  while( pprof < tracks_routes_prof.routes.pavail ) {
    assert( pprof );
    pprof->orgdst.dst.pdst_tr = pick_dest_track( pprof->orgdst.dst.signame_dst, tracks_routes_prof.routes.profs.nixls );
    if( ! pprof->orgdst.dst.pdst_tr ) {
      if( pbkp_dst ) {
	assert( tracks_routes_prof.routes.bkpatches.pdestin );
	pbkp_dst->orgdst.dst.pNext = pprof;
	pbkp_dst = pprof;
      } else {
	assert( !tracks_routes_prof.routes.bkpatches.pdestin );	  
	pbkp_dst = pprof;
	tracks_routes_prof.routes.bkpatches.pdestin = pbkp_dst;
      }
      pbkp_dst->orgdst.dst.pNext = NULL;
    } else
      n++;
    pprof++;    
  }
  return n;
}

static int bkpat_destin ( void ) {
  int res = 0;
  
  ROUTE_PROF_PTR *pprof = &tracks_routes_prof.routes.bkpatches.pdestin;
  assert( pprof );
  while( *pprof ) {
    assert( *pprof );
    assert( ! (*pprof)->orgdst.dst.pdst_tr );
    (*pprof)->orgdst.dst.pdst_tr = pick_dest_track( (*pprof)->orgdst.dst.signame_dst, tracks_routes_prof.routes.profs.nixls );
    if( (*pprof)->orgdst.dst.pdst_tr )
      *pprof = (*pprof)->orgdst.dst.pNext;
    else
      pprof = &(*pprof)->orgdst.dst.pNext;
  }
  {
    ROUTE_PROF_PTR p = tracks_routes_prof.routes.bkpatches.pdestin;
    while( p ) {
      res++;
      p = p->orgdst.dst.pNext;
    }
  }
  return res;
}

typedef struct blk_tracer {
  int sp;
  CBTC_BLOCK_PTR stack[MAX_TRACK_BLOCKS * ROUTE_MAX_CTRLTRACKS];
} BLK_TRACER, *BLK_TRACER_PTR;
static CBTC_BLOCK_PTR push_blk ( BLK_TRACER_PTR pstk, CBTC_BLOCK_PTR pblk ) {
  assert( pstk );
  assert( pblk );
  CBTC_BLOCK_PTR r = NULL;
  
  assert( pstk->sp < (MAX_TRACK_BLOCKS * ROUTE_MAX_CTRLTRACKS) );
  pstk->stack[pstk->sp] = pblk;
  r = pstk->stack[pstk->sp];
  pstk->sp++;
  return r;
}
static CBTC_BLOCK_PTR pop_blk ( BLK_TRACER_PTR pstk ) {
  assert( pstk );
  CBTC_BLOCK_PTR r = NULL;
  
  r = pstk->stack[pstk->sp];
  pstk->sp--;
  return r;
}

typedef enum WALK {
  ROUTEOUT,
  MISSTEP,
  BAD_KR,
  DEADEND,
  REACHOUT
} WALK;
typedef struct book {
  int ntrs;
  struct {
    struct route_tr *ptr;
  } ctrl_trax[ROUTE_MAX_CTRLTRACKS];
} BOOK, *BOOK_PTR;
static BOOL route_out( WALK *preason, CBTC_BLOCK_PTR pblk, BLK_TRACER_PTR pacc, BOOK_PTR pbok ) {
  assert( preason );
  assert( pblk );
  assert( pacc );
  assert( pbok );
  BOOL r = FALSE;
  
  BOOL found = FALSE;
  int i;
  for( i = 0; i < pbok->ntrs; i++ ) {
    assert( pblk );    
    assert( pbok );
    assert( pbok->ctrl_trax[i].ptr );
    if( strncmp( pbok->ctrl_trax[i].ptr->tr_name, cnv2str_il_sym(pblk->belonging_tr.track), CBI_STAT_IDENT_LEN ) == 0 ) {
      found = TRUE;
      break;
    }
  }  if( !found ) {
    *preason = ROUTEOUT;
    r = TRUE;
  } else {
    int i;
    for( i = 0; i < pacc->sp; i++ ) {
      if( pacc->stack[i] == pblk ) {
	*preason = MISSTEP;
	r = TRUE;
	break;
      }
    }
  }
  return r;
}

static BOOL reachout ( CBTC_BLOCK_PTR pblk, ROUTE_PROF_PTR pro_prof ) {
  assert( pblk );
  assert( pro_prof );
  BOOL r = FALSE;
  
  if( pro_prof->body.ntrs > 0 ) {
    struct route_tr *ptr = &pro_prof->body.tr[pro_prof->body.ntrs - 1];
    r = (strncmp( cnv2str_il_sym(pblk->belonging_tr.track), ptr->tr_name, CBI_STAT_IDENT_LEN ) == 0);
  } else {
    assert( pro_prof->body.ntrs == 0 );
    r = TRUE;
  }
  return r;
}

static WALK wandering( CBTC_BLOCK_PTR pblk, ROUTE_PROF_PTR pro_prof, BLK_TRACER_PTR pacc, BOOK_PTR pbok );
static WALK stepin_next( BLK_MORPH_PTR pmor_ahd, const int ln_id, ROUTE_PROF_PTR pro_prof, BLK_TRACER_PTR pacc, BOOK_PTR pbok ) {
  assert( pmor_ahd );
  assert( (ln_id > -1) && (ln_id < 2) );
  assert( pro_prof );
  assert( pacc );
  assert( pbok );
  WALK r = DEADEND;

  BLK_LINKAGE_PTR plnk = pmor_ahd->linkages[ln_id].bond.pln_neigh;
  if( plnk ) {
    assert( plnk->pmorph );
    assert( plnk->pmorph->pblock );
    r = wandering( plnk->pmorph->pblock, pro_prof, pacc, pbok );
  }
  return r;
}
static WALK wandering ( CBTC_BLOCK_PTR pblk, ROUTE_PROF_PTR pro_prof, BLK_TRACER_PTR pacc, BOOK_PTR pbok ) {
  assert( pblk );
  assert( pro_prof );
  assert( pacc );
  assert( pbok );
  WALK r = DEADEND;
  
  if( ! route_out( &r, pblk, pacc, pbok ) ) {
    push_blk( pacc, pblk );
    if( reachout( pblk, pro_prof ) )
      r = REACHOUT;
    else
      if( pblk->shape.num_morphs == 1 ) {      
	r = stepin_next( &pblk->shape.morphs[0], 0, pro_prof, pacc, pbok ); 
	if( r != REACHOUT ) {
	  r = stepin_next( &pblk->shape.morphs[0], 1, pro_prof, pacc, pbok );
	  if( r != REACHOUT )
	    pop_blk( pacc );
	}
      } else {
	int i;
	for( i = 0; i < pblk->shape.num_morphs; i++ ) {
	  BLK_MORPH_PTR pmor = &pblk->shape.morphs[i];
	  BOOL found = FALSE;
	  int j;	
	  for( j = 0; j < pmor->num_points; j++ ) {
	    int k;	 
	    for( found = FALSE, k = 0; k < pro_prof->body.npts; k++ ) {
	      char pt_kr[CBI_STAT_IDENT_LEN + 1] = "";
	      pt_kr[CBI_STAT_IDENT_LEN] = 0;
	      strncpy( pt_kr, pro_prof->body.pt[k].pt_name, CBI_STAT_IDENT_LEN );
	      if( pro_prof->body.pt[k].stat.normal ) {
		strncat( pt_kr, "_NKR", CBI_STAT_IDENT_LEN );
		if( strncmp( cnv2str_il_sym(pmor->points[j]), pt_kr, CBI_STAT_IDENT_LEN ) == 0 ) {
		  found = TRUE;
		  break;
		}
	      }
	      strncpy( pt_kr, pro_prof->body.pt[k].pt_name, CBI_STAT_IDENT_LEN );
	      if( pro_prof->body.pt[k].stat.reverse ) {	      
		strncat( pt_kr, "_RKR", CBI_STAT_IDENT_LEN );
		if( strncmp( cnv2str_il_sym(pmor->points[j]), pt_kr, CBI_STAT_IDENT_LEN ) == 0 ) {
		  found = TRUE;
		  break;
		}
	      }
	    }
	    if( !found ) {
	      r = BAD_KR;
	      break;
	    }
	  }
	  if( found ) {
	    r = stepin_next( pmor, 0, pro_prof, pacc, pbok );
	    if( r != REACHOUT ) {
	      r = stepin_next( pmor, 1, pro_prof, pacc, pbok );
	      if( r != REACHOUT )
		pop_blk( pacc );
	      else
		break;
	    }
	    break;
	  }
	}
      }
  } else
    assert( r != REACHOUT );
  return r;
}

static int trace_ctrl_tracks ( CBTC_BLOCK_PTR pro_blks[], ROUTE_PROF_PTR pro_prof, const int nro_blks ) {
  assert( pro_blks );
  assert( pro_prof );
  assert( nro_blks > MAX_TRACK_BLOCKS );
  BLK_TRACER blkstk = {};
  BOOK book = {};
  int r = -1;
  
  CBTC_BLOCK_PTR porg_blk = NULL;
  int i;
  book.ntrs = pro_prof->body.ntrs;
  for( i = 0; i < book.ntrs; i++ )
    book.ctrl_trax[i].ptr = &pro_prof->body.tr[i];
  assert( i == pro_prof->body.ntrs );
  assert( pro_prof->body.ntrs < ROUTE_MAX_CTRLTRACKS );
  if( pro_prof->orgdst.org.porg_tr ) {
    book.ctrl_trax[i].ptr = pro_prof->orgdst.org.porg_tr;
    book.ntrs++;
    {
      TRACK_PROF_PTR ptr_org = (pro_prof->orgdst.org.porg_tr)->tr_prof;
      if( ptr_org ) {
	if( ptr_org->consists_blks.nblks > 0 )
	  porg_blk = ptr_org->consists_blks.pblk_profs[0];
      }
    }
  }
  i = 0;
  while( !porg_blk ) {
    assert( pro_prof );
    TRACK_PROF_PTR ptr_org = NULL;
    if( i >= pro_prof->body.ntrs )
      break;
    ptr_org = pro_prof->body.tr[i].tr_prof;
    if( ptr_org ) {
      if( ptr_org->consists_blks.nblks > 0 )
	porg_blk = ptr_org->consists_blks.pblk_profs[0];
    }
    i++;
  }
  if( porg_blk )
    wandering( porg_blk, pro_prof, &blkstk, &book );
  i = 0;
  for( i = 0; i < blkstk.sp; i++ )
    pro_blks[i] = blkstk.stack[i];
  r = i;
  return r;
}

static int morph_ahead_blks ( BLK_MORPH_PTR pmphs_ahd[], TRACK_PROF_PTR ptr_ahd, ROUTE_PROF_PTR pro_prof, const int nmphs_ahd ) {
  assert( pmphs_ahd );
  assert( pro_prof );
  int nms = 0;
  if( ptr_ahd ) {
    struct route_sw *ppts[TURNOUT_MAX_POINTS] = {};
    int nps = 0;
    if( ptr_ahd->turnout.npts > 0 ) {
      int i;
      for( i = 0; i < ptr_ahd->turnout.npts; i++ ) {
	int j;
	for( j = 0; j < pro_prof->body.npts; j++ ) {
	  if( strncmp( ptr_ahd->turnout.point[i].pt_name, pro_prof->body.pt[j].pt_name, CBI_STAT_IDENT_LEN ) == 0 ) {
	    ppts[nps] = &pro_prof->body.pt[j];
	    nps++;
	  }
	}
      }
    panic:
      { 
	int i;    
	for( i = 0; i < ptr_ahd->consists_blks.nblks; i++ ) {
	  CBTC_BLOCK_PTR pblk = ptr_ahd->consists_blks.pblk_profs[i];
	  if( pblk ) {
	    int j;
	    for( j = 0; j < pblk->shape.num_morphs; j++ ) {
	      BOOL found = FALSE;
	      int k;
	      for( k = 0; k < pblk->shape.morphs[j].num_points; k++ ) {
		int l;
		for( found = FALSE, l = 0; l < nps; l++ ) {
		  assert( ppts[l] );
		  char pt_kr[CBI_STAT_IDENT_LEN + 1] = "";
		  pt_kr[CBI_STAT_IDENT_LEN] = 0;
		  strncpy( pt_kr, ppts[l]->pt_name, CBI_STAT_IDENT_LEN );
		  if( ppts[l]->stat.normal ) {
		    strncat( pt_kr, "_NKR", CBI_STAT_IDENT_LEN );
		    if( strncmp( cnv2str_il_sym(pblk->shape.morphs[j].points[k]), pt_kr, CBI_STAT_IDENT_LEN ) == 0 ) {
		      found = TRUE;
		      break;
		    }
		  }
		  strncpy( pt_kr, ppts[l]->pt_name, CBI_STAT_IDENT_LEN );
		  if( ppts[l]->stat.reverse ) {
		    strncat( pt_kr, "_RKR", CBI_STAT_IDENT_LEN );
		    if( strncmp( cnv2str_il_sym(pblk->shape.morphs[j].points[k]), pt_kr, CBI_STAT_IDENT_LEN ) == 0 ) {
		      found = TRUE;
		      break;
		    }
		  }
		}
		if( !found )
		  break;
	      }
	      if( found ) {
	      has_no_turnout:
		pmphs_ahd[nms] = &pblk->shape.morphs[j];
		nms++;
		break;
	      } else {
		if( pblk->shape.morphs[j].num_points == 0 ) {
		  assert( pblk->shape.num_morphs == 1 );
		  goto has_no_turnout;
		}
	      }
	    }
	  }
	}
      }
    } else {
      assert( ptr_ahd->turnout.npts == 0 );
      int i;
      for( i = 0; i < ptr_ahd->consists_blks.nblks; i++ ) {
	assert( ptr_ahd );
	CBTC_BLOCK_PTR pbs_ahd = ptr_ahd->consists_blks.pblk_profs[i];
	if( pbs_ahd ) {	
	  int j;
	  for( j = 0; j < pbs_ahd->shape.num_morphs; j++ ) {
	    assert( pbs_ahd );
	    if( pbs_ahd->shape.morphs[j].num_points > 0 ) {
	      int k;
	    failed:
	      for( k = 0; k < pro_prof->body.npts; k++ ) {
		ppts[nps] = &pro_prof->body.pt[k];
		nps++;
	      }
	      assert( nps == pro_prof->body.npts );
	      goto panic;
	    }
	  }
	}
      }
      if( ptr_ahd->hardbonds.nblks >= ptr_ahd->consists_blks.nblks ) {
	struct {	
	  struct fixed_pos *plnks;
	  BOOL chk;
	} blk_lnks[MAX_TRACK_BLOCKS] = {};
	for( i = 0; i < ptr_ahd->hardbonds.nblks; i++ ) {
	  blk_lnks[i].plnks = &ptr_ahd->hardbonds.pblk_fixes[i];
	  blk_lnks[i].chk = FALSE;
	}
	assert( i == ptr_ahd->hardbonds.nblks );
	for( i = 0; i < ptr_ahd->consists_blks.nblks; i++ ) {
	  assert( ptr_ahd );
	  CBTC_BLOCK_PTR pbs_ahd = ptr_ahd->consists_blks.pblk_profs[i];
	  if( pbs_ahd ) {
	    if( (pbs_ahd->shape.num_morphs == 1) && (pbs_ahd->shape.morphs[0].num_links == 2) ) {
	      BOOL found = FALSE;
	      int j;
	      for( j = 0; j < ptr_ahd->hardbonds.nblks; j++ ) {
		assert( ptr_ahd );
		assert( blk_lnks[j].plnks );
		if( (blk_lnks[j].plnks)->pprof == pbs_ahd ) {
		  int k;
		  if( (blk_lnks[j].plnks)->npos == 2 ) {
		    int fst = -1;
		    for( k = 0; k < 2; k++ ) {
		      if( (blk_lnks[j].plnks)->pos[k].plnk == &pbs_ahd->shape.morphs[0].linkages[0] ) {
			if( blk_lnks[j].chk ) {
			  printf( "fatal: ill-formed linkage found in (block, morpt_num, link_num) = (%s, %d, %d), such linkage seems to belong other blocks/morphs also.\n",
				  pbs_ahd->virt_blkname_str, 0, 0 );
			  goto failed;
			}
			fst = k;
			break;
		      }
		    }
		    if( fst > -1 ) {
		      assert( fst < 2 );
		      const int snd = (fst == 0) ? 1 : 0;
		      if( (blk_lnks[j].plnks)->pos[snd].plnk == &pbs_ahd->shape.morphs[0].linkages[1] ) {
			if( blk_lnks[j].chk ) {
			  printf( "fatal: ill-formed linkage found in (block, morpt_num, link_num) = (%s, %d, %d), such linkage seems to belong other blocks/morphs also.\n",
				  pbs_ahd->virt_blkname_str, 0, 0 );				
			  goto failed;
			}		      
			blk_lnks[j].chk = TRUE;
			found = TRUE;
			break;
		      }
		    }
		  }
		}
	      }
	      if( !found )
		goto failed;
	    } else
	      goto failed;
	  }
	}
	for( i = 0; i < ptr_ahd->hardbonds.nblks; i++ ) {
	  assert( ptr_ahd );
	  assert( blk_lnks[i].plnks );
	  assert( (blk_lnks[i].plnks)->pprof );
	  assert( (blk_lnks[i].plnks)->npos > 0 );
	  if( blk_lnks[i].chk ) {
	    assert( (blk_lnks[i].plnks)->npos == 2 );
	    BOOL omit = FALSE;
	    BLK_LINKAGE_PTR pfst = (blk_lnks[i].plnks)->pos[0].plnk;
	    BLK_LINKAGE_PTR psnd = (blk_lnks[i].plnks)->pos[1].plnk;
	    assert( pfst );
	    assert( psnd );
	    assert( pfst->pmorph == psnd->pmorph );       
	    if( ! pfst->bond.pln_neigh ) {
	      assert( pfst->bond.kind == LINK_NONE );
	      assert( ! (blk_lnks[i].plnks)->pos[0].bond );
	      pmphs_ahd[nms] = pfst->pmorph;
	      nms++;
	      omit = TRUE;
	    } else
	      assert( pfst->bond.kind != LINK_NONE );
	    if( ! psnd->bond.pln_neigh ) {
	      assert( psnd->bond.kind == LINK_NONE );
	      assert( ! (blk_lnks[i].plnks)->pos[1].bond );
	      if( !omit ) {
		pmphs_ahd[nms] = psnd->pmorph;
		nms++;
	      }
	    } else
	      assert( psnd->bond.kind != LINK_NONE );
	  }
	}
      } else
	goto failed;
    }
  }
  return nms;
}

struct frontier {
  int nms;
  TRACK_PROF_PTR pt_fro;
  BLK_MORPH_PTR pm_fro[MAX_TRACK_BLOCKS];
};
static int trylnk_ahead_blk ( struct frontier *pahead, struct frontier *pfront ) {
  struct fixed_pos ln_blks[MAX_TRACK_BLOCKS * 2] = {};
  int r = -1;
  
  int cnt = 0;
  int i;
  if( pfront ) {
    for( i = 0; i < pfront->nms; i++ ) {
      assert( pfront );
      assert( pfront->pm_fro[i] );
      int j;
      ln_blks[cnt].pprof = (pfront->pm_fro[i])->pblock;
      ln_blks[cnt].npos = (pfront->pm_fro[i])->num_links;
      for( j = 0; j < ln_blks[cnt].npos; j++ )
	ln_blks[cnt].pos[j].plnk = &(pfront->pm_fro[i])->linkages[j];
      cnt++;
    }
    assert( cnt <= MAX_TRACK_BLOCKS );
  }
  if( pahead ) {
    for( i = 0; i < pahead->nms; i++ ) {
      assert( pahead );
      assert( pahead->pm_fro[i] );
      int j;
      ln_blks[cnt].pprof = (pahead->pm_fro[i])->pblock;
      ln_blks[cnt].npos = (pahead->pm_fro[i])->num_links;
      for( j = 0; j < ln_blks[cnt].npos; j++ )
	ln_blks[cnt].pos[j].plnk = &(pahead->pm_fro[i])->linkages[j];
      cnt++;
    }
    assert( cnt <= (MAX_TRACK_BLOCKS * 2) );
  }
  {
    struct fixed_pos *pps[MAX_TRACK_BLOCKS * 2] = {};
    for( i = 0; i < cnt; i++ )
      pps[i] = &ln_blks[i];
    r = linking_blks( pps, cnt, LINK_SOFT );
  }
  return r;
}

static void creat_ctrl_tracks ( void ) {
  ROUTE_PROF_PTR pprof = tracks_routes_prof.routes.profs.pwhole;
  
  while( pprof < tracks_routes_prof.routes.pavail ) {
    assert( pprof );
#if 1 // *****
    if( strcmp( pprof->route_name, "S803B_VS801B" ) == 0 ) {
      printf( "HIT." );
    }
#endif
    struct frontier front = { -1 };
    struct frontier ahead = { -1 };
    int i;
    for( i = 0; i < pprof->body.ntrs; i++ ) {
      assert( pprof );
      struct frontier *pfro = NULL;
      struct frontier *pahd = NULL;
      if( i == 0 ) {
	if( pprof->orgdst.org.porg_tr ) {
	  int m = -1;
	  front.pt_fro = (pprof->orgdst.org.porg_tr)->tr_prof;
	  m = morph_ahead_blks( front.pm_fro, front.pt_fro, pprof, MAX_TRACK_BLOCKS );
	  assert( m > -1 );
	  front.nms = m;
	  trylnk_ahead_blk( NULL, &front );
	  pfro = &front;
	} else
	  pfro = NULL;
	if( pprof->body.pahead ) {
	  ahead.pt_fro = (pprof->body.pahead)->tr_prof;
	  pahd = &ahead;
	} else
	  pahd = NULL;
      } else {
	assert( i > 0 );
	front = ahead;
	ahead.pt_fro = pprof->body.tr[i].tr_prof;
	pfro = (front.nms > -1 ? &front : NULL);
	pahd = &ahead;
      }
      if( pahd ) {
	assert( pahd == &ahead );
	int n = -1;
	n = morph_ahead_blks( pahd->pm_fro, pahd->pt_fro, pprof, MAX_TRACK_BLOCKS );
	assert( n > -1 );
	pahd->nms = n;
      }     
      trylnk_ahead_blk( pahd, pfro );
    }
#if 1 // *****
    //if( strcmp( pprof->route_name, "S808A_VS802A" ) == 0 ) {
    {
	assert( pprof );
	const int ro_blk_maxnum = 256;
	CBTC_BLOCK_PTR ro_blks[ro_blk_maxnum] = {};
	int n = -1;
	n = trace_ctrl_tracks( ro_blks, pprof, ro_blk_maxnum );
	assert( n > -1 );      
	{
	  assert( pprof );
	  int i;
	  printf( "(route, [blocks]): (%s, [", pprof->route_name );
	  for( i = 0; i < n; i++ ) {
	    assert( ro_blks[i] );
	    if( i > 0 )
	      printf( ", " );
	    printf( "%s", ro_blks[i]->virt_blkname_str );
	  }
	  printf( "])\n" );
	  //assert( FALSE );
	}
    }
#endif
    pprof++;
  }
}

static int profile_routes ( FILE *fp_src_sig,  FILE *fp_src_rel ) {
  assert( fp_src_sig );
  assert( fp_src_rel );
  int n = -1;
  
  n = read_route_iltbls( fp_src_sig, fp_src_rel );
  bkpat_destin();
  fill_dest_tracks();
  creat_ctrl_tracks();
  
  return n;  
}

static int cons_route_profs ( const char *ixl ) {
  assert( ixl );
  int r = -1;
  char fname_sig[ILTBL_FILENAME_MAXLEN + 1] = "";
  FILE *fp_src_sig = NULL;
  
  fname_sig[ILTBL_FILENAME_MAXLEN] = 0;
  strncpy( fname_sig, ixl, ILTBL_FILENAME_MAXLEN );
  assert( strncmp( fname_sig, ixl, ILTBL_FILENAME_MAXLEN ) == 0 );
  strncat( fname_sig, "_SIGNAL.csv", ILTBL_FILENAME_MAXLEN );
  fp_src_sig = fopen( fname_sig, "r" );
  if( fp_src_sig ) {
    if( !ferror( fp_src_sig ) ) {
      char fname_rel[ILTBL_FILENAME_MAXLEN + 1] = "";
      FILE *fp_src_rel = NULL;
      fname_rel[ILTBL_FILENAME_MAXLEN] = 0;
      strncpy( fname_rel, ixl, ILTBL_FILENAME_MAXLEN );
      assert( strncmp( fname_rel, ixl, ILTBL_FILENAME_MAXLEN ) == 0 );
      strncat( fname_rel, "_ROUTEREL.csv", ILTBL_FILENAME_MAXLEN );
      fp_src_rel = fopen( fname_rel, "r" );
      if( fp_src_rel ) {
	if( !ferror( fp_src_rel ) ) {
	  r = profile_routes( fp_src_sig, fp_src_rel );
	}
      }
    }
  }
  return r;
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

static int emit_route_dataset ( FILE *fp_out ) {
  assert( fp_out );
  return 0;
}
static int gen_route_dataset ( FILE *fp_out ) {
  assert( fp_out );
  int r = -1;
  
  emit_route_dataset_prolog( fp_out );
  
  assert( tracks_routes_prof.routes.profs.nixls == 0 );
  tracks_routes_prof.routes.profs.pcrnt_ixl = tracks_routes_prof.routes.pavail;
  r = cons_route_profs( "BCGN" );
  
  tracks_routes_prof.routes.profs.pprevs[tracks_routes_prof.routes.profs.nixls++] = tracks_routes_prof.routes.profs.pcrnt_ixl;
  tracks_routes_prof.routes.profs.pcrnt_ixl = tracks_routes_prof.routes.pavail;
  r = cons_route_profs( "JLA" );
  
  emit_route_dataset( fp_out );
  print_route_prof( tracks_routes_prof.routes.profs.pwhole );
  
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
  tracks_routes_prof.routes.profs.nixls = 0;
  tracks_routes_prof.routes.profs.pwhole = calloc( ROUTE_PROF_DECL_MAXNUM, sizeof(ROUTE_PROF) );
  if( !tracks_routes_prof.routes.profs.pwhole )
    return r;
  else    
    tracks_routes_prof.routes.pavail = tracks_routes_prof.routes.profs.pwhole;
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

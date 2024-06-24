#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

char *cnv2name_cbi_stat_kind[] = {
#define CBI_STAT_KIND_DESC(enum, name) name,
#include "./cbi/cbi_stat_kind.def"
#undef CBI_STAT_KIND_DESC
  NULL
};

CBI_STAT_ATTR cbi_stat_prof[CBI_MAX_STAT_BITS];
static int frontier;

static CBI_STAT_ATTR_PTR cbi_stat_hash_budgets[CBI_STAT_HASH_BUDGETS_NUM];

static int hash_key ( char *ident ) {
  assert( ident );
  const int n = 5;
  assert( (n > 0) && (n <= CBI_STAT_IDENT_LEN) );
  
  int h;
  h = 0;
  {
    int i;
    for( i = 0; (i < n) && ident[i]; i++ ) {
      h = 13 * h + ident[i];
      h = (h < 0) ? ((h * -1) % CBI_STAT_HASH_BUDGETS_NUM) : h;
    }
  }
  return ( h % CBI_STAT_HASH_BUDGETS_NUM );
}

static BOOL chk_uniq_in_budget ( CBI_STAT_ATTR_PTR pE ) {
  assert( pE );
  BOOL found = FALSE;
  char id[CBI_STAT_IDENT_LEN + 1];
  
  id[CBI_STAT_IDENT_LEN] = 0;
  strncpy( id, pE->ident, CBI_STAT_IDENT_LEN );
  assert( ! strncmp(id, pE->ident, CBI_STAT_IDENT_LEN) );
  {
    CBI_STAT_ATTR_PTR p = pE->pNext_hsh;
    while( p ) {
      if( !strncmp(p->ident, id, CBI_STAT_IDENT_LEN) ) {
	found = TRUE;
	break;
      }
      p = p->pNext_hsh;
    }
    assert( !found );
  }
  return( !found );
}
static CBI_STAT_ATTR_PTR *walk_hash ( CBI_STAT_ATTR_PTR *ppB, char *ident ) {
  assert( ppB );
  assert( ident );
  CBI_STAT_ATTR_PTR *pp = ppB;
  
  assert( pp );
  while( *pp ) {
    if( !strncmp((*pp)->ident, ident, CBI_STAT_IDENT_LEN) ) {
#ifdef CHK_STRICT_CONSISTENCY
      assert( chk_uniq_in_budget(*pp) );
#endif // CHK_STRICT_CONSISTENCY
      break;
    }
    pp = &(*pp)->pNext_hsh;
  }
  return pp;
}

static void regist_hash ( CBI_STAT_ATTR_PTR pE ) {
  assert( pE );
  CBI_STAT_ATTR_PTR *ppB = NULL;
  
  {
    int h = -1;
    h = hash_key( pE->ident );
    assert( (h > -1) && (h < CBI_STAT_HASH_BUDGETS_NUM) );
    ppB = &cbi_stat_hash_budgets[h];
  }
  assert( ppB );
  pE->pNext_hsh = NULL;
  {
    CBI_STAT_ATTR_PTR *pp;
    pp = walk_hash( ppB, pE->ident );
    if( *pp ) {
      //assert( FALSE );
      errorF( "redefinition of cbi condition: %s\n", (*pp)->ident );
      pE->pNext_hsh = (*pp)->pNext_hsh;
    }
    assert( pp );
    assert( !(*pp) );
    *pp = pE;
  }
}

static CBI_STAT_ATTR_PTR re_hash ( char *ident, CBI_STAT_ATTR_PTR pE ) {
  assert( ident );
  assert( pE );
  CBI_STAT_ATTR_PTR r = NULL;
  CBI_STAT_ATTR_PTR *ppB = NULL;
  
  {
    int h = -1;
    h = hash_key( ident );
    assert( (h > -1) && (h < CBI_STAT_HASH_BUDGETS_NUM) );
    ppB = &cbi_stat_hash_budgets[h];
  }
  assert( ppB );
  
  {
    CBI_STAT_ATTR_PTR *pp = NULL;
    pp = walk_hash( ppB, ident );
    assert( pp );
    if( *pp ) {
      pE->pNext_hsh = (*pp)->pNext_hsh;
      r = *pp;
      *pp = pE;
    } else {
      *pp = pE;
      pE->pNext_hsh = NULL;
    }
  }
  return r;
}
CBI_STAT_ATTR_PTR cbi_stat_rehash ( char *ident, CBI_STAT_ATTR_PTR pE ) {
  assert( ident );
  assert( pE );
  return re_hash( ident, pE );
}

static CBI_STAT_ATTR_PTR conslt_hash ( char *ident ) {
  assert( ident );
  CBI_STAT_ATTR_PTR r = NULL;
  
  CBI_STAT_ATTR_PTR *ppB = NULL;
  {
    int h = -1;
    h = hash_key( ident );
    assert( (h > -1) && (h < CBI_STAT_HASH_BUDGETS_NUM) );
    ppB = &cbi_stat_hash_budgets[h];
  }
  assert( ppB );
  
  {
    CBI_STAT_ATTR_PTR *pp = NULL;
    pp = walk_hash( ppB, ident );
    assert( pp );
    r = *pp;
  }
  return r;
}
CBI_STAT_ATTR_PTR cbi_stat_idntify ( char *ident ) {
  assert( ident );
  return conslt_hash( ident );
}

static char *show_cbi_stat_bit_mask ( char *mask_name, int len, CBI_STAT_BIT_MASK mask ) {
  assert( mask_name );
  assert( len > strlen("CBI_STAT_BIT_x") );
  switch( mask ) {
  case CBI_STAT_BIT_0:
    strncpy( mask_name, "CBI_STAT_BIT_0", len );
    break;
  case CBI_STAT_BIT_1:
    strncpy( mask_name, "CBI_STAT_BIT_1", len );
    break;
  case CBI_STAT_BIT_2:
    strncpy( mask_name, "CBI_STAT_BIT_2", len );
    break;
  case CBI_STAT_BIT_3:
    strncpy( mask_name, "CBI_STAT_BIT_3", len );
    break;
  case CBI_STAT_BIT_4:
    strncpy( mask_name, "CBI_STAT_BIT_4", len );
    break;
  case CBI_STAT_BIT_5:
    strncpy( mask_name, "CBI_STAT_BIT_5", len );
    break;
  case CBI_STAT_BIT_6:
    strncpy( mask_name, "CBI_STAT_BIT_6", len );
    break;
  case CBI_STAT_BIT_7:
    strncpy( mask_name, "CBI_STAT_BIT_7", len );
    break;
  case END_OF_CBI_STAT_BIT_MASK:
    /* fall thru */
    default:
      assert( FALSE );
  }
  return mask_name;
}

static CBI_STAT_BIT_MASK cbi_stat_bit_mask_pattern ( int pos ) {
  assert( (pos >= 0) && (pos < 8) );
  CBI_STAT_BIT_MASK mask = END_OF_CBI_STAT_BIT_MASK;
  switch( pos ) {
  case 0:
    mask = CBI_STAT_BIT_7;
    break;
  case 1:
    mask = CBI_STAT_BIT_0;
    break;
  case 2:
    mask = CBI_STAT_BIT_1;
    break;
  case 3:
    mask = CBI_STAT_BIT_2;
    break;
  case 4:
    mask = CBI_STAT_BIT_3;
    break;
  case 5:
    mask = CBI_STAT_BIT_4;
    break;
  case 6:
    mask = CBI_STAT_BIT_5;
    break;
  case 7:
    mask = CBI_STAT_BIT_6;
    break;
  default:
    assert( FALSE );
  }
  assert( mask != END_OF_CBI_STAT_BIT_MASK );
  return mask;
}
typedef enum _cbi_lex_phase {
  CBI_LEX_NAME,
  CBI_LEX_GROUP,
  CBI_LEX_DISP,
  END_OF_CBI_LEX
} CBI_LEX_PHASE;
static BOOL cbi_lex ( char *src, int src_len, char *name, int name_len, int *pgroup, int *pdisp ) {
  assert( src );
  assert( (src_len > 0) && (src_len <= CBI_STAT_BITS_LEXBUF_SIZE) );
  assert( name );
  assert( (name_len > 0) && (name_len <= CBI_STAT_NAME_LEN) );
  assert( pgroup );
  assert( pdisp );
  BOOL r = TRUE;
  CBI_LEX_PHASE ph = CBI_LEX_NAME;
  char *pword = src;
  char *p = src;
  while( (p - src) < src_len ) {
    if( !(*p) ) {
      if( ph == CBI_LEX_DISP ) {
	*pdisp = atoi( pword );
	ph = END_OF_CBI_LEX;
      } else
	break;
    } else if( *p == ',' ) {
      *p = 0;
      switch( ph ) {
      case CBI_LEX_NAME:
	strncpy( name, pword, name_len );
	ph = CBI_LEX_GROUP;
	break;
      case CBI_LEX_GROUP:
	*pgroup = atoi( pword );
	ph = CBI_LEX_DISP;
	break;
      default:
	r = FALSE;
	break;
      }
    } else if( *p == '\n') {
      r = FALSE;
      break;
    } else {
      p++;
      continue;
    }
    pword = ++p;
  }
  if( ph != END_OF_CBI_LEX )
    r = FALSE;
  return r;
}

#if 1  // ***** for debugging.
#define DUP_FNAME "BCGN_dup.txt"
static FILE *fp_out = NULL;
static void dup_CBI_code_tbl ( const char *name, int group, int disp ) { // ***** for debugging.
  if( ! fp_out ) {
    fp_out = fopen( DUP_FNAME, "w" );
    assert( fp_out );
  }
  fprintf( fp_out, "%s,%d,%d\n", name, group, disp );
}
#endif
#define CBI_STAT_MASKNAME_MAXLEN 256
void dump_cbi_stat_prof ( void ) {
  int i;
  for( i = 0; i < CBI_MAX_STAT_BITS; i++ ) {
    printf( "name: %s\n", cbi_stat_prof[i].name );
    printf( "ident: %s\n", cbi_stat_prof[i].ident );
    printf( "disp.raw: %d\n", cbi_stat_prof[i].disp.raw );
    printf( "disp.bytes: %d\n", cbi_stat_prof[i].disp.bytes );
    printf( "disp.bits: %d\n", cbi_stat_prof[i].disp.bits );
    {
      char str[CBI_STAT_MASKNAME_MAXLEN + 1];
      str[CBI_STAT_MASKNAME_MAXLEN] = 0;
      show_cbi_stat_bit_mask( str, CBI_STAT_MASKNAME_MAXLEN, cbi_stat_prof[i].disp.mask );
      printf( "disp.mask: %s\n", str );
    }
    printf( "\n" );
  }
}
int load_cbi_code_tbl ( const char *fname ) {
  assert( fname );
  FILE *fp = NULL;
  
  fp = fopen( fname, "r" );
  if( fp ) {
    int lines = 0;
    int group = -1;
    int disp = -1;
    char name[CBI_STAT_NAME_LEN + 1];
    name[CBI_STAT_NAME_LEN] = 0;
    while( (! feof(fp)) && (frontier < CBI_MAX_STAT_BITS) ) {
      char buf[CBI_STAT_BITS_LEXBUF_SIZE + 1];
      buf[CBI_STAT_BITS_LEXBUF_SIZE] = 0;
      fscanf( fp, "%s\n", buf );
      if( ! cbi_lex( buf, CBI_STAT_BITS_LEXBUF_SIZE, name, CBI_STAT_NAME_LEN, &group, &disp ) ) {
	errorF( "failed lexical analyzing the CBI code-table, in line num: %d.\n,", lines );
	assert( FALSE );
      } else {
	CBI_STAT_ATTR_PTR pA = &cbi_stat_prof[frontier];
	assert( pA );
	strncpy( pA->name, name, CBI_STAT_NAME_LEN );
	strncpy( pA->ident, name, CBI_STAT_NAME_LEN );
	pA->group = (CBI_STAT_GROUP)group;
	pA->disp.raw = disp;
	{
	  int n = disp / 8;
	  int m = disp % 8;
	  assert( (m >= 0) && (m < 8) );
	  if( m > 0 )
	    pA->disp.bytes = n;
	  else {
	    assert( m == 0 );
	    pA->disp.bytes = ((n > 0) ? (n - 1) : n);
	  }
	  pA->disp.bits = m;
	}
	pA->disp.mask = cbi_stat_bit_mask_pattern( pA->disp.bits );
	frontier++;
      }
      lines++;
      //dup_CBI_code_tbl( name, group, disp ); // ***** for debugging.
    }
    fclose( fp );
    //fclose( fp_out ); // ***** for debugging.
  } else {
    errorF( "failed to open the file: %s.\n", fname );
    assert( FALSE );
  }

  {
    int i;
    for( i = 0; i < frontier; i++ )
      regist_hash( &cbi_stat_prof[i] );
    assert( i == frontier );
  }
  return frontier;
}

typedef enum _il_obj {
  S821B_S801B_R,
  S801B_S803B_R,
  S801B_S807B_R,
  S801B_S811B_R,
  S803B_S831B_R,
  S803B_S805A_R,
  S803B_VS831B_R,
  S803B_S803H_R,
  S831B_S821A_R,
  VS821B_S805B_R,
  S805B_S803B_R,
  S805B_S807B_R,
  S805B_S811B_R,
  S807B_S831B_R,
  S807B_S805A_R,
  S807B_VS831B_R,
  S807B_S803H_R,
  VS831B_VS821A_R,
  S832B_S802B_R,
  S802B_S804B_R,
  S802B_S810B_R,
  S802B_S816B_R,
  S804B_S822B_R,
  S804B_S806C_R,
  S804B_VS822B_R,
  S822B_S862C_R,
  VS832B_S806B_R,
  S806B_S804B_R,
  S806B_S810B_R,
  S806B_S816B_R,
  S810B_S822B_R,
  S810B_S806C_R,
  S810B_VS822B_R,
  VS822B_VS862C_R,
  S809B_S811B_R,
  S811B_S831B_R,
  S811B_S805A_R,
  S811B_VS831B_R,
  S811B_S801H_R,
  S811B_S803H_R,
  S811B_S849H_R,
  S808B_S804B_R,
  S808B_S810B_R,
  S808B_S816B_R,
  S812B_S816B_R,
  S814B_S816B_R,
  S816B_S822B_R,
  S816B_S806C_R,
  S816B_VS822B_R,
  S816B_S852H_R,
  S803B_VS801B_R,
  S803B_VS803B_R,
  S807B_VS801B_R,
  S807B_VS803B_R,
  S811B_VS801B_R,
  S811B_VS803B_R,
  S804B_VS802B_R,
  S804B_VS804B_R,
  S810B_VS802B_R,
  S810B_VS804B_R,
  S816B_VS802B_R,
  S816B_VS804B_R,
  ES821B_ES801B_R,
  ES801B_ES803B_R,
  ES801B_ES809B_R,
  ES801B_ES813B_R,
  ES803B_ES805B_R,
  ES803B_ES811B_R,
  ES805B_ES831B_R,
  ES831B_ES821A_R,
  ES823B_ES807B_R,
  ES807B_ES803B_R,
  ES807B_ES809B_R,
  ES807B_ES813B_R,
  ES809B_ES805B_R,
  ES809B_ES811B_R,
  ES811B_ES833B_R,
  ES833B_ES823A_R,
  ES832B_ES802B_R,
  ES802B_ES804B_R,
  ES802B_ES810B_R,
  ES802B_ES814B_R,
  ES804B_ES806B_R,
  ES804B_ES812B_R,
  ES806B_ES822B_R,
  ES822B_ES862C_R,
  ES834B_ES808B_R,
  ES808B_ES804B_R,
  ES808B_ES810B_R,
  ES808B_ES814B_R,
  ES810B_ES806B_R,
  ES810B_ES812B_R,
  ES812B_ES824B_R,
  ES824B_ES864C_R,
  ES813B_ES805B_R,
  ES813B_ES811B_R,
  ES814B_ES806B_R,
  ES814B_ES812B_R,
} IL_OBJ;

//{ROUTE_CONTROL, Sxxxy, Sxxxy,
struct attr {
  CBI_STAT_KIND kind;
  char ident[CBI_STAT_IDENT_LEN + 1];
  union {
    struct {
      IL_OBJ src;
      IL_OBJ dst;
    } route;
  } u;
};

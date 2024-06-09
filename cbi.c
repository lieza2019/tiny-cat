#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

static CBI_STAT_ATTR cbi_stat_prof[MAX_CBI_STAT_BITS];
static int frontier;

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

#if 0  // ***** for debugging.
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
int load_CBI_code_tbl ( const char *fname ) {
  assert( fname );
  FILE *fp = NULL;
  
  fp = fopen( fname, "r" );
  if( fp ) {
    int lines = 0;
    int group = -1;
    int disp = -1;
    char name[CBI_STAT_NAME_LEN + 1];
    name[CBI_STAT_NAME_LEN] = 0;
    while( ! feof(fp) ) {
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
  
  return frontier;
}

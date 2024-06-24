#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cbi.h"

#define CBI_LEX_PAT_LEN 256
#define CBI_STAT_RAWNAME_LEN 256
#define CBI_EXPAND_PAT_MAXNUM 5
typedef struct lex_il_obj {
  CBI_STAT_KIND kind;
  char match_pat[CBI_LEX_PAT_LEN + 1];
  char exp_ident_pat[CBI_LEX_PAT_LEN + 1];
  struct {
    char pat[CBI_LEX_PAT_LEN + 1];
  } exp[CBI_EXPAND_PAT_MAXNUM];
  char raw_name[CBI_STAT_RAWNAME_LEN + 1];
} LEX_IL_OBJ, *LEX_IL_OBJ_PTR;

#define CBI_IDENT_MAXLEN 256
#define CBI_MATCHED_CHRS_MAXLEN 256
#define CBI_LEX_SYMTBL_MAX_BUDGETS 64
typedef struct cbi_lex_symbol {
  char ident[CBI_IDENT_MAXLEN + 1];
  int sfx;
  char matched[CBI_MATCHED_CHRS_MAXLEN + 1];
  struct cbi_lex_symbol *pSucc;
} CBI_LEX_SYMBOL, *CBI_LEX_SYMBOL_PTR;
typedef struct cbi_lex_symtbl {
  CBI_LEX_SYMBOL budgets[CBI_LEX_SYMTBL_MAX_BUDGETS + 1];
} CBI_LEX_SYMTBL, *CBI_LEX_SYMTBL_PTR;

#define CBI_EXPAND_EMIT_MAXLEN 256

LEX_IL_OBJ cbi_lex_def[] = {
  /*
   * grammar:  {{kind, match_pattern, {expand_pat_1, expand_pat_2, expand_pat_3, expand_pat_4, expand_pat_5}, raw-stat-name}}
   * grammar': {{kind, match_pattern, expand_pat_4ident, {expand_pat_1, expand_pat_2, expand_pat_3, expand_pat_4, expand_pat_5}}}
   */
#include "cbi_pat_def.h"
  {END_OF_CBI_STAT_KIND, "", "", {{""}}}
};

static CBI_LEX_SYMBOL_PTR regist_symbol ( CBI_LEX_SYMTBL_PTR psymtbl, CBI_LEX_SYMBOL_PTR ancest, char *id, int id_len ) {
  assert( psymtbl );
  assert( id );
  assert( (id_len > 0) && (id_len <= CBI_IDENT_MAXLEN) );
  CBI_LEX_SYMBOL_PTR pS = NULL;
  {
    int i;
    for( i = 0; i < CBI_LEX_SYMTBL_MAX_BUDGETS; i++ ) {
      if( psymtbl->budgets[i].sfx < 1 )
	break;
    }
    assert( i < CBI_LEX_SYMTBL_MAX_BUDGETS );
    pS = &psymtbl->budgets[i];
  }
  assert( pS );
  {
    CBI_LEX_SYMBOL_PTR *pp = &ancest;
    assert( pp );
    int cnt = 1;
    while( *pp ) {
      assert( (*pp)->sfx == cnt );
      cnt++;
      pp = &(*pp)->pSucc;
      assert( pp );
    }
    strncpy( pS->ident, id, id_len );
    pS->sfx = cnt;
    pS->pSucc = NULL;
    *pp = pS;
  }
  return pS;
}

static CBI_LEX_SYMBOL_PTR walk_symtbl ( CBI_LEX_SYMTBL_PTR psymtbl, char *id, int id_len ) {
  assert( psymtbl );
  assert( id );
  assert( (id_len > 0) && (id_len <= CBI_IDENT_MAXLEN) );
  CBI_LEX_SYMBOL_PTR r = NULL;
  {
    BOOL found = FALSE;
    int i;
    for( i = 0; i < CBI_LEX_SYMTBL_MAX_BUDGETS; i++ ) {
      CBI_LEX_SYMBOL_PTR pS = &psymtbl->budgets[i];
      assert( pS );
      if( strncmp( pS->ident, id, id_len ) == 0 ) {
	assert( pS->sfx > 0 );
	found = TRUE;
	if( pS->sfx > 1 )
	  continue;
	r = pS;
	break;
      }
    }
    assert( found ? (r != NULL) : TRUE );
  }
  return r;
}

static char *match_name ( char *matched, int max_match_len, char *src, CBI_LEX_SYMBOL_PTR ppat ) {
  assert( matched );
  assert( (max_match_len > 0) && (max_match_len <= CBI_MATCHED_CHRS_MAXLEN) );
  assert( src );
  assert( ppat );
  
  assert( strnlen(ppat->ident, CBI_LEX_PAT_LEN) == 1 );
  assert( islower(ppat->ident[0]) );
  if( *src ) {
    matched[0] = *src;
    matched[1] = 0;
    src++;
  } else
    matched[0] = 0;
  return src;
}

#define MATCH_PAT_COL(pHd, pCrnt) (((int)((pCrnt) - (pHd))) + 1)
static char *lex_cbi_stat_name ( FILE *errfp, int line, CBI_LEX_SYMTBL_PTR psymtbl, char *src, int srclen, LEX_IL_OBJ_PTR plex ) {
  assert( psymtbl );
  assert( src );
  assert( (srclen > 0) && (srclen <= CBI_STAT_NAME_LEN) );
  assert( plex );
  BOOL err = FALSE;
  char *psrc = src;
  char *ppat = plex->match_pat;
  
  assert( psrc );
  assert( ppat );
  while( *ppat && (ppat < (plex->match_pat + CBI_LEX_PAT_LEN)) ) {
    if( islower(*ppat) ) {
      char var_id[2];
      int id_len = -1;
      var_id[0] = *ppat;
      var_id[1] = 0;
      id_len = strlen( var_id );
      assert( id_len == 1 );
      {
	CBI_LEX_SYMBOL_PTR pShd = NULL;
	CBI_LEX_SYMBOL_PTR pSnew = NULL;
	pShd = walk_symtbl( psymtbl, var_id, id_len );
	pSnew = regist_symbol( psymtbl, pShd, var_id, id_len );
	assert( pSnew );
	if( *psrc ) {
	  char matched_chrs[CBI_MATCHED_CHRS_MAXLEN + 1];
	  char *psrc1 = NULL;
	  matched_chrs[CBI_MATCHED_CHRS_MAXLEN] = 0;
	  psrc1 = match_name( matched_chrs, (sizeof(matched_chrs) - 1), psrc, pSnew );
	  assert( psrc1 );
	  strncpy( pSnew->matched, matched_chrs, CBI_MATCHED_CHRS_MAXLEN );
	  if( psrc < psrc1 ) {
	    psrc = psrc1;
	    ppat++;
	    continue;
	  }
	}	
      }
      fprintf( errfp, "line: %d, matching pattern, col= %d: has no bound,\n", line, MATCH_PAT_COL(plex->match_pat, ppat) );
      err = TRUE;
      break;
    } else {
      if( *psrc && (psrc < (src + srclen)) )
	if( *psrc == *ppat ) {
	  psrc++;
	  ppat++;
	  continue;
	}
      fprintf( errfp, "line: %d, matching pattern, col= %d: has no matching,\n", line, MATCH_PAT_COL(plex->match_pat, ppat) );
      err = TRUE;
      break;
    }
  }
  if( ! err ) {
    if( *ppat ) {
      assert( ppat < (plex->match_pat + CBI_LEX_PAT_LEN) );
      fprintf( stdout, "line: %d, matching pattern, col= %d: has no matching,\n", line, MATCH_PAT_COL(plex->match_pat, ppat) );
    } else {
      if( psrc < (src + strnlen(src, CBI_STAT_NAME_LEN)) ) {
	assert( *psrc );
	fprintf( stdout, "line: %d, matching pattern, col= %d: excessive unmatched characters remain,\n", line, MATCH_PAT_COL(plex->match_pat, ppat) );
      }
    }
  }
  return psrc;
}

static char *trim_pat_ident ( char *ident, int rem_chrs ) {
  assert( ident );
  char *p = ident;
  
  assert( p );
  while( *p && (p < (ident + rem_chrs)) ) {
    assert( p < (ident + rem_chrs) );
    if( p == ident ) {
      if( isalpha(*p) || (*p == '_') ) {
	if( islower(*p) )
	  break;
      } else
	break;
    } else {
      if( isalnum(*p) || (*p == '_') ) {
	if( islower(*p) )
	  break;
      } else
	break;
    }
    p++; 
  }
  return p;
}

static char *lex_pat_idx( int *pidx, char *digits, int rem_chrs ) {
  assert( pidx );
  assert( digits );
  char *p = digits;
  while( *p && (p < (digits + rem_chrs)) ) {
    assert( p < (digits + rem_chrs) );
    if( ! isdigit(*p) )
      break;
    p++;
  }
  {
    int n = -1;
    char atoi_buf[CBI_LEX_PAT_LEN + 1];
    atoi_buf[CBI_LEX_PAT_LEN] = 0;
    n = p - digits;
    assert( n >= 0 );
    if( n > 0 ) {
      strncpy( atoi_buf, digits, n );
      atoi_buf[n] = 0;
      *pidx = atoi( atoi_buf );
    }
  }
  return p;
}

#define EXP_PAT_COL(pHd, pCrnt) (((int)((pCrnt) - (pHd))) + 1)
static int lex_exp_pat ( FILE *errfp, int line, int patno, char *buf, int buflen, CBI_LEX_SYMTBL_PTR psymtbl, char *pat, int patlen ) {
  assert( errfp );
  assert( buf );
  assert( (buflen > 0) && (buflen <= CBI_EXPAND_EMIT_MAXLEN) );
  assert( psymtbl );
  assert( pat );
  assert( (patlen > 0) && (patlen <= CBI_LEX_PAT_LEN) );
  BOOL err = FALSE;
  const char *plim = pat + patlen;
  char *ppat = pat;
  char *pbuf = buf;
  
  assert( ppat );
  while( *ppat && (ppat < plim) ) {
    char *pp1 = NULL;
    pp1 = trim_pat_ident( ppat, (plim - ppat) );
    assert( pp1 );
    assert( pp1 <= plim );
    if( (pp1 == ppat) && (ppat < plim) ) {  // identifier over patterns, should be only 1 lowercase alphabet.
      assert( ppat );
      if( islower(*ppat) ) {
	char id[CBI_IDENT_MAXLEN + 1];
	id[CBI_IDENT_MAXLEN] = 0;
	strncpy( id, ppat, 1 );
	id[1] = 0;
	ppat++;
	assert( ppat <= plim );
	switch( *ppat ) {
	case '[':
	  assert( ppat < plim );
	  int idx = -1;
	  char *pp2 = NULL;
	  ppat++;
	  pp2 = lex_pat_idx( &idx, ppat, (plim - ppat) );
	  assert( pp2 );
	  assert( pp2 <= plim );
	  if( idx < 0 ) {  // LEXERR: index must be integers in the range on registration.
	    assert( pp2 == ppat );
	    err = TRUE;
	    fprintf( errfp, "line: %d, (expansion pattern, col)= (%d, %d): index must be integer, ", line, patno, EXP_PAT_COL(pat, pp2) );
	    goto err_illegal_chr;
	  } else {
	    ppat = pp2;
	    if( *ppat == ']' ) {
	      assert( ppat < plim );
	      CBI_LEX_SYMBOL_PTR pS = NULL;
	      int id_len = strnlen( id, CBI_IDENT_MAXLEN );
	      assert( (id_len > 0) && (id_len <= CBI_IDENT_MAXLEN) );
	      pS = walk_symtbl( psymtbl, id, id_len );
	      {
		int cnt;
		for( cnt = 1; pS; cnt++ ) {
		  assert( pS->sfx == cnt );
		  if( pS->sfx == idx )
		    break;
		  pS = pS->pSucc;
		}
	      }
	      if( !pS ) {  // LEXERR: specified index is out of scope over the variable registered on.
		err = TRUE;
		fprintf( errfp, "line :%d, (expansion pattern, col)= (%d, %d): index is out of range over the variables registered on.\n", line, patno, EXP_PAT_COL(pat, ppat) );
		break;
	      }
	      assert( pbuf );
	      assert( pbuf < (buf + buflen) );
	      {
		const int subst_strlen = strnlen( pS->matched, CBI_MATCHED_CHRS_MAXLEN );
		const int rem_chrs = (buf + buflen) - pbuf;
		const int emit_len = subst_strlen < rem_chrs ? subst_strlen : rem_chrs;
		strncpy( pbuf, pS->matched, emit_len );
		pbuf[emit_len] = 0;
		pbuf = &pbuf[emit_len];
	      }
	      assert( pbuf <= (buf + buflen) );
	      ppat++;
	      break;
	    } else {  // LEXERR: missing closing ']' for index description over variables.
	      err = TRUE;
	      fprintf( errfp, "line: %d, (expansion pattern, col)= (%d, %d): missing the closing ']' for index description over the variables", line, patno, EXP_PAT_COL(pat, ppat) );
	      if( *ppat ) {
		assert( ppat < plim );
		fprintf( errfp, ", " );
		goto err_illegal_chr;
	      }
	      fprintf( errfp, ".\n" );
	      break;
	    }
	  }
	default: // includes the case of (*ppat == 0).
	  /* lowercase of alphabet, i.e. letter, without trailing '[' for beginning index notation,
	     should be parsed and accepted as a character consists of the bare string to be simply
	     expanded onto the output. */
	  {
	    char *w = pp1;
	    pp1 = ppat;
	    ppat = w;
	  }
	  goto emit_chrs;
	}
	if( err )
	  break;
      } else {  // LEXERR: illegal character detected.
	err = TRUE;
	fprintf( errfp, "line:%d, (expansion pattern, col)= (%d, %d): ", line, patno, EXP_PAT_COL(pat, ppat) );
	err_illegal_chr:
	fprintf( errfp, "illegal character detected.\n" );
	break;
      }
    } else {
    emit_chrs:
      assert( ppat < pp1 );
      assert( pp1 <= plim );
      const int rem_chrs = (buf + buflen) - pbuf;
      const int emit_len = (pp1 - ppat) < rem_chrs ? (pp1 - ppat) : rem_chrs;
      strncpy( pbuf, ppat, emit_len );
      pbuf[emit_len] = 0;
      pbuf = &pbuf[emit_len];
      assert( pbuf <= (buf + buflen) );
      ppat = pp1;
    }
  }
  if( !((ppat == plim) && (*ppat == 0)) )
    err = TRUE;
  
  return( strnlen(buf, buflen) * (err ? -1 : 1) );
}
  
static int emit_il_obj_name ( FILE *fp, int line, CBI_LEX_SYMTBL_PTR psymtbl, LEX_IL_OBJ_PTR plex ) {
  assert( fp );
  assert( psymtbl );
  assert( plex );
  BOOL err = FALSE;
  int cnt = 0;
  {
    int i;
    for( i = 0; i < CBI_EXPAND_PAT_MAXNUM; i++ ) {
      char emit_buf[CBI_EXPAND_EMIT_MAXLEN + 1];
      emit_buf[CBI_EXPAND_EMIT_MAXLEN] = 0;
      if( ! plex->exp[i].pat[0] )
	break;
      {
	int n = -1;
	n = lex_exp_pat( stdout, line, (i + 1), emit_buf, CBI_EXPAND_EMIT_MAXLEN, psymtbl, plex->exp[i].pat, strnlen(plex->exp[i].pat, CBI_LEX_PAT_LEN) );
	assert( (abs(n) >= 0) && (abs(n) <= CBI_EXPAND_EMIT_MAXLEN) );
	emit_buf[abs(n)] = 0;
	if( n > 0 ) {
	  char *name = cnv2name_cbi_stat_kind[plex->kind];
	  assert( name );
	  fprintf( fp, "%s: ", name );
	  fprintf( fp, "%s, ", emit_buf );
	  cnt++;
	} else {
	  err = TRUE;
	  break;
	}
      }
    }
  }
  return ( cnt * (err ? -1 : 1) );
}

BOOL enum_il_objs ( FILE *fp, int line, CBI_LEX_SYMTBL_PTR psymtbl, CBI_STAT_ATTR_PTR pattr, LEX_IL_OBJ_PTR plex ) {
  assert( fp );
  assert( psymtbl );
  assert( pattr );
  assert( plex );
  BOOL r = FALSE;
  strncpy( plex->raw_name, pattr->name, CBI_STAT_NAME_LEN );
  {
    char src[CBI_STAT_NAME_LEN + 1];
    src[CBI_STAT_NAME_LEN] = 0;
    strncpy( src, plex->raw_name, CBI_STAT_NAME_LEN );
    {
      char *src1;
      src1 = lex_cbi_stat_name( stdout, line, psymtbl, src, strnlen(src, CBI_STAT_NAME_LEN), plex );
      assert( src1 );
      if( src1 >= (src + strnlen(src, CBI_STAT_NAME_LEN)) ) {
	assert( src1 == (src + strnlen(src, CBI_STAT_NAME_LEN)) );
	if( emit_il_obj_name( fp, line, psymtbl, plex ) > 0 )
	  r = TRUE;
      }
    }
  }
  return r;
}

#if 0
int main ( void ) {
  int n = -1;
  
  n = load_cbi_code_tbl ( "./BOTANICAL_GARDEN.csv" );
  assert( (n >= 0) && (n <= CBI_MAX_STAT_BITS) );
  printf( "read %d entries from csv.\n", n );
  
  // test correctness on construction of hash-map for cbi state bits.
  {
    int i;
    for( i = 0; i < n; i++ ) {
      CBI_STAT_ATTR_PTR pE = NULL;
      pE = cbi_stat_idntify( cbi_stat_prof[i].ident );
      assert( pE );
      assert( ! strncmp(pE->ident, cbi_stat_prof[i].ident, CBI_STAT_IDENT_LEN) );
    }
  }
  return 0;
}
#else
int main ( void ) {
  CBI_LEX_SYMTBL S;
  int n;
#if 0
  n = load_CBI_code_tbl ( "./test.csv" );
  assert( n == 1 );
#endif
  n = load_cbi_code_tbl ( "./BOTANICAL_GARDEN.csv" );
  printf( "read %d entries from csv.\n", n );
  {
    CBI_LEX_SYMTBL_PTR pS = &S;
    int i = 0;
    while( cbi_lex_def[i].kind != END_OF_CBI_STAT_KIND ) {
      int j;
      for( j = 0; (j < CBI_MAX_STAT_BITS) && cbi_stat_prof[j].name[0]; j++ ) {
	assert( pS );
	memset( pS, 0, sizeof(S) );
	printf( "%d: ", (j + 1) );
	enum_il_objs( stdout, (i + 1), pS, &cbi_stat_prof[j], &cbi_lex_def[i] );
	printf( "\n" );
      }
      i++;
    }
  }
  return 0;
}
#endif

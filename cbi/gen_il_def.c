#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cbi.h"

#define CBI_LEX_ERR_PREFX_MAXCHRS 256
#define CBI_LEX_EMIT_PREFX_MAXCHRS 256
#define CBI_LEX_ERR_SUFIX_MAXCHRS 256
#define CBI_LEX_EMIT_SUFIX_MAXCHRS 256
typedef struct prefx_sufix {
  struct {
    char err[CBI_LEX_ERR_PREFX_MAXCHRS + 1];
    char emit[CBI_LEX_EMIT_PREFX_MAXCHRS + 1];
  } prefix;
  struct {
    char err[CBI_LEX_ERR_SUFIX_MAXCHRS + 1];
    char emit[CBI_LEX_EMIT_SUFIX_MAXCHRS + 1];
  } suffix;
} PREFX_SUFIX, *PREFX_SUFIX_PTR;

#define CBI_LEX_PAT_MAXLEN 256
#define CBI_EXPAND_PAT_MAXNUM 3
typedef struct lex_il_obj {
  CBI_STAT_KIND il_stat_kind;
  char match_pat[CBI_LEX_PAT_MAXLEN + 1];
  char exp_ident_pat[CBI_LEX_PAT_MAXLEN + 1];
  struct {
    CBI_STAT_KIND il_sym_kind;
    char pat[CBI_LEX_PAT_MAXLEN + 1];
  } exp[CBI_EXPAND_PAT_MAXNUM];
  char raw_name[CBI_STAT_NAME_LEN + 1];
  char label[CBI_STAT_IDENT_LEN + 1];
} LEX_CBI_OBJ, *LEX_CBI_OBJ_PTR;

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

#define CBI_STAT_LABELNG_FNAME "./cbi_stat_label.h"
#define IL_INSTANCES_EMIT_FNAME "./il_obj_instance_desc.h"

LEX_CBI_OBJ cbi_lex_def[] = {
  /*
   * grammar': {cbi_stat_kind, cbi_stat_match_pattern, cbi_stat_expand_label, {{sym_kind1, sym_expand_instance1, sym_expand_str1}, ..., {sym_kind3, sym_expand_instance3, sym_expand_str3}}}
   */
#include "cbi_pat.def"
  {END_OF_CBI_STAT_KIND, "", "", {{END_OF_CBI_STAT_KIND, ""}}}
};

#define IL_OBJ_HASH_BUDGETS_NUM 256
struct {
  CBI_STAT_ATTR_PTR budgets[IL_OBJ_HASH_BUDGETS_NUM];
  CBI_STAT_ATTR sea[CBI_MAX_STAT_BITS];
  int frontier;
  CBI_STAT_ATTR_PTR ptop;
  CBI_STAT_ATTR_PTR *pplast;
} il_syms_hash;

static CBI_LEX_SYMBOL_PTR regist_symbol ( CBI_LEX_SYMTBL_PTR psymtbl, CBI_LEX_SYMBOL_PTR ancest, char *id, int id_len ) {
  assert( psymtbl );
  assert( id );
  assert( (id_len > 0) && (id_len < CBI_IDENT_MAXLEN) );
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
  assert( (id_len > 0) && (id_len < CBI_IDENT_MAXLEN) );
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
  
  assert( strnlen(ppat->ident, CBI_LEX_PAT_MAXLEN) == 1 );
  assert( islower((int)ppat->ident[0]) );
  if( *src ) {
    matched[0] = *src;
    matched[1] = 0;
    src++;
  } else
    matched[0] = 0;
  return src;
}

#define MATCH_PAT_COL(pHd, pCrnt) (((int)((pCrnt) - (pHd))) + 1)
static char *lex_match_pattrn ( BOOL *pr, FILE *errfp, PREFX_SUFIX_PTR pprsf, int line, CBI_LEX_SYMTBL_PTR psymtbl, char *src, int srclen, LEX_CBI_OBJ_PTR plex ) {
  assert( pr );
  assert( errfp );
  assert( pprsf );
  assert( psymtbl );
  assert( src );
  assert( (srclen > 0) && (srclen < CBI_STAT_NAME_LEN) );
  assert( plex );
  BOOL err = FALSE;
  char *psrc = src;
  char *ppat = plex->match_pat;
  
  assert( psrc );
  assert( ppat );
  *pr = FALSE;
  while( *ppat && (ppat < (plex->match_pat + CBI_LEX_PAT_MAXLEN)) ) {
    if( (*ppat != '\\') && islower((int)*ppat) ) {
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
      assert( pprsf->prefix.err );
      assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
      fprintf( errfp, "%d: matching pattern, col= %d: has no bound, from %s.\n", line, MATCH_PAT_COL(plex->match_pat, ppat), pprsf->prefix.err );
      err = TRUE;
      break;
    } else {
      assert( *ppat && (ppat < (plex->match_pat + CBI_LEX_PAT_MAXLEN)) );
      if( *psrc && (psrc < (src + srclen)) ) {
	if( *ppat == '\\' ) {
	  ppat++;
	  if( !(*ppat && (ppat < (plex->match_pat + CBI_LEX_PAT_MAXLEN))) ) {
	    fprintf( errfp, "%d: matching pattern, col= %d: escaping character has no meanings, from %s.\n", line, MATCH_PAT_COL(plex->match_pat, ppat), pprsf->prefix.err );
	    break;
	  }
	  /* fall thru. */
	}
	if( *ppat == *psrc ) {
	  psrc++;
	  ppat++;
	  continue;
	}
      }
      assert( pprsf->prefix.err );
      assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
#if 0 // ***** for debugging.
      fprintf( errfp, "%d: matching pattern, col= %d: has no matching with %s.\n", line, MATCH_PAT_COL(plex->match_pat, ppat), pprsf->prefix.err );
#endif
      err = TRUE;
      break;
    }
  }
  if( ! err ) {
    if( *ppat ) {
      assert( ppat < (plex->match_pat + CBI_LEX_PAT_MAXLEN) );
      assert( pprsf->prefix.err );
      assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
#if 0 // ***** for debugging.
      fprintf( errfp, "%d: matching pattern, col= %d: has no matching with %s.\n", line, MATCH_PAT_COL(plex->match_pat, ppat), pprsf->prefix.err );
#endif
    } else {
      if( psrc < (src + strnlen(src, CBI_STAT_NAME_LEN)) ) {
	assert( *psrc );
	assert( pprsf->prefix.err );
	assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
	fprintf( errfp, "%d: matching pattern, col= %d: excessive unmatched characters remain, from %s.\n", line, MATCH_PAT_COL(plex->match_pat, ppat), pprsf->prefix.err );
      } else
	*pr = TRUE;
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
      if( isalpha((int)*p) || (*p == '_') ) {
	if( islower((int)*p) )
	  break;
      } else
	break;
    } else {
      if( isalnum((int)*p) || (*p == '_') ) {
	if( islower((int)*p) )
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
    if( ! isdigit((int)*p) )
      break;
    p++;
  }
  {
    int n = -1;
    char atoi_buf[CBI_LEX_PAT_MAXLEN + 1];
    atoi_buf[CBI_LEX_PAT_MAXLEN] = 0;
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
static int lex_exp_pattrn ( FILE *errfp, PREFX_SUFIX_PTR pprsf, int line, int patno, char *buf, int buflen, CBI_LEX_SYMTBL_PTR psymtbl, char *pat, int patlen ) {
  assert( errfp );
  assert( pprsf );
  assert( buf );
  assert( (buflen > 0) && (buflen <= CBI_EXPAND_EMIT_MAXLEN) );
  assert( psymtbl );
  assert( pat );
  assert( (patlen > 0) && (patlen < CBI_LEX_PAT_MAXLEN) );
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
      if( islower((int)*ppat) ) {
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
	    assert( pprsf->prefix.err );
	    assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
	    fprintf( errfp, "%d: (expansion pattern, col)= (%d, %d): index must be integer, ", line, patno, EXP_PAT_COL(pat, pp2) );
	    goto err_illegal_chr;
	  } else {
	    ppat = pp2;
	    if( *ppat == ']' ) {
	      assert( ppat < plim );
	      CBI_LEX_SYMBOL_PTR pS = NULL;
	      int id_len = strnlen( id, CBI_IDENT_MAXLEN );
	      assert( (id_len > 0) && (id_len < CBI_IDENT_MAXLEN) );
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
		assert( pprsf->prefix.err );
		assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
		fprintf( errfp, "%d: (expansion pattern, col)= (%d, %d): index is out of range over the variables registered on.\n", line, patno, EXP_PAT_COL(pat, ppat) );
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
	      assert( pprsf->prefix.err );
	      assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
	      fprintf( errfp, "%d: (expansion pattern, col)= (%d, %d): missing the closing ']' for index description over the variables", line, patno, EXP_PAT_COL(pat, ppat) );
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
	assert( pprsf->prefix.err );
	assert( strlen(pprsf->prefix.err) < CBI_LEX_ERR_PREFX_MAXCHRS );
	fprintf( errfp, "%d: (expansion pattern, col)= (%d, %d): ", line, patno, EXP_PAT_COL(pat, ppat) );
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

static CBI_STAT_ATTR_PTR hash_regist ( CBI_STAT_ATTR_PTR pE, int line ) {
  assert( pE );
  assert( line > 0 );
  char buf[256];
  buf[255] = 0;
  {
    int n = -1;
    n = snprintf( buf, 256, "%d: ", line );
    assert( n > 0 );
  }
  return cbi_stat_regist( il_syms_hash.budgets, IL_OBJ_HASH_BUDGETS_NUM, pE, TRUE, buf );
}

static int emit_il_instances ( FILE *fp, FILE *errfp, PREFX_SUFIX_PTR pprsf, int line, CBI_LEX_SYMTBL_PTR psymtbl, LEX_CBI_OBJ_PTR plex, CBI_STAT_ATTR_PTR pattr ) {
  assert( fp );
  assert( errfp );
  assert( pprsf );
  assert( psymtbl );
  assert( plex );
  assert( pattr );
  BOOL err = FALSE;
  
  int cnt = 0;
  {
    struct {
      CBI_STAT_ATTR_PTR fst;
      CBI_STAT_ATTR_PTR *pp;
    } family = { NULL, NULL };
    int i;
    for( i = 0; i < CBI_EXPAND_PAT_MAXNUM; i++ ) {
      char emit_buf[CBI_EXPAND_EMIT_MAXLEN + 1];
      emit_buf[CBI_EXPAND_EMIT_MAXLEN] = 0;
      if( ! plex->exp[i].pat[0] )
	break;
      {
	CBI_STAT_ATTR_PTR pE = NULL;
	int n = -1;
	n = lex_exp_pattrn( errfp, pprsf, line, (i + 1), emit_buf, CBI_EXPAND_EMIT_MAXLEN, psymtbl, plex->exp[i].pat, strnlen(plex->exp[i].pat, CBI_LEX_PAT_MAXLEN) );
	assert( (abs(n) >= 0) && (abs(n) < CBI_EXPAND_EMIT_MAXLEN) );
	emit_buf[abs(n)] = 0;
	if( n > 0 ) {
	  assert( il_syms_hash.frontier < CBI_MAX_STAT_BITS );
	  pE = &il_syms_hash.sea[il_syms_hash.frontier++];
	  assert( pE );
	  pE->ident[CBI_STAT_IDENT_LEN] = 0;
	  strncpy( pE->ident, emit_buf, CBI_STAT_IDENT_LEN );
	  pE->name[CBI_STAT_NAME_LEN] = 0;
	  strncpy( pE->name, plex->raw_name, CBI_STAT_NAME_LEN );
	  pE->kind = plex->exp[i].il_sym_kind;
	  pE->src.fname[CBI_CTRLTBL_FILENAME_CHRLEN_MAX] = 0;
	  strncpy( pE->src.fname, pattr->src.fname, CBI_CTRLTBL_FILENAME_CHRLEN_MAX );
	  pE->src.line = pattr->src.line;
	  assert( plex );
	  pE->decl_gen.plex_il_obj = (void *)plex;
#if 0
	  printf( "HIT: (line: pat, name, ident) = (%d: %s, %s, %s)\n", line, plex->exp[i].pat, plex->raw_name, emit_buf ); // ***** for debugging.
#endif
	  {
	    assert( pE );
	    CBI_STAT_ATTR_PTR w = NULL;
	    w = hash_regist( pE, line );
	    assert( w );
	    if( w != pE ) {
	      w->ident[CBI_STAT_IDENT_LEN] = 0;
	      assert( !strncmp( w->ident, pE->ident, CBI_STAT_IDENT_LEN ) );
	      fprintf( errfp, "%d: found redeclaration of %s, and overridden.\n", line, w->ident );
	      continue;
	    }
	    assert( w == pE );
	    assert( w );
	    w->decl_gen.pNext = NULL;
	    w->decl_gen.pFamily = NULL;
	    if( !family.fst ) {
	      assert( !family.pp );
	      family.fst = w;
	    } else {
	      assert( family.pp );
	      *family.pp = w;
	    }
	    family.pp = &w->decl_gen.pFamily;
	  }
	  cnt++;
	} else {
	  err = TRUE;
	  break;
	}
      }
    }
    if( cnt > 0 ) {
      assert( family.fst );
      assert( family.pp );
      assert( !(*family.pp) );
      family.fst->decl_gen.nentities = cnt;
      if( ! il_syms_hash.ptop ) {
	assert( !il_syms_hash.pplast );
	il_syms_hash.ptop = family.fst;
      } else {
	assert( il_syms_hash.pplast );
	*(il_syms_hash.pplast) = family.fst;
      }
      il_syms_hash.pplast = &family.fst->decl_gen.pNext;
    }
  }
  return ( cnt * (err ? -1 : 1) );
}

static int emit_stat_abbrev ( FILE *fp, FILE *errfp, PREFX_SUFIX_PTR pprsf, int line, CBI_LEX_SYMTBL_PTR psymtbl, LEX_CBI_OBJ_PTR plex, CBI_STAT_ATTR_PTR pattr ) {
  assert( fp );
  assert( errfp );
  assert( pprsf );
  assert( psymtbl );
  assert( plex );
  assert( pattr );
  BOOL err = FALSE;
  
  assert( plex->raw_name );
  {
    const char *kind_s = NULL;
    kind_s = cnv2str_cbi_stat[plex->il_stat_kind];
    assert( kind_s );
    if( plex->exp_ident_pat[0] ) {
      char emit_buf[CBI_EXPAND_EMIT_MAXLEN + 1];
      int n = -1;    
      emit_buf[CBI_EXPAND_EMIT_MAXLEN] = 0;
      n = lex_exp_pattrn( errfp, pprsf, line, 0, emit_buf, CBI_EXPAND_EMIT_MAXLEN, psymtbl, plex->exp_ident_pat, strnlen(plex->exp_ident_pat, CBI_LEX_PAT_MAXLEN) );
      assert( (abs(n) >= 0) && (abs(n) < CBI_EXPAND_EMIT_MAXLEN) );
      emit_buf[abs(n)] = 0;
      if( n > 0 ) {
	assert( plex );
	fprintf( fp, "%s ", pprsf->prefix.emit );
	fprintf( fp, "%s", kind_s );
	fprintf( fp, ", \"%s\"", plex->raw_name );
	strncpy( plex->label, emit_buf, CBI_STAT_IDENT_LEN );
	plex->label[CBI_STAT_IDENT_LEN] = 0;
	fprintf( fp, ", \"%s\"", plex->label );
	fprintf( fp, " %s", pprsf->suffix.emit );
      } else
	err = TRUE;
    } else {
      fprintf( fp, "%s ", pprsf->prefix.emit );
      fprintf( fp, "%s", kind_s );
      fprintf( fp, ", \"%s\"", plex->raw_name );
      fprintf( fp, ", \"%s\"", "NO_ABBREV" );
      fprintf( fp, " %s", pprsf->suffix.emit );
    }
  }
  return (err ? -1 : 1);
}

static int transduce ( FILE *fp, FILE *errfp, PREFX_SUFIX_PTR pprsf, int line, CBI_LEX_SYMTBL_PTR psymtbl, LEX_CBI_OBJ_PTR plex, CBI_STAT_ATTR_PTR pattr ) {
  assert( fp );
  assert( errfp );
  assert( pprsf );
  assert( psymtbl );
  assert( plex );
  assert( pattr );  
  int r = -1;
  
  strncpy( plex->raw_name, pattr->name, CBI_STAT_NAME_LEN );
  {
    char src[CBI_STAT_NAME_LEN + 1];
    src[CBI_STAT_NAME_LEN] = 0;
    strncpy( src, plex->raw_name, CBI_STAT_NAME_LEN );
    {
      BOOL r_lex = FALSE;
      char *src1 = NULL;
      src1 = lex_match_pattrn( &r_lex, errfp, pprsf, line, psymtbl, src, strnlen(src, CBI_STAT_NAME_LEN), plex );
      assert( src1 );
      if( r_lex )
	if( src1 >= (src + strnlen(src, CBI_STAT_NAME_LEN)) ) {
	  int r_emit_abb = -1;
	  int r_emit_ins = -1;
	  assert( src1 == (src + strnlen(src, CBI_STAT_NAME_LEN)) );
	  strncpy( pprsf->prefix.emit, "{", CBI_LEX_EMIT_PREFX_MAXCHRS );
	  strncpy( pprsf->suffix.emit, "}," , CBI_LEX_EMIT_PREFX_MAXCHRS );
	  if( (r_emit_abb = emit_stat_abbrev( fp, errfp, pprsf, line, psymtbl, plex, pattr )) > 0 )
	    if( (r_emit_ins = emit_il_instances( fp, errfp, pprsf, line, psymtbl, plex, pattr )) >= 0 )
	      r = r_emit_ins;
	  if( (abs(r_emit_abb) + abs(r_emit_ins)) > 0 )
	    fprintf( fp, "\n" );
	}
    }
  }
  return r;
}

int main ( void ) {
  CBI_LEX_SYMTBL S;
  FILE *fp_err = NULL;
  FILE *fp_out = NULL;
  
  fp_err = stdout;
  fp_out = fopen( CBI_STAT_LABELNG_FNAME, "wb" );
  if( !fp_out ) {
    fprintf( fp_out, "failed to open the file: %s.\n", CBI_STAT_LABELNG_FNAME );
    return -1;
  }
  
  {
    BOOL prolog = TRUE;
    int oc_id = -1;
    oc_id = OC801;
    while( oc_id < (int)END_OF_OCs ) {
      assert( (oc_id >= OC801) && (oc_id < END_OF_OCs) );
      assert( fp_err );
      assert( fp_out );
      int n = -1;
      if( ! il_status_geometry_resources[oc_id].csv_fname ) {
	oc_id++;
	continue;
      }
      assert( il_status_geometry_resources[oc_id].csv_fname );
      assert( il_status_geometry_resources[oc_id].oc_id == oc_id );
      
      fprintf( fp_err, "reading the csv file: %s.\n", il_status_geometry_resources[oc_id].csv_fname );
      n = load_cbi_code( il_status_geometry_resources[oc_id].oc_id, il_status_geometry_resources[oc_id].csv_fname );
      assert( n >= 0 );
      fprintf( fp_err, "read %d entries from the csv file: %s.\n", n, il_status_geometry_resources[oc_id].csv_fname );
      {
	PREFX_SUFIX prsf;
	memset( &prsf, 0, sizeof(prsf) );
	if( prolog ) {
	  fprintf( fp_out, "#ifndef CBI_STAT_LABELING\n" );
	  fprintf( fp_out, "#define CBI_STAT_LABELING\n" );
	  fprintf( fp_out, "#endif\n" );
	  fprintf( fp_out, "static CBI_STAT_LABEL cbi_stat_label[] = {\n" );
	  prolog = FALSE;
	}
	{
	  CBI_LEX_SYMTBL_PTR pS = &S;
	  int i = 0;
	  while( cbi_lex_def[i].il_stat_kind != END_OF_CBI_STAT_KIND ) {
	    int j;
	    for( j = 0; (j < CBI_MAX_STAT_BITS) && cbi_stat_syms.cbi_stat_prof[oc_id].codes[j].name[0]; j++ ) {
	      assert( pS );
	      memset( pS, 0, sizeof(S) );
	      snprintf( prsf.prefix.err, CBI_LEX_ERR_PREFX_MAXCHRS, "%s:%d", cbi_stat_syms.cbi_stat_prof[oc_id].codes[j].src.fname, cbi_stat_syms.cbi_stat_prof[oc_id].codes[j].src.line );
	      transduce( fp_out, fp_err, &prsf, (i + 1), pS, &cbi_lex_def[i], &cbi_stat_syms.cbi_stat_prof[oc_id].codes[j] );
	    }
	    i++;
	  }
	  fprintf( fp_err, "\n" );
	}
      }
      oc_id++;
    }
    fprintf( fp_out, "{ _CBI_STAT_KIND_NONSENS, \"\", \"\" }\n" );
    fprintf( fp_out, "};\n" );
    fflush( fp_err );
    if( fp_out ) {
      fflush( fp_out );
      fclose( fp_out );
      fp_out = NULL;
    }
  }
  
  fp_out = fopen( IL_INSTANCES_EMIT_FNAME, "wb" );
  if( !fp_out ) {
    fprintf( fp_out, "failed to open the file: %s.\n", IL_INSTANCES_EMIT_FNAME );
    return -1;
  }
  {
    int cnt = 0;
    CBI_STAT_ATTR_PTR p = NULL;
    p = il_syms_hash.ptop;
    while( p ) {
      assert( p );
      switch( p->decl_gen.nentities ) {
      case 1:
	fprintf( fp_out, "IL_OBJ_INSTANCE_DESC( " );
	break;
      case 2:
	fprintf( fp_out, "IL_OBJ_INSTANCE_DESC2( " );
	break;
      case 3:
	fprintf( fp_out, "IL_OBJ_INSTANCE_DESC3( " );
	break;
      case 4:
	fprintf( fp_out, "IL_OBJ_INSTANCE_DESC4( " );
	break;
      case 5:
	fprintf( fp_out, "IL_OBJ_INSTANCE_DESC5( " );
	break;
      default:
	assert( FALSE );
      }
      assert( p );
      fprintf( fp_out, "%s, ", cnv2str_cbi_stat[p->kind] );
      {
	assert( p );
	assert( p->name );
	p->name[CBI_STAT_NAME_LEN] = 0;
	fprintf( fp_out, "%s, ", p->name );
      }
      {
	assert( p );
	assert( p->decl_gen.plex_il_obj );
	((LEX_CBI_OBJ_PTR)(p->decl_gen.plex_il_obj))->label[CBI_STAT_IDENT_LEN] = 0;
	fprintf( fp_out, "%s, ", ((LEX_CBI_OBJ_PTR)(p->decl_gen.plex_il_obj))->label );
      }
      assert( p );
      assert( p->ident );
      p->ident[CBI_STAT_IDENT_LEN] = 0;
      {
	CBI_STAT_ATTR_PTR q = p;
	while( q ) {
	  assert( q );
	  fprintf( fp_out, "IL_SYMS(%s, ", cnv2str_cbi_stat[q->kind] );
	  fprintf( fp_out, "%s, ", q->ident );
	  fprintf( fp_out, "\"%s\", ", q->ident );
	  fprintf( fp_out, "%d)", cnt );
	  cnt++;
	  q = q->decl_gen.pFamily;
	  if( q )
	    fprintf( fp_out, ", " );
	}
      }
      fprintf( fp_out, " )\n" );
      p = p->decl_gen.pNext;
    }
  }
  return 0;
}

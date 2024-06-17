#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cbi.h"

#define CBI_LEX_PAT_LEN 256
#define CBI_STAT_RAWNAME_LEN 256
typedef struct lex_il_obj {
  CBI_STAT_KIND kind;
  char pat[CBI_LEX_PAT_LEN + 1];
  struct {
    char pat[CBI_LEX_PAT_LEN + 1];
  } exp[5];
  char raw_name[CBI_STAT_RAWNAME_LEN + 1];
} LEX_IL_OBJ, *LEX_IL_OBJ_PTR;

#define CBI_IDENT_MAXLEN 256
#define CBI_MATCHED_CHRS_MAXLEN 256
#define CBI_LEX_SYMTBL_MAX_BUDGETS 256
typedef struct cbi_lex_symbol {
  char ident[CBI_IDENT_MAXLEN + 1];
  int sfx;
  char matched[CBI_MATCHED_CHRS_MAXLEN + 1];
  struct cbi_lex_symbol *pSucc;
} CBI_LEX_SYMBOL, *CBI_LEX_SYMBOL_PTR;
typedef struct cbi_lex_symtbl {
  CBI_LEX_SYMBOL budgets[CBI_LEX_SYMTBL_MAX_BUDGETS + 1];
} CBI_LEX_SYMTBL, *CBI_LEX_SYMTBL_PTR;

static CBI_LEX_SYMBOL_PTR regist_symbol ( CBI_LEX_SYMTBL_PTR psymtbl, CBI_LEX_SYMBOL_PTR ancest, char *id, int id_len ) {
  assert( psymtbl );
  assert( ancest );
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
  assert( (max_match_len > 1) && (max_match_len < CBI_MATCHED_CHRS_MAXLEN) );
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
static char *lex_cbi_stat_name ( CBI_LEX_SYMTBL_PTR psymtbl, char *src, int srclen, LEX_IL_OBJ_PTR plex ) {
  assert( psymtbl );
  assert( src );
  assert( (srclen > -1) && (srclen < CBI_STAT_NAME_LEN) );
  assert( plex );
  char *psrc = src;
  char *ppat = plex->pat;

  assert( psrc );
  assert( ppat );
  while( *ppat && (ppat < (plex->pat + CBI_LEX_PAT_LEN)) ) {
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
	  psrc1 = match_name( matched_chrs, sizeof(matched_chrs), psrc, pSnew );
	  assert( psrc1 );
	  strncpy( pSnew->matched, matched_chrs, CBI_MATCHED_CHRS_MAXLEN );
	  if( psrc < psrc1 ) {
	    psrc = psrc1;
	    ppat++;
	    continue;
	  }
	}
      }
      break;
    } else {
      if( *psrc && (psrc < (src + srclen)) )
	if( *psrc == *ppat ) {
	  psrc++;
	  ppat++;
	  continue;
	}
      break;
    }
  }
  return psrc;
}

static int emit_il_obj_name ( FILE *fp, CBI_LEX_SYMTBL_PTR psymtbl, LEX_IL_OBJ_PTR plex ) {
  assert( fp );
  assert( psymtbl );
  assert( plex );
  int cnt = 0;
  
  return cnt;
}

void enum_il_objs ( FILE *fp, CBI_LEX_SYMTBL_PTR psymtbl, CBI_STAT_ATTR_PTR pattr, LEX_IL_OBJ_PTR plex ) {
  assert( fp );
  assert( psymtbl );
  assert( pattr );
  assert( plex );
  strncpy( plex->raw_name, pattr->name, CBI_STAT_NAME_LEN );
  {
    char src[CBI_STAT_NAME_LEN + 1];
    src[CBI_STAT_NAME_LEN] = 0;
    strncpy( src, plex->raw_name, CBI_STAT_NAME_LEN );
    {
      char *src1;
      src1 = lex_cbi_stat_name( psymtbl, src, strnlen(src, CBI_STAT_NAME_LEN), plex );
      assert( src1 );
      if( src1 >= (src + strnlen(src, CBI_STAT_NAME_LEN)) ) {
	;
      }
    }
  }
  
}

/*
 * grammar: {KIND, match_pattern, {expand_pat_1, expand_pat_2, expand_pat_3, expand_pat_4, expand_pat_5}, raw-stat-name}
 */
LEX_IL_OBJ cbi_lex_def[] = {
  {_ROUTE, "Sxxxy_Sxxxy_R", {{"Sx[1]x[2]x[3]y[1]"}, {"Sx[4]x[5]x[6]y[2]"}}}
};

int main ( void ) {
  int n;
  n = load_CBI_code_tbl ( "./BOTANICAL_GARDEN.csv" );
  printf( "processed %d entries.\n", n );
  
  return 0;
}

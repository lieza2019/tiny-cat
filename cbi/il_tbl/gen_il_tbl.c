#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define CBTC_C
#include "../../cbtc.h"
#undef CBTC_C
#include "../../cbi.h"

#define TRACK_NAME_MAXLEN 16
#define TRACK_BOUNDALIGN_MAXLEN 8

#define ILCOND_IDENT_MAXLEN 256

#define GEN_INDENT( fp, n, m ) {int i; for(i = 0; i < (n); i++){ int b; for(b = 0; b < (m); b++ ) fprintf((fp), " "); }}

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

static BOOL track_prof_SR ( FILE *fp_out, char *ptr_name, const char * psfx_sr ) {
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
    if( pattr )
      if( !strncmp( pattr->ident, idstr, ILCOND_IDENT_MAXLEN ) ) {
	fprintf( fp_out, "TRUE, " );
	fprintf( fp_out, "%s, %s", psfx_sr, idstr );
	r = TRUE;
      }
  }
  if( !r )
    fprintf( fp_out, "FALSE" );
  return r;
}

static CBTC_BLOCK_PTR track_prof_blks ( FILE *fp_out, char *ptr_name ) {
  assert( fp_out );
  assert( ptr_name );
  CBTC_BLOCK_PTR phead = NULL;
  
  CBI_STAT_ATTR_PTR pattr = NULL;
  char idstr[ILCOND_IDENT_MAXLEN + 1];
  strncpy( idstr, ptr_name, ILCOND_IDENT_MAXLEN );
  strcat( idstr, "_TR" );
  pattr = conslt_cbi_code_tbl( idstr );
  if( pattr ) {
    CBTC_BLOCK_PTR ptail = NULL;
    int i = 0;
    while( block_state[i].virt_block_name != END_OF_CBTC_BLOCKs ) {
      if( block_state[i].belonging_tr.track == pattr->id ) {
	if( phead ) {
	  assert( ptail );
	  ptail->belonging_tr.pNext = &block_state[i];	    
	} else {
	  assert( !ptail );
	  phead = &block_state[i];
	}
	block_state[i].belonging_tr.pNext = NULL;
	ptail = &block_state[i];
      }
      i++;
    }
    assert( block_state[i].block_name == 0 );
  }
  return phead;
}

static void emit_track_prof ( FILE *fp_out, char *ptr_name, char *pbounds ) {  
  assert( ptr_name );
  assert( pbounds );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  fprintf( fp_out, "{ _TRACK, " );
  fprintf( fp_out, "\"%s_TR\", ", ptr_name );
  fprintf( fp_out, "%s_TR, ", ptr_name );
  // cbtc
#if 0 // *****
  fprintf( fp_out, "{}, " );
#else
  fprintf( fp_out, "{" );
  {
    int cnt = 0;
    {
      CBTC_BLOCK_PTR pblk_prof = NULL;
      pblk_prof = track_prof_blks( fp_out, ptr_name );
      while( pblk_prof ) {
	if( cnt > 0 )
	  fprintf( fp_out, ", " );
	fprintf( fp_out, "%s", pblk_prof->virt_blkname_str );
	pblk_prof = pblk_prof->belonging_tr.pNext;
	cnt++;
      }   
    }
    fprintf( fp_out, "%d", cnt );
    if( cnt > 0 ) {
      fprintf( fp_out, "{" );
      ;
      fprintf( fp_out, "}, " );
    }
  }
  fprintf( fp_out, "}, " );
#endif
  // lock
  fprintf( fp_out, "{" );
  // TLSR / TRSR
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_TLSR" ); // TLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_TRSR" ); // TRSR
  fprintf( fp_out, "}, " );
  // sTLSR / sTRSR
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_sTLSR" ); // sTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_sTRSR" ); // sTRSR
  fprintf( fp_out, "}, " );
  // eTLSR / eTRSR
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_eTLSR" ); // eTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_eTRSR" ); // eTRSR
  fprintf( fp_out, "}, " );
  // kTLSR / kTRSR
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_kTLSR" ); // kTLSR
  fprintf( fp_out, "}, " );
  fprintf( fp_out, "{" );
  track_prof_SR( fp_out, ptr_name, "_kTRSR" ); // kTRSR
  fprintf( fp_out, "}" );
  fprintf( fp_out, "}" );
  
  fprintf( fp_out, "},\n" );
}

static int emit_track_dataset ( FILE *fp_out, FILE *fp_src ) {
  assert( fp_out );
  assert( fp_src );
  int seq;
  char tr_name[TRACK_NAME_MAXLEN];
  int ce_id;
  int sc_id;
  char bounds[TRACK_BOUNDALIGN_MAXLEN];
  
  int cnt = 0;
  assert( !ferror( fp_out ) );
  assert( !ferror( fp_src ) );
  while( !feof(fp_src) ) {
    int n = -1;
#if 0 // *****
    n = fscanf( fp_src, "%d,%[^,],%d,%d,%[^,]", &seq, tr_name, &ce_id, &sc_id, bounds );
#else
    n = fscanf( fp_src, "%d,%[^,],%d,%d,%s", &seq, tr_name, &ce_id, &sc_id, bounds );
#endif
    if( n < 5 )
      skip_chr( fp_src );
    else {
      char Tr_name[TRACK_NAME_MAXLEN] = "T";
      printf( "(seq, tr_name, ce_id, sc_id, bound): (%d, %s, %d, %d, %s)\n", seq, tr_name, ce_id, sc_id, bounds );
      strcat( &Tr_name[1], tr_name );
      GEN_INDENT( fp_out, 1, 2 );
      emit_track_prof( fp_out, Tr_name, bounds );
      if( ferror( fp_out ) )
	break;
      else
	cnt++;
    }
    if( ferror( fp_src ) )
      break;
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

#define ERR_FAILED_OPEN_MEMMAP 1
static int init_gen_il_dataset ( void ) {
  int r = -1;
  
  int n = -1;
  n = load_cbi_code( OC801, "../memmap/BOTANICAL_GARDEN.csv" );
  if( n > 0 ) {
    n = load_cbi_code( OC802, "../memmap/JASOLA_VIHAR.csv" );
    if( n > 0 )
      r = 0;
    else
      r = ERR_FAILED_OPEN_MEMMAP;
  } else
    r = ERR_FAILED_OPEN_MEMMAP;
  return r;
}

#define ERR_FAILED_OPEN_IL_TBL_TRACKS 2
static int gen_track_dataset ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  int r = ERR_FAILED_OPEN_IL_TBL_TRACKS;
  FILE *fp_src = NULL;
  
  emit_track_dataset_prolog( fp_out );
  fp_src = fopen( "BCGN_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {
      r = emit_track_dataset( fp_out, fp_src );
    }
  }
  r = ERR_FAILED_OPEN_IL_TBL_TRACKS;
  fp_src = fopen( "JLA_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {
      r = emit_track_dataset( fp_out, fp_src );
    }
  }
  emit_track_dataset_epilog( fp_out );
  return r;
}

int main ( void ) {
  FILE *fp_out = NULL;
  int r = -1;
  
  init_gen_il_dataset();
  fp_out = fopen( "interlock_dataset0.h", "w" );
  if( fp_out ) {
    if( !ferror( fp_out ) ) {
      r = gen_track_dataset( fp_out );
    }
  }
  return r;
}

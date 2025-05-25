#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

static BOOL track_prof_TLSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_TLSR" );
}
static BOOL track_prof_TRSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_TRSR" );
}
static BOOL track_prof_sTLSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_sTLSR" );
}
static BOOL track_prof_sTRSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_sTRSR" );
}
static BOOL track_prof_eTLSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_eTLSR" );
}
static BOOL track_prof_eTRSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_eTRSR" );
}
static BOOL track_prof_kTLSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_kTLSR" );
}
static BOOL track_prof_kTRSR ( FILE *fp_out, char *ptr_name ) {
  assert( ptr_name );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  return track_prof_SR( fp_out, ptr_name, "_kTRSR" );
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
  fprintf( fp_out, "{}, " );  
  // lock
  {
    fprintf( fp_out, "{" );
    // TLSR / TRSR
    fprintf( fp_out, "{" );
    track_prof_TLSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    fprintf( fp_out, "{" );
    track_prof_TRSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    // sTLSR / sTRSR
    fprintf( fp_out, "{" );
    track_prof_sTLSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    fprintf( fp_out, "{" );
    track_prof_sTRSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    // eTLSR / eTRSR
    fprintf( fp_out, "{" );
    track_prof_eTLSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    fprintf( fp_out, "{" );
    track_prof_eTRSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    // kTLSR / kTRSR
    fprintf( fp_out, "{" );
    track_prof_kTLSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );
    fprintf( fp_out, "{" );
    track_prof_kTRSR( fp_out, ptr_name );
    fprintf( fp_out, "}, " );    
    fprintf( fp_out, "}" );
  }
  fprintf( fp_out, "},\n" );
}

static int gen_track_dataset ( FILE *fp_out, FILE *fp_src ) {
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
    n = fscanf( fp_src, "%d,%[^,],%d,%d,%[^,]", &seq, tr_name, &ce_id, &sc_id, bounds );
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

int main ( void ) {
  FILE *fp_src = NULL;
  int r = -1;
  
  int n = -1;
  n = load_cbi_code( OC801, "../memmap/JASOLA_VIHAR.csv" );
  
  fp_src = fopen( "JLA_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {
      FILE *fp_out = NULL;
      fp_out = fopen( "interlock_dataset.h", "w" );
      emit_track_dataset_prolog( fp_out );
      r = gen_track_dataset( fp_out, fp_src );
      emit_track_dataset_epilog( fp_out );
    }
  }
  return r;
}

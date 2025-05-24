#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../cbi.h"

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

static void emit_track_data_prof ( FILE *fp_out, char *ptr_name, char *pbound ) {  
  assert( ptr_name );
  assert( pbound );
  assert( fp_out );
  assert( !ferror( fp_out ) );
  
  fprintf( fp_out, "{ _TRACK, " );
  fprintf( fp_out, "\"%s_TR\", ", ptr_name );
  fprintf( fp_out, "%s_TR, ", ptr_name );
  // cbtc
  fprintf( fp_out, "{}, " );  
  // lock
  fprintf( fp_out, "{" );
  fprintf( fp_out, "{}, " ); // TLSR / TRSR
  fprintf( fp_out, "{}, " ); // sTLSR / sTRSR
  fprintf( fp_out, "{}, " ); // eTLSR / eTRSR
  fprintf( fp_out, "{}" ); // kTLSR / kTRSR
  fprintf( fp_out, "}" );
  
  fprintf( fp_out, "},\n" );
}

static int gen_track_dataset ( FILE *fp_out, FILE *fp_src ) {
  assert( fp_out );
  assert( fp_src );
  int seq;
  char tr_name[16];
  int ce_id;
  int sc_id;
  char bound[8];  
  
  int cnt = 0;
  assert( !ferror( fp_out ) );
  assert( !ferror( fp_src ) );
  while( !feof(fp_src) ) {
    int n = -1;
    n = fscanf( fp_src, "%d,%[^,],%d,%d,%[^,]", &seq, tr_name, &ce_id, &sc_id, bound );
    if( n < 5 )
      skip_chr( fp_src );
    else {
      printf( "(seq, tr_name, ce_id, sc_id, bound): (%d, %s, %d, %d, %s)\n", seq, tr_name, ce_id, sc_id, bound );
      GEN_INDENT( fp_out, 1, 2 );
      emit_track_data_prof( fp_out, tr_name, bound );
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

static void emit_track_dataset_def_prolog ( FILE *fp_out ) {
  assert( fp_out );
  assert( !ferror( fp_out ) );
  fprintf( fp_out, "#ifdef TRACK_ATTRIB_DEFINITION\n" );
  fprintf( fp_out, "#ifdef INTERLOCK_C\n" );
  fprintf( fp_out, "TRACK track_dataset_def[] = {\n" );
}
static void emit_track_dataset_def_epilog ( FILE *fp_out ) {
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
  fp_src = fopen( "JLA_TRACK.csv", "r" );
  if( fp_src ) {
    if( !ferror( fp_src ) ) {
      FILE *fp_out = NULL;
      fp_out = fopen( "interlock_dataset.h", "w" );
      emit_track_dataset_def_prolog( fp_out );
      r = gen_track_dataset( fp_out, fp_src );
      emit_track_dataset_def_epilog( fp_out );
    }
  }
  return r;
}

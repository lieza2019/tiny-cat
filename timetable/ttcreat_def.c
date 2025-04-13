#include "ttcreat.h"

PLTB_ID ttc_cnv2_pltb_id ( const char *pltb_str ) {
  assert( pltb_str );
  PLTB_ID r = END_OF_PLTB_ID;
  
  const int cmpmax = 8;
  if( ! strncmp( pltb_str, "PL1", cmpmax ) )
    r = PL1;
  else if( ! strncmp( pltb_str, "PL2", cmpmax ) )
    r = PL2;
  else if( ! strncmp( pltb_str, "PL3", cmpmax ) )
    r = PL3;
  else if( ! strncmp( pltb_str, "TB1", cmpmax ) )
    r = TB1;
  else if( ! strncmp( pltb_str, "TB2", cmpmax ) )
    r = TB2;
  else if( ! strncmp( pltb_str, "TB3", cmpmax ) )
    r = TB3;
  else if( ! strncmp( pltb_str, "TB4", cmpmax ) )
    r = TB4;
  else
    assert( FALSE );
  
  return r;
}

ST_ID ttc_cnv2_st_id ( const char *st_str ) {
  assert( st_str );
  ST_ID r = END_OF_ST_ID;
  
  const int cmpmax = 8;
  if( ! strncmp( st_str, "JLA", cmpmax ) )
    r = JLA;
  else if( ! strncmp( st_str, "KIKJ", cmpmax ) )
    r = KIKJ;
  else if( ! strncmp( st_str, "OKBS", cmpmax ) )
    r = OKBS;
  else if( ! strncmp( st_str, "BTGD", cmpmax ) )
    r = BTGD;
  else
    assert( FALSE );
  
  return r;
}

STOPPING_POINT_CODE ttc_cnv2_sp_code ( const char *sp_str ) {
  assert( sp_str );
  STOPPING_POINT_CODE r = END_OF_SPs;
  
  const int cmpmax = 16;
  if( ! strncmp( sp_str, "SP_NONSENS", cmpmax ) )
    r = SP_NONSENS;
  else if( ! strncmp( sp_str, "SP_73", cmpmax ) ) // JLA_PL1
    r = SP_73;
  else if( ! strncmp( sp_str, "SP_74", cmpmax ) ) // JLA_PL2
    r = SP_74;
  else if( ! strncmp( sp_str, "SP_75", cmpmax ) ) // JLA_PL3
    r = SP_75;
  else if( ! strncmp( sp_str, "SP_D4", cmpmax ) ) // JLA_TB4
    r = SP_D4;
  else if( ! strncmp( sp_str, "SP_D9", cmpmax ) ) // JLA_TB3
    r = SP_D9;
  else if( ! strncmp( sp_str, "SP_76", cmpmax ) ) // KIKJ_PL2
    r = SP_76;
  else if( ! strncmp( sp_str, "SP_77", cmpmax ) ) // KIKJ_PL1
    r = SP_77;
  else if( ! strncmp( sp_str, "SP_78", cmpmax ) ) // OKBS_PL2
    r = SP_78;
  else if( ! strncmp( sp_str, "SP_79", cmpmax ) ) // OKBS_PL1
    r = SP_79;
  else if( ! strncmp( sp_str, "SP_80", cmpmax ) ) // BTGD_PL2
    r = SP_80;
  else if( ! strncmp( sp_str, "SP_81", cmpmax ) ) // BTGD_PL1
    r = SP_81;
  else if( ! strncmp( sp_str, "SP_D5", cmpmax ) ) // BTGD_TB1
    r = SP_D5;
  else if( ! strncmp( sp_str, "SP_D0", cmpmax ) ) // BTGD_TB2
    r = SP_D0;
  else
    assert( FALSE );
  
  return r;
}

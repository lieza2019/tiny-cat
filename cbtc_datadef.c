#include <string.h>b
#include "generic.h"
#include "misc.h"

#include "cbtc.h"
#include "interlock.h"
#include "sparcs.h"

SP_PLTB sp_pltb[] = {
  {SP_73, JLA, PL1}, // JLA_PL1
  {SP_74, JLA, PL2}, // JLA_PL2
  {SP_75, JLA, PL3}, // JLA_PL3
  {SP_D4, JLA, TB4}, // JLA_TB4
  {SP_D9, JLA, TB3}, // JLA_TB3
  {SP_76, KIKJ, PL2}, // KIKJ_PL2
  {SP_77, KIKJ, PL1}, // KIKJ_PL1
  {SP_78, OKBS, PL2}, // OKBS_PL2
  {SP_79, OKBS, PL1}, // OKBS_PL1
  {SP_80, BTGD, PL2}, // BTGD_PL2
  {SP_81, BTGD, PL1}, // BTGD_PL1
  {SP_D5, BTGD, TB1}, // BTGD_TB1
  {SP_D0, BTGD, TB2}, // BTGD_TB2
  {SP_D3, BTGD, TB3}, // BTGD_TB3
};

static const char *spcode_2str[] = {
  "SP_NONSENS",
  "SP_73", // JLA_PL1
  "SP_74", // JLA_PL2
  "SP_75", // JLA_PL3
  "SP_D4", // JLA_TB4
  "SP_D9", // JLA_TB3
  "SP_76", // KIKJ_PL2
  "SP_77", // KIKJ_PL1
  "SP_78", // OKBS_PL2
  "SP_79", // OKBS_PL1
  "SP_80", // BTGD_PL2
  "SP_81", // BTGD_PL1
  "SP_D5", // BTGD_TB1
  "SP_D0", // BTGD_TB2
  "SP_D3", // BTGD_TB3
  NULL
};
const char *cnv2str_sp_code ( STOPPING_POINT_CODE sp_code ) {
  return cnv2str_lkup( spcode_2str, sp_code );
}

STOPPING_POINT_CODE str2_sp_code ( const char *sp_str ) {
  assert( sp_str );
  STOPPING_POINT_CODE r = END_OF_SPs;
  
  const int cmpmax = 16;
  if( ! strncmp( sp_str, "SP_73", cmpmax ) ) // JLA_PL1
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
  else if( ! strncmp( sp_str, "SP_D3", cmpmax ) ) // BTGD_TB3
    r = SP_D3;
  else
    r = SP_NONSENS;
  
  return r;
}

static const char *pltb_2str[] = {
  "PL_UNKNOWN", // see below line.
  "PL1", // PL1 = 1
  "PL2",
  "PL3",
  "TB1",
  "TB2",
  "TB3",
  "TB4",
  NULL
};
const char *cnv2str_pltb_id ( PLTB_ID pltb_id ) {
  return cnv2str_lkup( pltb_2str, pltb_id );
}

PLTB_ID str2_pltb_id ( const char *pltb_str ) {
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
    r = PL_UNKNOWN;
  
  return r;
}

static const char *stid_2str[] = {
  "ST_UNKNOWN", // see below line.
  "JLA", // JLA = 1
  "KIKJ",
  "OKBS",
  "BTGD",
  NULL
};
const char *cnv2str_st_id ( ST_ID st_id ) {
  return cnv2str_lkup( stid_2str, st_id );
}

ST_ID str2_st_id ( const char *st_str ) {
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
    r = ST_UNKNOWN;
  
  return r;
}

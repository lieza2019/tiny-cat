typedef enum {
  PL1 = 1,
  PL2,
  PL3,
  TB1,
  TB2,
  TB3,
  TB4,
  END_OF_PLTB_ID
} PLTB_ID;
extern const char *cnv2str_pltb_id ( PLTB_ID pltb_id );
extern PLTB_ID str2_pltb_id ( const char *pltb_str );

typedef enum {
  JLA = 1,
  KIKJ,
  OKBS,
  BTGD,
  END_OF_ST_ID
} ST_ID;
const char *cnv2str_st_id ( ST_ID st_id );
extern ST_ID str2_st_id ( const char *st_str );

const char *cnv2str_sp_code ( STOPPING_POINT_CODE sp_code );
extern STOPPING_POINT_CODE str2_sp_code ( const char *sp_str );

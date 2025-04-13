typedef enum {
  JLA = 1,
  KIKJ,
  OKBS,
  BTGD,
  END_OF_ST_ID
} ST_ID;

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

extern PLTB_ID ttc_cnv2_pltb_id ( const char *pltb_str );
extern ST_ID ttc_cnv2_st_id ( const char *st_str );
extern STOPPING_POINT_CODE ttc_cnv2_sp_code ( const char *sp_str );

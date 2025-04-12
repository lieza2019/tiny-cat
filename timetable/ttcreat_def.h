typedef enum {
  JLA,
  KIKJ,
  OKBS,
  BTGD
} ST_ID;

typedef enum {
  PL1,
  PL2,
  PL3,
  TB1,
  TB2,
  TB3,
  TB4
} PLTB_ID;

struct st_pltb  {
  ST_ID st;
  PLTB_ID pltb;
};

typedef struct sp_orgdst_pair {
  STOPPING_POINT_CODE sp_org;
  STOPPING_POINT_CODE sp_dst;
} SP_ORGDST_PAIR;

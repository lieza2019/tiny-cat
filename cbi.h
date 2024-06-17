#include "generic.h"
#include "misc.h"
#include "network.h"

#define CBI_MAX_STAT_BITS 65536
#define CBI_STAT_BITS_LEXBUF_SIZE 256

typedef enum _cbi_stat_group {
  CBI_STAT_i1 = 1,
  CBI_STAT_i2,
  CBI_STAT_i3, 
  CBI_STAT_i4,
  CBI_STAT_i5,
  CBI_STAT_i6,
  CBI_STAT_Li1,
  CBI_STAT_Li2,
  CBI_STAT_Li3,
  CBI_STAT_Li4,
  CBI_STAT_Si1,
  CBI_STAT_Si2,
  CBI_STAT_Si3,
  CBI_STAT_Si4,
  CBI_STAT_Si5,
  CBI_STAT_H,
  CBI_STAT_P,
  CBI_STAT_o1,
  CBI_STAT_o2,
  CBI_STAT_o3,
  CBI_STAT_o4,
  CBI_STAT_o5,
  CBI_STAT_o6,
  CBI_STAT_Lo,
  CBI_STAT_So,
  CBI_STAT_SRAM,
  CBI_STAT_M,
  CBI_STAT_A
} CBI_STAT_GROUP;

typedef enum cbi_stat_bit_mask {
  CBI_STAT_BIT_0 = 1,
  CBI_STAT_BIT_1 = 2,
  CBI_STAT_BIT_2 = 4,
  CBI_STAT_BIT_3 = 8,
  CBI_STAT_BIT_4 = 16,
  CBI_STAT_BIT_5 = 32,
  CBI_STAT_BIT_6 = 64,
  CBI_STAT_BIT_7 = 128,
  END_OF_CBI_STAT_BIT_MASK
} CBI_STAT_BIT_MASK;

#define CBI_STAT_IDENT_LEN 32
#define CBI_STAT_NAME_LEN 32
typedef struct cbi_stat_attr {
  char ident[CBI_STAT_IDENT_LEN + 1];
  char name[CBI_STAT_NAME_LEN + 1];
  CBI_STAT_GROUP group;
  struct {
    int raw;
    int bytes;
    int bits;
    CBI_STAT_BIT_MASK mask;
  } disp;
  struct cbi_stat_attr *pNext;
} CBI_STAT_ATTR, *CBI_STAT_ATTR_PTR;

extern CBI_STAT_ATTR cbi_stat_prof[CBI_MAX_STAT_BITS];

extern void dump_cbi_stat_prof ( void );
extern int load_CBI_code_tbl ( const char *fname );

typedef enum cbi_stat_kind {
  _ROUTE,
  END_OF_CBI_STAT_KIND
} CBI_STAT_KIND;

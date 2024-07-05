#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

CBI_STAT_INFO cbi_stat_ATS2OC[END_OF_ATS2OC] = {
  {{"OC801", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
   {{172, 21, 51, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {OC_LNN_801_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_801},
   {}
  },
  {{"", "OC802", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {172, 21, 52, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, OC_LNN_802_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_802},
   {}
  },
  {{"", "", "OC803", "", "", "", "", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 53, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, OC_LNN_803_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_803},
   {}
  },
  {{"", "", "", "OC804", "", "", "", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 54, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, OC_LNN_804_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_804},
   {}
  },
  {{"", "", "", "", "OC805", "", "", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 55, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, OC_LNN_805_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_805},
   {}
  },
  {{"", "", "", "", "", "OC806", "", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 56, 1}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, OC_LNN_806_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_806},
   {}
  },
  {{"", "", "", "", "", "", "OC807", "", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 57, 1}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, OC_LNN_807_SYS1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_807},
   {}
  },
  {{"", "", "", "", "", "", "", "OC808", "", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 58, 1},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, OC_LNN_808_SYS1, 0, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_808},
   {}
  },
  {{"", "", "", "", "", "", "", "", "OC809", "", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {172, 21, 59, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_809_SYS1, 0, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_809},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "OC810", "", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},{0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {172, 21, 60, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_810_SYS1, 0, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_810},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "OC811", "", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 61, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_811_SYS1, 0, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_811},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "", "OC812", "", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 62, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_812_SYS1, 0, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_812},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "", "", "OC813", "", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} ,{0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 63, 1}, {0, 0, 0, 0} ,{0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_813_SYS1, 0, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_813},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "", "", "", "OC814", "", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 64, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_814_SYS1, 0, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_814},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "", "", "", "", "OC815", ""},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 65, 1}, {0, 0, 0, 0}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_815_SYS1, 0},
   {UDP_BCAST_RECV_PORT_ATS2OC_815},
   {}
  },
  {{"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "OC816"},
   {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {172, 21, 66, 1}},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OC_LNN_816_SYS1},
   {UDP_BCAST_RECV_PORT_ATS2OC_816},
   {}
  }
};

CBI_STAT_INFO cbi_stat_OC2ATS[END_OF_OC2ATS] = {
  {{"OC801", "OC802", "OC803", "OC804", "OC805", "OC806", "OC807", "OC808",
     "OC809", "OC810", "OC811", "OC812", "OC813", "OC814", "OC815", "OC816"},
   {{172, 21, 51, 1}, {172, 21, 52, 1}, {172, 21, 53, 1}, {172, 21, 54, 1}, {172, 21, 55, 1}, {172, 21, 56, 1}, {172, 21, 57, 1}, {172, 21, 58, 1},
    {172, 21, 59, 1}, {172, 21, 60, 1}, {172, 21, 61, 1}, {172, 21, 62, 1}, {172, 21, 63, 1}, {172, 21, 64, 1}, {172, 21, 65, 1}, {172, 21, 66, 1}},
   {OC_LNN_801_SYS1, OC_LNN_802_SYS1, OC_LNN_803_SYS1, OC_LNN_804_SYS1, OC_LNN_805_SYS1, OC_LNN_806_SYS1, OC_LNN_807_SYS1, OC_LNN_808_SYS1,
    OC_LNN_809_SYS1, OC_LNN_810_SYS1, OC_LNN_811_SYS1, OC_LNN_812_SYS1, OC_LNN_813_SYS1, OC_LNN_814_SYS1, OC_LNN_815_SYS1, OC_LNN_816_SYS1},
   {},
   {UDP_BCAST_RECV_PORT_OC2ATS1_STAT}
  },
  {{"OC801", "OC802", "OC803", "OC804", "OC805", "OC806", "OC807", "OC808",
     "OC809", "OC810", "OC811", "OC812", "OC813", "OC814", "OC815", "OC816"},
   {{172, 21, 51, 1}, {172, 21, 52, 1}, {172, 21, 53, 1}, {172, 21, 54, 1}, {172, 21, 55, 1}, {172, 21, 56, 1}, {172, 21, 57, 1}, {172, 21, 58, 1},
    {172, 21, 59, 1}, {172, 21, 60, 1}, {172, 21, 61, 1}, {172, 21, 62, 1}, {172, 21, 63, 1}, {172, 21, 64, 1}, {172, 21, 65, 1}, {172, 21, 66, 1}},
   {OC_LNN_801_SYS1, OC_LNN_802_SYS1, OC_LNN_803_SYS1, OC_LNN_804_SYS1, OC_LNN_805_SYS1, OC_LNN_806_SYS1, OC_LNN_807_SYS1, OC_LNN_808_SYS1,
    OC_LNN_809_SYS1, OC_LNN_810_SYS1, OC_LNN_811_SYS1, OC_LNN_812_SYS1, OC_LNN_813_SYS1, OC_LNN_814_SYS1, OC_LNN_815_SYS1, OC_LNN_816_SYS1},
   {},
   {UDP_BCAST_RECV_PORT_OC2ATS2_STAT}
  },
  {{"OC801", "OC802", "OC803", "OC804", "OC805", "OC806", "OC807", "OC808",
     "OC809", "OC810", "OC811", "OC812", "OC813", "OC814", "OC815", "OC816"},
   {{172, 21, 51, 1}, {172, 21, 52, 1}, {172, 21, 53, 1}, {172, 21, 54, 1}, {172, 21, 55, 1}, {172, 21, 56, 1}, {172, 21, 57, 1}, {172, 21, 58, 1},
    {172, 21, 59, 1}, {172, 21, 60, 1}, {172, 21, 61, 1}, {172, 21, 62, 1}, {172, 21, 63, 1}, {172, 21, 64, 1}, {172, 21, 65, 1}, {172, 21, 66, 1}},
   {OC_LNN_801_SYS1, OC_LNN_802_SYS1, OC_LNN_803_SYS1, OC_LNN_804_SYS1, OC_LNN_805_SYS1, OC_LNN_806_SYS1, OC_LNN_807_SYS1, OC_LNN_808_SYS1,
    OC_LNN_809_SYS1, OC_LNN_810_SYS1, OC_LNN_811_SYS1, OC_LNN_812_SYS1, OC_LNN_813_SYS1, OC_LNN_814_SYS1, OC_LNN_815_SYS1, OC_LNN_816_SYS1},
   {},
   {UDP_BCAST_RECV_PORT_OC2ATS3_STAT}
  }
};

RECV_BUF_CBI_STAT cbi_stat_info[END_OF_OCs];

#include "./cbi/cbi_stat_label.h"
#ifndef CBI_STAT_LABELING
static CBI_STAT_LABEL cbi_stat_labeling[] = {
  { _CBI_KIND_NONSENS, "", "" }
};
#endif
#ifdef CBI_STAT_LABELING
#undef CBI_STAT_LABELING
#endif

char *cnv2str_cbi_stat_kind[] = {
#define CBI_STAT_KIND_DESC(enum, name) name,
#include "./cbi/cbi_stat_kind.def"
#undef CBI_STAT_KIND_DESC
  NULL
};

CBI_STAT_ATTR cbi_stat_prof[END_OF_OCs][CBI_MAX_STAT_BITS];
static int frontier[END_OF_OCs];

static CBI_STAT_ATTR_PTR cbi_stat_hash_budgets[CBI_STAT_HASH_BUDGETS_NUM];

static int hash_key ( const int budgets_num, char *ident ) {
  assert( budgets_num > 0 );
  assert( ident );
  const int n = 5;
  assert( (n > 0) && (n <= CBI_STAT_IDENT_LEN) );
  
  int h;
  h = 0;
  {
    int i;
    for( i = 0; (i < n) && ident[i]; i++ ) {
      h = 13 * h + ident[i];
      h = (h < 0) ? ((h * -1) % budgets_num) : h;
    }
  }
  return ( h % budgets_num );
}


static BOOL chk_uniq_in_budget ( CBI_STAT_ATTR_PTR pE ) {
  assert( pE );
  BOOL found = FALSE;
  char id[CBI_STAT_IDENT_LEN + 1];
  
  id[CBI_STAT_IDENT_LEN] = 0;
  strncpy( id, pE->ident, CBI_STAT_IDENT_LEN );
  assert( ! strncmp(id, pE->ident, CBI_STAT_IDENT_LEN) );
  {
    CBI_STAT_ATTR_PTR p = pE->pNext_hash;
    while( p ) {
      if( !strncmp(p->ident, id, CBI_STAT_IDENT_LEN) ) {
	found = TRUE;
	break;
      }
      p = p->pNext_hash;
    }
    assert( !found );
  }
  return( !found );
}
static CBI_STAT_ATTR_PTR *walk_hash ( CBI_STAT_ATTR_PTR *ppB, char *ident ) {
  assert( ppB );
  assert( ident );
  CBI_STAT_ATTR_PTR *pp = ppB;
  
  assert( pp );
  while( *pp ) {
    if( !strncmp((*pp)->ident, ident, CBI_STAT_IDENT_LEN) ) {
#ifdef CHK_STRICT_CONSISTENCY
      assert( chk_uniq_in_budget(*pp) );
#endif // CHK_STRICT_CONSISTENCY
      break;
    }
    pp = &(*pp)->pNext_hash;
  }
  return pp;
}

static CBI_STAT_ATTR_PTR regist_hash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, CBI_STAT_ATTR_PTR pE ) {
  assert( budgets );
  assert( budgets_num > 0 );
  assert( pE );
  CBI_STAT_ATTR_PTR r = NULL;
  CBI_STAT_ATTR_PTR *ppB = NULL;
  
  {
    int h = -1;
    h = hash_key( budgets_num, pE->ident );
    assert( (h > -1) && (h < budgets_num) );
    ppB = &budgets[h];
  }
  assert( ppB );

  r = pE;
  pE->pNext_hash = NULL;
  {
    CBI_STAT_ATTR_PTR *pp;
    pp = walk_hash( ppB, pE->ident );
    if( *pp ) {
     //assert( FALSE );
      errorF( "redefinition of cbi condition: %s\n", (*pp)->ident );
      pE->pNext_hash = (*pp)->pNext_hash;
      r = *pp;
    }
    assert( pp );
    *pp = pE;
  }
  assert( r );
  return r;
}
static CBI_STAT_ATTR_PTR regist_hash_local ( CBI_STAT_ATTR_PTR pE ) {
  assert( pE );
  return regist_hash( cbi_stat_hash_budgets, CBI_STAT_HASH_BUDGETS_NUM, pE );
}

CBI_STAT_ATTR_PTR cbi_stat_regist ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, CBI_STAT_ATTR_PTR pE ) {
  assert( budgets );
  assert( budgets_num );
  assert( pE );
  CBI_STAT_ATTR_PTR r = NULL;
  
  r = regist_hash( budgets, budgets_num, pE );
  assert( r );
  return r;
}

static CBI_STAT_ATTR_PTR re_hash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, char *ident, char *ident_new ) {
  assert( budgets );
  assert( budgets_num > 0 );
  assert( ident );
  assert( ident_new );
  CBI_STAT_ATTR_PTR pE = NULL;
  CBI_STAT_ATTR_PTR *ppB = NULL;
  
  {
    int h = -1;
    h = hash_key( budgets_num, ident );
    assert( (h > -1) && (h < budgets_num) );
    ppB = &budgets[h];
  }
  assert( ppB );
  
  {
    CBI_STAT_ATTR_PTR *pp = NULL;
    pp = walk_hash( ppB, ident );
    assert( pp );
    if( *pp ) {
      pE = *pp;
      *pp = pE->pNext_hash;
      assert( pE );
      strncpy( pE->ident, ident_new, CBI_STAT_IDENT_LEN );
      pE->ident[CBI_STAT_IDENT_LEN] = 0;
      pE->pNext_hash = NULL;
      regist_hash( budgets, budgets_num, pE );
    }
  }
  return pE;
}
static CBI_STAT_ATTR_PTR re_hash_local ( char *ident, char *ident_new ) {
  assert( ident );
  assert( ident_new );
  return re_hash( cbi_stat_hash_budgets, CBI_STAT_HASH_BUDGETS_NUM, ident, ident_new );
}

CBI_STAT_ATTR_PTR cbi_stat_rehash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, char *ident, char *ident_new ) {
  assert( budgets );
  assert( budgets_num > 0 );
  assert( ident );
  assert( ident_new );
  return re_hash( budgets, budgets_num, ident, ident_new );
}

static CBI_STAT_ATTR_PTR conslt_hash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, char *ident ) {
  assert( budgets );
  assert( budgets_num > 0 );
  assert( ident );
  CBI_STAT_ATTR_PTR r = NULL;
  
  CBI_STAT_ATTR_PTR *ppB = NULL;
  {
    int h = -1;
    h = hash_key( budgets_num, ident );
    assert( (h > -1) && (h < budgets_num) );
    ppB = &budgets[h];
  }
  assert( ppB );
  
  {
    CBI_STAT_ATTR_PTR *pp = NULL;
    pp = walk_hash( ppB, ident );
    assert( pp );
    r = *pp;
  }
  return r;
}
static CBI_STAT_ATTR_PTR conslt_hash_local ( char *ident ) {
  assert( ident );
  return conslt_hash( cbi_stat_hash_budgets, CBI_STAT_HASH_BUDGETS_NUM, ident );
}

CBI_STAT_ATTR_PTR cbi_stat_idntify ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, char *ident ) {
  assert( budgets );
  assert( budgets_num );
  assert( ident );
  return conslt_hash( budgets, budgets_num, ident );
}

static char *show_cbi_stat_bit_mask ( char *mask_name, int len, CBI_STAT_BIT_MASK mask ) {
  assert( mask_name );
  assert( len > strlen("CBI_STAT_BIT_x") );
  switch( mask ) {
  case CBI_STAT_BIT_0:
    strncpy( mask_name, "CBI_STAT_BIT_0", len );
    break;
  case CBI_STAT_BIT_1:
    strncpy( mask_name, "CBI_STAT_BIT_1", len );
    break;
  case CBI_STAT_BIT_2:
    strncpy( mask_name, "CBI_STAT_BIT_2", len );
    break;
  case CBI_STAT_BIT_3:
    strncpy( mask_name, "CBI_STAT_BIT_3", len );
    break;
  case CBI_STAT_BIT_4:
    strncpy( mask_name, "CBI_STAT_BIT_4", len );
    break;
  case CBI_STAT_BIT_5:
    strncpy( mask_name, "CBI_STAT_BIT_5", len );
    break;
  case CBI_STAT_BIT_6:
    strncpy( mask_name, "CBI_STAT_BIT_6", len );
    break;
  case CBI_STAT_BIT_7:
    strncpy( mask_name, "CBI_STAT_BIT_7", len );
    break;
  case END_OF_CBI_STAT_BIT_MASK:
    /* fall thru */
    default:
      assert( FALSE );
  }
  return mask_name;
}

static CBI_STAT_BIT_MASK cbi_stat_bit_mask_pattern ( int pos ) {
  assert( (pos >= 0) && (pos < 8) );
  CBI_STAT_BIT_MASK mask = END_OF_CBI_STAT_BIT_MASK;
  switch( pos ) {
  case 0:
    mask = CBI_STAT_BIT_7;
    break;
  case 1:
    mask = CBI_STAT_BIT_0;
    break;
  case 2:
    mask = CBI_STAT_BIT_1;
    break;
  case 3:
    mask = CBI_STAT_BIT_2;
    break;
  case 4:
    mask = CBI_STAT_BIT_3;
    break;
  case 5:
    mask = CBI_STAT_BIT_4;
    break;
  case 6:
    mask = CBI_STAT_BIT_5;
    break;
  case 7:
    mask = CBI_STAT_BIT_6;
    break;
  default:
    assert( FALSE );
  }
  assert( mask != END_OF_CBI_STAT_BIT_MASK );
  return mask;
}
typedef enum _cbi_lex_phase {
  CBI_LEX_NAME,
  CBI_LEX_GROUP,
  CBI_LEX_DISP,
  END_OF_CBI_LEX
} CBI_LEX_PHASE;
static BOOL cbi_lex ( char *src, int src_len, char *name, int name_len, int *pgroup, int *pdisp ) {
  assert( src );
  assert( (src_len > 0) && (src_len <= CBI_STAT_BITS_LEXBUF_SIZE) );
  assert( name );
  assert( (name_len > 0) && (name_len <= CBI_STAT_NAME_LEN) );
  assert( pgroup );
  assert( pdisp );
  BOOL r = TRUE;
  CBI_LEX_PHASE ph = CBI_LEX_NAME;
  char *pword = src;
  char *p = src;
  while( (p - src) < src_len ) {
    if( !(*p) ) {
      if( ph == CBI_LEX_DISP ) {
	*pdisp = atoi( pword );
	ph = END_OF_CBI_LEX;
      } else
	break;
    } else if( *p == ',' ) {
      *p = 0;
      switch( ph ) {
      case CBI_LEX_NAME:
	strncpy( name, pword, name_len );
	ph = CBI_LEX_GROUP;
	break;
      case CBI_LEX_GROUP:
	*pgroup = atoi( pword );
	ph = CBI_LEX_DISP;
	break;
      default:
	r = FALSE;
	break;
      }
    } else if( *p == '\n') {
      r = FALSE;
      break;
    } else {
      p++;
      continue;
    }
    pword = ++p;
  }
  if( ph != END_OF_CBI_LEX )
    r = FALSE;
  return r;
}

#if 1  // ***** for debugging.
#define DUP_FNAME "BCGN_dup.txt"
static FILE *fp_out = NULL;
static void dup_CBI_code_tbl ( const char *name, int group, int disp ) { // ***** for debugging.
  if( ! fp_out ) {
    fp_out = fopen( DUP_FNAME, "w" );
    assert( fp_out );
  }
  fprintf( fp_out, "%s,%d,%d\n", name, group, disp );
}
#endif
#define CBI_STAT_MASKNAME_MAXLEN 256
void dump_cbi_stat_prof ( OC_ID oc_id ) {
  int i;
  for( i = 0; i < CBI_MAX_STAT_BITS; i++ ) {
    printf( "name: %s\n", cbi_stat_prof[oc_id][i].name );
    printf( "ident: %s\n", cbi_stat_prof[oc_id][i].ident );
    printf( "disp.raw: %d\n", cbi_stat_prof[oc_id][i].disp.raw );
    printf( "disp.bytes: %d\n", cbi_stat_prof[oc_id][i].disp.bytes );
    printf( "disp.bits: %d\n", cbi_stat_prof[oc_id][i].disp.bits );
    {
      char str[CBI_STAT_MASKNAME_MAXLEN + 1];
      str[CBI_STAT_MASKNAME_MAXLEN] = 0;
      show_cbi_stat_bit_mask( str, CBI_STAT_MASKNAME_MAXLEN, cbi_stat_prof[oc_id][i].disp.mask );
      printf( "disp.mask: %s\n", str );
    }
    printf( "\n" );
  }
}
int load_cbi_code_tbl ( OC_ID oc_id, const char *fname ) {
  assert( fname );
  FILE *fp = NULL;
  
  fp = fopen( fname, "r" );
  if( fp ) {
    int lines = 0;
    int group = -1;
    int disp = -1;
    char name[CBI_STAT_NAME_LEN + 1];
    name[CBI_STAT_NAME_LEN] = 0;
    while( (! feof(fp)) && (frontier[oc_id] < CBI_MAX_STAT_BITS) ) {
      char buf[CBI_STAT_BITS_LEXBUF_SIZE + 1];
      buf[CBI_STAT_BITS_LEXBUF_SIZE] = 0;
      fscanf( fp, "%s\n", buf );
      if( ! cbi_lex( buf, CBI_STAT_BITS_LEXBUF_SIZE, name, CBI_STAT_NAME_LEN, &group, &disp ) ) {
	errorF( "failed lexical analyzing the CBI code-table, in line num: %d.\n,", lines );
	assert( FALSE );
      } else {
	CBI_STAT_ATTR_PTR pA = &cbi_stat_prof[oc_id][frontier[oc_id]];
	assert( pA );
	strncpy( pA->name, name, CBI_STAT_NAME_LEN );
	strncpy( pA->ident, name, CBI_STAT_NAME_LEN );
	pA->oc_id = oc_id;
	pA->kind = _UNKNOWN;
	pA->group = (CBI_STAT_GROUP)group;
	pA->disp.raw = disp;
	{
	  int n = disp / 8;
	  int m = disp % 8;
	  assert( (m >= 0) && (m < 8) );
	  if( m > 0 )
	    pA->disp.bytes = n;
	  else {
	    assert( m == 0 );
	    pA->disp.bytes = ((n > 0) ? (n - 1) : n);
	  }
	  pA->disp.bits = m;
	}
	pA->disp.mask = cbi_stat_bit_mask_pattern( pA->disp.bits );
	frontier[oc_id]++;
      }
      lines++;
      //dup_CBI_code_tbl( name, group, disp ); // ***** for debugging.
    }
    fclose( fp );
    //fclose( fp_out ); // ***** for debugging.
  } else {
    errorF( "failed to open the file: %s.\n", fname );
    assert( FALSE );
  }
  
  {
    int i;
    for( i = 0; i < frontier[oc_id]; i++ )
      regist_hash_local( &cbi_stat_prof[oc_id][i] );
    assert( i == frontier[oc_id] );
  }
  
  return frontier[oc_id];
}

int reveal_cbi_code_tbl ( void ) {
  int cnt = 0;
  int j = 0;
  
  while( cbi_stat_labeling[j].kind != _CBI_KIND_NONSENS ) {
    CBI_STAT_ATTR_PTR pS = NULL;
    pS = conslt_hash_local( cbi_stat_labeling[j].name );
    if( pS ) {
      CBI_STAT_ATTR_PTR pE = NULL;
      pS->kind = cbi_stat_labeling[j].kind;
      pE = re_hash_local( pS->ident, cbi_stat_labeling[j].ident );
      if( pE )
	cnt++;
#ifdef CHK_STRICT_CONSISTENCY
      assert( pE );
      assert( pE == pS );
    } else
      assert( pS );
#else
    }
#endif // CHK_STRICT_CONSISTENCY
    j++;
  }
  return cnt;
}

#if 0
int main ( void ) {
  int cnt = 0;
  int n = -1;
  
  n = load_cbi_code_tbl ( "./cbi/BOTANICAL_GARDEN.csv" );
  assert( (n >= 0) && (n <= CBI_MAX_STAT_BITS) );
  printf( "read %d entries on, from raw csv.\n", n );
  {
    int m = -1;
    m = reveal_cbi_code_tbl();
    assert( m > -1 );
    printf( "revised %d entries.\n", m );
  }
  
  // test correctness on construction of hash-map for cbi state bits.
  {
    int i;
    for( i = 0; i < n; i++ ) {
      CBI_STAT_ATTR_PTR pE = NULL;
      CBI_STAT_LABEL_PTR pL = NULL;
      int j = 0;
      pE = &cbi_stat_prof[oc_id][i];
      while( cbi_stat_labeling[j].kind != _CBI_KIND_NONSENS )  {
	if( ! strncmp(cbi_stat_labeling[j].name, cbi_stat_prof[oc_id][i].name, CBI_STAT_NAME_LEN) ) {
	  pL = &cbi_stat_labeling[j];
	  break;
	}
	j++;
      }
      
      assert( pE );
      {
	char id[CBI_STAT_NAME_LEN + 1];
	id[CBI_STAT_NAME_LEN] = 0;
	if( pL )
	  strncpy( id, pL->ident, CBI_STAT_NAME_LEN );
	else
	  strncpy( id, pE->name, CBI_STAT_NAME_LEN );
	pE = conslt_hash_local( id );
	assert( pE );
	assert( ! strncmp(pE->name, cbi_stat_prof[oc_id][i].name, CBI_STAT_NAME_LEN) );
	cnt++;
      }
    }
  }
  assert( n == cnt );
  
  return 0;
}
#endif

char *cnv2str_il_obj_instances[] = {
#define IL_3t(exp, str, code) str
#define IL_OBJ_INSTANCE_DESC(route, raw_name, exp) exp,
#include "./cbi/il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
#undef IL_3t
  NULL
};

//{ROUTE_CONTROL, Sxxxy, Sxxxy,
struct attr {
  CBI_STAT_KIND kind;
  char ident[CBI_STAT_IDENT_LEN + 1];
  union {
    struct {
      IL_OBJ_INSTANCES src;
      IL_OBJ_INSTANCES dst;
    } route;
  } u;
};

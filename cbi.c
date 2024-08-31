#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "cbi.h"

const char *cnv2str_cbi_stat_kind[] = {
#define CBI_STAT_KIND_DESC(enum, name) name,
#include "./cbi/cbi_stat_kind.def"
#undef CBI_STAT_KIND_DESC
  NULL
};

const CBI_STAT_CSV_FNAMES il_status_geometry_resources[END_OF_OCs + 1] = {
  {OC801, CBI_STAT_CSV_FNAME_BCGN},
  {OC802, CBI_STAT_CSV_FNAME_JLA},
  {OC803, CBI_STAT_CSV_FNAME_IWNA},
  {OC804, CBI_STAT_CSV_FNAME_RKPM},
  {OC805, CBI_STAT_CSV_FNAME_IGDA},
  {OC806, CBI_STAT_CSV_FNAME_JPW},
  {OC807, CBI_STAT_CSV_FNAME_KIKD_OC1},
  {OC808, CBI_STAT_CSV_FNAME_KIKD_OC2},
  {OC809, CBI_STAT_CSV_FNAME_KPEN},
  {OC810, CBI_STAT_CSV_FNAME_PAGI},
  {OC811, CBI_STAT_CSV_FNAME_DPCK},
  {OC812, CBI_STAT_CSV_FNAME_NPPR},
  {OC813, CBI_STAT_CSV_FNAME_MKPR},
  {OC814, CBI_STAT_CSV_FNAME_GAGR},
  {OC815, CBI_STAT_CSV_FNAME_RKAM},
  {OC816, CBI_STAT_CSV_FNAME_MKPD},
  {END_OF_OCs, NULL}
};

const char *CBI_STAT_GROUP_CONV2STR[] = {
  "",  // see below line.
  "CBI_STAT_i1", //CBI_STAT_i1 = 1,
  "CBI_STAT_i2",
  "CBI_STAT_i3",
  "CBI_STAT_i4",
  "CBI_STAT_i5",
  "CBI_STAT_i6",
  "CBI_STAT_Li1",
  "CBI_STAT_Li2",
  "CBI_STAT_Li3",
  "CBI_STAT_Li4",
  "CBI_STAT_Si1",
  "CBI_STAT_Si2",
  "CBI_STAT_Si3",
  "CBI_STAT_Si4",
  "CBI_STAT_Si5",
  "CBI_STAT_H",
  "CBI_STAT_P",
  "CBI_STAT_o1",
  "CBI_STAT_o2",
  "CBI_STAT_o3",
  "CBI_STAT_o4",
  "CBI_STAT_o5",
  "CBI_STAT_o6",
  "CBI_STAT_Lo",
  "CBI_STAT_So",
  "CBI_STAT_SRAM",
  "CBI_STAT_M",
  "CBI_STAT_A"
};

const static OC2ATS_STAT cbi_group2msg[] = {
  -1, // see below line.
  OC2ATS3, // CBI_STAT_i1 = 1
  OC2ATS3, // CBI_STAT_i2
  OC2ATS3, // CBI_STAT_i3
  OC2ATS3, // CBI_STAT_i4
  OC2ATS3, // CBI_STAT_i5
  OC2ATS3, // CBI_STAT_i6
  OC2ATS1, // CBI_STAT_Li1
  OC2ATS1, // CBI_STAT_Li2
  OC2ATS1, // CBI_STAT_Li3
  OC2ATS1, // CBI_STAT_Li4
  OC2ATS1, // CBI_STAT_Si1
  OC2ATS1, // CBI_STAT_Si2
  OC2ATS1, // CBI_STAT_Si3
  OC2ATS1, // CBI_STAT_Si4
  OC2ATS1, // CBI_STAT_Si5
  OC2ATS1, // CBI_STAT_H
  OC2ATS1, // CBI_STAT_P
  OC2ATS3, // CBI_STAT_o1
  OC2ATS3, // CBI_STAT_o2
  OC2ATS3, // CBI_STAT_o3
  OC2ATS3, // CBI_STAT_o4
  OC2ATS3, // CBI_STAT_o5
  OC2ATS3, // CBI_STAT_o6
  OC2ATS2, // CBI_STAT_Lo
  OC2ATS2, // CBI_STAT_So
  OC2ATS2, // CBI_STAT_SRAM
  OC2ATS2, // CBI_STAT_M
  OC2ATS3  // CBI_STAT_A
};

const static int cbi_group2addr[] = {
   -1, // see below line.
   76, // CBI_STAT_i1 = 1, of OC2ATS3
  100, // CBI_STAT_i2 of OC2ATS3
  124, // CBI_STAT_i3 of OC2ATS3
  148, // CBI_STAT_i4 of OC2ATS3
  172, // CBI_STAT_i5 of OC2ATS3
  196, // CBI_STAT_i6 of OC2ATS3
   76, // CBI_STAT_Li1 of OC2ATS1
  272, // CBI_STAT_Li2 of OC2ATS1
  468, // CBI_STAT_Li3 of OC2ATS1
  664, // CBI_STAT_Li4 of OC2ATS1
  860, // CBI_STAT_Si1 of OC2ATS1
  916, // CBI_STAT_Si2 of OC2ATS1
  972, // CBI_STAT_Si3 of OC2ATS1
  1028, // CBI_STAT_Si4 of OC2ATS1
  1084, // CBI_STAT_Si5 of OC2ATS1
  1140, // CBI_STAT_H of OC2ATS1
  1194, // CBI_STAT_P of OC2ATS1
  220, // CBI_STAT_o1 of OC2ATS3
  244, // CBI_STAT_o2 of OC2ATS3
  268, // CBI_STAT_o3 of OC2ATS3
  292, // CBI_STAT_o4 of OC2ATS3
  316, // CBI_STAT_o5 of OC2ATS3
  340, // CBI_STAT_o6 of OC2ATS3
   76, // CBI_STAT_Lo of OC2ATS2
  272, // CBI_STAT_So of OC2ATS2
  378, // CBI_STAT_SRAM of OC2ATS2
  432, // CBI_STAT_M of OC2ATS2
  364  // CBI_STAT_A of OC2ATS3
};

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
static const CBI_STAT_LABEL cbi_stat_labeling[] = {
  { _CBI_KIND_NONSENS, "", "" }
};
#endif
#ifdef CBI_STAT_LABELING
#undef CBI_STAT_LABELING
#endif

CBI_STAT_ATTR cbi_stat_prof[END_OF_OCs][CBI_MAX_STAT_BITS];
static int frontier[END_OF_OCs];

static CBI_STAT_ATTR_PTR cbi_stat_hash_budgets[CBI_STAT_HASH_BUDGETS_NUM];

static int hash_key ( const int budgets_num, const char *ident ) {
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
static CBI_STAT_ATTR_PTR *walk_hash ( CBI_STAT_ATTR_PTR *ppB, const char *ident ) {
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

static CBI_STAT_ATTR_PTR re_hash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident, const char *ident_new ) {
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
static CBI_STAT_ATTR_PTR re_hash_local ( const char *ident, const char *ident_new ) {
  assert( ident );
  assert( ident_new );
  return re_hash( cbi_stat_hash_budgets, CBI_STAT_HASH_BUDGETS_NUM, ident, ident_new );
}

CBI_STAT_ATTR_PTR cbi_stat_rehash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident, const char *ident_new ) {
  assert( budgets );
  assert( budgets_num > 0 );
  assert( ident );
  assert( ident_new );
  return re_hash( budgets, budgets_num, ident, ident_new );
}

static CBI_STAT_ATTR_PTR conslt_hash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident ) {
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
static CBI_STAT_ATTR_PTR conslt_hash_local ( const char *ident ) {
  assert( ident );
  return conslt_hash( cbi_stat_hash_budgets, CBI_STAT_HASH_BUDGETS_NUM, ident );
}
CBI_STAT_ATTR_PTR conslt_cbi_code_tbl ( const char *ident ) {
  assert( ident );
  return conslt_hash_local( ident );
}

CBI_STAT_ATTR_PTR cbi_stat_idntify ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident ) {
  assert( budgets );
  assert( budgets_num );
  assert( ident );
  return conslt_hash( budgets, budgets_num, ident );
}

char *show_cbi_stat_bitmask ( char *mask_name, int len, CBI_STAT_BIT_MASK mask ) {
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

CBI_STAT_BIT_MASK cbi_stat_bit_maskpat ( int pos ) {
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

static int lex_shname ( char *pshname, int shname_len ) {
  assert( pshname );
  assert( (shname_len > 0) && (shname_len <= CBI_STAT_NAME_LEN) );
  int disp_in_shname = -1;
  
  char *p = NULL;
  p = pshname;
  while( *p && (p < (pshname + shname_len)) ) {
    assert( *p && (p < (pshname + shname_len)) );
    if( *p == '_' ) {
      *p = 0;
      assert( strnlen( pshname, shname_len ) < shname_len );
      disp_in_shname = atoi( ++p );
      break;
    }
    p++;
  }
  return disp_in_shname;
}

typedef enum _cbi_lex_phase {
  CBI_LEX_NAME,
  CBI_LEX_GROUP,
  CBI_LEX_DISP,
  CBI_LEX_SHEETNAME,
  END_OF_CBI_LEX
} CBI_LEX_PHASE;
static BOOL cbi_lex ( char *src, int src_len, char *name, int name_len, int *pgroup, int *pdisp, char *pshname, int shname_len ) {
  assert( src );
  assert( (src_len > 0) && (src_len <= CBI_STAT_BITS_LEXBUF_SIZE) );
  assert( name );
  assert( (name_len > 0) && (name_len <= CBI_STAT_NAME_LEN) );
  assert( pgroup );
  assert( pdisp );
  assert( pshname );
  assert( (shname_len > 0) && (shname_len <= CBI_STAT_NAME_LEN) );
  
  BOOL r = TRUE;
  CBI_LEX_PHASE ph = CBI_LEX_NAME;
  char *pword = src;
  char *p = src;
  while( (p - src) < src_len ) {
    assert( pword <= p );
    if( !(*p) ) {
      if( ph == CBI_LEX_SHEETNAME ) {
	int d = -1;
	d = lex_shname( pword, shname_len );
	r = FALSE;
	if( d > 0 )
	  if( d == *pdisp ) {
	    strncpy( pshname, pword, shname_len );
	    r = TRUE;
	  }
	ph = END_OF_CBI_LEX;
      }
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
      case CBI_LEX_DISP:
	*pdisp = atoi( pword );
	ph = CBI_LEX_SHEETNAME;
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
      show_cbi_stat_bitmask( str, CBI_STAT_MASKNAME_MAXLEN, cbi_stat_prof[oc_id][i].disp.mask );
      printf( "disp.mask: %s\n", str );
    }
    printf( "\n" );
  }
}
int load_cbi_code_tbl ( OC_ID oc_id, const char *fname ) {
  assert( fname );
  BOOL err = FALSE;
  FILE *fp = NULL;
  
  fp = fopen( fname, "r" );
  if( fp ) {
    int lines = 1;
    int group = -1;
    int disp = -1;
    char bit_name[CBI_STAT_NAME_LEN + 1];
    char sh_name[CBI_STAT_NAME_LEN + 1];
    bit_name[CBI_STAT_NAME_LEN] = 0;
    sh_name[CBI_STAT_NAME_LEN] = 0;
    while( (! feof(fp)) && (frontier[oc_id] < CBI_MAX_STAT_BITS) ) {
      char buf[CBI_STAT_BITS_LEXBUF_SIZE + 1];
      buf[CBI_STAT_BITS_LEXBUF_SIZE] = 0;
      fscanf( fp, "%s\n", buf );
      if( ! cbi_lex( buf, CBI_STAT_BITS_LEXBUF_SIZE, bit_name, CBI_STAT_NAME_LEN, &group, &disp, sh_name, CBI_STAT_NAME_LEN ) ) {
	errorF( "failed lexical analyzing the CBI OC%3d code-table of %s, in the line: %d.\n,", OC_ID_CONV2INT(oc_id), fname, lines );
	assert( FALSE );
      } else {
	CBI_STAT_ATTR_PTR pA = &cbi_stat_prof[oc_id][frontier[oc_id]];
	assert( pA );		       
	if( (err = (BOOL)ferror( fp )) )
	  break;
	pA->src.fname = fname;
	pA->src.line = lines;
#if 0
	strncpy( pA->name, bit_name, CBI_STAT_NAME_LEN );
	strncpy( pA->ident, bit_name, CBI_STAT_NAME_LEN );
#else
	{
	  char *p = NULL;
	  p = stpncpy( pA->name, "[", 1 );
	  assert( p && !(*p) );
	  p = stpncpy( p, sh_name, CBI_STAT_NAME_LEN );
	  assert( p && !(*p) );
	  p = stpncpy( p, "]", 1 );
	  assert( p && !(*p) );
	  strncpy( p, bit_name, CBI_STAT_NAME_LEN );
	}
	strncpy( pA->ident, pA->name, CBI_STAT_NAME_LEN );
#endif
	pA->oc_id = oc_id;
	pA->kind = _UNKNOWN;
	pA->group.raw = (CBI_STAT_GROUP)group;
	pA->group.msg_id = cbi_group2msg[pA->group.raw];
	pA->group.addr = cbi_group2addr[pA->group.raw];
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
	pA->disp.mask = cbi_stat_bit_maskpat( pA->disp.bits );
	frontier[oc_id]++;
      }
      lines++;
      //dup_CBI_code_tbl( name, group, disp ); // ***** for debugging.
    }
    fclose( fp );
    //fclose( fp_out ); // ***** for debugging.
  } else {
    errorF( "failed to open the file: %s.\n", fname );
    err = TRUE;
    //assert( FALSE );
  }
  
  if( !err ) {
    int i;
    for( i = 0; i < frontier[oc_id]; i++ )
      regist_hash_local( &cbi_stat_prof[oc_id][i] );
    assert( i == frontier[oc_id] );
  }
  
  {
    int r = frontier[oc_id];
    assert( r >= 0 );
    if( err ) {
      if( r == 0 )
	r = 1;
      r *= -1;
    }
    return r;
  }
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

#if 0 // for MODULE-TEST
int main ( void ) {
  const OC_ID oc_id = OC801;
  int cnt = 0;
  int n = -1;
  
  n = load_cbi_code_tbl ( oc_id, "./cbi/BOTANICAL_GARDEN.csv" );
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

const CBI_STAT_KIND il_obj_kind[] = {
#define IL_OBJ_INSTANCE_DESC(kind, raw_name, exp) kind,
#include "./cbi/il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
  _CBI_KIND_NONSENS
};
const CBI_STAT_KIND whats_kind_of_il_obj ( IL_OBJ_INSTANCES obj ) {
  assert( (obj >= 0) && (obj < END_OF_IL_OBJ_INSTANCES) );
  CBI_STAT_KIND r = _CBI_KIND_NONSENS;
  r = il_obj_kind[obj];
  return r;
}

const char *cnv2str_il_obj_instances[] = {
#define IL_3t(exp, str, code) str
#define IL_OBJ_INSTANCE_DESC(kind, raw_name, exp) exp,
#include "./cbi/il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
#undef IL_3t
  NULL
};
const char *cnv2str_il_obj ( IL_OBJ_INSTANCES obj ) {
  assert( (obj >= 0) && (obj < END_OF_IL_OBJ_INSTANCES) );
  return cnv2str_il_obj_instances[obj];
}

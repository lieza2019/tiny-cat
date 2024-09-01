#ifndef CBI_H
#define CBI_H

#include <sys/types.h>
#include "generic.h"
#include "misc.h"
#include "network.h"

#include "./cbi/il_obj_instance_decl.h"
extern const char *cnv2str_il_obj_instances[];
const char *cnv2str_il_obj ( IL_OBJ_INSTANCES obj );

#define CBI_STAT_CSV_FNAME_BCGN "BOTANICAL_GARDEN.csv"
#define CBI_STAT_CSV_FNAME_JLA "JASOLA_VIHAR.csv"
#define CBI_STAT_CSV_FNAME_IWNA NULL
#define CBI_STAT_CSV_FNAME_RKPM NULL
#define CBI_STAT_CSV_FNAME_IGDA NULL
#define CBI_STAT_CSV_FNAME_JPW NULL
#define CBI_STAT_CSV_FNAME_KIKD_OC1 NULL
#define CBI_STAT_CSV_FNAME_KIKD_OC2 NULL
#define CBI_STAT_CSV_FNAME_KPEN NULL
#define CBI_STAT_CSV_FNAME_PAGI NULL
#define CBI_STAT_CSV_FNAME_DPCK NULL
#define CBI_STAT_CSV_FNAME_NPPR NULL
#define CBI_STAT_CSV_FNAME_MKPR NULL
#define CBI_STAT_CSV_FNAME_GAGR NULL
#define CBI_STAT_CSV_FNAME_RKAM NULL
#define CBI_STAT_CSV_FNAME_MKPD NULL

typedef enum oc_id {
  OC801,
  OC802,
  OC803,
  OC804,
  OC805,
  OC806,
  OC807,
  OC808,
  OC809,
  OC810,
  OC811,
  OC812,
  OC813,
  OC814,
  OC815,
  OC816,
  END_OF_OCs
} OC_ID;
#define OC_ID_CONV2INT( oc_id ) ((oc_id) + 801)

typedef enum ats2oc_cmd {
  ATS2OC801,
  ATS2OC802,
  ATS2OC803,
  ATS2OC804,
  ATS2OC805,
  ATS2OC806,
  ATS2OC807,
  ATS2OC808,
  ATS2OC809,
  ATS2OC810,
  ATS2OC811,
  ATS2OC812,
  ATS2OC813,
  ATS2OC814,
  ATS2OC815,
  ATS2OC816,
  END_OF_ATS2OC
} ATS2OC_CMD;

typedef enum oc2ats_stat {
  OC2ATS1,
  OC2ATS2,
  OC2ATS3,
  END_OF_OC2ATS
} OC2ATS_STAT;
#define OC_MSG_ID_CONV2INT( msg_id ) ((msg_id) + 1)

#define OC_LNN_801_SYS1 101
#define OC_LNN_802_SYS1 102
#define OC_LNN_803_SYS1 103
#define OC_LNN_804_SYS1 104
#define OC_LNN_805_SYS1 105
#define OC_LNN_806_SYS1 106
#define OC_LNN_807_SYS1 107
#define OC_LNN_808_SYS1 108
#define OC_LNN_809_SYS1 109
#define OC_LNN_810_SYS1 110
#define OC_LNN_811_SYS1 111
#define OC_LNN_812_SYS1 112
#define OC_LNN_813_SYS1 113
#define OC_LNN_814_SYS1 114
#define OC_LNN_815_SYS1 115
#define OC_LNN_816_SYS1 116

#define UDP_BCAST_RECV_PORT_ATS2OC_801 58199
#define UDP_BCAST_RECV_PORT_ATS2OC_802 58299
#define UDP_BCAST_RECV_PORT_ATS2OC_803 58399
#define UDP_BCAST_RECV_PORT_ATS2OC_804 58499
#define UDP_BCAST_RECV_PORT_ATS2OC_805 58599
#define UDP_BCAST_RECV_PORT_ATS2OC_806 58699
#define UDP_BCAST_RECV_PORT_ATS2OC_807 58799
#define UDP_BCAST_RECV_PORT_ATS2OC_808 58899
#define UDP_BCAST_RECV_PORT_ATS2OC_809 58999
#define UDP_BCAST_RECV_PORT_ATS2OC_810 59099
#define UDP_BCAST_RECV_PORT_ATS2OC_811 59199
#define UDP_BCAST_RECV_PORT_ATS2OC_812 59299
#define UDP_BCAST_RECV_PORT_ATS2OC_813 59399
#define UDP_BCAST_RECV_PORT_ATS2OC_814 59499
#define UDP_BCAST_RECV_PORT_ATS2OC_815 59599
#define UDP_BCAST_RECV_PORT_ATS2OC_816 59699

#define UDP_BCAST_RECV_PORT_OC2ATS1_STAT 58198
#define UDP_BCAST_RECV_PORT_OC2ATS2_STAT 58197
#define UDP_BCAST_RECV_PORT_OC2ATS3_STAT 58196

#define OC_ATS2OC_MSGSIZE 286
#define OC_OC2ATS1_MSGSIZE 1404
#define OC_OC2ATS2_MSGSIZE 944
#define OC_OC2ATS3_MSGSIZE 1260

#define OC_OC2ATS_MSGS_NUM 3
typedef struct recv_buf_cbi_stat {
  struct {
    struct {
      NXNS_HEADER header;
      unsigned char arena[OC_OC2ATS1_MSGSIZE - 76]; // for OC_OC2ATS1_MSGSIZE > OC_OC2ATS3_MSGSIZE > OC_OC2ATS2_MSGSIZE
    } buf;
    BOOL updated;
  } msgs[OC_OC2ATS_MSGS_NUM];
} RECV_BUF_CBI_STAT, *RECV_BUF_CBI_STAT_PTR;
extern RECV_BUF_CBI_STAT cbi_stat_info[END_OF_OCs];

typedef struct CBI_stat_infoset {
  char oc_name[END_OF_OCs][6];
  IP_ADDR_DESC oc_ipaddr[END_OF_OCs];
  uint16_t LNN[END_OF_OCs];
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_recv_cbi_stat;
    OC_ID dest_oc_id;
    RECV_BUF_CBI_STAT sent;
  } ats2oc;
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_recv_cbi_stat;
    OC_ID sender_oc_id;
    RECV_BUF_CBI_STAT recv;
  } oc2ats;
} CBI_STAT_INFO, *CBI_STAT_INFO_PTR;
extern CBI_STAT_INFO cbi_stat_ATS2OC[END_OF_ATS2OC];
extern CBI_STAT_INFO cbi_stat_OC2ATS[END_OF_OC2ATS];

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
extern const char *CBI_STAT_GROUP_CONV2STR[];

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

typedef enum cbi_stat_kind {
#define CBI_STAT_KIND_DESC(enum, name) enum,
#include "./cbi/cbi_stat_kind.def"
#undef CBI_STAT_KIND_DESC
  END_OF_CBI_STAT_KIND
} CBI_STAT_KIND;
extern const char *cnv2str_cbi_stat_kind[];

extern const CBI_STAT_KIND il_obj_kind[];
extern const CBI_STAT_KIND whats_kind_of_il_obj ( IL_OBJ_INSTANCES obj );
  
typedef struct cbi_stat_csv_fnames {
  OC_ID oc_id;
  //char *fname;
  char *csv_fname;
} CBI_STAT_CSV_FNAMES, *CBI_STAT_CSV_FNAMES_PTR;

extern const CBI_STAT_CSV_FNAMES il_status_geometry_resources[END_OF_OCs + 1];

#define CBI_STAT_IDENT_LEN 32
#define CBI_STAT_NAME_LEN 32
typedef struct cbi_stat_attr {
  char ident[CBI_STAT_IDENT_LEN + 1];
  char name[CBI_STAT_NAME_LEN + 1];
  struct {
    CBI_STAT_GROUP raw;
    //OC2ATS_STAT oc_from;
    OC2ATS_STAT msg_id;
    int addr;
  } group;
  struct {
    int raw;
    int bytes;
    int bits;
    CBI_STAT_BIT_MASK mask;
  } disp;
  CBI_STAT_KIND kind;
  OC_ID oc_id;
  struct cbi_stat_attr *pNext_hash;
  struct cbi_stat_attr *pNext_decl;
} CBI_STAT_ATTR, *CBI_STAT_ATTR_PTR;

#define CBI_MAX_STAT_BITS 65536
extern CBI_STAT_ATTR cbi_stat_prof[END_OF_OCs][CBI_MAX_STAT_BITS];

#define CBI_STAT_HASH_BUDGETS_NUM 256
extern CBI_STAT_ATTR_PTR cbi_stat_regist ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, CBI_STAT_ATTR_PTR pE );
extern CBI_STAT_ATTR_PTR cbi_stat_rehash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident, const char *ident_new );
//extern CBI_STAT_ATTR_PTR cbi_stat_idntify ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, char *ident );
extern CBI_STAT_ATTR_PTR cbi_stat_idntify ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident );

#define CBI_STAT_BITS_LEXBUF_SIZE 256
extern int load_cbi_code_tbl ( OC_ID oc_id, const char *fname );
extern CBI_STAT_ATTR_PTR conslt_cbi_code_tbl ( const char *ident );
extern int reveal_cbi_code_tbl ( void );
extern void dump_cbi_stat_prof ( OC_ID oc_id );

extern char *show_cbi_stat_bitmask ( char *mask_name, int len, CBI_STAT_BIT_MASK mask );
extern CBI_STAT_BIT_MASK cbi_stat_bit_maskpat ( int pos );

typedef struct cbi_stat_label {
  CBI_STAT_KIND kind;
  char name[CBI_STAT_NAME_LEN + 1];  
  char ident[CBI_STAT_IDENT_LEN + 1];
} CBI_STAT_LABEL, *CBI_STAT_LABEL_PTR;

#endif // CBI_H

#ifndef CBI_H_INCLUDED
#define CBI_H_INCLUDED

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

#define ATS2OC_MSGSIZE 286
#define OC2ATS1_MSGSIZE 1404
#define OC2ATS2_MSGSIZE 944
#define OC2ATS3_MSGSIZE 1260

#define OC_OC2ATS_MSGS_NUM 3
typedef struct recv_buf_cbi_stat {
  struct {
    struct {
      NXNS_HEADER header;
      unsigned char arena[OC2ATS1_MSGSIZE - sizeof(NXNS_HEADER)]; // for OC_OC2ATS1_MSGSIZE > OC_OC2ATS3_MSGSIZE > OC_OC2ATS2_MSGSIZE, sizeof(NXNS_HEADER) == 76.
    } buf;
    BOOL updated;
  } msgs[OC_OC2ATS_MSGS_NUM];
} RECV_BUF_CBI_STAT, *RECV_BUF_CBI_STAT_PTR;
typedef struct recv_buf_cbi_stat SENT_BUF_CBI_CTRL;
typedef struct recv_buf_cbi_stat *SENT_BUF_CBI_CTRL_PTR;
extern RECV_BUF_CBI_STAT cbi_stat_info[END_OF_OCs];

typedef struct cbi_ctrl_stat_comm_prof {
  ATS2OC_CMD dest_oc_id;
  const unsigned short dst_port;
  TINY_SOCK_DESC d_sent_cbi_ctrl;
  IP_ADDR_DESC dst_ipaddr;
  struct {
    time_t emission_start;
    uint32_t seq;
  } nx;
  SENT_BUF_CBI_CTRL sent;
} CBI_CTRL_STAT_COMM_PROF, *CBI_CTRL_STAT_COMM_PROF_PTR;
typedef struct cbi_ctrl_stat_infoset {
  char oc_name[END_OF_OCs][6];
  IP_ADDR_DESC oc_ipaddr[END_OF_OCs];
  uint16_t LNN[END_OF_OCs];
  CBI_CTRL_STAT_COMM_PROF ats2oc;
  struct {
    const unsigned short dst_port;
    TINY_SOCK_DESC d_recv_cbi_stat;
    OC_ID sender_oc_id;
    RECV_BUF_CBI_STAT recv;
  } oc2ats;
} CBI_CTRL_STAT_INFO, *CBI_CTRL_STAT_INFO_PTR;
extern CBI_CTRL_STAT_INFO cbi_stat_ATS2OC[END_OF_ATS2OC];
extern CBI_CTRL_STAT_INFO cbi_stat_OC2ATS[END_OF_OC2ATS];

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

#define CTRL_LIT_SUSTAIN_CNT 50
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
  struct {
    BOOL ctrl_bit;
    ATS2OC_CMD cmd_id;
    int cnt_2_kil;
    BOOL setval;
    struct cbi_stat_attr *pNext_ctrl;
  } attr_ctrl;
  struct {
    const char *fname;
    int line;
  } src;
  BOOL dirty;
  struct cbi_stat_attr *pNext_dirt;
  struct cbi_stat_attr *pNext_hash;
  struct {
    int nentities;
    struct cbi_stat_attr *pNext;
    struct cbi_stat_attr *pFamily;
    void *plex_il_obj;
  } decl_gen;
} CBI_STAT_ATTR, *CBI_STAT_ATTR_PTR;

#define CBI_MAX_STAT_BITS 65536

typedef struct cbi_code_tbl {
  CBI_STAT_ATTR codes[CBI_MAX_STAT_BITS];
  CBI_STAT_ATTR_PTR pctrl_codes;
  CBI_STAT_ATTR_PTR pdirty_bits;
} CBI_CODETBL, *CBI_CODETBL_PTR;

#if 0
extern CBI_CODE_TBL cbi_stat_prof[END_OF_OCs];
#else
#define IL_SYM_IDENTCHRS_LEN
#define MAX_IL_SYMS 65536
typedef struct il_sym_attrib {
  CBI_STAT_KIND kind;
  IL_OBJ_INSTANCES id;
  char identchrs[IL_SYM_IDENTCHRS_LEN + 1];
  int code;
} IL_SYM_ATTR, *IL_SYM_ATTR_PTR;
typedef struct cbi_lexica {
  CBI_CODETBL cbi_stat_prof[END_OF_OCs];
  IL_SYM_ATTR il_sym_attrs[MAX_IL_SYMS];
} CBI_LEXICA, *CBI_LEXICA_PTR;
extern CBI_LEXICA cbi_stat_syms;
#endif

#define CBI_STAT_HASH_BUDGETS_NUM 256
extern CBI_STAT_ATTR_PTR cbi_stat_regist ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, CBI_STAT_ATTR_PTR pE, BOOL mode, const char *errmsg_pre );
extern CBI_STAT_ATTR_PTR cbi_stat_rehash ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident, const char *ident_new, const char *errmsg_pre );
extern CBI_STAT_ATTR_PTR cbi_stat_idntify ( CBI_STAT_ATTR_PTR budgets[], const int budgets_num, const char *ident );

#define CBI_STAT_BITS_LEXBUF_SIZE 256
extern int load_cbi_code ( OC_ID oc_id, const char *fname );
extern CBI_STAT_ATTR_PTR conslt_cbi_code_tbl ( const char *ident );
extern int revise_cbi_codetbl ( const char *errmsg_pre );
extern void dump_cbi_stat_prof ( OC_ID oc_id );

extern char *show_cbi_stat_bitmask ( char *mask_name, int len, CBI_STAT_BIT_MASK mask );
extern CBI_STAT_BIT_MASK cbi_stat_bit_maskpat ( int pos );

typedef struct cbi_stat_label {
  CBI_STAT_KIND kind;
  char name[CBI_STAT_NAME_LEN + 1];
  char ident[CBI_STAT_IDENT_LEN + 1];
} CBI_STAT_LABEL, *CBI_STAT_LABEL_PTR;

#endif // CBI_H_INCLUDED

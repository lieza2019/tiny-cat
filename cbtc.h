#ifndef CBTC_H
#define CBTC_H

#include "generic.h"
#include "misc.h"
#include "sparcs.h"
#include "ars.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

typedef struct block {
  unsigned short block_name;
  CBTC_BLOCK_ID virt_block_name;
  char *virt_blkname_str;
  int len;
  struct {
    CBI_STAT_KIND kind;
    IL_OBJ_INSTANCES track;
  } belonging_tr;
  struct {
    BOOL has_sp;
    SP_ID sp_id;
  } sp;
  TINY_TRAIN_STATE_PTR occupancy; // the link to the train which FRONT_BLOCK-ID bites this block.
  struct {
    BOOL msc_flg1;
    BOOL msc_flg2;
    BOOL msc_flg3;
  } misc; // flags available for miscelleous purposes, e.g. debugging, testing and so forth.
} CBTC_BLOCK, *CBTC_BLOCK_PTR;
#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_def.h"
#undef BLOCK_ATTRIB_DEFINITION

extern void cons_lkuptbl_cbtc_block_prof ( void );
extern CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name );
extern CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname );

#endif // CBTC_H

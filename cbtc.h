#ifndef CBTC_H
#define CBTC_H

#include "generic.h"
#include "misc.h"
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
  /* the link to the train which FRONT_BLOCK-ID/REAR_BLOCK_ID bite this block, should be accessed with the type of TINY_TRAIN_STATE_PTR,
     via the designated access-methods of,
       TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
       TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
       TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB );
       TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB )
     declared in train_ctrl.h
  */
  struct {
    void *ptrains;
#if 0
    void *edge_L, *edge_R;
#else
    void *edge;
#endif
  } residents;
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

extern void purge_block_restrains ( void );

#endif // CBTC_H

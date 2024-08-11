#ifndef CBTC_H
#define CBTC_H

#include "generic.h"
#include "misc.h"
#include "ars.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_def.h"
#undef BLOCK_ID_DEFINITIONS

typedef enum stop_detection_cond {
  PO0_DETECTION,
  VIRTUAL_P0,
  FORWARD_BACK_BLOCKS,
} STOP_DETECTION_COND;

typedef struct block {
  const unsigned short block_name;
  const CBTC_BLOCK_ID virt_block_name;
  const char *virt_blkname_str;
  const int len;
  struct {
    const IL_OBJ_INSTANCES track;
    const struct track *ptrack;
  } belonging_tr;
  struct {
    BOOL has_sp;
    SP_ID sp_id;
    STOP_DETECTION_COND stop_detect_cond;
    union {
      struct {
	CBTC_BLOCK_ID forward, back;
	struct block *pforward, *pback;
      } forward_back;
    } u;
  } const sp;
  /* the link to the train which FRONT_BLOCK-ID/REAR_BLOCK_ID bite this block, should be accessed with the type of TINY_TRAIN_STATE_PTR,
     via the designated access-methods of,
       TINY_TRAIN_STATE_PTR border_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
       TINY_TRAIN_STATE_PTR write_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB, TINY_TRAIN_STATE_PTR pT );
       TINY_TRAIN_STATE_PTR read_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB );
       TINY_TRAIN_STATE_PTR *addr_residents_CBTC_BLOCK ( CBTC_BLOCK_PTR pB )
     declared in train_ctrl.h
  */
  struct {
#if 0
    void *overlayed;
    void *ptrains_from_L, *ptrains_from_R;
    void *edge_L, *edge_R;
#else
    void *ptrains;
    void *edge;
#endif
  } residents;
  struct {
    BOOL msc_flg1;
    BOOL msc_flg2;
    BOOL msc_flg3;
  } misc; // flags available for miscelleous purposes, e.g. debugging, testing and so forth.
} CBTC_BLOCK, *CBTC_BLOCK_PTR;
typedef const struct block *CBTC_BLOCK_C_PTR;

#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_def.h"
#undef BLOCK_ATTRIB_DEFINITION

extern void cons_lkuptbl_cbtc_block_prof ( void );
extern CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name );
extern CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname );

extern void purge_block_restrains ( void );

#endif // CBTC_H

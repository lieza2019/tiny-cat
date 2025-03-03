#ifndef CBTC_H
#define CBTC_H

#include "generic.h"
#include "misc.h"

#define BLOCK_ID_DEFINITIONS
#include "cbtc_dataset.h"
#undef BLOCK_ID_DEFINITIONS
#include "ars.h"

typedef enum stop_detection_cond {
  P0_COUPLING,
  VIRTUAL_P0,
  END_OF_STOP_DETECTION_TYPES
} STOP_DETECTION_TYPE;

#define MAX_BLOCK_MORPHS 2
#define MAX_POINTS_ON_MORPHING 1
#define MAX_ADJACENT_BLKS 3
typedef struct block {
  const unsigned short block_name;
  const CBTC_BLOCK_ID virt_block_name;
  const char *virt_blkname_str;
  const SC_ID zone;
  struct {
    int num_morphs;
    struct {
      struct {
	const unsigned short ln_blk;
	const int edge_pos;
      } linkages[MAX_ADJACENT_BLKS];
      const int len;
      IL_SYM points[MAX_POINTS_ON_MORPHING];
    } morphs[MAX_BLOCK_MORPHS];
  } shape;
  struct {
    const IL_SYM track;
    const struct track *ptrack;
  } belonging_tr;
  struct {
    const BOOL has_sp;
    const STOPPING_POINT_CODE sp_code;
    const STOP_DETECTION_TYPE stop_detect_type;
    struct {
      const CBTC_BLOCK_ID paired_blk;
      struct block *ppaired_blk;
    } stop_detect_cond;
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
    void *overwhelmed;
    void *ptrains;
    void *edges[MAX_ADJACENT_BLKS];
  } residents;
  struct {
    BOOL msc_flg1;
    BOOL msc_flg2;
    BOOL msc_flg3;
  } misc; // flags available for miscelleous purposes, e.g. debugging, testing and so forth.
} CBTC_BLOCK, *CBTC_BLOCK_PTR;
typedef const struct block *CBTC_BLOCK_C_PTR;

#define BLOCK_ATTRIB_DEFINITION
#include "cbtc_dataset.h"
#undef BLOCK_ATTRIB_DEFINITION

extern void cons_lkuptbl_cbtc_block_prof ( void );
extern CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name );
extern CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname );
extern void cons_lkuptbl_sp2_block ( void );
extern CBTC_BLOCK_C_PTR lookup_block_of_sp ( STOPPING_POINT_CODE sp );
extern void purge_block_restrains ( void );

#endif // CBTC_H

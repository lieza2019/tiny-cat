#ifndef CBTC_H
#define CBTC_H

#include <pthread.h>
#include "generic.h"
#include "misc.h"

typedef enum pltb_id {
  PL1 = 1,
  PL2,
  PL3,
  TB1,
  TB2,
  TB3,
  TB4,
  END_OF_PLTB_ID
} PLTB_ID;
#include "cbtc_datadef.h"
typedef struct sp_pltb {
  STOPPING_POINT_CODE sp;
  ST_ID st;
  PLTB_ID pltb;  
} SP_PLTB, *SP_PLTB_PTR;
extern SP_PLTB sp_pltb[];

#define SCID_CNV2_INT( sc_id ) ((sc_id) + 801)
extern const char *cnv2str_sp_code ( STOPPING_POINT_CODE sp_code );
extern STOPPING_POINT_CODE str2_sp_code ( const char *sp_str );
extern const char *cnv2str_pltb_id ( PLTB_ID pltb_id );
extern PLTB_ID str2_pltb_id ( const char *pltb_str );
extern const char *cnv2str_st_id ( ST_ID st_id );
extern ST_ID str2_st_id ( const char *st_str );

#include "ars.h"

typedef enum stop_detection_cond {
  P0_COUPLING,
  VIRTUAL_P0,
  END_OF_STOP_DETECTION_TYPES
} STOP_DETECTION_TYPE;

#define MAX_BLOCK_MORPHS 16
#define MAX_POINTS_ON_MORPHING 8
#define MAX_ADJACENT_BLKS 16
typedef enum linx_bondage_kind {
  LINK_NONE = 0,
  LINK_HARD = 1,
  LINK_SEMIHARD,
  LINK_SOFT,
} LINX_BONDAGE_KIND;
typedef struct blk_linkages {
  const unsigned short neigh_blk;
  const int edge_pos;
  struct blk_linkages *pNext;
  struct blk_morph *pmorph;
  struct {
    LINX_BONDAGE_KIND kind;
    struct blk_linkages *pln_neigh;
  } bond;
} BLK_LINKAGE, *BLK_LINKAGE_PTR;
typedef struct blk_morph {
  int num_links;
  BLK_LINKAGE linkages[MAX_ADJACENT_BLKS];
  const int len;
  int num_points;
  IL_SYM points[MAX_POINTS_ON_MORPHING];
  struct block *pblock;
} BLK_MORPH, *BLK_MORPH_PTR;
typedef struct block {
  const unsigned short block_name;
  const CBTC_BLOCK_ID virt_block_name;
  const char *virt_blkname_str;
  const SC_ID zone;
  struct {
    int num_morphs;
    BLK_MORPH morphs[MAX_BLOCK_MORPHS];
  } shape;
  struct {
    const IL_SYM track;
    const struct track *ptrack;
    struct block *pNext;
  } belonging_tr;
  struct {    
    const BOOL has_sp;
    //const STOPPING_POINT_CODE sp_code;
    const SP_ST sp_code;    
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
extern CBTC_BLOCK block_state[];

extern const char *cnv2str_lkup ( const char *id2str_tbl[], int id );
extern void print_block_prof ( FILE *fp_out, CBTC_BLOCK_PTR pprof );

extern void cons_block_state ( void );
extern int enum_fixed_edges ( CBTC_BLOCK_PTR pblk, BLK_LINKAGE_PTR fixes[], const int len );

extern void cons_lkuptbl_cbtc_block_prof ( void );
extern CBTC_BLOCK_PTR lookup_cbtc_block_prof ( unsigned short block_name );
extern CBTC_BLOCK_PTR conslt_cbtc_block_prof ( CBTC_BLOCK_ID virt_blkname );
extern void cons_lkuptbl_sp2_block ( void );
extern CBTC_BLOCK_C_PTR lookup_block_of_sp ( STOPPING_POINT_CODE sp );
extern void purge_block_restrains ( void );

extern pthread_mutex_t cbtc_ctrl_cmds_mutex;
extern pthread_mutex_t cbtc_stat_infos_mutex;
#endif // CBTC_H

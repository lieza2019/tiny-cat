#include "generic.h"
#include "misc.h"
#include "cbi.h"
#include "cbtc.h"

typedef struct route_lock {
  BOOL app;
  CBI_STAT_KIND kind;
  IL_OBJ_INSTANCES id;
} ROUTE_LOCK, *ROUTE_LOCK_PTR;

#define MAX_TRACK_BLOCKS 21
typedef struct track {
  const CBI_STAT_KIND kind_cbi;
  const IL_OBJ_INSTANCES id;
  const char *name;
  struct {
    const int num_blocks;
    const CBTC_BLOCK_ID blocks[MAX_TRACK_BLOCKS];
    CBTC_BLOCK_C_PTR pblocks[MAX_TRACK_BLOCKS];
  } cbtc;
  struct {
    ROUTE_LOCK TLSR, TRSR;
    ROUTE_LOCK sTLSR, sTRSR;
    ROUTE_LOCK eTLSR, eTRSR;
    ROUTE_LOCK kTLSR, kTRSR;
  } const lock;
} TRACK, *TRACK_PTR;
typedef const struct track *TRACK_C_PTR;

#define TRACK_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef TRACK_ATTRIB_DEFINITION

typedef enum route_kind {
  //DEPT_ROUTE,
  DEP_ROUTE,
  ENT_ROUTE,
  SHUNT_ROUTE,
  EMERGE_ROUTE,
  END_OF_ROUTE_KINDS
} ROUTE_KIND;
extern const CBI_STAT_KIND ROUTE_KIND2GENERIC[];

#define MAX_ROUTE_TRACKS 21
#define MAX_ROUTE_TRG_BLOCKS 21
typedef struct route {
  const CBI_STAT_KIND kind_cbi;
  const ROUTE_KIND kind_route;
  const IL_OBJ_INSTANCES id;
  const char *name;
  struct {
    const int num_tracks;
    const IL_OBJ_INSTANCES tracks[MAX_ROUTE_TRACKS];
    TRACK_C_PTR ptracks[MAX_ROUTE_TRACKS];
  } body;
  struct {
    struct {
      IL_OBJ_INSTANCES sig;
      CBTC_BLOCK_ID blk;  // data not implemented yet.
      CBTC_BLOCK_C_PTR pblk; // data not implemented yet.
      STOPPING_POINT_CODE sp; // data not implemented yet.
    } src;
    struct {
      IL_OBJ_INSTANCES sig;
      CBTC_BLOCK_ID blk; // data not implemented yet.
      CBTC_BLOCK_C_PTR pblk; // data not implemented yet.
      STOPPING_POINT_CODE sp; // data not implemented yet.
    } dst;
  } const sig_pair;
  struct {
    const BOOL app;
    struct {
      const int num_blocks;
      const CBTC_BLOCK_ID trg_blks[MAX_ROUTE_TRG_BLOCKS];
      CBTC_BLOCK_PTR ptrg_blks[MAX_ROUTE_TRG_BLOCKS];
    } trg_sect;
    struct {
      const int num_tracks_lok;
      const int num_tracks_occ;
      const IL_OBJ_INSTANCES chk_trks[MAX_ROUTE_TRACKS];
      const int num_ahead_tracks;
      const IL_OBJ_INSTANCES ahead_trks[MAX_ROUTE_TRACKS];
      TRACK_C_PTR pchk_trks[MAX_ROUTE_TRACKS];
      TRACK_C_PTR pahead_trks[MAX_ROUTE_TRACKS];
    } ctrl_tracks;
    struct {
      struct {
	CBTC_BLOCK_ID blk;
	STOPPING_POINT_CODE sp;	
	CBTC_BLOCK_C_PTR pblk;
      } dep;
      struct {
	CBTC_BLOCK_ID blk;
	STOPPING_POINT_CODE sp;
	CBTC_BLOCK_C_PTR pblk;
      } dst;
      time_t trip_time; // data not implemented yet.
    } trip_info;
  } ars_ctrl;
} ROUTE, *ROUTE_PTR;
typedef const struct route *ROUTE_C_PTR;

#define ROUTE_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef ROUTE_ATTRIB_DEFINITION

extern TRACK_C_PTR conslt_track_prof ( IL_OBJ_INSTANCES track_id );
extern ROUTE_C_PTR conslt_route_prof ( IL_OBJ_INSTANCES route_id );

extern void cons_track_state ( TRACK_PTR ptrack );
extern void cons_route_state ( ROUTE_PTR proute );
extern void cons_cbtc_block_state ( CBTC_BLOCK_PTR pblock );
extern void cons_il_obj_tables ( void );

extern BOOL establish_OC_stat_recv ( TINY_SOCK_PTR pS );
extern BOOL establish_OC_ctrl_send ( TINY_SOCK_PTR pS );

extern pthread_mutex_t cbi_stat_info_mutex;
extern void *pth_reveal_il_status ( void *arg );
extern pthread_mutex_t cbi_ctrl_sendbuf_mutex;
extern pthread_mutex_t cbi_ctrl_dispatch_mutex;
extern void *pth_expire_il_ctrl_bits ( void *arg );
extern void *pth_reveal_il_ctrl_bits ( void *arg );
extern int conslt_il_state ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident );
extern void diag_cbi_stat_attrib ( FILE *fp_out, char *ident );

extern BOOL chk_routeconf ( ROUTE_C_PTR r1, ROUTE_C_PTR r2 );

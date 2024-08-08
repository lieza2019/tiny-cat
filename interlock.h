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
  CBI_STAT_KIND kind_cbi;
  IL_OBJ_INSTANCES id;
  char *name;
  struct {
    int num_blocks;
    CBTC_BLOCK_ID blocks[MAX_TRACK_BLOCKS];
    CBTC_BLOCK_PTR pblocks[MAX_TRACK_BLOCKS];
  } cbtc;
  struct {
    ROUTE_LOCK TLSR, TRSR;
    ROUTE_LOCK sTLSR, sTRSR;
    ROUTE_LOCK eTLSR, eTRSR;
    ROUTE_LOCK kTLSR, kTRSR;
  } lock;
} TRACK, *TRACK_PTR;
typedef const struct track *TRACK_C_PTR;
#define TRACK_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef TRACK_ATTRIB_DEFINITION

typedef enum route_kind {
  DEPT_ROUTE,
  ENT_ROUTE,
  SHUNT_ROUTE,
  EMERGE_ROUTE,
  END_OF_ROUTE_KINDS
} ROUTE_KIND;
extern const CBI_STAT_KIND ROUTE_KIND2GENERIC[];

#define MAX_ROUTE_TRACKS 21
#define MAX_ROUTE_TRG_BLOCKS 21
typedef struct route {
  CBI_STAT_KIND kind_cbi;
  ROUTE_KIND kind_route;
  IL_OBJ_INSTANCES id;
  const char *name;
  struct {
    int num_tracks;
    IL_OBJ_INSTANCES tracks[MAX_ROUTE_TRACKS];
#if 0
    const TRACK_PTR ptracks[MAX_ROUTE_TRACKS];
#else
    //const struct track *ptracks[MAX_ROUTE_TRACKS];
    TRACK_C_PTR ptracks[MAX_ROUTE_TRACKS];
#endif
  } body;
  struct {
    struct {
      CBI_STAT_KIND kind;
      IL_OBJ_INSTANCES sig;
    } src;
    struct {
      CBI_STAT_KIND kind;
      IL_OBJ_INSTANCES sig;
    } dst;
  } sig_pair;
  struct {
    BOOL app;
    struct {
      int num_blocks;
      CBTC_BLOCK_ID trg_blks[MAX_ROUTE_TRG_BLOCKS];
      CBTC_BLOCK_PTR ptrg_blks[MAX_ROUTE_TRG_BLOCKS];
    } trg_sect;
    struct {
      int num_tracks_lok;
      int num_tracks_occ;
      IL_OBJ_INSTANCES chk_trks[MAX_ROUTE_TRG_BLOCKS];
#if 0
      TRACK_PTR pchk_trks[MAX_ROUTE_TRG_BLOCKS];
#else
      TRACK_C_PTR pchk_trks[MAX_ROUTE_TRG_BLOCKS];
#endif
    } ctrl_tracks;
  } ars_ctrl;
} ROUTE, *ROUTE_PTR;
typedef const struct route *ROUTE_C_PTR;
#define ROUTE_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef ROUTE_ATTRIB_DEFINITION

#if 0
#define TLSR_LOCKED( tr ) ((tr).locking.TLSR)
#define TRSR_LOCKED( tr ) ((tr).locking.TRSR)
#define sTLSR_LOCKED( tr ) ((tr).locking.sTLSR)
#define sTRSR_LOCKED( tr ) ((tr).locking.sTRSR)
#define eTLSR_LOCKED( tr ) ((tr).locking.eTLSR)
#define eTRSR_LOCKED( tr ) ((tr).locking.eTRSR)
#define kTLSR_LOCKED( tr ) ((tr).locking.kTLSR)
#define kTRSR_LOCKED( tr ) ((tr).locking.kTRSR)
#endif

extern void cons_track_state ( TRACK_PTR ptrack );
extern void cons_route_state ( ROUTE_PTR proute );
extern void cons_il_obj_tables ( void );

extern BOOL establish_OC_stat_recv ( TINY_SOCK_PTR pS );
extern BOOL establish_OC_stat_send ( TINY_SOCK_PTR pS );

extern pthread_mutex_t cbi_stat_info_mutex;
extern void reveal_il_state ( TINY_SOCK_PTR pS );
extern void *pth_reveal_il_status ( void *arg );
extern int conslt_il_state ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident );
extern void diag_cbi_stat_attrib ( FILE *fp_out, char *ident );

extern BOOL chk_routeconf ( ROUTE_C_PTR r1, ROUTE_C_PTR r2 );


#include "generic.h"
#include "misc.h"
#include "cbi.h"
#include "cbtc.h"

#ifndef INTERLOCK_H
#define INTERLOCK_H

#define ILTBL_FILENAME_MAXLEN 512

typedef enum track_bound {
  BOUND_DOWN = 1,
  BOUND_UP,
  BOUND_UNKNOWN
} TRACK_BOUND;

typedef struct route_lock {
  BOOL app;
  IL_SYM_KIND kind;
  IL_SYM id;
} ROUTE_LOCK, *ROUTE_LOCK_PTR;

#define MAX_TRACK_BLOCKS 64
#define MAX_TURNOUT_POINTS 16
typedef struct track {
  const IL_SYM_KIND kind;
  const char id_chr[CBI_STAT_IDENT_LEN + 1];
  const IL_SYM id;
  TRACK_BOUND bound;
  struct {
    const int num_blocks;
    const CBTC_BLOCK_ID blocks[MAX_TRACK_BLOCKS];
    CBTC_BLOCK_C_PTR pblocks[MAX_TRACK_BLOCKS];
  } cbtc;
  struct {
    const ROUTE_LOCK TLSR, TRSR;
    const ROUTE_LOCK sTLSR, sTRSR;
    const ROUTE_LOCK eTLSR, eTRSR;
    const ROUTE_LOCK kTLSR, kTRSR;
  } lock;
} TRACK, *TRACK_PTR;
typedef const struct track *TRACK_C_PTR;

#define TRACK_ATTRIB_DEFINITION
#include "interlock_datadef.h"
#undef TRACK_ATTRIB_DEFINITION

typedef enum route_align {
  ROUTE_ALIGN_UNKNOWN,
  ROUTE2_LEFT,
  ROUTE2_RIGHT,
  END_OF_ROUTE_ALIGN
} ROUTE_ALIGN;

typedef enum route_kind {
  ROUTE_UNKNOWN,
  DEP_ROUTE = 1,
  ENT_ROUTE,
  SHUNT_ROUTE,
  EMERGE_ROUTE,
  ROUTE_OTHER,
  END_OF_ROUTE_KINDS
} ROUTE_KIND;

#define MAX_OVERBLKS2_ROUTEDEST 16
#define MAX_ROUTE_TRACKS 32
#define MAX_ROUTE_APPTRACKS 32
#define MAX_ROUTE_POINTS 16
#define MAX_ROUTE_TRG_BLOCKS 32
typedef struct route {
  const IL_SYM_KIND kind;
  const ROUTE_KIND route_kind;
  const ROUTE_ALIGN bound;
  const char id_chr[CBI_STAT_IDENT_LEN + 1];
  const IL_SYM id;
  const IL_SYM id_ctrl;
  struct {
    const int num_tracks;
    const IL_SYM tracks[MAX_ROUTE_TRACKS];
    TRACK_C_PTR ptracks[MAX_ROUTE_TRACKS];
  } body;
  struct {
    struct {
      const IL_SYM sig;
      IL_SYM ptr_org;
    } org;
    struct {
      const IL_SYM sig;
      IL_SYM ptr_dst;
    } dst;
  } sig_pair;
  struct {
    const BOOL app;
    struct {
      const int num_blocks;
      const CBTC_BLOCK_ID trg_blks[MAX_ROUTE_TRG_BLOCKS];
      CBTC_BLOCK_C_PTR ptrg_blks[MAX_ROUTE_TRG_BLOCKS];
    } trg_sect;
    struct {
      const int num_tracks_lok;
      const int num_tracks_occ;
      const IL_SYM chk_trks[MAX_ROUTE_TRACKS];
      const int num_ahead_tracks;
      const IL_SYM ahead_trks[MAX_ROUTE_TRACKS];
      TRACK_C_PTR pchk_trks[MAX_ROUTE_TRACKS];
      TRACK_C_PTR pahead_trks[MAX_ROUTE_TRACKS];
    } ctrl_tracks;
    struct {
      struct {
	const CBTC_BLOCK_ID blk;
	const STOPPING_POINT_CODE sp;
	CBTC_BLOCK_C_PTR pblk;
      } dep;
      struct {
	const CBTC_BLOCK_ID blk;
	const STOPPING_POINT_CODE sp;
	CBTC_BLOCK_C_PTR pblk;
      } dst;
      time_t trip_time; // data not implemented yet.
    } trip_info;
  } ars_ctrl;  
} ROUTE, *ROUTE_PTR;
typedef const struct route *ROUTE_C_PTR;

#define MAX_ROUTE_DIVERGENCE 16
#define MAX_ROUTE_CONN_LEN 16
typedef struct il_obj_container {
  IL_SYM_KIND kind;
  IL_SYM sym;
  struct {
    struct {
      TRACK_C_PTR pprof;
    } track;
    struct {
      ROUTE_C_PTR pprof;
      int num_div_routes;
      struct il_obj_container const *psucc[MAX_ROUTE_DIVERGENCE];
    } route;
  } ln;
} IL_OBJ_CONTAINER, *IL_OBJ_CONTAINER_PTR;
typedef struct il_obj_container const *IL_OBJ_CONTAINER_C_PTR;

#define ROUTE_ATTRIB_DEFINITION
#include "interlock_datadef.h"
#undef ROUTE_ATTRIB_DEFINITION

extern const char *cnv2str_trbound ( TRACK_BOUND bound );
extern const char *cnv2str_roalign ( ROUTE_ALIGN align );
extern const char *cnv2str_route_kind ( ROUTE_KIND ro_kind );

extern TRACK_C_PTR conslt_track_prof ( IL_SYM track_id );
extern ROUTE_C_PTR conslt_route_prof ( IL_SYM route_id );
extern ROUTE_C_PTR conslt_route_prof_s ( const char *route_id_str );

extern void cons_track_attrib ( TRACK_PTR ptrack );
extern void cons_route_attrib ( ROUTE_PTR proute );
extern void cons_cbtc_block_attrib ( CBTC_BLOCK_PTR pblock );
extern void cons_il_obj_table ( void );

extern int establish_OC_comm_stat ( TINY_SOCK_PTR pS, TINY_SOCK_DESC *pdescs, const int ndescs );
extern int establish_OC_comm_ctrl ( TINY_SOCK_PTR pS, CBI_CTRL_STAT_COMM_PROF_PTR pprofs[], const int nprofs, const int ndsts );
extern pthread_mutex_t cbi_ctrl_dispatch_mutex;
extern pthread_mutex_t cbi_ctrl_sendbuf_mutex;
extern pthread_mutex_t cbi_stat_info_mutex;
extern int conslt_il_state ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident );
extern void *pth_reveal_il_status ( void *arg );
extern void *pth_expire_il_ctrl_bits ( void *arg );
extern void *pth_revise_il_ctrl_bits ( void *arg );

extern int engage_il_ctrl ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident );
extern int ungage_il_ctrl ( OC_ID *poc_id, CBI_STAT_KIND *pkind, const char *ident );
extern void ready_on_emit_OC_ctrl ( TINY_SOCK_PTR psocks, CBI_CTRL_STAT_COMM_PROF_PTR pprofs[], const int nprofs );

extern void diag_cbi_stat_attrib ( FILE *fp_out, char *ident );

extern BOOL chk_routeconf ( ROUTE_C_PTR r1, ROUTE_C_PTR r2 );

#endif // INTERLOCK_H

#include "generic.h"
#include "misc.h"
#include "sparcs.h"

#define ROUTE_AND_TRACK_ID_DEFINITIONS
#define TRACK_ID_DEFINITIONS
#include "interlock_def.h"
#undef ROUTE_AND_TRACK_ID_DEFINITIONS
#undef TRACK_ID_DEFINITIONS

typedef struct route_lock {
  BOOL valid;
  CBI_STAT_KIND kind;
  IL_OBJ_INSTANCES id;
} ROUTE_LOCK, *ROUTE_LOCK_PTR;

#define MAX_TRACK_BLOCKS 21
typedef struct track {
  CBI_STAT_KIND kind;
  IL_OBJ_INSTANCES id;
  char *name;
  struct {
    int num_blocks;
    BLOCK_ID blocks[MAX_TRACK_BLOCKS];
  } cbtc;
  struct {
    ROUTE_LOCK TLSR, TRSR;
    ROUTE_LOCK sTLSR, sTRSR;
    ROUTE_LOCK eTLSR, eTRSR;
    ROUTE_LOCK kTLSR, kTRSR;
  } lock;
} TRACK, *TRACK_PTR;

#define TRACK_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef TRACK_ATTRIB_DEFINITION

typedef enum route_kind {
  MAIN_ROUTE,
  SHUNTING_ROUTE,
  EMERGENCY_ROUTE,
  END_OF_ROUTE_KINDS
} ROUTE_KIND;
extern const CBI_STAT_KIND ROUTE_KIND2GENERIC[];

#define MAX_ROUTE_TRACKS 21
#define MAX_ROUTE_TRG_BLOCKS 21
typedef struct _route {
  ROUTE_KIND kind;
  IL_OBJ_INSTANCES id;
  const char *name;
  struct {
    int num_tracks;
    TRACK_ID tracks[MAX_ROUTE_TRACKS];
    TRACK_PTR ptracks[MAX_ROUTE_TRACKS];
  } tr;
  struct {
    CBI_STAT_KIND kind;
    IL_OBJ_INSTANCES src;    
  } sig_src;
  struct {
    CBI_STAT_KIND kind;
    IL_OBJ_INSTANCES dst;
  } sig_dst;  
  struct {
    BOOL is_ars;
    struct {
      int num_blocks;
      TRACK_ID trg_blks[MAX_ROUTE_TRG_BLOCKS];
      TRACK_PTR ptrg_blks[MAX_ROUTE_TRG_BLOCKS];
    } trg_section;
  } ars_ctrl;
} ROUTE, *ROUTE_PTR;

#define ROUTE_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef ROUTE_ATTRIB_DEFINITION

#define TLSR_LOCKED( tr ) ((tr).locking.TLSR)
#define TRSR_LOCKED( tr ) ((tr).locking.TRSR)
#define sTLSR_LOCKED( tr ) ((tr).locking.sTLSR)
#define sTRSR_LOCKED( tr ) ((tr).locking.sTRSR)
#define eTLSR_LOCKED( tr ) ((tr).locking.eTLSR)
#define eTRSR_LOCKED( tr ) ((tr).locking.eTRSR)
#define kTLSR_LOCKED( tr ) ((tr).locking.kTLSR)
#define kTRSR_LOCKED( tr ) ((tr).locking.kTRSR)

extern BOOL establish_CBI_comm ( TINY_SOCK_PTR pS );
extern void reveal_il_state ( TINY_SOCK_PTR pS );
extern void *pth_reveal_il_status ( void * pS );

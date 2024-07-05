#include "generic.h"
#include "misc.h"
#include "sparcs.h"

#define ROUTE_AND_TRACK_ID_DEFINITIONS
#define TRACK_ID_DEFINITIONS
#include "interlock_def.h"
#undef ROUTE_AND_TRACK_ID_DEFINITIONS
#undef TRACK_ID_DEFINITIONS

#define MAX_TRACK_BLOCKS 21
typedef struct track {
  TRACK_ID id;
  char *name;
  struct {
    int num_blocks;
    BLOCK_ID blocks[MAX_TRACK_BLOCKS];
  } cbtc;
  BOOL occupancy;
  struct {
    BOOL TLSR, TRSR;
    BOOL sTLSR, sTRSR;
    BOOL eTLSR, eTRSR;
    BOOL kTLSR, kTRSR;
  } locking;
} TRACK, *TRACK_PTR;

#define TRACK_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef TRACK_ATTRIB_DEFINITION

typedef enum route_kind {
  MAIN_ROUTE,
  ATP_SHUNTING_ROUTE,
  EMERGENCY_ROUTE,
  END_OF_ROUTE_KINDS
} ROUTE_KIND;

#define MAX_ROUTE_TRACKS 21
typedef struct _route {
  ROUTE_ID id;
  char *name;
  ROUTE_KIND kind;
  struct {
    int num_tracks;
    TRACK_ID tracks[MAX_ROUTE_TRACKS];
  } tr;
} ROUTE, *ROUTE_PTR;

#define ROUTE_ATTRIB_DEFINITION
#include "interlock_def.h"
#undef ROUTE_ATTRIB_DEFINITION

#define MAX_ROUTE_TRG_BLOCKS 21
typedef struct ars_route {
  //ROUTE route;
  ROUTE_ID route;
  struct {
    int num_blocks;
    TRACK_ID trg_blocks[MAX_ROUTE_TRG_BLOCKS];
  } trg_section;
} ARS_ROUTE, *ARS_ROUTE_PTR;

#define ARS_ROUTES_DEFINITIONS
#include "interlock_def.h"
#undef ARS_ROUTES_DEFINITIONS

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

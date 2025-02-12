#include "generic.h"
#include "misc.h"

typedef enum dock_detect_directive {
  DOCK_DETECT_MAJOR,
  DOCK_DETECT_MINOR
} DOCK_DETECT_DIRECTIVE;

extern STOPPING_POINT_CODE detect_train_docked ( ARS_SP_EVENTS *pev_sp, DOCK_DETECT_DIRECTIVE mode, TINY_TRAIN_STATE_PTR pT );
extern STOPPING_POINT_CODE detect_train_skip ( ARS_SP_EVENTS *pev_sp, TINY_TRAIN_STATE_PTR pT );

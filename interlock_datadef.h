#ifdef TRACK_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
#include "./cbi/il_tbl/interlock_dataset.h"
#else
extern TRACK track_dataset_def[];
#endif
#endif // TRACK_ATTRIB_DEFINITION

#ifdef ROUTE_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
#include "./cbi/il_tbl/interlock_dataset.h"
#else
extern ROUTE route_dataset_def[];
#endif
#endif // ROUTE_ATTRIB_DEFINITION

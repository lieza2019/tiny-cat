#ifndef IL_OBJ_INSTANCES_DECL
#define IL_OBJ_INSTANCES_DECL
typedef enum _il_obj_instances {
#define IL_OBJ_INSTANCE_DESC(route, raw_name, exp) exp,
#include "./il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
} IL_OBJ_INSTANCES;
#endif

#ifndef IL_OBJ_INSTANCES_DECL
#define IL_OBJ_INSTANCES_DECL

typedef enum _il_obj_instances {
#define IL_SYM_ATTRIB(kind, sym, str, code) sym=code
#define IL_OBJ_INSTANCE_DESC(route, raw_name, exp) exp,
#define IL_OBJ_INSTANCE_DESC1(route, raw_name, exp1) exp1,
#define IL_OBJ_INSTANCE_DESC2(route, raw_name, exp1, exp2) exp1, exp2,
#define IL_OBJ_INSTANCE_DESC3(route, raw_name, exp1, exp2, exp3) exp1, exp2, exp3,
#define IL_OBJ_INSTANCE_DESC4(route, raw_name, exp1, exp2, exp3, exp4) exp1, exp2, exp3, exp4,
#define IL_OBJ_INSTANCE_DESC5(route, raw_name, exp1, exp2, exp3, exp4, exp5) exp1, exp2, exp3, exp4, exp5,
#include "./il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
#undef IL_OBJ_INSTANCE_DESC1
#undef IL_OBJ_INSTANCE_DESC2
#undef IL_OBJ_INSTANCE_DESC3
#undef IL_OBJ_INSTANCE_DESC4
#undef IL_OBJ_INSTANCE_DESC5
#undef IL_SYM_ATTRIB
  END_OF_IL_OBJ_INSTANCES
} IL_OBJ_INSTANCES;

#endif // IL_OBJ_INSTANCES_DECL

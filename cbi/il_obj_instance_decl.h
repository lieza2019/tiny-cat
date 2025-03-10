#ifndef IL_OBJ_INSTANCES_DECL
#define IL_OBJ_INSTANCES_DECL

typedef enum il_sym {
#define IL_SYMS(sym_kind, sym, str, code) sym=code
#define IL_OBJ_INSTANCE_DESC0(stat_kind, raw_name, label, src_specifier)
#define IL_OBJ_INSTANCE_DESC(stat_kind, raw_name, label, src_specifier, exp) exp,
#define IL_OBJ_INSTANCE_DESC1(stat_kind, raw_name, label, src_specifier, exp1) exp1,
#define IL_OBJ_INSTANCE_DESC2(stat_kind, raw_name, label, src_specifier, exp1, exp2) exp1, exp2,
#define IL_OBJ_INSTANCE_DESC3(stat_kind, raw_name, label, src_specifier, exp1, exp2, exp3) exp1, exp2, exp3,
#define IL_OBJ_INSTANCE_DESC4(stat_kind, raw_name, label, src_specifier, exp1, exp2, exp3, exp4) exp1, exp2, exp3, exp4,
#define IL_OBJ_INSTANCE_DESC5(stat_kind, raw_name, label, src_specifier, exp1, exp2, exp3, exp4, exp5) exp1, exp2, exp3, exp4, exp5,
#include "./il_obj_instance_desc.h"
#undef IL_OBJ_INSTANCE_DESC
#undef IL_OBJ_INSTANCE_DESC1
#undef IL_OBJ_INSTANCE_DESC2
#undef IL_OBJ_INSTANCE_DESC3
#undef IL_OBJ_INSTANCE_DESC4
#undef IL_OBJ_INSTANCE_DESC5
#undef IL_SYMS
  END_OF_IL_SYMS
} IL_SYM;

#endif // IL_OBJ_INSTANCES_DECL

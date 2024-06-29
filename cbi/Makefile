MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY
GEN_IL_DEF_NAME = gen_il_def
GEN_IL_DEF_BIN = $(GEN_IL_DEF_NAME)

$(GEN_IL_DEF_BIN) : cbi.o gen_il_def.o
	$(CC) $(CFLAGS) $^ -o $@

il_obj_instance_decl.h: ./il_obj_instance_desc.h
	$(TOUCH) $@
../cbi.h: ../generic.h ../misc.h ../network.h ./cbi_stat_kind.def ./cbi_pat_def.h ./il_obj_instance_decl.h
	$(TOUCH) $@

cbi.o : ../generic.h ../misc.h ../cbi.h ../cbi.c
	$(CD) ../; \
	$(MAKE) $@; \
	$(CP) ./$@ ./cbi

gen_il_def.o : ../cbi.h gen_il_def.c

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f $(GEN_IL_DEF_BIN)

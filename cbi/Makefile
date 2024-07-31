MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY -DIN_CBI_RESOURCEDIR
GEN_IL_DEF_NAME = gen_il_def
GEN_IL_DEF_BIN = $(GEN_IL_DEF_NAME)

$(GEN_IL_DEF_BIN) : ../cbi.o gen_il_def.o
	$(CC) $(CFLAGS) $^ -o $@

il_obj_instance_desc.h:
	$(TOUCH) $@
il_obj_instance_decl.h: il_obj_instance_desc.h
	$(TOUCH) $@
../cbi.h: ../generic.h ../misc.h ../network.h ./cbi_stat_kind.def ./cbi_pat.def ./il_obj_instance_decl.h
	$(TOUCH) $@

../cbi.o : ../generic.h ../misc.h ../cbi.h ../cbi.c
	$(TOUCH) ./cbi_stat_label.h
	$(TOUCH) ./il_obj_instance_desc.h
	$(CD) ../; \
	$(MAKE) $(@F); \
#	$(CP) ./$@ ./cbi

gen_il_def.o : ../cbi.h gen_il_def.c ./cbi_pat.def

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(GEN_IL_DEF_BIN)
	$(RM) -f ./cbi_stat_label.h
	$(RM) -f ./il_obj_instance_desc.h

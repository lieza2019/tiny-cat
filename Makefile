RM = rm
TOUCH = touch
CD = cd
CC = gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY
MAKE = make
TINY_NAME = tiny_cat
TINY_NAME_BIN = $(TINY_NAME)
GEN_IL_DEF_NAME = gen_il_def
GEN_IL_DEF_BIN = $(GEN_IL_DEF_NAME)

$(TINY_NAME_BIN) : misc.o network.o interlock.o train_cmd.o sparcs.o cbi.o train_ctrl.o main.o
	$(CC) $(CFLAGS) $^ -o $@

train_info.h : sparcs_def.h
	$(TOUCH) $@
train_cmd.h : sparcs_def.h
	$(TOUCH) $@
sparcs.h : generic.h misc.h train_cmd.h train_info.h sparcs_def.h
	$(TOUCH) $@
train_ctrl.h : generic.h misc.h sparcs.h
	$(TOUCH) $@
./cbi/cbi_stat_label.h :
	$(CD) ./cbi; \
	$(MAKE); \
	./$(GEN_IL_DEF_BIN)
./cbi/il_obj_instance_decl.h :
	$(CD) ./cbi; \
	$(MAKE); \
	./$(GEN_IL_DEF_BIN)
#cbi.h: generic.h misc.h network.h ./cbi/cbi_stat_kind.def ./cbi/cbi_pat_def.h ./cbi/cbi_stat_label.h ./cbi/il_obj_instance_decl.h
cbi.h: generic.h misc.h network.h ./cbi/cbi_stat_kind.def ./cbi/cbi_pat.def ./cbi/cbi_stat_label.h ./cbi/il_obj_instance_decl.h
	$(TOUCH) $@
interlock.h : generic.h misc.h interlock_def.h sparcs.h
	$(TOUCH) $@
misc.o : generic.h misc.h misc.c
network.o : generic.h misc.h network.h network.c
train_cmd.o : generic.h misc.h sparcs_def.h train_cmd.h train_cmd.c
sparcs.o : generic.h misc.h sparcs.h sparcs.c
train_ctrl.o : generic.h misc.h network.h sparcs.h train_ctrl.c
cbi.o: generic.h misc.h cbi.h ./cbi/cbi_stat_label.h ./cbi/cbi_stat_kind.def cbi.c
interlock.o : generic.h misc.h network.h cbi.h srv.h interlock.h interlock.c
main.o : generic.h misc.h network.h srv.h interlock.h sparcs.h main.c

.PHONY : clean
clean:
	$(CD) ./cbi; $(MAKE) clean
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f $(TINY_NAME_BIN)

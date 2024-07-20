RM = rm
TOUCH = touch
CD = cd
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY
MAKE = make
TINY_NAME = tiny_cat
TINY_NAME_BIN = $(TINY_NAME)
GEN_IL_DEF_NAME = gen_il_def
GEN_IL_DEF_BIN = $(GEN_IL_DEF_NAME)

$(TINY_NAME_BIN) : misc.o network.o sparcs.o train_cmd.o cbtc.o train_ctrl.o cbi.o interlock.o ars.o timetable.o main.o
	$(CC) $(CFLAGS) $^ -o $@

train_info.h : sparcs_def.h
	$(TOUCH) $@
train_cmd.h : sparcs_def.h
	$(TOUCH) $@
sparcs.h : generic.h misc.h train_cmd.h train_info.h sparcs_def.h
	$(TOUCH) $@
cbtc.h : generic.h misc.h cbtc_def.h
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
cbi.h: generic.h misc.h network.h ./cbi/cbi_stat_kind.def ./cbi/cbi_pat.def ./cbi/cbi_stat_label.h ./cbi/il_obj_instance_decl.h
	$(TOUCH) $@
interlock.h : generic.h misc.h cbtc.h interlock_def.h
	$(TOUCH) $@
ars.h : generic.h misc.h cbi.h
	$(TOUCH) $@
timetable.h : generic.h misc.h ars.h
	$(TOUCH) $@

misc.o : generic.h misc.h misc.c
network.o : generic.h misc.h network.h network.c
sparcs.o : generic.h misc.h sparcs.h sparcs.c
train_cmd.o : generic.h misc.h sparcs_def.h train_cmd.h train_cmd.c
cbtc.o : generic.h misc.h cbtc.h
train_ctrl.o : generic.h misc.h network.h sparcs.h train_ctrl.c
cbi.o: generic.h misc.h cbi.h ./cbi/cbi_stat_label.h ./cbi/cbi_stat_kind.def cbi.c
interlock.o : generic.h misc.h network.h cbi.h srv.h interlock.h interlock.c
ars.o : generic.h misc.h cbi.h interlock.h ars.h timetable.h
main.o : generic.h misc.h network.h srv.h interlock.h sparcs.h main.c
timetable.o : generic.h misc.h timetable.h

.PHONY : clean
clean:
	$(CD) ./cbi; $(MAKE) clean
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(TINY_NAME_BIN)

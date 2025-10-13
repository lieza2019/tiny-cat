RM = rm
TOUCH = touch
CD = cd

CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY
LD = /usr/bin/gcc
LDFLAGS =
AR = ar
ARFLAGS =

MAKE = make
TINY_LIB_NAME = libtiny.a
TINY_EXE_NAME = tiny-cat
GEN_IL_DEF_BIN = gen_il_def
GEN_IL_DATA_BIN = gen_il_data

$(TINY_EXE_NAME) : main.o ./timetable/y.tab.o ./timetable/lex.yy.o ./timetable/ttcreat.o ./timetable/ttc_cmd.o $(TINY_LIB_NAME)
	for oname in `$(AR) -t $(TINY_LIB_NAME)`; do if [ -f $${oname} ]; then $(RM) $${oname}; fi; done; \
	$(RM) ./cbi/il_tbl/interlock_dataset.h
	$(MAKE) $(TINY_LIB_NAME)
	$(MAKE) main.o
	$(MAKE) ./timetable/y.tab.o
	$(MAKE) ./timetable/lex.yy.o
	$(MAKE) ./timetable/ttcreat.o
	$(MAKE) ./timetable/ttc_cmd.o
	$(LD) $(LDFLAGS) -o $@ $^

$(TINY_LIB_NAME) : misc.o network.o sparcs.o train_cmd.o cbtc_datadef.o cbtc.o train_ctrl.o cbi.o interlock.o surveill.o ars.o timetable.o cbtc_dataset.o
	$(AR) r $@ $^

train_info.h : generic.h misc.h
	$(TOUCH) $@
train_cmd.h : generic.h misc.h
	$(TOUCH) $@
sparcs.h : generic.h misc.h network.h train_cmd.h train_info.h train_ctrl.h sparcs_def.h
	$(TOUCH) $@
cbtc.h : generic.h misc.h ars.h cbtc_datadef.h
	$(TOUCH) $@
train_ctrl.h : generic.h misc.h cbtc.h
	$(TOUCH) $@
./cbi/memmap/cbi_stat_label.h :
	$(CD) ./cbi/memmap; \
	$(MAKE); \
	./$(GEN_IL_DEF_BIN)
./cbi/memmap/il_obj_instance_desc.h : ./cbi/memmap/cbi_pat.def
	$(CD) ./cbi/memmap; \
	$(MAKE); \
	./$(GEN_IL_DEF_BIN)
./cbi/memmap/il_obj_instance_decl.h : ./cbi/memmap/il_obj_instance_desc.h
	$(TOUCH) $@
cbi.h: generic.h misc.h network.h ./cbi/memmap/cbi_stat_kind.def ./cbi/memmap/il_obj_instance_decl.h
	$(TOUCH) $@
./cbi/il_tbl/interlock_dataset.h : ./cbi/il_tbl/BCGN_TRACK.csv ./cbi/il_tbl/BCGN_ROUTEREL.csv ./cbi/il_tbl/BCGN_POINT.csv ./cbi/il_tbl/BCGN_SIGNAL.csv ./cbi/il_tbl/JLA_TRACK.csv ./cbi/il_tbl/JLA_ROUTEREL.csv ./cbi/il_tbl/JLA_POINT.csv ./cbi/il_tbl/JLA_SIGNAL.csv
	if [ ! -f $(TINY_LIB_NAME) ]; then ($(CD) ./cbi/il_tbl; $(MAKE) phony); else ($(CD) ./cbi/il_tbl; $(MAKE) interlock_dataset.h); fi
interlock_datadef.h : ./cbi/il_tbl/interlock_dataset.h
	$(TOUCH) $@
interlock.h : generic.h misc.h cbi.h cbtc.h interlock_datadef.h
	$(TOUCH) $@
surveill.h : generic.h misc.h
	$(TOUCH) $@
ars.h : generic.h misc.h cbi.h
	$(TOUCH) $@
timetable.h : generic.h misc.h sparcs.h cbtc.h interlock.h timetable_def.h
	$(TOUCH) $@
network.h : generic.h
	$(TOUCH) $@
misc.h : generic.h
	$(TOUCH) $@

misc.o : generic.h misc.h misc.c
network.o : generic.h misc.h network.h network.c
sparcs.o : generic.h misc.h sparcs.h sparcs.c
train_cmd.o : generic.h misc.h sparcs.h train_cmd.c
train_ctrl.o : generic.h misc.h network.h sparcs.h train_ctrl.c
cbi.o: generic.h misc.h cbi.h ./cbi/memmap/cbi_stat_kind.def ./cbi/memmap/cbi_stat_label.h ./cbi/memmap/il_obj_instance_desc.h cbi.c
interlock.o : generic.h misc.h network.h cbi.h srv.h interlock.h interlock.c
surveill.o : generic.h misc.h sparcs.h cbi.h interlock.h surveill.h surveill.c
ars.o : generic.h misc.h sparcs.h cbi.h ars.h surveill.h timetable.h ./timetable/ttcreat.h ars.c
cbtc_datadef.o : generic.h misc.h cbtc.h interlock.h sparcs.h cbtc_datadef.c
cbtc.o : generic.h misc.h cbtc.h interlock.h sparcs.h cbtc.c
timetable.o : generic.h misc.h timetable.h ./timetable/ttcreat.h timetable.c
cbtc_dataset.o : generic.h misc.h cbtc.h cbtc_dataset.c

main.o : generic.h misc.h network.h sparcs.h cbi.h interlock.h surveill.h timetable.h ./timetable/ttcreat.h srv.h main.c

./timetable/ttcreat.h : generic.h cbtc.h interlock.h timetable.h
	$(TOUCH) $@

./timetable/ttcreat.o : generic.h ./timetable/ttcreat.h ./timetable/ttcreat.c
	$(CD) ./timetable; \
	$(MAKE) CFLAGS='-DNO_EXEC_BINARY' ttcreat.o
./timetable/tcreat_cmd.o : generic.h ./timetable/ttcreat.h
	$(CD) ./timetable; \
	$(MAKE) CFLAGS='-DNO_EXEC_BINARY' tcreat_cmd.o
./timetable/y.tab.o : ./timetable/ttcreat.h ./timetable/ttc_par.y
	$(CD) ./timetable; \
	$(MAKE) CFLAGS='-DNO_EXEC_BINARY' y.tab.o
./timetable/lex.yy.o : ./timetable/ttcreat.h ./timetable/ttc_par.y
	$(CD) ./timetable; \
	$(MAKE) CFLAGS='-DNO_EXEC_BINARY' lex.yy.o

.PHONY : clean
clean:
	$(CD) ./cbi/memmap; $(MAKE) clean
	$(CD) ./cbi/il_tbl; $(MAKE) clean
	$(CD) ./timetable; $(MAKE) clean
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(TINY_LIB_NAME)
	$(RM) -f $(TINY_EXE_NAME)

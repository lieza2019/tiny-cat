MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY -DIN_CBI_RESOURCEDIR

TINY_LIB_NAME = libtiny.a
GEN_IL_TBL_BIN = gen_il_tbl

#$(GEN_IL_TBL_BIN) : ../../cbtc.o ../../train_ctrl.o ../../cbi.o par_il_tbl.o gen_il_tbl.o
#	$(CC) $(CFLAGS) $^ -o $@

$(GEN_IL_TBL_BIN) : par_il_tbl.o gen_il_tbl.o ../../$(TINY_LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $^

gen_il_tbl.o : ../../cbi.h gen_il_tbl.c
par_il_tbl.o : ./par_il_tbl.c

../../$(TINY_LIB_NAME) :
	$(CD) ../../; $(MAKE) $(TINY_LIB_NAME)

#../../cbtc.o : ../../generic.h ../../misc.h ../../cbtc.h ../../interlock.h ../../sparcs.h ../../cbtc.c
#	$(CD) ../../; \
#	$(MAKE) $(@F)
#../../train_ctrl.o : ../../generic.h ../../misc.h ../../network.h ../../sparcs.h ../../train_ctrl.c
#	$(CD) ../../; \`
#	$(MAKE) $(@F)
#../../cbi.o : ../../generic.h ../../misc.h ../../cbtc.h ../../cbi.h ../../cbi.c
#	$(CD) ../../; \
#	$(MAKE) $(@F)

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(GEN_IL_TBL_BIN)

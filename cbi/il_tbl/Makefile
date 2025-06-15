MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY -DIN_CBI_RESOURCEDIR

TINY_LIB_NAME = libtiny.a
GEN_IL_TBL_BIN = gen_il_tbl

$(GEN_IL_TBL_BIN) : par_il_tbl.o gen_il_tbl.o ../../$(TINY_LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $^

gen_il_tbl.o : ../../cbi.h gen_il_tbl.c
par_il_tbl.o : ./par_il_tbl.c

../../$(TINY_LIB_NAME) :
	$(CD) ../../; $(MAKE) $(TINY_LIB_NAME)

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(GEN_IL_TBL_BIN)

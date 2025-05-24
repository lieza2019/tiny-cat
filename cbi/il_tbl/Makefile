MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY -DIN_CBI_RESOURCEDIR
GEN_IL_TBL_BIN = gen_il_tbl

$(GEN_IL_TBL_BIN) : gen_il_tbl.o
	$(CC) $(CFLAGS) $^ -o $@

gen_il_tbl.o : ../../cbi.h gen_il_tbl.c

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(GEN_IL_TBL_BIN)

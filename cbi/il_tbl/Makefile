MAKE = make
RM = rm
CP = cp
CD = cd
TOUCH = touch
CC = /usr/bin/gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY -DIN_CBI_RESOURCEDIR

TINY_LIB_NAME = libtiny.a
GEN_IL_DATA_BIN = gen_il_data

IL_DATASET_NIL = interlock_dataset.h.nil
IL_DATASET_H_PROTO = interlock_dataset0.h
IL_DATASET_H = interlock_dataset.h

$(GEN_IL_DATA_BIN) : par_il_tbl.o gen_il_data.o ../../$(TINY_LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $^

gen_il_data.o : ../../cbi.h gen_il_data.c
par_il_tbl.o : ./par_il_tbl.c

../../$(TINY_LIB_NAME) :
	$(CD) ../../; $(MAKE) $(TINY_LIB_NAME)

$(IL_DATASET_H) : $(GEN_IL_DATA_BIN)
	./$(GEN_IL_DATA_BIN)
	if ! (diff ./$(IL_DATASET_H_PROTO) ./$(IL_DATASET_H)); then $(CP) $(IL_DATASET_H_PROTO) $(IL_DATASET_H); fi

.PHONY: phony
phony:
	$(RM) -f $(IL_DATASET_H); $(CP) $(IL_DATASET_NIL) $(IL_DATASET_H)	

.PHONY: clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f $(GEN_IL_DATA_BIN)
	$(RM) -f $(IL_DATASET_H_PROTO) $(IL_DATASET_H)

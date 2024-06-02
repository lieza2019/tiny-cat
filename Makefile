RM = rm
TOUCH = touch
CC = gcc
CFLAGS = -Wall -Wno-unused-value  -g -DCHK_STRICT_CONSISTENCY
TINY_NAME = tiny_cat
TINY_NAME_BIN = $(TINY_NAME)

$(TINY_NAME_BIN) : misc.o network.o interlock.o train_cmd.o sparcs.o train_ctrl.o main.o
	$(CC) $(CFLAGS) $^ -o $@

train_info.h : sparcs_def.h
	$(TOUCH) $@
train_cmd.h : sparcs_def.h
	$(TOUCH) $@
sparcs.h : generic.h misc.h train_cmd.h train_info.h sparcs_def.h
	$(TOUCH) $@
train_ctrl.h : generic.h misc.h sparcs.h
	$(TOUCH) $@
interlock.h : generic.h misc.h interlock_def.h sparcs.h
	$(TOUCH) $@
misc.o : generic.h misc.h misc.c
network.o : generic.h misc.h network.h network.c
train_cmd.o : generic.h misc.h train_cmd.h train_cmd.c
sparcs.o : generic.h misc.h sparcs.h sparcs.c
train_ctrl.o : generic.h misc.h sparcs.h train_ctrl.h train_ctrl.c
interlock.o : generic.h misc.h srv.h interlock.h interlock.c
main.o : generic.h misc.h network.h srv.h interlock.h sparcs.h main.c

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f $(TINY_NAME_BIN)

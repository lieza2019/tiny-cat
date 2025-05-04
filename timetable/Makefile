EXE_SUFFIX = .exe

RM = rm
TOUCH = touch
CD = cd

YACC = bison
YFLAGS = -dy -Wconflicts-sr -Wconflicts-rr -Wcounterexamples -Wother
LEX = flex
LFLAGS = -l

AR = ar
ARFLAGS =
CC = /usr/bin/gcc
CFLAGS = -c -Wall -g
LD = /usr/bin/gcc
LDFLAGS = -L../

TINY_LIB_NAME = libtiny.a

MAKE = make
TINY_LIB_NAME = libtiny.a
TINY_EXE_NAME = tiny-cat
TTCREAT_EXE_NAME = ttcreat

ttcreat : y.tab.o lex.yy.o ttcreat.o ttcreat_def.o ttcreat_cmd.o ../$(TINY_LIB_NAME)
	$(LD) -o $(TTCREAT_EXE_NAME) $^

../$(TINY_LIB_NAME) :
	$(CD) ../; $(MAKE)

y.tab.o : y.tab.c
	$(CC) -c $<

y.tab.c : ttcreat_par.y ttcreat.h
	$(YACC) $(YFLAGS) $<

lex.yy.o : lex.yy.c
	$(CC) -c $<

lex.yy.c : ttcreat_lex.l ttcreat.h
	$(LEX) $(LFLAGS) $<

ttcreat.o : ttcreat.c ttcreat.h
	$(CC) $(CFLAGS) $<

ttcreat_def.o : ttcreat_def.c ttcreat.h
	$(CC) $(CFLAGS) $<

ttcreat_cmd.o : ttcreat_cmd.c ttcreat.h
	$(CC) $(CFLAGS) $<

ttcreat.h : ttcreat_def.h ttcreat_par.h ../$(TINY_LIB_NAME)
	$(TOUCH) $@

.PHONY : clean
clean:
	$(CD) ./tstsrc && $(MAKE) clean
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f y.tab.c lex.yy.c y.tab.h
	$(RM) -f $(TTCREAT_EXE_NAME)

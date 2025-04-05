EXE_SUFFIX = .exe

RM = rm
TOUCH = touch
CD = cd

YACC = bison
YFLAGS = -dy -Wconflicts-sr -Wconflicts-rr -Wcounterexamples -Wother
LEX = flex

LFLAGS = -l
CC = /usr/bin/gcc
CFLAGS = -c -Wall -g
LD = /usr/bin/gcc
LDFLAGS = -L../

TINY_DLL_NAME = libtiny.so

MAKE = make
TINY_DLL_NAME = libtiny.so
TINY_EXE_NAME = tiny-cat
TTCREAT_EXE_NAME = ttcreat

ttcreat : y.tab.o lex.yy.o ttcreat.o ../$(TINY_DLL_NAME)
	$(LD) -o $(TTCREAT_EXE_NAME) y.tab.o lex.yy.o ttcreat.o $(LDFLAGS) -ltiny

../$(TINY_DLL_NAME) : ../$(TINY_EXE_NAME)$(EXE_SUFFIX)
	$(TOUCH) $@

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

ttcreat.h : ../$(TINY_EXE_NAME)$(EXE_SUFFIX)
	$(TOUCH) $@

../$(TINY_EXE_NAME)$(EXE_SUFFIX) :
	$(CD) ../; $(MAKE);

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

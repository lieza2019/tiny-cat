TTCREAT_BINNAME = ttcreat

YACC = bison
YFLAGS = -dy -Wconflicts-sr -Wconflicts-rr -Wcounterexamples -Wother
LEX = flex
LFLAGS = -l

LD = gcc
LDFLAGS = -o $(TTCREAT_BINNAME)
CC = gcc
CFLAGS = -c -Wall -g

MAKE = make
TOUCH = touch
MV = mv
CP = cp
CD = cd
RM = rm

ttcreat : y.tab.o lex.yy.o helpers.o
	$(LD) $(LDFLAGS) $^

y.tab.o : y.tab.c
	$(CC) -c $<

y.tab.c : ttcreat.y ttcreat.h
	$(YACC) $(YFLAGS) $<

lex.yy.o : lex.yy.c
	$(CC) -c $<

lex.yy.c : ttcreat.l ttcreat.h
	$(LEX) $(LFLAGS) $<

helpers.o : helpers.c ttcreat.h
	$(CC) $(CFLAGS) $<

ttcreat.h : ../generic.h
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
	$(RM) -f $(TTCREAT_BINNAME)

TTCREAT_BINNAME = ttcreat

YACC = bison
YFLAGS = -dy
LEX = flex
LFLAGS = -l

LD = gcc
LDFLAGS = -o $(TTCREAT_BINNAME)
CC = gcc
CFLAGS = -c -Wall -g

TOUCH = touch
MV = mv
CP = cp
CD = cd
RM = rm

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f y.tab.c lex.yy.c y.tab.h
	$(RM) -f $(TTCREAT_BINNAME)

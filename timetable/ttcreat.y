%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ttcreat.h"
  
#define YYDEBUG 1
%}
%union {
  struct {
    KIND kind;
    char *name;
  } signal;
  char *sp;
}
%token <signal> MAIN_SIGNAL VIRT_SIGNAL
%token SP
%%
signal : MAIN_SIGNAL { printf( "(kind, sig_name): (%d, %s)", $1.kind, $1.name ); }
       | VIRT_SIGNAL { printf( "(kind, sig_name): (%d, %s)", $1.kind, $1.name ); }
%%
int yyerror ( const char *s ) {
  extern char *yytext;
  printf( "parse error near %s.\n", yytext );
  return 1;
}

int main ( void ) {
  extern int yyparse( void );
  extern FILE *yyin;
  
  yyin = stdin;
  if( yyparse() ) {
    return 1;
  }
  return 0;
}

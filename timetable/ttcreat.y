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
    char name[MAX_ROUTENAME_LEN];
  } rout;
  char sp[MAX_SPNAME_LEN];
}
%token <rout> ROUTE
%token <sp> SP
%type <rout> route
%start route
%%
route : ROUTE {
  $$ = $1;
  printf( "(kind, name): (%d, %s)\n", $$.kind, $$.name );  
 }
;
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

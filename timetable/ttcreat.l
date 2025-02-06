%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ttcreat.h"

#include "y.tab.h"
int yywrap( void ) {
  return 1;
}
%}
%%
"SP_"[0-9A-Z][0-9] {
  strncpy( yylval.sp, yytext, MAX_SPNAME_LEN );
  return SP;
}

"S"[0-9][0-9][0-9][A-Z]"_S"[0-9][0-9][0-9][A-Z] {
  yylval.kind = R;
  strncpy( yylval.name, yytext, MAX_ROUTENAME_LEN );
  return ROUTE;
}
%%

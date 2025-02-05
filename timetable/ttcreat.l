%{
#include <stdio.h>
#include "ttcreat.h"

#include "y.tab.h"
int yywrap( void ) {
  return 1;
}
%}
%%
SP_[0-9A-Z][0-9] {
  yylval.sp = yytext;
  return SP;
}
"S"[0-9][0-9][0-9][A-Z] {
  yylval.signal.kind = MSIG;
  yylval.signal.name = yytext;
  return MAIN_SIGNAL;
}
"VS"[0-9][0-9][0-9][A-Z] {
  yylval.signal.kind = VSIG;
  yylval.signal.name = yytext;
  return VIRT_SIGNAL;
}
%%

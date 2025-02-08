%{
#include <stdio.h>
#include "ttcreat.h"

#include "y.tab.h"
int yywrap( void ) {
  return 1;
}
%}
%%
";" { return ';'; }
":" { return ':'; }
"{" { return '{'; }
"}" { return '}'; }
"(" { return '('; }
")" { return ')'; }
"," { return ','; }

"JLA" { strncpy( yylval.st_name, yytext, MAX_STNAME_LEN); return TK_STNAME; }
"KIKJ" { strncpy( yylval.st_name, yytext, MAX_STNAME_LEN); return TK_STNAME; }
"OKBS" { strncpy( yylval.st_name, yytext, MAX_STNAME_LEN); return TK_STNAME; }
"BTGD" { strncpy( yylval.st_name, yytext, MAX_STNAME_LEN); return TK_STNAME; }

"PL1" { strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN); return TK_PLTB_NAME; }
"PL2" { strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN); return TK_PLTB_NAME; }
"PL3" { strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN); return TK_PLTB_NAME; }
"TB1" { strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN); return TK_PLTB_NAME; }
"TB2" { strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN); return TK_PLTB_NAME; }

"trips" { return TK_KEY_TRIPS; }

"SP_"[0-9A-Z][0-9] {
  strncpy( yylval.sp, yytext, MAX_SPNAME_LEN );
  return TK_SP;
 }

("S"|"VS")[0-9][0-9][0-9][A-Z]"_"("S"|"VS")[0-9][0-9][0-9][A-Z] {
  yylval.attr_route.kind = ROUTE;
  strncpy( yylval.attr_route.name, yytext, MAX_ROUTENAME_LEN );
  return TK_ROUTE;
 }
%%

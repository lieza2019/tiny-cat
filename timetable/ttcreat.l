%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

"perfslow" { yylval.perf_regime = PERFREG_SLOW; return TK_PERFREG; }
"perfnormal" { yylval.perf_regime = PERFREG_NORMAL; return TK_PERFREG; }
"perffast" { yylval.perf_regime = PERFREG_FAST; return TK_PERFREG; }

"trips" { return TK_KEY_TRIPS; }
"assignments" { return TK_KEY_ASSIGNMENTS; }

":=" { return TK_ASGN; }

[0-9]+ {
  yylval.nat = atoi( yytext );
  return TK_NAT;
}

("J"|"j")[0-9]+ {
  assert( (yytext[0] == 'J') || (yytext[0] == 'j') );
  yylval.journey_id = atoi( &yytext[1] );
  return TK_JOURNEY_ID;
}

"rake_"[0-9][0-9][0-9] {
  assert( (yytext[0] = 'r') && (yytext[1] = 'a') && (yytext[2] = 'k') && (yytext[3] = 'e') && (yytext[4] = '_') );
  yylval.rake_id = atoi( &yytext[5]);
  return TK_RAKE_ID;
}

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

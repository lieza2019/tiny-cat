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

static BOOL chk_time ( ATTR_TIME_PTR ptime, const char *phour, const char *pmin, const char *psec ) {
  assert( ptime );
  assert( phour );
  assert( pmin );
  assert( psec );
  BOOL r = FALSE;
  int hour;
  int min;
  int sec;
#if 0 // ***** for debugging.
  printf( "(hour, min, sec): (%s, %s, %s)\n", phour, pmin, psec );
#endif
  
  hour = atoi( phour );
  if( (hour >= 0) && (hour <= 23) ) {
    min = atoi( pmin );
    if( (min >= 0) && (min <= 59) ) {
      sec = atoi( psec );
      if( (sec >= 0) && (sec <= 59) ) {
	ptime->hour = hour;
	ptime->min = min;
	ptime->sec = sec;
	r = TRUE;
      }
    }
  }
  return r;
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

"revenue" {yylval.revenue = TRUE; return TK_REVENUE; }
"nonreve" {yylval.revenue = FALSE; return TK_REVENUE; }

"trips" { return TK_KEY_TRIPS; }
"assignments" { return TK_KEY_ASSIGNMENTS; }
"journeys" { return TK_JOURNEYS; }

":=" { return TK_ASGN; }

[0-9]+ {
  yylval.nat = atoi( yytext );
  return TK_NAT;
}

[0-9][0-9]":"[0-9][0-9]":"[0-9][0-9] {
  /* {(0|1|2)[0-9]":"[0-5][0-9]":"[0-5][0-9] */  
  assert( (yytext[2] == ':') && (yytext[5] == ':') );
  ATTR_TIME time = {-1, -1, -1};
  yytext[2] = 0;
  yytext[5] = 0;
  if( chk_time( &time, yytext, &yytext[3], &yytext[6] ) ) { 
    assert( (time.hour >= 0) && (time.hour <= 23) );
    assert( (time.min >= 0) && (time.min <= 59) );
    assert( (time.sec >= 0) && (time.sec <= 59) );
    yylval.attr_time = time;
    return TK_TIME;
  } else
    printf( "invalid time specifier, %s:%s:%s.\n", yytext, &yytext[3], &yytext[6] );
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

"crew_"[0-9]+ {
  assert( (yytext[0] == 'c') && (yytext[1] == 'r') && (yytext[2] == 'e') && (yytext[3] == 'w') && (yytext[4] == '_') );
  yylval.crew_id = atoi( &yytext[5] );
  return TK_CREWID;
}
%%

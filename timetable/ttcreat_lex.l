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

static BOOL chk_time ( TINY_TIME_DESC_PTR ptime, const char *phour, const char *pmin, const char *psec ) {
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
	ptime->minute = min;
	ptime->second = sec;
	r = TRUE;
      }
    }
  }
  return r;
}
%}
%%
";" {
  yylloc.first_column++;
  return ';';
}
":" {
  yylloc.first_column++;
  return ':';
}
"{" {
  yylloc.first_column++;
  return '{';
}
"}" {
  yylloc.first_column++;
  return '}';
}
"(" {
  yylloc.first_column++;
  return '(';
}
")" {
  yylloc.first_column++;
  return ')';
}
"," {
  yylloc.first_column++;
  return ',';
}

"JLA" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.st_name, yytext, MAX_STNAME_LEN);
  return TK_STNAME;
}
"KIKJ" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.st_name, yytext, MAX_STNAME_LEN);
  return TK_STNAME;
}
"OKBS" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.st_name, yytext, MAX_STNAME_LEN);
  return TK_STNAME;
}
"BTGD" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.st_name, yytext, MAX_STNAME_LEN);
  return TK_STNAME;
}

"PL1" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN);
  return TK_PLTB_NAME;
}
"PL2" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN);
  return TK_PLTB_NAME;
}
"PL3" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN);
  return TK_PLTB_NAME;
}
"TB1" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN);
  return TK_PLTB_NAME;
}
"TB2" {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.pltb_name, yytext, MAX_PLTB_NAMELEN);
  return TK_PLTB_NAME;
}

"perfslow" {
  yylloc.first_column += strlen( yytext );
  yylval.perf_regime = PERFREG_SLOW;
  return TK_PERFREG;
}
"perfnormal" {
  yylloc.first_column += strlen( yytext );
  yylval.perf_regime = PERFREG_NORMAL;
  return TK_PERFREG;
}
"perffast" {
  yylloc.first_column += strlen( yytext );
  yylval.perf_regime = PERFREG_FAST;
  return TK_PERFREG;
}

"revenue" {
  yylloc.first_column += strlen( yytext );
  yylval.revenue = TRUE;
  return TK_REVENUE;
}
"nonreve" {
  yylloc.first_column += strlen( yytext );
  yylval.revenue = FALSE;
  return TK_REVENUE;
}
"sp_assignments" {
  yylloc.first_column += strlen( yytext );
  return TK_SP_ASGNS;
}
"trips" {
  yylloc.first_column += strlen( yytext );
  return TK_KEY_TRIPS;
}
"assignments" {
  yylloc.first_column += strlen( yytext );
  return TK_KEY_ASSIGNMENTS;
}
"journeys" {
  yylloc.first_column += strlen( yytext );
  return TK_JOURNEYS;
}

":=" {
  yylloc.first_column += strlen( yytext );
  return TK_ASGN;
}

[0-9]+ {
  yylloc.first_column += strlen( yytext );
  yylval.nat = atoi( yytext );
  return TK_NAT;
}

[0-9][0-9]":"[0-9][0-9]":"[0-9][0-9] {
  /* {(0|1|2)[0-9]":"[0-5][0-9]":"[0-5][0-9] */  
  assert( (yytext[2] == ':') && (yytext[5] == ':') );
  assert( strlen(yytext) == 8 );
  TINY_TIME_DESC time = {-1, -1, -1, -1, -1, -1};
  yytext[2] = 0;
  yytext[5] = 0;
  yylloc.first_column += 8;
  if( chk_time( &time, yytext, &yytext[3], &yytext[6] ) ) { 
    assert( (time.hour >= 0) && (time.hour <= 23) );
    assert( (time.minute >= 0) && (time.minute <= 59) );
    assert( (time.second >= 0) && (time.second <= 59) );
    yylval.attr_time.t = time;
    return TK_TIME;
  } else
    printf( "invalid time specifier, %s:%s:%s.\n", yytext, &yytext[3], &yytext[6] );
}

("J"|"j")[0-9]+ {
  assert( (yytext[0] == 'J') || (yytext[0] == 'j') );  
  yylval.journey_id = atoi( &yytext[1] );
  yylloc.first_column += strlen( yytext );
  return TK_JOURNEY_ID;
}

"rake_"[0-9][0-9][0-9] {
  assert( (yytext[0] = 'r') && (yytext[1] = 'a') && (yytext[2] = 'k') && (yytext[3] = 'e') && (yytext[4] = '_') );
  yylval.rake_id = atoi( &yytext[5]);
  yylloc.first_column += strlen( yytext );
  return TK_RAKE_ID;
}

"SP_"[0-9A-Z][0-9] {
  yylloc.first_column += strlen( yytext );
  strncpy( yylval.sp, yytext, MAX_SPNAME_LEN );
  return TK_SP;
 }

("S"|"VS")[0-9][0-9][0-9][A-Z]"_"("S"|"VS")[0-9][0-9][0-9][A-Z] {  
  yylloc.first_column += strlen( yytext );
  yylval.attr_route.kind = PAR_ROUTE;
  strncpy( yylval.attr_route.name, yytext, MAX_ROUTENAME_LEN );
  return TK_ROUTE;
 }

"crew_"[0-9]+ {
  assert( (yytext[0] == 'c') && (yytext[1] == 'r') && (yytext[2] == 'e') && (yytext[3] == 'w') && (yytext[4] == '_') );
  yylval.crew_id = atoi( &yytext[5] );
  yylloc.first_column += strlen( yytext );
  return TK_CREWID;
}

[ \t] { yylloc.first_column++; }
"\n" { yylloc.first_line++; yylloc.first_column = 1; }

 /* <<eof>> { return TK_EOF; } */

. {
  yylloc.first_column++;
  return yytext[0];
}
%%

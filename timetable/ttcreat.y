%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ttcreat.h"

#define YYDEBUG 1

static void print_st_pltb ( ATTR_ST_PLTB_PTR pst_pltb ) {
  assert( pst_pltb );
  char st_pltb_strbuf[(MAX_STNAME_LEN + 1 + MAX_PLTB_NAMELEN) + 1] = "";
  
  strncat( st_pltb_strbuf, pst_pltb->st_name, MAX_STNAME_LEN );
  strncat( st_pltb_strbuf, "_", 2 );
  strncat( st_pltb_strbuf, pst_pltb->pltb_name, MAX_PLTB_NAMELEN );
  printf( "%s", st_pltb_strbuf );
}

static void print_sp_pair ( ATTR_SP_PAIR_PTR psps ) {
  assert( psps );
  printf( "(%s, %s)", psps->sp_org, psps->sp_dst );
}

static void print_routes ( ATTR_ROUTES_PTR proutes ) {
  assert( proutes );
  int i = 0;
  printf( "{" );
  while( i < proutes->nroutes ) {
    printf( "%s", proutes->route_prof[i].name );
    i++;
    if( i < proutes->nroutes )
      printf( ", " );
  }
  printf( "}" );
}

static void print_trip ( ATTR_TRIP *ptrip ) {
  assert( ptrip );
  
  printf( "(%d, ", ptrip->kind );
  
  printf( "(" );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_org );
  printf( ", " );
  print_st_pltb( &ptrip->attr_st_pltb_orgdst.st_pltb_dst );
  printf( "), " ); 
  
  print_sp_pair( &ptrip->attr_sp_orgdst );
  printf( ", " );
  print_routes( &ptrip->attr_route_ctrl );
  printf( ")" );
}
%}
%union {
  char st_name[MAX_STNAME_LEN];
  char pltb_name[MAX_PLTB_NAMELEN];
  ATTR_ST_PLTB attr_st_pltb;
  char sp[MAX_SPNAME_LEN];
  ATTR_SP_PAIR attr_sp_pair;
  ATTR_ROUTE attr_route;
  ATTR_ROUTES attr_routes;  
  ATTR_TRIP attr_trip;
  //ATTR_TRIPS attr_trips;
}
%token <st_name> TK_STNAME
%token <pltb_name> TK_PLTB_NAME
%type <attr_st_pltb> st_and_pltb
%token <sp> TK_SP
%type <attr_sp_pair> sp_orgdst_pair
%token <attr_route> TK_ROUTE
%type <attr_route> route
%type <attr_routes> routes routes0
%type <attr_trip> trip
%start trip
%%
/*trips : trip {
  $$.ntrips = 1;
  $$.trip_prof[0] = $1;
 }
      | trip ';' trips {
  $$.kind = TRIPS;
  {
    int i;
    for( i = 0; i < $3.ntrips; i++ )
      $$.trip_prof[i + 1] = $3.trip_prof[i];
  }
  $$.trip_prof[0] = $1;
  $$.ntrips = $3.ntrips + 1;
 }
 ; */
trip : '(' '('st_and_pltb ',' st_and_pltb')' ',' sp_orgdst_pair ',' '{' routes '}' ')' {
  $$.kind = TRIP;
  $$.attr_st_pltb_orgdst.kind = ST_PLTB_PAIR;
  $$.attr_st_pltb_orgdst.st_pltb_org = $3;
  $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
  $$.attr_sp_orgdst = $8;
  $$.attr_route_ctrl = $11;
#if 1
  printf( "(kind, st_pltb_pair, sp_orgdst_pair, trip_routes): (%d, ", $$.kind );
  {
    printf( "(" );
    print_st_pltb( &$$.attr_st_pltb_orgdst.st_pltb_org );
    printf( ", " );
    print_st_pltb( &$$.attr_st_pltb_orgdst.st_pltb_dst );
    printf( ")" );
    printf( ", " );
  }
  print_sp_pair( &$$.attr_sp_orgdst );
  printf( ", " );
  print_routes( &$$.attr_route_ctrl );
  printf( ")\n" );
#else
  printf( "(kind, st_pltb_pair, sp_orgdst_pair, trip_routes): " );
  print_trip( &$$ );
  printf( "\n" );
#endif
 }
;
st_and_pltb : '(' TK_STNAME ',' TK_PLTB_NAME ')' {
  $$.kind = ST_PLTB;
  strncpy( $$.st_name, $2, MAX_STNAME_LEN );
  strncpy( $$.pltb_name, $4, MAX_PLTB_NAMELEN );  
  //print_st_pltb( &$$ );
 }
;
sp_orgdst_pair : '(' TK_SP ',' TK_SP ')' {
  $$.kind = SP_PAIR;
  strncpy( $$.sp_org, $2, MAX_SPNAME_LEN );
  strncpy( $$.sp_dst, $4, MAX_SPNAME_LEN );
  //print_sp_pair( &$$ );
 }
;
routes0 : '{' routes '}' {
  printf( "routes: " );
  print_routes( &$2 );
  printf( "\n" );
 }
;
routes : route {
  $$.kind = ROUTES;
  $$.nroutes = 1;
  $$.route_prof[0] = $1;
  /* printf( "route: %s\n", $$.route_prof[0].name ); */
 }
       | route ',' routes {
  $$.kind = ROUTES;
  {
    int i;
    for( i = 0; i < $3.nroutes; i++ ) {
      $$.route_prof[i + 1] = $3.route_prof[i];
    }
    $$.route_prof[0] = $1;
  }
  $$.nroutes = $3.nroutes + 1;
 }
;
route : TK_ROUTE {
  $$.kind = $1.kind;
  strncpy( $$.name, $1.name, MAX_ROUTENAME_LEN );
  /* printf( "(kind, route): (%d, %s)\n", $$.kind, $$.name ); */
 }
;
%%
int yyerror ( const char *s ) {
#if 0
  extern char *yytext;
  printf( "parse error near %s.\n", yytext );
#endif
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

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

static void print_trip ( ATTR_TRIP_PTR ptrip ) {
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

ATTR_TRIPS trips_regtbl = { TRIPS };
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
  ATTR_TRIPS_PTR pattr_trips;
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
%token TK_KEY_TRIPS
%type <pattr_trips> trips trips_decl
%start trips_decl
%%
trips_decl : TK_KEY_TRIPS ':' trips {
  assert( $3 );
  assert( $3->kind == TRIPS );
  $$ = $3;
#if 1 /* ***** for debugging. */
  {
    assert( $3 );
    ATTR_TRIP_PTR p = $3->trip_prof;
    int i;
    for( i = 0; i < $3->ntrips; i++ ) {
      print_trip( &p[i] );
      printf( "\n" );
    }
  }
#else
  printf( "ntrips: %d\n", $3->ntrips );
#endif
 }
;
/* e.g.
   trips:
     (((JLA,PL1), (KIKJ, PL1)), (SP_73, SP_77), {S803B_S831B});
     (((KIKJ,PL1), (OKBS, PL1)), (SP_77, SP_79), {S831B_S821A});
     (((OKBS,PL1), (BTGD, PL1)), (SP_79, SP_81), {S821A_S801A, S801A_S803A});
     (((BTGD,PL1), (BTGD, TB1)), (SP_81, TB_D5), {S803A_S809A});
     (((BTGD,TB1), (BTGD, PL2)), (SP_D5, TB_80), {S806A_S804A});
     (((BTGD,PL2), (OKBS, PL2)), (SP_80, TB_78), {S804A_S822A});
     (((OKBS,PL2), (KIKJ, PL2)), (SP_78, TB_76), {S822A_S832B});
     (((KIKJ,PL2), (JLA, PL1)), (SP_76, TB_73), {S832B_S802B, S802B_S810B});
*/
trips : trip ';' {
  ATTR_TRIP_PTR p = NULL;
  p = reg_trip( &trips_regtbl, NULL, &$1 );
  if( p != &$1 ) {
    printf( "FATAL: INTERNAL-error, in trip definiton & registration, giving up.\n" );
    exit( 1 );
  }
  $$ = &trips_regtbl;
 }
      | trips trip ';' {
  ATTR_TRIP dropd = {};
  ATTR_TRIP_PTR p = NULL;
  p = reg_trip( &trips_regtbl, &dropd, &$2 );
  if( p != &$2 ) {
    assert( p == &dropd );
    assert( p->kind == TRIP );
    printf( "NOTICE: trip attribute has been overridden with.\n" );
  }
  $$ = &trips_regtbl;
 }
;
/* e.g. (((JLA,PL1), (KIKJ, PL1)), (SP_73, SP_77), {S803B_S831B}) */
trip : '(' '('st_and_pltb ',' st_and_pltb')' ',' sp_orgdst_pair ',' '{' routes '}' ')' {
  $$.kind = TRIP;
  $$.attr_st_pltb_orgdst.kind = ST_PLTB_PAIR;
  $$.attr_st_pltb_orgdst.st_pltb_org = $3;
  $$.attr_st_pltb_orgdst.st_pltb_dst = $5;
  $$.attr_sp_orgdst = $8;
  $$.attr_route_ctrl = $11;
#if 0 /* ***** for debugging. */
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
  /* print_st_pltb( &$$ ); // ***** for debugging. */
 }
;
sp_orgdst_pair : '(' TK_SP ',' TK_SP ')' {
  $$.kind = SP_PAIR;
  strncpy( $$.sp_org, $2, MAX_SPNAME_LEN );
  strncpy( $$.sp_dst, $4, MAX_SPNAME_LEN );
  /* print_sp_pair( &$$ ); // ***** for debugging. */
 }
;
routes0 : '{' routes '}' { /* its only for debugging. */
  printf( "routes: " );
  print_routes( &$2 );
  printf( "\n" );
 }
;
routes : route {
  $$.kind = ROUTES;
  $$.nroutes = 1;
  $$.route_prof[0] = $1;
  /* printf( "route: %s\n", $$.route_prof[0].name ); // ***** for debugging. */
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
  /* printf( "(kind, route): (%d, %s)\n", $$.kind, $$.name ); // ***** for debugging. */
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

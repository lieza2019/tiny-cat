#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

#define MAX_STNAME_LEN 8
#define MAX_PLTB_NAMELEN 8
#define MAX_ROUTENAME_LEN 16
#define MAX_SPNAME_LEN 8
#define MAX_TRIP_ROUTES 8
#define MAX_TRIPS 256

typedef enum kind {
  ST_PLTB,
  ST_PLTB_PAIR,
  SP,
  SP_PAIR,
  ROUTE,
  ROUTES,
  TRIP,
  TRIPS
} KIND;

typedef struct attr_sp_pair {
  KIND kind;
  char sp_org[MAX_SPNAME_LEN];
  char sp_dst[MAX_SPNAME_LEN];
} ATTR_SP_PAIR, *ATTR_SP_PAIR_PTR;

typedef struct attr_st_pltb {
  KIND kind;
  char st_name[MAX_STNAME_LEN];
  char pltb_name[MAX_PLTB_NAMELEN];
} ATTR_ST_PLTB, *ATTR_ST_PLTB_PTR;
typedef struct attr_st_pltb_pair {
  KIND kind;
  ATTR_ST_PLTB st_pltb_org;
  ATTR_ST_PLTB st_pltb_dst;
} ATTR_ST_PLTB_PAIR, *ATTR_ST_PLTB_PAIR_PTR;

typedef struct attr_route {
  KIND kind;
  char name[MAX_ROUTENAME_LEN];
} ATTR_ROUTE, *ATTR_ROUTE_PTR;
typedef struct attr_routes {
  KIND kind;
  int nroutes;
  ATTR_ROUTE route_prof[MAX_TRIP_ROUTES];
} ATTR_ROUTES, *ATTR_ROUTES_PTR;

typedef struct attr_trip {
  KIND kind;
  ATTR_ST_PLTB_PAIR attr_st_pltb_orgdst;
  ATTR_SP_PAIR attr_sp_orgdst;
  ATTR_ROUTES attr_route_ctrl;
} ATTR_TRIP, *ATTR_TRIP_PTR;
typedef struct attr_trips {
  KIND kind;
  int ntrips;
  ATTR_TRIP trip_prof[MAX_TRIPS];
} ATTR_TRIPS, *ATTR_TRIPS_PTR;

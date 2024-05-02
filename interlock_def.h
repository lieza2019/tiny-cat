#ifdef ROUTE_AND_TRACK_ID_DEFINITIONS
typedef enum route_id {
  S111_S222,
  END_OF_ROUTES
} ROUTE_ID;

typedef enum track_id {
  T109A,
  T110A,
  T111A,
  T112A,
  T113A,
  T114A,
  END_OF_TRACKS
} TRACK_ID;
#endif /* ROUTE_AND_TRACK_ID_DEFINITIONS */

#ifdef TRACK_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
TRACK track_state[] = {
  {T109A, "T109A"},
  {T110A, "T110A"},
  {T111A, "T111A"},
  {T112A, "T112A"},
  {T113A, "T113A"},
  {T114A, "T114A"},
  {END_OF_TRACKS, "END_OF_TRACKS"}
};
#else
extern TRACK track_state[];
#endif
#endif /* TRACK_ATTRIB_DEFINITION */

#ifdef ROUTE_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
ROUTE route_state[] = {
  {S111_S222, "S111_S222", MAIN_ROUTE, {4, {T111A, T112A, T113A, T114A}}},
  {END_OF_ROUTES, "END_OF_ROUTES" , END_OF_ROUTE_KINDS}
};
#else
extern ROUTE route_state[];
#endif
#endif /* ROUTE_ATTRIB_DEFINITION */

#ifdef ARS_ROUTES_DEFINITIONS
#ifdef INTERLOCK_C
ARS_ROUTE ars_routes[] = {
#if 0
  {{ S111_S222, "S111_S222", MAIN_ROUTE, {4, {T111A, T112A, T113A, T114A}}}, {2, {T109A, T110A}}},
  {{ END_OF_ROUTES, "END_OF_ROUTES", END_OF_ROUTE_KINDS, {0} }, {0}}
#else
  {S111_S222, {2, {T109A, T110A}}},
  {END_OF_ROUTES}
#endif
};
#else
extern ARS_ROUTE ars_routes[];
#endif
#endif /* ARS_ROUTES_DEFINITIONS */

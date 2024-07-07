#ifdef ROUTE_AND_TRACK_ID_DEFINITIONS
typedef enum track_id {
  T109A,
  T110A,
  T111A,
  T112A,
  T113A,
  T114A,
  END_OF_TRACKS
} TRACK_ID;
#endif // ROUTE_AND_TRACK_ID_DEFINITIONS

#ifdef TRACK_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
TRACK track_state[] = {
  { _TRACK, T801A_TR, "T801A_TR", {}, {{}, {}, {}, {}} },
  { _TRACK, T803A_TR, "T803A_TR", {}, {{}, {}, {}, {}} },
  { _TRACK, T805A_TR, "T805A_TR", {}, {{}, {}, {}, {}} },
  { _TRACK, T807A_TR, "T807A_TR", {}, {{}, {}, {}, {}} },
  { _TRACK, T809A_TR, "T809A_TR", {}, {{}, {}, {}, {}} },
  { _TRACK, T811A_TR, "T811A_TR", {}, {{}, {}, {}, {}} },
  { END_OF_CBI_STAT_KIND }
};
#else
extern TRACK track_state[];
#endif
#endif // TRACK_ATTRIB_DEFINITION

#ifdef ROUTE_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
ROUTE route_state[] = {
  {MAIN_ROUTE, S821A_S801A, "S821A_S801A", {4, {T111A, T112A, T113A, T114A}}, {_SIGNAL, S821A}, {_SIGNAL, S801A}},
  {END_OF_ROUTE_KINDS}
};
#else
extern ROUTE route_state[];
#endif
#endif // ROUTE_ATTRIB_DEFINITION

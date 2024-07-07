#ifdef TRACK_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
TRACK track_state[] = {
  { _TRACK, T801A_TR, "T801A_TR", {0}, {{TRUE, _TLSR, T801A_TLSR}, {TRUE, _TRSR, T801A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { _TRACK, T803A_TR, "T803A_TR", {0}, {{TRUE, _TLSR, T803A_TLSR}, {TRUE, _TRSR, T803A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { _TRACK, T805A_TR, "T805A_TR", {0}, {{TRUE, _TLSR, T805A_TLSR}, {TRUE, _TRSR, T805A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { _TRACK, T807A_TR, "T807A_TR", {0}, {{TRUE, _TLSR, T807A_TLSR}, {TRUE, _TRSR, T807A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { _TRACK, T809A_TR, "T809A_TR", {0}, {{TRUE, _TLSR, T809A_TLSR}, {TRUE, _TRSR, T809A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { _TRACK, T811A_TR, "T811A_TR", {0}, {{TRUE, _TLSR, T811A_TLSR}, {TRUE, _TRSR, T811A_TRSR}, {}, {}, {}, {}, {}, {}} },
  { END_OF_CBI_STAT_KIND }};
#else
extern TRACK track_state[];
#endif
#endif // TRACK_ATTRIB_DEFINITION

#ifdef ROUTE_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
ROUTE route_state[] = {
  {MAIN_ROUTE, S821A_S801A, "S821A_S801A", {4, {T801A_TR, T803A_TR, T805A_TR, T807A_TR}}, {{_SIGNAL, S821A}, {_SIGNAL, S801A}}},
  {END_OF_ROUTE_KINDS}
};
#else
extern ROUTE route_state[];
#endif
#endif // ROUTE_ATTRIB_DEFINITION

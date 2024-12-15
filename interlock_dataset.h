#ifdef TRACK_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
TRACK track_dataset_def[] = {
  { _TRACK, T815B_TR, {1, {VB304DB}}, {{TRUE, _TLSR, T815B_TLSR}, {TRUE, _TRSR, T815B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T815B_eTLSR}, {TRUE, _eTRSR, T815B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T817B_TR, {1, {VB306D}}, {{TRUE, _TLSR, T817B_TLSR}, {TRUE, _TRSR, T817B_TRSR}, {FALSE}, {TRUE, _sTRSR, T817B_sTRSR}, {TRUE, _eTLSR, T817B_eTLSR}, {TRUE, _eTRSR, T817B_eTRSR}, {FALSE}, {TRUE, _kTRSR, T817B_kTRSR}} },
  { _TRACK, T819B_TR, {0}, {{TRUE, _TLSR, T819B_TLSR}, {TRUE, _TRSR, T819B_TRSR}, {FALSE}, {TRUE, _sTRSR, T819B_sTRSR}, {TRUE, _eTLSR, T819B_eTLSR}, {TRUE, _eTRSR, T819B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T821B_TR, {0}, {{TRUE, _TLSR, T821B_TLSR}, {TRUE, _TRSR, T821B_TRSR}, {FALSE}, {TRUE, _sTRSR, T821B_sTRSR}, {TRUE, _eTLSR, T821B_eTLSR}, {TRUE, _eTRSR, T821B_eTRSR}, {TRUE, _kTLSR, T821B_kTLSR}, {FALSE}} },
  { _TRACK, T823B_TR, {7, {VB309DB, VB311D, VB313D, VB315D, VB318D, VB320D, VB322D}}, {{TRUE, _TLSR, T823B_TLSR}, {TRUE, _TRSR, T823B_TRSR}, {FALSE}, {TRUE, _sTRSR, T823B_sTRSR}, {TRUE, _eTLSR, T823B_eTLSR}, {TRUE, _eTRSR, T823B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T825B_TR, {1, {VB324D}}, {{TRUE, _TLSR, T825B_TLSR}, {TRUE, _TRSR, T825B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T825B_eTLSR}, {TRUE, _eTRSR, T825B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T827B_TR, {1, {VB326D}}, {{TRUE, _TLSR, T827B_TLSR}, {TRUE, _TRSR, T827B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T827B_eTLSR}, {TRUE, _eTRSR, T827B_eTRSR}, {FALSE}, {TRUE, _kTRSR, T827B_kTRSR}} },
  { _TRACK, T801A_TR, {7, {VB327D, VB328D, VB331D, VB333D, VB334D, VB336D, VB339D}}, {{TRUE, _TLSR, T801A_TLSR}, {TRUE, _TRSR, T801A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T801A_eTLSR}, {TRUE, _eTRSR, T801A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T803A_TR, {1, {VB340D}}, {{TRUE, _TLSR, T803A_TLSR}, {TRUE, _TRSR, T803A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T803A_eTLSR}, {TRUE, _eTRSR, T803A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T805A_TR, {1, {VB342DA}}, {{TRUE, _TLSR, T805A_TLSR}, {TRUE, _TRSR, T805A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T805A_eTLSR}, {TRUE, _eTRSR, T805A_eTRSR}, {FALSE}, {TRUE, _kTRSR, T805A_kTRSR}} },
  { _TRACK, T807A_TR, {9, {VB342DB, VB344D, VB346D, VB348D, VB350D, VB352D, VB354D, VB356D, VB358D}}, {{TRUE, _TLSR, T807A_TLSR}, {TRUE, _TRSR, T807A_TRSR}, {TRUE, _sTLSR, T807A_sTLSR}, {FALSE}, {TRUE, _eTLSR, T807A_eTLSR}, {TRUE, _eTRSR, T807A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T809A_TR, {1, {VB359D}}, {{TRUE, _TLSR, T809A_TLSR}, {TRUE, _TRSR, T809A_TRSR}, {TRUE, _sTLSR, T809A_sTLSR}, {FALSE}, {TRUE, _eTLSR, T809A_eTLSR}, {TRUE, _eTRSR, T809A_eTRSR}, {FALSE}, {TRUE, _kTRSR, T809A_kTRSR}} },
  { _TRACK, T811A_TR, {0}, {{TRUE, _TLSR, T811A_TLSR}, {TRUE, _TRSR, T811A_TRSR}, {TRUE, _sTLSR, T811A_sTLSR}, {FALSE}, {TRUE, _eTLSR, T811A_eTLSR}, {TRUE, _eTRSR, T811A_eTRSR}, {TRUE, _kTLSR, T811A_kTLSR}, {FALSE}} },
  { _TRACK, T813A_TR, {1, {VB360DB}}, {{TRUE, _TLSR, T813A_TLSR}, {TRUE, _TRSR, T813A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T813A_eTLSR}, {TRUE, _eTRSR, T813A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T815A_TR, {2, {VB362D, VB363DA}}, {{TRUE, _TLSR, T815A_TLSR}, {TRUE, _TRSR, T815A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T815A_eTLSR}, {TRUE, _eTRSR, T815A_eTRSR}, {FALSE}, {TRUE, _kTRSR, T815A_kTRSR}} },
  { _TRACK, T817A_TR, {1, {VB363DB}}, {{FALSE}, {TRUE, _TRSR, T817A_TRSR}, {FALSE}, {FALSE}, {FALSE}, {TRUE, _eTRSR, T817A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T816A_TR, {1, {VB364U}}, {{FALSE}, {TRUE, _TRSR, T816A_TRSR}, {FALSE}, {FALSE}, {FALSE}, {TRUE, _eTRSR, T816A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T814A_TR, {1, {VB363UB}}, {{TRUE, _TLSR, T814A_TLSR}, {TRUE, _TRSR, T814A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T814A_eTLSR}, {TRUE, _eTRSR, T814A_eTRSR}, {FALSE}, {TRUE, _kTRSR, T814A_kTRSR}} },
  { _TRACK, T812A_TR, {1, {VB361U}}, {{TRUE, _TLSR, T812A_TLSR}, {TRUE, _TRSR, T812A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T812A_eTLSR}, {TRUE, _eTRSR, T812A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T810A_TR, {1, {VB360UB}}, {{TRUE, _TLSR, T810A_TLSR}, {TRUE, _TRSR, T810A_TRSR}, {TRUE, _sTLSR, T810A_sTLSR}, {FALSE}, {TRUE, _eTLSR, T810A_eTLSR}, {TRUE, _eTRSR, T810A_eTRSR}, {TRUE, _kTLSR, T810A_kTLSR}, {FALSE}} },
  { _TRACK, T808A_TR, {0}, {{TRUE, _TLSR, T808A_TLSR}, {TRUE, _TRSR, T808A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T808A_eTLSR}, {TRUE, _eTRSR, T808A_eTRSR}, {FALSE}, {TRUE, _kTRSR, T808A_kTRSR}} },
  { _TRACK, T806A_TR, {9, {VB359U, VB357U, VB355U, VB352U, VB350U, VB348U, VB346U, VB344U, VB342U}}, {{TRUE, _TLSR, T806A_TLSR}, {TRUE, _TRSR, T806A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T806A_eTLSR}, {TRUE, _eTRSR, T806A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T804A_TR, {1, {VB341U}}, {{TRUE, _TLSR, T804A_TLSR}, {TRUE, _TRSR, T804A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T804A_eTLSR}, {TRUE, _eTRSR, T804A_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T802A_TR, {1, {VB340U}}, {{TRUE, _TLSR, T802A_TLSR}, {TRUE, _TRSR, T802A_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T802A_eTLSR}, {TRUE, _eTRSR, T802A_eTRSR}, {TRUE, _kTLSR, T802A_kTLSR}, {FALSE}} },
  { _TRACK, T828B_TR, {8, {VB339U, VB337U, VB335U, VB333U, VB331U, VB329U, VB328U, VB326U}}, {{TRUE, _TLSR, T828B_TLSR}, {TRUE, _TRSR, T828B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T828B_eTLSR}, {TRUE, _eTRSR, T828B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T826B_TR, {1, {VB325U}}, {{TRUE, _TLSR, T826B_TLSR}, {TRUE, _TRSR, T826B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T826B_eTLSR}, {TRUE, _eTRSR, T826B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T824B_TR, {1, {VB324U}}, {{TRUE, _TLSR, T824B_TLSR}, {TRUE, _TRSR, T824B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T824B_eTLSR}, {TRUE, _eTRSR, T824B_eTRSR}, {TRUE, _kTLSR, T824B_kTLSR}, {FALSE}} },
  { _TRACK, T822B_TR, {8, {VB323U, VB320U, VB318U, VB316U, VB314U, VB313U, VB311U, VB309UB}}, {{TRUE, _TLSR, T822B_TLSR}, {TRUE, _TRSR, T822B_TRSR}, {FALSE}, {TRUE, _sTRSR, T822B_sTRSR}, {TRUE, _eTLSR, T822B_eTLSR}, {TRUE, _eTRSR, T822B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T820B_TR, {1, {VB309UA}}, {{TRUE, _TLSR, T820B_TLSR}, {TRUE, _TRSR, T820B_TRSR}, {FALSE}, {TRUE, _sTRSR, T820B_sTRSR}, {TRUE, _eTLSR, T820B_eTLSR}, {TRUE, _eTRSR, T820B_eTRSR}, {TRUE, _kTLSR, T820B_kTLSR}, {FALSE}} },
  { _TRACK, T818B_TR, {0}, {{TRUE, _TLSR, T818B_TLSR}, {TRUE, _TRSR, T818B_TRSR}, {FALSE}, {TRUE, _sTRSR, T818B_sTRSR}, {TRUE, _eTLSR, T818B_eTLSR}, {TRUE, _eTRSR, T818B_eTRSR}, {FALSE}, {FALSE}} },
  { _TRACK, T816B_TR, {1, {VB306U}}, {{TRUE, _TLSR, T816B_TLSR}, {TRUE, _TRSR, T816B_TRSR}, {FALSE}, {TRUE, _sTRSR, T816B_sTRSR}, {TRUE, _eTLSR, T816B_eTLSR}, {TRUE, _eTRSR, T816B_eTRSR}, {FALSE}, {TRUE, _kTRSR, T816B_kTRSR}} },
  { _TRACK, T814B_TR, {1, {VB305U}}, {{TRUE, _TLSR, T814B_TLSR}, {TRUE, _TRSR, T814B_TRSR}, {FALSE}, {FALSE}, {TRUE, _eTLSR, T814B_eTLSR}, {TRUE, _eTRSR, T814B_eTRSR}, {FALSE}, {FALSE}} },
  { END_OF_CBI_STAT_KIND }
};
#else
extern TRACK track_dataset_def[];
#endif
#endif // TRACK_ATTRIB_DEFINITION

#ifdef ROUTE_ATTRIB_DEFINITION
#ifdef INTERLOCK_C
ROUTE route_dataset_def[] = {
  { _ROUTE, DEP_ROUTE, S803B_S831B, P_S803B_S831B, {5, {T817B_TR, T819B_TR, T821B_TR, T823B_TR, T825B_TR}}, {{S803B}, {S831B}}, {TRUE, {1, {VB304DB}}, {2, 2, {T817B_TR, T819B_TR}, 1, {T817B_TR}}, {{VB304DB, SP_73}, {VB324D, SP_77}}}},
  { _ROUTE, DEP_ROUTE, S831B_S821A, P_S831B_S821A, {3, {T827B_TR, T801A_TR, T803A_TR}}, {{S831B}, {S821A}}, {TRUE, {8, {VB309DB, VB311D, VB313D, VB315D, VB318D, VB320D, VB322D, VB324D}}, {1, 1, {T827B_TR}, 1, {T827B_TR}}, {{VB324D, SP_77},{VB340D, SP_79}}}},
  { _ROUTE, DEP_ROUTE, S821A_S801A, P_S821A_S801A, {2, {T805A_TR, T807A_TR}}, {{S821A}, {S801A}}, {TRUE, {8, {VB327D, VB328D, VB331D, VB333D, VB334D, VB336D, VB339D, VB340D}}, {1, 1,{T805A_TR}, 1, {T805A_TR}}, {{VB340D, SP_79}, {VB358D, SP_NONSENS}}}},
  { _ROUTE, ENT_ROUTE, S801A_S803A, P_S801A_S803A, {3, {T809A_TR, T811A_TR, T813A_TR}}, {{S801A}, {S803A}}, {TRUE, {9, {VB342DB, VB344D, VB346D, VB348D, VB350D, VB352D, VB354D, VB356D, VB358D}}, {3, 3, {T809A_TR, T811A_TR, T813A_TR}, 1, {T809A_TR}}, {{VB358D, SP_NONSENS}, {VB360DB, SP_81}}}},
  { _ROUTE, ENT_ROUTE, S801A_S807A, P_S801A_S807A, {4, {T809A_TR, T811A_TR, T810A_TR, T812A_TR}}, {{S801A}, {S807A}}, {TRUE, {9, {VB342DB, VB344D, VB346D, VB348D, VB350D, VB352D, VB354D, VB356D, VB358D}}, {4, 4, {T809A_TR, T811A_TR, T810A_TR, T812A_TR}, 1, {T809A_TR}}, {{VB358D, SP_NONSENS},{VB361U, SP_80}}}},
  { _ROUTE, SHUNT_ROUTE, S803A_S809A, P_S803A_S809A, {2, {T815A_TR, T817A_TR}}, {{S803A}, {S809A}}, {TRUE, {10, {VB342DB, VB344D, VB346D, VB348D, VB350D, VB352D, VB354D, VB356D, VB358D, VB360DB}}, {2, 2, {T815A_TR, T817A_TR}, 1, {T815A_TR}}, {{VB360DB, SP_81}, {VB363DB, SP_D5}}}},
  { _ROUTE, SHUNT_ROUTE, S803A_S811A, P_S803A_S811A, {3, {T815A_TR, T814A_TR, T816A_TR}}, {{S803A}, {S811A}}, {TRUE, {10, {VB342DB, VB344D, VB346D, VB348D, VB350D, VB352D, VB354D, VB356D, VB358D, VB360DB}}, {3, 3, {T815A_TR, T814A_TR, T816A_TR}, 1, {T815A_TR}}, {{VB360DB, SP_81}, {VB364U, SP_D0}}}},
  { _ROUTE, SHUNT_ROUTE, S806A_S804A, P_S806A_S804A, {3, {T815A_TR, T814A_TR, T812A_TR}}, {{S806A}, {S804A}}, {TRUE, {1, {VB363DB}}, {3, 3, {T815A_TR, T814A_TR, T812A_TR}, 1, {T815A_TR}}, {{VB363DB, SP_D5}, {VB361U, SP_80}}}},
  { _ROUTE, SHUNT_ROUTE, S802A_S804A, P_S802A_S804A, {2, {T814A_TR, T812A_TR}}, {{S802A}, {S804A}}, {TRUE, {1, {VB364U}}, {2, 2, {T814A_TR, T812A_TR}, 1, {T814A_TR}}, {{VB364U, SP_D0}, {VB361U, SP_80}}}},
  { _ROUTE, DEP_ROUTE, S804A_S822A, P_S804A_S822A, {4, {T810A_TR, T808A_TR, T806A_TR, T804A_TR}}, {{S804A}, {S822A}}, {TRUE, {3, {VB363DB, VB364U, VB361U}}, {1, 1, {T810A_TR}, 1, {T810A_TR}}, {{VB361U, SP_80}, {VB341U, SP_78}}}},
  { _ROUTE, DEP_ROUTE, S822A_S832B, P_S822A_S832B, {3, {T802A_TR, T828B_TR, T826B_TR}}, {{S822A}, {S832B}}, {TRUE, {10, {VB359U, VB357U, VB355U, VB352U, VB350U, VB348U, VB346U, VB344U, VB342U, VB341U}}, {1, 1, {T802A_TR}, 1, {T802A_TR}}, {{VB341U, SP_78}, {VB325U, SP_76}}}},
  { _ROUTE, DEP_ROUTE, S832B_S802B, P_S832B_S802B, {2, {T824B_TR, T822B_TR}}, {{S832B}, {S802B}}, {TRUE, {9, {VB339U, VB337U, VB335U, VB333U, VB331U, VB329U, VB328U, VB326U, VB325U}}, {1, 1, {T824B_TR}, 1, {T824B_TR}}, {{VB325U, SP_76}, {VB309UB, SP_NONSENS}} }},
  { _ROUTE, ENT_ROUTE, S802B_S804B, P_S802B_S804B, {4, {T820B_TR, T818B_TR, T816B_TR, T814B_TR}}, {{S802B}, {S804B}}, {TRUE, {8, {VB323U, VB320U, VB318U, VB316U, VB314U, VB313U, VB311U, VB309UB}}, {4, 4, {T820B_TR, T818B_TR, T816B_TR, T814B_TR}, 1, {T820B_TR}}, {{VB309UB, SP_NONSENS}, {VB305U, SP_74}}}},
  { _ROUTE, ENT_ROUTE, S802B_S810B, P_S802B_S810B, {5, {T820B_TR, T818B_TR, T819B_TR, T817B_TR, T815B_TR}}, {{S802B}, {S810B}}, {TRUE, {8, {VB323U, VB320U, VB318U, VB316U, VB314U, VB313U, VB311U, VB309UB}}, {5, 5,{T820B_TR, T818B_TR, T819B_TR, T817B_TR, T815B_TR}, 1, {T820B_TR}}, {{VB309UB, SP_NONSENS}, {VB304DB, SP_73}}}},
  //{ _ROUTE, SHUNT_ROUTE, S803B_VS801B, P_S803B_VS801B, {4, {T817B_TR, T819B_TR, T821B_TR, T823B_TR}}, {{_SIGNAL, S803B}, {_SIGNAL, VS801B}},{TRUE, {}, {4, 4, {T817B_TR, T819B_TR, T821B_TR, T823BV1_TR}, 1, {T817B_TR}} }},
  //{ _ROUTE, SHUNT_ROUTE, S803B_VS803B, P_S803B_VS803B, {5, {T817B_TR, T819B_TR, T818B_TR, T820B_TR, T822B_TR}}, {{_SIGNAL, S803B}, {_SIGNAL, VS803B}}, {TRUE, {}, {5, 5, {T817B_TR, T819B_TR, T818B_TR, T820B_TR, T822BV2_TR}, 1, {T817B_TR}} }},
  { _ROUTE, SHUNT_ROUTE, S806B_S804B, P_S806B_S804B, {5, {T821B_TR, T819B_TR, T817B_TR, T816B_TR, T814B_TR}}, {{S806B}, {S804B}}, {TRUE, {1, {VB309DB}}, {5, 5, {T821B_TR, T819B_TR, T817B_TR, T816B_TR, T814B_TR}, 1, {T821B_TR}}, {{VB309DB, SP_NONSENS}, {VB305U, SP_74}}}},
  //{ _ROUTE, SHUNT_ROUTE, S807B_VS801B, P_S807B_VS801B, {5, {T816B_TR, T817B_TR, T819B_TR, T821B_TR, T823B_TR}}, {{_SIGNAL, S807B}, {_SIGNAL, VS801B}}, {TRUE, {}, {5, 5, {T816B_TR, T817B_TR, T819B_TR, T821B_TR, T823BV1_TR}, 1, {T816B_TR}} }},
  //{ _ROUTE, SHUNT_ROUTE, S807B_VS803B, P_S807B_VS803B, {4, {T816B_TR, T818B_TR, T820B_TR, T822B_TR}}, {{_SIGNAL, S807B}, {_SIGNAL, VS803B}}, {TRUE, {}, {4, 4, {T816B_TR, T818B_TR, T820B_TR, T822BV2_TR}, 1, {T816B_TR}} }},
  { _ROUTE, SHUNT_ROUTE, S806B_S810B, P_S806B_S810B, {4, {T821B_TR, T819B_TR, T817B_TR, T815B_TR}}, {{S806B}, {S810B}}, {TRUE, {1, {VB309DB}}, {4, 4, {T821B_TR, T819B_TR, T817B_TR, T815B_TR}, 1, {T821B_TR}}, {{VB309DB, SP_NONSENS} ,{VB304DB, SP_73}}}},
  { _ROUTE, DEP_ROUTE, S807B_S831B, P_S807B_S831B, {6, {T816B_TR, T817B_TR, T819B_TR, T821B_TR, T823B_TR, T825B_TR}}, {{S807B}, {S831B}}, {TRUE, {1, {VB305U}}, {3, 3, {T816B_TR, T817B_TR, T819B_TR}, 1, {T816B_TR}}, {{VB305U, SP_74},{VB324D, SP_77}}}},
  { END_OF_CBI_STAT_KIND, END_OF_ROUTE_KINDS }
};
#else
extern ROUTE route_dataset_def[];
#endif
#endif // ROUTE_ATTRIB_DEFINITION

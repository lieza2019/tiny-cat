#ifdef BLOCK_ID_DEFINITIONS
typedef enum cbtc_block_id {
  VB_NONSENS,
  VB309DB,
  VB311D,
  VB313D,
  VB315D,
  VB318D,
  VB320D,
  VB322D,
  VB324D,
  VB326U,
  VB328U,
  VB329U,
  VB331U,
  VB333U,
  VB335U,
  VB337U,
  VB339U,
  VB325U,
  VB327D,
  VB328D,
  VB331D,
  VB333D,
  VB334D,
  VB336D,
  VB339D,
  VB340D,  
  VB342U,
  VB344U,
  VB346U,
  VB348U,
  VB350U,
  VB352U,
  VB355U,
  VB357U,
  VB359U,
  VB341U,
  VB342DB,
  VB344D,
  VB346D,
  VB348D,
  VB350D,
  VB352D,
  VB354D,
  VB356D,
  VB358D,
  VB309UB,
  VB311U,
  VB313U,
  VB314U,
  VB316U,
  VB318U,
  VB320U,
  VB323U,  
  VB304DB,
  VB305U,
  VB363DB,
  VB364U,
  VB361U,
  VB360DB,
  END_OF_CBTC_BLOCKs
} CBTC_BLOCK_ID;
#endif // BLOCK_ID_DEFINITIONS

#ifdef BLOCK_ATTRIB_DEFINITION
#ifdef CBTC_C
CBTC_BLOCK block_state[] = {
  // S803B_VS801B, S803B_VS803B, S803B_S831B
  {2418, VB304DB, "VB304DB", -1, {T815B_TR}, {TRUE, SP_73, P0_COUPLING, {VB_NONSENS}}}, // JLA_PL1 // CHECKED.
  
  // S807B_VS801B, S807B_VS803B, S807B_S831B
  {2013, VB305U, "VB305U", -1, {T814B_TR}, {TRUE, SP_74, P0_COUPLING, {VB_NONSENS}}}, // JLA_PL2 // CHECKED.
  
  // S831B_S821A
  {2422, VB309DB, "VB309DB", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2423, VB311D, "VB311D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2424, VB313D, "VB313D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2425, VB315D, "VB315D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2426, VB318D, "VB318D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2427, VB320D, "VB320D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2428, VB322D, "VB322D", -1, {T823B_TR}, {FALSE}}, // CHECKED.
  {2429, VB324D, "VB324D", -1, {T825B_TR}, {TRUE, SP_77, P0_COUPLING, {VB_NONSENS}}}, // KIKJ_PL1 // CHECKED.
  
  // S832B_S802B
  //{1003, VB340U, "VB340U", -1, {T802A_TR}, {FALSE}}, // CHECKED.
  {1002, VB339U, "VB339U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {1001, VB337U, "VB337U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {1000, VB335U, "VB335U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2031, VB333U, "VB333U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2030, VB331U, "VB331U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2029, VB329U, "VB329U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2028, VB328U, "VB328U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2027, VB326U, "VB326U", -1, {T828B_TR}, {FALSE}}, // CHECKED.
  {2026, VB325U, "VB325U", -1, {T826B_TR}, {TRUE, SP_76, P0_COUPLING, {VB_NONSENS}}}, // KIKJ_PL2 // CHECKED.
  
  // S821A_S801A
  //{2430, VB326D, "VB326D", -1, {T827B_TR}, {FALSE}}, // CHECKED.
  {2431, VB327D, "VB327D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {2432, VB328D, "VB328D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {2433, VB331D, "VB331D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {2434, VB333D, "VB333D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {1400, VB334D, "VB334D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {1401, VB336D, "VB336D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {1402, VB339D, "VB339D", -1, {T801A_TR}, {FALSE}}, // CHECKED.
  {1403, VB340D, "VB340D", -1, {T803A_TR}, {TRUE, SP_79, P0_COUPLING, {VB_NONSENS}}}, // OKBS_PL1 // CHECKED.
  
  // S822A_S832B
  {1013, VB359U, "VB359U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1012, VB357U, "VB357U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1011, VB355U, "VB355U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1010, VB352U, "VB352U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1009, VB350U, "VB350U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1008, VB348U, "VB348U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1007, VB346U, "VB346U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1006, VB344U, "VB344U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1005, VB342U, "VB342U", -1, {T806A_TR}, {FALSE}}, // CHECKED.
  {1004, VB341U, "VB341U", -1, {T804A_TR}, {TRUE, SP_78, P0_COUPLING, {VB_NONSENS}}}, // OKBS_PL2 // CHECKED.
  
  // S801A_S803A, S801A_S807A
  //{1404, VB342DA, "VB342DA", -1, {T805A_TR}, {FALSE}}, // CHECKED.
  {1405, VB342DB, "VB342DB", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1406, VB344D, "VB344D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1407, VB346D, "VB346D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1408, VB348D, "VB348D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1409, VB350D, "VB350D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1410, VB352D, "VB352D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1411, VB354D, "VB354D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1412, VB356D, "VB356D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  {1413, VB358D, "VB358D", -1, {T807A_TR}, {FALSE}}, // CHECKED.
  
  // S802B_S810B, S802B_S804B
  //{2025, VB324U, "VB324U", -1, {T824B_TR}, {FALSE}}, // CHECKED.
  {2024, VB323U, "VB323U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2023, VB320U, "VB320U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2022, VB318U, "VB318U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2021, VB316U, "VB316U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2020, VB314U, "VB314U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2019, VB313U, "VB313U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2018, VB311U, "VB311U", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  {2017, VB309UB, "VB309UB", -1, {T822B_TR}, {FALSE}}, // CHECKED.
  
  // S806A_S804A
  {1418, VB363DB, "VB363DB", -1, {T817A_TR}, {TRUE, SP_D5, P0_COUPLING, {VB_NONSENS}}}, // BTGD_TB1 // CHECKED.
  
  // S802A_S804A
  {1018, VB364U, "VB364U", -1, {T816A_TR}, {TRUE, SP_D0, P0_COUPLING, {VB_NONSENS}}}, // BTGD_TB2 // CHECKED.
  
  // S804A_S822A
  /* (1418, VB363DB) in T817A_TR, is registered in the case of S806A_S804A. */
  /* (1018, VB364U) in T816A_TR, is registered in the case of S802A_S804A. */
  {1016, VB361U, "VB361U", -1, {T812A_TR}, {TRUE, SP_80, P0_COUPLING, {VB_NONSENS}}}, // BTGD_PL2 // CHECKED.
  
  // S803A_S809A, S803A_S811A
  //{1404, VB342DA, "VB342DA", -1, {T805A_TR}, {FALSE}}, // CHECKED.
  /* all below are registered in the case of S802B_S810B / S802B_S804B. 
     (1405, VB342DB) in T807A_TR
     (1406, VB344D) in T807A_TR
     (1407, VB346D) in T807A_TR
     (1408, VB348D) in T807A_TR
     (1409, VB350D) in T807A_TR
     (1410, VB352D) in T807A_TR
     (1411, VB354D) in T807A_TR
     (1412, VB356D) in T807A_TR
     (1413, VB358D) in T807A_TR
  */
  {1416, VB360DB, "VB360DB", -1, {T813A_TR}, {TRUE, SP_81, P0_COUPLING, {VB_NONSENS}}}, // BTGD_PL1 // CHECKED.
  
  {0, END_OF_CBTC_BLOCKs, "END_OF_CBTC_BLOCKs" }
};
#else
extern CBTC_BLOCK block_state[];
#endif // CBTC_C
#endif // BLOCK_ATTRIB_DEFINITION

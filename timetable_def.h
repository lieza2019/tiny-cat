#ifdef TIMETABLE_C
SCHEDULED_COMMAND trial_scheduled_commands_1[] = {
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {1, SP_74, {05, 00, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {1, S807B_S831B, TRUE, {05, 00, 05, 2025, 01, 04}} }},
  //{1, ARS_SCHEDULED_SKIP, { .sch_skip = {2, SP_74, {05, 00, 05, 2025, 01, 04}, TRUE} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {2, 30, SP_74, {05, 00, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S807B_S831B} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {1, S807B_S831B, {05, 00, 30, 2025, 01, 04}} }},
  
  //{1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {6, SP_77, {05, 03, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {2, S831B_S821A, TRUE, {05, 03, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_SKIP, { .sch_skip = {3, SP_77, {05, 03, 00, 2025, 01, 04}, TRUE} }},
  //{1, ARS_SCHEDULED_DEPT, { .sch_dept = {3, 30, SP_77, {05, 03, 35, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S831B_S821A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {2, S831B_S821A, {05, 03, 00, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {4, SP_79, {05, 05, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {3, S821A_S801A, TRUE, {05, 05, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {5, 30, SP_79, {05, 05, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S821A_S801A, FALSE} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {3, S821A_S801A, {05, 05, 30, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {4, S801A_S803A, FALSE, {05, 05, 30, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {4, S801A_S803A, {05, 05, 30, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {6, SP_81, {05, 45, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {5, S803A_S809A, FALSE, {05, 45, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {7, 30, SP_81, {05, 45, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S803A_S809A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {5, S803A_S809A, {05, 45, 30, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {8, SP_D5, {05, 48, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {6, S806A_S804A, FALSE, {05, 48, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {9, 30, SP_D5, {05, 51, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {TRUE, FALSE}, S806A_S804A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {6, S806A_S804A, {05, 51, 00, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {10, SP_80, {05, 53, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {7, S804A_S822A, TRUE, {05, 53, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {11, 30, SP_80, {05, 53, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {TRUE, FALSE}, S804A_S822A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {7, S804A_S822A, {05, 53, 30, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {12, SP_78, {05, 58, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {8, S822A_S832B, TRUE, {05, 58, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {13, 30, SP_78, {05, 58, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {TRUE, FALSE}, S822A_S832B} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {8, S822A_S832B, {05, 58, 30, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {9, S832B_S802B, TRUE, {05, 58, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_SKIP, { .sch_skip = {14, SP_76, {05, 58, 00, 2025, 01, 04}, TRUE} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {9, S832B_S802B, {05, 58, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {10, S802B_S810B, FALSE, {05, 58, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {10, S802B_S810B, {05, 58, 00, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {15, SP_73, {06, 11, 00, 2025, 01, 04}} }},
  {1, END_OF_SCHEDULED_CMDS}
};
SCHEDULED_COMMAND trial_scheduled_commands_2[] = {
  {2, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {4, SP_74, {06, 00, 00, 2025, 01, 04}} }},
  {2, ARS_SCHEDULED_DEPT, { .sch_dept = {5, 30, SP_74, {06, 00, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S807B_S831B} }},
  {2, ARS_SCHEDULED_DEPT, { .sch_dept = {6, 30, SP_74, {06, 03, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S807B_S831B} }},
  {2, END_OF_SCHEDULED_CMDS}
};
SCHEDULED_COMMAND trial_scheduled_commands_3[] = {
  //{3, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {10, SP_77, {05, 00, 00, 2025, 01, 04}} }},
  {3, ARS_SCHEDULED_ROUTESET, { .sch_roset = {11, S831B_S821A, TRUE, {05, 00, 05, 2025, 01, 04}} }},
  //{1, ARS_SCHEDULED_DEPT, { .sch_dept = {12, 30, SP_77, {05, 00, 35, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S831B_S821A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {13, S831B_S821A, {05, 00, 35, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {14, SP_79, {05, 03, 00, 2025, 01, 04}} }},
  {3, END_OF_SCHEDULED_CMDS}
};
ONLINE_TIMETABLE trial_timetable = {
  3,
  { {{TRUE, 1, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_1, &trial_scheduled_commands_1[1]}}},
    {{TRUE, 2, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_2, trial_scheduled_commands_2}}},
    {{TRUE, 3, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_3, trial_scheduled_commands_3}}}
  }
};
#else
extern ONLINE_TIMETABLE trial_timetable;
#endif // TIMETABLE_C

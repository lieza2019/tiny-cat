#ifdef TIMETABLE_C
SCHEDULED_COMMAND trial_scheduled_commands_1[] = {
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {1, SP_74, {05, 00, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {1, S807B_S831B, TRUE, {05, 00, 05, 2025, 01, 04}} }},
  //{1, ARS_SCHEDULED_SKIP, { .sch_skip = {2, SP_74, {05, 00, 05, 2025, 01, 04}, TRUE} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {2, 30, SP_74, {05, 00, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S807B_S831B} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {1, S807B_S831B, {05, 00, 30, 2025, 01, 04}} }},
  
  //{1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {6, SP_77, {05, 03, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {2, S831B_S821A, TRUE, {05, 03, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_SKIP, { .sch_skip = {3, SP_77, {05, 03, 05, 2025, 01, 04}, TRUE} }},
  //{1, ARS_SCHEDULED_DEPT, { .sch_dept = {3, 30, SP_77, {05, 03, 35, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S831B_S821A} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {2, S831B_S821A, {05, 03, 05, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {4, SP_79, {05, 05, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {3, S821A_S801A, FALSE, {05, 05, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {5, 30, SP_79, {05, 05, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}, S821A_S801A, FALSE} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {3, S821A_S801A, {05, 05, 30, 2025, 01, 04}} }},
  
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
TIMETABLE trial_timetable = {
  3,
  { {{TRUE, 1, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_1, &trial_scheduled_commands_1[1]}}},
    {{TRUE, 2, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_2, trial_scheduled_commands_2}}},
    {{TRUE, 3, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_3, trial_scheduled_commands_3}}}
  }
};
#else
extern TIMETABLE trial_timetable;
#endif // TIMETABLE_C

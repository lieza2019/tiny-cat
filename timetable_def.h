#ifdef TIMETABLE_C
SCHEDULED_COMMAND trial_scheduled_commands_1[] = {
  //{2, ARS_SCHEDULED_SKIP, { .sch_arriv = {31, SP_74, {05, 00, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {1, SP_74, {05, 00, 00, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_ROUTESET, { .sch_roset = {2, S807B_S831B, TRUE, {05, 00, 05, 2025, 01, 04}} }},
  {1, ARS_SCHEDULED_DEPT, { .sch_dept = {3, 30, SP_74, {05, 00, 30, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}} }},
  //{1, ARS_SCHEDULED_DEPT, { .sch_dept = {4, 30, SP_74, {05, 00, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}} }},
  {1, ARS_SCHEDULED_ROUTEREL, { .sch_rorel = {5, S807B_S831B, {05, 00, 30, 2025, 01, 04}} }},
  
  {1, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {6, SP_77, {05, 03, 00, 2025, 01, 04}} }},
  
  {1, END_OF_SCHEDULED_CMDS}
};
SCHEDULED_COMMAND trial_scheduled_commands_2[] = {
  {2, ARS_SCHEDULED_ARRIVAL, { .sch_arriv = {7, SP_74, {06, 00, 00, 2025, 01, 04}} }},
  {2, ARS_SCHEDULED_DEPT, { .sch_dept = {8, 30, SP_74, {06, 00, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}} }},
  {2, ARS_SCHEDULED_DEPT, { .sch_dept = {9, 30, SP_74, {06, 03, 00, 2025, 01, 04}, TRUE, PERFREG_NORMAL, CREW_NO_ID, {FALSE, TRUE}} }},
  {2, END_OF_SCHEDULED_CMDS}
};
SCHEDULED_COMMAND trial_scheduled_commands_3[] = {
  {3, ARS_SCHEDULED_ROUTESET, { .sch_roset = {10, S803B_S831B, TRUE, {05, 00, 00, 2025, 01, 04}} }},
  {3, END_OF_SCHEDULED_CMDS}
};
TIMETABLE trial_timetable = {
  3,
  { {{TRUE, 1, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_1, trial_scheduled_commands_1}}},
    {{TRUE, 2, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_2, trial_scheduled_commands_2}}},
    {{TRUE, 3, {05, 00, 00, 2025, 01, 04}, {trial_scheduled_commands_3, trial_scheduled_commands_3}}}
  }
};
#else
extern TIMETABLE trial_timetable;
#endif // TIMETABLE_C

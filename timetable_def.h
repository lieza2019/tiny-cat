#ifdef TIMETABLE_C
SCHEDULED_COMMAND trial_scheduled_commands[] = {
  {ARS_SCHEDULED_ARRIVAL},
  {ARS_SCHEDULED_DEPT},
  {ARS_SCHEDULED_ROUTESET},
  {ARS_SCHEDULED_ROUTEREL}
};
#else
extern SCHEDULED_COMMAND trial_scheduled_commands[];
#endif // TIMETABLE_C

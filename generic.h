#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0

typedef struct tiny_time_desc {
  int hour;
  int minute;
  int second;
  int year;
  int month;
  int day;
} TINY_TIME_DESC, *TINY_TIME_DESC_PTR;

#define CMP_TINYTIME( t1, t2 ) (		\
  ((t1).hour == (t2).hour) &&			\
  ((t1).minute == (t2).minute) &&		\
  ((t1).second == (t2).second) &&		\
  ((t1).year == (t2).year) &&			\
  ((t1).month == (t2).month) &&			\
  ((t1).day == (t2).day)			\
)
#endif

//#define USE_REENTRANT_BASENAME

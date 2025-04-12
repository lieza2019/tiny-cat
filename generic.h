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
#endif

//#define USE_REENTRANT_BASENAME

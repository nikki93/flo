#define  _BSD_SOURCE
#define  _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GET_FILENAME(s)	sprintf(s, "%s/.flo", getenv("HOME"))
#define ITEM_COUNT 1024
#define LINE_LENGTH 1024

#define ARGS_ADD 0
#define ARGS_REMOVE 1
#define ARGS_CHANGE 2

#define FORMAT_DATE		"%m-%d %H:%M"
#define FORMAT_DATE_DUPLICATE	"      %H:%M"
#define FORMAT_DATE_LENGTH 12
#define FORMAT_DEADLINE		"d%*d  %s      %s\n"
#define FORMAT_DEADLINE_D	"d%*d  %s  d%d  %s\n"
#define FORMAT_EVENT		"%*d  %s      %s\n"
#define FORMAT_EVENT_D		"%*d  %s  d%d  %s\n"
#define FORMAT_EVENT_TO		"      %s    \n"
#define FORMAT_EVENT_TO_D	"      %s  d%d\n"
#define FORMAT_TODO		"t%*d  %s\n"

#define ARE_DATES_EQUAL(tm1, tm2) \
	(((tm1)->tm_year == (tm2)->tm_year && \
	(tm1)->tm_mon == (tm2)->tm_mon) && \
	(tm1)->tm_mday == (tm2)->tm_mday)
#define IS_DEADLINE(it) (it->from == 0 && it->to != 0)
#define IS_TODO(it) (it->from == 0 && it->to == 0)

struct args {
	char *what;
	char *from;
	char *to;
	int flag;
	unsigned int id;
};

struct item {
	char *what;
	time_t from;
	time_t to;
};

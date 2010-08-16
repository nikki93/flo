#define  _BSD_SOURCE
#define  _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DATE_FORMAT "%m-%d %H:%M"
#define DATE_FORMAT_DUPLICATE "      %H:%M"
#define DATE_FORMAT_LENGTH 12
#define GET_FILENAME(s)	sprintf(s, "%s/Dropbox/.flo", getenv("HOME"))
#define ITEM_COUNT 1024
#define LINE_LENGTH 1024

#define ARE_DATES_EQUAL(tm1, tm2) \
	(((tm1)->tm_year == (tm2)->tm_year && \
	(tm1)->tm_mon == (tm2)->tm_mon) && \
	(tm1)->tm_mday == (tm2)->tm_mday)
#define ARGS_ADD 0
#define ARGS_REMOVE 1
#define ARGS_CHANGE 2
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

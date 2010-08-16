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
#define GET_FILENAME(s)	sprintf(s, "%s/.flo", getenv("HOME"))
#define ITEM_COUNT 128
#define LINE_LENGTH 1024

#define ARE_DATES_EQUAL(tm1, tm2) (((tm1)->tm_year == (tm2)->tm_year && \
(tm1)->tm_mon == (tm2)->tm_mon) && (tm1)->tm_mday == (tm2)->tm_mday)
#define IS_DEADLINE(it) (it->from == 0 && it->to != 0)
#define IS_TODO(it) (it->from == 0 && it->to == 0)

struct args {
	char *what;
	char *from;
	char *to;
	int change;
	int remove;
	unsigned int id;
};

struct item {
	char *what;
	time_t from;
	time_t to;
};

int add_item(struct args *a);
int change_item(struct args *a);
int list_items(struct args *a);
int read_args(struct args *a, const int argc, char *argv[]);
int read_args_short(struct args *a, const int argc, char *argv[]);
int remove_item(struct args *a);
void fail(struct args *a, const char *e, const int print_usage);
void free_args(struct args *a);

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ARE_DATES_EQUAL(tm1, tm2) (((tm1)->tm_year == (tm2)->tm_year && \
(tm1)->tm_mon == (tm2)->tm_mon) && (tm1)->tm_mday == (tm2)->tm_mday)
#define GET_FILENAME(s)	sprintf(s, "%s/.flo", getenv("HOME"))
#define IS_DEADLINE(it) (it->from == 0 && it->to != 0)
#define IS_TODO(it) (it->from == 0 && it->to == 0)
#define ITEM_COUNT 128
#define LINE_LENGTH 1024

struct args {
	char *tag;
	char *what;
	char *from;
	char *to;
	int change;
	int remove;
	unsigned int id;
};

struct item {
	char *tag;
	char *what;
	time_t from;
	time_t to;
};

int add_item(struct args *a);
int change_item(struct args *a);
int compare_items(const struct item *ia, const struct item *ib);
int complete_date(char *s1, const char *s2);
int ctoi(const char c);
int date_to_time(time_t *t, const char *s);
int first_index_of(const char *s, const char c);
int is_today(const struct tm *tm);
int is_tomorrow(const struct tm *tm);
int last_index_of(const char *s, const char c);
int list_items(struct args *a);
int parse_date(time_t *t, const char *s);
int read_args(struct args *a, const int argc, char *argv[]);
int read_args_short(struct args *a, const int argc, char *argv[]);
int remove_item(struct args *a);
int sort_items(const void *a, const void *b);
int write_item(struct args *a, const time_t from, const time_t to);
int write_item_to_stream(
	FILE *f,
	const char *tag,
	const char *what,
	const time_t from,
	time_t to);
int write_items(const struct item *items, const size_t n, unsigned int except);
size_t read_items(struct item *items);
void adjust_month(struct tm *tm, const char *s);
void fail(struct args *a, const char *e, const int print_usage);
void format_date(char *s, const time_t t1, const time_t t2);
void free_args(struct args *a);
void free_items(struct item *items, const size_t n);
void line_to_item(struct item *it, char *line);
void print_items(const struct item *items, const size_t n, const char *tag);
void set_year_and_month(char *year, char *month, const struct tm *tm);

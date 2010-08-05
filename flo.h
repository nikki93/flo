#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DATE_FORMAT "%Y-%m-%d %H:%M"
#define IS_DEADLINE(it) (it->from == 0 && it->to != 0)
#define IS_TODO(it) (it->from == 0 && it->to == 0)
#define ITEM_COUNT 128
#define LINE_LENGTH 1024

struct args {
	char *what;
	char *tag;
	char *from;
	char *to;
	int change;
	int remove;
	int id;
};

struct item {
	char *what;
	char *tag;
	time_t from;
	time_t to;
};

int read_args(struct args *a, const int argc, char *argv[]);
int read_args_short(struct args *a, const int argc, char *argv[]);
void free_args(struct args *a);
int list_items(struct args *a);
int add_item(struct args *a);
int change_item(struct args *a);
int remove_item(struct args *a);
int read_items(struct item *items);
int write_items(const struct item *items, const int n, int except);
int write_item(struct args *a, const time_t from, const time_t to);
int is_same_day(const struct tm *tm1, const struct tm *tm2);
void format_date(char *s, const size_t len, const time_t t1, const time_t t2);
void print_items(const struct item *items, const int n, const char *tag);
int sort_items(const void *a, const void *b);
void free_items(struct item *items, const int n);
int write_item_to_stream(
	FILE *f,
	const char *what,
	const char* tag,
	time_t from,
	time_t to);
void line_to_item(struct item *it, char *line);
int parse_datestr(time_t *t, const char *s);
int complete_datestr(char *s1, const char *s2);
int datestr_to_time(time_t *t, const char *s);
void inc_month(struct tm *tm, const char *s);
void set_year_and_month(char *year, char *month, const struct tm *tm);
int first_index_of(const char *s, const char c);
int last_index_of(const char *s, const char c);
void get_filename(char *s);
void fail(struct args *a, const char *e, const int print_usage);

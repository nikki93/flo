#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LIST_ITEMS 128
#define DATE_FORMAT "%Y-%m-%d %H:%M"
#define IS_DEADLINE(it) (it->from == 0 && it->to != 0)
#define IS_TODO(it) (it->from == 0 && it->to == 0)
#define IS_EVENT(it) (!IS_TODO(it) && !IS_DEADLINE(it))

struct args {
	char *what;
	char *at;
	char *from;
	char *to;
	int change;
	int remove;
	int id;
};

struct item {
	char *what;
	char *at;
	time_t from;
	time_t to;
};

static void fail(struct args *a, const char *e, int show_usage);

static int read_args(struct args *a, const int argc, char *argv[]) {
	char c;

	while ((c = getopt(argc, argv, "c:r:w:a:f:t:")) != -1) {
		switch (c) {
			case 'c':
				a->change = 1;
				a->id = atoi(optarg);
				break;
			case 'r':
				a->remove = 1;
				a->id = atoi(optarg);
				break;
			case 'w':
				a->what = malloc(strlen(optarg) + 1);
				strcpy(a->what, optarg);
				break;
			case 'a':
				a->at = malloc(strlen(optarg) + 1);
				strcpy(a->at, optarg);
				break;
			case 'f':
				a->from = malloc(strlen(optarg) + 1);
				strcpy(a->from, optarg);
				break;
			case 't':
				a->to = malloc(strlen(optarg) + 1);
				strcpy(a->to, optarg);
				break;
			case '?':
				return 0;
		}
	}

	return 1;
}

static void free_args(struct args *a) {
	free(a->what);
	free(a->at);
	free(a->from);
	free(a->to);
}

static void set_year_and_month(char *year, char *month, const struct tm *tm) {
	sprintf(year, "%d", 1900 + tm->tm_year);
	sprintf(month, "%02d", tm->tm_mon + 1);
}

/*
	For a short date format, if the user has typed in a day of the month
	that is before today, increase the month.
*/
static void inc_month(struct tm *tm, const char *s) {
	int day;
	char day_tmp[3];

	memset(day_tmp, 0, sizeof(day_tmp));

	if (s[0] == '0')
		memcpy(day_tmp, s, 1);
	else
		memcpy(day_tmp, s, 2);

	day = atoi(day_tmp);

	if (day < tm->tm_mday) {
		if (tm->tm_mon < 11)
			tm->tm_mon += 1;
		else {
			tm->tm_mon = 0;
			tm->tm_year++;
		}
	}
}

/*
	Add the current year or year and month to date string if the date
	specified is in a short format.
*/
static int complete_datestr(char *s1, const char *s2) {
	time_t t;
	struct tm *tm;
	char year[5];
	char month[3];

	time(&t);
	tm = localtime(&t);

	switch (strlen(s2)) {
		case 2:
		case 4:
		case 6:
			inc_month(tm, s2);
			set_year_and_month(year, month, tm);
			strcat(s1, year);
			strcat(s1, month);
			break;
		case 8:
			set_year_and_month(year, month, tm);
			strcat(s1, year);
			break;
		case 12:
			break;
		default:
			return 0;
	}

	strcat(s1, s2);

	switch (strlen(s2)) {
		case 2:
			strcat(s1, "00");
		case 4:
			strcat(s1, "00");
			break;
	}

	strcat(s1, "00");

	return 1;
}

static int datestr_to_time(time_t *t, const char *s) {
	struct tm tm;

	memset(&tm, 0, sizeof(struct tm));

	if (strptime(s, "%Y%m%d%H%M%S", &tm) == 0)
		return 0;
	else {
		tm.tm_isdst = -1;
		*t = mktime(&tm);

		return 1;
	}
}

static void get_filename(char *s) {
	sprintf(s, "%s/.flo", getenv("HOME"));
}

static int print_item_to_stream(
	FILE *f,
	const char *what,
	const char* at,
	time_t from,
	time_t to) {

	if (what != 0)
		fprintf(f, "%s", what);

	fprintf(f, "\t");

	if (at != 0)
		fprintf(f, "%s", at);

	fprintf(f, "\t");

	if (from != 0)
		fprintf(f, "%llu", (unsigned long long)from);

	fprintf(f, "\t");

	if (to != 0)
		fprintf(f, "%llu", (unsigned long long)to);

	fprintf(f, "\n");

	return 1;
}

static int write_to_file(struct args *a, const time_t from, const time_t to) {
	FILE *f;
	char fn[256];

	get_filename(fn);

	if ((f = fopen(fn, "a")) == NULL)
		return 0;

	print_item_to_stream(f, a->what, a->at, from, to);

	fclose(f);

	return 1;
}

static void fail(struct args *a, const char *e, const int show_usage) {
	if (e != NULL)
		puts(e);

	if (show_usage)
		puts("Usage: flo [-r id || [-c id] -w what | -a at | -f from | -t to]");

	free_args(a);

	exit(EXIT_FAILURE);
}

static int verify_args(const struct args *a) {
	if (a->what == 0)
		return 0;

	return 1;
}

static void print_items(const struct item* items, const int n) {
	int i;
	struct item *it;
	struct tm *tm;
	char s[17];

	for (i = 0; i < n; i++) {
		it = (struct item *)&items[i];

		if (IS_DEADLINE(it)) {
			tm = localtime(&it->to);
			strftime(s, sizeof(s), DATE_FORMAT, tm);
			printf("d% 3d  %s  %s", i, s, it->what);

			if (it->at != 0)
				printf("@%s", it->at);

			printf("\n");
		}
		else if (IS_TODO(it)) {
			printf("% 4d  %s", i, it->what);

			if (it->at != 0)
				printf("@%s", it->at);

			printf("\n");
		}
		else {
			tm = localtime(&it->from);
			strftime(s, sizeof(s), DATE_FORMAT, tm);
			printf("% 4d  %s  %s", i, s, it->what);

			if (it->at != 0)
				printf("@%s", it->at);

			printf("\n");

			if (it->to != 0) {
				tm = localtime(&it->to);
				strftime(s, sizeof(s), DATE_FORMAT, tm);
				printf("      %s\n", s);
			}
		}
	}
}

static void free_items(struct item *items) {
	int i;

	for (i = 0; i < LIST_ITEMS; i++) {
		if (items[i].what == 0)
			break;

		free(items[i].what);
		free(items[i].at);
	}

	free(items);
}

static void line_to_item(struct item *it, char *line) {
	int col;
	char *token = NULL;
	char *delims = "\t";

	for (col = 0; (token = strsep(&line, delims)) != NULL; col++) {
		switch (col) {
			case 0:
				if (strlen(token) > 0) {
					it->what = malloc(strlen(token) + 1);
					strcpy(it->what, token);
				}

				break;
			case 1:
				if (strlen(token) > 0) {
					it->at = malloc(strlen(token) + 1);
					strcpy(it->at, token);
				}

				break;
			case 2:
				it->from = atoi(token);
				break;
			case 3:
				it->to = atoi(token);
				break;
		}
	}
}

static int read_items(struct item *items) {
	char fn[256];
	FILE *f;
	int n;
	ssize_t read;
	char *line = NULL;
	size_t len = 0;

	get_filename(fn);

	if ((f = fopen(fn, "r")) == NULL)
		return 0;

	for (n = 0; (read = getline(&line, &len, f)) != -1; n++) {
		line[read - 1] = '\0'; /* remove newline */
		line_to_item(&items[n], line);
	}

	free(line);

	fclose(f);

	return n;
}

static int write_items(const struct item *items, const int n, int except) {
	char fn[256];
	FILE *f;
	int i;

	get_filename(fn);

	if ((f = fopen(fn, "w")) == NULL)
		return 0;

	for (i = 0; i < n; i++) {
		if (i == except)
			continue;

		print_item_to_stream(
			f,
			items[i].what,
			items[i].at,
			items[i].from,
			items[i].to);
	}

	fclose(f);

	return 1;
}

static int sort_items(const void *a, const void *b) {
	struct item *ia = (struct item *)a;
	struct item *ib = (struct item *)b;

	if (IS_EVENT(ia) && !IS_EVENT(ib))
		return -1;
	else if (IS_DEADLINE(ia) && !IS_DEADLINE(ib)) {
		if (IS_TODO(ib))
			return -1;
		else
			return ia->to > ib->from;
	}
	else if (IS_DEADLINE(ia) && IS_DEADLINE(ib))
		return ia->to > ib->to;
	else if (IS_TODO(ia) && !IS_TODO(ib))
		return 1;
	else
		return ia->from > ib->from;

	return 0;
}

static int list_items() {
	int n;
	struct item *items;

	items = (struct item *)calloc(LIST_ITEMS, sizeof(struct item));

	n = read_items(items);
	qsort(items, n, sizeof(struct item), sort_items);
	print_items(items, n);

	free_items(items);

	return EXIT_SUCCESS;
}

static int parse_date(time_t *t, const char *s) {
	char s2[15];
	memset(s2, 0, sizeof(s2));

	if (complete_datestr(s2, s) == 0)
		return 0;

	if (datestr_to_time(t, s2) == 0)
		return 0;

	return 1;
}

static int add_item(struct args *a) {
	time_t from = 0;
	time_t to = 0;

	if (verify_args(a) == 0)
		fail(a, NULL, 1);

	if (a->from != 0)
		if (parse_date(&from, a->from) == 0)
			fail(a, "Could not parse from-date.", 1);

	if (a->to != 0)
		if (parse_date(&to, a->to) == 0)
			fail(a, "Could not parse to-date.", 1);

	if (write_to_file(a, from, to) == 0)
		fail(a, "Could not write to ~/.flo.", 0);

	free_args(a);

	return list_items();
}

static int change_item(struct args *a) {
	int n;
	struct item *items;
	struct item *it;

	items = (struct item *)calloc(LIST_ITEMS, sizeof(struct item));

	n = read_items(items);

	if (a->id < 0 || a->id > n) {
		free_items(items);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);

	it = &items[a->id];

	if (a->what != 0) {
		it->what = malloc(strlen(a->what) + 1);
		strcpy(it->what, a->what);
	}

	if (a->at != 0) {
		it->at = malloc(strlen(a->at) + 1);
		strcpy(it->at, a->at);
	}

	if (a->from != 0)
		if (parse_date(&it->from, a->from) == 0)
			fail(a, "Could not parse from-date.", 1);

	if (a->to != 0)
		if (parse_date(&it->to, a->to) == 0)
			fail(a, "Could not parse to-date.", 1);

	write_items(items, n, -1);
	print_items(items, n);
	
	free_args(a);
	free_items(items);

	return EXIT_SUCCESS;
}

static int remove_item(struct args *a) {
	int n;
	struct item *items;

	items = (struct item *)calloc(LIST_ITEMS, sizeof(struct item));

	n = read_items(items);

	if (a->id < 0 || a->id > n) {
		free_items(items);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);

	write_items(items, n, a->id);

	free_args(a);
	free_items(items);

	return list_items();
}

int main(int argc, char *argv[]) {
	struct args a;

	if (argc < 2)
		return list_items();
	else {
		memset(&a, 0, sizeof(struct args));

		if (read_args(&a, argc, argv) == 0)
			fail(&a, NULL, 1);

		if (a.change != 0)
			return change_item(&a);
		else if (a.remove != 0)
			return remove_item(&a);
		else
			return add_item(&a);
	}
}

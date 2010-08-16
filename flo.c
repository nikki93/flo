#include "flo.h"

static int complete_date(char *s1, const char *s2);
static int ctoi(const char c);
static int date_diff(time_t t1, time_t t2);
static int date_to_time(time_t *t, const char *s);
static int last_index_of(const char *s, const char c);
static int parse_date(time_t *t, const char *s);
static int sort_items(const void *a, const void *b);
static int write_item(struct args *a, const time_t from, const time_t to);
static int write_item_to_stream(
	FILE *f,
	const char *what,
	const time_t from,
	time_t to);
static int write_items(const struct item *items, const size_t n, unsigned int except);
static size_t read_items(struct item *items);
static void adjust_month(struct tm *tm, const char *s);
static void format_date(char *s, const time_t t1, const time_t t2);
static void free_items(struct item *items, const size_t n);
static void line_to_item(struct item *it, char *line);
static void print_help();
static void print_items(const struct item *items, const size_t n);
static void set_year_and_month(char *year, char *month, const struct tm *tm);

int read_args(struct args *a, const int argc, char *argv[]) {
	int c;

	while ((c = getopt(argc, argv, "c:r:w:f:t:h")) != -1) {
		switch ((char)c) {
			case 'c':
				a->change = 1;
				a->id = strtol(optarg, NULL, 10);
				break;
			case 'r':
				a->remove = 1;
				a->id = strtol(optarg, NULL, 10);
				break;
			case 'w':
				a->what = malloc(strlen(optarg) + 1);
				strcpy(a->what, optarg);
				break;
			case 'f':
				a->from = malloc(strlen(optarg) + 1);
				strcpy(a->from, optarg);
				break;
			case 't':
				a->to = malloc(strlen(optarg) + 1);
				strcpy(a->to, optarg);
				break;
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
			case '?':
				return 0;
		}
	}

	return 1;
}

int read_args_short(struct args *a, const int argc, char *argv[]) {
	char line[LINE_LENGTH];
	char *rest = &line[0];
	int i;
	int ind;
	size_t n;

	memset(line, 0, sizeof(line));

	for (i = 1; i < argc; i++) {
		strcat(line, argv[i]);

		if (i != (argc - 1))
			strcat(line, " ");
	}

	ind = last_index_of(rest, '-');

	if (ind != -1) {
		n = strlen(rest) - ind - 1;
		a->to = calloc(n + 1, 1);
		strncpy(a->to, rest + ind + 1, n);
		rest[ind] = '\0';
	}

	ind = last_index_of(rest, ',');

	if (ind != -1) {
		n = strlen(rest) - ind - 1;
		a->from = calloc(n + 1, 1);
		strncpy(a->from, rest + ind + 1, n);
		rest[ind] = '\0';
	}

	a->what = calloc(strlen(rest) + 1, 1);
	strcpy(a->what, rest);	

	if (strlen(a->what) == 0)
		return 0;

	return 1;
}

void free_args(struct args *a) {
	free(a->what);
	free(a->from);
	free(a->to);
}

int list_items(struct args *a) {
	size_t n;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);
	qsort(items, n, sizeof(struct item), sort_items);
	print_items(items, n);

	if (a != NULL)
		free_args(a);

	free_items(items, n);

	return EXIT_SUCCESS;
}

int add_item(struct args *a) {
	time_t from = 0;
	time_t to = 0;

	if (a->what == NULL)
		fail(a, NULL, 1);

	if (a->from != NULL)
		if (parse_date(&from, a->from) == 0)
			fail(a, "Could not parse from-date.", 1);

	if (a->to != NULL)
		if (parse_date(&to, a->to) == 0)
			fail(a, "Could not parse to-date.", 1);

	if (write_item(a, from, to) == 0)
		fail(a, "Could not write to ~/.flo.", 0);

	free_args(a);

	return list_items(NULL);
}

int change_item(struct args *a) {
	size_t n;
	struct item *it;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || a->id >= n) {
		free_items(items, n);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);
	it = &items[a->id];

	if (a->what != NULL) {
		free(it->what);
		it->what = malloc(strlen(a->what) + 1);
		strcpy(it->what, a->what);
	}

	if (a->from != NULL) {
		if (strcmp(a->from, "r") == 0)
			it->from = 0;
		else
			if (parse_date(&it->from, a->from) == 0)
				fail(a, "Could not parse from-date.", 1);
	}

	if (a->to != NULL) {
		if (strcmp(a->to, "r") == 0)
			it->to = 0;
		else
			if (parse_date(&it->to, a->to) == 0)
				fail(a, "Could not parse to-date.", 1);
	}

	write_items(items, n, -1);
	free_args(a);
	free_items(items, n);

	return list_items(NULL);
}

int remove_item(struct args *a) {
	size_t n;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || a->id >= n) {
		free_items(items, n);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);
	write_items(items, n, a->id);
	free_args(a);
	free_items(items, n);

	return list_items(NULL);
}

static size_t read_items(struct item *items) {
	FILE *f;
	char line[LINE_LENGTH];
	char s[256];
	int n;

	GET_FILENAME(s);

	if ((f = fopen(s, "r")) == NULL)
		return 0;

	for (n = 0; (fgets(line, LINE_LENGTH, f)) != NULL; n++) {
		line[strlen(line) - 1] = '\0';
		line_to_item(&items[n], line);
	}

	fclose(f);

	return n;
}

static int write_items(const struct item *items, const size_t n, unsigned int except) {
	FILE *f;
	char s[256];
	unsigned int i;

	GET_FILENAME(s);

	if ((f = fopen(s, "w")) == NULL)
		return 0;

	for (i = 0; i < n; i++) {
		if (i == except)
			continue;

		write_item_to_stream(
			f,
			items[i].what,
			items[i].from,
			items[i].to);
	}

	fclose(f);

	return 1;
}

static int write_item(struct args *a, const time_t from, const time_t to) {
	FILE *f;
	char s[256];

	GET_FILENAME(s);

	if ((f = fopen(s, "a")) == NULL)
		return 0;

	write_item_to_stream(f, a->what, from, to);
	fclose(f);

	return 1;
}

static void format_date(char *s, const time_t t1, const time_t t2) {
	struct tm *tm, tm1, tm2;

	tm = localtime(&t1);
	memcpy(&tm1, tm, sizeof(struct tm));

	if (t2 != 0) {
		tm = localtime(&t2);
		memcpy(&tm2, tm, sizeof(struct tm));

		if (ARE_DATES_EQUAL(&tm2, &tm1)) {
			strftime(
				s,
				DATE_FORMAT_LENGTH,
				DATE_FORMAT_DUPLICATE,
				&tm1);

			return;
		}
	}

	strftime(s, DATE_FORMAT_LENGTH, DATE_FORMAT, &tm1);
}

static int date_diff(time_t t1, time_t t2) {
	struct tm *tm, tm1, tm2;

	memset(&tm1, 0, sizeof(struct tm));
	memset(&tm2, 0, sizeof(struct tm));

	tm = localtime(&t1);
	memcpy(&tm1, tm, sizeof(struct tm));

	tm = localtime(&t2);
	memcpy(&tm2, tm, sizeof(struct tm));

	tm1.tm_sec = 0;
	tm1.tm_min = 0;
	tm1.tm_hour = 0;

	tm2.tm_sec = 0;
	tm2.tm_min = 0;
	tm2.tm_hour = 0;

	return (mktime(&tm1) - mktime(&tm2)) / 60 / 60 / 24;
}

static void print_items(const struct item *items, const size_t n) {
	char s[DATE_FORMAT_LENGTH];
	unsigned int i;
	struct item *it;
	int d;

	for (i = 0; i < n; i++) {
		it = (struct item *)&items[i];

		if (IS_TODO(it)) {
			printf("t% 3d  %s\n", i, it->what);
		}
		else if (IS_DEADLINE(it)) {
			d = date_diff(it->to, time(NULL));
			format_date(s, it->to, 0);

			if (d >= 0 && d < 10)
				printf(
					"d% 3d  %s  d%d  %s\n",
					i,
					s,
					d,
					it->what);
			else
				printf("d% 3d  %s      %s\n", i, s, it->what);
		}
		else {
			d = date_diff(it->from, time(NULL));
			format_date(s, it->from, 0);

			if (d >= 0 && d < 10)
				printf(
					"% 4d  %s  d%d  %s\n",
					i,
					s,
					d,
					it->what);
			else
				printf("% 4d  %s      %s\n", i, s, it->what);

			if (it->to != 0) {
				d = date_diff(it->to, time(NULL));
				format_date(s, it->to, it->from);

				if (d >= 0 && d < 10)
					printf("      %s  d%d\n", s, d);
				else
					printf("      %s    \n", s);
			}
		}
	}
}

static int sort_items(const void *a, const void *b) {
	struct item *ia = (struct item *)a;
	struct item *ib = (struct item *)b;

	if (IS_TODO(ia)) {
		if (IS_TODO(ib))
			return strcmp(ia->what, ib->what);
		else
			return 1;
	}
	else if (IS_DEADLINE(ia)) {
		if (IS_TODO(ib))
			return -1;
		else if (IS_DEADLINE(ib)) {
			if (ia->to > ib->to)
				return 1;
			else if (ia->to < ib->to)
				return -1;
			else
				return 0;
		}
		else {
			if (ia->to > ib->from)
				return 1;
			else if (ia->to < ib->from)
				return -1;
			else
				return 0;
		}
	}
	else {
		if (IS_TODO(ib))
			return -1;
		else if (IS_DEADLINE(ib)) {
			if (ia->from > ib->to)
				return 1;
			else if (ia->from < ib->to)
				return -1;
			else
				return 0;
		}
		else {
			if (ia->from > ib->from)
				return 1;
			else if (ia->from < ib->from)
				return -1;
			else
				return 0;
		}
	}
}

static void free_items(struct item *items, const size_t n) {
	unsigned int i;

	for (i = 0; i < n; i++) {
		free(items[i].what);
	}

	free(items);
}

static int write_item_to_stream(
	FILE *f,
	const char *what,
	time_t from,
	time_t to) {

	if (what != NULL)
		fprintf(f, "%s", what);

	fprintf(f, "\t");

	if (from != 0)
		fprintf(f, "%ld", from);

	fprintf(f, "\t");

	if (to != 0)
		fprintf(f, "%ld", to);

	fprintf(f, "\n");

	return 1;
}

static void line_to_item(struct item *it, char *line) {
	char *delims = "\t";
	char *token = NULL;
	int col;

	for (col = 0; (token = strsep(&line, delims)) != NULL; col++) {
		switch (col) {
			case 0:
				if (strlen(token) > 0) {
					it->what = malloc(strlen(token) + 1);
					strcpy(it->what, token);
				}

				break;
			case 1:
				it->from = strtol(token, NULL, 10);
				break;
			case 2:
				it->to = strtol(token, NULL, 10);
				break;
		}
	}
}

static int parse_date(time_t *t, const char *s) {
	char s2[15];

	memset(s2, 0, sizeof(s2));

	if (complete_date(s2, s) == 0)
		return 0;

	if (date_to_time(t, s2) == 0)
		return 0;

	return 1;
}

static int complete_date(char *s1, const char *s2) {
	char day[3] = "";
	char month[3];
	char year[5];
	struct tm *tm;
	time_t t;

	time(&t);

	switch (strlen(s2)) {
		case 2:
		case 4:
		case 6:
			if (s2[0] == 'd') {
				t += ctoi(s2[1]) * 86400;
				tm = localtime(&t);
				sprintf(day, "%02d", (int)tm->tm_mday);
			}
			else {
				tm = localtime(&t);
				adjust_month(tm, s2);
			}

			set_year_and_month(year, month, tm);
			strcat(s1, year);
			strcat(s1, month);
			break;
		case 8:
			tm = localtime(&t);
			set_year_and_month(year, month, tm);
			strcat(s1, year);
			break;
		case 12:
			break;
		default:
			return 0;
	}

	strcat(s1, s2);

	if (day[0] != '\0') {
		memcpy(s1 + 6, day, 2);
	}

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

static int date_to_time(time_t *t, const char *s) {
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

void adjust_month(struct tm *tm, const char *s) {
	char day[3];

        strncpy(day, s, 2);

	if (strtol(day, NULL, 10) < tm->tm_mday) {
		if (tm->tm_mon < 11)
			tm->tm_mon += 1;
		else {
			tm->tm_mon = 0;
			tm->tm_year++;
		}
	}
}

static int ctoi(const char c) {
	char s[2];

	s[0] = c;
	s[1] = '\0';

	return strtol(s, NULL, 10);
}

static void set_year_and_month(char *year, char *month, const struct tm *tm) {
	sprintf(year, "%d", 1900 + tm->tm_year);
	sprintf(month, "%02d", tm->tm_mon + 1);
}

static int last_index_of(const char *s, const char c) {
	int i;

	for (i = strlen(s) - 1; i >= 0; i--)
		if (s[i] == c)
			return i;

	return -1;
}

void fail(struct args *a, const char *e, const int print_help_hint) {
	if (e != NULL)
		puts(e);

	if (print_help_hint) {
		puts("Try “flo -h” for more information.");
	}

	free_args(a);

	exit(EXIT_FAILURE);
}

static void print_help() {
	puts("Add item\n\
    flo what[,from][-to]\n\
\n\
    flo -w what [-f from | -t to]\n\
\n\
List items\n\
    flo\n\
\n\
Remove item\n\
    flo -r id\n\
\n\
Change item\n\
    For -f from and -t to, r as value removes the field.\n\
\n\
    flo -c id -w what | -f from | -t to");
	puts("\n\
Date formats\n\
    YYYYMMDDhhmm\n\
        MMDDhhmm\n\
          DDhhmm\n\
          DDhh\n\
          DD\n\
\n\
    Replace DD with dn to set the date n days from today’s date.\n\
\n\
    If the year or the month isn’t specified, the current year and month is\n\
    used. For formats without a month, if the date specified is before today’s\n\
    date, the month is set to the next month.\n\
\n\
    The value for hours and minutes is set to 00 if no other value is specified.");
}

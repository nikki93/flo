#include "flo.h"

static int add_item(struct args *);
static int change_item(struct args *);
static int complete_date(char *, const char *);
static int date_diff(time_t, time_t);
static int date_to_time(time_t *, const char *);
static int last_index_of(const char *, const char);
static int list_items(const int);
static int parse_date(time_t *, const char *);
static int read_args(struct args *, const int, char **);
static int read_args_short(struct args *, const int, char **);
static int remove_item(struct args *);
static int sort_items(const void *, const void *);
static int write_item(struct args *, const time_t, const time_t);
static int write_item_to_stream(FILE *, const char *, const time_t, time_t);
static int write_items(const struct item *, const size_t, unsigned int);
static size_t read_items(struct item *);
static void adjust_month(struct tm *, const char *);
static void fail(struct args *, const char *, const int);
static void format_date(char *, const time_t, const time_t);
static void free_args(struct args *);
static void free_items(struct item *, const size_t);
static void get_year_and_month(char *, char *, const struct tm *);
static void line_to_item(struct item *, char *);
static void print_items(const struct item *, const size_t, const int);
static void usage();

static int read_args(struct args *a, const int argc, char **argv) {
	char c;

	while ((c = getopt(argc, argv, "w:f:t:r:c:")) != -1) {
		switch (c) {
			case 'w':
				a->what = strdup(optarg);
				break;
			case 'f':
				a->from = strdup(optarg);
				break;
			case 't':
				a->to = strdup(optarg);
				break;
			case 'r':
				a->flag = ARGS_REMOVE;

				if (!sscanf(optarg, "%u", &a->id))
					fail(a, "Id is not valid.", 0);

				break;
			case 'c':
				a->flag = ARGS_CHANGE;

				if (!sscanf(optarg, "%u", &a->id))
					fail(a, "Id is not valid.", 0);

				break;
			case '?':
				return 0;
		}
	}

	return 1;
}

static int read_args_short(struct args *a, const int argc, char **argv) {
	char line[LINE_BUFFER_SIZE] = "";
	char *rest = &line[0];
	int i;
	int ind;
	size_t n;

	for (i = 1; i < argc; i++) {
		strcat(line, argv[i]);

		if (i != argc - 1)
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

static int list_items(const int list_all) {
	size_t n;
	struct item *items;

	items = (struct item *)malloc(sizeof(struct item) * ITEM_BUFFER_SIZE);

	n = read_items(items);
	qsort(items, n, sizeof(struct item), sort_items);
	print_items(items, n, list_all);
	free_items(items, n);

	return EXIT_SUCCESS;
}

static int add_item(struct args *a) {
	time_t from = 0;
	time_t to = 0;

	if (a->what == NULL)
		fail(a, NULL, 1);

	if (a->from != NULL)
		if (parse_date(&from, a->from) == 0)
			fail(a, "From-date is not valid.", 0);

	if (a->to != NULL)
		if (parse_date(&to, a->to) == 0)
			fail(a, "To-date is not valid.", 0);

	if (write_item(a, from, to) == 0)
		fail(a, "File can not be written to.", 0);

	puts("Ids are updated.");

	free_args(a);

	return EXIT_SUCCESS;
}

static int change_item(struct args *a) {
	size_t n;
	struct item *it;
	struct item *items;

	items = (struct item *)malloc(sizeof(struct item) * ITEM_BUFFER_SIZE);

	n = read_items(items);

	if (n == 0 || a->id >= n) {
		free_items(items, n);
		fail(a, "Item does not exist.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);
	it = &items[a->id];

	if (a->what != NULL) {
		free(it->what);
		it->what = strdup(a->what);
	}

	if (a->from != NULL) {
		if (strcmp(a->from, "r") == 0)
			it->from = 0;
		else
			if (parse_date(&it->from, a->from) == 0)
				fail(a, "From-date is not valid.", 0);
	}

	if (a->to != NULL) {
		if (strcmp(a->to, "r") == 0)
			it->to = 0;
		else
			if (parse_date(&it->to, a->to) == 0)
				fail(a, "To-date is not valid.", 0);
	}

	write_items(items, n, -1);

	puts("Ids might be updated.");

	free_args(a);
	free_items(items, n);

	return EXIT_SUCCESS;
}

static int remove_item(struct args *a) {
	size_t n;
	struct item *items;

	items = (struct item *)malloc(sizeof(struct item) * ITEM_BUFFER_SIZE);

	n = read_items(items);

	if (n == 0 || a->id >= n) {
		free_items(items, n);
		fail(a, "Item does not exist.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);
	write_items(items, n, a->id);

	if (a->id != n - 1 && n != 1)
		puts("Ids are updated.");

	free_args(a);
	free_items(items, n);

	return EXIT_SUCCESS;
}

static size_t read_items(struct item *items) {
	FILE *f;
	char line[LINE_BUFFER_SIZE];
	char s[256];
	int n;

	GET_FILENAME(s);

	if ((f = fopen(s, "r")) == NULL)
		return 0;

	for (n = 0; (fgets(line, LINE_BUFFER_SIZE, f)) != NULL; n++) {
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
				FORMAT_DATE_LENGTH,
				FORMAT_DATE_DUPLICATE,
				&tm1);

			return;
		}
	}

	strftime(s, FORMAT_DATE_LENGTH, FORMAT_DATE, &tm1);
}

static int date_diff(time_t t1, time_t t2) {
	struct tm *tm, tm1, tm2;

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

	return (mktime(&tm1) - mktime(&tm2)) / 86400;
}

static void print_items(const struct item *items, const size_t n, const int list_all) {
	char s[FORMAT_DATE_LENGTH];
	unsigned int i;
	struct item *it;
	int d;
        int j = 0;

	for (i = 0; i < n; i++) {
		it = (struct item *)&items[i];

		if (IS_TODO(it))
			printf(FORMAT_TODO, 3, i, it->what);
		else if (IS_DEADLINE(it)) {
			if (!list_all) {
				if (j == ITEM_COUNT)
					continue;

				j++;
			}

			d = date_diff(it->to, time(NULL));
			format_date(s, it->to, 0);

			if (d >= 0 && d < 10)
				printf(FORMAT_DEADLINE_D, 3, i, s, d, it->what);
			else
				printf(FORMAT_DEADLINE, 3, i, s, it->what);
		}
		else {
			if (!list_all) {
				if (j == ITEM_COUNT)
					continue;

				j++;
			}

			d = date_diff(it->from, time(NULL));
			format_date(s, it->from, 0);

			if (d >= 0 && d < 10)
				printf(FORMAT_EVENT_D, 4, i, s, d, it->what);
			else
				printf(FORMAT_EVENT, 4, i, s, it->what);

			if (it->to != 0) {
				d = date_diff(it->to, time(NULL));
				format_date(s, it->to, it->from);

				if (d >= 0 && d < 10)
					printf(FORMAT_EVENT_TO_D, s, d);
				else
					printf(FORMAT_EVENT_TO, s);
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

	for (i = 0; i < n; i++)
		free(items[i].what);

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
				t += (s2[1] - '0') * 86400;
				tm = localtime(&t);
				sprintf(day, "%02d", (int)tm->tm_mday);
			}
			else {
				tm = localtime(&t);
				adjust_month(tm, s2);
			}

			get_year_and_month(year, month, tm);
			strcat(s1, year);
			strcat(s1, month);
			break;
		case 8:
			tm = localtime(&t);
			get_year_and_month(year, month, tm);
			strcat(s1, year);
			break;
		case 12:
			break;
		default:
			return 0;
	}

	strcat(s1, s2);

	if (day[0] != '\0')
		memcpy(s1 + 6, day, 2);

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

static void get_year_and_month(char *year, char *month, const struct tm *tm) {
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

void fail(struct args *a, const char *e, const int print_usage) {
	if (e != NULL)
		puts(e);

	if (print_usage)
		usage();

	free_args(a);

	exit(EXIT_FAILURE);
}

static void usage() {
	puts("usage: flo [-a] [-c id] [-f from] [-r id] [-t to] [-w what] [what[,from][-to]]");
}

int main(int argc, char **argv) {
	struct args a;

	if (argc < 2)
		return list_items(0);
	else if (strcmp(argv[1], "-a") == 0)
		return list_items(1);
	else if (strcmp(argv[1], "-h") == 0) {
		usage();
		return EXIT_SUCCESS;
	}
	else {
		memset(&a, 0, sizeof(struct args));

		if (argv[1][0] != '-') {
			if (!read_args_short(&a, argc, argv))
				fail(&a, NULL, 1);

			return add_item(&a);
		}
		else {
			if (read_args(&a, argc, argv) == 0)
				fail(&a, NULL, 1);

			switch (a.flag) {
				case ARGS_ADD:
					return add_item(&a);
				case ARGS_REMOVE:
					return remove_item(&a);
				default:
					return change_item(&a);
			}
		}
	}
}

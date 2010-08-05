#include "flo.h"

int read_args(struct args *a, const int argc, char *argv[]) {
	char c;

	while ((c = getopt(argc, argv, "c:r:T:w:f:t:")) != -1) {
		switch (c) {
			case 'c':
				a->change = 1;
				a->id = atoi(optarg);
				break;
			case 'r':
				a->remove = 1;
				a->id = atoi(optarg);
				break;
			case 'T':
				a->tag = malloc(strlen(optarg) + 1);
				strcpy(a->tag, optarg);
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
			case '?':
				return 0;
		}
	}

	return 1;
}

/*
	Parses a short version of the input string.
*/
int read_args_short(struct args *a, const int argc, char *argv[]) {
	int i;
	char line[LINE_LENGTH];
	char *rest = &line[0];
	int n;
	size_t len;

	memset(line, 0, sizeof(line));

	for (i = 1; i < argc; i++) {
		strcat(line, argv[i]);

		if (i != (argc - 1))
			strcat(line, " ");
	}

	if (line[0] == '@') {
		n = first_index_of(line, ' ');

		if (n != -1) {
			a->tag = calloc(n, 1);
			strncpy(a->tag, &line[0] + 1, n - 1);
			rest = &line[0] + n + 1;
		}
		else
			return 0;
	}

	n = last_index_of(rest, '-');

	if (n != -1) {
		len = strlen(rest) - n - 1;
		a->to = calloc(len + 1, 1);
		strncpy(a->to, rest + n + 1, len);
		rest[n] = '\0';
	}

	n = last_index_of(rest, ',');

	if (n != -1) {
		len = strlen(rest) - n - 1;
		a->from = calloc(len + 1, 1);
		strncpy(a->from, rest + n + 1, len);
		rest[n] = '\0';
	}

	a->what = calloc(strlen(rest) + 1, 1);
	strcpy(a->what, rest);	

	return 1;
}

void free_args(struct args *a) {
	free(a->what);
	free(a->tag);
	free(a->from);
	free(a->to);
}

int list_items() {
	struct item *items;
	size_t n;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);
	qsort(items, n, sizeof(struct item), sort_items);
	print_items(items, n);

	free_items(items, n);

	return EXIT_SUCCESS;
}

int add_item(struct args *a) {
	time_t from = 0;
	time_t to = 0;

	if (a->what == 0)
		fail(a, NULL, 1);

	if (a->from != 0)
		if (parse_datestr(&from, a->from) == 0)
			fail(a, "Could not parse from-date.", 1);

	if (a->to != 0)
		if (parse_datestr(&to, a->to) == 0)
			fail(a, "Could not parse to-date.", 1);

	if (write_item(a, from, to) == 0)
		fail(a, "Could not write to ~/.flo.", 0);

	free_args(a);

	return list_items();
}

int change_item(struct args *a) {
	int n;
	struct item *items;
	struct item *it;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || (a->id < 0 || a->id >= n)) {
		free_items(items, n);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);

	it = &items[a->id];

	if (a->what != 0) {
		free(it->what);
		it->what = malloc(strlen(a->what) + 1);
		strcpy(it->what, a->what);
	}

	if (a->tag != 0) {
		free(it->tag);
		it->tag = malloc(strlen(a->tag) + 1);
		strcpy(it->tag, a->tag);
	}

	if (a->from != 0)
	{
		if (strcmp(a->from, "r") == 0) {
			it->from = 0;
		}
		else
			if (parse_datestr(&it->from, a->from) == 0)
				fail(a, "Could not parse from-date.", 1);
	}

	if (a->to != 0)
	{
		if (strcmp(a->to, "r") == 0)
			it->to = 0;
		else
			if (parse_datestr(&it->to, a->to) == 0)
				fail(a, "Could not parse to-date.", 1);
	}

	write_items(items, n, -1);
	
	free_args(a);
	free_items(items, n);

	return list_items();
}

int remove_item(struct args *a) {
	int n;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || (a->id < 0 || a->id > n)) {
		free_items(items, n);
		fail(a, "Could not find item.", 0);

		return EXIT_FAILURE;
	}

	qsort(items, n, sizeof(struct item), sort_items);

	write_items(items, n, a->id);

	free_args(a);
	free_items(items, n);

	return list_items();
}

int read_items(struct item *items) {
	char fn[256];
	FILE *f;
	int n;
	char line[LINE_LENGTH];

	get_filename(fn);

	if ((f = fopen(fn, "r")) == NULL)
		return 0;

	for (n = 0; (fgets(line, LINE_LENGTH, f)) != NULL; n++) {
		line_to_item(&items[n], line);
	}

	fclose(f);

	return n;
}

int write_items(const struct item *items, const int n, int except) {
	char fn[256];
	FILE *f;
	int i;

	get_filename(fn);

	if ((f = fopen(fn, "w")) == NULL)
		return 0;

	for (i = 0; i < n; i++) {
		if (i == except)
			continue;

		write_item_to_stream(
			f,
			items[i].what,
			items[i].tag,
			items[i].from,
			items[i].to);
	}

	fclose(f);

	return 1;
}

int write_item(struct args *a, const time_t from, const time_t to) {
	char fn[256];
	FILE *f;

	get_filename(fn);

	if ((f = fopen(fn, "a")) == NULL)
		return 0;

	write_item_to_stream(f, a->what, a->tag, from, to);

	fclose(f);

	return 1;
}

int is_same_day(const struct tm *tm1, const struct tm *tm2) {
	return ((tm1->tm_year == tm2->tm_year && tm1->tm_mon == tm2->tm_mon) &&
		tm1->tm_mday == tm2->tm_mday);
}

void format_date(char *s, const size_t len, const time_t t1, const time_t t2) {
	struct tm *tm, tm_t1, tm_t2, tm_now;
	time_t t_now;

	tm = localtime(&t1);
	memcpy(&tm_t1, tm, sizeof(struct tm));

	t_now = time(NULL);
	tm = localtime(&t_now);
	memcpy(&tm_now, tm, sizeof(struct tm));

	if (t2 != 0) {
		tm = localtime(&t2);
		memcpy(&tm_t2, tm, sizeof(struct tm));

		if (is_same_day(&tm_t2, &tm_t1)) {
			strftime(s, len, "           %H:%M", &tm_t1);
			return;
		}
	}

	if (is_same_day(&tm_t1, &tm_now))
		strftime(s, len, "     today %H:%M", &tm_t1);
	else
		strftime(s, len, DATE_FORMAT, &tm_t1);
}

void print_items(const struct item *items, const int n) {
	int i;
	struct item *it;
	char s[17];

	for (i = 0; i < n; i++) {
		it = (struct item *)&items[i];

		if (IS_TODO(it)) {
			printf("t% 3d  ", i);

			if (it->tag != 0)
				printf("@%s ", it->tag);

			printf("%s\n", it->what);
		}
		else if (IS_DEADLINE(it)) {
			format_date(s, sizeof(s), it->to, 0);
			printf("d% 3d  %s  ", i, s);

			if (it->tag != 0)
				printf("@%s ", it->tag);

			printf("%s\n", it->what);
		}
		else {
			format_date(s, sizeof(s), it->from, 0);
			printf("% 4d  %s  ", i, s);

			if (it->tag != 0)
				printf("@%s ", it->tag);

			printf("%s\n", it->what);

			if (it->to != 0) {
				format_date(s, sizeof(s), it->to, it->from);
				printf("      %s\n", s);
			}
		}
	}
}

int sort_items(const void *a, const void *b) {
	struct item *ia = (struct item *)a;
	struct item *ib = (struct item *)b;

	if (IS_TODO(ia)) {
		if (IS_TODO(ib))
			return 0;
		else
			return 1;
	}
	else if (IS_DEADLINE(ia)) {
		if (IS_TODO(ib))
			return -1;
		else if (IS_DEADLINE(ib))
			return ia->to > ib->to;
		else
			return ia->to > ib->from;
	}
	else {
		if (IS_TODO(ib))
			return -1;
		else if (IS_DEADLINE(ib))
			return ia->from > ib->to;
		else
			return ia->from > ib->from;
	}
}

void free_items(struct item *items, const int n) {
	int i;

	for (i = 0; i < n; i++) {
		free(items[i].what);
		free(items[i].tag);
	}

	free(items);
}

int write_item_to_stream(
	FILE *f,
	const char *what,
	const char* tag,
	time_t from,
	time_t to) {

	if (what != 0)
		fprintf(f, "%s", what);

	fprintf(f, "\t");

	if (tag != 0)
		fprintf(f, "%s", tag);

	fprintf(f, "\t");

	if (from != 0)
		fprintf(f, "%ld", from);

	fprintf(f, "\t");

	if (to != 0)
		fprintf(f, "%ld", to);

	fprintf(f, "\n");

	return 1;
}

void line_to_item(struct item *it, char *line) {
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
					it->tag = malloc(strlen(token) + 1);
					strcpy(it->tag, token);
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

int parse_datestr(time_t *t, const char *s) {
	char s2[15];
	memset(s2, 0, sizeof(s2));

	if (complete_datestr(s2, s) == 0)
		return 0;

	if (datestr_to_time(t, s2) == 0)
		return 0;

	return 1;
}

/*
	Add the current year or year and month to date string if the date
	specified is in a short format.
*/
int complete_datestr(char *s1, const char *s2) {
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

int datestr_to_time(time_t *t, const char *s) {
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

/*
	For a short date format, if the user has typed in a day of the month
	that is before today, increase the month.
*/
void inc_month(struct tm *tm, const char *s) {
	int day;
	char day_tmp[3];

	memset(day_tmp, 0, sizeof(day_tmp));

	if (s[0] == '0')
		day_tmp[0] = s[1];
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

void set_year_and_month(char *year, char *month, const struct tm *tm) {
	sprintf(year, "%d", 1900 + tm->tm_year);
	sprintf(month, "%02d", tm->tm_mon + 1);
}

int first_index_of(const char *s, const char c) {
	unsigned int i;

	for (i = 0; i < strlen(s); i++) {
		if (s[i] == c)
			return i;
	}

	return -1;
}

int last_index_of(const char *s, const char c) {
	int i;

	for (i = strlen(s) - 1; i >= 0; i--) {
		if (s[i] == c)
			return i;
	}

	return -1;
}

void get_filename(char *s) {
	sprintf(s, "%s/.flo", getenv("HOME"));
}

void fail(struct args *a, const char *e, const int print_usage) {
	if (e != NULL)
		puts(e);

	if (print_usage)
    		puts("Usage: flo [[@tag ]what[,from][-to] || [-c id] [-T tag] -\
w what [-f from | -t to] || -r id]");

	free_args(a);

	exit(EXIT_FAILURE);
}

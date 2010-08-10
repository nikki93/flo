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
	char line[LINE_LENGTH];
	char *rest = &line[0];
	int i;
	int n;
	size_t len;

	memset(line, 0, sizeof(line));

	for (i = 1; i < argc; i++) {
		strcat(line, argv[i]);

		if (i != (argc - 1))
			strcat(line, " ");
	}

	if (line[0] == '.') {
		n = first_index_of(line, ' ');

		if (n != -1) {
			a->tag = calloc(n, 1);
			strncpy(a->tag, &line[0] + 1, n - 1);
			rest = &line[0] + n + 1;
		}
		else if (strlen(line) > 1) {
			a->tag = calloc(strlen(line), 1);
			strncpy(a->tag, &line[1], strlen(line) - 1);

			return 2;
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

int list_items(struct args *a) {
	size_t n;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);
	qsort(items, n, sizeof(struct item), sort_items);

	if (a != NULL) {
		print_items(items, n, a->tag);
		free_args(a);
	}
	else
		print_items(items, n, NULL);

	free_items(items, n);

	return EXIT_SUCCESS;
}

int add_item(struct args *a) {
	time_t from = 0;
	time_t to = 0;

	if (a->what == NULL)
		fail(a, NULL, 1);

	if (a->from != NULL)
		if (parse_datestr(&from, a->from) == 0)
			fail(a, "Could not parse from-date.", 1);

	if (a->to != NULL)
		if (parse_datestr(&to, a->to) == 0)
			fail(a, "Could not parse to-date.", 1);

	if (write_item(a, from, to) == 0)
		fail(a, "Could not write to ~/.flo.", 0);

	free_args(a);

	return list_items(NULL);
}

int change_item(struct args *a) {
	int n;
	struct item *it;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || (a->id < 0 || a->id >= n)) {
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

	if (a->tag != NULL) {
		if (strcmp(a->tag, "r") == 0) {
			free(it->tag);
			it->tag = NULL;
		}
		else {
			free(it->tag);
			it->tag = malloc(strlen(a->tag) + 1);
			strcpy(it->tag, a->tag);
		}
	}

	if (a->from != NULL) {
		if (strcmp(a->from, "r") == 0)
			it->from = 0;
		else
			if (parse_datestr(&it->from, a->from) == 0)
				fail(a, "Could not parse from-date.", 1);
	}

	if (a->to != NULL) {
		if (strcmp(a->to, "r") == 0)
			it->to = 0;
		else
			if (parse_datestr(&it->to, a->to) == 0)
				fail(a, "Could not parse to-date.", 1);
	}

	write_items(items, n, -1);
	free_args(a);
	free_items(items, n);

	return list_items(NULL);
}

int remove_item(struct args *a) {
	int n;
	struct item *items;

	items = (struct item *)calloc(ITEM_COUNT, sizeof(struct item));

	n = read_items(items);

	if (n == 0 || (a->id < 0 || a->id >= n)) {
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

int read_items(struct item *items) {
	FILE *f;
	char fn[256];
	char line[LINE_LENGTH];
	int n;

	get_filename(fn);

	if ((f = fopen(fn, "r")) == NULL)
		return 0;

	for (n = 0; (fgets(line, LINE_LENGTH, f)) != NULL; n++)
		line_to_item(&items[n], line);

	fclose(f);

	return n;
}

int write_items(const struct item *items, const int n, int except) {
	FILE *f;
	char fn[256];
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
	FILE *f;
	char fn[256];

	get_filename(fn);

	if ((f = fopen(fn, "a")) == NULL)
		return 0;

	write_item_to_stream(f, a->what, a->tag, from, to);
	fclose(f);

	return 1;
}

int is_today(const struct tm *tm) {
	time_t t;

	t = time(NULL);

	return ARE_DATES_EQUAL(localtime(&t), tm);
}

int is_tomorrow(const struct tm *tm) {
	time_t t;

	t = time(NULL) + 172800;

	return ARE_DATES_EQUAL(localtime(&t), tm);
}

void format_date(char *s, const time_t t1, const time_t t2) {
	struct tm *tm, tm1, tm2;

	tm = localtime(&t1);
	memcpy(&tm1, tm, sizeof(struct tm));

	if (t2 != 0) {
		tm = localtime(&t2);
		memcpy(&tm2, tm, sizeof(struct tm));

		if (ARE_DATES_EQUAL(&tm2, &tm1)) {
			strftime(s, 17, "           %H:%M", &tm1);

			return;
		}
	}

	if (is_today(&tm1))
		strftime(s, 17, "     today %H:%M", &tm1);
	else if (is_tomorrow(&tm1))
		strftime(s, 17, "  tomorrow %H:%M", &tm1);
	else
		strftime(s, 17, "%Y-%m-%d %H:%M", &tm1);
}

void print_items(const struct item *items, const int n, const char *tag) {
	char s[17];
	int i;
	struct item *it;

	for (i = 0; i < n; i++) {
		it = (struct item *)&items[i];

		if (tag != NULL) {
			if (it->tag == 0)
				continue;

			if (strcmp(tag, it->tag) != 0)
				continue;
		}

		if (IS_TODO(it)) {
			printf("t% 3d  ", i);

			if (it->tag != NULL && tag == NULL)
				printf("%s: ", it->tag);

			printf("%s\n", it->what);
		}
		else if (IS_DEADLINE(it)) {
			format_date(s, it->to, 0);
			printf("d% 3d  %s  ", i, s);

			if (it->tag != NULL && tag == NULL)
				printf("%s: ", it->tag);

			printf("%s\n", it->what);
		}
		else {
			format_date(s, it->from, 0);
			printf("% 4d  %s  ", i, s);

			if (it->tag != NULL && tag == NULL)
				printf("%s: ", it->tag);

			printf("%s\n", it->what);

			if (it->to != 0) {
				format_date(s, it->to, it->from);
				printf("      %s\n", s);
			}
		}
	}
}

int compare_items(struct item *ia, struct item* ib) {
	int res;

	if (ia->tag != NULL && ib->tag != NULL) {
		res = strcmp(ia->tag, ib->tag);

		return res == 0 ? strcmp(ia->what, ib->what) : res;
	}
	else if (ia->tag != NULL && ib->tag == NULL)
		return -1;
	else if (ia->tag == NULL && ib->tag != NULL)
		return 1;
	else
		return strcmp(ia->what, ib->what);
}

int sort_items(const void *a, const void *b) {
	struct item *ia = (struct item *)a;
	struct item *ib = (struct item *)b;

	if (IS_TODO(ia)) {
		if (IS_TODO(ib))
			return compare_items(ia, ib);
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

	if (what != NULL)
		fprintf(f, "%s", what);

	fprintf(f, "\t");

	if (tag != NULL)
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
			if (s2[0] == 't' && s2[1] == 'd') {
				tm = localtime(&t);
				sprintf(day, "%02d", (int)tm->tm_mday);
			}
			else if (s2[0] == 't' && s2[1] == 'm') {
				t += 172800;
				tm = localtime(&t);
				sprintf(day, "%02d", (int)tm->tm_mday);
			}
			else if (s2[0] == '+') {
				t += ctoi(s2[1]) * 86400;
				tm = localtime(&t);
				sprintf(day, "%02d", (int)tm->tm_mday);
			}
			else {
				tm = localtime(&t);
				inc_month(tm, s2);
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
	char day_tmp[3];
	int day;

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

int ctoi(const char c) {
	char s[2];

	s[0] = c;
	s[1] = '\0';

	return atoi(s);
}

void set_year_and_month(char *year, char *month, const struct tm *tm) {
	sprintf(year, "%d", 1900 + tm->tm_year);
	sprintf(month, "%02d", tm->tm_mon + 1);
}

int first_index_of(const char *s, const char c) {
	unsigned int i;

	for (i = 0; i < strlen(s); i++)
		if (s[i] == c)
			return i;

	return -1;
}

int last_index_of(const char *s, const char c) {
	int i;

	for (i = strlen(s) - 1; i >= 0; i--)
		if (s[i] == c)
			return i;

	return -1;
}

void get_filename(char *s) {
	sprintf(s, "%s/.flo", getenv("HOME"));
}

void fail(struct args *a, const char *e, const int print_usage) {
	if (e != NULL)
		puts(e);

	if (print_usage)
    		puts("Usage: flo [.tag |what[,from][-to] || [-c id] [-T tag] -w\
 what [-f from | -t to] || -r id]");

	free_args(a);

	exit(EXIT_FAILURE);
}

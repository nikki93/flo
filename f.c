#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct args {
	char *what;
	char *at;
	char *from;
	char *to;
};

struct item {
	char *what;
	char *at;
	time_t from;
	time_t to;
};

static void fail(struct args *a, const char *e, int show_usage);

static int read_args(struct args *a, int argc, char *argv[]) {
	char c;

	while ((c = getopt(argc, argv, "w:a:f:t:")) != -1) {
		switch (c) {
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
		if (tm->tm_mon < 11) {
			tm->tm_mon += 1;
		}
		else {
			tm->tm_mon = 0;
			tm->tm_year++;
		}
	}
}

/*
	Add the current year or year and month to date string if the date specified
	is in a short format.
*/
static int complete_datestr(char *ds, char *s) {
	time_t t;
	struct tm *tm;
	char year[5];
	char month[3];

	time(&t);
	tm = localtime(&t);

	switch (strlen(s)) {
		case 8:
			set_year_and_month(year, month, tm);
			strcat(ds, year);
			break;
		case 6:
		case 4:
		case 2:
			inc_month(tm, s);
			set_year_and_month(year, month, tm);
			strcat(ds, year);
			strcat(ds, month);
			break;
		case 12:
			break;
		default:
			return 0;
	}

	strcat(ds, s);

	switch (strlen(s)) {
		case 2:
			strcat(ds, "00");
		case 4:
			strcat(ds, "00");
			break;
	}

	strcat(ds, "00");

	return 1;
}

static int datestr_to_time(time_t *t, char *ds) {
	struct tm tm;

	memset(&tm, 0, sizeof(struct tm));

	if (strptime(ds, "%Y%m%d%H%M%S", &tm) == 0)
		return 0;
	else {
		tm.tm_isdst = -1;
		*t = mktime(&tm);
		return 1;
	}
}

static void get_filename(char *s) {
	sprintf(s, "%s/.f", getenv("HOME"));
}

static int write_to_file(struct args *a, time_t from, time_t to) {
	FILE *f;
	char fn[256];

	memset(fn, 0, sizeof(fn));

	get_filename(fn);

	if ((f = fopen(fn, "a")) == NULL)
		return 0;

	if (a->what != 0) {
		fprintf(f, "%s", a->what);
	}

	fprintf(f, "\t");

	if (a->at != 0) {
		fprintf(f, "%s", a->at);
	}

	fprintf(f, "\t");

	if (from != 0) {
		fprintf(f, "%d", from);
	}

	fprintf(f, "\t");

	if (to != 0) {
		fprintf(f, "%d", to);
	}

	fprintf(f, "\n");
	fclose(f);

	return 1;
}

static void fail(struct args *a, const char *e, int show_usage) {
	if (e != NULL)
		puts(e);

	if (show_usage)
		puts("Usage: f [-w what [-a at -f [[YYYY]MM]DD[hh[mm]] -t [[YYYY]MM]DD[hh[mm]]]]");

	free_args(a);

	exit(EXIT_FAILURE);
}

static int verify_args(struct args *a) {
	if (a->what == 0)
		return 0;

	return 1;
}

static void list_items(struct item* items) {
	int i;
	struct item it;
	struct tm *tm;
	char s[17];

	for (i = 0; i < 1024; i++) {
		it = items[i];

		if (it.what == NULL)
			break;

		if (it.from != 0) {
			tm = localtime(&it.from);
			strftime(s, sizeof(s), "%Y-%m-%d %H:%M", tm);
			printf("%s", s);
		}

		if (it.to != 0) {
			printf("--");
			tm = localtime(&it.to);
			strftime(s, sizeof(s), "%Y-%m-%d %H:%M", tm);
			printf("%s", s);
		}

		if (it.from != 0 || it.to != 0) {
			printf(": ");
		}

		printf("%s", it.what);

		if (it.at != NULL) {
			printf("@%s", items[i].at);
		}

		printf("\n");
	}
}

static void free_items(struct item* items) {
	int i;

	for (i = 0; i < 1024; i++) {
		if (items[i].what == 0)
			break;

		free(items[i].what);
		free(items[i].at);
	}
}

static void parse_read_item(struct item *item, char *line) {
	int col;
	char *token = NULL;
	char *delims = "\t";

	for (col = 0; (token = strsep(&line, delims)) != NULL; col++) {
		switch (col) {
			case 0:
				if (strlen(token) > 0) {
					item->what = malloc(strlen(token) + 1);
					strcpy(item->what, token);
				}

				break;
			case 1:
				if (strlen(token) > 0) {
					item->at = malloc(strlen(token) + 1);
					strcpy(item->at, token);
				}

				break;
			case 2:
				item->from = atoi(token);
				break;
			case 3:
				item->to = atoi(token);
				break;
		}
	}
}

static int read_items(struct item* items) {
	char fn[256];
	FILE *f;
	int i;
	ssize_t read;
	char *line = NULL;
	size_t len = 0;

	memset(fn, 0, sizeof(fn));

	get_filename(fn);

	if ((f = fopen(fn, "r")) == NULL)
		return 0;

	for (i = 0; (read = getline(&line, &len, f)) != -1; i++) {
		line[read - 1] = '\0'; /* remove newline */

		parse_read_item(&items[i], line);
	}

	fclose(f);
}

static int main_list_items() {
	int i;
	struct item *items;

	items = (struct item *)calloc(1024, sizeof(struct item));

	read_items(items);
	list_items(items);
	free_items(items);

	return EXIT_SUCCESS;
}

static int main_add_item(int argc, char *argv[]) {
	struct args a;
	char s[15];
	time_t from = 0;
	time_t to = 0;

	memset(&a, 0, sizeof(struct args));

	if (read_args(&a, argc, argv) == 0)
		fail(&a, "Could not parse arguments.", 1);

	if (verify_args(&a) == 0)
		fail(&a, NULL, 1);

	memset(&s, 0, sizeof(s));

	if (a.from != 0) {
		if (complete_datestr(s, a.from) == 0)
			fail(&a, "Could not complete from-date.", 1);

		if (datestr_to_time(&from, s) == 0)
			fail(&a, "Could not convert from-date string to time.", 1);
	}

	memset(&s, 0, sizeof(s));

	if (a.to != 0) {
		if (complete_datestr(s, a.to) == 0)
			fail(&a, "Could not complete to-date.", 1);

		if (datestr_to_time(&to, s) == 0)
			fail(&a, "Could not convert to-date string to time.", 1);
	}

	if (write_to_file(&a, from, to) == 0)
		fail(&a, "Could not write to ~/.f.", 0);

	free_args(&a);

	return main_list_items();
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return main_list_items();
	}
	else {
		return main_add_item(argc, argv);
	}
}

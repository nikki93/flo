#include "flo.h"

int main(int argc, char *argv[]) {
	struct args a;
	int res;

	if (argc < 2)
		return list_items();
	else {
		memset(&a, 0, sizeof(struct args));

		if (argv[1][0] != '-') {
			res = read_args_short(&a, argc, argv);

			if (res == 0)
				fail(&a, NULL, 1);

			return add_item(&a);
		}
		else {
			if (read_args(&a, argc, argv) == 0)
				fail(&a, NULL, 1);

			switch (a.flag) {
				case ARGS_REMOVE:
					return remove_item(&a);
				case ARGS_CHANGE:
					return change_item(&a);
				default:
					return add_item(&a);
			}
		}
	}
}

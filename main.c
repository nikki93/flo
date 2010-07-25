#include "flo.h"

int main(int argc, char *argv[]) {
	struct args a;

	if (argc < 2)
		return list_items();
	else {
		memset(&a, 0, sizeof(struct args));

		if (argv[1][0] != '-') {
			read_args_short(&a, argc, argv);

			return add_item(&a);
		}
		else {
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
}

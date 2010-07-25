#include "flo.h"
#include <assert.h>

static void test_read_args_short() {
	struct args a;
	int argc = 2;
	char* argv[2] = { "flo", "something@somewhere,201001011200" };

	memset(&a, 0, sizeof(struct args));

	assert(a.what == 0);
	assert(a.at == 0);
	assert(a.from == 0);
	assert(a.to == 0);

	read_args_short(&a, argc, argv);

	assert(strcmp(a.what, "something") == 0);
	assert(strcmp(a.at, "somewhere") == 0);
	assert(a.from != 0);
	assert(a.to == 0);

	free_args(&a);
}

int main(int argc, char *argv[]) {
	test_read_args_short();

	return EXIT_SUCCESS;
}

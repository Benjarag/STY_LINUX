#include "testlib.h"
#include "copy.h"
#include <stdio.h>
#include <assert.h>

static void _printUsage(char* argv[])
{
	fprintf(stderr, "Usage: %s [-b blocksize] from to\n", argv[0]);
}

int main(int argc, char* argv[])
{
	CopyArgs args;

	if (parseCopyArgs(argc, argv, &args) != 0) {
		_printUsage(argv);
		return 1;
	}

	assert(args.blocksize > 0);
	if (doCopy(&args) != 0) {
		return 1;
	}

	printf("All done.\n");
	return 0;
}

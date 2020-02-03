#include <stdio.h>
#include "Arguments.h"

int main(int argc, char** argv) {
	initializeFlags();

	setFlags(argc, argv);

	if (helpFlag) {
		printFlags();
		return 0;
	}
	return 0;
}

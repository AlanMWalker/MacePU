#include <stdio.h>
#include "error\ErrorUtils.h"

void pauseForReturnKey()
{
	printf("Hit return to close...\n");
	scanf_s("");
}

void handleError(const int8 * failedString)
{
	printf("%s", failedString);
	printf("\n");
	pauseForReturnKey();
}

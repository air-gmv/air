#include "AVT_dependent.h"


void
Initial () {

	RETURN_CODE_TYPE RetCode = 0;


  while (1) {
  	printf("Partition 1\n");
  	w_TIMED_WAIT(45000000ll, &RetCode);
  }

}

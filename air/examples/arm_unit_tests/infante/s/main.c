#include <air.h>

char message[] = {'h', 'e', 'l', 'l', 'o', ' ', 
    'I', 'N', 'F', 'A', 'N', 'T', 'E', '!', '\n'};

/**
 * @brief Partition entry point
 */
void entry_point() {

    air_syscall_print(message, sizeof(message));
    for(;;);
}

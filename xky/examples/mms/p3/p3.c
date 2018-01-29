
#include <air.h>
#include <bare.h>
#include <pprintf.h>

void entry_func() {

    int id;
    air_syscall_get_partition_id(NULL, &id);
    while (1) {
        pprintf("Partition %d: present in schedule B\n", id);
        wake_after(50);
    }
}

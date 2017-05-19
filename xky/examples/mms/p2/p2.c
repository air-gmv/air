
#include <xky.h>
#include <bare.h>
#include <pprintf.h>

void entry_func() {

    int id;
    xky_syscall_get_partition_id(NULL, &id);
    while (1) {
        pprintf("Partition %d: present in schedule A & B\n", id);
        wake_after(50);
    }
}

/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <air.h>
#include <bare.h>
#include <pprintf.h>

void entry_func() {

    int id;
    air_syscall_get_partition_id(NULL, &id);
    while (1) {
        pprintf("Partition %d: present in schedule A\n", id);
        wake_after(50);
    }
}

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

    air_partition_status_t status;

    while (1) {
        air_syscall_get_partition_status(-1, &status);
        pprintf("Partition %d: present in schedule B Window %d\n", status.identifier, status.window_id);
        wake_after(50);
    }
}

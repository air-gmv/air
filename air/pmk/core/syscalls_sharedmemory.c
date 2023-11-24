/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file syscalls_sharedarea.c
 * @author pfnf
 * @brief Shared-memory system calls
 */

#include <pmk.h>
#include <workspace.h>
#include <segregation.h>
#include <sharedmemory.h>

air_status_code_e pmk_syscall_get_sharedmemory(pmk_core_ctrl_t *core, air_name_ptr_t name,
                                               air_sharedmemory_t *sharedmemory)
{

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* copy shared memory name to PMK space */
    air_name_t l_name;
    if (pmk_segregation_copy_from_user(context, l_name, name, sizeof(air_name_t)) != 0)
    {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }

    /* check if shared memory area exists  */
    pmk_shm_t *shm = pmk_sharedmemory_get_by_name(name);
    if (NULL == shm)
    {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_CONFIG;
    }

    /* check if the partition have access to shared memory area */
    pmk_shm_partition_t *shm_partition = pmk_sharedmemory_is_accessible_to_partition(shm, partition);
    if (NULL == shm_partition)
    {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_CONFIG;
    }

    /* create structure to send to the partition */
    air_sharedmemory_t air_shm;
    air_shm.id = shm->id;
    air_shm.size = shm->size;
    air_shm.address = (void *)shm->addr;
    air_shm.permissions = shm_partition->permissions;

    /* copy structure to partition space */
    if (pmk_segregation_put_user(context, air_shm, sharedmemory) != 0)
    {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return AIR_NO_ERROR;
}

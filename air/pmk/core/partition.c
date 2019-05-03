/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Partitions initialization and context switch
 */

#include <pmk.h>
#include <bsp.h>
#include <ipc.h>
#include <ports.h>
#include <printk.h>
#include <barrier.h>
#include <workspace.h>
#include <configurations.h>

void pmk_partitions_init(void) {

    int i, j;

#ifdef PMK_DEBUG
    printk(" :: Partitions initialization\n");
#endif

    /* get the partition data array */
    pmk_partition_t *partition;
    pmk_list_t *list = pmk_get_usr_partitions();

    /* setup the partition table */
    for(i = 0; i < list->length; ++i){

        partition = pmk_get_from_list(pmk_partition_t, list, i);

#ifdef PMK_DEBUG
    printk("    partition %02i - %s\n", i, partition->name);
#endif

        /* define the partition index */
        partition->idx = i;

        /* Define partition mode and start condition */
        partition->mode = AIR_MODE_COLD_START;
        partition->start_condition = AIR_START_CONDITION_NORMAL;

        /* initialize the partition reset count */
        partition->restart_count = 0;

        /* initialize PMK internal partition state*/
        partition->state = PMK_PARTITION_STATE_NOT_RUN;

        /* allocate space for the partition context switch and virtual core */
        partition->context =
                pmk_workspace_alloc(sizeof(core_context_t) * partition->cores);

        /* initialize each core context */
        for (j = 0; j < partition->cores; ++j) {
            core_context_init(&partition->context[j], j);
        }

        /* allocate space for the partition virtual to real core mapping */
        partition->core_mapping =
                pmk_workspace_alloc(sizeof(air_u32_t) * PMK_MAX_CORES);

        /* initialize each core context */
        for (j = 0; j < PMK_MAX_CORES; ++j) {
            partition->core_mapping[j] = PMK_MAX_CORES;
        }

        /* initialize a SMP barrier */
        pmk_barrier_init(&partition->smp_barrier, partition->cores);

        /* initialize partition ports */
        pmk_partition_ports_init(partition);
    }

#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif
    return;
}

/**
 * @brief Partition start code
 * @param partition partition control structure
 * @param context current core context structure
 */
void pmk_partition_start(pmk_partition_t *partition, core_context_t *context) {

   /* reset elapsed ticks and last tick */
   partition->elapsed_ticks = 0;
   partition->last_clock_tick = 0;

   /* reset entry point */
   core_context_set_entry_point(context, NULL);

   /* partition virtual core 0 needs to perform some extra work */
   if (core_context_id(context) == 0) {

       /* set entry point from the ELF */
       core_context_set_entry_point(context, partition->elf->entry);
       /* set partition state to running */
       partition->state = PMK_PARTITION_STATE_RUNNING;
   }

   /* setup the partition context */
   core_context_setup_partition(context, partition);
   return;
}

/**
 * @brief Halts a partition
 * @param partition partition control structure
 */
void pmk_partition_halt(pmk_partition_t *partition) {

    air_u32_t i;

    /* get current core */
    air_u32_t core_id = bsp_get_core_id();
    for (i = 0; i < partition->cores; ++i) {

        /* partition is running */
        if (partition->core_mapping[i] < PMK_MAX_CORES &&
            partition->core_mapping[i] != core_id) {

            core_context_set_ipc_message(&partition->context[i],
                    PMK_IPC_TRASH_PARTITION_CORE);
            bsp_interrupt_core(partition->core_mapping[i], BSP_IPC_IRQ);

        /* partition is not running */
        } else {

            core_context_setup_idle(&partition->context[i]);
        }
    }

    /* flag the partition as halted */
    partition->state = PMK_PARTITION_STATE_HALTED;
    return;
}

/**
 * @brief Restarts a partition
 * @param partition partition control structure
 */
void pmk_partition_restart(pmk_partition_t *partition) {

    air_u32_t i;
    air_u32_t vcpus = 0x00000000;

    partition->state = PMK_PARTITION_STATE_RESTARTING;

    /*Identify all running vcpu*/
    for (i = 0; i < partition->cores; ++i)
        if (partition->core_mapping[i] < PMK_MAX_CORES)
            vcpus |= 1 << i;

    if(!vcpus)
    {   /*No vcpu running. Assign vcpu 0 for partition reload*/
        core_context_setup_reload_partition(&partition->context[0], partition);
        for (i = 1; i < partition->cores; ++i)
            core_context_setup_idle(&partition->context[i]);
    }
    else
    {   /*Theres vcpu running. Assign the first we find to go partition reload right away*/
        for(i = 0; i < partition->cores; i++){
            if(vcpus & (1 << i))
            {
                if (partition->core_mapping[i] != bsp_get_core_id())
                {
                    if(1 << i  & (vcpus & (-vcpus)))
                        core_context_set_ipc_message(&partition->context[i],
                            PMK_IPC_PARTITION_RESTART);
                    else
                        core_context_set_ipc_message(&partition->context[i],
                            PMK_IPC_TRASH_PARTITION_CORE);

                    bsp_interrupt_core(partition->core_mapping[i], BSP_IPC_IRQ);
                }
                else
                {
                    if(1 << i  & (vcpus & (-vcpus)))
                        core_context_setup_reload_partition(&partition->context[i], partition);
                    else
                        core_context_setup_idle(&partition->context[i]);
                }
            }
            else
                core_context_setup_idle(&partition->context[i]);
        }
    }
    return;
}

/**
 * @brief Reload ELF into partition's memory
 * @param partition partition control structure
 */
void pmk_partition_reload(pmk_partition_t *partition) {

    cpu_preemption_flags_t flags = (0x0F << 8);

    /* reload partition */
    pmk_partition_load(partition->elf, cpu_get_physical_addr(partition->mmu_ctrl, partition->mmap->v_addr), partition->mmap->v_addr);

    cpu_disable_preemption(flags);

    /* increment the number of restarts */
    ++partition->restart_count;

    /* flag it to initialize on the next scheduling point */
    partition->state = PMK_PARTITION_STATE_INIT;

    cpu_enable_preemption(flags);

    bsp_idle_loop();
}

/**
 * @brief Get partition configuration by Id
 * @param pid partition Id
 * @return partition configuration pointer if Id is valid, NULL otherwise
 */
pmk_partition_t *pmk_get_partition_by_id(air_identifier_t pid) {

    air_u32_t i;
    pmk_partition_t *found = NULL;
    pmk_list_t *list = pmk_get_usr_partitions();
    for(i = 0; i < list->length; ++i){
        pmk_partition_t *partition = pmk_get_from_list(pmk_partition_t, list, i);
        if (pid == partition->id) {
            found = partition;
        }
    }
    return found;
}

/**
 * @brief Get partition configuration by name
 * @param name partition name
 * @return partition configuration pointer if name is valid, NULL otherwise
 */
pmk_partition_t *pmk_get_partition_by_name(air_name_ptr_t name) {

    air_u32_t i;
    pmk_partition_t *found = NULL;
    pmk_list_t *list = pmk_get_usr_partitions();
    for(i = 0; i < list->length; ++i){
        pmk_partition_t *partition =
                pmk_get_from_list(pmk_partition_t, list, i);
        if (strncmp(name, partition->name, sizeof(air_name_t)) == 0) {
            found = partition;
        }
    }
    return found;
}

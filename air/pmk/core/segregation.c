/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
 /**
  *  @file segregation.c
  *  @author pfnf
  *  @brief Functions to setup the partition, shared areas and IO devices memory
  *         segregation
  */

#include <bsp.h>
#include <printk.h>
#include <partition.h>
#include <workspace.h>
#include <segregation.h>
#include <sharedmemory.h>
#include <configurations.h>

void pmk_segregation_int(void) {

    air_u32_t i, j;

#ifdef PMK_DEBUG
    printk(" :: Segregation initialization\n");
#endif

    /* arch dependent initialization */
    cpu_segregation_init();

    air_uptr_t p_addr = (air_uptr_t)&air_kernel_memory_end;

    /* get lists of partitions and shared memory areas */
    pmk_list_t *partition_list = pmk_get_usr_partitions();
    pmk_list_t *shm_areas_list = pmk_get_shared_memory_areas();

    /* configure partitions main memory area */
    for(i = 0; i < partition_list->length; ++i){

        /* get partition pointer */
        pmk_partition_t *partition =
                pmk_get_from_list(pmk_partition_t, partition_list, i);

#ifdef PMK_DEBUG
        printk("    Partition %i %s - Memory Map\n", partition->id, partition->name);
#endif

        /* BSP segregation */
        bsp_segregation(partition);

        /* align physical address with the memory area unit */
        p_addr = (air_uptr_t)ADDR_ALIGN(p_addr, partition->mmap->p_unit);

        /* map the Partition memory region for user access */
        cpu_segregation_map_memory(
                partition->mmu_ctrl,
                (void *)p_addr,
                partition->mmap->v_addr,
                partition->mmap->size,
                partition->mmap->p_unit,
                PMK_MMU_R | PMK_MMU_W | PMK_MMU_E | PMK_MMU_CACHEABLE);

        /* load the partition ELF data into the main memory */
        pmk_partition_load(partition->elf, (void *)p_addr);

        /* increment the physical address by size requested by the partition*/
        p_addr += ADDR_ALIGN(partition->mmap->size, partition->mmap->p_unit);
    }

    /* configure share main memory area */
    for(i = 0; i < shm_areas_list->length; ++i) {

        pmk_shm_t *shm = pmk_get_from_list(pmk_shm_t, shm_areas_list, i);
        pmk_list_t *shm_access_list = &shm->partitions;

#ifdef PMK_DEBUG
        printk("    SharedMemory %i %s - Memory Map\n", shm->id, shm->name);
#endif

        /* align physical address with the memory area unit */
        p_addr = (air_uptr_t)ADDR_ALIGN(p_addr, shm->unit);

        for (j = 0; j < shm_access_list->length; ++j) {

            pmk_shm_partition_t *shm_partition =
                    pmk_get_from_list(pmk_shm_partition_t, shm_access_list, j);
            pmk_partition_t *partition = shm_partition->partition;

            /* clear the 1st n bytes of the shared area */
(void)            memset((void *)p_addr, 0x0, PMK_SHARED_MEMORY_CLEAR);


            /* map the Shared Memory Area for partition access */
            cpu_segregation_map_memory(
                    partition->mmu_ctrl,
                    (void *)p_addr,
                    shm->addr,
                    shm->size,
                    shm->unit,
                    shm_partition->permissions | PMK_MMU_CACHEABLE);
        }

        /* increment the real address by the space used */
        p_addr += ADDR_ALIGN(shm->size, shm->unit);
    }

#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif
}

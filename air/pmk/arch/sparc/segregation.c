/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief SPARC memory segregation implementation via MMU
 */

#include <cpu.h>
#include <bsp.h>
#include <error.h>
#include <printk.h>
#include <workspace.h>
#include <partition.h>
#include <segregation.h>
#include <configurations.h>

/**
 *
 */
void sparc_mmu_initialize(void *context_tables);



#define MMU_ACCESS_R                        (0 << 2)
#define MMU_ACCESS_RW                       (1 << 2)
#define MMU_ACCESS_RE                       (2 << 2)
#define MMU_ACCESS_RWE                      (3 << 2)
#define MMU_ACCESS_E                        (4 << 2)
#define MMU_ACCESS_SRW                      (5 << 2)
#define MMU_ACCESS_SRE                      (6 << 2)
#define MMU_ACCESS_SRWE                     (7 << 2)
#define MMU_CACHEABLE                       (1 << 7)

#define L1_SIZE                             (1 << 24)
#define L2_SIZE                             (1 << 18)
#define L3_SIZE                             (1 << 12)

#define L1_ENTRIES                          (256)
#define L2_ENTRIES                          (64)
#define L3_ENTRIES                          (64)

#define CONTEXT_ENTRIES     (256U)


#define MMU_ENTRY_TYPE_MASK             (3)

/** Align an address for a PTP entry */
#define MMU_PTD_PTP(addr)               (((air_i32_t)addr >> 6U) << 2U)
#define MMU_PTD_ET                      (1)

/** Align an address for a PPN entry (Physical Page Number : first 24 bits on a PTE*/
#define MMU_PTE_PPN(addr)               ((((air_i32_t)addr) >> 12U) << 8U)
#define MMU_PTE_ET                      (2)

#define MMU_INVALID_PAGE                (0)

/**
 * @brief MMU Context Table pointer
 */
static air_u32_t *mmu_context;
/**
 * @brief MMU L1, L2, L3 Table pointers
 */
static air_u32_t *ln_tables[4];

static air_u32_t ln_index[3] = { 0, 0, 0 };
static air_u32_t ln_size[3] = { L1_SIZE, L2_SIZE, L3_SIZE };
static air_u32_t ln_entries[3] = { L1_ENTRIES, L2_ENTRIES, L3_ENTRIES };

/* ram_end - pointer to ram end address, from application linker file*/
extern air_u32_t *ram_end;


/**
 * @brief Converts PMK MMU abstraction flags into SPARC MMU hardware flags
 * @brief flags PMK MMU abstraction flags
 * @return SPARC MMU hardware flags
 */
static air_u32_t sparc_get_mmu_access_type(air_u32_t flags) {

    air_u32_t access_type = 0;

    /* MMU device isn't required by SPARC */
    flags &= ~PMK_MMU_DEVICE;

    switch (flags & ~PMK_MMU_CACHEABLE) {

        case PMK_MMU_R:
            access_type = MMU_ACCESS_R;
            break;

        case PMK_MMU_R | PMK_MMU_W:
            access_type = MMU_ACCESS_RW;
            break;

        case PMK_MMU_R | PMK_MMU_E:
            access_type = MMU_ACCESS_RE;
            break;

        case PMK_MMU_R | PMK_MMU_W | PMK_MMU_E:
            access_type = MMU_ACCESS_RWE;
            break;

        case PMK_MMU_E:
            access_type = MMU_ACCESS_E;
            break;

        case PMK_MMU_SR | PMK_MMU_SW:
            access_type = MMU_ACCESS_SRW;
            break;

        case PMK_MMU_SR | PMK_MMU_SE:
            access_type = MMU_ACCESS_SRE;
            break;

        case PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE:
            access_type = MMU_ACCESS_SRWE;
            break;
    }

    /* apply cache-able flag */
    if ((flags & PMK_MMU_CACHEABLE) != 0) {

        access_type |= MMU_CACHEABLE;
    }

    return access_type;
}

/**
 * @brief Gets a L(n + 1) MMU empty table
 * @param n level of the table minus one
 * @return L(n + 1) MMU table pointer
 */
static air_u32_t *sparc_mmu_get_table(air_u32_t n) {

    air_u32_t i;
    air_u32_t *table = ln_tables[n]; /* [ln_index[n]]; */
    for (i = 0; i < ln_entries[n]; ++i) {
        table[i] = MMU_INVALID_PAGE;
    }
    if ((air_u32_t)table >= (air_u32_t)ln_tables[n + 1]) {

        printk("  L%i table overflow...\n", n + 1);
    }

    /* update table pointer */
    /* ln_index[n] += ln_entries[n]; */
    ln_tables[n] += ln_entries[n];
    return table;
}

/**
 * @brief Gets the L(n + 1) MMU table index
 * @param n level of the table minus one
 * @return L(n + 1) MMU table index
 */
static air_u32_t sparc_mmu_get_level_index(air_u32_t n, void *v_addr) {

    air_u32_t idx = 0;
    switch (n) {

        case 0:
            idx = ((air_uptr_t)v_addr) / L1_SIZE;
            break;

        case 1:
            idx = (((air_uptr_t)v_addr) & (L1_SIZE - L2_SIZE)) >> 18;
            break;

        case 2:
            idx = (((air_uptr_t)v_addr) & (L2_SIZE - L3_SIZE)) >> 12;
            break;
    }
    return idx;
}

/**
 * @brief Fills an MMU table
 * @param table pointer to the table
 * @param p_addr physical address to map
 * @param v_addr virtual address to map
 * @param n level of the current table
 * @param unit desired memory unit
 * @param size of the memory block
 * @param access access permission to the mapped memory block
 * @return consumed size of the memory block
 *
 * @note This function recursive but bounded
 */
static air_u32_t sparc_mmu_fill_table(
        air_u32_t *table, void *p_addr, void *v_addr,
        air_u32_t n, air_u32_t unit, air_sz_t size, air_u32_t access) {

    air_u32_t i = 0;
    air_u32_t consumed = 0;
    air_u32_t l_consumed = 0;
    air_u32_t level_unit = ln_size[n];
    air_u32_t level_entries = ln_entries[n];

    while (size > 0 && i < level_entries - 1) {

        i = sparc_mmu_get_level_index(n, v_addr);

        /* lower unit ? */
        if (unit < level_unit) {

            air_u32_t *ltable;

            /* check if required table already exists */
            switch (table[i] & MMU_ENTRY_TYPE_MASK) {

                /* page table descriptor */
                case MMU_PTD_ET:

                    ltable = (air_u32_t *)((table[i] >> 2) << 6);
                    break;

                /* page table entry */
                case MMU_PTE_ET:

                    return consumed;
                    break;

                /* no table assigned */
                default:

                    ltable = sparc_mmu_get_table(n + 1);
                    table[i] = MMU_PTD_PTP(ltable) | MMU_PTD_ET;
                    break;
            }

            l_consumed = sparc_mmu_fill_table(
                    ltable,
                    p_addr, v_addr,
                    n + 1, unit, size,
                    access);

        } else {

            l_consumed = level_unit;
            table[i] = MMU_PTE_PPN(p_addr) | access | MMU_PTE_ET;

        }

        consumed += l_consumed;
        p_addr += l_consumed;
        v_addr += l_consumed;
        size -= l_consumed;
    }

    return consumed;
}

void sparc_segregation_init() {

    air_u32_t i;

    /* allocate the MMU tables */
    arch_configuration_t *configuration = \
            (arch_configuration_t *)pmk_get_arch_configuration();
    ln_index[0] = 0;
    ln_index[1] = 0;
    ln_index[2] = 0;

    air_u32_t l0_space = configuration->mmu_context_entries * sizeof(air_u32_t);
    air_u32_t l1_space = configuration->mmu_l1_tables_entries * L1_ENTRIES * sizeof(air_u32_t);
    air_u32_t l2_space = configuration->mmu_l2_tables_entries * L2_ENTRIES * sizeof(air_u32_t);
    air_u32_t l3_space = configuration->mmu_l3_tables_entries * L3_ENTRIES * sizeof(air_u32_t);

    mmu_context  = pmk_workspace_aligned_alloc(l0_space, 0x400);
    ln_tables[0] = pmk_workspace_aligned_alloc(l1_space, 0x400);
    ln_tables[1] = pmk_workspace_aligned_alloc(l2_space, 0x100);
    ln_tables[2] = pmk_workspace_aligned_alloc(l3_space, 0x100);
    ln_tables[3] = ln_tables[2] + l3_space;

    /* setup partition MMU control structure for each partition */
    pmk_list_t *list = pmk_get_usr_partitions();
    for(i = 0; i < list->length; ++i){

        pmk_partition_t *partition = &((pmk_partition_t *)list->elements)[i];

        /* get a MMU control structure for the partition */
        mmu_context_t *ctrl = pmk_workspace_alloc(sizeof(mmu_context_t));
        ctrl->context = i;                              /* MMU context      */
        ctrl->l1_tables = sparc_mmu_get_table(0);       /* MMU L1 Tables    */
        partition->mmu_ctrl = ctrl;

        /* link the context to the L1 tables */
        mmu_context[i] = MMU_PTD_PTP(ctrl->l1_tables) | MMU_PTD_ET;
    }


#ifdef PMK_DEBUG
    printk(" :: RAM end for application 0x%06x\n", &ram_end);
#endif
}

void sparc_map_memory(
        mmu_context_t *ctrl, void *p_addr, void *v_addr,
        air_sz_t size, air_sz_t unit, air_u32_t permissions) {

    /* sanity check */
    if (size != 0) {

        /* identity map */
        if (v_addr == NULL) {
            v_addr = p_addr;
        }

        /* convert permissions */
        air_u32_t sparc_permissions = sparc_get_mmu_access_type(permissions);

#ifdef PMK_DEBUG
        printk("      [%p-%p]->[%p-%p] (u:%p, p:0x%03x)\n",
            v_addr, v_addr + size - 1,
            p_addr, p_addr + size - 1,
            unit, sparc_permissions);
#endif
        /* size must aligned to the memory unit */
        size = ((size) + ((unit) - 1)) & ~((unit) - 1);

        /* fill tables */
        sparc_mmu_fill_table(
                ctrl->l1_tables, p_addr, v_addr,
                0, unit, size, sparc_permissions);
    }
}



extern void sparc_mmu_disable_faults(core_context_t *core_ctx);
extern air_u32_t sparc_mmu_enable_faults(core_context_t *core_ctx);

/**
 * @brief Check if the access to the partition is valid
 * @param addr partition space address
 * @param size size of the memory block
 * @return 1 if the address might be accessible to the partition, 0 if the
 *         address isn't accessible to the partition
 */
static air_u32_t sparc_segregation_access_ok(void *addr, air_sz_t size) {

    /* check alignment
    if (((air_uptr_t)addr & 0x03) != 0) {

        return 0;
    }*/

    /* check if address is in the range of the partitions */
    if (addr < (void *)&ram_end) {
        #ifdef PMK_DEBUG
            printk(" :: Segregation access not ok\n");
            printk("     on address 0x%06x\n", addr);
            printk("     with content: %s\n", &addr);

        #endif

        return 0;
    }

    return 1;
}

/**
 * @brief SPARC secure memory copy
 * @param src source address
 * @param dst destination address
 * @param size size of the memory block
 * @retun 0 if no faults, 1 otherwise
 */
static air_u32_t sparc_segregation_memcpy(
        core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    /* disable MMU faults */
    sparc_mmu_disable_faults(core_ctx);

    /* memory copy */
    memcpy(dst, src, size);

    /* return state of faults */
    return sparc_mmu_enable_faults(core_ctx);
}

air_u32_t sparc_copy_to_user(
        core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    /* check if access is ok (destination is in user space) */
    if (sparc_segregation_access_ok(dst, size) == 0) {

        return 1;
    }

    /* perform a secure copy */
    return sparc_segregation_memcpy(core_ctx, dst, src, size);
}

air_u32_t sparc_copy_from_user(
        core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    /* check if access is ok (source is in user space) */
    if (sparc_segregation_access_ok(src, size) == 0) {

        return 1;
    }

    /* perform a secure copy */
    return sparc_segregation_memcpy(core_ctx, dst, src, size);
}

void *sparc_get_physical_addr(mmu_context_t *context, void *v_addr) {

    /* get partition L1 tables */
    air_u32_t *tbl = context->l1_tables;

    /* walk tables */
    air_u32_t j;
    void *p_addr = NULL;
    air_u32_t mask = 0x00FFFFFF;

    for (j = 0; j < 3; ++j) {

        /* get current level index */
        air_u32_t i = sparc_mmu_get_level_index(j, v_addr);

        /* get entry type */
        air_u32_t et = tbl[i] & MMU_ENTRY_TYPE_MASK;

        /* invalid entry */
        if (et == 0 || et == 3) {
            break;
        }

        /* page table pointer */
        if (et == 1) {
            mask >>= 6;
            tbl = (air_u32_t *)((tbl[i] >> 2) << 6);
            continue;
        }

        /* page table entry */
        if (et == 2) {

            /* check permissions */
            if (((tbl[i] & 0x1C) >> 2) < 6) {

                p_addr = (void *)(((tbl[i] << 4) & ~mask) |
                                  ((air_uptr_t)v_addr & mask));
            }

            break;
        }
    }
    return p_addr;
}

void sparc_enable_mmu() {

    sparc_mmu_initialize(mmu_context);
}

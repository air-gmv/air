/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file mmu.c
 * @author lumm
 * @brief ARM virtualization MMU tables and segregation
 * TODO ASSUMES THERE IS ONLY ONE TTBR, AS IT EASIES WRITING CODE
 * STARTING WITH NO SUPERTABLE SUPPORT
 */

#include <mmu.h>
#include <bsp.h>
#include <configurations.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_mmu_init(void) {

    air_u32_t ttbcr = arm_cp15_get_translation_table_control();
    ttbcr = (ttbcr & ~0b110111) | TTBR_N;
    arm_cp15_set_translation_table_control(ttbcr);

    air_u32_t dac = ((DAC_CLIENT) << (2 * DEFAULT_DOMAIN));
    arm_cp15_set_domain_access_control(dac);

    air_u32_t sctlr = arm_cp15_get_system_control();
    sctlr &= ~(ARM_SCTLR_TRE | ARM_SCTLR_A);
    sctlr |= (ARM_SCTLR_AFE | ARM_SCTLR_Z);
    arm_cp15_set_system_control(sctlr);

    arm_cp15_tlb_invalidate();
}

void arm_mmu_enable(void) {

    /* THE 1st MMU CTRL IS TAKEN FROM THE 1ST PARTITION. THIS MEANS PARTITION
     * SEGREGATION MUST BE DONE BY NOW.
     */

    pmk_list_t *list = pmk_get_usr_partitions();
    pmk_partition_t *partition = &((pmk_partition_t *)list->elements)[0];
    arm_mmu_context_t *mmu_ctrl = partition->mmu_ctrl;


    arm_cp15_set_translation_table0_base(mmu_ctrl->ttbr0);
#if TTBR_N
    arm_cp15_set_translation_table1_base(mmu_ctrl->ttbr1);
#endif

    air_u32_t sctlr = arm_cp15_get_system_control();
    sctlr |= (ARM_SCTLR_M);

#ifdef PMK_DEBUG
    printk("    :: arm_mmu_enable , mmu not enabled\n");
#endif

//  arm_cp15_set_system_control(sctlr);

}

void arm_segregation_init(void) {

    /* setup partition MMU control structure for each partition */
    pmk_list_t *list = pmk_get_usr_partitions();
    for (air_u32_t i = 0; i < list->length; ++i) {

        pmk_partition_t *partition = &((pmk_partition_t *)list->elements)[i];

        /* get a MMU control structure for the partition */
        arm_mmu_context_t *ctrl = pmk_workspace_alloc(sizeof(arm_mmu_context_t));

        ctrl->ttbr0 = pmk_workspace_aligned_alloc(TTBR0_SIZE, TTBR0_SIZE);
        for (i = 0; i < TTBR0_ENTRIES; ++i) {
            ctrl->ttbr0[i] = 0;
        }
#if TTBR_N
        ctrl->ttbr1 = pmk_workspace_aligned_alloc(TTBR1_SIZE, TTBR1_SIZE);
        for (i = 0; i < TTBR1_ENTRIES; ++i) {
            ctrl->ttbr1[i] = 0;
        }
#else
        ctrl->ttbr1 = NULL;
#endif

        partition->mmu_ctrl = ctrl;
    }
}

air_u32_t arm_get_mmu_permissions(
        air_u32_t permissions, air_u32_t level, air_u32_t type) {

    /* when an entry is initialized, the access flag must be set to 1.
     * also assuming TEX[2:0] = 0b000 for all entries
     * and shareable all over (only applies for cacheable) */
    air_u32_t access_mask = (0b000 << MMU_TEX_OFFSET(level, type))
                          | MMU_ACCESS_A(level, type)
                          | MMU_ATTR_S(level, type);

    if (level == MMU_L1 && type == MMU_PTD) {
        return 0;
    }

    /* if a memory position is device, it cannot be cacheable.
     * for a write-through cache, if it is cacheable, it can be buffereable */
    if (permissions & PMK_MMU_DEVICE) {
        access_mask |= MMU_ATTR_B;
        permissions &= ~(PMK_MMU_DEVICE | PMK_MMU_CACHEABLE);
    } else if (permissions & PMK_MMU_CACHEABLE) {
        access_mask |= MMU_ATTR_C;
        permissions &= ~(PMK_MMU_CACHEABLE);
    }

    switch (permissions) {

    case PMK_MMU_R:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_XN(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_W:
        access_mask |= MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_XN(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_E:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_W | PMK_MMU_E:
        access_mask |= MMU_ACCESS_UP(level, type);
        break;

    case PMK_MMU_E:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type);
        break;

    case PMK_MMU_SR:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_XN(level, type);
        break;

    case PMK_MMU_SR | PMK_MMU_SW:
        access_mask |= MMU_ACCESS_XN(level, type);
        break;

    case PMK_MMU_SR | PMK_MMU_SE:
        access_mask |= MMU_ACCESS_RO(level, type);
        break;

    case PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE:
        break;
    }

    return access_mask;
}

void arm_mmu_level1(
        air_uptr_t ttbr, void *p_addr, void *v_addr,
        air_u32_t unit, air_sz_t size, air_u32_t permissions) {

    air_u32_t idx = 0;
    air_uptr_t pt_ptr = 0;
    air_sz_t consumed = 0;

    while (size > 0) {

        idx = ((air_u32_t)v_addr >> 20);

        if (unit < SECTION_SIZE) {

            /* check if entry already exists */
            switch((air_u32_t)ttbr[idx] & MMU_D_TYPE_MASK) {

            case MMU_PTD:
                pt_ptr = (air_uptr_t)((air_u32_t)ttbr[idx] & 0xfffffc00);
                break;

            case MMU_SECTION:
                /* entry already in use TODO what to do???*/
                break;

            case MMU_SUPERSECTION:
                /* TODO shouldnt exist */

            default:
                /* unused */
                pt_ptr = pmk_workspace_aligned_alloc(TTL2_SIZE, TTL2_SIZE);
                ttbr[idx] = ( ((air_u32_t)pt_ptr & 0xfffffc00)
                        | (DEFAULT_DOMAIN << 5)
                        | MMU_PTD );
                break;
            }

            consumed = arm_mmu_level2(pt_ptr, p_addr, v_addr, unit, size, permissions);

        } else {

            air_u32_t arm_permissions = arm_get_mmu_permissions(permissions, MMU_L1, MMU_SECTION);
            ttbr[idx] = (((air_u32_t)p_addr & 0xfff00000) | arm_permissions);
            consumed = TTL1_SECT_SIZE;
        }

        size -= consumed;
        p_addr += consumed;
        v_addr += consumed;
    }
}

air_sz_t arm_mmu_level2(
        air_uptr_t pt_ptr, void *p_addr, void *v_addr,
        air_u32_t unit, air_sz_t size, air_u32_t permissions) {

    air_u32_t idx = (((air_u32_t)v_addr >> 12) & 0xff);
    air_u32_t used_entries = 0;
    air_u32_t lpage_idx = 0;
    air_u32_t arm_permissions;
    air_sz_t consumed = 0;
    air_sz_t delta = 0;

    while (used_entries < TTL2_ENTRIES) {

        if (unit < LPAGE_SIZE) {
            /* Small Page */
            arm_permissions = arm_get_mmu_permissions(permissions, MMU_L2, MMU_PAGE1);

            pt_ptr[idx] = (((air_u32_t)p_addr & 0xfffff000) | arm_permissions);
            used_entries += 1;
            delta = TTL2_PAGE_SIZE;

        } else {
            arm_permissions = arm_get_mmu_permissions(permissions, MMU_L2, MMU_LPAGE);
            for (lpage_idx = 0; lpage_idx < 16; ++lpage_idx) {
                pt_ptr[idx + lpage_idx] = (((air_u32_t)p_addr & 0xffff0000) | arm_permissions);
            }
            used_entries += 16;
            delta = TTL2_LPAGE_SIZE;
        }

        consumed += delta;
        size -= delta;

        /* last memory mapped -> was last page needed */
        if (size < 0) {
            break;
        }
    }

    return consumed;
}

void arm_map_memory(
        arm_mmu_context_t *ctrl, void *p_addr, void *v_addr,
        air_sz_t size, air_sz_t unit, air_u32_t permissions) {

    if (size != 0) {

        if (v_addr == NULL) {
            v_addr = p_addr;
        }

        /* size must aligned to the memory unit */
        size = ((size) + ((unit) - 1)) & ~((unit) - 1);

        arm_mmu_level1(ctrl->ttbr0, p_addr, v_addr, unit, size, permissions);
    }
}

air_u32_t arm_segregation_memcpy(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    /* todo MMU disable faults. DOESNT EXIST ON ARM */
    memcpy(dst, src, size);
    return 0;
}

air_u32_t arm_segregation_access_valid(void *addr) {

    if ((air_u32_t)addr < PMK_PARTITION_BASE_ADDR)
        return 0;

    return 1;
}

air_u32_t arm_copy_to_user(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    if (!arm_segregation_access_valid(dst))
        return 1;

    return arm_segregation_memcpy(core_ctx, dst, src, size);
}

air_u32_t arm_copy_from_user(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    if (!arm_segregation_access_valid(src))
        return 1;

    return arm_segregation_memcpy(core_ctx, dst, src, size);
}

air_uptr_t arm_get_physical_addr(arm_mmu_context_t *context, void *v_addr) {

    air_uptr_t ttbr = context->ttbr0;

    air_u32_t idx = ((air_u32_t)v_addr >> 20);
    air_uptr_t p_addr = NULL;
    air_uptr_t pt_ptr = NULL;

    switch(ttbr[idx] & MMU_D_TYPE_MASK) {

    case MMU_INVALID_ENTRY:
    case MMU_RESERVED_ENTRY:
        break;

    case MMU_PTD:
        pt_ptr = (air_uptr_t)((air_u32_t)ttbr[idx] & 0xfffffc00);
        idx = (((air_u32_t)v_addr >> 12) & 0xff);

        switch((air_u32_t)pt_ptr[idx] & MMU_D_TYPE_MASK) {
        case MMU_INVALID_PAGE:
            break;

        case MMU_LPAGE:
            if (((air_u32_t)pt_ptr[idx] & MMU_ACCESS_UP(MMU_L2, MMU_LPAGE)))
                p_addr = (air_uptr_t)(((air_u32_t)pt_ptr[idx] & 0xffff0000) | ((air_u32_t)v_addr & 0x0000ffff));

            break;
        // small page 1x
        case MMU_PAGE1:
        case MMU_PAGE2:
            if (((air_u32_t)pt_ptr[idx] & MMU_ACCESS_UP(MMU_L2, MMU_PAGE)))
                p_addr = (air_uptr_t)(((air_u32_t)pt_ptr[idx] & 0xfffff000) | ((air_u32_t)v_addr & 0x00000fff));

            break;
        }
        break;

    case MMU_SECTION:
        if (((air_u32_t)ttbr[idx] & MMU_ACCESS_UP(MMU_L1, MMU_SECTION)))
            p_addr = (air_uptr_t)(((air_u32_t)ttbr[idx] & 0xfff00000) | ((air_u32_t)v_addr & 0x000fffff));

        break;

    case MMU_SUPERSECTION:
        // todo not implemented. also, these bits not checked correctly
        break;
    }

    return p_addr;
}

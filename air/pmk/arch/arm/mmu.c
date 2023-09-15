/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file mmu.c
 * \author lumm
 * \brief ARM virtualization MMU tables and segregation
 *
 * TODO ASSUMES THERE IS ONLY ONE TTBR, AS IT EASIES WRITING CODE
 * STARTING WITH NO SUPERTABLE SUPPORT
 */

#include <mmu.h>
#include <cp15.h>
#include <workspace.h>
#include <segregation.h>
#include <configurations.h>
#include <error.h>

#ifdef PMK_DEBUG
#include <printk.h>
#endif

static air_uptr_t *arm_ln_table[2];
static air_u32_t arm_ln_index[2] = {0, 0};
const air_u32_t ln_entries[2] = {TTBR0_ENTRIES, TTL2_ENTRIES};

const air_u32_t ln_unit[2] = {SECTION_SIZE, PAGE_SIZE};

static air_u32_t arm_mmu_get_index(void *addr, air_u32_t n) {

    air_u32_t id = 0;

    switch (n) {
    case 0:
        id = ((air_u32_t)addr >> 20);
        break;

    case 1:
        id = (((air_u32_t)addr >> 12) & 0xff);
        break;
    }

    return id;
}

static air_uptr_t *arm_mmu_get_table(air_u32_t n) {

    air_u32_t e;
    air_uptr_t *table = arm_ln_table[n] + arm_ln_index[n];

for(e = 0; e < ln_entries[n]; ++e)
{
        table[e] = 0;
}



    arm_ln_index[n] += ln_entries[n];

    return table;
}

static air_u32_t arm_get_mmu_permissions(
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
if((permissions & PMK_MMU_DEVICE) != 0) {
        access_mask |= MMU_ATTR_B;
        permissions &= ~(PMK_MMU_DEVICE | PMK_MMU_CACHEABLE);
    } else if((permissions & PMK_MMU_CACHEABLE) != 0) {

        access_mask |= MMU_ATTR_C;
        permissions &= ~(PMK_MMU_CACHEABLE);
    }

    switch (permissions) {

    case PMK_MMU_R:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_XN(level, type)
                    | MMU_ACCESS_NG(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_W:
        access_mask |= MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_XN(level, type)
                    | MMU_ACCESS_NG(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_E:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_NG(level, type);
        break;

    case PMK_MMU_R | PMK_MMU_W | PMK_MMU_E:
        access_mask |= MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_NG(level, type);
        break;

    case PMK_MMU_E:
        access_mask |= MMU_ACCESS_RO(level, type)
                    | MMU_ACCESS_UP(level, type)
                    | MMU_ACCESS_NG(level, type);
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

    default:
        pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);

    }
    return access_mask;
}

static air_sz_t arm_mmu_map_l2(
        air_uptr_t *pt_ptr, void *p_addr, void *v_addr, air_u32_t unit,
        air_sz_t size, air_u32_t permissions, air_u32_t n) {

    air_u32_t idx = 0;
    air_sz_t consumed = 0;
    air_sz_t _consumed = 0;

#ifdef PMK_DEBUG_MMU
    air_u32_t initial_idx = arm_mmu_get_index(v_addr, n);
#endif

    while (size > 0 && idx < ln_entries[n] - 1) {

        idx = arm_mmu_get_index(v_addr, n);

        /* Small Page */
        if (unit < LPAGE_SIZE) {

            air_u32_t arm_permissions = arm_get_mmu_permissions(permissions, MMU_L2, MMU_PAGE1);
            pt_ptr[idx] = (((air_u32_t)p_addr & 0xfffff000) | arm_permissions | MMU_PAGE1);
            _consumed = TTL2_PAGE_SIZE;

        /* Large Page */
        } else {

            air_u32_t arm_permissions = arm_get_mmu_permissions(permissions, MMU_L2, MMU_LPAGE);
            for (air_u32_t lpage_idx = 0; lpage_idx < 16; ++lpage_idx) {
                pt_ptr[idx + lpage_idx] = (((air_u32_t)p_addr & 0xffff0000) | arm_permissions | MMU_LPAGE);
            }
            _consumed = TTL2_LPAGE_SIZE;
        }

        v_addr += _consumed;
        p_addr += _consumed;
        consumed += _consumed;
        size -= _consumed;
    }
#ifdef PMK_DEBUG_MMU
    printk("\n        Level 2\n"
            "          ptd             = 0x%08x\n"
            "          ptd[%03i]        = 0x%08x\n"
            "          ptd[%03i]        = 0x%08x\n\n",
            pt_ptr,
            initial_idx, pt_ptr[initial_idx],
            idx,
            pt_ptr[idx]);
#endif

    return consumed;
}

static void arm_mmu_map_l1(
        air_uptr_t *ttbr, void *p_addr, void *v_addr, air_sz_t unit,
        air_sz_t size, air_u32_t permissions, air_u32_t n) {

    air_u32_t idx = 0;
    air_sz_t consumed = 0;
    air_sz_t _consumed = 0;
    air_uptr_t *pt_ptr = 0;

#ifdef PMK_DEBUG_MMU
    air_u32_t initial_idx = arm_mmu_get_index(v_addr, n);

    printk("\n         unit             = 0x%08x\n"
            "         SECTION_SIZE     = 0x%08x\n"
            "         LPAGE_SIZE       = 0x%08x\n"
            "         PAGE_SIZE        = 0x%08x\n\n",
            unit, SECTION_SIZE, LPAGE_SIZE, PAGE_SIZE);
#endif

    while (size > 0 && idx < ln_entries[n] - 1) {

        idx = arm_mmu_get_index(v_addr, n);

        if (unit < ln_unit[n]) {

            /* check if entry already exists */
            switch(ttbr[idx] & MMU_D_TYPE_MASK) {

            case MMU_PTD:
                pt_ptr = (air_uptr_t *)(ttbr[idx] & 0xfffffc00);
                break;

            case MMU_SECTION:
                /* entry already in use TODO what to do???*/
                pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);
                break;

            case MMU_SUPERSECTION:
                /* TODO shouldn't exist */
                pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);
                break;

            default:
                /* unused */
                pt_ptr = arm_mmu_get_table(n + 1);
                ttbr[idx] = ( ((air_u32_t)pt_ptr & 0xfffffc00)
                        | (DEFAULT_DOMAIN << 5)
                        | MMU_PTD );
                break;
            }

            _consumed = arm_mmu_map_l2(pt_ptr, p_addr, v_addr, unit, size, permissions, 1);

        /* Section */
        } else {

            air_u32_t arm_permissions = arm_get_mmu_permissions(permissions, MMU_L1, MMU_SECTION);
            ttbr[idx] = ( ((air_u32_t)p_addr & 0xfff00000)
                    | arm_permissions
                    | (DEFAULT_DOMAIN << 5)
                    | MMU_SECTION );

            _consumed = TTL1_SECT_SIZE;
        }

        v_addr += _consumed;
        p_addr += _consumed;
        consumed += _consumed;
        size -= _consumed;
    }
#ifdef PMK_DEBUG_MMU
    printk("\n        Level 1\n"
            "          ttbr            = 0x%08x\n"
            "          ttbr[%04i]      = 0x%08x\n"
            "          ttbr[%04i]      = 0x%08x\n\n",
            ttbr,
            initial_idx, ttbr[initial_idx],
            idx,
            ttbr[idx]);
#endif
}

void arm_mmu_init(void) {
#ifdef PMK_DEBUG_ISR
    printk("\n\t\t\t\t---> MMU initialization <---\n\n");
#endif

    air_u32_t ttbcr = arm_cp15_get_translation_table_control();
    ttbcr = (ttbcr & ~0b110111) | TTBR_N;
    arm_cp15_set_translation_table_control(ttbcr);

    air_u32_t dac = ((DAC_CLIENT) << (2 * DEFAULT_DOMAIN));
    air_arm_cp15_set_domain_access_control(dac);

    air_u32_t sctlr = arm_cp15_get_system_control();
    sctlr &= ~(ARM_SCTLR_TRE | ARM_SCTLR_A);
    sctlr |= (ARM_SCTLR_AFE | ARM_SCTLR_Z);
    arm_cp15_set_system_control(sctlr);

    air_arm_cp15_tlb_invalidate();
}

void arm_mmu_disable(void) {
#ifdef PMK_DEBUG_ISR
    printk("\n\t\t\t\t---> MMU disable <---\n");
#endif

    air_u32_t sctlr = arm_cp15_get_system_control();
    sctlr &= ~(ARM_SCTLR_M);
    arm_cp15_set_system_control(sctlr);
}

void arm_mmu_enable(arm_mmu_context_t *ctx) {
#ifdef PMK_DEBUG_ISR
    printk("\n\t\t    ---> MMU enable with ttbr = 0x%08x <---\n\n", (air_u32_t)ctx->ttbr0);
#endif

    arm_cp15_set_CONTEXTIDR(0);
    arm_instruction_synchronization_barrier();
    air_u32_t ttbr0 = ((air_u32_t)ctx->ttbr0 & ~(TTBR0_SIZE - 1)) | (0b0010011);

    arm_cp15_set_translation_table0_base(ttbr0);
#if TTBR_N
    arm_cp15_set_translation_table1_base((air_u32_t)ctx->ttbr1 & ~(TTBR1_SIZE - 1) | (0b0010011));
#endif
    arm_instruction_synchronization_barrier();
    arm_cp15_set_CONTEXTIDR(ctx->id);

    air_u32_t sctlr = arm_cp15_get_system_control();
    sctlr |= (ARM_SCTLR_M);
    arm_cp15_set_system_control(sctlr);

    /* Enable instruction and data cache */
    sctlr |= (ARM_SCTLR_I & ARM_SCTLR_C);
    arm_cp15_set_system_control(sctlr);

    arm_data_synchronization_barrier();
}

air_u32_t arm_is_mmu_enabled(void) {

    air_u32_t sctlr = arm_cp15_get_system_control();
    return sctlr & ARM_SCTLR_M;
}

void arm_mmu_change_context(arm_mmu_context_t *ctx) {
#ifdef PMK_DEBUG_ISR
    printk("\n\t\t    ---> MMU change context with ttbr = 0x%08x <---\n\n", (air_u32_t)ctx->ttbr0);
#endif

    arm_cp15_set_CONTEXTIDR(0);
    arm_instruction_synchronization_barrier();
    air_u32_t ttbr0 = ((air_u32_t)ctx->ttbr0 & ~(TTBR0_SIZE - 1)) | (0b0010011);
    arm_cp15_set_translation_table0_base(ttbr0);
    arm_instruction_synchronization_barrier();
    arm_cp15_set_CONTEXTIDR(ctx->id);

    arm_data_synchronization_barrier();
}

void arm_segregation_init(void) {

    arch_configuration_t *configuration =
            (arch_configuration_t *)pmk_get_arch_configuration();

    air_u32_t arm_l1_table_size = configuration->mmu_l1_tables_entries * TTBR0_SIZE;

    air_u32_t arm_l2_table_size = configuration->mmu_l2_tables_entries * TTL2_SIZE;

    arm_ln_table[0] = pmk_workspace_aligned_alloc(arm_l1_table_size, TTBR0_SIZE);

    arm_ln_table[1] = pmk_workspace_aligned_alloc(arm_l2_table_size, TTL2_SIZE);

    /* setup partition MMU control structure for each partition */
    pmk_list_t *list = pmk_get_usr_partitions();

    for (air_u32_t i = 0; i < list->length; ++i) {

        pmk_partition_t *partition = &((pmk_partition_t *)list->elements)[i];

        /* get a MMU control structure for the partition */
        arm_mmu_context_t *ctrl = pmk_workspace_alloc(sizeof(arm_mmu_context_t));

        ctrl->id = i + 1;

        ctrl->ttbr0 = arm_mmu_get_table(0);
#if TTBR_N
//      ctrl->ttbr1 = arm_mmu_get_table(X);
#else
        ctrl->ttbr1 = NULL;
#endif

        partition->mmu_ctrl = ctrl;
    }
}

void arm_map_memory(
        arm_mmu_context_t *ctrl, void *p_addr, void *v_addr,
        air_sz_t size, air_sz_t unit, air_u32_t permissions) {
#ifdef PMK_DEBUG_MMU
    printk("       ttbr0              = 0x%08x\n"
            "       Start PHY address  = 0x%08x\n"
            "       VIRT address       = 0x%08x : 0x%08x\n"
            "       Size               = 0x%08x\n"
            "       Unit               = 0x%08x\n",
            ctrl->ttbr0, (air_u32_t)p_addr, (air_u32_t)v_addr,
            (air_u32_t)v_addr + (air_u32_t)size, size, unit);
#endif

    if (size != 0) {

        if (v_addr == NULL) {
            v_addr = p_addr;
        }

        /* size must aligned to the memory unit */
        size = ((size) + ((unit) - 1)) & ~((unit) - 1);
#ifdef PMK_DEBUG_MMU
        printk("       size after unit    = 0x%08x\n", size);
#endif

if(size % unit != 0)
{
            pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);
}



        arm_mmu_map_l1(ctrl->ttbr0, p_addr, v_addr, unit, size, permissions, 0);
    }
}

air_u32_t arm_segregation_memcpy(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

    /* todo MMU disable faults. DOESNT EXIST ON ARM */
(void)    memcpy(dst, src, size);

    return 0;
}

air_u32_t arm_segregation_access_valid(void *addr) {

if((air_u32_t)addr < PMK_PARTITION_BASE_ADDR)
{
        return 0;
}



    return 1;
}

air_u32_t arm_copy_to_user(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

if(!arm_segregation_access_valid(dst))
{
        return 1;
}



    return arm_segregation_memcpy(core_ctx, dst, src, size);
}

air_u32_t arm_copy_from_user(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size) {

if(!arm_segregation_access_valid(src))
{
        return 1;
}



    return arm_segregation_memcpy(core_ctx, dst, src, size);
}

air_uptr_t *arm_get_physical_addr(arm_mmu_context_t *context, void *v_addr) {

    air_uptr_t *ttbr = context->ttbr0;

    air_u32_t idx = arm_mmu_get_index(v_addr, 0);
    air_uptr_t *p_addr = NULL;
    air_uptr_t *pt_ptr = NULL;

    switch(ttbr[idx] & MMU_D_TYPE_MASK) {

    case MMU_INVALID_ENTRY:
    case MMU_RESERVED_ENTRY:
        break;

    case MMU_PTD:
        pt_ptr = (air_uptr_t *)((air_u32_t)ttbr[idx] & 0xfffffc00);
        idx = arm_mmu_get_index(v_addr, 1);

        switch((air_u32_t)pt_ptr[idx] & MMU_D_TYPE_MASK) {
        case MMU_INVALID_PAGE:
            break;

        case MMU_LPAGE:
if((((air_u32_t)pt_ptr[idx] & MMU_ACCESS_UP(MMU_L2, MMU_LPAGE))) != 0)
{
                p_addr = (air_uptr_t *)(((air_u32_t)pt_ptr[idx] & 0xffff0000) | ((air_u32_t)v_addr & 0x0000ffff));
}



            break;
        // small page 1x
        case MMU_PAGE1:
        case MMU_PAGE2:
if((((air_u32_t)pt_ptr[idx] & MMU_ACCESS_UP(MMU_L2, MMU_PAGE))) != 0)
{
                p_addr = (air_uptr_t *)(((air_u32_t)pt_ptr[idx] & 0xfffff000) | ((air_u32_t)v_addr & 0x00000fff));
}



            break;
        }
        break;

    case MMU_SECTION:
if((((air_u32_t)ttbr[idx] & MMU_ACCESS_UP(MMU_L1, MMU_SECTION))) != 0)
{
            p_addr = (air_uptr_t *)(((air_u32_t)ttbr[idx] & 0xfff00000) | ((air_u32_t)v_addr & 0x000fffff));
}



        break;

    case MMU_SUPERSECTION:
        // todo not implemented. also, these bits not checked correctly
        pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);
        break;
    }

    return p_addr;
}

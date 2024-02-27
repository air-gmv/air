/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file syscalls_virtualization.c
 * \author lumm
 * \brief ARM virtualization system calls
 */

#include <svc.h>
#include <printk.h>

extern pmk_sharedarea_t air_shared_area;

air_u32_t arm_syscall_disable_interrupts(pmk_core_ctrl_t *core)
{

    air_u32_t level = core->context->vcpu.psr;
    core->context->vcpu.psr |= ARM_PSR_I;
    return level;
}
void arm_syscall_enable_interrupts(pmk_core_ctrl_t *core)
{

    core->context->vcpu.psr &= ~ARM_PSR_I;
}

void arm_syscall_disable_traps(pmk_core_ctrl_t *core)
{

    core->context->vcpu.psr |= (ARM_PSR_A | ARM_PSR_I);
}

void arm_syscall_enable_traps(pmk_core_ctrl_t *core)
{

    core->context->vcpu.psr &= ~(ARM_PSR_A | ARM_PSR_I);
}

air_u32_t arm_syscall_disable_fpu(pmk_core_ctrl_t *core)
{
#if PMK_FPU_SUPPORT
    if (core->partition->permissions & AIR_PERMISSION_FPU_CONTROL == AIR_PERMISSION_FPU_CONTROL)
    {
         core->context->vfp_context->fpexc &= ~ARM_VFP_FPEXC_ENABLE;
         core->context->vfp_context->fpexc |= 0x00000F80;
        //((arm_interrupt_stack_frame_t *)core->context->isf_pointer)->vfp_context.fpexc &= 0x00000000;
        //((arm_interrupt_stack_frame_t *)core->context->isf_pointer)->vfp_context.fpscr |= 0x00000F80;
        return AIR_NO_ERROR;
    }
    else
    {
        return AIR_INVALID_CONFIG;
    }
#endif
    return AIR_INVALID_CONFIG;

}

void arm_syscall_enable_fpu(pmk_core_ctrl_t *core)
{
#if PMK_FPU_SUPPORT
    //((arm_interrupt_stack_frame_t *)core->context->isf_pointer)->vfp_context.fpexc |= ARM_VFP_FPEXC_ENABLE;
     core->context->vfp_context->fpexc |= ARM_VFP_FPEXC_ENABLE;
     core->context->vfp_context->fpscr &=0xFFF8FFFF;
#endif

}

air_u32_t arm_syscall_get_tbr(pmk_core_ctrl_t *core)
{

    air_uptr_t *vbar = core->context->vcpu.vbar;
    return (air_u32_t)vbar;
}

void arm_syscall_set_tbr(pmk_core_ctrl_t *core, void *val)
{

    core->context->vcpu.vbar = (air_uptr_t *)val;
}

air_u32_t arm_syscall_get_psr(pmk_core_ctrl_t *core)
{

    air_u32_t psr = core->context->vcpu.psr;
    return psr;
}

void arm_syscall_set_psr(pmk_core_ctrl_t *core, air_u32_t val)
{

    arm_interrupt_stack_frame_t *isf_pointer = ((arm_interrupt_stack_frame_t *)core->context->isf_pointer);

    switch ((core->context->vcpu.psr) & ARM_PSR_MODE_MASK)
    {
        case ARM_PSR_SVC:
            core->context->virt.sp_svc = isf_pointer->usr_sp;
            core->context->virt.usr_svc_lr = isf_pointer->usr_lr;
            break;
        case ARM_PSR_IRQ:
            core->context->virt.sp_irq = isf_pointer->usr_sp;
            core->context->virt.usr_irq_lr = isf_pointer->usr_lr;
            break;
    }


    if (((core->context->vcpu.psr) & ARM_PSR_MODE_MASK) != (val & ARM_PSR_MODE_MASK))
    {
        switch (val & ARM_PSR_MODE_MASK){
            case ARM_PSR_SVC:
                isf_pointer->usr_sp = core->context->virt.sp_svc;
                isf_pointer->usr_lr = core->context->virt.usr_svc_lr;
                break;
            case ARM_PSR_IRQ:
                isf_pointer->usr_sp = core->context->virt.sp_irq;
                isf_pointer->usr_lr = core->context->virt.usr_irq_lr;
                break;
        }
    }

    core->context->vcpu.psr = val;
}


air_u32_t arm_syscall_get_spsr(pmk_core_ctrl_t *core) 
{

    air_u32_t spsr = core->context->virt.usr_spsr;
    return spsr;
}

void arm_syscall_set_spsr(pmk_core_ctrl_t *core, air_u32_t val) {

    core->context->virt.usr_spsr = val;

}

void arm_syscall_return(pmk_core_ctrl_t *core) 
{

    arm_interrupt_stack_frame_t *isf_pointer = (arm_interrupt_stack_frame_t *)core->context->isf_pointer;

    air_u32_t handler_lr = isf_pointer->usr_lr;

    isf_pointer->usr_lr = core->context->virt.usr_svc_lr;
    isf_pointer->ret_addr = handler_lr;

    core->context->virt.sp_irq = isf_pointer->usr_sp;
    isf_pointer->usr_sp = core->context->virt.sp_svc;
    isf_pointer->ret_psr = core->context->virt.usr_spsr;
}


void arm_syscall_rett(pmk_core_ctrl_t *core) 
{
    core->context->vcpu.psr &= ~(ARM_PSR_A | ARM_PSR_I);
    core->context->vgic.pmr = 255; // return the priority mask to initial state

    // if previous virtual mode is IRQ, switch to SVC
    if (((core->context->vcpu.psr) & ARM_PSR_MODE_MASK) == ARM_PSR_IRQ)
    {
        (core->context->vcpu.psr) &= ~(ARM_PSR_MODE_MASK);
        (core->context->vcpu.psr) |= ARM_PSR_SVC;
    }

    // TODO: check if there are pending interrupts before returning to the partition;
}

// air_u32_t arm_syscall_get_cache_register(void);
// air_u32_t arm_syscall_set_cache_register(void);
// air_u32_t AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER

air_u32_t arm_syscall_get_irq_mask_register(pmk_core_ctrl_t *core)
{

    air_u32_t pmr = core->context->vgic.pmr;
    return pmr;
}

void arm_syscall_set_irq_mask_register(pmk_core_ctrl_t *core, air_u32_t val)
{

    core->context->vgic.pmr = val;
}

// air_u32_t arm_syscall_set_irq_force_register(void);

air_u32_t arm_syscall_get_core_id(pmk_core_ctrl_t *core)
{

    return core->context->vcpu.id;
}

air_u64_t arm_syscall_get_elapsed_ticks(pmk_core_ctrl_t *core)
{

    return core->partition->elapsed_ticks;
}

air_u32_t arm_syscall_acknowledge_int(pmk_core_ctrl_t *core)
{
    air_u32_t id = (core->context->vgic.iar) & 0x3ff; //Interrupt ID Mask
    return id;
}

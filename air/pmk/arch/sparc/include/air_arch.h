/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief SPARC architecture base types
 */

#ifndef __AIR_ARCH_H__
#define __AIR_ARCH_H__
#ifndef ASM
#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief NULL pointer
 */
#define NULL ((void *)0)

/**
 * @brief Base Size
 */
typedef long unsigned int air_sz_t;

/**
 * @brief Struct member offset
 */
#define air_offsetof(st, m) ((air_sz_t)(&((st *) 0)->m))

/**
 * @brief Signed 8 bit integer
 **/
typedef signed char air_i8_t;

/**
 * @brief Unsigned 8 bit integer
 **/
typedef unsigned char air_u8_t;

/**
 * @brief Signed 16 bit integer
 **/
typedef signed short air_i16_t;

/**
 * @brief Unsigned 16 bit integer
 **/
typedef unsigned short air_u16_t;

/**
 * @brief Signed 32 bit integer
 **/
typedef signed long air_i32_t;

/**
 * @brief Unsigned 32 bit integer
 **/
typedef unsigned long air_u32_t;

/**
 * @brief Signed 64 bit integer
 **/
typedef signed long long air_i64_t;

/**
 * @brief Unsigned 64 bit integer
 **/
typedef unsigned long long air_u64_t;

/**
 * @brief Signed pointer
 **/
typedef signed long air_iptr_t;

/**
 * @brief Unsigned pointer
 **/
typedef unsigned long air_uptr_t;

/**
 * @brief Unsigned 32 bit integer
 **/
typedef unsigned long air_isr_level_t;

/**
 * @brief SPARC system call: disable virtual interrupts
 * @return previous PIL
 */
air_u32_t air_syscall_disable_interrupts(void);
/**
 * @brief SPARC system call: enable virtual interrupts
 * @param pil PIL level to apply
 */
void air_syscall_enable_interrupts(air_u32_t pil);
/**
 * @brief SPARC system call: disable virtual traps
 */
void air_syscall_disable_traps(void);
/**
 * @brief SPARC system call: enable virtual traps
 */
void air_syscall_enable_traps(void);
/**
 * @brief SPARC system call: disable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
air_u32_t air_syscall_disable_fpu(void);
/**
 * @brief SPARC system call: enable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
air_u32_t air_syscall_enable_fpu(void);
/**
 * @brief SPARC system call: get PSR
 * @return Core PSR
 */
air_u32_t air_syscall_get_psr(void);
/**
 * @brief SPARC system call: set PSR
 * @param psr PSR value
 */
void air_syscall_set_psr(air_u32_t psr);
/**
 * @brief SPARC system call: virtual RETT
 * @param pc program counter
 * @param n_npc next program counter
 */
void air_syscall_virtual_rett(air_u32_t pc, air_u32_t n_pc);
/**
 * @brief SPARC system call: get cache register
 * @returns current cache register value
 */
air_u32_t air_syscall_get_cache_register(void);
/**
 * @brief SPARC system call: set cache register
 * @param cache cache register new value
 *
 * @note This system call requires permission
 */
air_u32_t air_syscall_set_cache_register(air_u32_t cache);
/**
 * @brief SPARC system call: set TBR register
 * @param tbr pointer to the trap table
 */
void air_syscall_set_tbr(air_u32_t tbr);
/**
 * @brief SPARC system call: get TBR register
 * @return TBR register value
 */
air_u32_t air_syscall_get_tbr(void);
/**
 * @brief SPARC system call: get IRQ mask register
 * @param core identifier
 * @return value of in the core IRQ mask register
 */
air_u32_t air_syscall_get_irq_mask_register(air_u32_t core_id);
/**
 * @brief SPARC system call: get IRQ mask register
 * @param core identifier
 * @param to apply to the core IRQ mask register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
air_u32_t air_syscall_set_irq_mask_register(air_u32_t core_id, air_u32_t mask);
/**
 * @brief SPARC system call: set IRQ force register
 * @param core identifier
 * @param to apply to the core IRQ force register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
air_u32_t air_syscall_set_irq_force_register(air_u32_t core_id, air_u32_t force);
/**
 * @brief SPARC system call: get instruction cache configuration register
 * @returns current cache register value
 */
air_u32_t air_syscall_get_inst_cache_config_register(void);
/**
 * @brief SPARC system call: get data cache configuration register
 * @returns current cache register value
 */
air_u32_t air_syscall_get_data_cache_config_register(void);

/**
 * @brief SPARC system call: RTEMS5 syscall_irqdis_fp version
 * @returns NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
air_u32_t air_syscall_set_ef(void);


#ifdef  __cplusplus
}
#endif /* C++ */
#endif /* !ASM */

#define AIR_SYSCALL_OS_TRAP                             0x80
#define AIR_SYSCALL_SPARC_TRAP                          0x82
#define AIR_SYSCALL_FLUSH_TRAP                          0x83
#define AIR_SYSCALL_HM_TRAP                             0x84
#define AIR_SYSCALL_PS_TRAP                             0x89

/** @todo this is hack, these defines must be normalized across ARCHs */
#define AIR_IRQ_HM_EVENT                    AIR_SYSCALL_HM_TRAP


#define AIR_SYSCALL_SPARC_DISABLE_INTERRUPTS            0
#define AIR_SYSCALL_SPARC_ENABLE_INTERRUPTS             1
#define AIR_SYSCALL_SPARC_DISABLE_TRAPS                 2
#define AIR_SYSCALL_SPARC_ENABLE_TRAPS                  3
#define AIR_SYSCALL_SPARC_DISABLE_FPU                   4
#define AIR_SYSCALL_SPARC_ENABLE_FPU                    5
#define AIR_SYSCALL_SPARC_GET_TBR                       6
#define AIR_SYSCALL_SPARC_SET_TBR                       7
#define AIR_SYSCALL_SPARC_GET_PSR                       8
#define AIR_SYSCALL_SPARC_SET_PSR                       9
#define AIR_SYSCALL_SPARC_RETT                         10
#define AIR_SYSCALL_SPARC_GET_CACHE_REGISTER           11
#define AIR_SYSCALL_SPARC_SET_CACHE_REGISTER           12
#define AIR_SYSCALL_SPARC_RESTORE_CACHE_REGISTER       13
#define AIR_SYSCALL_SPARC_GET_IRQ_MASK_REGISTER        14
#define AIR_SYSCALL_SPARC_SET_IRQ_MASK_REGISTER        15
#define AIR_SYSCALL_SPARC_SET_IRQ_FORCE_REGISTER       16
#define AIR_SYSCALL_SPARC_GET_INST_CACHE_CFG_REGISTER  17
#define AIR_SYSCALL_SPARC_GET_DATA_CACHE_CFG_REGISTER  18
#define AIR_SYSCALL_SPARC_SET_EF                       19
#define AIR_SYSCALL_SPARC_COUNT                        20

/**
 * @brief Defined as 0, as the SPARC uses two traps (one for the OS, another for
 *        the architecture para-virtualization)
 */
#define AIR_SYSCALL_ARCH_COUNT                          0

#endif /* __AIR_ARCH_H__ */

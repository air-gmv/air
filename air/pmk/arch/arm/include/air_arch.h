/**
 * @file air_arch
 * @author lumm
 * @brief ARM architecture base types. Keeping data types with the same name
 * for coherency on the pmk core. TODO
 */

#ifndef ARM_AIR_ARCH_H
#define ARM_AIR_ARCH_H
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
typedef unsigned long air_sz_t;

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
typedef signed long *air_iptr_t;

/**
 * @brief Unsigned pointer
 **/
typedef unsigned long *air_uptr_t;

/**
 * @brief Unsigned 32 bit integer
 **/
typedef unsigned long air_isr_level_t;

/**
 * @brief Boolean
 */
#define false   0
#define true    1

/**************** ARM paravirtualization syscalls declarations ****************/
/**
 * @brief ARM system call: disable virtual interrupts
 * @return previous PIL
 */
air_u32_t air_disable_interrupts(void);
/**
 * @brief ARM system call: enable virtual interrupts
 * @param pil PIL level to apply
 */
void air_enable_interrupts(air_u32_t pil);
/**
 * @brief ARM system call: disable virtual traps
 */
void air_disable_traps(void);
/**
 * @brief ARM system call: enable virtual traps
 */
void air_enable_traps(void);
/**
 * @brief ARM system call: disable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
air_u32_t air_disable_fpu(void);
/**
 * @brief ARM system call: enable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
air_u32_t air_enable_fpu(void);
/**
 * @brief ARM system call: get PSR
 * @return Core PSR
 */
air_u32_t air_get_psr(void);
/**
 * @brief ARM system call: set PSR
 * @param psr PSR value
 */
void air_set_psr(air_u32_t psr);
/**
 * @brief ARM system call: virtual RETT
 * @param pc program counter
 * @param n_npc next program counter
 */
void air_virtual_rett(air_u32_t pc, air_u32_t n_pc);
/**
 * @brief ARM system call: get cache register
 * @returns current cache register value
 */
air_u32_t air_get_cache_register(void);
/**
 * @brief ARM system call: set cache register
 * @param cache cache register new value
 *
 * @note This system call requires permission
 */
air_u32_t air_set_cache_register(air_u32_t cache);
/**
 * @brief ARM system call: set TBR register
 * @param tbr pointer to the trap table
 */
void air_set_tbr(air_u32_t tbr);
/**
 * @brief ARM system call: get TBR register
 * @return TBR register value
 */
air_u32_t air_get_tbr(void);
/**
 * @brief ARM system call: get IRQ mask register
 * @param core identifier
 * @return value of in the core IRQ mask register
 */
air_u32_t air_get_irq_mask_register(air_u32_t core_id);
/**
 * @brief ARM system call: get IRQ mask register
 * @param core identifier
 * @param to apply to the core IRQ mask register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
air_u32_t air_set_irq_mask_register(air_u32_t core_id, air_u32_t mask);
/**
 * @brief ARM system call: set IRQ force register
 * @param core identifier
 * @param to apply to the core IRQ force register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
air_u32_t air_set_irq_force_register(air_u32_t core_id, air_u32_t force);

/**
 * @brief Disable Interrupts
 * @param level Interrupt level
 */
#define air_syscall_disable_interrupts(level) \
        level = air_disable_interrupts();

/**
 * @brief Enable Interrupts
 * @param level Interrupt level
 */
#define air_syscall_enable_interrupts(level) \
        air_enable_interrupts(level);

#ifdef  __cplusplus
}
#endif /* C++ */
#endif /* !ASM */

/**
 * @brief This define contains the emulated HM event passed to the application
 *  It is coded as the 14th SGI */
#define AIR_SYSCALL_HM_TRAP                             14
#define AIR_IRQ_HM_EVENT                                AIR_SYSCALL_HM_TRAP


#define AIR_SYSCALL_ARM_DISABLE_INTERRUPTS              0
#define AIR_SYSCALL_ARM_ENABLE_INTERRUPTS               1
#define AIR_SYSCALL_ARM_DISABLE_TRAPS                   2
#define AIR_SYSCALL_ARM_ENABLE_TRAPS                    3
#define AIR_SYSCALL_ARM_DISABLE_FPU                     4
#define AIR_SYSCALL_ARM_ENABLE_FPU                      5
#define AIR_SYSCALL_ARM_GET_TBR                         6
#define AIR_SYSCALL_ARM_SET_TBR                         7
#define AIR_SYSCALL_ARM_GET_PSR                         8
#define AIR_SYSCALL_ARM_SET_PSR                         9
#define AIR_SYSCALL_ARM_RETT                            10
#define AIR_SYSCALL_ARM_GET_CACHE_REGISTER              11
#define AIR_SYSCALL_ARM_SET_CACHE_REGISTER              12
#define AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER          13
#define AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER           14
#define AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER           15
#define AIR_SYSCALL_ARM_SET_IRQ_FORCE_REGISTER          16
#define AIR_SYSCALL_ARM_COUNT                           16

/**
 * @brief Defined as the number of ARM paravirtualization system calls,
 *  as ARM uses only one virtual syscall table, the syscalls ids defined in
 *  air.h are a continuation of these
 */
#define AIR_SYSCALL_ARCH_COUNT                          17

#endif /* ARM_AIR_ARCH_H */

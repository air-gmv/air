/**
 * @file
 * @author pfnf
 * @brief SPARC architecture base types
 */

#ifndef __XKY_ARCH_H__
#define __XKY_ARCH_H__
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
typedef long unsigned int xky_sz_t;

/**
 * @brief Struct member offset
 */
#define air_offsetof(st, m) ((xky_sz_t)(&((st *) 0)->m))

/**
 * @brief Signed 8 bit integer
 **/
typedef signed char xky_i8_t;

/**
 * @brief Unsigned 8 bit integer
 **/
typedef unsigned char xky_u8_t;

/**
 * @brief Signed 16 bit integer
 **/
typedef signed short xky_i16_t;

/**
 * @brief Unsigned 16 bit integer
 **/
typedef unsigned short xky_u16_t;

/**
 * @brief Signed 32 bit integer
 **/
typedef signed long xky_i32_t;

/**
 * @brief Unsigned 32 bit integer
 **/
typedef unsigned long xky_u32_t;

/**
 * @brief Signed 64 bit integer
 **/
typedef signed long long xky_i64_t;

/**
 * @brief Unsigned 64 bit integer
 **/
typedef unsigned long long xky_u64_t;

/**
 * @brief Signed pointer
 **/
typedef signed long xky_iptr_t;

/**
 * @brief Unsigned pointer
 **/
typedef unsigned long xky_uptr_t;

/**
 * @brief Unsigned 32 bit integer
 **/
typedef unsigned long xky_isr_level_t;

/**
 * @brief SPARC system call: disable virtual interrupts
 * @return previous PIL
 */
xky_u32_t xky_sparc_disable_interrupts(void);
/**
 * @brief SPARC system call: enable virtual interrupts
 * @param pil PIL level to apply
 */
void xky_sparc_enable_interrupts(xky_u32_t pil);
/**
 * @brief SPARC system call: disable virtual traps
 */
void xky_sparc_disable_traps(void);
/**
 * @brief SPARC system call: enable virtual traps
 */
void xky_sparc_enable_traps(void);
/**
 * @brief SPARC system call: disable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
xky_u32_t xky_sparc_disable_fpu(void);
/**
 * @brief SPARC system call: enable FPU
 * @return NO_ERROR if the partition have permissions to control the FPU
 *         INVALID_CONFIG otherwise
 */
xky_u32_t xky_sparc_enable_fpu(void);
/**
 * @brief SPARC system call: get PSR
 * @return Core PSR
 */
xky_u32_t xky_sparc_get_psr(void);
/**
 * @brief SPARC system call: set PSR
 * @param psr PSR value
 */
void xky_sparc_set_psr(xky_u32_t psr);
/**
 * @brief SPARC system call: virtual RETT
 * @param pc program counter
 * @param n_npc next program counter
 */
void xky_sparc_virtual_rett(xky_u32_t pc, xky_u32_t n_pc);
/**
 * @brief SPARC system call: get cache register
 * @returns current cache register value
 */
xky_u32_t xky_sparc_get_cache_register(void);
/**
 * @brief SPARC system call: set cache register
 * @param cache cache register new value
 *
 * @note This system call requires permission
 */
xky_u32_t xky_sparc_set_cache_register(xky_u32_t cache);
/**
 * @brief SPARC system call: set TBR register
 * @param tbr pointer to the trap table
 */
void xky_sparc_set_tbr(xky_u32_t tbr);
/**
 * @brief SPARC system call: get TBR register
 * @return TBR register value
 */
xky_u32_t xky_sparc_get_tbr(void);
/**
 * @brief SPARC system call: get IRQ mask register
 * @param core identifier
 * @return value of in the core IRQ mask register
 */
xky_u32_t xky_sparc_get_irq_mask_register(xky_u32_t core_id);
/**
 * @brief SPARC system call: get IRQ mask register
 * @param core identifier
 * @param to apply to the core IRQ mask register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
xky_u32_t xky_sparc_set_irq_mask_register(xky_u32_t core_id, xky_u32_t mask);
/**
 * @brief SPARC system call: set IRQ force register
 * @param core identifier
 * @param to apply to the core IRQ force register
 * @return INVALID_CONFIG if the core isn't available
 *         NO_ERROR otherwise
 */
xky_u32_t xky_sparc_set_irq_force_register(xky_u32_t core_id, xky_u32_t force);

/**
 * @brief Disable Interrupts
 * @param level Interrupt level
 */
#define xky_syscall_disable_interrupts(level) \
        level = xky_sparc_disable_interrupts();

/**
 * @brief Enable Interrupts
 * @param level Interrupt level
 */
#define xky_syscall_enable_interrupts(level) \
        xky_sparc_enable_interrupts(level);

#ifdef  __cplusplus
}
#endif /* C++ */
#endif /* !ASM */

#define XKY_SYSCALL_OS_TRAP                             0x80
#define XKY_SYSCALL_SPARC_TRAP                          0x82
#define XKY_SYSCALL_FLUSH_TRAP                          0x83
#define XKY_SYSCALL_HM_TRAP                             0x84
#define XKY_SYSCALL_PS_TRAP                             0x89

/** @todo this is hack, these defines must be normalized across ARCHs */
#define XKY_IRQ_HM_EVENT                    XKY_SYSCALL_HM_TRAP


#define XKY_SYSCALL_SPARC_DISABLE_INTERRUPTS            0
#define XKY_SYSCALL_SPARC_ENABLE_INTERRUPTS             1
#define XKY_SYSCALL_SPARC_DISABLE_TRAPS                 2
#define XKY_SYSCALL_SPARC_ENABLE_TRAPS                  3
#define XKY_SYSCALL_SPARC_DISABLE_FPU                   4
#define XKY_SYSCALL_SPARC_ENABLE_FPU                    5
#define XKY_SYSCALL_SPARC_GET_TBR                       6
#define XKY_SYSCALL_SPARC_SET_TBR                       7
#define XKY_SYSCALL_SPARC_GET_PSR                       8
#define XKY_SYSCALL_SPARC_SET_PSR                       9
#define XKY_SYSCALL_SPARC_RETT                         10
#define XKY_SYSCALL_SPARC_GET_CACHE_REGISTER           11
#define XKY_SYSCALL_SPARC_SET_CACHE_REGISTER           12
#define XKY_SYSCALL_SPARC_RESTORE_CACHE_REGISTER       13
#define XKY_SYSCALL_SPARC_GET_IRQ_MASK_REGISTER        14
#define XKY_SYSCALL_SPARC_SET_IRQ_MASK_REGISTER        15
#define XKY_SYSCALL_SPARC_SET_IRQ_FORCE_REGISTER       16
#define XKY_SYSCALL_SPARC_COUNT                        16

/**
 * @brief Defined as 0, as the SPARC uses two traps (one for the OS, another for
 *        the architecture para-virtualization)
 */
#define XKY_SYSCALL_ARCH_COUNT                          0

#endif /* __XKY_ARCH_H__ */

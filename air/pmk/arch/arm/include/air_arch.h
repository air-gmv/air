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

#ifdef  __cplusplus
}
#endif /* C++ */
#endif /* !ASM */

/** @todo this is hack, these defines must be normalized across ARCHs */
#define AIR_IRQ_HM_EVENT                    AIR_SYSCALL_HM_TRAP


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
 * @brief Defined as the number of ARM Syscalls, as ARM uses only one virtual syscall table
 */
#define AIR_SYSCALL_ARCH_COUNT                          17

#endif /* ARM_AIR_ARCH_H */

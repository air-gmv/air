/**
 * @file
 * @author pfnf
 * @brief
 */

#include <air_arch.h>

#ifndef __AIR_H__
#define __AIR_H__

#ifndef ASM
#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief boolean value
 */
typedef air_u32_t air_boolean;
/**
 * @brief Identifier type
 */
typedef air_i32_t air_identifier_t;
/**
 * @brief Name type
 */

typedef char air_name_t[256];

/**
 * @brief Name type pointer
 */
typedef char* air_name_ptr_t;
/**
 * @brief Clock tick types
 */
typedef air_u64_t air_clocktick_t;

/**
 * @brief AIR Message type
 */
typedef char* air_message_ptr_t;

/**
 *  @brief AIR error codes
 */
typedef enum {

    AIR_NO_ERROR                = 0x00,
    AIR_SUCCESSFUL              = 0x00,    /**<  request valid and operation performed */
    AIR_NO_ACTION               = 0x01,    /**<  status of system unaffected by request */
    AIR_NOT_AVAILABLE           = 0x02,    /**<  resource required by request unavailable */
    AIR_INVALID_PARAM           = 0x03,    /**<  invalid parameter specified in request */
    AIR_INVALID_CONFIG          = 0x04,    /**<  parameter incompatible with configuration */
    AIR_INVALID_MODE            = 0x05,    /**<  request incompatible with current mode */
    AIR_TIMED_OUT               = 0x06,    /**<  time-out tied up with request has expired */
    AIR_INVALID_POINTER         = 0x07,    /**<  Invalid Memory Pointer            */
    AIR_ERROR_MSGQUEUE_FULL     = 0x08,    /**<  Queueing port full                */
    AIR_ERROR_MSGQUEUE_EMPTY    = 0x09,    /**<  Queueing port empty               */
    AIR_ERROR_MAX_PORT_NUM      = 0x0A,    /**<  Maximum number of ports exceeded  */
    AIR_INVALID_PORT_TYPE       = 0x0B,    /**<  Port has the wrong type           */
    AIR_UNSUCCESSFUL            = 0x0C,
    AIR_INTERNAL_ERROR          = 0x0D,
    AIR_INVALID_SIZE            = 0x0E,
    AIR_INVALID_ADDRESS         = 0x0F,
    AIR_OUT_OF_MEMORY           = 0x10,
    AIR_DEVICE_NOT_FOUND        = 0x11,    /**< Device not found                */
    AIR_DEVICE_ERROR            = 0x12,

} air_status_code_e;



#endif
#ifdef ASM

#define AIR_MODE_IDLE                                   0
#define AIR_MODE_COLD_START                             1
#define AIR_MODE_WARM_START                             2
#define AIR_MODE_NORMAL                                 3

#define AIR_PERMISSION_FPU_CONTROL          0x00000001
#define AIR_PERMISSION_CACHE_CONTROL        0x00000002
#define AIR_PERMISSION_GLOBAL_TIME          0x00000004
#define AIR_PERMISSION_SET_TOD              0x00000008
#define AIR_PERMISSION_SET_PARTITION_MODE   0x00000010
#define AIR_PERMISSION_SET_SCHEDULE         0x00000020
#define AIR_PERMISSION_MODULE_CONTROL       0x00000040
#define AIR_PERMISSION_SUPERVISOR           0x0000007F

#else



/**
 * @defgroup air_partition AIR - Partition Management Services
 * @brief Partition Management Services definitions and system calls
 *
 * Partition Management services allows one partition to acquire information
 * about it's execution and other partitions execution. These services also
 * allow to control the execution of other partitions. (services affecting
 * other partitions than the executing one require explicit permissions given
 * at the application configuration time).
 *
 * @{
 */

/**
 * @brief Partition operating mode enumeration
 */
typedef enum {

    AIR_MODE_IDLE         = 0x0,    /**< Partition is Idle                  */
    AIR_MODE_COLD_START   = 0x1,    /**< Partition is Cold Starting         */
    AIR_MODE_WARM_START   = 0x2,    /**< Partition is Warm Starting         */
    AIR_MODE_NORMAL       = 0x3     /**< Partition is running Normal        */

} air_operating_mode_e;

/**
 * @brief Partition start condition enumeration
 */
typedef enum {

    /** Start condition normal                                              */
    AIR_START_CONDITION_NORMAL                  = 0x0,
    /** Start condition partition restart                                   */
    AIR_START_CONDITION_PARTITION_RESTART       = 0x1,
    /** Start condition health-monitor module restart                       */
    AIR_START_CONDITION_HM_MODULE_RESTART       = 0x2,
    /** Start condition health-monitor partition restart                    */
    AIR_START_CONDITION_HM_PARTITION_RESTART    = 0x3

} air_start_condition_e;

/**
 * @brief Partition system permissions flags
 */
typedef enum {


    /** Floating Point unit control                                         */
    AIR_PERMISSION_FPU_CONTROL            = 1,
    /** Cache control                                                       */
    AIR_PERMISSION_CACHE_CONTROL          = 1 << 1,
    /** Global monotonic timer                                              */
    AIR_PERMISSION_GLOBAL_TIME            = 1 << 2,
    /** Can set system time of Day                                          */
    AIR_PERMISSION_SET_TOD                = 1 << 3,
    /** Can set other partitions operation mode                             */
    AIR_PERMISSION_SET_PARTITION_MODE     = 1 << 4,
    /** Can set change the module schedule                                  */
    AIR_PERMISSION_SET_SCHEDULE           = 1 << 5,
    /** Can control the module (without supervisor)                         */
    AIR_PERMISSION_MODULE_CONTROL         = 1 << 6,
    /** Supervisor access                                                   */
    AIR_PERMISSION_SUPERVISOR             = (1 << 7) - 1

} air_permissions_e;

/**
 * @brief Partition Status structure
 */
typedef struct {

    air_identifier_t index;                 /**< Partition index              */
    air_identifier_t window_id;             /**< Partition schedule window id */
    air_clocktick_t period;                 /**< Partition schedule period    */
    air_clocktick_t duration;               /**< Partition schedule duration  */
    air_identifier_t identifier;            /**< Partition identifier         */
    air_permissions_e permissions;          /**< Partition permissions        */
    air_operating_mode_e operating_mode;    /**< Partition operating mode     */
    air_start_condition_e start_condition;  /**< Partition start condition    */
    air_u32_t restart_count;                 /**< Number of restarts           */

} air_partition_status_t;

/**
 * @brief Get Partition Identifier
 * @param name Partition name (NULL for current partition id)
 * @param pid Partition Identifier
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition name,
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_partition_id(
        air_name_ptr_t name, air_identifier_t *pid);
/**
 * @brief Get Partition Status
 * @param pid Partition Identifier
 * @param status Partition status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition id,
 *         NO_ERROR        - no error otherwise
 */
air_status_code_e air_syscall_get_partition_status(
        air_identifier_t pid, air_partition_status_t *status);

/**
 * @brief Set Partition Operating Mode
 * @param pid Partition Identifier
 * @param mode Operating Mode to be set
 * @return INVALID_CONFIG  - if no permissions
 *         INVALID_PARAM   - if invalid mode or target partition
 *         NO_ACTION       - if transition from normal to normal
 *         NO_ERROR        - no error otherwise
 */
air_status_code_e air_syscall_set_partition_mode(
        air_identifier_t pid, air_operating_mode_e mode);

/**
 * @brief Get physical address
 * @param addr Virtual address
 * @return Physical address if available to the partition, NULL otherwise
 */
air_uptr_t air_syscall_get_physical_addr(air_uptr_t addr);

/** @} */

/**
 * @defgroup air_time AIR - Time Management Services
 * @brief Time Management Services definitions and system calls
 *
 * Time management services allows a partition to acquire information
 * about its execution time and global time of day.
 *
 * @{
 */

/**
 * @brief Time structure
 */
typedef struct {

    air_i32_t tv_sec;
    air_i32_t tv_nsec;

} air_time_t;

/**
 * @brief Gets the micro-seconds per tick
 * @return Micro-seconds per tick
 */
air_clocktick_t air_syscall_get_us_per_tick(void);

/**
 * @brief Gets the number of elapsed ticks
 * @return Number of elapsed ticks
 */
air_clocktick_t air_syscall_get_elapsed_ticks(void);

/**
 * @brief Gets the system time of day
 * @param tod System Time of Day
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the ToD haven't been set
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_tod(air_time_t *tod);

/**
 * @brief Sets the System time of day
 * @param tod System Time of Day to be set
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_set_tod(air_time_t *tod);

/** @} */

/**
 * @defgroup air_mms AIR - Multiple Module Schedules Services
 * @brief AIR Multiple Module Schedules Services definitions and system calls
 *
 * Multiple Module Schedules Services allow the acquisition of information about
 * the current module schedule and if permissions are given at configuration
 * time, they also allow to control the module schedule.
 *
 * @{
 */

/**
 * @brief Schedule Status structure
 */
typedef struct {

    /** Time of the last schedule change                                    */
    air_clocktick_t time_of_last_schedule_change;
    /** Current Schedule Identifier                                         */
    air_identifier_t current_schedule_id;
    /** Next Schedule Identifier                                            */
    air_identifier_t next_schedule_id;
    /** Current Schedule index                                              */
    air_identifier_t current_schedule_index;
    /** Next Schedule index                                                 */
    air_identifier_t next_schedule_index;

} air_schedule_status_t;

/**
 * @brief Get Schedule Identifier
 * @param name Name of the schedule to look for (current if name is NULL)
 * @param sid Schedule identifier
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_PARAM   - if invalid schedule name,
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_schedule_id(
        air_name_ptr_t name, air_identifier_t *sid);

/**
 * @brief Get Schedule Status
 * @param status Current chedule status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition id,
 *         NO_ERROR        - no error otherwise
 */
air_status_code_e air_syscall_get_schedule_status(
        air_schedule_status_t *status);

/**
 * @brief Set Module schedule
 * @param sid Next Schedule Identifier
 * @return INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         INVALID_PARAM   - if schedule id isn't valid,
 *         NO_ACTION       - if the current id is the schedule id
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_set_schedule(air_identifier_t sid);

/** @} */

/**
 * @defgroup air_ports AIR - Inter-Partition Communication
 * @brief Inter-Partition Communication definitions and system calls
 *
 * Inter-Partition Communication services allow two or more partitions to
 * exchange messages and information between them in a safe manner. The
 * communication channels between each partition must be defined at the
 * configuration time.
 *
 * @{
 */

/**
 * @brief Port type enumeration
 */
typedef enum {

    AIR_QUEUING_PORT                = 0x01,  /**< Queuing port type         */
    AIR_SAMPLING_PORT               = 0x02   /**< Sampling port type        */

} air_port_type_e;

/**
 * @brief Port direction enumeration
 */
typedef enum {

    AIR_SOURCE_PORT                 = 0x00,  /**< Port is source            */
    AIR_DESTINATION_PORT            = 0x01   /**< Port is destination       */

} air_port_direction_e;

/**
 * @brief Port message validity enumeration
 */
typedef enum {

    AIR_MESSAGE_INVALID             = 0x00,  /**< Message is invalid        */
    AIR_MESSAGE_VALID               = 0x01   /**< Message is valid          */

} air_message_validity_e;

/**
 * @brief Port message age enumeration
 */
typedef enum {

    AIR_MESSAGE_STALE               = 0x00,  /**< Message is stale          */
    AIR_MESSAGE_FRESH               = 0x01   /**< Message is fresh          */

} air_message_age_e;

/**
 * @brief Port updated state enumeration
 */
typedef enum {

    AIR_EMPTY_PORT                  = 0x00,  /**< Port is empty             */
    AIR_CONSUMED_MESSAGE            = 0x01,  /**< Port message consumed     */
    AIR_NEW_MESSAGE                 = 0x02   /**< Port message is new       */

} air_port_updated_e;

/**
 * @brief Sampling port mode enumeration
 */
typedef enum {

    AIR_SAMPLING_MODE_REGULAR       = 0x0,   /**< Sampling port regular     */
    AIR_SAMPLING_MODE_UPDATED       = 0x1,   /**< Sampling port updated     */
    AIR_SAMPLING_MODE_CONDITIONAL   = 0x2    /**< Sampling port conditional */

} air_sampling_port_mode_e;

/**
 * @brief Queuing Port Status and Configuration structure
 */
typedef struct {

    air_u32_t nb_message;                        /**< Current message number  */
    air_u32_t max_nb_message;                    /**< Max number of messages  */
    air_u32_t max_message_size;                  /**< Max message size        */
    air_u32_t port_direction;                    /**< Port direction          */
    air_u32_t port_discipline;                   /**< Port discipline         */

} air_queuing_port_status_t, air_queuing_port_configuration_t;

/**
 * @brief Sampling Port Status and Configuration structure
 */
typedef struct {

    air_clocktick_t refresh_period;             /**< Port refresh period     */
    air_clocktick_t time_stamp;                 /**< Message time stamp      */
    air_u32_t max_message_size;                  /**< Max number of messages  */
    air_port_direction_e port_direction;        /**< Port direction          */
    air_u32_t port_discipline;                   /**< Port discipline         */
    air_message_age_e message_age;              /**< Message age             */
    air_port_updated_e updated;                 /**< Message updated state   */
    air_message_validity_e last_msg_validity;   /**< Message validity        */
    air_sampling_port_mode_e operation;         /**< Port mode               */

} air_sampling_port_status_t, air_sampling_port_configuration_t;

/**
 * @brief Memory attributes flags
 */
typedef enum {

    AIR_MEMORY_ATTRIBUTE_READ       = 0x01, /**< Memory can be read         */
    AIR_MEMORY_ATTRIBUTE_WRITE      = 0x02, /**< Memory can be written      */
    AIR_MEMORY_ATTRIBUTE_EXECUTE    = 0x04, /**< Memory can be executed     */

} air_memory_attributes_e;

/**
 * @brief Shared Memory structure
 */
typedef struct {

    air_identifier_t id;                    /**< Shared Memory identifier   */
    air_memory_attributes_e permissions;    /**< Shared Memory attributes   */
    void *address;                          /**< Shared Memory bloc address */
    air_sz_t size;                            /**< Shared Memory block size   */

} air_sharedmemory_t;

/**
 * @brief Get port id
 * @param type Type of the port to look for
 * @param name Name of the port to look for
 * @param pid Pointer to store the port id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if port not found
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_port_id(
        air_port_type_e type, air_name_ptr_t name, air_identifier_t *pid);

/**
 * @brief Create and initializes a communication port
 * @param type Type of the port
 * @param name Name of the port to look for
 * @param config Pointer to the port configuration
 * @param pid Pointer to store the port id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if port not found or configuration
 *         NO_ACTION       - if port already created
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_create_port(
        air_port_type_e type, air_name_ptr_t name,
        void *config, air_identifier_t *pid);

/**
 * @brief Get port status
 * @param type Type of the port
 * @param pid Port identifier to get the status
 * @param status Pointer to store the port status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if configuration mismatch or port not created
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_port_status(
        air_port_type_e type, air_identifier_t pid, void *status);

/**
 * @brief Read a message from a port
 * @param type Type of the port
 * @param pid Port identifier
 * @param msg Pointer to store the message
 * @param length Pointer to store the message length
 * @param config Pointer to read configuration
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if configuration mismatch or port not created
 *         INVALID_MODE    - if port isn't configured as destination
 *         NO_ERROR        - otherwise
 *
 * @note Refer to ARINC 653 specification for type dependent return codes
 */
air_status_code_e air_syscall_read_port(
        air_port_type_e type, air_identifier_t pid,
        air_message_ptr_t msg, air_sz_t *length, void *config);

/**
 * @brief Write message in a port
 * @param type Type of the port
 * @param pid Port identifier
 * @param msg Pointer to the message
 * @param length Length of the message
 * @param config Pointer to read configuration
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if configuration mismatch or port not created
 *         INVALID_MODE    - if port isn't configured as source
 *         NO_ERROR        - otherwise
 *
 * @note Refer to ARINC 653 specification for type dependent return codes
 */
air_status_code_e air_syscall_write_port(
        air_port_type_e type, air_identifier_t pid,
        air_message_ptr_t msg, air_sz_t length, void *config);

/**
 * @brief Get shared memory area
 * @param name of the shared memory area
 * @param shm shared memory area parameters
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if shared area not found
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_get_sharedmemory(
        air_name_ptr_t name, air_sharedmemory_t *shm);

/** @} */

/**
 * @defgroup air_hm AIR - Health-Monitoring Services
 * @brief Health-Monitoring Services definitions and system calls
 *
 * Health-Monitoring services allows the partition to attempt recovering actions
 * or to report errors during its execution. The recovery actions are defined at
 * the configuration time.
  *
 * @{
 */

/**
 * @brief System state enumeration
 */
typedef enum {

    AIR_STATE_MODULE_INIT       = 0x00,     /**< Module Initialization      */
    AIR_STATE_MODULE_EXEC       = 0x01,     /**< Module Execution           */
    AIR_STATE_PARTITION_INIT    = 0x02,     /**< Partition Initialization   */
    AIR_STATE_PARTITION_EXEC    = 0x03,     /**< Partition Execution        */
    AIR_STATE_PARTITION_PROC    = 0x04,     /**< Partition Process          */
    AIR_STATE_PARTITION_HM      = 0x05,     /**< Partition Health-Monitor   */
    AIR_STATE_MODULE_HM         = 0x06,     /**< Module Heath-Monitor       */

} air_state_e;

/**
 * @brief Errors enumeration
 */
typedef enum {

    AIR_POWER_ERROR             = 0x00,     /**< power error                */
    AIR_VIOLATION_ERROR         = 0x01,     /**< violation error            */
    AIR_SEGMENTATION_ERROR      = 0x02,     /**< segmentation error         */
    AIR_UNIMPLEMENTED_ERROR     = 0x03,     /**< unimplemented error        */
    AIR_FLOAT_ERROR             = 0x04,     /**< floating point error       */
    AIR_ARITHMETIC_ERROR        = 0x05,     /**< arithmetic error           */
    AIR_DIVISION_BY_0_ERROR     = 0x06,     /**< division by zero error     */
    AIR_IO_ERROR                = 0x07,     /**< I/O error                  */
    AIR_USR0_ERROR              = 0x08,     /**< user defined error 0       */
    AIR_USR1_ERROR              = 0x09,     /**< user defined error 1       */
    AIR_USR2_ERROR              = 0x0A,     /**< user defined error 2       */
    AIR_USR3_ERROR              = 0x0B,     /**< user defined error 3       */
    AIR_USR4_ERROR              = 0x0C,     /**< user defined error 4       */
    AIR_USR5_ERROR              = 0x0D,     /**< user defined error 5       */
    AIR_USR6_ERROR              = 0x0E,     /**< user defined error 6       */
    AIR_USR7_ERROR              = 0x0F,     /**< user defined error 7       */

} air_error_e;

/**
 * @brief Health-Monitor event
 */
typedef struct {

    air_state_e state_id;                   /**< State identifier           */
    air_error_e error_id;                   /**< Error identifier           */

} air_hm_event_t;

/**
 * @brief Get the current system state
 * @return Current system state
 */
air_state_e air_syscall_get_system_state(void);

/**
 * @brief Set the current system state
 * @param state State to be set
 * @return SUCCESS if the state transition is valid,
 *         NO_ACTION otherwise
 */
air_status_code_e air_syscall_set_system_state(air_state_e state);

/**
 * @brief Get the lasted health-monitor event
 * @param event Pointer to store the HM event
 */
void air_syscall_get_hm_event(air_hm_event_t *event);

/**
 * @brief Raise an health-monitor event
 * @param error Error code for the HM event
 */
void air_syscall_raise_hm_event(air_error_e error);

/** @} */

/**
 * @defgroup air_cache AIR - Cache Management Services
 * @brief Cache Management Services definitions and system calls
 *
 * The following services allows a Partition to control the cache state
 * (provided that the partition was given permission via the configuration).
 *
 * @{
 */

/**
 * @brief Cache type enumeration
 */
typedef enum {

    AIR_CACHE_ALL         = 0x00,       /**< instruction and Data cache   */
    AIR_CACHE_DATA        = 0x01,       /**< data cache only              */
    AIR_CACHE_INSTRUCTION = 0x02,       /**< instruction cache only       */
    AIR_CACHE_NONE        = 0x03        /**< no cache                     */

} air_cache_e;

/**
 * @brief Enable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */
air_status_code_e air_syscall_enable_cache(air_cache_e type);

/**
 * @brief Disable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

air_status_code_e air_syscall_disable_cache(air_cache_e type);
/**
 * @brief Freeze cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

air_status_code_e air_syscall_freeze_cache(air_cache_e type);

/**
 * @brief Flush cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

air_status_code_e air_syscall_flush_cache(air_cache_e type);

/** @} */

/**
 * @defgroup air_multicore AIR - Multi-Core Services
 * @brief Multi-Core Services definitions and system calls
 *
 * Multi-Core services allows partitions configured to use more than one core to
 * control the flow of execution of its allocated cores in a safe manner.
 *
 * @{
 */

/**
 * @brief Get last IRQ number
 * @return Number of the IRQ, -1 if no IRQ is available
 */
air_i32_t air_syscall_get_irq(void);
/**
 * @brief Mask IRQ
 * @param irq Interrupt number
 */
void air_syscall_mask_irq(air_u32_t irq);
/**
 * @brief Unmask IRQ
 * @param irq Interrupt number
 */
void air_syscall_unmask_irq(air_u32_t irq);

/**
 * @brief Get the current partition number of cores
 * @return partition number of cores
 */
air_u32_t air_syscall_get_nb_cores(void);
/**
 * @brief Get the current core id
 * @return current core id
 */
air_u32_t air_syscall_get_core_id(void);
/**
 * @brief Boot a core at a given entry point
 * @param idx core index
 * @param entry_point core entry point
 * @return INVALID_CONFIG  - if the partition doesn't have access to core idx,
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_boot_core(air_u32_t idx, void *entry_point);
/**
 * @brief Copy boot core virtual tbr to secondary core
 * @param idx secondary core index
 * @return INVALID_CONFIG  - if the partition doesn't have access to core idx,
 *         NO_ERROR        - otherwise
 */
air_status_code_e air_syscall_set_tbr(air_u32_t idx);
/** @} */

/**
 * @defgroup air_events AIR - Event Services
 * @brief Event Services definitions and system calls
 * @{
 */

#define AIR_EVENT_CLOCKTICK             0x00000001
#define AIR_EVENT_HEALTH_MONITOR        0x00000002
#define AIR_EVENT_SCHEDULE_CHANGE       0x00000004
#define AIR_EVENT_SCHEDULE_START        0x00000008
#define AIR_EVENT_PERIOD_START          0x00000010
#define AIR_EVENT_WINDOW_START          0x00000020
#define AIR_EVENT_PORT_UPDATED          0x00000040

/**
 * @brief Gets Events raised by the PMK
 * @return Events Mask
 * @note Reading the events will clear them
 */
air_u32_t air_syscall_get_event(void);

/** @} */


/**
 * @brief Prints a partition character
 * @param ch Character to be printed
 */
void air_syscall_putchar(char ch);

/**
 * @brief Prints a partition buffer
 * @param buffer Buffer to be printed
 * @param len Length of the buffer
 * @return Number of characters printed
 */
air_u32_t air_syscall_print(char *str, air_sz_t len);


/**
 * @brief Shutdown module
 */
void air_syscall_shutdown_module(void);

/**
 * @brief Extract AMBA Plug & Play configuration area
 * @param ambaptr pointer to amba config area
 */
air_uptr_t air_syscall_get_ambaconf(void);

/**
 * @}
 */

#ifdef  __cplusplus
}
#endif
#endif  /* !ASM */

#define AIR_SYSCALL_GET_IRQ                        (AIR_SYSCALL_ARCH_COUNT + 0 )
#define AIR_SYSCALL_MASK_IRQ                       (AIR_SYSCALL_ARCH_COUNT + 1 )
#define AIR_SYSCALL_UNMASK_IRQ                     (AIR_SYSCALL_ARCH_COUNT + 2 )
#define AIR_SYSCALL_GET_P_ADDR                     (AIR_SYSCALL_ARCH_COUNT + 3 )
#define AIR_SYSCALL_GET_NB_CORES                   (AIR_SYSCALL_ARCH_COUNT + 4 )
#define AIR_SYSCALL_GET_CORE_ID                    (AIR_SYSCALL_ARCH_COUNT + 5 )
#define AIR_SYSCALL_SET_TBR                        (AIR_SYSCALL_ARCH_COUNT + 6 )
#define AIR_SYSCALL_BOOT_CORE                      (AIR_SYSCALL_ARCH_COUNT + 7 )
#define AIR_SYSCALL_GET_US_PER_TICK                (AIR_SYSCALL_ARCH_COUNT + 8 )
#define AIR_SYSCALL_GET_ELAPSED_TICKS              (AIR_SYSCALL_ARCH_COUNT + 9 )
#define AIR_SYSCALL_GET_PARTITION_ID               (AIR_SYSCALL_ARCH_COUNT + 10)
#define AIR_SYSCALL_GET_PARTITION_STATUS           (AIR_SYSCALL_ARCH_COUNT + 11)
#define AIR_SYSCALL_SET_PARTITION_MODE             (AIR_SYSCALL_ARCH_COUNT + 12)
#define AIR_SYSCALL_GET_SCHEDULE_ID                (AIR_SYSCALL_ARCH_COUNT + 13)
#define AIR_SYSCALL_GET_SCHEDULE_STATUS            (AIR_SYSCALL_ARCH_COUNT + 14)
#define AIR_SYSCALL_SET_SCHEDULE                   (AIR_SYSCALL_ARCH_COUNT + 15)
#define AIR_SYSCALL_GET_TIME_OF_DAY                (AIR_SYSCALL_ARCH_COUNT + 16)
#define AIR_SYSCALL_SET_TIME_OF_DAY                (AIR_SYSCALL_ARCH_COUNT + 17)
#define AIR_SYSCALL_GET_PORT_ID                    (AIR_SYSCALL_ARCH_COUNT + 18)
#define AIR_SYSCALL_GET_PORT_STATUS                (AIR_SYSCALL_ARCH_COUNT + 19)
#define AIR_SYSCALL_CREATE_PORT                    (AIR_SYSCALL_ARCH_COUNT + 20)
#define AIR_SYSCALL_READ_PORT                      (AIR_SYSCALL_ARCH_COUNT + 21)
#define AIR_SYSCALL_WRITE_PORT                     (AIR_SYSCALL_ARCH_COUNT + 22)
#define AIR_SYSCALL_GET_SHARED_AREA                (AIR_SYSCALL_ARCH_COUNT + 23)
#define AIR_SYSCALL_RESTART_MODULE                 (AIR_SYSCALL_ARCH_COUNT + 24)
#define AIR_SYSCALL_SHUTDOWN_MODULE                (AIR_SYSCALL_ARCH_COUNT + 25)
#define AIR_SYSCALL_GET_EVENT                      (AIR_SYSCALL_ARCH_COUNT + 26)
#define AIR_SYSCALL_GET_SYSTEM_STATE               (AIR_SYSCALL_ARCH_COUNT + 27)
#define AIR_SYSCALL_SET_SYSTEM_STATE               (AIR_SYSCALL_ARCH_COUNT + 28)
#define AIR_SYSCALL_GET_HM_EVENT                   (AIR_SYSCALL_ARCH_COUNT + 29)
#define AIR_SYSCALL_RAISE_HM_EVENT                 (AIR_SYSCALL_ARCH_COUNT + 30)
#define AIR_SYSCALL_PUTCHAR                        (AIR_SYSCALL_ARCH_COUNT + 31)
#define AIR_SYSCALL_PRINT                          (AIR_SYSCALL_ARCH_COUNT + 32)
#define AIR_SYSCALL_ENABLE_CACHE                   (AIR_SYSCALL_ARCH_COUNT + 33)
#define AIR_SYSCALL_DISABLE_CACHE                  (AIR_SYSCALL_ARCH_COUNT + 34)
#define AIR_SYSCALL_FREEZE_CACHE                   (AIR_SYSCALL_ARCH_COUNT + 35)
#define AIR_SYSCALL_FLUSH_CACHE                    (AIR_SYSCALL_ARCH_COUNT + 36)
#define AIR_SYSCALL_GET_AMBACONF                   (AIR_SYSCALL_ARCH_COUNT + 37)
#define AIR_SYSCALL_COUNT                          (AIR_SYSCALL_ARCH_COUNT + 38)

#endif /* __AIR_H__ */

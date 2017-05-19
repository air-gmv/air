/**
 * @file
 * @author pfnf
 * @brief
 */

#include <xky_arch.h>

#ifndef __XKY_H__
#define __XKY_H__

#ifndef ASM
#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief boolean value
 */
typedef xky_u32_t xky_boolean;
/**
 * @brief Identifier type
 */
typedef xky_i32_t xky_identifier_t;
/**
 * @brief Name type
 */
typedef char xky_name_t[32];
/**
 * @brief Name type pointer
 */
typedef char* xky_name_ptr_t;
/**
 * @brief Clock tick types
 */
typedef xky_i64_t xky_clocktick_t;

/**
 * @brief XKY Message type
 */
typedef char* xky_message_ptr_t;

/**
 *  @brief XKY error codes
 */
typedef enum {

    XKY_NO_ERROR                = 0x00,
    XKY_SUCCESSFUL              = 0x00,    /**<  request valid and operation performed */
    XKY_NO_ACTION               = 0x01,    /**<  status of system unaffected by request */
    XKY_NOT_AVAILABLE           = 0x02,    /**<  resource required by request unavailable */
    XKY_INVALID_PARAM           = 0x03,    /**<  invalid parameter specified in request */
    XKY_INVALID_CONFIG          = 0x04,    /**<  parameter incompatible with configuration */
    XKY_INVALID_MODE            = 0x05,    /**<  request incompatible with current mode */
    XKY_TIMED_OUT               = 0x06,    /**<  time-out tied up with request has expired */
    XKY_INVALID_POINTER         = 0x07,    /**<  Invalid Memory Pointer            */
    XKY_ERROR_MSGQUEUE_FULL     = 0x08,    /**<  Queueing port full                */
    XKY_ERROR_MSGQUEUE_EMPTY    = 0x09,    /**<  Queueing port empty               */
    XKY_ERROR_MAX_PORT_NUM      = 0x0A,    /**<  Maximum number of ports exceeded  */
    XKY_INVALID_PORT_TYPE       = 0x0B,    /**<  Port has the wrong type           */
    XKY_UNSUCCESSFUL            = 0x0C,
    XKY_INTERNAL_ERROR          = 0x0D,
    XKY_INVALID_SIZE            = 0x0E,
    XKY_INVALID_ADDRESS         = 0x0F,
    XKY_OUT_OF_MEMORY           = 0x10,
    XKY_DEVICE_NOT_FOUND        = 0x11,    /**< Device not found                */
    XKY_DEVICE_ERROR            = 0x12,

} xky_status_code_e;



#endif
#ifdef ASM

#define XKY_MODE_IDLE                                   0
#define XKY_MODE_COLD_START                             1
#define XKY_MODE_WARM_START                             2
#define XKY_MODE_NORMAL                                 3

#define XKY_PERMISSION_SUPERVISOR           0x00000001
#define XKY_PERMISSION_FPU_CONTROL          0x00000002
#define XKY_PERMISSION_CACHE_CONTROL        0x00000004
#define XKY_PERMISSION_GLOBAL_TIME          0x00000008
#define XKY_PERMISSION_SET_TOD              0x00000010
#define XKY_PERMISSION_SET_PARTITION_MODE   0x00000020
#define XKY_PERMISSION_SET_SCHEDULE         0x00000040
#define XKY_PERMISSION_ARINC_SYSTEM         0x00000080
#define XKY_PERMISSION_MODULE_CONTROL       0x00000100
#else



/**
 * @defgroup xky_partition XKY - Partition Management Services
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

    XKY_MODE_IDLE         = 0x0,    /**< Partition is Idle                  */
    XKY_MODE_COLD_START   = 0x1,    /**< Partition is Cold Starting         */
    XKY_MODE_WARM_START   = 0x2,    /**< Partition is Warm Starting         */
    XKY_MODE_NORMAL       = 0x3     /**< Partition is running Normal        */

} xky_operating_mode_e;

/**
 * @brief Partition start condition enumeration
 */
typedef enum {

    /** Start condition normal                                              */
    XKY_START_CONDITION_NORMAL                  = 0x0,
    /** Start condition partition restart                                   */
    XKY_START_CONDITION_PARTITION_RESTART       = 0x1,
    /** Start condition health-monitor module restart                       */
    XKY_START_CONDITION_HM_MODULE_RESTART       = 0x2,
    /** Start condition health-monitor partition restart                    */
    XKY_START_CONDITION_HM_PARTITION_RESTART    = 0x3

} xky_start_condition_e;

/**
 * @brief Partition system permissions flags
 */
typedef enum {

    /** Supervisor access                                                   */
    XKY_PERMISSION_SUPERVISOR             = 0x00000001,
    /** Floating Point unit control                                         */
    XKY_PERMISSION_FPU_CONTROL            = 0x00000002,
    /** Cache control                                                       */
    XKY_PERMISSION_CACHE_CONTROL          = 0x00000004,
    /** Global monotonic timer                                              */
    XKY_PERMISSION_GLOBAL_TIME            = 0x00000008,
    /** Can set system time of Day                                          */
    XKY_PERMISSION_SET_TOD                = 0x00000010,
    /** Can set other partitions operation mode                             */
    XKY_PERMISSION_SET_PARTITION_MODE     = 0x00000020,
    /** Can set change the module schedule                                  */
    XKY_PERMISSION_SET_SCHEDULE           = 0x00000040,
    /** Act as ARINC system partition                                       */
    XKY_PERMISSION_ARINC_SYSTEM           = 0x00000080,
    /** Can control the module (without supervisor)                         */
    XKY_PERMISSION_MODULE_CONTROL         = 0x00000100,

} xky_permissions_e;

/**
 * @brief Partition Status structure
 */
typedef struct {

    xky_identifier_t index;                 /**< Partition index              */
    xky_identifier_t window_id;             /**< Partition schedule window id */
    xky_clocktick_t period;                 /**< Partition schedule period    */
    xky_clocktick_t duration;               /**< Partition schedule duration  */
    xky_identifier_t identifier;            /**< Partition identifier         */
    xky_permissions_e permissions;          /**< Partition permissions        */
    xky_operating_mode_e operating_mode;    /**< Partition operating mode     */
    xky_start_condition_e start_condition;  /**< Partition start condition    */
    xky_u32_t restart_count;                 /**< Number of restarts           */

} xky_partition_status_t;

/**
 * @brief Get Partition Identifier
 * @param name Partition name (NULL for current partition id)
 * @param pid Partition Identifier
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition name,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_partition_id(
        xky_name_ptr_t name, xky_identifier_t *pid);
/**
 * @brief Get Partition Status
 * @param pid Partition Identifier
 * @param status Partition status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition id,
 *         NO_ERROR        - no error otherwise
 */
xky_status_code_e xky_syscall_get_partition_status(
        xky_identifier_t pid, xky_partition_status_t *status);

/**
 * @brief Set Partition Operating Mode
 * @param pid Partition Identifier
 * @param mode Operating Mode to be set
 * @return INVALID_CONFIG  - if no permissions
 *         INVALID_PARAM   - if invalid mode or target partition
 *         NO_ACTION       - if transition from normal to normal
 *         NO_ERROR        - no error otherwise
 */
xky_status_code_e xky_syscall_set_partition_mode(
        xky_identifier_t pid, xky_operating_mode_e mode);

/**
 * @brief Get physical address
 * @param addr Virtual address
 * @return Physical address if available to the partition, NULL otherwise
 */
xky_uptr_t xky_syscall_get_physical_addr(xky_uptr_t addr);

/** @} */

/**
 * @defgroup xky_time XKY - Time Management Services
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

    xky_i32_t tv_sec;
    xky_i32_t tv_nsec;

} xky_time_t;

/**
 * @brief Gets the micro-seconds per tick
 * @return Micro-seconds per tick
 */
xky_clocktick_t xky_syscall_get_us_per_tick(void);

/**
 * @brief Gets the number of elapsed ticks
 * @return Number of elapsed ticks
 */
xky_clocktick_t xky_syscall_get_elapsed_ticks(void);

/**
 * @brief Gets the system time of day
 * @param tod System Time of Day
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the ToD haven't been set
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_tod(xky_time_t *tod);

/**
 * @brief Sets the System time of day
 * @param tod System Time of Day to be set
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_set_tod(xky_time_t *tod);

/** @} */

/**
 * @defgroup xky_mms XKY - Multiple Module Schedules Services
 * @brief XKY Multiple Module Schedules Services definitions and system calls
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
    xky_clocktick_t time_of_last_schedule_change;
    /** Current Schedule Identifier                                         */
    xky_identifier_t current_schedule_id;
    /** Next Schedule Identifier                                            */
    xky_identifier_t next_schedule_id;
    /** Current Schedule index                                              */
    xky_identifier_t current_schedule_index;
    /** Next Schedule index                                                 */
    xky_identifier_t next_schedule_index;

} xky_schedule_status_t;

/**
 * @brief Get Schedule Identifier
 * @param name Name of the schedule to look for (current if name is NULL)
 * @param sid Schedule identifier
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_PARAM   - if invalid schedule name,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_schedule_id(
        xky_name_ptr_t name, xky_identifier_t *sid);

/**
 * @brief Get Schedule Status
 * @param status Current chedule status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition id,
 *         NO_ERROR        - no error otherwise
 */
xky_status_code_e xky_syscall_get_schedule_status(
        xky_schedule_status_t *status);

/**
 * @brief Set Module schedule
 * @param sid Next Schedule Identifier
 * @return INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         INVALID_PARAM   - if schedule id isn't valid,
 *         NO_ACTION       - if the current id is the schedule id
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_set_schedule(xky_identifier_t sid);

/** @} */

/**
 * @defgroup xky_ports XKY - Inter-Partition Communication
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

    XKY_QUEUING_PORT                = 0x01,  /**< Queuing port type         */
    XKY_SAMPLING_PORT               = 0x02   /**< Sampling port type        */

} xky_port_type_e;

/**
 * @brief Port direction enumeration
 */
typedef enum {

    XKY_SOURCE_PORT                 = 0x00,  /**< Port is source            */
    XKY_DESTINATION_PORT            = 0x01   /**< Port is destination       */

} xky_port_direction_e;

/**
 * @brief Port message validity enumeration
 */
typedef enum {

    XKY_MESSAGE_INVALID             = 0x00,  /**< Message is invalid        */
    XKY_MESSAGE_VALID               = 0x01   /**< Message is valid          */

} xky_message_validity_e;

/**
 * @brief Port message age enumeration
 */
typedef enum {

    XKY_MESSAGE_STALE               = 0x00,  /**< Message is stale          */
    XKY_MESSAGE_FRESH               = 0x01   /**< Message is fresh          */

} xky_message_age_e;

/**
 * @brief Port updated state enumeration
 */
typedef enum {

    XKY_EMPTY_PORT                  = 0x00,  /**< Port is empty             */
    XKY_CONSUMED_MESSAGE            = 0x01,  /**< Port message consumed     */
    XKY_NEW_MESSAGE                 = 0x02   /**< Port message is new       */

} xky_port_updated_e;

/**
 * @brief Sampling port mode enumeration
 */
typedef enum {

    XKY_SAMPLING_MODE_REGULAR       = 0x0,   /**< Sampling port regular     */
    XKY_SAMPLING_MODE_UPDATED       = 0x1,   /**< Sampling port updated     */
    XKY_SAMPLING_MODE_CONDITIONAL   = 0x2    /**< Sampling port conditional */

} xky_sampling_port_mode_e;

/**
 * @brief Queuing Port Status and Configuration structure
 */
typedef struct {

    xky_u32_t nb_message;                        /**< Current message number  */
    xky_u32_t max_nb_message;                    /**< Max number of messages  */
    xky_u32_t max_message_size;                  /**< Max message size        */
    xky_u32_t port_direction;                    /**< Port direction          */
    xky_u32_t port_discipline;                   /**< Port discipline         */

} xky_queuing_port_status_t, xky_queuing_port_configuration_t;

/**
 * @brief Sampling Port Status and Configuration structure
 */
typedef struct {

    xky_clocktick_t refresh_period;             /**< Port refresh period     */
    xky_clocktick_t time_stamp;                 /**< Message time stamp      */
    xky_u32_t max_message_size;                  /**< Max number of messages  */
    xky_port_direction_e port_direction;        /**< Port direction          */
    xky_u32_t port_discipline;                   /**< Port discipline         */
    xky_message_age_e message_age;              /**< Message age             */
    xky_port_updated_e updated;                 /**< Message updated state   */
    xky_message_validity_e last_msg_validity;   /**< Message validity        */
    xky_sampling_port_mode_e operation;         /**< Port mode               */

} xky_sampling_port_status_t, xky_sampling_port_configuration_t;

/**
 * @brief Memory attributes flags
 */
typedef enum {

    XKY_MEMORY_ATTRIBUTE_READ       = 0x01, /**< Memory can be read         */
    XKY_MEMORY_ATTRIBUTE_WRITE      = 0x02, /**< Memory can be written      */
    XKY_MEMORY_ATTRIBUTE_EXECUTE    = 0x04, /**< Memory can be executed     */

} xky_memory_attributes_e;

/**
 * @brief Shared Memory structure
 */
typedef struct {

    xky_identifier_t id;                    /**< Shared Memory identifier   */
    xky_memory_attributes_e permissions;    /**< Shared Memory attributes   */
    void *address;                          /**< Shared Memory bloc address */
    xky_sz_t size;                            /**< Shared Memory block size   */

} xky_sharedmemory_t;

/**
 * @brief Get port id
 * @param type Type of the port to look for
 * @param name Name of the port to look for
 * @param pid Pointer to store the port id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if port not found
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_port_id(
        xky_port_type_e type, xky_name_ptr_t name, xky_identifier_t *pid);

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
xky_status_code_e xky_syscall_create_port(
        xky_port_type_e type, xky_name_ptr_t name,
        void *config, xky_identifier_t *pid);

/**
 * @brief Get port status
 * @param type Type of the port
 * @param pid Port identifier to get the status
 * @param status Pointer to store the port status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if configuration mismatch or port not created
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_port_status(
        xky_port_type_e type, xky_identifier_t pid, void *status);

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
xky_status_code_e xky_syscall_read_port(
        xky_port_type_e type, xky_identifier_t pid,
        xky_message_ptr_t msg, xky_sz_t *length, void *config);

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
xky_status_code_e xky_syscall_write_port(
        xky_port_type_e type, xky_identifier_t pid,
        xky_message_ptr_t msg, xky_sz_t length, void *config);

/**
 * @brief Get shared memory area
 * @param name of the shared memory area
 * @param shm shared memory area parameters
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if shared area not found
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_get_sharedmemory(
        xky_name_ptr_t name, xky_sharedmemory_t *shm);

/** @} */

/**
 * @defgroup xky_hm XKY - Health-Monitoring Services
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

    XKY_STATE_MODULE_INIT       = 0x00,     /**< Module Initialization      */
    XKY_STATE_MODULE_EXEC       = 0x01,     /**< Module Execution           */
    XKY_STATE_PARTITION_INIT    = 0x02,     /**< Partition Initialization   */
    XKY_STATE_PARTITION_EXEC    = 0x03,     /**< Partition Execution        */
    XKY_STATE_PARTITION_PROC    = 0x04,     /**< Partition Process          */
    XKY_STATE_PARTITION_HM      = 0x05,     /**< Partition Health-Monitor   */
    XKY_STATE_MODULE_HM         = 0x06,     /**< Module Heath-Monitor       */

} xky_state_e;

/**
 * @brief Errors enumeration
 */
typedef enum {

    XKY_POWER_ERROR             = 0x00,     /**< power error                */
    XKY_VIOLATION_ERROR         = 0x01,     /**< violation error            */
    XKY_SEGMENTATION_ERROR      = 0x02,     /**< segmentation error         */
    XKY_UNIMPLEMENTED_ERROR     = 0x03,     /**< unimplemented error        */
    XKY_FLOAT_ERROR             = 0x04,     /**< floating point error       */
    XKY_ARITHMETIC_ERROR        = 0x05,     /**< arithmetic error           */
    XKY_DIVISION_BY_0_ERROR     = 0x06,     /**< division by zero error     */
    XKY_IO_ERROR                = 0x07,     /**< I/O error                  */
    XKY_USR0_ERROR              = 0x08,     /**< user defined error 0       */
    XKY_USR1_ERROR              = 0x09,     /**< user defined error 1       */
    XKY_USR2_ERROR              = 0x0A,     /**< user defined error 2       */
    XKY_USR3_ERROR              = 0x0B,     /**< user defined error 3       */
    XKY_USR4_ERROR              = 0x0C,     /**< user defined error 4       */
    XKY_USR5_ERROR              = 0x0D,     /**< user defined error 5       */
    XKY_USR6_ERROR              = 0x0E,     /**< user defined error 6       */
    XKY_USR7_ERROR              = 0x0F,     /**< user defined error 7       */

} xky_error_e;

/**
 * @brief Health-Monitor event
 */
typedef struct {

    xky_state_e state_id;                   /**< State identifier           */
    xky_error_e error_id;                   /**< Error identifier           */

} xky_hm_event_t;

/**
 * @brief Get the current system state
 * @return Current system state
 */
xky_state_e xky_syscall_get_system_state(void);

/**
 * @brief Set the current system state
 * @param state State to be set
 * @return SUCCESS if the state transition is valid,
 *         NO_ACTION otherwise
 */
xky_status_code_e xky_syscall_set_system_state(xky_state_e state);

/**
 * @brief Get the lasted health-monitor event
 * @param event Pointer to store the HM event
 */
void xky_syscall_get_hm_event(xky_hm_event_t *event);

/**
 * @brief Raise an health-monitor event
 * @param error Error code for the HM event
 */
void xky_syscall_raise_hm_event(xky_error_e error);

/** @} */

/**
 * @defgroup xky_cache XKY - Cache Management Services
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

    XKY_CACHE_ALL         = 0x00,       /**< instruction and Data cache   */
    XKY_CACHE_DATA        = 0x01,       /**< data cache only              */
    XKY_CACHE_INSTRUCTION = 0x02,       /**< instruction cache only       */
    XKY_CACHE_NONE        = 0x03        /**< no cache                     */

} xky_cache_e;

/**
 * @brief Enable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */
xky_status_code_e xky_syscall_enable_cache(xky_cache_e type);

/**
 * @brief Disable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

xky_status_code_e xky_syscall_disable_cache(xky_cache_e type);
/**
 * @brief Freeze cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

xky_status_code_e xky_syscall_freeze_cache(xky_cache_e type);

/**
 * @brief Flush cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          PAL_INVALID_PARAM if invalid cache type
 */

xky_status_code_e xky_syscall_flush_cache(xky_cache_e type);

/** @} */

/**
 * @defgroup xky_multicore XKY - Multi-Core Services
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
xky_i32_t xky_syscall_get_irq(void);
/**
 * @brief Mask IRQ
 * @param irq Interrupt number
 */
void xky_syscall_mask_irq(xky_u32_t irq);
/**
 * @brief Unmask IRQ
 * @param irq Interrupt number
 */
void xky_syscall_unmask_irq(xky_u32_t irq);

/**
 * @brief Get the current core id
 * @return current core id
 */
xky_u32_t xky_syscall_get_core_id(void);
/**
 * @brief Boot a core at a given entry point
 * @param idx core index
 * @param entry_point core entry point
 * @return INVALID_CONFIG  - if the partition doesn't have access to core idx,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e xky_syscall_boot_core(xky_u32_t idx, void *entry_point);

/** @} */

/**
 * @defgroup xky_events XKY - Event Services
 * @brief Event Services definitions and system calls
 * @{
 */

#define XKY_EVENT_CLOCKTICK             0x00000001
#define XKY_EVENT_HEALTH_MONITOR        0x00000002
#define XKY_EVENT_SCHEDULE_CHANGE       0x00000004
#define XKY_EVENT_SCHEDULE_START        0x00000008
#define XKY_EVENT_PERIOD_START          0x00000010
#define XKY_EVENT_WINDOW_START          0x00000020
#define XKY_EVENT_PORT_UPDATED          0x00000040

/**
 * @brief Gets Events raised by the PMK
 * @return Events Mask
 * @note Reading the events will clear them
 */
xky_u32_t xky_syscall_get_event(void);

/** @} */


/**
 * @brief Prints a partition character
 * @param ch Character to be printed
 */
void xky_syscall_putchar(char ch);

/**
 * @brief Prints a partition buffer
 * @param buffer Buffer to be printed
 * @param len Length of the buffer
 * @return Number of characters printed
 */
xky_u32_t xky_syscall_print(char *str, xky_sz_t len);


/**
 * @brief Shutdown module
 */
void xky_syscall_shutdown_module(void);

/**
 * @}
 */

#ifdef  __cplusplus
}
#endif
#endif  /* !ASM */

#define XKY_SYSCALL_GET_IRQ                        (XKY_SYSCALL_ARCH_COUNT + 0 )
#define XKY_SYSCALL_MASK_IRQ                       (XKY_SYSCALL_ARCH_COUNT + 1 )
#define XKY_SYSCALL_UNMASK_IRQ                     (XKY_SYSCALL_ARCH_COUNT + 2 )
#define XKY_SYSCALL_GET_P_ADDR                     (XKY_SYSCALL_ARCH_COUNT + 3 )
#define XKY_SYSCALL_GET_CORE_ID                    (XKY_SYSCALL_ARCH_COUNT + 4 )
#define XKY_SYSCALL_BOOT_CORE                      (XKY_SYSCALL_ARCH_COUNT + 5 )
#define XKY_SYSCALL_GET_US_PER_TICK                (XKY_SYSCALL_ARCH_COUNT + 6 )
#define XKY_SYSCALL_GET_ELAPSED_TICKS              (XKY_SYSCALL_ARCH_COUNT + 7 )
#define XKY_SYSCALL_GET_PARTITION_ID               (XKY_SYSCALL_ARCH_COUNT + 8 )
#define XKY_SYSCALL_GET_PARTITION_STATUS           (XKY_SYSCALL_ARCH_COUNT + 9 )
#define XKY_SYSCALL_SET_PARTITION_MODE             (XKY_SYSCALL_ARCH_COUNT + 10)
#define XKY_SYSCALL_GET_SCHEDULE_ID                (XKY_SYSCALL_ARCH_COUNT + 11)
#define XKY_SYSCALL_GET_SCHEDULE_STATUS            (XKY_SYSCALL_ARCH_COUNT + 12)
#define XKY_SYSCALL_SET_SCHEDULE                   (XKY_SYSCALL_ARCH_COUNT + 13)
#define XKY_SYSCALL_GET_TIME_OF_DAY                (XKY_SYSCALL_ARCH_COUNT + 14)
#define XKY_SYSCALL_SET_TIME_OF_DAY                (XKY_SYSCALL_ARCH_COUNT + 15)
#define XKY_SYSCALL_GET_PORT_ID                    (XKY_SYSCALL_ARCH_COUNT + 16)
#define XKY_SYSCALL_GET_PORT_STATUS                (XKY_SYSCALL_ARCH_COUNT + 17)
#define XKY_SYSCALL_CREATE_PORT                    (XKY_SYSCALL_ARCH_COUNT + 18)
#define XKY_SYSCALL_READ_PORT                      (XKY_SYSCALL_ARCH_COUNT + 19)
#define XKY_SYSCALL_WRITE_PORT                     (XKY_SYSCALL_ARCH_COUNT + 20)
#define XKY_SYSCALL_GET_SHARED_AREA                (XKY_SYSCALL_ARCH_COUNT + 21)
#define XKY_SYSCALL_RESTART_MODULE                 (XKY_SYSCALL_ARCH_COUNT + 22)
#define XKY_SYSCALL_SHUTDOWN_MODULE                (XKY_SYSCALL_ARCH_COUNT + 23)
#define XKY_SYSCALL_GET_EVENT                      (XKY_SYSCALL_ARCH_COUNT + 24)
#define XKY_SYSCALL_GET_SYSTEM_STATE               (XKY_SYSCALL_ARCH_COUNT + 25)
#define XKY_SYSCALL_SET_SYSTEM_STATE               (XKY_SYSCALL_ARCH_COUNT + 26)
#define XKY_SYSCALL_GET_HM_EVENT                   (XKY_SYSCALL_ARCH_COUNT + 27)
#define XKY_SYSCALL_RAISE_HM_EVENT                 (XKY_SYSCALL_ARCH_COUNT + 28)
#define XKY_SYSCALL_PUTCHAR                        (XKY_SYSCALL_ARCH_COUNT + 29)
#define XKY_SYSCALL_PRINT                          (XKY_SYSCALL_ARCH_COUNT + 30)
#define XKY_SYSCALL_ENABLE_CACHE                   (XKY_SYSCALL_ARCH_COUNT + 31)
#define XKY_SYSCALL_DISABLE_CACHE                  (XKY_SYSCALL_ARCH_COUNT + 32)
#define XKY_SYSCALL_FREEZE_CACHE                   (XKY_SYSCALL_ARCH_COUNT + 33)
#define XKY_SYSCALL_FLUSH_CACHE                    (XKY_SYSCALL_ARCH_COUNT + 34)
#define XKY_SYSCALL_COUNT                          (XKY_SYSCALL_ARCH_COUNT + 35)

#endif /* __XKY_H__ */

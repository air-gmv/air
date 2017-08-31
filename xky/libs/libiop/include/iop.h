/** 
 * @file
 * 
 *  COPYRIGHT (c) 2011\n
 *  GMV-SKYSOFT \n
 * 
 * @author cdcs
 * @brief Defines the internal memory structures used by the IOP.
 */  


#ifndef __IOP_H__
#define __IOP_H__

#include <xky.h>
#include <rtems.h>
#include <iop_chain.h>
#include <iop_headers.h>

/**
 * @brief IOP Buffer size
 * This the maximum size of a packet of IO that can be transmitted via IOP
 * (including the overhead of any required header)
 */
#define IOP_BUFFER_SIZE                             (1520)

/**
 * @brief Convert to Network order
 * @param val Value do convert
 * @return Value in big endian notation
 */
#if CPU_BIG_ENDIAN
#define HTONS(val)      (val)
#else
#define HTONS(val)      ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))
#endif

/**
 * @brief
 */
typedef struct __iop_device_driver_t iop_device_driver_t;
/**
 * @brief
 */
typedef struct __iop_logical_device_t iop_logical_device_t;
/**
 * @brief
 */
typedef struct __iop_physical_device_t iop_physical_device_t;

/**
 * @brief IOP buffer
 */
typedef struct {

    void *v_addr;                   /**< virtual buffer address     */
    void *p_addr;                   /**< physical buffer address    */

    uint32_t header_off;
    uint32_t header_size;
    uint32_t payload_off;
    uint32_t payload_size;

    size_t size;

} iop_buffer_t;


typedef struct {

    iop_chain_node node;            /**< RTEMS Chain Node */

    uint32_t timer;                 /**< Number of periods after which this request will be discarded*/
    iop_buffer_t *buffer;           /**< reply header   */

} iop_wrapper_t;









/**
 * @brief List of elements
 */
typedef struct {

    uint32_t length;                    /**< length of the list             */
    void *elements;                     /**< elements in list               */

} iop_list_t;

/**
 * @brief Get an element from list
 * @param type type of element
 * @param list list of elements
 * @param index index of the element to get
 * @param pointer to the element
 */
#define iop_get_from_list(t, list, index)    ((t*)(list)->elements)[index]




/**
 * @brief Device Task
 * @param arg Input argument (device structure pointer)
 * @return return type of an RTEMS task
 */
typedef rtems_task (*iop_device_driver_task)(rtems_task_argument arg);


typedef void (*iop_header_prebuild_f)(iop_header_t *header);

typedef uint32_t (*iop_header_compare_f)(iop_wrapper_t *, iop_header_t *);

typedef void (*iop_header_copy_f)(
        iop_physical_device_t *, iop_wrapper_t *, iop_header_t *);

/**
 * @brief Device operation function
 * @param dev Device configuration structure pointer
 * @param arg Operation arguments pointer
 */
typedef uint32_t (*iop_device_operation)(iop_device_driver_t *dev, void *args);

/**
 * @brief Device configuration
 */
struct __iop_device_driver_t {

    /* driver configuration */
    void *driver;                   /**< driver pointer */

    /* device functions */
    iop_device_operation init;      /**< device initialization          */
    iop_device_operation open;      /**< device open                    */
    iop_device_operation read;      /**< device read                    */
    iop_device_operation write;     /**< device write                   */
    iop_device_operation close;     /**< device close                   */
};

/**
 * @brief Port configuration
 */
typedef struct {

    xky_port_type_e type;
    xky_identifier_t id;
    xky_name_t name;
    iop_logical_device_t *device;
    void *configuration;

} iop_port_t;

/**
 * @brief IOP main configuration
 */
typedef struct {

    /* storages */
    iop_wrapper_t *wrappers;
    iop_buffer_t *iop_buffers;
    uint8_t *iop_buffers_storage;
    uint32_t wrappers_count;

    /* devices and ports */
    iop_list_t remote_ports;
    iop_list_t request_ports;
    iop_list_t logical_devices;
    iop_list_t physical_devices;

    /* other */
    uint32_t time_to_live;

    /* requests chains */
    iop_chain_control free_wrappers;        /**< Chain of empty requests */

} iop_configuration_t;





/**
 * @brief Defines a physical device and its routing information
 *
 * A physical device is composed by the routes that are applicable to itself,
 * and by the chains of requests and replies. Data that is specific to a driver
 * is also be contained in this structure.
 */
struct __iop_physical_device_t {

    iop_device_driver_t *driver;            /**< device driver              */



    iop_list_t routes;                      /**< list of physical routes    */
    uint32_t *reads_per_period;


    iop_chain_control rcvqueue;             /**< type: service_reply_t      */
    iop_chain_control sendqueue;            /**< type: service_request_t    */

    /* device tasks */
    rtems_id reader_id;
    rtems_id writer_id;
    iop_device_driver_task reader_task;     /**< device reader task         */
    iop_device_driver_task writer_task;     /**< device writer task         */

    /*  */
    iop_header_prebuild_f header_prebuild;  /**< header pre-build           */
    iop_header_compare_f header_compare;
    iop_header_copy_f header_copy;
};

/**
 * @brief Defines a logical devices and their routing information
 *
 * A logical device is composed by the routes that are applicable to itself,
 * and by the chains of requests and replies.
 */
struct __iop_logical_device_t {

    iop_list_t routes;                  /**< list of logical routes     */
    iop_chain_control rcvqueue;         /**< type: service_reply_t      */
    iop_chain_control pending_rcvqueue;
    iop_chain_control sendqueue;        /**< type: service_request_t    */

};

/**
 * @brief Defines a logical route
 *
 * A logical route specifies how to transform from a logical device into a
 * physical one.
 */
typedef struct {

    uint32_t *schedule;
    iop_header_t *header;
    iop_physical_device_t *device;

} iop_logical_route_t;

/**
 * @brief Defines a physical route
 *
 * A physical route specifies how to route data received on a physical bus
 * to a logical device.
 */
typedef struct {

    uint32_t *schedule;
    iop_header_t *header;
    iop_port_t *port;

} iop_physical_route_t;



/**
 * @brief IOP user defined configuration
 *
 * This configuration is automatically generated the IOP configuration tool,
 * and linked against the IOP library to compile the IO Partition
 */
extern iop_configuration_t usr_configuration;

void iop_main_loop(void);

//#define DEBUG_SPWRTR
//#define DEBUG_SPACEWIRE_ONOFF
//#define IOP_NEEDS_DEBUG
#ifdef IOP_NEEDS_DEBUG
#include <pprintf.h>
#define iop_debug(fmt ...)              pprintf(fmt)
#else
#define iop_debug(fmt ...)
#endif

#endif  /* __IOP_H__ */

#include <iop.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[4];

/**
 * @brief Logical devices
 */
extern iop_logical_device_t logical_device_1;
extern iop_logical_device_t logical_device_2;

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_1;
extern iop_physical_device_t physical_device_0;

/**
 * @brief Physical devices list
 */
static iop_physical_device_t *physical_device_list[2] = \
{
    &physical_device_1,
    &physical_device_0,
};

/**
 * @brief Logical devices list
 */
static iop_logical_device_t *logical_device_list[2] = \
{
    &logical_device_1,
    &logical_device_2,
};

static iop_wrapper_t requests_storage[1024];

/**
 * @brief IOP buffers
 */
static iop_buffer_t iop_buffers[1024];
static uint8_t iop_buffers_storage[1024 * (109)];
static iop_fragment_t fragments[1024];

/**
 * @brief IOP application configurations
 */
iop_configuration_t usr_configuration = {

    .wrappers               = requests_storage,
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,
    .wrappers_count         = 1024,
    .fragments              = fragments,
    .fragment_count         = 1024,
    .time_to_live           = 5,

    .physical_devices       = {
        .length             = 2,
        .elements           = physical_device_list
     },
    .logical_devices       = {
        .length             = 2,
        .elements           = logical_device_list
     },
    .remote_ports           = {
        .length             = 4,
        .elements           = remote_ports
    }
};


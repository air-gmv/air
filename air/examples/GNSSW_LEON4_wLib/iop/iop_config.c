#include <iop.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[2];

/**
 * @brief Logical devices
 */
extern iop_logical_device_t logical_device_0;

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_0;

/**
 * @brief Physical devices list
 */
static iop_physical_device_t *physical_device_list[1] = \
{
    &physical_device_0,
};

/**
 * @brief Logical devices list
 */
static iop_logical_device_t *logical_device_list[1] = \
{
    &logical_device_0,
};

static iop_wrapper_t requests_storage[1024];

/**
 * @brief IOP buffers
 */
static iop_buffer_t iop_buffers[1024];
static uint8_t iop_buffers_storage[1024 * (60094 + 94)];
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

    .physical_devices       = {
        .length             = 1,
        .elements           = physical_device_list
     },
    .logical_devices       = {
        .length             = 1,
        .elements           = logical_device_list
     },
    .remote_ports           = {
        .length             = 2,
        .elements           = remote_ports
    }
};


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
extern iop_physical_device_t physical_device_1;
extern iop_physical_device_t physical_device_2;
extern iop_physical_device_t physical_device_3;
extern iop_physical_device_t physical_device_4;

/**
 * @brief Physical devices list
 */
static iop_physical_device_t *physical_device_list[5] = \
{
    &physical_device_0,
    &physical_device_1,
    &physical_device_2,
    &physical_device_3,
    &physical_device_4,
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
static uint8_t iop_buffers_storage[1024 * IOP_BUFFER_SIZE];


/**
 * @brief IOP application configurations
 */
iop_configuration_t usr_configuration = {

    .wrappers               = requests_storage,
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,
    .wrappers_count           = 1024,

    .physical_devices       = {
        .length             = 5,
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


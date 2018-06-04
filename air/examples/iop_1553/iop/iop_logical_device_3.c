
#include <iop.h>

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_0;


/**
 * @brief Routes Headers
 */
static iop_header_t route_header[1] = \
{
    {
        // RCS
        .milstd_hdr = {
          .desc = 4,
          .address = 2,
        }
    }
};

/**
 * @brief Routes Scheduling
 */
static uint32_t route_schedule_3[1] = \
    { 1 };


/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[1] = \
{
    {
        .schedule   = route_schedule_3,
        .header     = &route_header[0],
        .device     = &physical_device_0
    }
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_3 = \
{
    .routes = {
        .length     = 1,
        .elements   = logical_routes
    },
};


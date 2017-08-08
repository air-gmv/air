
#include <iop.h>

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_0;
extern iop_physical_device_t physical_device_1;
extern iop_physical_device_t physical_device_2;

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[2] = \
{
    {
        .spw_header = {
            .hdr         = { 0x00,0x01,0x0A }
        }
    },
    {
        .spw_header = {
            .hdr         = {  }
        }
    }
};

/**
 * @brief Routes Scheduling
 */
static uint32_t route_schedule_0[1] = \
    { 1 };
static uint32_t route_schedule_1[1] = \
    { 0 };



/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[2] = \
{
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[0],
        .device     = &physical_device_1
    },
    {
        .schedule   = route_schedule_1,
        .header     = &route_header[1],
        .device     = &physical_device_2
    }
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_0 = \
{
    .routes = {
        .length     = 2,
        .elements   = logical_routes
    },
};


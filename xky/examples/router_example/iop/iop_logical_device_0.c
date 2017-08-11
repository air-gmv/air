
#include <iop.h>

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_0;
extern iop_physical_device_t physical_device_1;
extern iop_physical_device_t physical_device_2;
extern iop_physical_device_t physical_device_3;
extern iop_physical_device_t physical_device_4;

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[4] = \
{
    {
        .spw_header = {
            .hdr         = { 0x01,0x0A }
        }
    },
    {
        .spw_header = {
            .hdr         = {  }
        }
    },
    {
        .spw_header = {
            .hdr         = {  }
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
static uint32_t route_schedule_2[1] = \
    { 0 };
static uint32_t route_schedule_3[1] = \
    { 0 };



/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[4] = \
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
    },
    {
        .schedule   = route_schedule_2,
        .header     = &route_header[2],
        .device     = &physical_device_3
    },
    {
        .schedule   = route_schedule_3,
        .header     = &route_header[3],
        .device     = &physical_device_4
    }
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_0 = \
{
    .routes = {
        .length     = 4,
        .elements   = logical_routes
    },
};



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
        .eth_header = {
            .dst_ip      = { 172,19,13,68 },
            .dst_mac     = { 0x10,0x62,0xe5,0x4f,0xd8,0xd2},
            .dst_port    = HTONS(3334),
            .src_port    = HTONS(3334)
        }
    }
};

/**
 * @brief Routes Scheduling
 */
static uint32_t route_schedule_0[1] = \
    { 1 };



/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[1] = \
{
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[0],
        .device     = &physical_device_0
    }
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_0 = \
{
    .routes = {
        .length     = 1,
        .elements   = logical_routes
    },
};


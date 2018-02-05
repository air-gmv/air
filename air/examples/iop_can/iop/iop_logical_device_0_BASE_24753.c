
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
            .dst_ip      = { 192,168,0,3},
            .dst_mac     = { 0x00,0x50,0xbf,0x50,0x07,0x0d},
            .dst_port    = HTONS(13000),
            .src_port    = HTONS(13000)
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


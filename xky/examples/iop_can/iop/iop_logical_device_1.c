
#include <iop.h>

/**
 * @brief Physical devices
 */
extern iop_physical_device_t physical_device_0;

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[0] = \
{
};

/**
 * @brief Routes Scheduling
 */



/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[0] = \
{
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_1 = \
{
    .routes = {
        .length     = 0,
        .elements   = logical_routes
    },
};


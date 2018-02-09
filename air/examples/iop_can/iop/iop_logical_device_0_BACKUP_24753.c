
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
<<<<<<< HEAD:air/examples/iop_example/iop/iop_logical_device_0.c
    {
        .eth_header = {
            .dst_ip      = { 192,168,0,2},
            .dst_mac     = { 0x68,0x05,0xca,0x1f,0x39,0xaf},
            .dst_port    = HTONS(13000),
            .src_port    = HTONS(13000)
        }
    }
=======
	{
		.can_header = {
			.extended 	= 0,
			.rtr 		= 0,
			.sshot 		= 0,
			.id  		= 2
		}
	}
>>>>>>> fb18a89153... Changes to try solve the makefile.inc problem. rtems48i is being:air/examples/iop_can/iop/iop_logical_device_0.c
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
		.schedule = route_schedule_0,
		.header = &route_header[0],
		.device = &physical_device_0
	}
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_0= \
{
    .routes = {
        .length     = 1,
        .elements   = logical_routes
    },
};


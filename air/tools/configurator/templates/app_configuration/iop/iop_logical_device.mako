<%namespace name="iop_template" file="/iop_templates.mako"/>\

#include <iop.h>

${iop_template.PhysicalDevicesList(iop_configuration)}\

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[${len(device.routes)}] = ${'\\'}
{
% for i, route in enumerate(device.routes):
	% if route.device.type == 'ETH':
${iop_template.EthHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% elif route.device.type == 'SPW':
${iop_template.SpwHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% elif route.device.type == 'CAN':
${iop_template.CanHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% elif route.device.type == 'MIL':
${iop_template.MILHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% elif route.device.type == 'SD':
${iop_template.SdHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% elif route.device.type == 'ADC':
${iop_template.AdcHeader(route.header)}${',' if i < len(device.routes) - 1 else ''}
	% endif
% endfor
};

/**
 * @brief Routes Scheduling
 */
% for i, route in enumerate(device.routes):
static uint32_t route_schedule_${i}[${len(iop_configuration.schedules)}] = ${'\\'}
<%
    active = []
    for schedule in iop_configuration.schedules:
        active.append('1') if route in schedule.routes else active.append('0')
%>\
    { ${', '.join(active)} };
% endfor



/**
 * @brief Routes Configuration
 */
static iop_logical_route_t logical_routes[${len(device.routes)}] = ${'\\'}
{
% for i, route in enumerate(device.routes):
    {
        .schedule   = route_schedule_${i},
        .header     = &route_header[${i}],
        .device     = &physical_device_${route.device.id}
    }${',' if i < len(device.routes) - 1 else ''}
% endfor
};

/**
 * @brief Logical Device configuration
 */
iop_logical_device_t logical_device_${device.id} = ${'\\'}
{
    .routes = {
        .length     = ${len(device.routes)},
        .elements   = logical_routes
    },
};


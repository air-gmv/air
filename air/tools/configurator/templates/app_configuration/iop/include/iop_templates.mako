
<%def name="PhysicalDevice(iop_configuration, pdevice, funcs)">\
/**
 * @brief Device Scheduling
 */
static uint32_t reads_per_period[] = ${'\\'}
    { \
% for i, schedule in enumerate(iop_configuration.schedules):
<%
    ds = [ ds for ds in schedule.devices if ds.device == pdevice ]
%>\
${ds[0].reads if len(ds) > 0 else '0'}${',' if i < len(iop_configuration.schedules) - 1 else ''}\
% endfor
 };

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[${len(pdevice.routes)}] = ${'\\'}
{
% if pdevice.type == 'ETH':
	% for i, route in enumerate(pdevice.routes):
${EthHeader(route.header)}${',' if i < len(pdevice.routes) - 1 else ''}
	% endfor
% elif pdevice.type == 'SPW':
	% for i, route in enumerate(pdevice.routes):
${SpwHeader(route.header)}${',' if i < len(pdevice.routes) - 1 else ''}
	% endfor
% endif
};

/**
 * @brief Routes Scheduling
 */
% for i, route in enumerate(pdevice.routes):
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
static iop_physical_route_t physical_routes[${len(pdevice.routes)}] =${'\\'}
{
% for i, route in enumerate(pdevice.routes):
    {
        .schedule   = route_schedule_${i},
        .header     = &route_header[${i}],
        .port       = &remote_ports[${route.port.iop_index}]
    }${',' if i < len(pdevice.routes) - 1 else ''}
% endfor
};

/**
 * @brief Pysical Device configuration
 */
iop_physical_device_t physical_device_${pdevice.idx} =${'\\'}
{
    .driver             = (iop_device_driver_t *)&device_configuration,
    .routes             = {
        .length         = ${len(pdevice.routes)},
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = ${funcs['reader_task']},
    .writer_task        = ${funcs['writer_task']},
    .header_prebuild    = ${funcs['header_prebuild']},
    .header_compare     = ${funcs['header_compare']},
    .header_copy        = ${funcs['header_copy']},
};
</%def>

<%def name="PhysicalDevicesList(iop_configuration)">\
/**
 * @brief Physical devices
 */
% for i, device in enumerate(iop_configuration.physical_devices):
extern iop_physical_device_t physical_device_${device.idx};
% endfor
</%def>

<%def name="LogicalDevicesList(iop_configuration)">\
/**
 * @brief Logical devices
 */
% for i, device in enumerate(iop_configuration.logical_devices):
extern iop_logical_device_t logical_device_${device.idx};
% endfor
</%def>

<%def name="RemotePortList(iop_configuration)">\
/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[${len(iop_configuration.ports)}];
</%def>

<%def name="EthHeader(header)">\
    {
        .eth_header = {
            .dst_ip      = { ${','.join(header.ip)} },
            .dst_mac     = { ${','.join(['0x{0}'.format(o) for o in header.mac ])}},
            .dst_port    = HTONS(${header.port}),
            .src_port    = HTONS(${header.port})
        }
    }\
</%def>

<%def name="SpwHeader(header)">\
    {
        .spw_header = {
            .hdr         = { ${','.join(header.address)} }
        }
    }\
</%def>

<%def name="IopBuffersStorage(size)">\
/**
 * @brief IOP buffers
 */
static iop_buffer_t iop_buffers[${size}];
static uint8_t iop_buffers_storage[${size} * IOP_BUFFER_SIZE];
</%def>
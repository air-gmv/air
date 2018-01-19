<%




%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
%if header:
#include <iop.h>


%else:
#include <iop.h>

${iop_template.RemotePortList(iop_configuration)}\

${iop_template.LogicalDevicesList(iop_configuration)}\

${iop_template.PhysicalDevicesList(iop_configuration)}\

/**
 * @brief Physical devices list
 */
static iop_physical_device_t *physical_device_list[${len(iop_configuration.physical_devices)}] = ${'\\'}
{
% for i, device in enumerate(iop_configuration.physical_devices):
    &physical_device_${device.idx},
% endfor
};

/**
 * @brief Logical devices list
 */
static iop_logical_device_t *logical_device_list[${len(iop_configuration.logical_devices)}] = ${'\\'}
{
% for i, device in enumerate(iop_configuration.logical_devices):
    &logical_device_${device.idx},
% endfor
};

static iop_wrapper_t requests_storage[${iop_configuration.requests}];

${iop_template.IopBuffersStorage(iop_configuration.requests)}\


/**
 * @brief IOP application configurations
 */
iop_configuration_t usr_configuration = {

    .wrappers               = requests_storage,
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,
    .wrappers_count           = ${iop_configuration.requests},

    .physical_devices       = {
        .length             = ${len(iop_configuration.physical_devices)},
        .elements           = physical_device_list
     },
    .logical_devices       = {
        .length             = ${len(iop_configuration.logical_devices)},
        .elements           = logical_device_list
     },
    .remote_ports           = {
        .length             = ${len(iop_configuration.ports)},
        .elements           = remote_ports
    }
};
%endif

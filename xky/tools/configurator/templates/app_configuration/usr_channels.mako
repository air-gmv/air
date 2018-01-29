<%
    import air
    import utils.templates as makoutils
    import parsers.a653.definitions as a653_definitions

    ## convert nanoseconds to ticks
    def nsec_to_ticks(value):
        return int((value / a653_definitions.A653_TIMESCALE) * configuration.ticks_per_second)
%>\
${makoutils.cfileHeader('usr_channels.c', 'Channels configuration')}

#include <ports.h>
#include <ports_queuing.h>
#include <ports_sampling.h>
#include <usr_config.h>

%if any([len(partition.ports) > 0 for partition in configuration.partitions]):
/**
 * @brief Ports
 */
static pmk_port_t usr_ports[] = ${'\\'}
{<% port_idx = 0 %>
% for i, partition in enumerate(configuration.partitions):
% for j, port in enumerate(partition.ports):
    {   /* ${port} */
        .id         = ${j},
        .name       = "${port.name}",
        .direction  = AIR_${port.direction}_PORT,
    }, <% port.index = port_idx; port_idx += 1  %>
% endfor
% endfor
};
%endif
% for i, channel in enumerate(configuration.channels):

/**
 * @brief Channel ${i} configuration
 */
% if   channel.type == a653_definitions.QUEUING_CHANNEL:
static pmk_queuing_config_t usr_channel${i}_configuration = ${'\\'}
{
    .max_message_size   = ${channel.src[0].max_message_size},
    .max_nb_message     = ${channel.src[0].max_nb_message},
    .discipline         = 0,
};
% elif channel.type == a653_definitions.SAMPLING_CHANNEL:
static pmk_sampling_config_t usr_channel${i}_configuration = ${'\\'}
{
    .max_message_size   = ${channel.src[0].max_message_size},
    .refresh_period     = ${nsec_to_ticks(channel.src[0].refresh_period)},
};
% endif
% endfor
% for i, partition in enumerate(configuration.partitions):

/**
 * @brief Partition ${i} Port Table
 */
pmk_port_t *usr_partition${i}_ports[${len(partition.ports)}] = ${'\\'}
{
% for j, port in enumerate(partition.ports):
    &usr_ports[${port.index}],
% endfor
};
% endfor
% for i, channel in enumerate(configuration.channels):

/**
 * @brief Channel ${i} source ports
 */
pmk_port_t *usr_channel${i}_source_ports[${len(channel.src)}] = ${'\\'}
{
% for j, port in enumerate(channel.src):
    &usr_ports[${port.index}],
% endfor
};

/**
 * @brief Channel ${i} destination ports
 */
pmk_port_t *usr_channel${i}_destination_ports[${len(channel.dst)}] = ${'\\'}
{
% for j, port in enumerate(channel.dst):
    &usr_ports[${port.index}],
% endfor
};
% endfor

/**
 * @brief Module Communication Channels table
 */
pmk_channel_t usr_channels_table[USR_CHANNELS] = ${'\\'}
{
%for i, channel in enumerate(configuration.channels):
    {   /* ${channel} */
        .id             = ${channel.id},
        .name           = "${channel.name}",
        .type           = AIR_${channel.type},
        .src            = ${'\\'}
        {
            .length     =  ${len(channel.src)},
            .elements   = (void *)usr_channel${i}_source_ports
        },
        .dst            = ${'\\'}
        {
            .length     =  ${len(channel.dst)},
            .elements   = (void *)usr_channel${i}_destination_ports
        },
        .configuration  = (void *)&usr_channel${i}_configuration,
    }${"," if i < len(configuration.channels) - 1 else ""}
%endfor
};

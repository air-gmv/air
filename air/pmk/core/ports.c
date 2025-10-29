/*
 * Copyright (C) 2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief
 */

#include <pmk.h>
#include <ports.h>
#include <error.h>
#include <printk.h>
#include <workspace.h>
#include <segregation.h>
#include <ports_queuing.h>
#include <ports_sampling.h>
#include <configurations.h>

void pmk_channels_init(void)
{

#ifdef PMK_DEBUG
    printk(" :: Communication Channels initialization\n");
#endif

    /* get list of configured communication channels */
    pmk_list_t *list = pmk_get_usr_channels();

    /* initialize every channel */
    int i, j;
    for (i = 0; i < list->length; ++i)
    {

        pmk_channel_t *channel = pmk_get_from_list(pmk_channel_t, list, i);
        /* configure source ports */
        for (j = 0; j < channel->src.length; ++j)
        {

            /* get source port, and link it to the channel */
            pmk_port_t *port = ((pmk_port_t **)channel->src.elements)[j];
            atomic_set(AIR_EMPTY_PORT, &port->updated);
            port->channel = channel;
        }

        /* configure destination ports */
        for (j = 0; j < channel->dst.length; ++j)
        {

            /* get destination port, and link it to the channel */
            pmk_port_t *port = ((pmk_port_t **)channel->dst.elements)[j];
            atomic_set(AIR_EMPTY_PORT, &port->updated);
            port->channel = channel;
        }

        /* configure the channel */
        switch (channel->type)
        {

        /* queuing channel initialization */
        case AIR_QUEUING_PORT:
            pmk_queuing_channel_init(channel);
            break;

        /* sampling channel initialization */
        case AIR_SAMPLING_PORT:
            pmk_sampling_channel_init(channel);
            break;

        /* configuration error */
        default:
            pmk_fatal_error(PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
            break;
        }
    }
#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif
    return;
}

void pmk_partition_ports_init(pmk_partition_t *partition)
{

#ifdef PMK_DEBUG
    printk(" :: The pmk_partition_ports_init is taking place.\n Number of ports for this parition: %d \n",
           partition->ports.length);
#endif
    int i;
    for (i = 0; i < partition->ports.length; ++i)
    {

        /* get port */
        pmk_port_t *port = ((pmk_port_t **)partition->ports.elements)[i];
#ifdef PMK_DEBUG
        printk(" :: The pmk_partition_ports_%s\n", port->name);
#endif

        /* link port to partition */
        port->partition = partition;

        /* flag that port isn't created if state is COLD_START */
        if (AIR_MODE_COLD_START == partition->mode)
        {
            atomic_set(0, &port->created);
        }
    }
    return;
}

pmk_port_t *pmk_port_get_from_partition_by_id(pmk_partition_t *partition, air_identifier_t id, air_port_type_e type)
{

    /* port doesn't belong to the partition */
    if (id < 0 || id >= partition->ports.length)
    {
        return NULL;
    }

    /* check if port is from the correct type */
    pmk_port_t *port = ((pmk_port_t **)partition->ports.elements)[id];
    if ((port->channel->type & type) == 0)
    {
        return NULL;
    }

    return port;
}

void pmk_channel_update_ports(pmk_channel_t *channel, air_port_updated_e state)
{

    int i;
    for (i = 0; i < channel->dst.length; ++i)
    {

        /* get destination port, and link it to the channel */
        pmk_port_t *port = ((pmk_port_t **)channel->dst.elements)[i];
        atomic_set(state, &port->updated); // como e atomic pode estar outra operacao atomica a bloquear na
                                           //  na leitura

        /** @todo each partition currently running must received an
         *        port updated interrupt
         *
         *
         */
    }
    return;
}

pmk_port_t *pmk_port_get_from_partition_by_name(pmk_partition_t *partition, air_name_ptr_t name)
{

    int i;
    pmk_port_t *found = NULL;

    /* iterate through the partition ports */
    for (i = 0; i < partition->ports.length; ++i)
    {

        /* get port */
        pmk_port_t *port = ((pmk_port_t **)partition->ports.elements)[i];
        if (strncmp(name, port->name, sizeof(air_name_t)) == 0)
        {
            found = port;
        }
    }

    /* no errors detected */
    return found;
}

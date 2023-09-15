/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Queuing Ports functions
 */

#include <multicore.h>
#include <pmk.h>
#include <ports.h>
#include <ports_queuing.h>
#include <printk.h>
#include <segregation.h>
#include <workspace.h>

void pmk_queuing_channel_init(pmk_channel_t *channel)
{

    air_u32_t i;

    /* get channel configuration */
    pmk_queuing_config_t *config = (pmk_queuing_config_t *)channel->configuration;

#ifdef PMK_DEBUG
    printk("    queuing channel %02i %s\n", channel->id, channel->name);
#endif

    /* allocate space for the queue */
    config->queue = (pmk_message_slot_t *)pmk_workspace_alloc(sizeof(pmk_message_slot_t) * config->max_nb_message);

    /* initialize each message slot */
    for (i = 0; i < config->max_nb_message; ++i)
    {
        config->queue[i].length = 0;
        config->queue[i].timestamp = 0;
        config->queue[i].message = (air_message_ptr_t)pmk_workspace_alloc(config->max_message_size);
#ifdef PMK_DEBUG
        printk("    :: queue %03i at 0x%p, msg: 0x%p\n", i, &config->queue[i], config->queue[i].message);
#endif
    }

    /* initialize queue */
    config->last = config->max_nb_message - 1;
    atomic_set(0, &config->reading);
    atomic_set(0, &config->writing);
    atomic_set(0, &config->count);
    config->first = 0;
    return;
}

air_u32_t pmk_queuing_port_check_configuration(pmk_port_t *port, air_queuing_port_configuration_t *usr_config)
{

    /* check if the parameters are valid */
    pmk_queuing_config_t *port_config = (pmk_queuing_config_t *)port->channel->configuration;
    if (port_config->max_message_size != usr_config->max_message_size ||
        port_config->max_nb_message != usr_config->max_nb_message ||
        port_config->discipline != usr_config->port_discipline || port->direction != usr_config->port_direction)
    {

        return 1;
    }

    /* all configurations checked out */
    return 0;
}

air_status_code_e pmk_queuing_port_get_status(core_context_t *context, pmk_port_t *port,
                                              air_queuing_port_status_t *status)
{

    /* get channel configuration */
    pmk_queuing_config_t *config = (pmk_queuing_config_t *)port->channel->configuration;

    /* get port status */
    air_queuing_port_status_t l_status;
    l_status.nb_message = atomic_fetch(&config->count);
    l_status.max_nb_message = config->max_nb_message;
    l_status.max_message_size = config->max_message_size;
    l_status.port_direction = port->direction;

    /* copy status to partition */
    if (pmk_segregation_put_user(context, l_status, status) != 0)
    {
        return AIR_INVALID_POINTER;
    }

    return AIR_NO_ERROR;
}

air_status_code_e pmk_queuing_port_read(core_context_t *context, pmk_port_t *port, air_message_ptr_t msg,
                                        air_sz_t *length, air_queuing_port_status_t *status)
{

    /* get channel configuration */
    pmk_queuing_config_t *config = (pmk_queuing_config_t *)port->channel->configuration;

    /* check if no one else is reading the channel */
    if (atomic_swap(1, &config->reading) == 1)
    {

        return AIR_NOT_AVAILABLE;
    }

    /* check if a message is available for read */
    if (atomic_fetch(&config->count) == 0)
    {

        atomic_set(0, &config->reading);
        return AIR_NOT_AVAILABLE;
    }

    /* get current message slot */
    pmk_message_slot_t *slot = &config->queue[config->first];

    /* copy message to partition space */
    if (((pmk_segregation_copy_to_user(context, msg, slot->message, slot->length) != 0) != 0) != 0 ||
        pmk_segregation_put_user(context, slot->length, length) != 0)
    {

        atomic_set(0, &config->reading);
        return AIR_INVALID_POINTER;
    }

    /* remove message from queue (protected by reading lock) */
    config->first = (config->first + 1) % config->max_nb_message;
    atomic_dec(&config->count);

    /* read terminated */
    atomic_set(0, &config->reading);

    /* set message validity */
    port->last_msg_validity = AIR_MESSAGE_VALID;

    /* check if an overflow occurred */
    if (atomic_swap(0, &config->overflowed) == 1)
    {
        return AIR_INVALID_CONFIG;
    }
    return AIR_NO_ERROR;
}

air_status_code_e pmk_queuing_port_write(core_context_t *context, pmk_port_t *port, air_message_ptr_t msg,
                                         air_sz_t length, air_queuing_port_status_t *status)
{

    /* get channel configuration */
    pmk_queuing_config_t *config = (pmk_queuing_config_t *)port->channel->configuration;

    /* check if length is valid */
    if ((length > config->max_message_size) != 0)
    {

        return AIR_INVALID_CONFIG;
    }

    /* check if no one else is writing in the queue */
    if (atomic_swap(1, &config->writing) == 1)
    {

        return AIR_NOT_AVAILABLE;
    }

    /* check if the queue is full */
    if (atomic_fetch(&config->count) >= config->max_nb_message)
    {

        /* overflow in queue */
        atomic_set(1, &config->overflowed);
        atomic_set(0, &config->writing);
        return AIR_NOT_AVAILABLE;
    }

    /* add message from slot */
    air_u32_t last = (config->last + 1) % config->max_nb_message;
    pmk_message_slot_t *slot = &config->queue[last];

    /* copy message from partition space */
    if (pmk_segregation_copy_from_user(context, slot->message, msg, length) != 0)
    {

        atomic_set(0, &config->writing);
        return AIR_INVALID_POINTER;
    }
    slot->timestamp = air_shared_area.schedule_ctrl->total_ticks;
    slot->length = length;

    /* add message to queue */
    atomic_inc(&config->count);
    config->last = last;

    /* finish writing operation */
    atomic_set(0, &config->writing);

    /* update all destination ports on the channel */
    pmk_channel_update_ports(port->channel, AIR_NEW_MESSAGE);
    return AIR_NO_ERROR;
}

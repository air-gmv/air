/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file ports_sampling.c
 * @author pfnf
 * @brief Sampling Ports functions
 */

#include <bsp.h>
#include <pmk.h>
#include <ports.h>
#include <printk.h>
#include <workspace.h>
#include <multicore.h>
#include <segregation.h>
#include <ports_sampling.h>

/**
 * @brief Sampling Channel buffer size
 */
#define BUFFER_SIZE                                 (2)

void pmk_sampling_channel_init(pmk_channel_t *channel) {

    int i;

    /* get sampling channel configuration */
    pmk_sampling_config_t *config =
            (pmk_sampling_config_t *)channel->configuration;

#ifdef PMK_DEBUG
    printk("    sampling channel %02i %s\n", channel->id, channel->name);
#endif

    /* allocate space for the buffer */
    config->buffer = (pmk_message_slot_t *)pmk_workspace_alloc(
            sizeof(pmk_message_slot_t) * BUFFER_SIZE);

    /* initialize each message slot */
    for (i = 0; i < BUFFER_SIZE; ++i) {
        config->buffer[i].length = 0;
        config->buffer[i].timestamp = 0;
        config->buffer[i].message = (air_message_ptr_t)
                pmk_workspace_alloc(config->max_message_size);
#ifdef PMK_DEBUG
        printk("    :: buffer %03i at 0x%p, msg: 0x%p\n",
                i, &config->buffer[i], config->buffer[i].message);
#endif
    }

    /* setup the first slot */
    atomic_set(0, &config->current_slot);
    return;
}

air_u32_t pmk_sampling_port_check_configuration(
        pmk_port_t *port, air_sampling_port_status_t *usr_config) {

    air_u32_t rc = 0;

    /* check if the parameters are valid */
    pmk_sampling_config_t *port_config =
            (pmk_sampling_config_t *)port->channel->configuration;
    if (port_config->max_message_size != usr_config->max_message_size ||
        port_config->refresh_period != usr_config->refresh_period ||
        port->direction != usr_config->port_direction ) {

        rc = 1;
    }

    return rc;
}

air_status_code_e pmk_sampling_port_get_status(
        core_context_t *context,
        pmk_port_t *port, air_sampling_port_status_t *status) {

    /* get channel configuration */
    pmk_sampling_config_t *config =
            (pmk_sampling_config_t *)port->channel->configuration;

    /* get current message slot */
    air_u32_t idx = atomic_fetch(&config->current_slot);
    pmk_message_slot_t *slot = &config->buffer[idx];

    /* copy status configuration from partition */
    air_sampling_port_status_t l_status;
    l_status.refresh_period = config->refresh_period;
    l_status.max_message_size = config->max_message_size;
    l_status.port_direction = port->direction;
    l_status.time_stamp = slot->timestamp;
    l_status.updated = atomic_fetch(&port->updated);
    l_status.last_msg_validity = port->last_msg_validity;
    if ((air_shared_area.schedule_ctrl->total_ticks - l_status.time_stamp) >
        l_status.refresh_period) {
        l_status.message_age = AIR_MESSAGE_STALE;
    } else {
        l_status.message_age = AIR_MESSAGE_FRESH;
    }

    /* copy status to partition */
    if (pmk_segregation_put_user(context, l_status, status) != 0) {
        return AIR_INVALID_POINTER;
    }

    return AIR_NO_ERROR;
}

air_status_code_e pmk_sampling_port_read(
        core_context_t *context,
        pmk_port_t *port, air_message_ptr_t msg,
        air_sz_t *length, air_sampling_port_status_t *status) {

    /* get sampling channel configuration */
    pmk_sampling_config_t *config =
            (pmk_sampling_config_t *)port->channel->configuration;

    /* copy status to local memory */
    air_sampling_port_status_t l_status;
    if (pmk_segregation_get_user(context, l_status, status) != 0) {
        return AIR_INVALID_POINTER;
    }

    /* check if port is being written */
    if (atomic_fetch(&config->writing))
        return AIR_NOT_AVAILABLE;

    /* get current slot */
    air_u32_t slot_idx = atomic_fetch(&config->current_slot);
    pmk_message_slot_t *slot = &config->buffer[slot_idx];

    /* get the output parameters */
    air_message_validity_e validity = AIR_MESSAGE_INVALID;
    air_port_updated_e updated = atomic_fetch(&port->updated);
    air_clocktick_t time_stamp = 0;

    /* check if the port contains messages */
    air_status_code_e rc = AIR_NO_ACTION;
    
	 if (updated != AIR_EMPTY_PORT) {

        /* set time stamp */
        time_stamp = slot->timestamp;

        /* set message validity */
          if ((air_shared_area.schedule_ctrl->total_ticks - slot->timestamp) >
              config->refresh_period) {
              validity = AIR_MESSAGE_INVALID;
          } else {
              validity = AIR_MESSAGE_VALID;
          }

        /* perform the correct operation */
        switch (l_status.operation) {

            /* regular mode */
            case AIR_SAMPLING_MODE_REGULAR:
                rc = AIR_NO_ERROR;
                break;

            /* updated mode */
            case AIR_SAMPLING_MODE_UPDATED:
                if (updated == AIR_NEW_MESSAGE) {
                    rc = AIR_NO_ERROR;
                }
                break;

            /* conditional mode */
            case AIR_SAMPLING_MODE_CONDITIONAL:
                if (slot->timestamp > l_status.time_stamp) {
                    rc = AIR_NO_ERROR;
                }
                break;

            /* configuration error */
            default:
                return AIR_INVALID_CONFIG;
        }
    }

    /* check if the operation can proceed*/
	 
    if (rc == AIR_NO_ERROR) {

        /* copy message to partition space */
        if (pmk_segregation_copy_to_user(context, msg, slot->message, slot->length) != 0 ||
            pmk_segregation_put_user(context, slot->length, length)) {
            return AIR_INVALID_POINTER;
        }

    /* error */
    } else {

        air_u32_t zero = 0;
        if (pmk_segregation_put_user(context, zero, length) != 0) {
            return AIR_INVALID_POINTER;
        }
    }

    /* copy output back to partition space */
    l_status.last_msg_validity = validity;
    l_status.time_stamp = time_stamp;
    l_status.updated = updated;
    if (pmk_segregation_put_user(context, l_status, status) != 0) {
        return AIR_INVALID_POINTER;
    }

    /* clear updated flag */
    atomic_set(0, &port->updated);

    /* save message validity */
    port->last_msg_validity = validity;
    return rc;
}

air_status_code_e pmk_sampling_port_write(
        core_context_t *context,
        pmk_port_t *port,air_message_ptr_t msg,
        air_sz_t length, air_sampling_port_status_t *status) {

    /* get sampling channel configuration */
    pmk_sampling_config_t *config =
            (pmk_sampling_config_t *)port->channel->configuration;

    /* check if length is valid */
    if (length > config->max_message_size) {

        return AIR_INVALID_CONFIG;
    }

    /* check if no one else is writing in the port */
    if (atomic_swap(1, &config->writing) == 1) {

        return AIR_NOT_AVAILABLE;
    }

    /* get current slot */
    air_u32_t slot_idx = (atomic_fetch(&config->current_slot) + 1) % BUFFER_SIZE;
    pmk_message_slot_t *slot = &config->buffer[slot_idx];

    /* copy message from partition space */
    if (pmk_segregation_copy_from_user(context, slot->message, msg, length) != 0) {
        atomic_set(0, &config->writing);
        return AIR_INVALID_POINTER;
    }

    slot->timestamp = air_shared_area.schedule_ctrl->total_ticks;
    slot->length = length;

    /* store new slot index */
    atomic_set(slot_idx, &config->current_slot);

    /* update all destination ports on the channel */
    pmk_channel_update_ports(port->channel, AIR_NEW_MESSAGE);

    /* finish writing operation */
    atomic_set(0, &config->writing);

    return AIR_NO_ERROR;
}

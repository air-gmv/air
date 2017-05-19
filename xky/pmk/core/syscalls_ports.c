/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file syscalls_ports.c
 * @author pfnf
 * @brief System calls related to ports
 */

#include <pmk.h>
#include <ports.h>
#include <workspace.h>
#include <multicore.h>
#include <segregation.h>
#include <ports_queuing.h>
#include <ports_sampling.h>
#include <configurations.h>

xky_status_code_e pmk_syscall_get_port_id(
    pmk_core_ctrl_t *core,
    xky_port_type_e type,
    xky_name_ptr_t name,
    xky_identifier_t *pid) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* copy name to local memory */
    xky_name_t l_name;
    if (pmk_segregation_copy_from_user(
            context, l_name, name, sizeof(xky_name_t)) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* get port pointer and check if its valid */
    pmk_port_t *port = pmk_port_get_from_partition_by_name(partition, l_name);
    if (port == NULL || (port->channel->type & type) == 0 ||
        atomic_fetch(&port->created) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_CONFIG;
    }

    /* pass id to partition */
    if (pmk_segregation_put_user(context, port->id, pid) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

xky_status_code_e pmk_syscall_create_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_name_ptr_t name,
        void *config,
        xky_identifier_t *pid) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* copy name to local memory */
    xky_name_t l_name;
    if (pmk_segregation_copy_from_user(
            context, l_name, name, sizeof(xky_name_t)) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* get port pointer and check if its valid */
    pmk_port_t *port = pmk_port_get_from_partition_by_name(partition, l_name);
    if (port == NULL || (port->channel->type & type) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_CONFIG;
    }

    /* check if port is already created */
    if (atomic_fetch(&port->created) == 1) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_NO_ACTION;
    }

    /* check if the port configurations are valid */
    xky_status_code_e rc = XKY_INVALID_POINTER;

    /* queuing port type */
    if (XKY_QUEUING_PORT == type) {

        /* copy configuration to local memory and check if */
        xky_queuing_port_status_t qconfig;
        if (pmk_segregation_get_user(context, qconfig, config) == 0) {

            rc = XKY_INVALID_CONFIG;
            if (pmk_queuing_port_check_configuration(port, &qconfig) == 0) {
                rc = XKY_NO_ERROR;
            }
        }
    }

    /* sampling port type */
    if (XKY_SAMPLING_PORT == type) {

        /* copy configuration to local memory and check if */
        xky_sampling_port_status_t sconfig;
        if (pmk_segregation_get_user(context, sconfig, config) == 0) {

            rc = XKY_INVALID_CONFIG;
            if (pmk_sampling_port_check_configuration(port, &sconfig) == 0) {
                rc = XKY_NO_ERROR;
            }
        }
    }

    /* check if errors occurred check the port configuration */
    if (rc != XKY_NO_ERROR) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return rc;
    }

    /* pass id to partition */
    if (pmk_segregation_put_user(context, port->id, pid) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* flag port as created */
    atomic_set(1, &port->created);

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

xky_status_code_e pmk_syscall_get_port_status(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t id,
        void *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get the current port */
    pmk_port_t *port = pmk_port_get_from_partition_by_id(partition, id, type);

    /* check if port exists */
    if (port == NULL || atomic_fetch(&port->created) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* call the correct function to read */
    xky_status_code_e rc = XKY_NO_ERROR;
    switch (type) {

        /* queuing port */
        case XKY_QUEUING_PORT:
            rc = pmk_queuing_port_get_status(
                    context,
                    port,
                    (xky_queuing_port_status_t *)status);
            break;

        /* sampling port */
        case XKY_SAMPLING_PORT:
            rc = pmk_sampling_port_get_status(
                    context,
                    port,
                    (xky_sampling_port_status_t *)status);
            break;

        /* invalid port */
        default:
            rc = XKY_INVALID_CONFIG;
            break;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return rc;
}

xky_status_code_e pmk_syscall_read_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t id,
        xky_message_ptr_t msg,
        xky_sz_t *length,
        void *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get the current port */
    pmk_port_t *port = pmk_port_get_from_partition_by_id(partition, id, type);

    /* check if port exists */
    if (port == NULL || atomic_fetch(&port->created) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* check if port is destination */
    if (port->direction != XKY_DESTINATION_PORT) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_MODE;
    }


    /* call the correct function to read */
    xky_status_code_e rc = XKY_NO_ERROR;
    switch (type) {

        /* queuing port */
        case XKY_QUEUING_PORT:
            rc = pmk_queuing_port_read(
                    context,
                    port,
                    msg,
                    length,
                    (xky_queuing_port_status_t *)status);
            break;

        /* sampling port */
        case XKY_SAMPLING_PORT:
            rc = pmk_sampling_port_read(
                    context,
                    port,
                    msg,
                    length,
                    (xky_sampling_port_status_t *)status);
            break;

        /* invalid port */
        default:
            rc = XKY_INVALID_CONFIG;
            break;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return rc;
}

xky_status_code_e pmk_syscall_write_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t id,
        xky_message_ptr_t msg,
        xky_sz_t length,
        void *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get the current port */
    pmk_port_t *port = pmk_port_get_from_partition_by_id(partition, id, type);

    /* check if port exists */
    if (port == NULL || atomic_fetch(&port->created) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* check if port is source */
    if (port->direction != XKY_SOURCE_PORT) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_MODE;
    }

    /* check if lenght is valid */
    if (length == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* call the correct function to read */
    xky_status_code_e rc = XKY_NO_ERROR;
    switch (type) {

        /* queuing port */
        case XKY_QUEUING_PORT:
            rc = pmk_queuing_port_write(
                    context,
                    port,
                    msg,
                    length,
                    (xky_queuing_port_status_t *)status);
            break;

        /* sampling port */
        case XKY_SAMPLING_PORT:
            rc = pmk_sampling_port_write(
                    context,
                    port,
                    msg,
                    length,
                    (xky_sampling_port_status_t *)status);
            break;

        /* invalid port */
        default:
            rc = XKY_INVALID_CONFIG;
            break;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return rc;
}

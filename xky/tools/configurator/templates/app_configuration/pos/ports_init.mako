<%
    import xky
    import utils.templates as makoutils

    # port define
    def definePort(port, type, obj):
        return '{0}_{1}_{2}_{3}'.format(
            partition.name,
            type, port.name, obj).replace(' ', '_').replace('-', '_').upper()
%>\
%if header:
${makoutils.cfileHeader('ports_init.h', 'Partition {0} - {1} ports initialization'.format(partition.id, partition.name))}

#ifndef __PORTS_INIT_H__
#define __PORTS_INIT_H__

## Queueing ports
% for i, port in enumerate(partition.ports):
% if getattr(port, 'max_nb_message', 0) > 0:
#define ${definePort(port, 'QPORT', 'NAME')} "${port.name}"
#define ${definePort(port, 'QPORT', 'MSG_SIZE')} ${port.max_message_size}
#define ${definePort(port, 'QPORT', 'DIRECTION')} ${port.direction}
#define ${definePort(port, 'QPORT', 'MAX_NB_MSG')} ${port.max_nb_message}

% endif
% endfor

## Sampling ports
% for i, port in enumerate(partition.ports):
% if getattr(port, 'max_nb_message', 0) == 0:
#define ${definePort(port, 'SPORT', 'NAME')} "${port.name}"
#define ${definePort(port, 'SPORT', 'MSG_SIZE')} ${port.max_message_size}
#define ${definePort(port, 'SPORT', 'DIRECTION')} ${port.direction}
#define ${definePort(port, 'SPORT', 'REFRESH')} ${int(port.refresh_period)}ll

% endif
% endfor

#endif /* __PORTS_INIT_H__ */

%else:
${makoutils.cfileHeader('ports_init.c', 'Partition {0} - {1} ports initialization'.format(partition.id, partition.name))}

#include <imaspex.h>
#include <pprintf.h>

#include "ports_init.h"

void ports_init(void) {

    RETURN_CODE_TYPE rc;
% if any([ p for p in partition.ports if getattr(p, 'max_nb_message', 0) > 0]):
    QUEUING_PORT_ID_TYPE qp_id;
% endif
% if any([ p for p in partition.ports if getattr(p, 'max_nb_message', 0) == 0]):
    SAMPLING_PORT_ID_TYPE sp_id;
% endif

## Queueing ports
% for i, port in enumerate(partition.ports):
% if getattr(port, 'max_nb_message', 0) > 0:
    /* create port : ${port} */
    CREATE_QUEUING_PORT(${definePort(port, 'QPORT', 'NAME')},
            ${definePort(port, 'QPORT', 'MSG_SIZE')},
            ${definePort(port, 'QPORT', 'MAX_NB_MSG')},
            ${definePort(port, 'QPORT', 'DIRECTION')},
            FIFO,
            &qp_id,
            &rc);

% endif
% endfor
\
## Queueing ports
% for i, port in enumerate(partition.ports):
% if getattr(port, 'max_nb_message', 0) == 0:
    /* create port : ${port} */
    CREATE_SAMPLING_PORT(${definePort(port, 'SPORT', 'NAME')},
             ${definePort(port, 'SPORT', 'MSG_SIZE')},
             ${definePort(port, 'SPORT', 'DIRECTION')},
             ${definePort(port, 'SPORT', 'REFRESH')},
             &sp_id,
             &rc);

% endif
% endfor
}
%endif

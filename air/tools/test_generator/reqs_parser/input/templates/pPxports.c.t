#include <rtems.h>
#include <string.h>

#include <pmk_plugin.h>
#include <pal_callbacks.h>
#include <pal.h>

#define P${part_num}_NUMBER_OF_PORTS (${port_num}U)

% if ports:
/* Ports definition 							 		*/
/* Mandatory: should be the first entry of this file 		*/
/* Thus, it is always the first entry in the data section 	*/
pal_port_t ports[P${part_num}_NUMBER_OF_PORTS] = { \

% for p in range(0,len(ports)):
 { \
	${ports[p]["id"]},			/* id 		*/
	${ports[p]["channel"]},		/* channel 	*/
	${ports[p]["type"]},		/* type		*/
	${ports[p]["addr"]},		/* addr		*/
	${ports[p]["dirty"]},		/* dirty		*/
	${ports[p]["msg_size"]},	/* msg_size	*/
	${ports[p]["max_num_msg"]},	/* max_num_msg*/
	"${ports[p]["name"]}",		/* name		*/
	{${ports[p]["this_end"][0]},${ports[p]["this_end"][1]}},	/* this  channel_end (part,port)	*/
	{${ports[p]["othr_end"][0]},${ports[p]["othr_end"][1]}},	/* other channel_end (part,port)	*/
% if p < len(ports) -1:
	&(ports[${p+1}])			/* nxt		*/
% else:
	NULL
% endif
 },
% endfor
};


rtems_status_code get_usr_ports (int i,void *p) {
	pal_port_t ** port_pointer = (pal_port_t **) p;
	*port_pointer = &(ports[0]);
	return RTEMS_SUCCESSFUL;
}

rtems_status_code refresh_ports (int i,void *p) {
	airpex_pmk_ports_refresh();
	return RTEMS_SUCCESSFUL;
}

% endif
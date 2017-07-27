#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

typedef struct pos_dispatcher_{
	iop_wrapper_t *reply_wrapper;
	iop_chain_control error;
	iop_chain_initialize_empty(&error);
}pos_dispatcher_inst;


void pre_dispatcher();
void pos_dispatcher();

#endif

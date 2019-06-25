<%
    # device functions
    device_functions = dict(
		reader_task='gr1553_read_task',
		writer_task='gr1553_write_task',
		header_prebuild='NULL',
		header_compare='gr1553_compare_header',
		header_copy='gr1553_copy_header')

    # Init and calculate dummy command(s) to obtain the desired MajorFrame period
    if device.setup.mode == "BC":
        slots_time = 0
        for i, command in enumerate(device.setup.millist[0].slot):
            slots_time += device.setup.millist[0].slot[i].time
        if (device.setup.millist[0].majorframe > slots_time):
            #We need to add dummy commands. How many with max timing of 262140us?
            dummy_slots = ((device.setup.millist[0].majorframe) - slots_time) / 262140 
            dummy_time = ((device.setup.millist[0].majorframe) - slots_time) - 262140 * dummy_slots
            if dummy_time:
                init_nb_cmd = len(device.setup.millist[0].slot) + (dummy_slots + 1) + 1
            else:
                init_nb_cmd = len(device.setup.millist[0].slot) + dummy_slots + 1
        else:
            init_nb_cmd = len(device.setup.millist[0].slot) + 1

        init_nb_cmd_async = device.setup.lroutes + 1
        init_data_buf = len(device.setup.millist[0].slot)
        if (device.setup.lroutes > len(device.setup.millist[0].slot)):
            init_data_buf = device.setup.lroutes;
    else:
        init_nb_cmd = 0;
        init_nb_cmd_async = 0;
        init_data_buf = 0;
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for MIL-STD-1553 interface using GR1553B driver.
 * ============================================================================
 */
 
#include <iop.h>
#include <IOPgr1553b.h>
#include <IOPgr1553${device.setup.mode.lower()}.h>
#include <IOPmilstd_config.h>
#include <IOPdriverconfig_interface.h>
#include <gr1553_support.h>

${MILAlloc(device)}\

${iop_template.RemotePortList(iop_configuration)}\

/** @brief GR1553B driver configuration */
static iop_1553_device_t device_configuration = ${'\\'}
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&mildriver,
        .init           = gr1553b_initialize,
        .open           = gr1553b_open,
        .read           = gr1553${device.setup.mode.lower()}_read,
        .write          = gr1553${device.setup.mode.lower()}_write,
        .close          = gr1553b_close,
    }
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\


${MILFuncs(device)}\
<% return %>


<%def name="MILAlloc(pdevice)">\

#define COMMAND_LIST_SIZE ${init_nb_cmd}
#define ASYNCHRONOUS_COMMAND_LIST_SIZE ${init_nb_cmd_async}
#define DATA_BUFFERS ${init_data_buf}

% if pdevice.setup.mode == 'BC':
/**
 * The memory is structured as follows:
 * COMMAND_LIST | DATA_BUFFERS | ASYNCHRONOUS_COMMAND_LIST | DATA_BUFFERS | 16 BYTE ALIGNMENT
 */
#define GR1553_MEMORY_SIZE ((COMMAND_LIST_SIZE*4) + 2*(DATA_BUFFERS*16) + (ASYNCHRONOUS_COMMAND_LIST_SIZE*4) + 4)

/**
 * @brief List of matching physical/virtual addresses used in the GR1553BC 
 * Need one for the async and sync register and one for each in Command List
 */
static gr1553hwaddr gr1553hwlist[COMMAND_LIST_SIZE + 2];

/**
 * @brief List of matching physical/virtual addresses used in the GR1553BC 
 * Need one for the async and sync register and one for each in Command List
 */
static write_cmd_shortcut_t shortcut_mem[${device.setup.lroutes}];

/**
 * @brief BC Transfer List
 */
static bc_command_t command_list[COMMAND_LIST_SIZE] = ${'\\'}
{
% for command in pdevice.setup.millist[0].slot:
${MILBCListStruct(command)}${','}
% endfor
% if dummy_slots:
% for i in range(dummy_slots):
${MILBCListStruct_management('DUMMY', 262140)}${','}
% endfor
% endif
% if dummy_time:
${MILBCListStruct_management('DUMMY', dummy_time)}${','}
% endif
${MILBCListStruct_management('LOOP', 0)}
};
% else:
/**
 * SA_TABLE_SIZE*2 + WORDS_PER_DESC(20)*DESC_PER_SUBADRESSES(12)*ENABLED_SUBADDRESSES(32)*2(RX and TX) + 16 BYTE ALIGNMENT
 */
#define GR1553_MEMORY_SIZE (256+(16+3+1)*32*12*2+4)

/**
 * @brief BC related
 */
#define gr1553hwlist (void*)NULL
#define command_list (void*)NULL
#define shortcut_mem (void*)NULL
% endif

/**
 * @brief Device Internal Memory
 */
static uint32_t gr1553bmem[GR1553_MEMORY_SIZE];

/**
 * @brief MIL-STD-1553 user configurations
 */
static grb_user_config_t userconf = ${'\\'}
{
    .operating_mode = GR1553B_MODE_${pdevice.setup.mode},
    .msg_timeout = 0,
    .retry_mode = 1,
    .rtaddress = 10,
    .modecode = 0x55555555,  	/* legalize ALL mode codes */
    .enabled_subs = 0xFFFFFFFF, /* legalize ALL subaddresses*/
    .databufs_per_sub = 12,
    .time_res = 10
};

/**
 * @brief Device Internal Struture
 */
static grb_priv mildriver = \
{
    .mem_start      = gr1553bmem,
    .minor          = ${pdevice.setup.id},
    .user_config    = &userconf,
    .shortcut_cmd   = (void *)shortcut_mem
};
</%def>


<%def name="MILFuncs(pdevice)">\
/**
 * @brief Driver Interface Functions
 */
bc_command_t *iop_milstd_get_command_list(){
    return command_list;
}

int iop_milstd_get_command_list_size(){
    return COMMAND_LIST_SIZE;
}

int iop_milstd_get_async_command_list_size(){
    return ASYNCHRONOUS_COMMAND_LIST_SIZE;
}

int iop_milstd_get_data_buffers_size(){
    return DATA_BUFFERS;
}

gr1553hwaddr *iop_get_gr1553hwlist(){
    return gr1553hwlist;
}
</%def>


<%def name="MILBCListStruct(cmd)">\
	{
        .ccw = CCW_${cmd.type}_${cmd.bus} | ${int(cmd.wcmode)},
        .rtaddr[0] = ${int(cmd.addr)},
        .rtaddr[1] = ${int(cmd.addrtx)},
        .subaddr[0] = ${int(cmd.subaddr)},
        .subaddr[1] = ${int(cmd.subaddrtx)},
        .branch_offset = 0,
        .time_slot = ${int(cmd.time)}
	}\
</%def>


<%def name="MILBCListStruct_management(mode, time)">\
	{
        .ccw = CCW_${mode},
        .rtaddr[0] = 0,
        .rtaddr[1] = 0,
        .subaddr[0] = 0,
        .subaddr[1] = 0,
        .branch_offset = 0,
        .time_slot = ${time}
	}\
</%def>

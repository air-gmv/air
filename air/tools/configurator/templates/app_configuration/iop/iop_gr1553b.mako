<%
    # device functions
    device_functions = dict(
		reader_task='gr1553{0}_read_task'.format(device.setup.mode.lower()),
		writer_task='gr1553{0}_write_task'.format(device.setup.mode.lower()),
		
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

        init_nb_cmd_async = device.setup.lroutes
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
#include <stdint.h>
#include <IOPgr1553b.h>
#include <IOPmilstd_config.h>
#include <IOPdriverconfig_interface.h>
#include <gr1553_support.h>
 
${MILDefs(device)}\

${MILAlloc(device)}\

${iop_template.RemotePortList(iop_configuration)}\

/** @brief GR1553B driver configuration */
static iop_device_driver_t device_configuration = ${'\\'}
{
    /* device configuration */
        .driver         = NULL,
        .init           = gr1553b_initialize,
        .open           = gr1553b_open,
        .read           = NULL,
        .write          = NULL,
        .close          = gr1553b_close,
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\


${MILFuncs(device)}\
<% return %>


<%def name="MILDefs(pdevice)">\
#define GR1553B_DEVICES 1

#define COMMAND_LIST_SIZE ${init_nb_cmd}
#define ASYNCHRONOUS_COMMAND_LIST_SIZE ${init_nb_cmd_async}
#define DATA_BUFFERS ${init_data_buf}

/**
 * The memory is structured as follows:
 * COMMAND_LIST | DATA_BUFFERS | ASYNCHRONOUS_COMMAND_LIST | DATA_BUFFERS
 */
#define BC_MEMORY_SIZE ((COMMAND_LIST_SIZE*4) + 2*(DATA_BUFFERS*16) + (ASYNCHRONOUS_COMMAND_LIST_SIZE*4))

/**
 * SA_TABLE_SIZE*2 + WORDS_PER_DESC(20)*DESC_PER_SUBADRESSES(12)*ENABLED_SUBADDRESSES(32)*2(RX and TX)
 */
#define RT_MEMORY_SIZE (256+(16+3+1)*32*12*2)
</%def>


<%def name="MILAlloc(pdevice)">\
/**
 * @brief Device Internal Struture
 */
static grb_priv bdevs[GR1553B_DEVICES];

/**
 * @brief MIL-STD-1553 user configurations
 */
static grb_user_config_t userconf[GR1553B_DEVICES] = ${'\\'}
{
${MILConfigStruct(pdevice)}
};

% if pdevice.setup.mode == 'BC':
/**
 * @brief Device Internal Memory. This memory will be 16 bytes aligned (+4)
 */
static uint32_t gr1553bmem[BC_MEMORY_SIZE+4];

/**
 * @brief List of matching physical/virtual addresses used in the GR1553BC 
 * Need one for the async and sync register and one for each in Command List
 */
static gr1553hwaddr gr1553hwlist[COMMAND_LIST_SIZE + 2];

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
 * @brief Device Internal Memory
 */
static uint32_t gr1553bmem[RT_MEMORY_SIZE];

/**
 * @brief BC related
 */
static gr1553hwaddr *gr1553hwlist = NULL;
static bc_command_t *command_list = NULL;
% endif
</%def>




<%def name="MILFuncs(pdevice)">\
/**
 * @brief Driver Interface Functions
 */
bc_command_t *iop_milstd_get_command_list(){
    return &command_list[0];
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
    return &gr1553hwlist[0];
}

void *iop_get_grb_mem(void){
    return (void *)&gr1553bmem[0];
}

grb_user_config_t *iop_grb_get_user_config(unsigned int minor){

    /* Return value */
    grb_user_config_t *rc;

    /* Verify if the requested minor is available */
    if(minor > GR1553B_DEVICES){

        /* There is no such device */
        rc = NULL;
	
    } else{

        /* return user config */
        rc = &userconf[minor];
    }

    return rc;
}

grb_priv *iop_grb_get_priv_mem(void){
    return &bdevs[0];
}

int iop_get_number_grb_cores(void){
    return GR1553B_DEVICES;
}
</%def>


<%def name="MILConfigStruct(pdevice)">\
    {
        .operating_mode = GR1553B_MODE_${pdevice.setup.mode},
        .msg_timeout = 0,
        .retry_mode = 1,
        .rtaddress = 10,
        .modecode = 0x55555555,  	/* legalize ALL mode codes */
        .enabled_subs = 0xFFFFFFFF, /* legalize ALL subaddresses*/
        .databufs_per_sub = 12,
        .time_res = 10
    }\
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

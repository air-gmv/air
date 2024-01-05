## @package air.arm
#  @author lumm
#  @brief AIR ARM definitions and utility functions

__author__ = 'lumm'

import utils
from air import *
from parsers.a653.definitions import SAMPLING_CHANNEL, QUEUING_CHANNEL


##
# @brief Class to hold ARM specific configuration
class ARMConfiguration(object):

    def __init__(self):

        self.mmu_context_entries = 0
        self.mmu_l1_tables_entries = 0
        self.mmu_l2_tables_entries = 0
        self.workspace_size = 0

MAX_CORES                  = 2

CPU_CORE_CONTROL            = [       0x8,      0x24]
CPU_CORE_CONTEXT            = [       0x8,      0xA0]
CPU_CORE_CONTEXT_STACK      = [       0x8,    0x13B0] # 15x isf (0x336 each)
#CPU_CORE_CONTEXT_FPU        = [       0x8,     0x108]
CPU_CORE_CONTEXT_HM         = [       0x8,      0x10]

CPU_MMU_CONTEXT             = [       0x8,      0x10]
CPU_MMU_L1_TABLES           = [    0x4000,    0x4000]
CPU_MMU_L2_TABLES           = [     0x400,     0x400]

CPU_PORT_MSG                = [       0x8,       0x1]
CPU_PORT_MSG_SLOT           = [       0x8,      0x10]

MMU_LEVEL_ENTRIES           = [      4096,       256]
MMU_LEVEL_UNIT              = [   1 << 20,   1 << 12]


def partition_mmu_get_index(v_addr, n):

    if n == 0:
        return v_addr / (1 << 20)
    elif n == 1:
        return (v_addr & ((1 << 20) - (1 << 12))) >> 12

def partition_mmu_tables(table, v_addr, n, size, unit):

    i = 0
    consumed = 0
    level_entries = MMU_LEVEL_ENTRIES[n]
    level_unit = MMU_LEVEL_UNIT[n]

    while size > 0 and i < level_entries - 1:

        i = int(partition_mmu_get_index(v_addr, n))

        if unit < level_unit:

            if table[i] is None:
                table[i] = MMU_LEVEL_ENTRIES[n+1] * [None]

            l_consumed = partition_mmu_tables(table[i], v_addr, n + 1, size, unit)

        else:

            l_consumed = level_unit

        consumed += l_consumed
        v_addr += l_consumed
        size -= l_consumed

    return consumed

def align(addr, unit):
    return ((addr + (unit - 1)) & ~(unit - 1))

def align_space(space, spec):
    return align(space, spec[0]) + spec[1]

def get_arm_configuration(a653configuration):

    p_count = len(a653configuration.partitions)

    # ARM configuration
    arm_configuration = ARMConfiguration()

    # determine the required MMU tables
    arm_configuration.mmu_context_entries = p_count
    arm_configuration.mmu_l1_tables_entries = p_count
    arm_configuration.mmu_l2_tables_entries = 0

    for partition in a653configuration.partitions:

        mmu_table = MMU_LEVEL_ENTRIES[0] * [None]

        # main partition memory
        partition_mmu_tables(mmu_table, partition.memory_vaddr, 0, 
                            align(partition.memory_size, 
                                  partition.memory_unit), 
                            partition.memory_unit)

        # shared area partition memory
        for shared_memory in a653configuration.shared_memory:
            for permission in shared_memory.permissions:
                if permission.partition == partition:
                    partition_mmu_tables(mmu_table, shared_memory.addr, 0, 
                                        align(shared_memory.size, 
                                              shared_memory.unit),
                                        shared_memory.unit)

        for i in range(0, MMU_LEVEL_ENTRIES[0]):
            if mmu_table[i] is not None:
                arm_configuration.mmu_l2_tables_entries += 1

    # determine the space for the MMU table
    mmu_context =   [CPU_MMU_CONTEXT[0],   arm_configuration.mmu_context_entries * CPU_MMU_CONTEXT[1]]
    mmu_l1_tables = [CPU_MMU_L1_TABLES[0], arm_configuration.mmu_l1_tables_entries * CPU_MMU_L1_TABLES[1]]
    mmu_l2_tables = [CPU_MMU_L2_TABLES[0], arm_configuration.mmu_l2_tables_entries * CPU_MMU_L2_TABLES[1]]

    # space for the MMU tables
    arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, mmu_l1_tables)
    arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, mmu_l2_tables)
    arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, mmu_context)

    # space for the core context IDLE
    core_context = [ CPU_CORE_CONTEXT[0], a653configuration.core_count * CPU_CORE_CONTEXT[1]]
    arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, core_context)
    for i in range(0, a653configuration.core_count):
        arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_STACK)
        #arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_FPU)
        arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_HM)

    # space for the core control structures
    core_control = [ CPU_CORE_CONTROL[0], a653configuration.core_count * CPU_CORE_CONTROL[1]]
    arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, core_control)

    # space for the partitions
    for partition in a653configuration.partitions:

        # space for the partition cores context
        core_context = [CPU_CORE_CONTEXT[0], partition.core_count * CPU_CORE_CONTEXT[1]]
        arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, core_context)

        # space for the partition cores context internals
        for i in range(0, partition.core_count):
            arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_STACK)
            #arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_FPU)
            arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, CPU_CORE_CONTEXT_HM)

        # partition core mapping
        arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, [0x4, MAX_CORES * 0x4])

    # space for the ports
    for channel in a653configuration.channels:

        # message size
        msg_spec = [CPU_PORT_MSG[0], channel.src[0].max_message_size * CPU_PORT_MSG[1]]

        if   channel.type == SAMPLING_CHANNEL:

            # double buffer messaging
            arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, [CPU_PORT_MSG_SLOT[0], 2 * CPU_PORT_MSG_SLOT[1]])

            for i in range(0, 2):
                arm_configuration.workspace_size =  align_space(arm_configuration.workspace_size, msg_spec)

        elif channel.type == QUEUING_CHANNEL:

            arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, [CPU_PORT_MSG_SLOT[0], channel.src[0].max_nb_message * CPU_PORT_MSG_SLOT[1]])
            for i in range(0, channel.src[0].max_nb_message):
                arm_configuration.workspace_size = align_space(arm_configuration.workspace_size, msg_spec)

    # final alignment
    #arm_configuration.workspace_size = \
    #    align_space(arm_configuration.workspace_size, [0x1000, 0x0000])

    return arm_configuration

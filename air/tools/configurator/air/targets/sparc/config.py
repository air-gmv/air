## @package air.sparc
#  @author pfnf
#  @brief AIR SPARC definitions and utility functions

__author__ = 'pfnf'

import utils
from air import *
from parsers.a653.definitions import SAMPLING_CHANNEL, QUEUING_CHANNEL


##
# @brief Class to hold SPARC specific configuration
class SPARCConfiguration(object):

    def __init__(self):

        self.mmu_context_entries = 0
        self.mmu_l1_tables_entries = 0
        self.mmu_l2_tables_entries = 0
        self.mmu_l3_tables_entries = 0
        self.workspace_size = 0

MAX_CORES                  = 4

CPU_CORE_CONTROL           = [        0x8,        0x28]
CPU_CORE_CONTEXT           = [        0x4,        0x44]
CPU_CORE_CONTEXT_STACK     = [        0x8,       0xB80]
CPU_CORE_CONTEXT_FPU       = [        0x8,        0x88]
CPU_CORE_CONTEXT_HM        = [        0x8,        0x10]
CPU_MMU_CONTROL            = [        0x4,         0x8]
CPU_MMU_CONTEXT            = [      0x400,         0x4]
CPU_MMU_L1_TABLES          = [      0x400,       0x400]
CPU_MMU_L2_TABLES          = [      0x100,       0x100]
CPU_MMU_L3_TABLES          = [      0x100,       0x100]
CPU_PORT_MSG               = [        0x8,         0x1]
CPU_PORT_MSG_SLOT          = [        0x8,        0x10]


def partition_mmu_get_index(n, v_addr):

    if n == 0:
        return v_addr / (1 << 24)
    elif n == 1:
        return (v_addr & ((1 << 24) - (1 << 18))) >> 18
    elif n == 2:
        return (v_addr & ((1 << 18) - (1 << 12))) >> 12


def partition_mmu_tables(table, v_addr, n, size, unit):

    i = 0
    consumed = 0
    level_entries = [256, 64, 64][n]
    level_unit = [1 << 24, 1 << 18, 1 << 12][n]

    while size > 0 and i < level_entries - 1:

        i = int(partition_mmu_get_index(n, v_addr))
        
        if table[i] is None:
            table[i] = [256, 64, 64][n] * [ None ]

        if unit < level_unit:

            l_consumed = partition_mmu_tables(table[i], v_addr, n + 1, size, unit)

        else:

            l_consumed = level_unit

        consumed += l_consumed
        v_addr += l_consumed
        size -= l_consumed

    return consumed



def align_space(space, spec):

    space = ((space + (spec[0] - 1)) & ~(spec[0] - 1))
    return space + spec[1]

def get_sparc_configuration(a653configuration):

    p_count = len(a653configuration.partitions)

    # SPARC configuration
    sparc_configuration = SPARCConfiguration()

    # determine the required MMU tables
    sparc_configuration.mmu_context_entries = p_count
    sparc_configuration.mmu_l1_tables_entries = p_count
    sparc_configuration.mmu_l2_tables_entries = 0
    sparc_configuration.mmu_l3_tables_entries = 0

    for partition in a653configuration.partitions:

        mmu_table = 256 * [None]

        # main partition memory
        partition_mmu_tables(
            mmu_table,
            partition.memory_vaddr, 0, partition.memory_size, partition.memory_unit)

        # shared area partition memory
        for shared_memory in a653configuration.shared_memory:
            for permission in shared_memory.permissions:
                if permission.partition == partition:
                    partition_mmu_tables(
                        mmu_table,
                        shared_memory.addr, 0, shared_memory.size, shared_memory.unit)

        for i in range(0, 256):
            if mmu_table[i] is not None:
                for j in range(0, 64):
                    if mmu_table[i][j] is not None:
                        sparc_configuration.mmu_l2_tables_entries += 1
                        for k in range(0, 64):
                            if mmu_table[i][j][k] is not None:
                                sparc_configuration.mmu_l3_tables_entries += 1

    # determine the space for the MMU table
    mmu_context =   [CPU_MMU_CONTEXT[0],   sparc_configuration.mmu_context_entries * CPU_MMU_CONTEXT[1]]
    mmu_l1_tables = [CPU_MMU_L1_TABLES[0], sparc_configuration.mmu_l1_tables_entries * CPU_MMU_L1_TABLES[1]]
    mmu_l2_tables = [CPU_MMU_L2_TABLES[0], sparc_configuration.mmu_l2_tables_entries * CPU_MMU_L2_TABLES[1]]
    mmu_l3_tables = [CPU_MMU_L3_TABLES[0], sparc_configuration.mmu_l3_tables_entries * CPU_MMU_L2_TABLES[1]]

    # space for the MMU tables
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, mmu_context)
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, mmu_l1_tables)
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, mmu_l2_tables)
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, mmu_l3_tables)

    # space for the MMU control structures
    for i in range(0, len(a653configuration.partitions)):
        sparc_configuration.workspace_size = \
            align_space(sparc_configuration.workspace_size, CPU_MMU_CONTROL)

    # space for the core context IDLE
    core_context = [ CPU_CORE_CONTEXT[0], a653configuration.core_count * CPU_CORE_CONTEXT[1]]
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, core_context)
    for i in range(0, a653configuration.core_count):
        sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_STACK)
        sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_FPU)
        sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_HM)

    # space for the core control structures
    core_control = [ CPU_CORE_CONTROL[0], a653configuration.core_count * CPU_CORE_CONTROL[1]]
    sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, core_control)

    # space for the partitions
    for partition in a653configuration.partitions:

        # space for the partition cores context
        core_context = [CPU_CORE_CONTEXT[0], partition.core_count * CPU_CORE_CONTEXT[1]]
        sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, core_context)

        # space for the partition cores context internals
        for i in range(0, partition.core_count):
            sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_STACK)
            sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_FPU)
            sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, CPU_CORE_CONTEXT_HM)

        # partition core mapping
        sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, [0x4, MAX_CORES * 0x4])

    # space for the ports
    for channel in a653configuration.channels:

        # message size
        msg_spec = [CPU_PORT_MSG[0], channel.src[0].max_message_size * CPU_PORT_MSG[1]]

        if   channel.type == SAMPLING_CHANNEL:

            # double buffer messaging
            sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, [CPU_PORT_MSG_SLOT[0], 2 * CPU_PORT_MSG_SLOT[1]])

            for i in range(0, 2):
                sparc_configuration.workspace_size =  align_space(sparc_configuration.workspace_size, msg_spec)

        elif channel.type == QUEUING_CHANNEL:

            sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, [CPU_PORT_MSG_SLOT[0], channel.src[0].max_nb_message * CPU_PORT_MSG_SLOT[1]])
            for i in range(0, channel.src[0].max_nb_message):
                sparc_configuration.workspace_size = align_space(sparc_configuration.workspace_size, msg_spec)

    # final alignment
    #sparc_configuration.workspace_size = \
    #    align_space(sparc_configuration.workspace_size, [0x1000, 0x0000])

    return sparc_configuration





<%
    import os
    import xky
    import utils.templates as makoutils

    mmap = os_configuration.get_memory_map()
%>\
/**
 * @file
 * @brief Linker defintions for partition code
 */

MEMORY
{
    rom     : ORIGIN = 0x00000000, LENGTH = 0x00000000
    ram     : ORIGIN = ${'0x{:08X}'.format(partition.memory_vaddr)}, LENGTH = ${'0x{:08X}'.format(partition.memory_size)}
}

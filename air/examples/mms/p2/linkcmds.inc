/**
 * @file
 * @brief Linker defintions for partition code
 */

MEMORY
{
    rom     : ORIGIN = 0x00000000, LENGTH = 0x00000000
    ram     : ORIGIN = 0x41000000, LENGTH = 0x00100000
}

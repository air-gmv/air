/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file linkcmds.ld
 * @author lumm
 * @brief This is the base OS POS linkcmds
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

HeapSize = DEFINED(HeapSize) ? HeapSize : 0x40000;
StackSize = DEFINED(StackSize) ? StackSize : 0x4000;

MEMORY {
    rom : ORIGIN = 0x00000000, LENGTH = 0x00000000
    ram : ORIGIN = 0x04000000, LENGTH = 0x04000000
}

ENTRY(bare_start)

SECTIONS {
    .text : {
        CREATE_OBJECT_SYMBOLS
        *(.text)
        . = ALIGN(32);
        etext = .;
    } >ram
    
    .data : {
        *(.data)
        . = ALIGN(32);
        edata = .;
    } >ram
    
    .bss : {
        bss_start = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(32);
        bss_end = .;
        
        _HeapStart = .;
        . += HeapSize;
        
        . += StackSize;
        . += ALIGN(32);
        stack_start = .;
    }>ram
}
/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file libelf.c
 * @author pfnf
 * @brief Provides set of functions to provide a minimal ELF support
 */

#include <stdbool.h>


#include "libelf.h"

/**
 * @brief Checks if the machine is big or little endian
 * @return true if big endian, false otherwise
 */
bool elf_machine_is_bigendian() {

    volatile int32_t num = 1;
    return *(int8_t*)&num == 1 ? false : true;
}

/**
 *  \brief Fixes the endianness of a uint16_t
 *
 *  \param [in] value input value
 *  \param [in] source_big_endian Value source endianness
 *  \param [in] target_big_endian Target endianness for the value
 *
 *  \return true if big endian, false otherwise
 *
 *  Fixes the endianness of a uint16_t based on the value source endianness and
 *  target value endianness
 */
uint16_t elf_correct_uint16(uint16_t value,
                            bool source_big_endian,
                            bool target_big_endian){

    uint32_t ret = value;

    /* Swap bytes if the sorces are diferent */
    if ((source_big_endian && !target_big_endian) ||
        (!source_big_endian && target_big_endian)){

        ret = (value << 8) | (value>> 8 );

    }

    return ret;
}

/**
 *  \brief Fixes the endianness of a uint32_t
 *
 *  \param [in] value input value
 *  \param [in] source_big_endian Value source endianness
 *  \param [in] target_big_endian Target endianness for the value
 *
 *  \return true if big endian, false otherwise
 *
 *  Fixes the endianness of a uint32_t based on the value source endianness and
 *  target value endianness
 */
uint32_t elf_correct_uint32(uint32_t value,
                            bool source_big_endian,
                            bool target_big_endian){

    uint32_t ret = value;

    /* Swap bytes if the sorces are diferent */
    if ((source_big_endian && !target_big_endian) ||
        (!source_big_endian && target_big_endian)){

        ret = ((value >> 24) & 0x000000ff) | ((value << 8 ) & 0x00ff0000) |
              ((value >> 8 ) & 0x0000ff00) | ((value << 24) & 0xff000000);

    }

    return ret;
}

/**
 *  \brief Validate an ELF 32 for sparc
 *
 *  \param [in] in_buffer Buffer ELF input
 *  \return true if valid, false otherwise
 *
 *  Valides an ELF for 32 for sparc by examing the file header
 */
unsigned char elf_check(unsigned char *in_buffer){

    unsigned char ret = 1;
    Elf32_Ehdr *Ehdr = (Elf32_Ehdr *)in_buffer;

    /* Check header: ([0x7f]ELF for valid elf files) */
    if (0x7f != Ehdr->e_ident[0] || 'E' != Ehdr->e_ident[1] ||
        'L' != Ehdr->e_ident[2] || 'F' != Ehdr->e_ident[3]){
        // printf(" elf_check() invalid magic number \n");
        ret = 0;
    }

    /* Check data class: (1 for 32 bits objects) */
    if (ret && 1 != Ehdr->e_ident[4]){
        // printf(" elf_check() invalid class\n");
        ret = 0;
    }

    /* Get the machine and file endinaess */
    bool mbig = elf_machine_is_bigendian();
    bool fbig = Ehdr->e_ident[5] == 2 ? true : false;

    /* Check machine: (2 for sparc) */
    if (ret && 2 != elf_correct_uint16(Ehdr->e_machine, fbig, mbig)){
        // printf(" elf_check() invalid machine code\n");
        ret = 0;
    }

    /* Check version: (1 for current) */
    if (ret && 1 != elf_correct_uint32(Ehdr->e_version, fbig, mbig)){
        // printf(" elf_check() invalid version\n");
        ret = 0;
    }

    /* Check type: (2 for executable) */
    if (ret && 2 != elf_correct_uint16(Ehdr->e_type, fbig, mbig)){
        // printf(" elf_check() invalid type\n");
        ret = 0;
    }

    /* Check if the file contains program segements */
    if (ret && 0 >= elf_correct_uint16(Ehdr->e_phnum, fbig, mbig)){
        // printf(" elf_check() doesn't contain program segments\n");
        ret = 0;
    }

    return ret;
}

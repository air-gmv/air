/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 *  @file
 *  @author pfnf
 *  @brief Functions to de-compress a partition into memory
 */

#include <pmk.h>
#include <error.h>
#include <atomic.h>
#include <loader.h>

/**
 * @brief LZSS algorithm (MAGIC_NUMBER symbol)
 * @ingroup pmk_loader
 */
#define MAGIC_NUMBER                                    ('\xaa')

/**
 * @brief LZSS algorithm (EOP symbol)
 * @ingroup pmk_partition
 */
#define EOP                                             ('\x55')
/**
 * @brief LZSS algorithm
 * @ingroup pmk_partition
 */
#define WINDOW_SIZE                                     (4096)

/**
 * @brief LZSS Packet header
 * @ingroup pmk_partition
 */
typedef struct {

    air_i8_t magic;                   /**< Magic number                       */
    air_u8_t parameters;             /**< Compression parameters             */
    air_u8_t checksum;               /**< Compression checksum               */
    air_u8_t dummy;                  /**< Dummy offset                       */
    air_u8_t encoded_size[4];        /**< Encoded_size size                  */
    air_u8_t decoded_size[4];        /**< Decoded size                       */

} lzss_header_t;

/**
 * @brief LZSS buffer
 * @note Defined as global variable to prevent stack overflows
 * @ingroup pmk_partition
 */
static char dictionary[WINDOW_SIZE];

/**
 * @brief De-compresses an input buffer sing LZSS algorithm
 * @param input Input buffer
 * @param output Output buffer
 * @param output_size Size available in the output
 * @return 0 if no errors occurred, 1 otherwise
 * @ingroup pmk_partition
 */
static int lzss_decompress(
        air_u8_t *input, air_u8_t *output, air_sz_t output_size){

    int i, j, k, r, c;
    air_u32_t flags;
    air_u8_t checksum = 0xff;

    /* get LZSS header */
    lzss_header_t *header = (lzss_header_t *)input;

    /* check if the magic match */
    if ((air_i8_t)header->magic  != (air_i8_t)MAGIC_NUMBER){
        return 1;
    }

    /* extract header information */
    air_u32_t ws = (header->parameters & 0xF0) << 6;
    air_u8_t threshold = (header->parameters & 0x03) + 1;
    air_u32_t lhs = (1 << ((header->parameters & 0x0c) >> 2)) * 9;
    air_sz_t input_size = \
            (header->encoded_size[0] << 24) | (header->encoded_size[1] << 16) |
            (header->encoded_size[2] <<  8) | (header->encoded_size[3]);

    /* initialize carrets and input buffsers */
    input += sizeof(lzss_header_t);

    /* initialize dictionary window buffer  */
    r = ws - lhs;
    for (i = 0; i < ws - lhs; ++i) {
        dictionary[i] = ' ';
    }

    /* compute the end of the streams */
    air_u8_t *input_end = input + input_size;
    air_u8_t *output_end = output + output_size;

    /* de-compress the input stream */
    flags = 0;
    while (1) {

        /* read the encoded/not encoded flag */
        if (((flags >>= 1) & 0x100) == 0){

            /* check if we reached the end of the input stream */
            if (input == input_end) {
                break;
            }

            c = *input++;
            flags = c | 0xff00;
        }

        /* check if the flag indicates an encoded string */
        if ((flags & 1) == 1){

            /* check if we reached the end of one of the streams */
            if (input > input_end || (output + 1) > output_end) {
                break;
            }

            /* read character from input and copy it to the output stream */
            c = *input++;
            *output++ = c;

            /* compute input checksum */
            checksum ^= c;

            /* shift a copy of the symbols written to the decoded output */
            dictionary[r++] = c;
            r &= (ws - 1);

        /* the flag represents an encoded string */
        } else {

            /* check if we reached the end of the input stream */
            if ((input + 2) > input_end) {
                break;
            }

            /* the the position and length of the string in dictionary */
            i = *input++;
            j = *input++;
            i |= ((j & 0xF0) << 4);
            j  =  (j & 0x0F) + threshold;

            /* check if we reached the end of the output stream */
            if ((output + j) > output_end) {
                break;
            }

            /* copy dictionary entry to the output and update the dictionary */
            for (k = 0; k <= j; k++) {
                c = dictionary[(i + k) & (ws - 1)];
                *output++ = c; checksum ^= c;
                dictionary[r++] = c;
                r &= (ws - 1);
            }
        }
    }

    /* checksum fail or unexpected end of streams */
    if (checksum != header->checksum || *input != EOP){
        return 1;
    }

    return 0;
}

void pmk_partition_load(pmk_elf_t *elf, void *p_addr, void *v_addr) {

    int i;
    static atomic_t loading;

    /* wait until other processor finishes loading */
    while (atomic_swap(1, &loading) == 1);

    for (i = 0; i < elf->count; ++i) {

        /* de-compress partition */
        air_u32_t error = lzss_decompress(
                (air_u8_t *)(elf->data + (air_sz_t)elf->segments[i].offset),
                (air_u8_t *)((air_uptr_t)p_addr + elf->segments[i].addr),
                elf->segments[i].size);

        /* check if error occurred */
        if (error != 0) {
            pmk_fatal_error(
                    PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
        }
    }

    /* release resources */
    atomic_set(0, &loading);
    return;
}

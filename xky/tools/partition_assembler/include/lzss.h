/**
 * @file
 * @author pfnf
 * @brief
 */

#ifndef __LZSS_H__
#define __LZSS_H__

#include <stdint.h>
#include <stdlib.h>

#define MAGIC_NUMBER '\xaa'
#define PH_SIZE 12
#define EOP '\x55'

/**
 * @brief lzss packet header
 */
typedef struct {

    char MAGIC;
    unsigned char PARAMS;
    unsigned char CHECKSUM;
    unsigned char dummy;
    unsigned char ENCODED_SIZE[4];
    unsigned char DECODED_SIZE[4];

} lzss_header_t;

/**
 * @brief
 * @param inbuf
 * @param outbuf
 * @param size
 * @return
 */
size_t lzss_compress(char *inbuf, char *outbuf, size_t size);

#endif /* __LZSS_H__ */

/**
 * @file
 * @author pfnf
 * @brief
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

typedef struct {

    char *filename;
    char *elf_data;
    char *segments;
    uint64_t mem_origin;
    uint64_t mem_length;

    size_t size;
    size_t c_size;


    uint32_t entry;
    uint32_t count;
    uint32_t *addrs;
    uint32_t *sizes;
    uint32_t *offsets;


} p_data;

/**
 * @brief Memory allocation with error handling
 * @param size Size of the memory block to allocate
 * @return Pointer to allocated the memory block
 */
static void *safe_malloc(size_t size) {

    void *ptr = NULL;
    if ((ptr = malloc(size)) == NULL) {
        errx(EX_SOFTWARE, " malloc() failed - %s", strerror(errno));
    }
    return ptr;
}

#endif /* __COMMON_H__ */

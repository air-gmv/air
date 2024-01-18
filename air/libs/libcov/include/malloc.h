/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  * linux/compr_mm.h
 *   *
 *    * Memory management for pre-boot and ramdisk uncompressors
 *     *
 *      * Authors: Alain Knaff <alain@knaff.lu>
 *       *
 *        */

#ifndef DECOMPR_MM_H
#define DECOMPR_MM_H

/* A trivial malloc implementation, adapted from
 *  *  malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 *   */

#ifndef STATIC_RW_DATA

#define STATIC_RW_DATA static

#endif

static char aux[2048];

unsigned long free_mem_ptr = &aux[0];
unsigned long free_mem_end_ptr = &aux[2047];

STATIC_RW_DATA unsigned long malloc_ptr;
STATIC_RW_DATA int malloc_count;

static void *malloc(int size)
{
    void *p;

    if (size < 0)
    {
        return 0;
    }

    if (!malloc_ptr)
    {
        malloc_ptr = free_mem_ptr;
    }

    malloc_ptr = (malloc_ptr + 3) & ~3; /* Align */

    p = (void *)malloc_ptr;
    malloc_ptr += size;

    if (free_mem_end_ptr && malloc_ptr >= free_mem_end_ptr)
    {
        return 0;
    }

    malloc_count++;
    return p;
}

static void free(void *where)
{
    malloc_count--;
    if (!malloc_count)
    {
        malloc_ptr = free_mem_ptr;
    }
}

#endif /* DECOMPR_MM_H */

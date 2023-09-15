/*
 * Copyright (C) 2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file libc.c
 * @author $(AIR_RUNNER_USER)
 * @brief Defines functionality of LIBC needed by AIR
 */

#include <libc.h>

/**
 * @brief Implementation of memory copy function
 * @param source pointer memory area with original data
 * @param destination pointer memory are to write a copy
 * @param length amount of bytes to copy
 */
void *memcpy(void *destination , const void *source , air_sz_t length)
{
    /*byte pointers */
    air_i8_t *b_destination = (air_i8_t *) destination;
    const air_i8_t *b_source = source;

    /* integer pointers */
    air_i32_t *i_destination;
    const air_i32_t *i_source;
    air_u32_t i_len = (air_u32_t) length;

if((((AIR_CHECK_LEN128(i_len) != 0)) == 0U) &&
       ((AIR_MEM_ALIGN((air_u32_t) source , (air_u32_t) destination)) == 0))
    {
        i_destination = (air_i32_t*) b_destination;
        i_source = (air_i32_t*) b_source;
 
        /* copy 128 words at a time  */
        while(i_len >= AIR_INT128SIZE)
        {
            *i_destination = *i_source;
            i_destination++;
            i_source++;

            *i_destination = *i_source;
            i_destination++;
            i_source++;

            *i_destination = *i_source;
            i_destination++;
            i_source++;

            *i_destination = *i_source;
            i_destination++;
            i_source++;

            i_len -= AIR_INT128SIZE;
        }

        /* copy 4 bytes at a time*/
        while(i_len >= AIR_INT32SIZE)
        {
            *i_destination = *i_source;
            i_destination++;
            i_source++;
            i_len -= AIR_INT32SIZE;
        }

        /* remaining bytes pointer*/
        b_destination = (air_i8_t*) i_destination;
        b_source = (air_i8_t*) i_source;
    }

    /* copy remaining bytes */
    while(i_len > 0U)
    {
        *b_destination = *b_source;
        b_destination++;
        b_source++;
        i_len--;
    }

    return destination;
}

/**
 * @brief Implementation of compare 2 strings of size n
 * @param string1 String to compare
 * @param string2 String to compare with
 * @param n number of bytes to compare 
 */
int strncmp(const char *string1 , const char *string2 , air_sz_t n)
{
    air_i32_t result = 0;
    air_u32_t counter = 0U;

    /* jump while bytes are equal or untiln end of strin*/
    while(( counter < n ) && 
          ( string1[counter] == string2[counter] ) && 
          ( string1[counter] != 0 ))
    {
        counter++;
    }

    if(counter == n)
    {
        /* equal strings*/
        result = 0;
    }
    else
    {
        /* return index in string of where diff occurs */
        result = (air_i32_t) string1[counter] - (air_i32_t) string2[counter];
    }
    return result;
}

/**
 * @brief Implementation set memory region with a value
 * @param mem_pointer Pointer to memory region
 * @param value The value to set
 * @param length number of bytes to set the value in region 
 */
void *memset(void *mem_pointer , int value , air_sz_t length)
{
    /* pointers*/
    air_i8_t *write_pointer = (air_i8_t *) mem_pointer;
    air_uptr_t *aligned_addr;
    air_u32_t unsign_var = (air_u32_t) value & 0xffU;

    /* iterator */
    air_u32_t i;

    /* buffer  */
    air_u32_t buffer;

    /* if length is not small and address is aligned */
    if(( AIR_CHECK_LENUINTPT(length) == 0U ) && ( AIR_ALIGN_UINT(mem_pointer) == 0U ))
    {
        aligned_addr = (air_uptr_t *) mem_pointer;

        if(AIR_UINTPTSIZE == 4U)
        {
            /* copy to the buffer */
            buffer = (air_u32_t) ( ( unsign_var << 8U ) | unsign_var );
            buffer = buffer | ( buffer << 16U );
        }
        else
        {
            buffer = 0U;
            for(i = 0U; i < AIR_UINTPTSIZE; i++)
            {
                buffer = ( buffer << 8U ) | unsign_var;
            }
        }

        /* copy the aligned bytes */
        while(length >= AIR_UINTPTSIZE * 4U)
        {
            *aligned_addr = buffer;
            aligned_addr++;

            *aligned_addr = buffer;
            aligned_addr++;

            *aligned_addr = buffer;
            aligned_addr++;

            *aligned_addr = buffer;
            aligned_addr++;

            length -= (air_sz_t) ( 4U * AIR_UINTPTSIZE );
        }

        /* copy the 4 bytes */
        while(length >= AIR_UINTPTSIZE)
        {
            *aligned_addr = buffer;
            aligned_addr++;
            length -= AIR_UINTPTSIZE;
        }
        write_pointer = (air_i8_t*) aligned_addr;
    }

    /* copy the remaining bytes */
    while(length > 0U)
    {
        *write_pointer = (air_i8_t) unsign_var;
        write_pointer++;
        length--;
    }

    return mem_pointer;
}

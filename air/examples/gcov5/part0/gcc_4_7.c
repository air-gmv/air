/*
 *  *  This code provides functions to handle gcc's profiling data format
 *   *  introduced with gcc 4.7.
 *    *
 *     *  This file is based heavily on gcc_3_4.c file.
 *      *
 *       *  For a better understanding, refer to gcc source:
 *        *  gcc/gcov-io.h
 *         *  libgcc/libgcov.c
 *          *
 *           *  Uses gcc-internal data definitions.
 *            */

#include "gcov.h"
//#include <memory.h>
#include <rtems.h>
//#include <pmk.h>
#include <pprintf.h>

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 9
#define GCOV_COUNTERS           9
#else
#define GCOV_COUNTERS           8
#endif

#define GCOV_TAG_FUNCTION_LENGTH    3

typedef unsigned int u32;
typedef unsigned long long u64;
#define EINVAL 22

/**
 *  * struct gcov_ctr_info - information about counters for a single function
 *   * @num: number of counter values for this type
 *    * @values: array of counter values for this type
 *     *
 *      * This data is generated by gcc during compilation and doesn't change
 *       * at run-time with the exception of the values array.
 *        */
struct gcov_ctr_info {
        unsigned int num;
            gcov_type *values;
};

/**
 *  * struct gcov_fn_info - profiling meta data per function
 *   * @key: comdat key
 *    * @ident: unique ident of function
 *     * @lineno_checksum: function lineo_checksum
 *      * @cfg_checksum: function cfg checksum
 *       * @ctrs: instrumented counters
 *        *
 *         * This data is generated by gcc during compilation and doesn't change
 *          * at run-time.
 *           *
 *            * Information about a single function.  This uses the trailing array
 *             * idiom. The number of counters is determined from the merge pointer
 *              * array in gcov_info.  The key is used to detect which of a set of
 *               * comdat functions was selected -- it points to the gcov_info object
 *                * of the object file containing the selected comdat function.
 *                 */
struct gcov_fn_info {
        const struct gcov_info *key;
            unsigned int ident;
                unsigned int lineno_checksum;
                    unsigned int cfg_checksum;
                        struct gcov_ctr_info ctrs[0];
};

/**
 *  * struct gcov_info - profiling data per object file
 *   * @version: gcov version magic indicating the gcc version used for compilation
 *    * @next: list head for a singly-linked list
 *     * @stamp: uniquifying time stamp
 *      * @filename: name of the associated gcov data file
 *       * @merge: merge functions (null for unused counter type)
 *        * @n_functions: number of instrumented functions
 *         * @functions: pointer to pointers to function information
 *          *
 *           * This data is generated by gcc during compilation and doesn't change
 *            * at run-time with the exception of the next pointer.
 *             */
struct gcov_info {
        unsigned int version;
            struct gcov_info *next;
                unsigned int stamp;
                    const char *filename;
                        void (*merge[GCOV_COUNTERS])(gcov_type *, unsigned int);
                            unsigned int n_functions;
                                struct gcov_fn_info **functions;
};

/*
 *  * Determine whether a counter is active. Doesn't change at run-time.
 *   */
static int counter_active(struct gcov_info *info, unsigned int type)
{
        return info->merge[type] ? 1 : 0;
}

/**
 *  * gcov_info_filename - return info filename
 *   * @info: profiling data set
 *    */
const char *gcov_info_filename(struct gcov_info *info)
{
        return info->filename;
}

/**
 *  * store_gcov_u32 - store 32 bit number in gcov format to buffer
 *   * @buffer: target buffer or NULL
 *    * @off: offset into the buffer
 *     * @v: value to be stored
 *      *
 *       * Number format defined by gcc: numbers are recorded in the 32 bit
 *        * unsigned binary form of the endianness of the machine generating the
 *         * file. Returns the number of bytes stored. If @buffer is %NULL, doesn't
 *          * store anything.
 *           */
static air_sz_t store_gcov_u32(void *buffer, air_sz_t off, u32 v)
{
        u32 *data;

            if (buffer) {
                        data = buffer + off;
                                *data = v;
                                    }

                return sizeof(*data);
}

/**
 *  * store_gcov_u64 - store 64 bit number in gcov format to buffer
 *   * @buffer: target buffer or NULL
 *    * @off: offset into the buffer
 *     * @v: value to be stored
 *      *
 *       * Number format defined by gcc: numbers are recorded in the 32 bit
 *        * unsigned binary form of the endianness of the machine generating the
 *         * file. 64 bit numbers are stored as two 32 bit numbers, the low part
 *          * first. Returns the number of bytes stored. If @buffer is %NULL, doesn't store
 *           * anything.
 *            */
static air_sz_t store_gcov_u64(void *buffer, air_sz_t off, u64 v)
{
        u32 *data;

            if (buffer) {
                        data = buffer + off;

                                data[0] = (v & 0xffffffffUL);
                                        data[1] = (v >> 32);
                                            }

                return sizeof(*data) * 2;
}

/**
 *  * convert_to_gcda - convert profiling data set to gcda file format
 *   * @buffer: the buffer to store file data or %NULL if no data should be stored
 *    * @info: profiling data set to be converted
 *     *
 *      * Returns the number of bytes that were/would have been stored into the buffer.
 *       */
unsigned int convert_to_gcda(char *buffer, struct gcov_info *info)
{
        struct gcov_fn_info *fi_ptr;
            struct gcov_ctr_info *ci_ptr;
                unsigned int fi_idx;
                    unsigned int ct_idx;
                        unsigned int cv_idx;
                            air_sz_t pos = 0;

                                /* File header. */
                                pos += store_gcov_u32(buffer, pos, GCOV_DATA_MAGIC);
                                    pos += store_gcov_u32(buffer, pos, info->version);
                                        pos += store_gcov_u32(buffer, pos, info->stamp);

                                            for (fi_idx = 0; fi_idx < info->n_functions; fi_idx++) {
                                                        fi_ptr = info->functions[fi_idx];

                                                                /* Function record. */
                                                                pos += store_gcov_u32(buffer, pos, GCOV_TAG_FUNCTION);
                                                                        pos += store_gcov_u32(buffer, pos, GCOV_TAG_FUNCTION_LENGTH);
                                                                                pos += store_gcov_u32(buffer, pos, fi_ptr->ident);
                                                                                        pos += store_gcov_u32(buffer, pos, fi_ptr->lineno_checksum);
                                                                                                pos += store_gcov_u32(buffer, pos, fi_ptr->cfg_checksum);

                                                                                                        ci_ptr = fi_ptr->ctrs;

                                                                                                                for (ct_idx = 0; ct_idx < GCOV_COUNTERS; ct_idx++) {
                                                                                                                                if (!counter_active(info, ct_idx))
                                                                                                                                                    continue;

                                                                                                                                            /* Counter record. */
                                                                                                                                            pos += store_gcov_u32(buffer, pos,
                                                                                                                                                                              GCOV_TAG_FOR_COUNTER(ct_idx));
                                                                                                                                                        pos += store_gcov_u32(buffer, pos, ci_ptr->num * 2);

                                                                                                                                                                    for (cv_idx = 0; cv_idx < ci_ptr->num; cv_idx++) {
                                                                                                                                                                                        pos += store_gcov_u64(buffer, pos,
                                                                                                                                                                                                                              ci_ptr->values[cv_idx]);
                                                                                                                                                                                                    }

                                                                                                                                                                                ci_ptr++;
                                                                                                                                                                                        }
                                                                                                                    }

                                                return pos;
}

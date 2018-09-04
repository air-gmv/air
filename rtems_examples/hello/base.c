/*This code is based on the GCOV-related code of the Linux kernel (kept under
         * "kernel/gcov"). It basically uses the convert_to_gcda function to generate
*  * the .gcda files information upon application completion, and dump it on the
*   * host filesystem via GDB scripting.
*    *
*     * Original Linux banner follows below - but note that the Linux guys have
*      * nothing to do with these modifications, so blame me (and contact me)
    *       * if something goes wrong.
    *        *
    *         * Thanassis Tsiodras
    *          * Real-time Embedded Software Engineer 
    *           * System, Software and Technology Department
    *            * European Space Agency
    *             *
    *              * e-mail: ttsiodras@gmail.com / Thanassis.Tsiodras@esa.int (work)
    *               *
    *                */


    /*
     *  *  This code maintains a list of active profiling data structures.
     *   *
     *    *    Copyright IBM Corp. 2009
     *     *    Author(s): Peter Oberparleiter <oberpar@linux.vnet.ibm.com>
     *      *
     *       *    Uses gcc-internal data definitions.
     *        *    Based on the gcov-kernel patch by:
     *         *         Hubertus Franke <frankeh@us.ibm.com>
     *          *        Nigel Hinds <nhinds@us.ibm.com>
     *           *       Rajan Ravindran <rajancr@us.ibm.com>
     *            *      Peter Oberparleiter <oberpar@linux.vnet.ibm.com>
     *             *         Paul Larson
     *              */
#define COVERAGE_ENABLED

#include "gcov_public.h"
#include "gcov.h"
#include <rtems.h>
//#include <stdlib.h>
typedef struct tagGcovInfo {
         struct gcov_info *info;
         struct tagGcovInfo *next;
} GcovInfo;
GcovInfo *headGcov = NULL;

/*
 *  * __gcov_init is called by gcc-generated constructor code for each object
 *   * file compiled with -fprofile-arcs.
 *    */
void __gcov_init(struct gcov_info *info)
{
        printk("gcov init\n");
        printk("__gcov_init called for %s!\n", gcov_info_filename(info));
 //       GcovInfo *newHead = malloc(sizeof(GcovInfo));
       GcovInfo *newHead;

        if (!newHead) {
            printk("Out of memory!\n");
            return;
        }
        newHead->info = info;
        newHead->next = headGcov;
        headGcov = newHead;
        printk("headGcov %d headgcov.info %d\n", headGcov, headGcov->info );
}

void __gcov_exit()
{
        GcovInfo *tmp = headGcov;
        printk("on gcov_exit %d %d\n", tmp, tmp->info);
        while(tmp) {
               // char *buffer;
                unsigned bytesNeeded = convert_to_gcda(NULL, tmp->info);
                char buffer[bytesNeeded];
            
                convert_to_gcda(buffer, tmp->info);
                printk("Emitting %6d bytes for %s\n", bytesNeeded, gcov_info_filename(tmp->info));
                printk("for the bp\n");
                tmp = tmp->next;
        }
}

void __gcov_merge_add(gcov_type *counters, unsigned int n_counters)
{
        printk("__gcov_merge_add isn't called, right? Right? RIGHT?");
        return;
}

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

#include "gcov_public.h"
#include <rtems.h>
#include <pprintf.h>

#include "malloc.h"

typedef struct tagGcovInfo {
    struct gcov_info *info;
    struct tagGcovInfo *next;
} GcovInfo;

struct GcovInfo *headGcov = NULL;

void get_filename(char * original, char * new){
    int i =0;
    int j=0;
    
    //original filename format .../../[filename].gcda
    //new filename format [filename].gcda
    
    while(original[i]!='\0')
        i++;
    
    if(i<5)
        return;

    i=i-6;
    while(original[i]!='/')
        i--;
    i++;
    while(original[i]!='\0'){
        new[j]=original[i];
        i++;
        j++;
    }
    new[j]='\0';
    for(i=0; i<j+1;i++)
        original[i]=new[i];
    return;
}

/*
 *  * __gcov_init is called by gcc-generated constructor code for each object
 *   * file compiled with -fprofile-arcs.
 *    */
void __gcov_init(struct gcov_info *info)
{
        char name[24];
        get_filename(gcov_info_filename(info), name);
     
        pprintf("__gcov_init called for %s!\n", gcov_info_filename(info)); 
        GcovInfo *newHead = malloc(sizeof(GcovInfo));
        if (!newHead) {
            pprintf("Out of memory!\n");
            return;
        }
        newHead->info=info;
        newHead->next = headGcov;
        headGcov = newHead;
}

void __gcov_exit()
{
        GcovInfo *tmp = headGcov;
        pprintf("head gcov %d %d\n", headGcov, tmp->next);
        pprintf("on gcov_exit for %s\n", gcov_info_filename(tmp->info));
        while(tmp) {
                unsigned bytesNeeded = convert_to_gcda(NULL, tmp->info);
                char buffer[bytesNeeded];
                if (!buffer) {
                    pprintf("Out of memory!");
                    return;
                }

                convert_to_gcda(buffer, tmp->info);
                pprintf("Emitting %6d bytes for %s\n", bytesNeeded, gcov_info_filename(tmp->info));
                tmp = tmp->next;
        
        }
}


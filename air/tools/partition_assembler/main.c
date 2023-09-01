/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 *  @file main.c
 *  @author pfnf
 *  @brief AIR Partition Assembler
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <common.h>
#include <libelf.h>
#include <lzss.h>

#define HEADER                                                                                                         \
    "--------------------------------------------------------------------------\n"                                     \
    "  AIR Partition Assembler v1.0 (alpha)                                    \n"                                     \
    "--------------------------------------------------------------------------\n"

#define VALID_ADDR(addr, base_origin, base_length) ((addr) >= (base_origin) && (addr) < ((base_origin) + (base_length)))

#define VALID_ADDR_RANGE(origin, length, base_origin, base_length)                                                     \
    (VALID_ADDR((origin), (base_origin), (base_length)) && (((origin) + (length)) <= ((base_origin) + (base_length))))

/**
 * @brief Generates tool outpot
 * @param p_elfs Partition compress ELFs
 * @param p_count Number of partitions
 */
extern void generate_output(p_data *p_elfs, uint32_t p_count);

/**
 * @brief Reads a file into memory
 * @param filename Filename
 * @param[out] size Size of the file
 * @return File memory block pointer
 */
char *read_file(const char *filename, size_t *size)
{

    int fd;
    char *file;
    size_t count;

    /* open file */
    if ((fd = open(filename, O_RDWR, 0777)) == -1)
    {
        errx(EX_NOINPUT, " open() failed - %s", strerror(errno));
    }

    /* get file size */
    if ((*size = count = lseek(fd, 0L, SEEK_END)) == -1)
    {
        errx(EX_DATAERR, " lseek() failed - %s", strerror(errno));
    }

    /* alloc memory to hold the file */
    file = (char *)safe_malloc(*size * sizeof(char));

    /* Read file into memory */
    lseek(fd, 0, 0);
    while (count > 0)
    {
        char *offset = file + *size - count;
        int i = read(fd, offset, count);
        count -= i;
    }

    /* Close file */
    close(fd);
    return file;
}

/**
 * @brief Tool entry point
 * @param argc number of input arguments
 * @param argv input arguments
 */
int main(int argc, char **argv)
{

    /* print header */
    (void)printf(HEADER "\n");

    /* check input arguments */
    if (argc == 1 || (argc - 1) % 3 != 0)
    {

        errx(EX_USAGE,
             "invalid number of arguments...\n"
             "usage: %s [<ELF 0> <ORIGIN> <LENGTH>] ...\n",
             argv[0]);
    }

    int errors = 0;

    /* load into memory */
    (void)printf(" :: Loading partitions :: \n\n");
    uint32_t i, j;
    uint32_t p_count = (argc - 1) / 3;
    p_data *p_elfs = (p_data *)safe_malloc(sizeof(p_data) * p_count);
    for (i = 0; i < p_count; ++i)
    {

        /* get partition data */ // p0.exe
        p_elfs[i].filename = argv[i * 3 + 1];
        p_elfs[i].mem_origin = strtoll(argv[i * 3 + 2], NULL, 16); // 0x41000000
        p_elfs[i].mem_length = strtoll(argv[i * 3 + 3], NULL, 16); // 0x00100000
        p_elfs[i].elf_data = read_file(p_elfs[i].filename, &p_elfs[i].size);

        printf("    Partition %02i - %06i KB - Memory: [0x%08x:0x%08x]\n", i, (int)(p_elfs[i].size / 1024),
               (int)(p_elfs[i].mem_origin), (int)(p_elfs[i].mem_origin + p_elfs[i].mem_length - 1));
    }

    /* process partition elf */
    (void)printf("\n :: Compressing partitions ELFs :: \n");
    for (i = 0; i < p_count; ++i)
    {

        char *elf = p_elfs[i].elf_data;

        /* extract ELF required headers */
        Elf32_Ehdr *Ehdr = (Elf32_Ehdr *)elf;

        /* get the machine and file endianess */
        bool m_is_big = elf_machine_is_bigendian();
        bool t_is_big = Ehdr->e_ident[5] == 2 ? true : false;
        uint16_t phnum = elf_correct_uint16(Ehdr->e_phnum, t_is_big, m_is_big);
        uint32_t phoff = elf_correct_uint32(Ehdr->e_phoff, t_is_big, m_is_big);
        Elf32_Phdr *Phdr = (Elf32_Phdr *)(elf + phoff);

        /* get ELF entry point and check if it is valid */
        p_elfs[i].entry = elf_correct_uint32(Ehdr->e_entry, t_is_big, m_is_big);
        if (!VALID_ADDR(p_elfs[i].entry, p_elfs[i].mem_origin, p_elfs[i].mem_length))
        {

            printf("\n    Partition %02i entry point is outside the "
                   "defined memory region...\n\n",
                   i);
            ++errors;
            continue;
        }

        printf("\n    Partition %02i (entry point: 0x%08x)\n\n", i,
               elf_correct_uint32(Ehdr->e_entry, t_is_big, m_is_big));

        /* get usable program sections */
        uint32_t ps_count = 0;
        size_t wc_lzss_size = 0;
        for (j = 0; j < phnum; ++j)
        {

            /* get the program header values in the current machine format */
            uint64_t type, memsz, filesz;
            type = elf_correct_uint32(Phdr[j].p_type, t_is_big, m_is_big);
            memsz = elf_correct_uint32(Phdr[j].p_memsz, t_is_big, m_is_big);
            filesz = elf_correct_uint32(Phdr[j].p_filesz, t_is_big, m_is_big);

            /* check if the program segment is usable */
            if (1 == type && filesz <= memsz && memsz > 0)
            {
                wc_lzss_size += memsz + memsz / 2 + 256;
                ++ps_count;
            }
        }

        /* allocate space for the program segments information */
        p_elfs[i].segments = (char *)safe_malloc(sizeof(char) * wc_lzss_size);
        p_elfs[i].offsets = (uint32_t *)safe_malloc(sizeof(uint32_t) * ps_count);
        p_elfs[i].addrs = (uint32_t *)safe_malloc(sizeof(uint32_t) * ps_count);
        p_elfs[i].sizes = (uint32_t *)safe_malloc(sizeof(uint32_t) * ps_count);
        p_elfs[i].count = ps_count;

        /* compress partitions */
        uint32_t prg_count = 0;
        uint32_t ct_size = 0;
        char *compressed_offset = p_elfs[i].segments;
        for (j = 0; j < phnum; ++j)
        {

            /* get the program header values in the current machine format */
            uint64_t type, memsz, filesz, v_addr, offset;
            type = elf_correct_uint32(Phdr[j].p_type, t_is_big, m_is_big);
            memsz = elf_correct_uint32(Phdr[j].p_memsz, t_is_big, m_is_big);
            filesz = elf_correct_uint32(Phdr[j].p_filesz, t_is_big, m_is_big);
            v_addr = elf_correct_uint32(Phdr[j].p_vaddr, t_is_big, m_is_big);
            offset = elf_correct_uint32(Phdr[j].p_offset, t_is_big, m_is_big);

            /* check if the program segment is usable */
            if (1 == type && filesz <= memsz && memsz > 0)
            {

                /* check if segment is inside the memory region */
                if (!VALID_ADDR_RANGE(v_addr, memsz, p_elfs[i].mem_origin, p_elfs[i].mem_length))
                {

                    printf("        Partition %02i, segment %02i is outside "
                           "the defined memory region...\n",
                           i, j);
                    ++errors;
                    continue;
                }

                /* extract segment into new buffer */
                char *segment = (char *)safe_malloc(sizeof(char) * memsz);
                memset(segment, '\0', memsz);          /* add 0 padding */
                memcpy(segment, elf + offset, filesz); /* copy segment  */

                p_elfs[i].sizes[prg_count] = memsz;
                p_elfs[i].addrs[prg_count] = v_addr - p_elfs[i].mem_origin;
                p_elfs[i].offsets[prg_count] = ct_size;

                /* compress the segment */
                uint32_t c_size = lzss_compress(segment, compressed_offset, memsz);
                compressed_offset += c_size;
                ct_size += c_size;
                ++prg_count;

                printf("        Program segment %02i:  %06i ->  %06i (%6.3f)\n", j, (int)(memsz / 1024),
                       (int)(c_size / 1024), (float)(memsz / (float)c_size));

                /* free segment */
                free(segment);
            }
        }

        p_elfs[i].c_size = ct_size;

        if (ct_size > 0)
        {
            printf("         Total Compression:  %06i ->  %06i (%6.3f)\n", (int)(p_elfs[i].size / 1024),
                   (int)(ct_size / 1024), (float)(p_elfs[i].size / (float)ct_size));
        }
        else
        {
            (void)printf("        Total Compression:     ------\n");
        }
    }

    (void)printf("\n :: Generating output file :: \n\n");

    /* check if the output can be generated */
    if (errors == 0)
    {

        generate_output(p_elfs, p_count);
        (void)printf("    Done!\n\n");
    }
    else
    {

        fprintf(stderr, "    Errors occurred while processing the partitions ELFs\n\n");
    }

    /* free all used memory */
    for (i = 0; i < p_count; ++i)
    {
        free(p_elfs[i].elf_data);
        free(p_elfs[i].segments);
    }
    free(p_elfs);

    /* finish execution */
    exit(errors == 0 ? 0 : -1);
    return 0;
}

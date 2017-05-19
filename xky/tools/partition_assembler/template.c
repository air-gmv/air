/**
 * @file
 * @author pfnf
 * @brief
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <common.h>

void ouput_program_segment_data(
        FILE *fd,
        uint32_t idx,
        uint32_t *addr,
        uint32_t *sizes,
        uint32_t *offsets,
        uint32_t size) {

    uint32_t i;

    fprintf(fd,
            "/**\n"\
            " * @brief Partition %i - ELF Program Segments Information\n" \
            " */\n" \
            "pmk_segment_t usr_partition%i_segments[%i] = \\\n" \
            "{\n",
            idx, idx, size);
    for (i = 0; i < size; ++i) {
        fprintf(fd, "    {\n" \
                    "        .addr   = (void *)0x%08x,\n" \
                    "        .size   = 0x%08x,\n" \
                    "        .offset = 0x%08x,\n" \
                    "    }%s\n",
                    addr[i], sizes[i], offsets[i], i == size - 1 ? "" : ",");
    }
    fprintf(fd, "};\n\n");
}

void output_partition_data(FILE *fd, uint32_t idx, char *data, uint32_t size) {

    uint32_t i;

    fprintf(fd,
            "/**\n" \
            " * @brief Partition %i - Compressed ELF Program Segments\n" \
            " */\n" \
            "xky_u8_t usr_partition%i_data[%i] = \\\n" \
            "{\n    ",
            idx, idx, size);
    for (i = 0; i < size; ++i) {

        fprintf(fd, "0x%02x%s", (uint8_t)data[i], i < size - 1 ? ", " : "");
        if ((i + 1) % 12 == 0 && i < size - 1) {
            fprintf(fd, "\n    ");
        }
    }
    fprintf(fd, "\n};\n\n");
}

void generate_output(p_data *p_elfs, uint32_t p_count) {

    uint32_t i;

    FILE *fd = fopen("usr_partitions_data.c", "w");




    /* file header */
    fprintf(fd, "\n\n");
    fprintf(fd, "#include <loader.h>\n");
    fprintf(fd, "#include <usr_config.h>\n");


    /* external definitions */
    fprintf(fd, "\n");
    for (i = 0; i < p_count; ++i) {

        fprintf(
                fd,
                "extern xky_u8_t usr_partition%i_data[%i];\n",
                i, (int)(p_elfs[i].c_size));
    }
    for (i = 0; i < p_count; ++i) {

        fprintf(fd,
                "extern pmk_segment_t usr_partition%i_segments[%i];\n",
                i, p_elfs[i].count);
    }
    fprintf(fd, "\n");


    fprintf(fd, "/**\n"\
                " * @brief Partitions ELF segments\n"
                " */\n");
    fprintf(fd, "pmk_elf_t usr_partitions_data[USR_PARTITIONS] = \\\n{\n");
    for (i = 0; i < p_count; ++i) {
        fprintf(fd,
                "    {\n" \
                "        .entry = (void *)0x%08x,\n" \
                "        .count = %i,\n" \
                "        .segments = &usr_partition%i_segments[0],\n" \
                "        .data     = &usr_partition%i_data[0],\n"
                "    }%s\n",
                p_elfs[i].entry,
                p_elfs[i].count,
                i,
                i,
                i == p_count - 1 ? "" : ",");
    }
    fprintf(fd, "};\n\n");

    /* program segments */
    for (i = 0; i < p_count; ++i) {

        ouput_program_segment_data(
                fd,
                i,
                p_elfs[i].addrs,
                p_elfs[i].sizes,
                p_elfs[i].offsets,
                p_elfs[i].count);
    }

    for (i = 0; i < p_count; ++i) {

        output_partition_data(
                fd,
                i,
                p_elfs[i].segments,
                p_elfs[i].c_size);
    }

    fclose(fd);
}

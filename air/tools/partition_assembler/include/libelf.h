
#include <stdint.h>
#include <stdbool.h>


#ifndef _ELF_H_
#define _ELF_H_

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;

#define EI_NIDENT (16)

typedef struct
{
    unsigned char e_ident[EI_NIDENT];           /* Magic number and other info */
    Elf32_Half e_type;                          /* Object file type */
    Elf32_Half e_machine;                       /* Architecture */
    Elf32_Word e_version;                       /* Object file version */
    Elf32_Addr e_entry;                         /* Entry point virtual address */
    Elf32_Off e_phoff;                          /* Program header table file offset */
    Elf32_Off e_shoff;                          /* Section header table file offset */
    Elf32_Word e_flags;                         /* Processor-specific flags */
    Elf32_Half e_ehsize;                        /* ELF header size in bytes */
    Elf32_Half e_phentsize;                     /* Program header table entry size */
    Elf32_Half e_phnum;                         /* Program header table entry count */
    Elf32_Half e_shentsize;                     /* Section header table entry size */
    Elf32_Half e_shnum;                         /* Section header table entry count */
    Elf32_Half e_shstrndx;                      /* Section header string table index */

} Elf32_Ehdr;

/* Section Header */
typedef struct
{
    Elf32_Word  sh_name;                        /* Section name of the section (string table index)*/
    Elf32_Word  sh_type;                        /* Section contents and semantics */
    Elf32_Word  sh_flags;                       /* Miscellaneous attribute flags */
    Elf32_Addr  sh_addr;                        /* Section first byte addr */
    Elf32_Off   sh_offset;                      /* Offset of the first byte counting from the beggining of the file*/
    Elf32_Word  sh_size;                        /* Section size in bytes */
    Elf32_Word  sh_link;                        /* Section header table index link */
    Elf32_Word  sh_info;                        /* Extra info */
    Elf32_Word  sh_addralign;                   /* Specific alignment constraints */
    Elf32_Word  sh_entsize;                     /* Entry size (if fixed) */
} Elf32_Shdr;

/* Program segment header.  */
typedef struct
{
    Elf32_Word p_type;                          /* Segment type */
    Elf32_Off p_offset;                         /* Segment file offset */
    Elf32_Addr p_vaddr;                         /* Segment virtual address */
    Elf32_Addr p_paddr;                         /* Segment physical address */
    Elf32_Word p_filesz;                        /* Segment size in file */
    Elf32_Word p_memsz;                         /* Segment size in memory */
    Elf32_Word p_flags;                         /* Segment flags */
    Elf32_Word p_align;                         /* Segment alignment */
} Elf32_Phdr;

bool elf_machine_is_bigendian();
unsigned char elf_check(unsigned char *in_buffer);
uint16_t elf_correct_uint16(uint16_t value, bool source_big_endian, bool target_big_endian);
uint32_t elf_correct_uint32(uint32_t value, bool source_big_endian, bool target_big_endian);

#endif /* !_ELF_H_ */

/*  linkcmds
 *
 *  $Id: linkcmds,v 1.8 2008/08/08 15:54:59 joel Exp $
 */

OUTPUT_ARCH(sparc)
__DYNAMIC  =  0;

/*
 * The memory map looks like this:
 * +--------------------+ <- low memory
 * | .text              |
 * |        etext       |
 * |        ctor list   | the ctor and dtor lists are for
 * |        dtor list   | C++ support
 * |        _endtext    |
 * +--------------------+
 * | .data              | initialized data goes here
 * |        _sdata      |
 * |        _edata      |
 * +--------------------+
 * | .bss               |
 * |        __bss_start | start of bss, cleared by crt0
 * |        _end        | start of heap, used by sbrk()
 * +--------------------+
 * |    heap space      |
 * |        _ENDHEAP    |
 * |    stack space     | 
 * |        __stack     | top of stack
 * +--------------------+ <- high memory
 */


/*
 * User modifiable values:
 *
 * _CLOCK_SPEED               in Mhz (used to program the counter/timers)
 *
 * _PROM_SIZE                 size of PROM (permissible values are 128K, 256K,
 *                               512K, 1M, 2M, 4M, 8M and 16M)
 * _RAM_SIZE                  size of RAM (permissible values are 256K, 512K,
 *                               1M, 2M, 4M, 8M, 16M, and 32M)
 *
 */

/* Default values, can be overridden */

INCLUDE linkcmds.inc

_PROM_SIZE = 2M;
_RAM_SIZE = 4M;

_RAM_START = ORIGIN(ram);
_RAM_END = _RAM_START + _RAM_SIZE;

_PROM_START = ORIGIN(rom);
_PROM_END = _PROM_START + _PROM_SIZE;

/*
 *  Alternate names without leading _.
 */

PROM_START = _PROM_START;
PROM_SIZE = _PROM_SIZE;
PROM_END = _PROM_END;

RAM_START = _RAM_START;
RAM_SIZE = _RAM_SIZE;
RAM_END = _RAM_END;

/*
 *  Base address of the on-CPU peripherals
 */

_LEON_REG = 0x80000000;
LEON_REG = 0x80000000;

ENTRY(bare_start)
/*
 * stick everything in ram (of course)
 */
SECTIONS
{
  
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    text_start = .;
    _text_start = .;
    *(.text*)
    . = ALIGN (16);

    /*
     * Special FreeBSD sysctl sections.
     */
    . = ALIGN (16);
    __start_set_sysctl_set = .;
    *(set_sysctl_*);
    __stop_set_sysctl_set = ABSOLUTE(.);
    *(set_domain_*);
    *(set_pseudo_*);

    *(.eh_frame)
    . = ALIGN (16);

    *(.gnu.linkonce.t*)

    /*
     * C++ constructors
     */
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))

    _rodata_start = . ;
    *(.rodata*)
    *(.gnu.linkonce.r*)
    _erodata = ALIGN( 0x10 ) ;

    etext = ALIGN(0x10);
    _etext = .;
    *(.init)
    *(.fini)
    *(.lit)
    *(.shdata)
    . = ALIGN (16);
    _endtext = .;
  } > ram
  .rela.dyn       :
    {
      *(.rela.init)
      *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
      *(.rela.fini)
      *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
      *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
      *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
      *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
      *(.rela.ctors)
      *(.rela.dtors)
      *(.rela.got)
      *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
    } >ram
  .data :
  {
    data_start = .;
    _data_start = .;
    _sdata = . ;
    *(.data*)
    *(.gnu.linkonce.d*)
    *(.gcc_except_table*)
    . = ALIGN(0x10);
    edata = .;
    _edata = .;
  } > ram
  .dynamic    : { *(.dynamic)   } >ram
  .jcr        : { *(.jcr)       } >ram
  .got        : { *(.got)       } >ram
  .plt        : { *(.plt)       } >ram
  .hash       : { *(.hash)      } >ram
  .dynrel     : { *(.dynrel)    } >ram
  .dynsym     : { *(.dynsym)    } >ram
  .dynstr     : { *(.dynstr)    } >ram
  .hash       : { *(.hash)      } >ram
  .shbss :
  {
    *(.shbss)
  } > ram
  .bss :
  {
    __bss_start = ALIGN(0x8);
    _bss_start = .;
    bss_start = .;
    *(.bss .bss* .gnu.linkonce.b*)
    *(COMMON)
    end = .;
    _end = ALIGN(0x8);
    __end = ALIGN(0x8);
  } > ram
  .stab . (NOLOAD) :
  {
    [ .stab ]
  }
  .stabstr . (NOLOAD) :
  {
    [ .stabstr ]
  }
}
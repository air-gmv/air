/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: build tools
 * Purpose  : Linking script
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/control/config/linkcmds.t,v $
 * $Branch:  $
 * $Revision: 1.3 $
 * $CommitId: 7044e674d2e6cfa $
 * $Author: jaac $
 * $Date: 2011/09/07 10:53:35 $
 * ===========================================================================*/

/*  linkcmds
 *
 *  $Id: linkcmds.t,v 1.3 2011/09/07 10:53:35 jaac Exp $
 */

OUTPUT_ARCH(sparc)
__DYNAMIC  =  0;

/* SKYSOFT *********************************************** (begin) **/

/* Memory configuration		*/
<% 	ram_start_hex	= hex(ram_start) 			
	ram_size_hex 	= hex(ram_size) 			
	rom_start_hex 	= hex(rom_start) 			
	rom_size_hex 	= hex(rom_size) 			
	max_part_size_hex= hex(max_part_ram_size) 	%>

/* Maximum RAM 		 			 */
air_memmap_RAM_SIZE				= ${ram_size_hex};
air_memmap_ROM_SIZE				= ${rom_size_hex};
/* Maximum partition size 		 */
air_memmap_PARTITION_MAX_RAM	= ${max_part_size_hex};

/* Constants for MMU page tables */
air_memmap_MMU_CONTEXT_TABLE_ENTRIES 	= (256);
air_memmap_MMU_CONTEXT_TABLE_SIZE		= (air_memmap_MMU_CONTEXT_TABLE_ENTRIES * 4);
air_memmap_MMU_LEVEL1_TABLE_ENTRIES  	= (256);
air_memmap_MMU_LEVEL1_TABLE_SIZE		= (air_memmap_MMU_LEVEL1_TABLE_ENTRIES * 4);
air_memmap_MMU_LEVEL2_TABLE_ENTRIES  	= (64);
air_memmap_MMU_LEVEL2_TABLE_SIZE		= (air_memmap_MMU_LEVEL2_TABLE_ENTRIES * 4);
air_memmap_MMU_LEVEL3_TABLE_ENTRIES  	= (64);
air_memmap_MMU_LEVEL3_TABLE_SIZE		= (air_memmap_MMU_LEVEL3_TABLE_ENTRIES * 4);


/* Level 1 table address space : 16 MB 	*/
air_memmap_MMU_LEVEL1_REGION_SIZE	  = (1<<24);
/* Level 2 table address space : 256 KB 	*/
air_memmap_MMU_LEVEL2_REGION_SIZE	  = (1<<18);
/* Level 3 table address space :   4 KB 	*/
air_memmap_MMU_LEVEL3_REGION_SIZE	  = (1<<12);

/* WARNING: division sign must be surrounded by spaces, else the linker will 
 *			consider both operands plus division sign as just one big symbol	*/

/* Total number of Level 1 entries per partition	*/
air_memmap_MMU_PARTITION_MAX_L1_ENTRIES 	= ((air_memmap_PARTITION_MAX_RAM + air_memmap_MMU_LEVEL1_REGION_SIZE - 1 ) / air_memmap_MMU_LEVEL1_REGION_SIZE); 

/* Total number of Level 2 entries and tables per partition	*/
air_memmap_MMU_PARTITION_MAX_L2_ENTRIES 	= (air_memmap_PARTITION_MAX_RAM / air_memmap_MMU_LEVEL2_REGION_SIZE);
/* Table per partition (rounded up) */
air_memmap_MMU_PARTITION_MAX_L2_TABLES  	= ((air_memmap_MMU_PARTITION_MAX_L2_ENTRIES + air_memmap_MMU_LEVEL2_TABLE_ENTRIES - 1) / air_memmap_MMU_LEVEL2_TABLE_ENTRIES);
/* Entries per partition (air_memmap_MMU_LEVEL2_TABLE_ENTRIES)  */
air_memmap_MMU_PARTITION_L2_ENTRIES  		= (air_memmap_MMU_PARTITION_MAX_L2_TABLES * air_memmap_MMU_LEVEL2_TABLE_SIZE);


/* Total number of Level 3 pages and tables per partition 		*/
air_memmap_MMU_PARTITION_MAX_L3_ENTRIES 	= (air_memmap_PARTITION_MAX_RAM / air_memmap_MMU_LEVEL3_REGION_SIZE);
air_memmap_MMU_PARTITION_MAX_L3_TABLES	 	= ((air_memmap_MMU_PARTITION_MAX_L3_ENTRIES + air_memmap_MMU_LEVEL3_TABLE_ENTRIES - 1) / air_memmap_MMU_LEVEL3_TABLE_ENTRIES);
/* Entries per partition (air_memmap_MMU_LEVEL3_TABLE_ENTRIES)  */
air_memmap_MMU_PARTITION_L3_ENTRIES  		= (air_memmap_MMU_PARTITION_MAX_L3_TABLES * air_memmap_MMU_LEVEL3_TABLE_SIZE);

/* SKYSOFT *********************************************** (end) **/

air_memmap_RAM_START 	= ${ram_start_hex};
air_memmap_RAM_END 	 	= air_memmap_RAM_START + air_memmap_RAM_SIZE;

air_memmap_PROM_START 	= ${rom_start_hex};
air_memmap_PROM_END 	= air_memmap_PROM_START + air_memmap_ROM_SIZE;

/*
 *  Alternate names without leading _.
 */
/*
 *  Base address of the on-CPU peripherals
 */

_LEON_REG 		= 0x80000000;
LEON_REG		= 0x80000000;

/* these are the maximum values */

MEMORY
{
  rom     : ORIGIN = ${rom_start_hex}, LENGTH = ${rom_size_hex}
  ram     : ORIGIN = ${ram_start_hex}, LENGTH = ${ram_size_hex}
}

/*
 * SPARC monitor assumes this is present to provide proper RTEMS awareness.
 */
EXTERN(rtems_get_version_string);

/*
 * stick everything in ram (of course)
 */

SECTIONS
{

	air_memmap_memory_start = .;
	/* ***********************************************************************	*/
	/* MMU PAGING TABLES and SHARED MEMORY ************************************	*/
	/* ***********************************************************************	*/	
	.paging :
	{			
		. = ALIGN(0x1000);
			/* PMK */
			
		air_memmap_MMU_PagingAreaStart = .;
		/* Determine number of bytes necessary for Context table */
		air_memmap_MMU_ContextTableStart = .;
		. +=  (air_memmap_MMU_CONTEXT_TABLE_SIZE);		
		/* Determine number of bytes necessary for Level 1 tables */
		air_memmap_MMU_Level1TableStart = .;
		% for p in range(0, number_of_partitions + 1) :
			. +=  (air_memmap_MMU_LEVEL1_TABLE_SIZE);		
		% endfor
		/* Determine number of bytes necessary for Level 2 tables */
		/* repeat as many times as partition number + 1 */
		air_memmap_MMU_Level2TablesStart = .;
		% for p in range(0, number_of_partitions + 1) :
			. +=  (air_memmap_MMU_PARTITION_L2_ENTRIES);		
		% endfor
	
		/* Determine number of bytes necessary for Level 3 page tables (multiply by 4 = sizeof(word)  */
		/* repeat as many times as partition number + 1 */
		. = ALIGN(0x1000);
		air_memmap_MMU_Level3TablesStart = .;	
		% for p in range(0, number_of_partitions + 1) :
			. +=  (air_memmap_MMU_PARTITION_L3_ENTRIES);		
		% endfor		

		. = ALIGN(0x1000);
		air_memmap_MMU_PagingAreaEnd = .;	
		
		/* PMK/P0 Shared area zone */
		air_memmap_PMK_SharedAreaStart	= .;	
		/* Partition Context storage area */
		. += 0x12000;
		air_memmap_PMK_PartitionDynMemMap	= .;	
		. += 0x1000;		
		air_memmap_PMK_MMU_InterruptStack	= .;	
		. += 0x1000;
		air_memmap_PMK_SharedAreaEnd	= .;
		
		air_memmap_PMK_syscall_perm		= .;
		o-optimize/usr_syscall.o(.data)
		
	} > ram
		
	/* ***********************************************************************	*/
	/* AIR/PMK DATA **********************************************************	*/
	/* ***********************************************************************	*/
	.air_data :
	{
		. = ALIGN(0x1000);
		air_memmap_data_start = .;
	
		% for plib in pmk_libs :
		${plib}(.data)
		% endfor
	
		. = ALIGN(0x1000);
		air_memmap_data_end = .;
	} >ram
	
	/* ***********************************************************************	*/
	/* AIR/PMK BSS & COMMON***************************************************	*/
	/* ***********************************************************************	*/
	.air_bss :
	{
		. = ALIGN(0x1000);
		air_memmap_bss_start = .;
		
		% for plib in pmk_libs :	
		${plib}(.bss COMMON)
		% endfor
		
		. = ALIGN(0x1000);		
		air_memmap_bss_end = .;		
	} >ram
	
	/* ***********************************************************************	*/
	/* PAL TEXT **************************************************************	*/
	/* ***********************************************************************	*/
	.pal_text :
	{
		CREATE_OBJECT_SYMBOLS
		. = ALIGN(0x1000);
		air_memmap_pal_code_start = .;
		
		% for po in pal_objs :
		${po}(.text .rodata*)		
		% endfor
		
		air_memmap_pal_code_end = .;
		. = ALIGN(0x1000);
	} >ram
	
	/* ***********************************************************************	*/
	/* PAL DATA **************************************************************	*/
	/* ***********************************************************************	*/
	.pal_data :
	{
		CREATE_OBJECT_SYMBOLS
		. = ALIGN(0x1000);
		air_memmap_pal_data_start = .;
		
		% for po in pal_objs :
		${po}(.data)		
		% endfor
		
		air_memmap_pal_data_end = .;
		. = ALIGN(0x1000);
	} >ram	
	
	/* ***********************************************************************	*/
	/* PAL BSS **************************************************************	*/
	/* ***********************************************************************	*/
	.pal_bss :
	{
		CREATE_OBJECT_SYMBOLS
		. = ALIGN(0x1000);
		air_memmap_pal_bss_start = .;
		
		% for po in pal_objs :
		${po}(.bss COMMON)		
		% endfor
		
		air_memmap_pal_bss_end = .;
		. = ALIGN(0x1000);
	} >ram		
	
	/* ***********************************************************************	*/
	/* POS TEXT **************************************************************	*/
	/* ***********************************************************************	*/
	.pos_text :
	{
		CREATE_OBJECT_SYMBOLS
		. = ALIGN(0x1000);
		air_memmap_pos_code_start = .;
		
		% for po in pos_objs :
		${po}(.text .rodata*)
		% endfor
		
		air_memmap_pos_code_end = .;
		. = ALIGN(0x1000);
	} >ram

	/* ************************************************************************	*/
	/* POS DATA **************************************************************	*/
	/* ************************************************************************	*/

	air_memmap_ovrl_load_pointer	= 0;	
	air_memmap_pos_data_start		= .;
		
	.pos_data0 air_memmap_pos_data_start : AT (air_memmap_pos_data_start + air_memmap_ovrl_load_pointer)
	{	
		% for po in pos_objs :
		${po}(.data)
		% endfor
	
		*(.gnu.linkonce.d*)
		*(.gcc_except_table*)
		*(.shdata)		
	} > ram
			
    air_memmap_pos_data0_load	= air_memmap_pos_data_start;
	air_memmap_pos_data0_size	= (SIZEOF (.pos_data0) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
    air_memmap_pos_data0_end	= air_memmap_pos_data0_load + air_memmap_pos_data0_size;
	air_memmap_ovrl_load_pointer += air_memmap_pos_data0_size;

	% for p in range(1, number_of_partitions) :
	.pos_data${p} air_memmap_pos_data_start : AT (air_memmap_pos_data_start + air_memmap_ovrl_load_pointer) { } > ram	
    air_memmap_pos_data${p}_load	= air_memmap_pos_data${p-1}_end;
    air_memmap_pos_data${p}_end	= air_memmap_pos_data${p}_load + air_memmap_pos_data0_size;
	air_memmap_ovrl_load_pointer += air_memmap_pos_data0_size;
	
	% endfor	
	
	/* Move the current pointer to the biggest virtual address */
	. =  air_memmap_pos_data${p}_end;
	air_memmap_pos_data_end	= .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* POS BSS **************************************************************	*/
	/* ************************************************************************	*/

	air_memmap_ovrl_load_pointer	= 0;	
	air_memmap_pos_bss_start		= .;
		
	.pos_bss0 air_memmap_pos_bss_start : AT (air_memmap_pos_bss_start + air_memmap_ovrl_load_pointer)
	{
		% for po in pos_objs :
		${po}(.bss COMMON)
		% endfor
		
		*(.shbss)	
	} > ram
	
    air_memmap_pos_bss0_load	= air_memmap_pos_bss_start;
	air_memmap_pos_bss0_size	= (SIZEOF (.pos_bss0) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
    air_memmap_pos_bss0_end		= air_memmap_pos_bss0_load + air_memmap_pos_bss0_size;
	air_memmap_ovrl_load_pointer += air_memmap_pos_bss0_size;

	% for p in range(1, number_of_partitions) :
	.pos_bss${p} air_memmap_pos_bss_start : AT (air_memmap_pos_bss_start + air_memmap_ovrl_load_pointer) { } > ram	
    air_memmap_pos_bss${p}_load	= air_memmap_pos_bss${p-1}_end;
    air_memmap_pos_bss${p}_end		= air_memmap_pos_bss${p}_load + air_memmap_pos_bss0_size;
	air_memmap_ovrl_load_pointer += air_memmap_pos_bss0_size;
	
	% endfor	
	

	/* Move the current pointer to the biggest virtual address */
	. =  air_memmap_pos_bss${p}_end;
	air_memmap_pos_bss_end = .;
	. = ALIGN(0x1000);

	/* ************************************************************************	*/
	/* POS INIT CODE *********************************************************	*/
	/* ************************************************************************	*/
	
	/* Holds the current load address displacement */
	air_memmap_ovrl_load_pointer = 0;

	/* text init */
	air_memmap_part_init_code_start = .;
	
	/* P0 Init Code 		*/
	.text0 air_memmap_part_init_code_start : AT (air_memmap_part_init_code_start + air_memmap_ovrl_load_pointer )
	{ 
		${pos_user_init[0]}(*.text .rodata*) 
	} >ram
    air_memmap_part_init_text0_load	= air_memmap_part_init_code_start;
	air_memmap_part_init_text0_size = (SIZEOF (.text0) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
	air_memmap_part_init_text0_end 	= air_memmap_part_init_text0_load + air_memmap_part_init_text0_size;
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_text0_size;
	
	% for p in range(1, number_of_partitions) :
	/* P${p} Init Code 		*/	
	.text${p} air_memmap_part_init_code_start : AT (air_memmap_part_init_code_start + air_memmap_ovrl_load_pointer) 
	{
		${pos_user_init[p]}(*.text .rodata*) 
	} >ram
    air_memmap_part_init_text${p}_load	= air_memmap_part_init_text${p-1}_end;
	air_memmap_part_init_text${p}_size = (SIZEOF (.text${p}) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
    air_memmap_part_init_text${p}_end	= air_memmap_part_init_text${p}_load + air_memmap_part_init_text${p}_size;	
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_text${p}_size;
	
	% endfor
	
	/* Move the current pointer to the biggest virtual address */
	. =  air_memmap_part_init_text${p}_end;
	air_memmap_part_init_text_end = .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* POS INIT DATA *********************************************************	*/
	/* ************************************************************************	*/
	
	/* Holds the current load address displacement */
	air_memmap_ovrl_load_pointer = 0;

	/* data init */
	air_memmap_part_init_data_start = .;
	
	/* P0 Init Code 		*/
	.data0 air_memmap_part_init_data_start : AT (air_memmap_part_init_data_start + air_memmap_ovrl_load_pointer )
	{ 
		${pos_user_init[0]}(*.data) 
	} >ram
    air_memmap_part_init_data0_load	= air_memmap_part_init_data_start;
	air_memmap_part_init_data0_size = (SIZEOF (.data0) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
	air_memmap_part_init_data0_end 	= air_memmap_part_init_data0_load + air_memmap_part_init_data0_size;
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_data0_size;
	
	% for p in range(1, number_of_partitions) :
	/* P${p} Init Code 		*/	
	.data${p} air_memmap_part_init_data_start : AT (air_memmap_part_init_data_start + air_memmap_ovrl_load_pointer) 
	{
		${pos_user_init[p]}(*.data) 
	} >ram
    air_memmap_part_init_data${p}_load	= air_memmap_part_init_data${p-1}_end;
	air_memmap_part_init_data${p}_size = (SIZEOF (.data${p}) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
    air_memmap_part_init_data${p}_end 	= air_memmap_part_init_data${p}_load + air_memmap_part_init_data${p}_size;	
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_data${p}_size;
	
	% endfor	
	
	/* Move the current pointer to the biggest virtual address */
	. =  air_memmap_part_init_data${p}_end;
	air_memmap_part_init_data_end = .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* POS INIT BSS **********************************************************	*/
	/* ************************************************************************	*/
	
	/* Holds the current load address displacement */
	air_memmap_ovrl_load_pointer = 0;

	/* bss init */
	air_memmap_part_init_bss_start = .;
	
	/* P0 Init Code 		*/
	.bss0 air_memmap_part_init_bss_start : AT (air_memmap_part_init_bss_start + air_memmap_ovrl_load_pointer )
	{ 
		${pos_user_init[0]}(*.bss COMMON) 
	} >ram
    air_memmap_part_init_bss0_load	= air_memmap_part_init_bss_start;
	air_memmap_part_init_bss0_size 	= (SIZEOF (.bss0) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
	air_memmap_part_init_bss0_end 	= air_memmap_part_init_bss0_load + air_memmap_part_init_bss0_size;
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_bss0_size;
	
	% for p in range(1, number_of_partitions) :
	/* P${p} Init Code 		*/	
	.bss${p} air_memmap_part_init_bss_start : AT (air_memmap_part_init_bss_start + air_memmap_ovrl_load_pointer) 
	{
		${pos_user_init[p]}(*.data) 
	} >ram
    air_memmap_part_init_bss${p}_load	= air_memmap_part_init_bss${p-1}_end;
	air_memmap_part_init_bss${p}_size 	= (SIZEOF (.bss${p}) + 0xFFF) & ~(0xFFF); /* Align load address to 4096 boundary */
    air_memmap_part_init_bss${p}_end 	= air_memmap_part_init_bss${p}_load + air_memmap_part_init_bss${p}_size;	
	air_memmap_ovrl_load_pointer 	+= air_memmap_part_init_bss${p}_size;
	
	% endfor	
	
	/* Move the current pointer to the biggest virtual address */
	. =  air_memmap_part_init_bss${p}_end;
	air_memmap_part_init_bss_end = .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* PART CODE **************************************************************	*/
	/* ************************************************************************	*/	
	air_memmap_part_text_start = .;
	.part_text air_memmap_part_text_start : AT (air_memmap_part_text_start)
	{
		% for p in range(0, number_of_partitions) :
		. = ALIGN(0x1000);
		air_memmap_part_text${p}_start = .;
		
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/*(EXCLUDE_FILE (P${p}init.o) .text .rodata* .rodata.str0.8)  /* */

		% if part_libs:
			% for plib in part_libs[p]:
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/P${p}${plib}(.text .rodata* .rodata.str0.8)
			% endfor
		% endif

		air_memmap_part_text${p}_end = .;
		
		% endfor
	
	} > ram
	air_memmap_part_text_end= .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* PART DATA **************************************************************	*/
	/* ************************************************************************	*/	
	air_memmap_part_data_start = .;
	.part_data air_memmap_part_data_start :
	{
		% for p in range(0, number_of_partitions) :
		. = ALIGN(0x1000);
		air_memmap_part_data${p}_start = .;
		
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/*(EXCLUDE_FILE (P${p}init.o) .data) /* */

		% if part_libs:
			% for plib in part_libs[p]:
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/P${p}${plib}(.data)
			% endfor
		% endif

		air_memmap_part_data${p}_end = .;
		
		% endfor
	
	} > ram
	air_memmap_part_data_end = .;
	. = ALIGN(0x1000);
	
	/* ************************************************************************	*/
	/* PART BSS **************************************************************	*/
	/* ************************************************************************	*/	
	air_memmap_part_bss_start = .;
	.part_bss air_memmap_part_bss_start :
	{
		% for p in range(0, number_of_partitions) :
		. = ALIGN(0x1000);
		air_memmap_part_bss${p}_start = .;
		
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/*(EXCLUDE_FILE (P${p}init.o) .bss COMMON)	/* */

		% if part_libs:
			% for plib in part_libs[p]:
		[SAMPLE_ROOT]/${part_folders[p]}/renamed/P${p}${plib}(.bss COMMON)
			% endfor
		% endif

		air_memmap_part_bss${p}_end = .;
		
		% endfor
	
	} > ram
	air_memmap_part_bss_end = .;
	
	.shbss :
	{
		_air_memmap_shbs_start = .;
		*(.shbss)
		_air_memmap_shbs_end   = .;
	} > ram
		
	/* ***********************************************************************	*/
	/* AIR/PMK TEXT **********************************************************	*/
	/* ***********************************************************************	*/
	. = ALIGN(0x1000);
	air_memmap_code_start = .;
	.text air_memmap_code_start : AT(air_memmap_code_start)
	{
		% for plib in pmk_libs :
		${plib}(.text .rodata*)
		% endfor		
		
		*(.text)
		*(.text.*)
		*(.rodata*)
		
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
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		*(.ctors)
		LONG(0)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		*(.dtors)
		LONG(0)
		__DTOR_END__ = .;

		_rodata_start = . ;
		*(.rodata*)
		*(.gnu.linkonce.r*)
		_erodata = ALIGN( 0x10 );
		*(.init)
		*(.fini)
		*(.lit)
		_Oth_text_end = .;
		
		
	} > ram
	
	.rela.dyn       :
    {
		_Dyn_text_start = .;
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
		_Dyn_text_end = .;
		. = ALIGN (16);
    } >ram

	/* TODO: validate this: was previously @ end of data section 					*/
	/* since it seems to not be used, it was moved to the rtems code shared section 	*/
	_Others_start = .;
	.dynamic	  : { *(.dynamic) 	} >ram
	.jcr		  : { *(.jcr) 		} >ram
	.got		  : { *(.got) 		} >ram
	.plt   	  : { *(.plt) 		} >ram
	.hash		  : { *(.hash)		} >ram
	.dynrel	  : { *(.dynrel)	} >ram
	.dynsym	  : { *(.dynsym)	} >ram
	.dynstr	  : { *(.dynstr)	} >ram
	.hash		  : { *(.hash)		} >ram
	_Others_end = .;	
	
	. = ALIGN(0x1000);
	

	air_memmap_code_end  = .;		
	
	.data : {
		*(.data*)
	}  >ram
	.bss : {
		*(.bss* COMMON)
	}  >ram
	
	air_memmap_end = ALIGN(0x1000);
	
	.comment . (NOLOAD) :
	{
		*(.comment)
	}  
	
	.stab . (NOLOAD) :
	{
		[ .stab ]
	}
	
	.stabstr . (NOLOAD) :
	{
		[ .stabstr ]
	}

	. = ALIGN(0x1000);
	_End = .;
	
	/* TODO SKY 2011/06 : HACK : This is here just to be able to compile with
		regular (i.e. not modified) start.S. Their values are not used		*/
	air__endtext	= .;
	air__data_start = .;
	air___bss_start = .;
	air__edata		= .;
	air__end		= .;
	
	_endtext	= .;
	_data_start = .;
	__bss_start = .;
	_edata		= .;
	_end		= .;
	end			= .;
}

/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) GMVIS Skysoft S.A., 2008-2010
* ============================================================================
* This file is part of GMV's Partition Management Kernel (PMK).
* The PMK is free software: 
 * you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The PMK is distributed in the hope 
 * that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PMK.  
 * If not, see <http://www.gnu.org/licenses/>.
 * ===========================================================================
 */
 
 
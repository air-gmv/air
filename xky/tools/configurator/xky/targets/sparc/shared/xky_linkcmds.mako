<%
    import xky
    import utils.templates as makoutils

    mmap = os_configuration.get_memory_map()
%>
${makoutils.cfileHeader('linkcmds.ld', 'Linker script for XKY - OS (SPARC v8)')}

OUTPUT_ARCH(sparc)

MEMORY
{
    rom     : ORIGIN = 0x00000000, LENGTH = 0x00000000
    ram     : ORIGIN = ${'0x{:08X}'.format(mmap.kernel_space[0])}, LENGTH = ${'0x{:08X}'.format(mmap.kernel_space[1])}
}

ENTRY(pmk_trap_table)
SECTIONS
{
    /* ------------------------------------------------------------------------
     * XKY Kernel
     * --------------------------------------------------------------------- */
    . = ORIGIN(ram);
    xky_kernel_memory_start = .;
    .xky_kernel_text :
    {
        build/pmk.a(.text .rodata*)
        build/libgcc.a(.text .rodata*)
        build/libc.a(.text .rodata*)
    } >ram
    .xky_kernel_data :
    {
        build/pmk.a(.data*)
        build/libgcc.a(.data*)
        build/libc.a(.data*)
    } >ram
    .xky_kernel_bss :
    {
        xky_kernel_bss_start = .;
        build/pmk.a(.bss* COMMON)
        build/libgcc.a(.bss* COMMON)
        build/libc.a(.bss* COMMON)
        xky_kernel_bss_end = .;
    } >ram
    /* ------------------------------------------------------------------------
     * XKY Configuration
     * --------------------------------------------------------------------- */
    .xky_configuration :
    {
        xky_configuration = .;
        build/usr_config.o(.data* .rodata*)
        build/usr_arch.o(.data* .rodata*)
        build/usr_hm.o(.data* .rodata*)
        build/usr_schedules.o(.data* .rodata*)
        build/usr_partitions.o(.data* .rodata*)
        build/usr_channels.o(.data* .rodata*)
        build/usr_sharedmemory.o(.data* .rodata*)
        build/usr_partitions_data.o(.data* .rodata*)
    } >ram
    /* ------------------------------------------------------------------------
     * XKY Workspace
     * --------------------------------------------------------------------- */
    .xky_workspace :
    {
        . = ALIGN(0x1000);
        xky_workspace = .;
        . += ${hex(configuration.arch.workspace_size)};
    } >ram
    xky_kernel_memory_end = .;
}

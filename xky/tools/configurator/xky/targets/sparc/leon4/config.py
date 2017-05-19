##
# @package xky.targets.sparc.leon4
# @author pfnf
# @brief XKY OS for SPARC - Leon 4

__author__ = 'pfnf'

from xky import *
import utils
import utils.file as file_tools
import xky.targets.sparc.config as xky_sparc


##
# @brief Apply IOP arch dependent configurations
def iop_arch(config, partition):
    pass

# Target Description
description = "XKY OS for SPARC-LEON 4"

# Target Number of cores
cores = 4

# Kernel Compiler
kernel_compiler = dict(
    CC="sparc-rtems4.11-gcc --pipe",
    CXX="sparc-rtems4.11-g++ --pipe",
    LD="sparc-rtems4.11-gcc --pipe",
    AR="sparc-rtems4.11-ar",
    RANLIB="sparc-rtems4.11-ranlib",
    CFLAGS="",
    CPPFLAGS="-mcpu=v8 -msoft-float -mflat -fno-builtin -nodefaultlibs -gdwarf-2 -O2 -Wall",
    CXXFLAGS="",
    LDFLAGS="",
    ARFLAGS="ruv"
)

# kernel sources
kernel_sources = [path.join(SOURCE_PMK_DIRECTORY, f) for f in [
    # Arch files
    'arch/sparc/start.S',                           # entry point must be the 1st on the list
    'arch/sparc/lock.S',
    'arch/sparc/segregation_asm.S',
    'arch/sparc/window.S',
    'arch/sparc/isr_handler.c',
    'arch/sparc/isr_handler_asm.S',
    'arch/sparc/context_switch.S',
    'arch/sparc/syscall_sparc.S',
    'arch/sparc/syscall_asm.S',
    'arch/sparc/syscall_os.c',
    'arch/sparc/cpu.c',
    'arch/sparc/isr.c',
    'arch/sparc/health_monitor.c',
    'arch/sparc/segregation.c',
    'arch/sparc/atomic.c',
    # BSP files
    'bsp/sparc/leon3/amba.c',
    'bsp/sparc/leon3/bsp.c',
    'bsp/sparc/leon3/clock.c',
    'bsp/sparc/leon3/console.c',
    # 'bsp/sparc/leon3/iommu.c', @todo IOMMU support is disabled
    'bsp/sparc/leon3/l2cache.c',
    'bsp/sparc/leon3/irqmp.c',
    'bsp/sparc/leon3/bsp_asm.S',
    # Core files
    'core/error.c',
    'core/barrier.c',
    'core/ipc.c',
    'core/multicore.c',
    'core/tod.c',
    'core/init.c',
    'core/partition.c',
    'core/segregation.c',
    'core/schedule.c',
    'core/health_monitor.c',
    'core/printk.c',
    'core/workspace.c',
    'core/syscalls_tod.c',
    'core/syscalls_ports.c',
    'core/syscalls_schedule.c',
    'core/syscalls_partition.c',
    'core/syscalls_sharedmemory.c',
    'core/syscalls_healthmonitor.c',
    'core/sharedmemory.c',
    'core/loader.c',
    'core/ports.c',
    'core/ports_queuing.c',
    'core/ports_sampling.c',
    'core/configurations.c'
]]

# kernel ASM generator
kernel_asm = path.join(SOURCE_PMK_DIRECTORY, 'arch', 'sparc', 'asm_offsets.c')

# kernel headers
kernel_headers = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'core'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'arch', 'sparc'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'bsp', 'sparc', 'leon3'), ['.h', '.ld']),
]))

# Lib XKY sources
libxky_sources = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'core'), ['.c']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'arch', 'sparc'), ['.c', '.S']),
]))

# Lib XKY headers
libxky_headers = [h for h in kernel_headers
                  if h.endswith('xky.h') or h.endswith('xky_arch.h') or h.endswith('xky_bsp.h')]

# available permissions
permissions = [PERMISSION_SUPERVISOR, PERMISSION_FPU, PERMISSION_CACHE,PERMISSION_SET_TOD,
               PERMISSION_SET_PARTITION, PERMISSION_GLOBAL_TIME, PERMISSION_MODULE]

# memory map
mmap = MMAP(kernel_space=[0x40000000, 0x01000000],
            partition_space=[0x41000000, 0x10000000],
            units=[1 << 12, 1 << 18, 1 << 24],
            default_unit=1 << 18)

# specific defines
defines = ['PMK_FPU_SUPPORT=1']

# Architecture dependent configuration
arch_configure = xky_sparc.get_sparc_configuration

# IOP devices and definitions
iop = IOP(defines=[],
          devices=['greth0', 'greth1'],
          drivers=['amba', 'greth'],
          alias=dict(eth0='greth0',eth1='greth1'),
          arch=iop_arch)

# XKY application arch config
xkyapp_usr_arch = path.join(TARGETS_DIRECTORY, 'sparc', 'shared', 'usr_arch.mako')

# XKY application linkcmds
xkyapp_linkcmds = path.join(TARGETS_DIRECTORY, 'sparc', 'shared', 'xky_linkcmds.mako')

##
# @package air.targets.sparc.leon3
# @author pfnf
# @brief AIR OS for SPARC - Leon 3

__author__ = 'pfnf'

from air import *
import utils
import utils.file as file_tools
import air.targets.sparc.config as air_sparc


##
# @brief Apply IOP arch dependent configurations
def iop_arch(config, partition):
    pass

# Target Description
description = "AIR OS for SPARC-LEON 3"

# Target Number of cores
cores = 1

# Kernel Compiler with FPU
kernel_compiler = dict(
	CC="sparc-rtems5-gcc --pipe ",
	CXX="sparc-rtems5-g++ --pipe ",
	LD="sparc-rtems5-ld",
	AR="sparc-rtems5-ar",
	RANLIB="sparc-rtems5-ranlib",
	CFLAGS="",
	CPPFLAGS="-mcpu=leon3 -mflat -g -fno-builtin -nodefaultlibs -O2 -Wall",
	CXXFLAGS="",
	LDFLAGS="--gc-sections --wrap=printf --wrap=puts --wrap=putchar",
	ARFLAGS="ruv"
)

# Kernel Compiler with NO FPU
kernel_compiler_no_fpu = dict(
	CC="sparc-rtems5-gcc --pipe ",
	CXX="sparc-rtems5-g++ --pipe ",
	LD="sparc-rtems5-ld",
	AR="sparc-rtems5-ar",
	RANLIB="sparc-rtems5-ranlib",
	CFLAGS="",
	CPPFLAGS="-mcpu=leon3 -msoft-float -mflat -g -fno-builtin -nodefaultlibs -O2 -Wall",
	CXXFLAGS="",
    LDFLAGS="--gc-sections --wrap=printf --wrap=puts --wrap=putchar",
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
    'core/libc.c',
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

# Lib AIR sources
libair_sources = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'core'), ['.c']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'arch', 'sparc'), ['.c', '.S']),
]))

# Lib AIR headers
libair_headers = [h for h in kernel_headers
                  if h.endswith('air.h') or h.endswith('air_arch.h') or h.endswith('air_bsp.h')]

# available permissions
permissions = [PERMISSION_SUPERVISOR, PERMISSION_FPU, PERMISSION_CACHE,PERMISSION_SET_TOD,
               PERMISSION_SET_PARTITION, PERMISSION_GLOBAL_TIME, PERMISSION_MODULE]

# memory map
mmap = MMAP(kernel_space=[0x40000000, 0x01000000],
            partition_space=[0x41000000, 0x10000000],
            units=[1 << 12, 1 << 18, 1 << 24],
            default_unit=1 << 18)

# specific defines
defines = ['PMK_FPU_SUPPORT=1','PMK_DEBUG=0']

# Architecture dependent configuration
arch_configure = air_sparc.get_sparc_configuration

# IOP devices and definitions
iop = IOP(defines=[],
          devices=['greth0', 'greth1', 'grmil0', 'grspw0', 'grspw1', 'grspw2', 'grspw3', 'grspw4', 'spwrtr0', 'cpsw0', 'mil0', 'grcan0', 'grcan1'],
          drivers=['amba', 'greth', 'gr1553', 'cpsw', 'mil1553', 'grspw', 'grcan'],
          alias=dict(eth0='greth0',eth1='greth1', spw0='grspw0', spw1='grspw1', spw2='grspw2', spw3='grspw3', spw4='grspw4', cpsw='cpsw0', spwrtr='spwrtr0', milbus='grmil0', can0 = 'grcan0', can1 = 'grcan1'),
          arch=iop_arch)

# AIR application arch config
airapp_usr_arch = path.join(TARGETS_DIRECTORY, 'sparc', 'shared', 'usr_arch.mako')

# AIR application linkcmds
airapp_linkcmds = path.join(TARGETS_DIRECTORY, 'sparc', 'shared', 'air_linkcmds.mako')

##
# @package air.targets.arm.zynqz1
# @author lumm
# @brief AIR OS for ARM Zynq Z1

__author__ = 'lumm'

from air import *
import utils
import utils.file as file_tools
import air.targets.arm.config as air_arm


##
# @brief Apply IOP arch dependent configurations
def iop_arch(config, partition):
    pass

# Target Description
description = "AIR OS for ARM-ZYNQ Z1"

# Target Number of cores
cores = 2

# Kernel Compiler with FPU
kernel_compiler = dict(
    CC="arm-rtems5-gcc --pipe",
    CXX="arm-rtems5-g++ --pipe",
    LD="arm-rtems5-gcc --pipe",
    AR="arm-rtems5-ar",
    RANLIB="arm-rtems5-ranlib",
    CFLAGS="",
    CPPFLAGS="-g -c -fno-builtin -nodefaultlibs -Wall -march=armv7-a -mthumb \
-mfpu=neon -mfloat-abi=hard -mtune=cortex-a9",
    CXXFLAGS="",
    LDFLAGS="-Wl,--gc-sections -Wl,--wrap=printf -Wl,--wrap=puts -Wl,--wrap=putchar",
    ARFLAGS="ruv"
)

# Kernel Compiler with NO FPU
kernel_compiler_no_fpu = dict(
    CC="arm-rtems5-gcc --pipe",
    CXX="arm-rtems5-g++ --pipe",
    LD="arm-rtems5-gcc --pipe",
    AR="arm-rtems5-ar",
    RANLIB="arm-rtems5-ranlib",
    CFLAGS="",
    CPPFLAGS="-g -c -fno-builtin -nodefaultlibs -Wall -march=armv7-a -mthumb \
-mtune=cortex-a9",
    CXXFLAGS="",
    LDFLAGS="-Wl,--gc-sections -Wl,--wrap=printf -Wl,--wrap=puts -Wl,--wrap=putchar",
    ARFLAGS="ruv"
)

# kernel sources
kernel_sources = [path.join(SOURCE_PMK_DIRECTORY, f) for f in [
    # Arch files
    'arch/arm/start.S',                           # entry point must be the 1st on the list
    'arch/arm/atomic.c',
    'arch/arm/context_switch.c',
    'arch/arm/cpu.c',
    'arch/arm/exception_default_handler.c',
    'arch/arm/exception_default.S',
    'arch/arm/exception_fiq.S',
    'arch/arm/exception_irq.S',
    'arch/arm/exception_svc.S',
    'arch/arm/health_monitor.c',
    'arch/arm/irq_handler.c',
    'arch/arm/isr.c',
    'arch/arm/mmu.c',
    'arch/arm/svc.c',
    'arch/arm/virtualization_syscall.c',
    # BSP files
    'bsp/arm/zynqz1/bsp.c',
    'bsp/arm/zynqz1/gic.c',
    'bsp/arm/zynqz1/global_timer.c',
    'bsp/arm/zynqz1/mmu.c',
    'bsp/arm/zynqz1/triple_timer.c',
    'bsp/arm/zynqz1/uart.c',
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

# kernel headers
kernel_headers = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'core'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'arch', 'arm'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'bsp', 'arm', 'zynqz1'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'bsp', 'arm', 'shared'), ['.h', '.ld']),
]))

# Lib AIR sources
libair_sources = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'core'), ['.c']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'arch', 'arm'), ['.c', '.S']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'bsp', 'arm', 'zynqz1'), ['.c', '.S']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'bsp', 'arm', 'shared'), ['.c', '.S']),
]))

# Lib AIR headers
libair_headers = [h for h in kernel_headers
                  if h.endswith('air.h') or h.endswith('air_arch.h') or h.endswith('air_bsp.h')]

# available permissions
permissions = [PERMISSION_SUPERVISOR, PERMISSION_FPU, PERMISSION_CACHE,PERMISSION_SET_TOD,
               PERMISSION_SET_PARTITION, PERMISSION_GLOBAL_TIME, PERMISSION_MODULE]

# memory map
mmap = MMAP(kernel_space=[0x00100000, 0x0ff00000],
            partition_space=[0x10000000, 0x10000000],
            units=[1 << 12, 1 << 18, 1 << 24],
            default_unit=1 << 18)

# specific defines
defines = ['PMK_FPU_SUPPORT=1', 'PMK_SMP=0']

# Architecture dependent configuration
arch_configure = air_arm.get_arm_configuration

# IOP devices and definitions
iop = IOP(defines=[],
          devices=['greth0', 'greth1', 'gr1553b0', 'grspw0', 'grspw1', 'grspw2', 'grspw3', 'grspw4', 'spwrtr0', 'cpsw0', 'mil0', 'grcan0', 'grcan1'],
          drivers=['amba', 'greth', 'gr1553', 'cpsw', 'mil1553', 'grspw', 'grcan'],
          alias=dict(eth0='greth0',eth1='greth1', spw0='grspw0', spw1='grspw1', spw2='grspw2', spw3='grspw3', spw4='grspw4', cpsw='cpsw0', spwrtr='spwrtr0', mil='mil0', can0 = 'grcan0', can1 = 'grcan1'),
          arch=iop_arch)

# AIR application arch config
airapp_usr_arch = path.join(TARGETS_DIRECTORY, 'arm', 'shared', 'usr_arch.mako')

# AIR application linkcmds
airapp_linkcmds = path.join(TARGETS_DIRECTORY, 'arm', 'shared', 'air_linkcmds.mako')

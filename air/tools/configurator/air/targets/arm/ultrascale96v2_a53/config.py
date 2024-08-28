##
# @package air.targets.arm.ultrascale96v2_A53
# @author ansi
# @brief AIR OS for UltraScale 96 V2 - A53

__author__ = 'ansi'

from air import *
import utils
import utils.file as file_tools
import air.targets.arm.config as air_arm


##
# @brief Apply IOP arch dependent configurations
def iop_arch(config, partition):
    pass

# Target Description
description = "AIR OS for UltraScale 96 V2 - A53"

# Target Number of cores
cores = 4

# Kernel Compiler with FPU
kernel_compiler = dict(
    CC="arm-rtems5-gcc --pipe",
    CXX="arm-rtems5-g++ --pipe",
    LD="arm-rtems5-ld",
    AR="arm-rtems5-ar",
    RANLIB="arm-rtems5-ranlib",
    CFLAGS="",
    CPPFLAGS="-g -c -fno-builtin -nodefaultlibs -Wall -march=armv7-a -mthumb \
-mfpu=vfpv4 -mfloat-abi=softfp -mtune=cortex-a53", #Using the softfp float abi gives more compatibility but can decrease performance
            # Change to -mfloat-abi=hard for extra performance
    CXXFLAGS="",
    LDFLAGS="--gc-sections --wrap=printf --wrap=puts --wrap=putchar",
    ARFLAGS="ruv"
)

# Kernel Compiler with NO FPU
kernel_compiler_no_fpu = dict(
    CC="arm-rtems5-gcc --pipe",
    CXX="arm-rtems5-g++ --pipe",
    LD="arm-rtems5-ld",
    AR="arm-rtems5-ar",
    RANLIB="arm-rtems5-ranlib",
    CFLAGS="",
    CPPFLAGS="-g -c -fno-builtin -nodefaultlibs -Wall -march=armv7-a -mthumb \
-mtune=cortex-a53",
    CXXFLAGS="",
    LDFLAGS="--gc-sections --wrap=printf --wrap=puts --wrap=putchar",
    ARFLAGS="ruv"
)

# kernel sources
kernel_sources = [path.join(SOURCE_PMK_DIRECTORY, f) for f in [
    # Arch files
    'arch/arm/start.S',                           # entry point must be the 1st on the list
    'arch/arm/atomic.c',
    'arch/arm/cache.c',
    'arch/arm/context_switch.c',
    'arch/arm/cpu.c',
    'arch/arm/exception.S',
    'arch/arm/exception_handler.c',
    'arch/arm/fpu.S',
    'arch/arm/hm.c',
    'arch/arm/isr.c',
    'arch/arm/lock.c',
    'arch/arm/mmu.c',
    'arch/arm/svc.c',
    'arch/arm/syscalls_virtualization.c',
    # BSP files
    'bsp/arm/ultrascale96v2_a53/bsp.c',
    'bsp/arm/ultrascale96v2_a53/gic.c',
    'bsp/arm/ultrascale96v2_a53/global_timer.c',
    'bsp/arm/ultrascale96v2_a53/ipc.c',
    'bsp/arm/ultrascale96v2_a53/segregation.c',
    'bsp/arm/ultrascale96v2_a53/uart.c',
    'bsp/arm/ultrascale96v2_a53/slcr.c',
    'bsp/arm/ultrascale96v2_a53/xilinx/xil_assert.c',
    # Core files
    'core/error.c',
    'core/barrier.c',
    'core/ipc.c',
    'core/libc.c',
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
kernel_h_list = utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'core'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'arch', 'arm'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'bsp', 'arm', 'shared'), ['.h', '.ld']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PMK_DIRECTORY, 'bsp', 'arm', 'ultrascale96v2_a53'), ['.h', '.ld']),
])

# Remove the a9mpcore.h file from the kernel headers because it is not needed for the a53
for h in kernel_h_list:
    if h.endswith("a9mpcore.h"):
        kernel_h_list.remove(h)
        break

kernel_headers = set(kernel_h_list)

# Lib AIR sources
libair_sources = set(utils.flatten([
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'core'), ['.c']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'arch', 'arm'), ['.c', '.S']),
    file_tools.getFilesByExtensions(path.join(SOURCE_PAL_DIRECTORY, 'bsp', 'arm', 'ultrascale96v2_a53'), ['.c', '.S']),
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
            partition_space=[0x10000000, 0x02000000],
            units=[1 << 12, 1 << 16, 1 << 20],
            default_unit=1 << 20)

# specific defines
defines = ['PMK_FPU_SUPPORT=1', 'PMK_SMP=1', 'PMK_ARM_ULTRASCALE96V2_A53=1']

# Architecture dependent configuration
arch_configure = air_arm.get_arm_configuration

# IOP devices and definitions
iop = IOP(defines=[],
          devices=[],
          drivers=[],
          alias=dict(),
          arch=iop_arch)

# AIR application arch config
airapp_usr_arch = path.join(TARGETS_DIRECTORY, 'arm', 'shared', 'usr_arch.mako')

# AIR application linkcmds
airapp_linkcmds = path.join(TARGETS_DIRECTORY, 'arm', 'shared', 'air_linkcmds.mako')

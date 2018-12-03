RTEMS_API = 5
RTEMS_CPU = sparc
RTEMS_BSP = gr740

prefix = /opt/rtems/$(RTEMS_API)
exec_prefix = $(prefix)/$(RTEMS_CPU)-rtems$(RTEMS_API)

RTEMS_ROOT = $(prefix)
RTEMS_SHARE = $(RTEMS_ROOT)/share/rtems$(RTEMS_API)
PROJECT_ROOT = $(RTEMS_ROOT)/$(RTEMS_CPU)-rtems$(RTEMS_API)/$(RTEMS_BSP)
PROJECT_INCLUDE = $(PROJECT_ROOT)/lib/include
PROJECT_LIB = $(PROJECT_ROOT)/lib
SUPPORT_INCLUDE = ../../support/include

export PATH := $(RTEMS_ROOT)/bin:$(PATH)

export AR = $(RTEMS_CPU)-rtems$(RTEMS_API)-ar
export AS = $(RTEMS_CPU)-rtems$(RTEMS_API)-as
export CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc --pipe 
# export CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc 
export CXX = $(RTEMS_CPU)-rtems$(RTEMS_API)-g++
export LD = $(RTEMS_CPU)-rtems$(RTEMS_API)-ld
export NM = $(RTEMS_CPU)-rtems$(RTEMS_API)-nm
export OBJCOPY = $(RTEMS_CPU)-rtems$(RTEMS_API)-objcopy
export RANLIB = $(RTEMS_CPU)-rtems$(RTEMS_API)-ranlib
export SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
export STRIP = $(RTEMS_CPU)-rtems$(RTEMS_API)-strip

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
SYSFLAGS = -B $(PROJECT_LIB) -specs bsp_specs -qrtems
WARNFLAGS = -Wall
OPTFLAGS = -O2 -g -ffunction-sections -fdata-sections -mfpu -mhard-float -D_rtems_app

FLAGS = $(DEPFLAGS) $(SYSFLAGS) $(WARNFLAGS) $(CPU_CFLAGS) $(OPTFLAGS)

# LDFLAGS = -Wl,--gc-sections -Wl,--wrap=printf -Wl,--wrap=puts -Wl,--wrap=putchar
LDFLAGS = -Wl,--gc-sections -Wl,--wrap=puts -Wl,--wrap=putchar 
#-Wl,--wrap=printf   -> printf %f does't work

LINKFLAGS = $(SYSFLAGS) $(CPU_CFLAGS) $(LDFLAGS) $(OPTFLAGS)

CCLINK = $(CC) $(LINKFLAGS) -Wl,-Map,$(basename $@).map

I_DIRS += -I$(PROJECT_INCLUDE)/bsp
I_DIRS += -I$(SUPPORT_INCLUDE)
# -mfpu -mhard-float

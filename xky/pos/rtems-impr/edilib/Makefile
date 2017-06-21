# definition of the compiler, archiver and ranlib (in the future, this should be obtained by searching in the cross-compiler definitions)
CC = sparc-rtems4.8-gcc
AR = sparc-rtems4.8-ar
RANLIB = sparc-rtems4.8-ranlib

# change this directory for your path to the RTEMS directory (change this line for your specific settings)
RTEMS_SOURCE_PATH = /rtems/mcoutinho/rtems/rtems-impr

# definition of the standard (without coverage) object directory where to place the object files (dont change)
STANDARD_OBJECT_DIRECTORY=$(BUILD_DIR)/object

# definition of the standard (without coverage) library directory where to place the edilib library (dont change)
STANDARD_LIBRARY_DIRECTORY=$(BUILD_DIR)/library

# definition of the standard (without coverage) object directory where to place the object files (dont change)
COVERAGE_OBJECT_DIRECTORY=$(BUILD_DIR)/object_coverage

# definition of the standard (without coverage) library directory where to place the edilib library (dont change)
COVERAGE_LIBRARY_DIRECTORY=$(BUILD_DIR)/library_coverage



# directory where the edilib headers are at (dont change)
#INCDIRS = -I$(RTEMS_SOURCE_PATH)/cpukit/include -I$(RTEMS_SOURCE_PATH)/cpukit/score/include -I$(RTEMS_SOURCE_PATH)/cpukit/score/inline -I$(RTEMS_SOURCE_PATH)/cpukit/score/cpu/sparc

# definition of the compilation flags (dont change)
CFLAGS = $(INCDIRS) -O2 -Wall -fasm -mcpu=v8 -mhard-float -g

# definition of the coverage flags (dont change)
CFLAGS_COVERAGE = $(INCDIRS) -O0 -Wall -fasm -mcpu=v8 -mhard-float -g -fprofile-arcs -ftest-coverage

# definition of the edilib source code files (dont change)
#CSRCS = src/mktm_r.c src/strncmp.c src/memcpy.c src/memset.c
CSRCS =
CSRCS += src/mktm_r.c
CSRCS += src/strncmp.c
CSRCS += src/memcpy.c
CSRCS += src/memset.c

# the edilib object files are the source code files names in which the ".c" is replaced by ".o" (dont change)
COBJS_STANDARD = $(CSRCS:src/%.c=$(STANDARD_OBJECT_DIRECTORY)/%.o)

COBJS_COVERAGE = $(CSRCS:src/%.c=$(COVERAGE_OBJECT_DIRECTORY)/%.o)


# definition of the name of the library that will be created (dont change)
LIBC = liblibc.a

all: $(COVERAGE_LIBRARY_DIRECTORY)/$(LIBC) $(STANDARD_LIBRARY_DIRECTORY)/$(LIBC)


$(STANDARD_OBJECT_DIRECTORY)/%.o: src/%.c
	$(CC) $(CFLAGS)  -c -o $@ $<


$(COVERAGE_OBJECT_DIRECTORY)/%.o: src/%.c
	$(CC) $(CFLAGS_COVERAGE) -c -o $@ $<


$(STANDARD_LIBRARY_DIRECTORY)/$(LIBC): $(shell mkdir -p $(STANDARD_OBJECT_DIRECTORY)) $(shell mkdir -p $(STANDARD_LIBRARY_DIRECTORY)) $(COBJS_STANDARD) 
	$(AR) cru $(STANDARD_LIBRARY_DIRECTORY)/$(LIBC) $(COBJS_STANDARD)
	$(RANLIB) $(STANDARD_LIBRARY_DIRECTORY)/$(LIBC) 


$(COVERAGE_LIBRARY_DIRECTORY)/$(LIBC) : $(shell mkdir -p $(COVERAGE_OBJECT_DIRECTORY)) $(shell mkdir -p $(COVERAGE_LIBRARY_DIRECTORY)) $(COBJS_COVERAGE) 
	$(AR) cru $(COVERAGE_LIBRARY_DIRECTORY)/$(LIBC) $(COBJS_COVERAGE)
	$(RANLIB) $(COVERAGE_LIBRARY_DIRECTORY)/$(LIBC) 


clean:
	rm -rf $(STANDARD_OBJECT_DIRECTORY) $(COVERAGE_OBJECT_DIRECTORY) $(STANDARD_LIBRARY_DIRECTORY) $(COVERAGE_LIBRARY_DIRECTORY) $(LIBC)

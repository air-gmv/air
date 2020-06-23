"""@package definitions
Holds all the global definitions used in the entire project.
"""
################################################################################
#  Project     : AIRII
#  Date        : 2010/09/09
#  Author      : BRSV
#  Version     : $Revision: 1.4 $
#  Description : This module is used to place definitions for python module.
#  History     : 
#  $Log: definitions.py,v $
#  Revision 1.4  2011/06/30 15:19:45  brsv
#  bug found for configurations with more than 5 partitions
#
#  Revision 1.3  2011/06/14 15:39:08  brsv
#  inclusion of doxygen comments
#
#  Revision 1.2  2011/03/03 15:19:58  brsv
#  *** empty log message ***
#
#  Revision 1.5  2010/11/22 16:25:29  brsv
#  partition PXKthread added to generate.
#  three functions added in usr_partition.c
#  some changes made in every makefile in include definitions section.
#  validations added for ports pair values in a connection channel.
#
#  Revision 1.4  2010/11/03 18:35:44  brsv
#  partitionFilesHandler added. Partition files are now generated (ports, init and makefile)
#  usr_hm_tables.c file initiated - first phase
#
#  Revision 1.3  2010/10/25 17:33:07  brsv
#  Some minor improvements
#
#  Revision 1.2  2010/10/18 16:29:54  brsv
#  GUI Integration
#
#  Revision 1.1  2010/10/12 16:27:30  brsv
#  First issue
#
################################################################################

# ----------------------------[ IMPORTS ]---------------------------------------

from globalUtils import os
import platform

# -----------------------------[ DEFINES ]--------------------------------------

# Software version
VERSION           = "1.7"
# architectures
ARCH_INTEL        = "intel"
ARCH_SPARC        = "sparc"
# template for the search of schema file in the input folder
SCHEMA_FILE_NAME_TEMPLATE = "A653"

# -------------------------- [ DEFAULT VALUES ] --------------------------------
# Default architecture
DEF_ARCHITECTURE  = ARCH_SPARC
# SPARC file extension
SPARC_EXTENSION   = "leon"
# INTEL file extension
INTEL_EXTENSION   = "i386"
# Default tick value (in seconds)
DEF_TICK_VALUE = 100
# The value of one millisecond
MIL_SECOND = 1000
# The value of one second in nanoseconds
NANO_SECOND = 1000000000
# Default system memory size
DEF_SYS_MEM       = 0x400000
# PMK necessary libs for linkcmds
PMK_LIB_LIST      = ['pmk.a', 'pal_rtems.a']

# -----------------------------[ PLATFORM ]-------------------------------------
# Platform identification
if platform.uname()[4] == "x86_64":
  LIB_PREFIX="lib64"
else:
  LIB_PREFIX="lib"

# -----------------------------[ DEFINES ]--------------------------------------

# The different types of software architecture
ARCH_TYPE_LIST    = [ARCH_INTEL, ARCH_SPARC]

INIT_WELCOME      = """
+-------------------------------------------+
| PMK Files Generator                       |
| Version: %s                              |
+-------------------------------------------+
""" % VERSION

USAGE_MSG         ="""
    USAGE:
      python main.py [OPTIONS] <configuration file>

        OPTIONS:
          -h, --help           : Application help
          -s, --schema         : run with schema validation flag active
          -a, --architecture   : define the software architecture (Optional)
          -t, --templatefolder : template folder (Optional)
          -o, --outputfolder   : destination folder (Optional)
          
        EXAMPLES:
          1) Execute without schema validation
              python main.py -a sparc airctl.xml 
          
          2) Execute with schema validation
              python main.py -s -a intel airctl.xml
              
          3) Execute with graphical user interface
              python main.py
              
              
        NOTES:
          * schema file should be located in the input folder
          * if architecture option is not given the default will be used (%s)
          * the configuration file should be the last argument in the cmd line
""" % DEF_ARCHITECTURE

NO_ERROR_SIGNAL       = 0 # Unix quit signal

################################################################################
##                          FILENAME DEFINITIONS
################################################################################
MAKEFILE              = "Makefile"
# Config files
CONFIGURATION_FOLDER  = "config"
LINKCMDS_FILE         = "linkcmds"
USR_CALLBACKS_C_FILE  = "usr_callbacks.c"
USR_HM_TABLES_C_FILE  = "usr_hm.c"
USR_INIT_C_FILE       = "usr_init.c"
USR_PARTITION_H_FILE  = "usr_partitions.h"
USR_PARTITION_C_FILE  = "usr_partitions.c"
USR_PLUGINS_C_FILE    = "usr_plugins.c"
USR_CONFIG_H_FILE     = "usr_rtems_config.h"
USR_SCHEDULES_H_FILE  = "usr_schedules.h"
USR_SCHEDULES_C_FILE  = "usr_schedules.c"
USR_SYSCALL_H_FILE    = "usr_syscall.h"
USR_SYSCALL_C_FILE    = "usr_syscall.c"
# Partition Files
PX_PORTS_C_FILE       = "P%sports.c"
PX_PORTS_C_FILES      = "PXports.c"
PX_INIT_C_FILE        = "P%sinit.c"
PX_INIT_C_FILES       = "PXinit.c"
PX_KTHREAD_C_FILE     = "P%skthread.c"
PX_KTHREAD_C_FILES    = "PXkthread.c"
# Partition test files
PX_TESTDEF_H_FILE     = "P%stestdef.h"
PX_TESTDEF_C_FILE     = "P%stestdef.c"
PX_TESTPRNT_C_FILE    = "P%stestprint.c"
# Test definition file
TEST_DEF_FILE         = "testdef_%s.xml"
# Main Test File
PX_TESTXML_FILE       = "test.xml"
EDITESTXML_FILE    = "testedi.xml"

################################################################################
##                    SYSTEM DEFINITIONS
################################################################################

SYS_ROM_START     = 0x00000000
SYS_RAM_START     = 0x40000000

SYS_ROM_SIZE      = (1<<21) # 2 MB
MAX_PART_RAM_SIZE = (1<<23) # 8 MB - TODO : this should be determined 
                            # automatically from the Partition_Memory section of
                            # the XML file
                            

NO_OS         = "NONE"
RTEMS481      = "RTEMS481"
RTEMSIMPR     = "RTEMSIMPR"
VALID_OSs     = [NO_OS  , RTEMS481, RTEMSIMPR ]

NO_APEX       = "NONE"
AIRPEX        = "AIRPEX"
IMASPEX       = "IMASPEX"
VALID_APEXs   = [NO_APEX, AIRPEX  , IMASPEX   ]

NO_TEST       = "NONE"
TEST_EDI_UNT  = "TEST_EDI_UNT"
TEST_EDI_INT  = "TEST_EDI_INT"
TEST_EDI_VAL  = "TEST_EDI_VAL"
TEST_GMV_UNT  = "TEST_GMV_UNT"
TEST_GMV_INT  = "TEST_GMV_UNI"
TEST_GMV_VAL  = "TEST_GMV_VAL"
VALID_TESTs   = [ NO_TEST
                , TEST_EDI_UNT, TEST_EDI_INT, TEST_EDI_VAL
                , TEST_GMV_UNT, TEST_GMV_INT, TEST_GMV_VAL
                ]
EDI_TESTS     = [ TEST_EDI_UNT, TEST_EDI_INT, TEST_EDI_VAL ]
GMV_TESTS     = [ TEST_GMV_UNT, TEST_GMV_INT, TEST_GMV_VAL ]


################################################################################
##                    SYSTEM LIBRARIES
################################################################################

PMK_LIBS  = ["[PMK_ROOT]/lib/pmk.a"]

COMMON_PAL_OBJECTS =  [ "o-optimize/libgcc.a"
                      , "o-optimize/usr_hm.o" 
                      , "o-optimize/usr_partitions.o"
                      , "o-optimize/usr_callbacks.o"
                      , "o-optimize/usr_plugins.o"
                      , "o-optimize/usr_schedules.o"
                      , "o-optimize/usr_syscall.o"      ]

PAL_LIBS  = { RTEMS481  :
              [ "[PMK_ROOT]/lib/pal_rtems.a"  ] +
              COMMON_PAL_OBJECTS
              
            , RTEMSIMPR :
              [ "[PMK_ROOT]/lib/pal_rtems.a"  ] +
              COMMON_PAL_OBJECTS
            
            , NO_OS     :
              [ "[PMK_ROOT]/lib/pal_rtems.a"  ] +
              COMMON_PAL_OBJECTS
            }
            
################################################################################
##                    PARTITION OPERATING SYSTEM LIBRARIES
################################################################################
POS_LIBS =  { RTEMS481  :
              [ "o-optimize/usr_init.o"  
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemscpu.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemsbsp.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/start.o"
              , "/opt/rtems-4.8/sparc-rtems4.8/lib/soft/libc.a"
              , "$(OPT_LIB)/gcc/sparc-rtems4.8/4.2.4/soft/libgcc.a"
              , "$(OPT_LIB)/gcc/sparc-rtems4.8/4.2.4/crti.o"
              ]
            , RTEMSIMPR :
              [ "o-optimize/usr_init.o"  
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemscpu.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemsbsp.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/start.o"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libevent.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libext.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libio.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libmsg.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtmon.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libsem.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libtimer.a"
              , "[PMK_ROOT]/lib/../../edilib/library/liblibc.a" 
              ]
            , NO_OS :
              [ "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemscpu.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtemsbsp.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/start.o"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libevent.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libext.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libio.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libmsg.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/librtmon.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libsem.a"
              , "[AIR_PART_RTEMS]/sparc-rtems4.8/leon3/lib/libtimer.a"
              , "[PMK_ROOT]/../edilib/library/liblibc.a" 
              ]
            }

################################################################################
##                    APEX  LIBRARIES
################################################################################
APEX_LIBS = { AIRPEX   :
              [ "libpos.a"
              , "libairports.a"
              ]
            , IMASPEX  :
              [ "libpos.a"
              , "libairports.a"
              ]
            , NO_APEX :
              []
            }

################################################################################
##                    PARTITION  LIBRARIES
################################################################################
PART_LIBS = { "IOP"   :
              [ "libIOP.a"]
            , "MW"  :
              [ "libmw.a" ]
            , "NO_LIB" :
              []
            }

            
################################################################################
##                    TEST  LIBRARIES
################################################################################
TEST_LIBS = { NO_TEST   :
              [ ]
            , TEST_EDI_UNT  :
              [ "[SAMPLE_ROOT]/edisupport/o-optimize/libedisupport.a"
              , "[SAMPLE_ROOT]/performance/o-optimize/libperformance.a"
              ]
            , TEST_EDI_INT  :
              [ "[SAMPLE_ROOT]/edisupport/o-optimize/libedisupport.a"
              , "[SAMPLE_ROOT]/performance/o-optimize/libperformance.a"
              ]
            , TEST_EDI_VAL  :
              [ "[SAMPLE_ROOT]/edisupport/o-optimize/libedisupport.a"
              , "[SAMPLE_ROOT]/performance/o-optimize/libperformance.a"
              ]
            , TEST_GMV_UNT  :
              []
            , TEST_GMV_INT  :
              []
            , TEST_GMV_VAL  :
              []
            }

################################################################################
##                    TEST SUPPORT FLAGS 
################################################################################
TEST_SUPPORT_FLAGS = { NO_TEST      : ""
                     , TEST_EDI_UNT : "-Wl,--wrap,boot_card"
                     , TEST_EDI_INT : "-Wl,--wrap,_IO_Initialize_all_drivers"
                     , TEST_EDI_VAL : "-Wl,--wrap,_IO_Initialize_all_drivers"
                     }


################################################################################
##                    TEST SUPPORT DIRS 
################################################################################
TEST_DIRS = { NO_TEST   :
              [ ]
            , TEST_EDI_UNT  :
              [ "edisupport"
              , "performance"
              ]
            , TEST_EDI_INT  :
              [ "edisupport"
              , "performance"
              ]
            , TEST_EDI_VAL  :
              [ "edisupport" 
              , "performance"
              ]
            , TEST_GMV_UNT  :
              []
            , TEST_GMV_INT  :
              []
            , TEST_GMV_VAL  :
              []
            }

################################################################################
##                    TEST FILE RENAME EXCEPTIONS
################################################################################
TEST_SYMBS= { NO_TEST   :
              [ ]
            , TEST_EDI_UNT  :
              [ "_numberTestSteps"
              , "_requirementsTraced"
              , "_testPurpose"
              , "app_safe_state_handler"
              , "_detailedDesignComponentTraced"
              , "startUnitTest"
              ]
            , TEST_EDI_INT  :
              [ "_numberTestSteps"
              , "_requirementsTraced"
              , "app_safe_state_handler"
              , "_architectureComponentsTraced"
              , "_architectureInterfacesTraced"
              , "_testPurpose"
              , "_detailedDesignComponentTraced"
              ]
            , TEST_EDI_VAL  :
              [ "_numberTestSteps"
              , "_requirementsTraced"
              , "_testPurpose"
              , "app_safe_state_handler"
              , "_detailedDesignComponentTraced"
              ]
            , TEST_GMV_UNT  :
              []
            , TEST_GMV_INT  :
              []
            , TEST_GMV_VAL  :
              []
            }

          
################################################################################
##                    HEALTH MONITOR DEFINITIONS
################################################################################

# HM ERROR LEVELS
HM_ERROR_LEVELS = { "MODULE"    : 2
                  , "PARTITION" : 1
                  ,  "PROCESS"   : 0 }
                    
# HM ERROR ACTIONS
HM_ERROR_ACTIONS = {  "IGNORE"    : 0
                    , "SHUTDOWN"  : 1
                    , "RESET"     : 2
                    , "IDLE"      : 3
                    , "COLD_START": 4
                    , "WARM_START": 5  }


################################################################################
##                          AUXILIARY DEFINITIONS
################################################################################

POSITION_DICT         = {0: "first",
                         1: "second",
                         2: "third",
                         3: "forth",
                         4: "fifth",
                         5: "sixth",
                         6: "seventh",
                         7: "eighth",
                         8: "ninth",
                         9: "tenth"}

colours = { 'none'       :    "",
            'default'    :    "\033[0m",
            'bold'       :    "\033[1m",
            'underline'  :    "\033[4m",
            'blink'      :    "\033[5m",
            'reverse'    :    "\033[7m",
            'concealed'  :    "\033[8m",

            'black'      :    "\033[30m",
            'red'        :    "\033[31m",
            'green'      :    "\033[32m",
            'yellow'     :    "\033[33m",
            'blue'       :    "\033[34m",
            'magenta'    :    "\033[35m",
            'cyan'       :    "\033[36m",
            'white'      :    "\033[37m",
            
            'beep'       :    "\007",

            # non-standard attributes, supported by some terminals
            'dark'         :    "\033[2m",
            'italic'       :    "\033[3m",
            'rapidblink'   :    "\033[6m",
            'strikethrough':    "\033[9m",
            }

################################################################################
##                             XML DEFINITIONS
################################################################################

# -------------------- [ARINC_653_Module Definitions] --------------------------
ARINC_653_MODULE  = "ARINC_653_Module"
# Attributes
MODULE_ID         = "ModuleId"
MODULE_NAME       = "ModuleName"
MODULE_VERSION    = "ModuleVersion"
# System_State_Entry attributes
SSE_SYS_STATE     = "SystemState"
SSE_DESCRIPTION   = "Description"
# Error attributes
ERROR_ID          = "ErrorIdentifier"
ERROR_DESCRIPTION = "Description"
ERROR_LEVEL       = "ErrorLevel"
ERROR_CODE        = "ErrorCode"
ERROR_ACTION      = "Action"
# Partition attributes
PARTITION_ID      = "PartitionIdentifier"
PARTITION_NAME    = "PartitionName"
PART_CRITICALITY  = "Criticality"
PART_SYS_PARTIT   = "SystemPartition"
PART_ENTRY_POINT  = "EntryPoint"
# Queuing Port attributes
QUEUING_PORT_NAME = "Name"
QUEU_MAX_MSG_SIZE = "MaxMessageSize"
QUEUING_DIRECTION = "Direction"
QUEU_MAX_NB_MSGS  = "MaxNbMessages"
# Channel attributes
CHANNEL_ID        = "ChannelIdentifier"
CHANNEL_NAME      = "ChannelName"
# Address attributes
PORT_NAME         = "PortName"
PHYSICAL_ADDRESS  = "PhysicalAddress"
# Module HM Table attribute
MODULE_CALLBACK   = "ModuleCallback"
# Memory_Requirements attributes 
MEM_REQ_TYPE      = "Type"
MEM_REQ_SIZE_BYTE = "SizeBytes"
MEM_REQ_PHYS_ADD  = "PhysicalAddress"
MEM_REQ_ACCESS    = "Access"
MEM_REQ_REG_NAME  = "RegionName"
# Schedule attributes
SCHEDULE_ID       = "ScheduleIdentifier"
SCHEDULE_NAME     = "ScheduleName"
INITIAL_MOD_SCHED = "InitialModuleSchedule"
SCH_MAJ_FRAME_SEC = "MajorFrameSeconds"
SCH_PERIOD_SEC    = "PeriodSeconds"
SCH_PERID_DUR_SEC = "PeriodDurationSeconds"
SET_MOD_SCHEDULE  = "SetModuleSchedule"
SCH_CHANGE_ACTION = "ScheduleChangeAction"
# Window Schedule attributes
WINDOW_ID         = "WindowIdentifier" 
WINDOW_START_SEC  = "WindowStartSeconds"
WINDOW_DUR_SEC    = "WindowDurationSeconds"
PART_PERIOD_SEC   = "PartitionPeriodStart"  
# Partition HM Table attribute
PART_CALLBACK     = "PartitionCallback"
# Extension attribute properties
TICK_PROPERTY     = "air_ticks_per_second"
SYSMEM_PROPERTY   = "air_system_memory"
PART_PERSONALITY  = "air_partition_personality"

# -------------------- [Level 2 Module Tag Definitions] ------------------------

MOD_SCHEDULE      = "Module_Schedule"
# HM Table
SYS_HM_TABLE      = "System_HM_Table"
MOD_HM_TABLE      = "Module_HM_Table"
PART_HM_TABLE     = "Partition_HM_Table"
# Partition
PARTITION         = "Partition"
PART_MEMORY       = "Partition_Memory"
# Schedule
PART_SCHEDULE     = "Partition_Schedule"

CONNECTION_TABLE  = "Connection_Table"
LOG_BOOK_SYS      = "LogbookSystem"

MEM_REQUIREMENTS  = "Memory_Requirements"
# Module extension
MOD_EXTENSION     = "air_module_ext"
# Partition extension
PART_EXTENSION    = "tnsa:air_partition_ext"

# -------------------- [Level 3 Module Tag Definitions] ------------------------

SYS_STATE_ENTRY   = "System_State_Entry"
QUEUING_PORT      = "Queuing_Port"
CHANNEL           = "Channel"
WINDOW_SCHEDULE   = "Window_Schedule"

SUPERVISOR        = "supervisor"
TEST              = "test"
CACHE             = "cache"
TOD_SYNC          = "tod_sync"
LOCALTIME         = "localtime"
IOP 			  = "is_IOP"
MW 				  = "uses_middleware"

CACHE_CONTROL     = "cache_control"
FP_CONTROL        = "fp_control"
SCHED_CONTROL     = "sched_control"

# -------------------- [Level 4 Module Tag Definitions] ------------------------
ERROR_ID_LEVEL    = "Error_ID_Level"
ERROR_ID_ACTION   = "Error_ID_Action"
SOURCE            = "Source"
DESTINATION       = "Destination"
STAND_PARTITION   = "Standard_Partition"
PSEUDO_PARTITION  = "Pseudo_Partition"

CACHE_PERMISSION  = "permission"
CACHE_CODE        = "code"
CACHE_DATA        = "data"

################################################################################
##                          GLOBAL MESSAGE DEFINITIONS
################################################################################

NOT_APPLICABLE        = "N/A"

# Output generation files messages
HANDLE_GEN_FILE       = "Start creating file %s ..."

START_LOADING         = " %s \n Load config file ... \n" % ("="*80)

START_PROCESSING      = "\n %s \n Generating files ... \n" % ("="*80)

END_PROCESSING        = "\n %s \n Final result \n" % ("="*80)

CONFIG_FILE_LOADED    = "ARINC config file loaded"

FILE_CREATION_START   = "Start the files creation .. "

FILES_GEN_SUCCESS     = "'%s' module created"

################################################################################
##                             ERROR DEFINITIONS
################################################################################
CFG_FILE_NOT_EXIST = "Configuration file does not exist in %s"

NO_ARCH_DEFINE        = "No architecture defined, the default value will "\
                        "be used! "

CMD_LINE_SINTAX_ERROR = "Abnormal command line used"
                        
UNRECOGNIZED_ARCH     = "Unrecognized architecture"

GENERATED_FILE_NOK    = "%s\t\t## NOK ##"

GENERATED_FILE_OK     = "%s\t\t** OK **"

NO_MODULE_DEF_ERROR   = "-"*50 + "\n%s: N/A\n"

NO_ELEMS_IN_COMPONENT = "No %s elements found in %s component"

APPLICATION_EXIT      = "Program will now exit!"

NO_SCHEMA_FILE_ERROR  = "No schema file found in the input folder, validation "\
                        "was not performed"
                        
SCHEMA_VALIDATION_ERR = "Schema validator error, config file may not be valid"

FILES_OUTPUT_ERROR    = "Some files could hold some errors or not created at "\
                        "all"
                        
FINAL_FOLDER_ERROR = "Unable to move ouput files to the desired folder"

MAX_MSG_SIZE_ERR_MSG  = "The MaxMessageSize value is incorrect for the port "\
                        "'%s', inconsistent with the value defined in the "\
                        "other side of the channel. The value in both ports "\
                        "has been modified to '%s' in partition '%d'"

MAX_NB_MSG_ERR_MSG    = "The MaxNbMessages value is incorrect for the port "\
                        "'%s', inconsistent with the value defined in the "\
                        "other side of the channel. The value in both ports "\
                        "has been modified to '%s' in partition '%d'"
                        

################################################################################
##                             PATH DEFINITIONS
##
##                            W A R N I N G !!!!
##
##  The paths, if changed, MUST BE KEPT in the same format. That is, if it ends
##  with a backslash, it MUST KEEP existing, if not, it MUST NOT be added. If 
##  new are to be added they should not contain the backslash.
##
################################################################################

TEMPLATE_FOLDER = "templates"
SCHEMA_FOLDER   = "schemas"

# Base folder path
BASE_PATH = os.getcwd()

# Input configuration files folder
INPUT_FOLDER    = os.path.join(BASE_PATH, "input")
# Output result files folder
OUTPUT_FOLDER   = os.path.join(BASE_PATH, "output")
# Log file path
LOG_FILE_FOLDER = os.path.join(BASE_PATH, "log")
# Templates folder
TEMP_FLDER_PATH = os.path.join(INPUT_FOLDER, TEMPLATE_FOLDER)  
# Tests definitions folder
TESTSDEFS_FOLDER   = os.path.join(BASE_PATH, "testdefs")

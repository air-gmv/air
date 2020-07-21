#!/usr/bin/python
"""@package input.scripts
Holds the output generator handler for both Makefile and linkcmds files
"""
################################################################################
#  Project     : AIRII
#  Date        : 2011/07/01
#  Author      : JC / BV
#  Version     : $Revision: 1.3 $
#  Description : This module is responsible to handle the generation of 
#                 linkcmds file using an external lib called mako.
#  History     : 
#  $Log: mk_sysfiles.py,v $
#  Revision 1.3  2011/09/07 12:17:03  jaac
#  PMK2.4.0
#  Minor bug fix: missing space
#
#  Revision 1.2  2011/09/07 11:03:02  jaac
#  PMK2.4.0
#  Improved the mk_sysfiles.py to have support for an optional template folder and test libs
#  http://imaserver.gmv.es:8000/air/ticket/27
#
#  Revision 1.1  2011/08/09 17:27:39  brsv
#  generate system files using mako first draft
#
#  Revision 1.2  2011/07/28 14:20:50  jaac
#  PMK2.3.1
#  Splited common definitions and functions to mk_defs.py
#  Created mk_makefile.py to create config/makefile
#
#  Revision 1.1  2011/07/01 17:35:38  brsv
#  Mako processing templates added
#
################################################################################

# ----------------------------[ IMPORTS ]---------------------------------------

from mako.template import Template
from mako.runtime import Context
from types import ListType
import os.path
import sys
import getopt
import StringIO

# -----------------------------[ DEFINITIONS ]----------------------------------
USAGE_MSG = "aaaa..."

TEMPLATES_FOLDER  = "."
SCHED_H_FILE      = "usr_schedules.h"
SCHED_C_FILE      = "usr_schedules.c"
PART_H_FILE       = "usr_partitions.h"
PART_C_FILE       = "usr_partitions.c"
PLUG_C_FILE       = "usr_plugins.c"
SYSC_C_FILE       = "usr_syscall.c"

# input constants
USR_PERM = 1  # user mode can change this feature in runtime
SPV_PERM = 0  # user mode cannot change this feature in runtime

DATA_CACHE_DISABLED = 0x0
DATA_CACHE_FROZEN   = 0x1
DATA_CACHE_ENABLED  = 0x3

CODE_CACHE_DISABLED = 0x0
CODE_CACHE_FROZEN   = 0x4
CODE_CACHE_ENABLED  = 0xC

# -----------------------------[ FUNCTIONS ]------------------------------------
# process a template file (filename) with the (v_dict) dictionary
# if it is file for a partition, the template file is preceeded by a 'p'
# if the index is >= 0, this should be used in the generation of the output file
def process_file (  filename, v_dict, template_folder="", output_folder="", 
                    type = "", index = -1):
  """
  Process the generation of a file from a template
    * filename          = file to process
    * v_dict            = dictionary of value to replace
    * template_folder   = path for the templates folder
    * output_folder     = the path to where should be created the output file
    * type              = PARTITION | MAIN | CONFIG | TESTDEF
  """
  
  # --------------------------- MAKO PROCESSING -------------------------------
  # get the path for the template file
  if type == "PARTITION":
    if index == -1:
      template_file = os.path.join(template_folder, "p" + filename + ".t")
    else:
      template_file = os.path.join(template_folder, "p" + (filename % "x") + ".t")
  elif type == "MAIN":
    template_file = os.path.join(template_folder, "m" + filename + ".t")    
  elif type == "CONFIG":
    template_file = os.path.join(template_folder, filename + ".t")    
  elif type == "TESTDEF":
    template_file = os.path.join(template_folder, (filename % "x") + ".t")
  else:
    return False, "Invalid template type"

  # debug: report the template filename
  print template_file.replace(os.getcwd(),"").ljust(40),"=>",
  
  # Mako template load
  try:    
    mytemplate = Template(filename=template_file)
  except IOError:
    return False, \
           "ERROR: Missing template file (%s)" % template_file
  except:
    return False, \
           "ERROR: Error loading template file (%s) : %s" % (template_file,sys.exc_info()[0])
  
  # processing
  file_ctx = mytemplate.render(**v_dict )
  
  # ------------------------ FILE GENERATION -----------------------------------
  
  # Output file path
  if  type == "PARTITION" and index > -1:
    dstFilePath  = os.path.join(output_folder, filename % str(index))
  elif type == "TESTDEF" and len(index) > 0:
    dstFilePath  = os.path.join(output_folder, filename % index)
  else:
    dstFilePath  = os.path.join(output_folder, filename)
  
  # Create the file
  try:
    # delete any previous file that could exist
    if(os.path.exists(dstFilePath)):
      os.remove(dstFilePath);
    # initialise output file
    with open(dstFilePath, 'w') as fdOut:
      # write the context to file
      fdOut.write(file_ctx)
    # Report    
    print dstFilePath.replace(os.getcwd(),"")
  except:
    return False, sys.exc_info()[1]
    
  return True, "File processed!"


#===============================================================================
# Main routine execution
#===============================================================================
if __name__ == '__main__':
  """
  Application main
  """
  
  # Validate input parameters
  try:
    optList, args = getopt.getopt(sys.argv[1:], "hat:z", 
	  ["help", "all", "test", "templatefolder="])
  except getopt.GetoptError, err:
    print USAGE_MSG;
    sys.exit(1)
    
  # Command line processing and error checking
  #if len(sys.argv) < 1 or optList == []:
  if len(sys.argv) < 1:
    print "Missing arguments:"
    print USAGE_MSG;
    sys.exit(1)
  
  # default values
  template_folder = ""
  tests           = False
  
  
  # get input parameters
  for opt in optList:
    # Help
    if opt[0] in ("-h", "--help"):
      print USAGE_MSG;
      sys.exit(0)
    # alternative template folder
    if opt[0] in ("-t", "--templatefolder"):
      template_folder = opt[1][1:]
	# include test libraries
    if opt[0] in ("-z", "--test"):
      tests = True
      
  # path for the template folder
  if not template_folder:
    template_folder = os.path.join("..", TEMPLATES_FOLDER)
    
  resValue = True
  

  
  # input variables ###########################################################
    # Schedule
  ms_per_tick         = 10
  num_sched           = 2
  sch_preempt_points  = [5,7]
  init_sched          = 0
  # tick, partition
  sch_preempt_ticks   = [ [[0,0],[30,0],[50,1],[100,2],[125,3]],
                          [[0,0],[30,0],[50,1],[ 70,1],[105,2],[125,3], [150,3]]]
  sch_mtfs            = [150,200]
    
    # Partition data
  available_ram       = 0x400000
  num_parts           = 4
  default_heap        = 100*1024
  
    # permission data
  supervisor_perm   = [0,0,0,0]
  cache_status      = [ [USR_PERM,DATA_CACHE_ENABLED  ,CODE_CACHE_ENABLED],
                        [USR_PERM,DATA_CACHE_DISABLED ,CODE_CACHE_ENABLED],
                        [USR_PERM,DATA_CACHE_ENABLED  ,CODE_CACHE_ENABLED],
                        [USR_PERM,DATA_CACHE_ENABLED  ,CODE_CACHE_FROZEN]]
                        
  tod_sync          = [ False, False, False, False ]
  local_time        = [ False, False, False, False ]
  
  # channels and ports
  qport_ctl_size 		= 4096
  parts_with_qports	= [0,1,2]
  qchannels 			  = [0,1,2]
  qchannels_links		= [[0,1],[1,0],[0,2]]
  
  printf_mem_size		= 4096
  parts_with_printf	= [0,1,2,3]
  
  # Partition OS
  apex = "AIRPEX"
  printf_present = True
  
  # System calls
  cache_control = [0,1,2,3]
  fp_control    = [0,1,2,3]
  sched_control = []
  
  # Process the usr_schedules.h ###############################################
  schedh_vars = dict( ms_per_tick         = ms_per_tick
                    , num_sched           = num_sched
                    , sch_preempt_points  = sch_preempt_points )
  
  res, msg = process_file(SCHED_H_FILE, schedh_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg
  
  # Process the usr_schedules.c ###############################################
  schedc_vars = dict( num_sched           = num_sched
                    , sch_preempt_points  = sch_preempt_points
                    , init_sched          = init_sched
                    , sch_preempt_ticks   = sch_preempt_ticks
                    , sch_mtfs            = sch_mtfs  )
  
  res, msg = process_file(SCHED_C_FILE, schedc_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg
  
  # Process the usr_partitions.h ###############################################
  parth_vars = dict(  num_parts     = num_parts
                    , available_ram = available_ram
                    , default_heap  = default_heap )
  
  res, msg = process_file(PART_H_FILE, parth_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg
  
  # Process the usr_partitions.c ###############################################
  partc_vars = dict(  num_parts       = num_parts
                    , supervisor_perm = supervisor_perm
                    , cache_status    = cache_status
                    , tod_sync        = tod_sync
                    , local_time      = local_time)
  
  res, msg = process_file(PART_C_FILE, partc_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg

  # Process the usr_plugins.c ###############################################
  pluginc_vars = dict(  qport_ctl_size    = qport_ctl_size
                      , parts_with_qports = parts_with_qports
                      , qchannels         = qchannels
                      , qchannels_links   = qchannels_links
                      , printf_mem_size   = printf_mem_size
                      , parts_with_printf = parts_with_printf
                      , apex              = apex  )
  
  res, msg = process_file(PLUG_C_FILE, pluginc_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg
  
  # Process the usr_syscall.c ###############################################
  syscc_vars = dict(  num_parts     = num_parts
                    , cache_control = cache_control
                    , fp_control    = fp_control
                    , sched_control = sched_control )
  
  res, msg = process_file(SYSC_C_FILE, syscc_vars, 
                          template_folder=template_folder)
  resValue &= res
  print msg

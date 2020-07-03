#!/usr/bin/python
import os
import os.path
import sys
import shutil
import subprocess
import re
import test_post_processing
import time
from mako.template import Template
from test_list import test_list

try:
  from colorama import init, Fore, Back, Style
  colors = True
except:
  colors = False

##########################################################
## SYNTAX ################################################
##########################################################
Syntax = """
  ./run_tests.py [-nc] [-vX] [-eE] [-d] [test] 
  ./run_tests.py --report test

    -nc: no color output

    -v : report level, where X is one of:
         0 : just report test results
         1 : report short failed test step status
         2 : report short test step failing results for all tests
         3 : report verbose failed test step results 
         4 : report verbose test step results for all tests
         5 : report Excel table (Not implemented yet)


    -e : execution/debugger tool, where E is one of:
        t50,t   : tsim @ 50 MHz
        t40     : tsim @ 40 MHz
        tg50,tg : tsim @ 50 MHz in GDB Server mode
        tg40    : tsim @ 40 MHz in GDB Server mode
        g       : grmon
        gg      : grmon in GDB Server mode
        r1      : rasta.1 (192.168.0.78)
        r2      : rasta.2 (192.168.0.64)

    -d : debug (print all test output)

    test: optionally run just the specified test

    ---------------------------------------------

    Alternative syntax:

    --report test: print the test step info without running anything

"""

###########################################################
## ENVIRONMENT ############################################
###########################################################


# test binary
executable = "executable/AIRAPP.exe"

# grmon custom script (will be rewritten by the tool)
grmon_script_name = "grmon-execute.sh"


ansi_escape = re.compile(r'\x1b[^m]*m')

grmon_script = "\
load %s\n\
run\n\
quit\n\
"

# execution tool
ex_tools  = \
  { "tsim_50"    : "tsim-leon3 -mmu -nb -c tsim-execute.sh -freq 50"
  , "tsim_40"    : "tsim-leon3 -mmu -nb -c tsim-execute.sh -freq 40"
  , "tsim_gdb40" : "tsim-leon3 -mmu -gdb -port 12345 -freq 40"
  , "tsim_gdb50" : "tsim-leon3 -mmu -gdb -port 12345 -freq 50"
  , "rasta1"      : "grmon -eth -ip  192.168.0.78 -udp 6768 -u -c " + grmon_script_name 
  , "rasta2"      : "grmon -eth -ip  192.168.0.64 -udp 6768 -u -c " + grmon_script_name 
  , "grmon"      : "grmon -c " + grmon_script_name 
  , "grmon_gdb"  : "grmon -gdb -port 12345" 
  }

# grmon output port
grmon_output = "/dev/ttyUSB0"

# grmon hang limits
i_LIMIT  	= 300
SLEEPSEC    = 0.2

###########################################################
## DEBUG LEVELS ###########################################
###########################################################
debug_level   = 1 

PRINT_STEPS_FAILED = [1,3]
PRINT_STEPS_ALWAYS = [2,4]
PRINT_STEP_LONG    = [3,4]

###########################################################
## TEST ENUMS #############################################
###########################################################

## Test result vs test passed               
TEST_RESULTS =  [ False # TEST_FAILURE
                , True  # TEST_SUCCESS
                , False # TEST_UNEXPECTED
                ]

## Test comparison type
TEST_COMP_TYPE =  [ "is unexpected (different)"  # RESULT_DIFF 
                  , "is unexpected (equal)"      # RESULT_EQUAL
                  , "is greater than expected"   #  RESULT_GT 
                  , "is less than expected"      #  RESULT_LT 
                  ]

## Test obtained value type
TEST_VALUE_TYPE = [ "No value"        # RESULT_TYPE_NONE
                  , "Value %i"        # RESULT_TYPE_VALUE
                  , "Value %s"        # RESULT_TYPE_STRING
                  , "Pointer %08x"    # RESULT_TYPE_POINTER
                  , "Return Value %d" # RESULT_TYPE_RETVAL
                  , "RTEMS Return %d" # RESULT_TYPE_RTEMSRETVAL
                  ]

###########################################################
## Global definitions #####################################
###########################################################
TEST_FILE = "testdef.c"

###########################################################
## Path Util Functions ####################################
###########################################################
def get_immediate_subdirectories(dir,wildcard=None):
  return sorted(\
    [name for name in os.listdir(dir)
       if os.path.isdir(os.path.join(dir, name)) and 
          (wildcard == None or (wildcard in name)) ])

def get_immediate_files(dir,wildcard=None):
  return sorted(\
    [name for name in os.listdir(dir)
       if os.path.isfile(os.path.join(dir, name)) and 
          (wildcard == None or (wildcard in name)) ])
          
def get_files_ended(dir,wildcard=None):
  return sorted(\
    [name for name in os.listdir(dir)
       if os.path.isfile(os.path.join(dir, name)) and 
          (wildcard == None or ((wildcard in name) and (wildcard[-1] == name[-1])))])
          
def path_join(path_array):
  path = ""
  for p in path_array:
    path = os.path.join(path,p)
  return os.path.abspath(path)

def get_screen_width():
  try:
    rows, columns = os.popen('stty size', 'r').read().split()
    return int(columns)
  except:
    return 80

def get_size(filename):
  statinfo = os.stat(filename)
  return statinfo.st_size
  
  
###########################################################
## Text Util Functions ####################################
###########################################################
def clean_spaces(text):
  text = text.replace("\n"," ")
  text = text.replace("\t","")
  text = text.replace("  "," ")  
  return text

def remove_spaces(text):
  text = text.replace("\n","")
  text = text.replace("\t","")
  text = text.replace(" ","")  
  return text

def remove_tabs(text):
  return text.replace("\t", "")

  
# Left align text, split it in line  
def align_paragraph(paragraph, width,tab=0):
  '''
  align paragraph to specified width,
  returns list of paragraph lines
  '''
  lw = 0
  line = []
  result = []
  for word in paragraph.split():
    if lw + len(word) + 1 < width:
      line.append(word)
      lw += len(word) + 1
    else:
      result.append("\t"*tab + " ".join(line))
      lw = 0
      line = []
      line.append(word)
  result.append("\t"*tab + " ".join(line))
  return "\n".join(result)

###########################################################
## Generate Report XML from Mako ##########################
###########################################################

def generateReportXml(resultList):
  v_dict = dict(testResults = resultList)

  mytemplate = Template(
    filename="testResults.xml.mako",
    input_encoding='utf-8',
    output_encoding='utf-8',
    default_filters=['decode.utf8'],
    encoding_errors='replace')

  file_ctx = mytemplate.render(**v_dict)
  outfile = open("../testResults.xml", "w")
  outfile.write(file_ctx)



###########################################################
## Test Step Description Functions ########################
###########################################################
def clean_stepinfo(text, width=80, tab=0):
  text = text.replace("*/","")
  return align_paragraph(text, width, tab)
  
def get_steps_info(tests):
  steps_info = {}
  ssize = get_screen_width()
  # process each test
  for tname in tests:
    steps_info[tname] = {}  
    
    # scan all relevant files
    for folder in get_immediate_subdirectories(tname,"p"):
      fldr = path_join([os.getcwd(),tname,folder])
      
      for file in get_files_ended(fldr,TEST_FILE):
        # determine file full path
        filename = path_join([os.getcwd(),tname,folder,file])
        # state variables
        in_stepinfo = False
        step_info   = ""
        # scan files
        with open(filename) as f:
          p_num = int(re.findall(".*(\d+).*",os.path.basename(filename))[0])
          for line in f:
            # Step info processing #############
            # Test step start #
            m = re.search("Test\sStep\s(\d+)\s+", clean_spaces(line))
            if m:
              in_stepinfo = True
              index =  int(m.group(1))
            # Test step description body # 
            elif in_stepinfo:
              step_info += line
            # Test step end #
              if "*/" in line:
                in_stepinfo = False
                data = {}
                data["d"] = clean_stepinfo(step_info,ssize-12,1)
                data["p"] = p_num
                steps_info[tname][str(index)] = data
                step_info = ""
  return steps_info
 
###########################################################
## Create test specific grmon script ######################
###########################################################
def create_grmon_script(test_exec):
  with open(grmon_script_name,"w") as gs:
    gs.write(grmon_script % test_exec)
  
###########################################################
## Process Command Line Arguments #########################
###########################################################
curr_argv = 1

# report #################################
if (len(sys.argv) > curr_argv) and "--report" in sys.argv[curr_argv]:
  curr_argv += 1
  if (len(sys.argv) <= curr_argv):
    print "Must specify a test for test step report"
    sys.exit(-1)
  test = sys.argv[2]
  steps_info = get_steps_info([test])
  for k in sorted(map(lambda x: int(x),steps_info[test])):
    key = str(k)
    print "\t<teststep id=\"%i\" partition=\"%i\" repeat=\"1\">" % (int(key),steps_info[test][key]["p"])
    print "\t  <description>"
    print "\t      ", steps_info[test][key]["d"]
    print "\t  </description>"
    print "\t</teststep>"
  sys.exit(0)

# colors ###############################
if (len(sys.argv) > curr_argv) and "-nc" in sys.argv[curr_argv]:
  curr_argv += 1
  colors = False

# verbosity ###############################
if (len(sys.argv) > curr_argv) and "v" in sys.argv[curr_argv]:
  try:
    debug_level = int(sys.argv[curr_argv][2:])
    curr_argv += 1
  except:
    print "Incorrect debug level: must be in the range of 0 to 5"
    sys.exit(-1)
# determine execution tool ###############
ex_tool_index = "tsim_50"
if (len(sys.argv) > curr_argv) and "e" in sys.argv[curr_argv]:
  # parameter translation
  param =  sys.argv[curr_argv][2:] 
  if    param == "t40":
    ex_tool_index = "tsim_40"
  elif  param == "tg" or param == "tg50":
    ex_tool_index = "tsim_gdb50"
  elif  param == "tg40":
    ex_tool_index = "tsim_gdb40"
  elif  param == "g":
    ex_tool_index = "grmon"
  elif param == "gg":
    ex_tool_index = "grmon_gdb"
  elif param == "r1":
    ex_tool_index = "rasta1"
  elif param == "r2":
    ex_tool_index = "rasta2"
  curr_argv += 1

# finally, determine the execution tool
ex_tool = ex_tools[ex_tool_index]

# Debug execution? ######################
if (len(sys.argv) > curr_argv) and "-d" in sys.argv[curr_argv]:
  debug = True
  curr_argv += 1
else:
  debug = False

if (len(sys.argv) > curr_argv) and "-ima4br" in sys.argv[curr_argv]:
  ima4br = True
  curr_argv += 1
else:
  ima4br = False



# test to process #######################
# (if none specified, all will be runned)
if (len(sys.argv) > curr_argv):
  test = sys.argv[curr_argv]
else:
  test = ""

###########################################################
## Get Test List ##########################################
###########################################################   
if ima4br:
  tests = test_list
else: 
  tests = sorted(get_immediate_subdirectories("../../examples/validation/", "TEST-"))
  if len(test) > 0 and test in tests:
    tests = [test]

  if len(tests) <= 0 or (len(sys.argv) > 1 and sys.argv[1] in ["--help","-?"]):
    print Syntax
    print "[ERROR] No tests found!"
    sys.exit(-1)

###########################################################
## Cycle through all tests ################################
###########################################################
count = 0
total = 0
passed_tests_count = 0
failed_test_count  = 0
failed_list = []
reportResults = []


if colors:
  TEST_RESULTS_STR = [  Fore.RED    + "FAILED" + Fore.RESET
                      , Fore.GREEN  + "PASSED" + Fore.RESET
                      , Fore.RED    + "FAILED" + Fore.RESET]
else:
  TEST_RESULTS_STR = ["FAILED", "PASSED", "FAILED"]



for tname in tests:
  ## variable initialization
  total += 1
  passed = False
  reason_known = False
  ts_count  = -1
  initial_time = time.time()
  tool_output_buffer = []
  # get test steps description
  steps_info = get_steps_info([tname])
  
  ## define test path, and check if it exists
  test_exec = os.path.join(tname,executable);
  if os.path.isfile(test_exec):
    count += 1
  else:
    raise Exception("%s does not exist" % test_exec)

  ## ok, time to run the test
  try:
    if ex_tool_index in ["grmon", "grmon_gdb", "rasta1", "rasta2"]:
      command_line = ex_tool
      create_grmon_script(test_exec)
    else:
      command_line = ex_tool + " " + test_exec
    if debug_level in PRINT_STEP_LONG and debug_level in PRINT_STEPS_ALWAYS:
      print command_line
    tool_output_buffer.append(remove_tabs(command_line))
    outpipe=open("test_output", "w")
    p = subprocess.Popen (  command_line.split(), 
                            stdin=subprocess.PIPE,
                            stdout=outpipe,
                            stderr=outpipe,
                            universal_newlines=True)
  except:
    print "[ERROR] Error executing %s: please make sure it is in the PATH" % ex_tool.split()[0]
    sys.exit(-1)

  ## and now capture its results
  if not os.path.isdir("logs"):
    os.mkdir("logs")
  log_filename = os.path.join("logs",tname + ".txt")
  f = open (log_filename,"w")
  
  # some intermediate variables needed
  passed        = False
  output        = "dummy_non_empty_text"
  last_step_id  = -1
  test_ended    = False
  test_output_started = False
  external_lines = []
  # match external : if test is to be evaluated through external script, use this variable
  mtx = False
  # determine correct output buffer
  #outbuf = p.stdout
  outbuf = open("test_output", "r")
  if ex_tool_index == "grmon" or ex_tool_index == "grmon_gdb":
    outbuf = open(grmon_output,"r")
 
  # Wait for end of execution
  i		= 0

  while ((p.returncode == None) and (i < i_LIMIT)):
      time.sleep(SLEEPSEC)
      p.poll()
      i += 1

  if i == i_LIMIT:
    p.kill()

  ## Initialize test/teststep result report  
  ts_reported_errors = []

  ## for each line generated ######################
  while (output) and (not "**" in output) and (test_ended != True):
    ## get it and save it #########################
    output_ = outbuf.readline()
    output =  ansi_escape.sub('', output_)
    f.write(output)




    # Initialise local data structures ############
    ts_passed = False
    ts_str    = []


    ## extract relevant data ######################
    # test begin
    mtb = re.match("####(\d+)####",output)
    # test begin (post-interpreted)
    mti = re.match(".?###o(\d+)o###",output)
    # full test step status 
    mts = re.match("(\w{4})(\w{3})\|(\w{1})\|(\w{1})\:(\w{2})",output)
    # partial test step status 
    mtp = re.match("(\d{4})(\d{3})\n",output)
    # main test status #
    mtr = re.match("\*\*(\d+)\*\*",output)
    # Simulator (unexpected exit)
    mte = re.search("tsim\>\squit",output)
    # GRMon end 
    mtg = True if len(output) <= 0 else False 

    # internal script debug
    if len(output) > 0 and debug:
      print output,

    # process test begin
    if mtb or mti:
      test_output_started = True
      if debug_level in PRINT_STEPS_FAILED + PRINT_STEPS_ALWAYS:
        if colors:
          outstr = "\n" + Fore.CYAN + "%s test STARTED" % tname + Fore.RESET 
        else:
          outstr = "\n%s test STARTED" % tname
        print outstr
        tool_output_buffer.append(remove_tabs(outstr))

      if mti: # process test begin (for post-processing)
        mtx = True
        #print "POST-PROCESSED"
      continue

    # process test using external script #################
    elif mtx and not(mtr or mte or mtg):
      if mtp:
        # Decode data
        test_step_id    = int(mtp.group(1),16)
        test_step_rep   = int(mtp.group(2),16)
        # Long header
        if debug_level in PRINT_STEP_LONG and debug_level in PRINT_STEPS_ALWAYS:
          ts_str.append("\n" + steps_info[tname][str(test_step_id)]["d"])        
          # Common header
          ts_str.append("\tTS: %04i Rep: %03i Part: %i Result: N/A" % \
            (test_step_id,  test_step_rep, steps_info[tname][str(test_step_id)]["p"]))
          
        for ts in ts_str:
          print ts
          tool_output_buffer.append(remove_tabs(ts))
      else:
        external_lines.append(output)
        tool_output_buffer.append(remove_tabs(output))
    # process test step results ##################
    elif mts:

      # Decode data
      test_step_id    = int(mts.group(1),16)
      test_step_rep   = int(mts.group(2),16)
      test_step_res   = int(mts.group(3),16)
      test_step_type  = int(mts.group(4),16)
      test_step_value = int(mts.group(5),16)
      # Long header
      if debug_level in PRINT_STEP_LONG and last_step_id != test_step_id:
        ts_str.append("\n" + steps_info[tname][str(test_step_id)]["d"])        
        last_step_id = test_step_id
      # Common header
      ts_str.append("\tTS: %04i Rep: %03i Part: %i Result: %s" % \
         (test_step_id,  test_step_rep,  steps_info[tname][str(test_step_id)]["p"],
          TEST_RESULTS_STR[test_step_res]))
      # Determine test step result
      ts_passed = TEST_RESULTS[test_step_res]
      
      # if test step did not pass, register why...
      if not ts_passed:
        test_comp     = test_step_type & 3;
        test_valtype  = test_step_type >> 2
        if debug_level in PRINT_STEP_LONG :
          if test_valtype == 0:
            ts_str.append(\
              "\t%s %s" % ( TEST_VALUE_TYPE[test_valtype],   \
                            TEST_COMP_TYPE[test_comp]))
          else:
             ts_str.append(\
              "\t%s %s" % ( TEST_VALUE_TYPE[test_valtype] % test_step_value,\
                            TEST_COMP_TYPE[test_comp]))
        # ... and now report it
        if debug_level in PRINT_STEPS_FAILED + PRINT_STEPS_ALWAYS:
          for line in ts_str:
            print line
            ts_reported_errors.append(remove_tabs(line))
            tool_output_buffer.append(remove_tabs(line))
          ts_str = []
      # else, if debug level says so, report anyhow
      elif debug_level in PRINT_STEPS_ALWAYS:
        for line in ts_str:
          print line
          tool_output_buffer.append(remove_tabs(line))
        ts_str = []

    # process test end and its results ###################
    elif mtr or mtg :
      # final test result
      if mtx:   # post processing 
        test_result = test_post_processing.post_process(tname, external_lines,debug_level)
      else:
        test_result = int(mtr.group(1))
      passed = TEST_RESULTS[test_result]
      outstr = "%s test %s | %s" % (tname, TEST_RESULTS_STR[test_result],mtr.group(1) if mtr else "-")
      print outstr
      tool_output_buffer.append(remove_tabs(outstr))
    elif mte:
      if colors:
        print Fore.RED + "\tUnexpected simulation end" + Fore.RESET 
      else:
        print "\tUnexpected simulation end"
      ts_reported_errors = ['Unexpected Simulation End']
      test_ended = True 
    # process invalid output #################
    else:
      if test_output_started and remove_spaces(output) != "":
        test_ended = True    
        if colors:
          outstr = Fore.RED + "\tInvalid line: %s" % output + Fore.RESET 
        else:
          outstr = "\tInvalid line: %s" % output
        tool_output_buffer.append(remove_tabs(outstr))
        ts_reported_errors.append('Invalid output line from simulation')
      passed = False
  # end cycle
  ##########################################
  #Append test results to a vector 
  elapsed_time = time.time() - initial_time
  test_report = {
    'name': tname, 
    'passed': passed, 
    'reported_errors': ts_reported_errors,
    'time': elapsed_time,
    'tool_output_buffer': tool_output_buffer}

  reportResults.append(test_report)
  ts_reported_errors = []
  tool_output_buffer = []
  ##########################################
  try:
    p.terminate()
  except:
    pass
  f.close()

  # actualize statistic data
  if passed:
    passed_tests_count += 1
  else:
    failed_list.append(tname)
    failed_test_count += 1

###########################################################
# Print final report ######################################
###########################################################
print "\n"
print "Tests ran: %i/%i Successfull:%i Failed:%i Success rate:%i%%\n" \
    % (count,total,passed_tests_count,failed_test_count,100*passed_tests_count/total)
if failed_test_count:
  if colors:
    print Fore.RED + "Failed tests:" + Fore.RESET
  else:
    print "Failed tests:"
  print "*" * get_screen_width()
  for tf in failed_list:
    print tf,

generateReportXml(reportResults)

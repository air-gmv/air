"""@package logFile
Log file management module
"""

################################################################################
#  Project     : AIRII
#  Date        : 2010/09/09
#  Author      : BRSV
#  Version     : $Revision: 1.3 $
#  Description : This module is used to place definitions to log tool executions
#  History     : 
#  $Log: logFile.py,v $
#  Revision 1.3  2011/06/14 15:39:08  brsv
#  inclusion of doxygen comments
#
#  Revision 1.2  2011/03/03 15:19:58  brsv
#  *** empty log message ***
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
#--------------------------[ IMPORTS ]------------------------------------------

from globalUtils import sys, definitions
from time import gmtime, asctime, strftime

#--------------------------[ GLOBAL VARIABLES ]---------------------------------

# General use global variables
gNumOfOccurrences = 0
gErrorCount       = 0

# Time macros
EXE_TIME      = gmtime()
EXE_TIME_FORM = asctime()

LOG_FILE_HEADER = "###########################################################"\
"#####################" +\
"""
#  Project     : PMK Config Files Generator V%s
#  Time        : %s
#  Description : This report file contains the traces from all tool actions.
################################################################################
\n
""" % (definitions.VERSION, "%s")

LOG_SEPARATOR   = "\n\n ** %s ** \n" + "-"*80 + "\n ** %s ** \n\n"

LOG_FILE_FOOTER = """\n
################################################################################
#  TESTS FINISHED. END OF LOG FILE @ %s
################################################################################
"""

LOG_FILE_NAME   = "logfile.log"
FUNC_LOG_ENTRY  = "Entering in function: "
FUNC_LOG_EXIT   = "Leaving function: "
FUNC_LOG_ERROR  = "="*80 + """\n>> ## ERROR ##
# %s %s
================================================================================
"""
FUNC_LOG_INFO   = "INFO: %s"
CLOCK_TIME      = " === " + "%s" + " ==="
TOTAL_ERRORS_FOUND  = " (!) Total number of errors: "
SUCCESS_NO_ERROR    = "Process terminated without errors"

# File descriptor
LOG_FILE_FD     = None

#--------------------------[ LOCAL DEFINITIONS ]--------------------------------

#===============================================================================
# Prints in the system output the message given in user colour
#===============================================================================
def pprintLog(printMsg, colour, guiHandler=None):
  """
  Prints in the system output the message given in user colour
  """
  
  if guiHandler == None:
    
    sys.stdout.write("%s%s%s" % (definitions.colours[colour], 
                                             str(printMsg), 
                                             definitions.colours['default']))
    sys.stdout.flush()
    
  else:
    guiHandler.outFrameObj.set(printMsg, colour)

#===============================================================================
# Get the current system time
#===============================================================================
def getCurrentTime(format=True):
  """
  Obtain the current time system in text format or list 
  (see gmtime specification) 
  """
  
  if format == True:
    return EXE_TIME_FORM
  else:
    return EXE_TIME
  
#===============================================================================
# Get the current system time in a clock format
#===============================================================================
def getCurrentClockTime():
  """
  Obtain the current time string in format hour, minute, seconds
  """
  
  return strftime("%H:%M:%S", EXE_TIME)

#--------------------------[ FUNCTIONS IMPLEMENTATION ]-------------------------

#===============================================================================
# Create a log file in output folder
#===============================================================================
def logFileInit(guiHandler=None):
  """
  Create a log file in output folder  
  """ 
  
  global LOG_FILE_FD
  global gErrorCount
  
  gErrorCount = 0
  
  try:
    logFilePath = definitions.os.path.join(definitions.OUTPUT_FOLDER, 
                                           LOG_FILE_NAME)
    LOG_FILE_FD = open(logFilePath, "w")
    
    MSG = LOG_FILE_HEADER % getCurrentTime()
    LOG_FILE_FD.write(MSG)
    
    return True
      
  except: 
    pprintLog("Unable to create the log file (%s)" \
              % sys.exc_info()[1], 'red', guiHandler) 
    # Close file if open
    if LOG_FILE_FD != None:
      LOG_FILE_FD.close()
    return False
      
#===============================================================================
# Close the log file
#===============================================================================
def logFileClose(guiHandler=None):
  """
  Close the log file
  """
  
  global LOG_FILE_FD
  global gErrorCount
  
  if LOG_FILE_FD != None:
    
    pprintLog(definitions.END_PROCESSING, 'bold', guiHandler)
    
    if gErrorCount > 0:
      pprintLog("\n" + TOTAL_ERRORS_FOUND, 'bold', guiHandler)
      pprintLog(" %d\n" % gErrorCount, 'red', guiHandler)
      pprintLog("\n (!) Check logfile for more related information\n", 'bold', 
                guiHandler)
      
      LOG_FILE_FD.write(TOTAL_ERRORS_FOUND + " %d" % gErrorCount)
      
    else:
      pprintLog("\n" + SUCCESS_NO_ERROR, 'green', guiHandler)
    
    MSG = LOG_FILE_FOOTER % getCurrentTime()
    LOG_FILE_FD.write(MSG)
    # Close file
    LOG_FILE_FD.close()
    
#===============================================================================
# Return the current errors counter
#===============================================================================
def getCurrentErrorCount():
  """
  Return the current errors counter
  """
  
  global gErrorCount
  
  return gErrorCount

#===============================================================================
# Write a separator in the log file
#===============================================================================
def writeSeparatorLog():
  """
  Write a separator in the log file 
  """
  
  global LOG_FILE_FD
  
  if LOG_FILE_FD != None:
    LOG_FILE_FD.write(LOG_SEPARATOR % (definitions.CONFIG_FILE_LOADED, 
                                       definitions.FILE_CREATION_START))
  else:
    print "Error (writeErrorLog): write error log, log file not open\n"

#===============================================================================
# Write error in the log file
#===============================================================================
def writeErrorLog(logEntry):
  """
  Write logEntry error in the log file 
  """
  
  global LOG_FILE_FD
  global gErrorCount
  
  gErrorCount += 1
  
  MSG = FUNC_LOG_ERROR % (str(logEntry), CLOCK_TIME % getCurrentClockTime())
                     
  if LOG_FILE_FD != None:
    LOG_FILE_FD.write(MSG)
  else:
    print "Error (writeErrorLog): write error log, log file not open\n"
      
#===============================================================================
# Write information in the log file the logEntry 
#===============================================================================
def writeInfoLog(logEntry):
  """
  Write information in the log file the logEntry 
  """
  
  global LOG_FILE_FD
  global gErrorCount
  
  MSG = FUNC_LOG_INFO % str(logEntry) + "\n"
                     
  if LOG_FILE_FD != None:
    LOG_FILE_FD.write(MSG)
  else:
    print "Error (writeInfoLog): write error log, log file not open\n"

#===============================================================================
# A Handler to capture the keyboard interrupt
#===============================================================================
#def ctrlCHandler():
#  """
#  A Handler to capture the keyboard interrupt
#  """
#  
#  if LogFile.opened == True:
#    logEntry = "A ctrl-c key was pressed, program will now exit!\n"
#    MSG = str(logEntry) + CLOCK_TIME % getCurrentClockTime() +"\n"
#    LogFile.logMessage.append(MSG)
#    msgLog = string.join(LogFile.logMessage, "")
#    LogFile.fdIn.write(msgLog)
#    LogFile.fdIn.flush()
#    
#    # Close file
#    LogFile.fdIn.close()
##    os.system(utils.CHMOD_CMD % LogFile.path)
#    LogFile.opened = False
#    
#  else:
#    print "Error (writeInfoLog): write error log, log file not open\n"

# FILE END 

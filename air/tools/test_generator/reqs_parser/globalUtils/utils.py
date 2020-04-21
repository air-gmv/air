"""@package utils
Holds all the global functions needed for the entire application 
"""
################################################################################
#  Project     : AIRII
#  Date        : 2010/09/09
#  Author      : BRSV
#  Version     : $Revision: 1.3 $
#  Description : This module is used to place definitions for python module.
#  History     : 
#  $Log: utils.py,v $
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
#  partitionFilesHandler added. Partition files are now generated (ports, 
#  init and makefile)
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

from globalUtils import sys, os, logFile
from globalUtils.definitions import *
import decimal                  # For operations with floats
import subprocess               # For the app xmllint system call
import shutil

#===============================================================================
# This function prints the usage information for this script.
#===============================================================================
def usage():
  print USAGE_MSG

#===============================================================================
# Prints in the system output the message given in bold formatted
#===============================================================================
def pprint(printMsg, colour, guiHandler=None):
  """
  Prints in the system output the message given in user colour
  """

  if guiHandler == None:

    sys.stdout.write("%s%s%s" % (colours[colour], str(printMsg),
                                 colours['default']))
    sys.stdout.flush()

  else:
    guiHandler.outFrameObj.set(printMsg, colour)


#===============================================================================
# Convert a string representing the value in seconds to the same value in 
# milliseconds 
#===============================================================================
def convertToMs(stringIn):
  """
  Convert a string representing the value in seconds to the same value in 
  milliseconds 
  """

  intOut = None

  try:
    floatNb = float(stringIn)
    intOut = int(floatNb * MIL_SECOND)
  except:
    intOut = None
    logFile.writeErrorLog('Unable to convert the string "%s"' % stringIn)

  return intOut

#===============================================================================
# Convert a string representing the value in seconds to the same value in 
# nanoseconds 
#===============================================================================
def convertToNs(stringIn):
  """
  Convert a string representing the value in seconds to the same value in 
  nanoseconds 
  """

  intOut = None

  try:
    floatNb = float(stringIn)
    intOut = int(floatNb * NANO_SECOND)
  except:
    intOut = None
    logFile.writeErrorLog('Unable to convert the string "%s"' % stringIn)

  return intOut

#===============================================================================
# Convert a string to a int value and return it
#===============================================================================
def convertStringToInt(stringIn, base=10):
  """
  Convert a string to a int value and return it
  """

  intOut = None
  try:  # if parameter is already an int, having the base parameter implies _exception raised_,
        # instead of silent copy!
    if isinstance(stringIn, int):
      intOut = stringIn
    else:
      intOut = int(stringIn, base=base)
  except:
    logFile.writeErrorLog('Unable to convert the string "%s" to int' % stringIn)

  return intOut

#===============================================================================
# Convert a string to a boolean value and return it
#===============================================================================
def convertStringToBoolean(stringIn):
  """
  Convert a string to a boolean value and return it
  """
  boolOut = False
  try:
    if isinstance(stringIn, int):
      if stringIn in [0, 1]:
        boolOut = stringIn
      else:
        raise Exception('Not a boolean', '%d' % stringIn)
    elif stringIn in ["False", "false"]:
      boolOut = 0
    elif stringIn in ["True", "true"]:
      boolOut = 1
    else:
      intOut = int(stringIn)
      if intOut in [0, 1]:
        boolOut = intOut
      else:
        raise Exception('Not a boolean', '%d' % intOut)
  except:
    logFile.writeErrorLog('Unable to convert the string "%s" to boolean' % stringIn)

  return boolOut

#===============================================================================
# Convert a string to a int value and return it
#===============================================================================
def convertStringToFloat(stringIn):
  """
  Convert a string to a int value and return it
  """

  floatOut = None

  try:
    floatOut = decimal.Decimal(stringIn)
  except:
    logFile.writeErrorLog('Unable to convert the string "%s" to float' %
                          stringIn)

  return floatOut

#===============================================================================
# Compare two floats
#===============================================================================
def compareFloat(x1, x2):
  """
  Compare two floats
  OUTPUT:
    * 0   - If equal
    * 1   - If greater than
    * -1  - If less than 
  """

  if decimal.Decimal(x1) == decimal.Decimal(x2):
    return 0
  elif decimal.Decimal(x1) < decimal.Decimal(x2):
    return -1
  else:
    return 1

#===============================================================================
# Returns the content of a directory by showing directories first
# and then files by ordering the names alphabetically
#===============================================================================
def custom_listdir(path, pattern=None, dirsOnly=False):
  """
  Returns the content of a directory by showing directories first
  and then files by ordering the names alphabetically
  Input:
    * pattern - If pattern is given it will return only the files containing 
                part of the pattern in their name files
  """

  if os.path.exists(path) == True:
    if pattern == None:
      # Directories
      dirs = sorted([d for d in os.listdir(path)
                     if os.path.isdir(path + os.path.sep + d)])
      # Exclude files if flag is active
      if dirsOnly == False:
        # Files
        dirs.extend(sorted([f for f in os.listdir(path)
                            if os.path.isfile(path + os.path.sep + f)]))
    else:
      # Directories
      dirs = sorted([d for d in os.listdir(path)
                     if (os.path.isdir(path + os.path.sep + d) and
                         (d.find(pattern) != -1))])
      # Exclude files if flag is active
      if dirsOnly == False:
        # Files
        dirs.extend(sorted([f for f in os.listdir(path)
                      if (os.path.isfile(path + os.path.sep +
                                         f) and (f.find(pattern) != -1))]))
  else:
    print "\nWARNING: Path %s does not exist!\n" % path
    dirs = []

  return dirs

#===============================================================================
# Return the output from the ARINC 653 config file schema validation
#===============================================================================
def schemaValidation(schemaFile, a653CfgFile, guiHandler=None):
  """
  Return the output from the ARINC 653 config file schema validation
  """

  try:
    proc = subprocess.Popen(["xmllint", "--noout", "--nowarning", "--schema",
                             schemaFile, a653CfgFile], stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
    proc.wait()
    output = proc.communicate()[1].split(a653CfgFile + ":")

    if len(output) > 1:
      pprint("="*80, 'default', guiHandler)
      pprint("\n## Errors reported by schema validator:\n", 'red', guiHandler)
      for line in output[1:]:
        pprint(" + Line:", 'bold', guiHandler)
        pprint(line, 'default', guiHandler)

      pprint("="*80, 'default', guiHandler)

    if proc.returncode != NO_ERROR_SIGNAL:
      logFile.writeErrorLog("Validation process return an unexpected end "\
                            "signal (%d)" % proc.returncode)
      return False
  except:
    errorMsg = "Unable to process the schema validation! Possible reason: "\
                "option unavailable for windows"
    logFile.writeErrorLog(errorMsg)
    pprint(errorMsg, 'red', guiHandler)
    del errorMsg
    return False

  return True

#===============================================================================
# Create the system path if not exist
#===============================================================================
def createFolder(pathToCreate, printThis=False):
  """
  Create the system path if not exist
  """

  if printThis:
    logFile.writeInfoLog("Creating %s .." % pathToCreate)

  try:
    os.mkdir(pathToCreate)
  except:
    logFile.writeErrorLog(sys.exc_info()[1])
    return False

  return True

#===============================================================================
# custom_move
#===============================================================================
def custom_move(srcFolder, dstFolder):
  """
  Move an entire folder into a destination in which if already exists it will
  overwrite it's contents
  """
  names = os.listdir(srcFolder)

  errors = []
  for name in names:
    srcname = os.path.join(srcFolder, name)
    dstname = os.path.join(dstFolder, name)
    try:
      if os.path.isdir(srcname):
        if not (os.path.exists(dstname)):
          # create folder
          createFolder(dstname)
        custom_move(srcname, dstname)
      else:
        shutil.copy2(srcname, dstname)
    except (IOError, os.error), why:
      logFile.writeErrorLog(why)
      errors.append((srcname, dstname, str(why)))
    # catch the Error from the recursive copytree so that we can
    # continue with other files
    except shutil.Error, err:
      errors.extend(err.args[0])
  try:
    shutil.copystat(srcFolder, dstFolder)
  except WindowsError:
    # can't copy file access times on Windows
    pass
  except OSError, why:
    logFile.writeErrorLog(why)
    errors.extend((srcFolder, dstFolder, str(why)))
  if errors:
    raise shutil.Error(errors)

#===============================================================================
# moveOutputFiles
#===============================================================================
def moveOutputFiles(moduleName, finalFolder):
  """
  Create the module output folder
  """

  resValue = True

  if finalFolder:
    try:
      moduleFolderToMove = os.path.join(OUTPUT_FOLDER, moduleName)

      if(os.path.exists(moduleFolderToMove)):
        print moduleFolderToMove
        custom_move(moduleFolderToMove, finalFolder)
      else:
        logFile.writeErrorLog("Failed to move output folder. Path (%s) does "\
                              "not exist!" % moduleFolderToMove)
    except IOError, why:
      logFile.writeErrorLog(why)
      resValue &= False

  return resValue

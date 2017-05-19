## @package utils
## @package utils
# A bunch of utility functions

from sys import exit
from hashlib import sha256
from localization.common import *
from shutil import copy2, rmtree
from mako.template import Template
from subprocess import Popen, PIPE
from collections import namedtuple
from os import path, makedirs, walk, system, getenv, getcwd, listdir, remove

LOGGER_FILE			= 'logfile.log'
LOGGER_ERROR 		= 'error'
LOGGER_WARNING		= 'warning'
LOGGER_SUCCESS		= 'green'
CLEAN_RECORD_FILE	= '.configclean'

# List of the available colors
COLORS = { 'none'       	:    '',
           'default'    	:    '\033[0m',
           'bold'       	:    '\033[1m',
           'underline'  	:    '\033[4m',
           'blink'      	:    '\033[5m',
           'reverse'    	:    '\033[7m',
           'concealed'  	:    '\033[8m',
           'black'      	:    '\033[30m',
           'red'        	:    '\033[31m',
           'green'      	:    '\033[32m',
           'yellow'     	:    '\033[33m',
           'blue'       	:    '\033[34m',
           'magenta'    	:    '\033[35m',
           'cyan'       	:    '\033[36m',
           'white'      	:    '\033[37m',
           'purple'			:	 '\033[95m',
           'beep'       	:    '\007',
           'dark'         	:    '\033[2m',
           'italic'       	:    '\033[3m',
           'rapidblink'   	:    '\033[6m',
           'strikethrough'	:    '\033[9m',
           'error'        	:    '\033[31m',	# Same as red
           'warning'     	:    '\033[33m',	# Same as yellow
         }

## Class to generate container objects
class Container(object):

    def __str__(self):

        # Get attributes
        attrs = dir(self)

        lines = ['Container:']
        for a in attrs:
            if not a.startswith('__'):
                lines.append('  {0}: {1}'.format(a, getattr(self, a)))

        return '\n'.join(lines) + '\n'

    def __repr__(self):

        return self.__str__()


## Handle user input arguments
# @param description Description of the tool
def handleInputArguments(description):

    from argparse import ArgumentParser, RawTextHelpFormatter
    parser = ArgumentParser(description = description, formatter_class = RawTextHelpFormatter)
    parser.add_argument('xml_file', metavar ='[<xml file> | clean]', nargs = '?', default = None, help = INPUT_ARG_INPUT_HELP)
    parser.add_argument('-o', '--output', dest = 'output'   , default = None		,  help = INPUT_ARG_OUTPUT_HELP)
    parser.add_argument('-s', '--support',dest = 'support'  , action = 'store_true'	, help = INPUT_ARG_SUPPORT_HELP)
    parser.add_argument('-f', '--force',  dest = 'force',	 action = 'store_true'	, help = INPUT_ARG_FORCE_HELP)
    parser.add_argument('-m', '--mode',   dest = 'mode'		, choices=['strict', 'relaxed', 'no-schema'], default='strict', help = INPUT_ARG_MODE_HELP)
    parser.add_argument('-v', '--verbose',dest = 'verbose', choices =['1', '2', '3'], default='2', help = INPUT_ARG_VERBOSE_HELP)
    parser.add_argument('-g', '--graph',  dest = 'graph',	 action = 'store_true'	, help = INPUT_ARG_GRAPH_HELP)
    parser.add_argument('-d', '--default',dest = 'default',  action = 'store_true', help = INPUT_ARG_DEFAULT)
    args = parser.parse_args()

    return args



## Asks user form confirmation action
# @param prompt Action to prompt to the user
# @param default Default action to take (True: yes, False: no)
# @return Confirmation (True or False)
def promptConfirm(prompt, default = False):

    # Setup the prompt options
    fmt = (COMMON_YES, COMMON_NO) if default else (COMMON_NO, COMMON_YES)
    prompt = '{0} [{1}]|{2}: '.format(prompt, *fmt)

    # Ask for user confirmation
    while True:
        ans = raw_input(prompt).lower()
        if ans == '':
            return default
        elif ans == COMMON_YES.lower():
            return True
        elif ans == COMMON_NO.lower():
            return False
        else:
            print(COMMON_CONFIRM_ERR)



## Print colored text
# @param string Text string to convert
# @param color Color of the text
def cprint(string, color = 'none'):
    print('{0}{1}{2}'.format(COLORS[color.lower()], string, COLORS['default']))



## Safely create directory paths, and asks the user to try again in case of errors
# @param directory Directory to create
# @param logger Logger to Log the actions and acknowledge errors
def safeMakeDir(directory, logger):

    # Try to create the directory
    try:
        if not path.exists(directory): makedirs(directory)

    # Error creating directory
    except Exception, why:
        logger.write(1, '{0}'.format(why), LOGGER_ERROR)
        logger.errorFlag = True

## Safe file copy, with user interactivity
# @param src Source file
# @param dst Destination file
# @param logger Logger to register the events
# @param force Force the file copy
def safeFileCopy(src, dst, logger, force = False, followDefault = False):

    src = path.normpath(src)
    dst = path.normpath(dst)

    # Copy the file interactively
    if not force and path.exists(dst) and genFileS256Hash(src) != genFileS256Hash(dst):
        print('')
        print('-' * getTerminalSize()[1])
        while True:
            result = promptActions(FILE_COPY_EXISTS.format(dst),
                                   [FILE_COPY_REPLACE, FILE_COPY_KEEP, FILE_COPY_MERGE, FILE_COPY_DIFF], 1, followDefault)

            # Replace file
            if result == 0:
                try:
                    copy2(src, dst)
                    logger.write(1, FILE_COPY_OK.format(dst))
                except Exception, why:
                    logger.write(1, str(why), LOGGER_ERROR)
                    logger.errorFlag = True

                return

            # Keep file
            if result == 1:
                logger.write(1, FILE_COPY_OLD_KEEP.format(dst), 'yellow')
                return

            # Merge files
            if result == 2:
                proc = Popen([getenv('MERGETOOL', 'meld'), src, dst], stdout = PIPE, stderr = PIPE)
                proc.wait()
                if proc.returncode == 0:
                    logger.write(1, FILE_COPY_MERGED.format(src), 'yellow')
                    return
                else:
                    cprint(FILE_COPY_MERGE_ERROR.format(proc.returncode), LOGGER_ERROR)

            # View file diff
            if result == 3:
                system(' '.join(['diff -wEB --strip-trailing-cr', dst, src]))

    # Just try to copy the file
    try:
        copy2(src, dst)
    except Exception, why:
        logger.write(1, str(why), LOGGER_ERROR)
        logger.errorFlag = True

## Safe multi file copy, with user interactivity
# @param src Source file
# @param dst Destination file
# @param logger Logger to register the events
# @param force Force the file copy
# @param removeSrc Flag to remove src files after copy
def safeMultiFileCopy(src, dst, logger, force = False, removeSrc = True, followDefault = False):

    # Copy files
    for p, ds, fs in walk(src):
        rp = path.relpath(p, src)

        # Create destination directory
        if rp != '.':
            safeMakeDir(path.join(dst, rp), logger)

        # Process files in source directory
        for f in fs:
            fsrc = path.join(src, rp, f)
            fdst = path.join(dst, rp, f)
            safeFileCopy(fsrc, fdst, logger, force, followDefault)

    # Remove the source directory
    if not logger.haveErrors() and removeSrc:
        rmtree(src, ignore_errors = True)

## Save file remove
# @param file File to remove
# @param logger Logger to register the events
def safeRemove(file, logger):
    try:
        remove(file)
    except Exception, why:
        logger.write(1, str(why), LOGGER_ERROR)


def createFromMakoTemplate(input, output, v_dict, logger):

    # Log Action
    pIn  = path.join(path.basename(path.dirname(input)), path.basename(input))
    pOut =  path.join(path.basename(path.dirname(output)), path.basename(output))
    #logger.write(2, '{0:40} -> {1:40}'.format(pIn, pOut))

    # Mako template load
    try:
        mytemplate = Template(filename = input)
    except Exception, why:
        logger.error(str(why))
        return

    # add output file name to the template parameters
    v_dict["output_file"] = pOut

    try:
        file_ctx = mytemplate.render(**v_dict)
    except:
        from mako import exceptions
        logger.error('Error processing \'{0}\' template', pIn)
        if promptConfirm('View debug trace?', False):
            print(exceptions.text_error_template().render())
        return

    # Generate file
    try:
        fd = open(output, 'w')
        fd.write(file_ctx)
        fd.close()
    except:
        logger.error('Error creating \'{0}\' from template', pOut)
        return

## Gets the current terminal size
# @return (Number of rows, Number of columns)
def getTerminalSize():
    import os, struct
    def ioctl_GWINSZ(fd):
        import fcntl, termios
        return struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))

    # try stdin, stdout, stderr
    def fromStd():
        for fd in (0, 1, 2):
            try:
                return ioctl_GWINSZ(fd)
            except:
                pass
        return ()

    # try os.ctermid()
    def fromCtermId():
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            try:
                size = ioctl_GWINSZ(fd)
                return
            finally:
                os.close(fd)
        except:
            return ()

    # try `stty size`
    def fromStty():
        try:
            return tuple(int(x) for x in os.popen('stty size', 'r').read().split())
        except:
            return ()

    # try environment variables
    def fromEnv():
        try:
            return tuple(int(os.getenv(var)) for var in ('LINES', 'COLUMNS'))
        except:
            return ()

    size = fromStd()
    if len(size) == 2: return size
    size = fromCtermId()
    if len(size) == 2: return size
    size = fromStty()
    if len(size) == 2: return size
    size = fromEnv()
    if len(size) == 2: return size

    # i give up. return default.
    return (25, 80)

## Pretty prints the tool welcome messages
# @param welcomeMsgs List of welcoming messages
# @param version Version of the tool
def printWelcomeBox(welcomeMsgs, version):
    width = getTerminalSize()[1]
    cprint('+' + '-' * (width - 2) + '+', 'green')
    for msg in welcomeMsgs:
        msg = msg.format(version)
        cprint('|' + msg + ' ' * (width - 2 - len(msg)) + '|', 'green')
    cprint('+' + '-' * (width - 2) + '+', 'green')


def getPaths(inputFile, outputDirectory):

    # Get absolute path to the input file
    inputFile = path.abspath(inputFile);

    # Check if input file exists and is that is a file
    if not path.isfile(inputFile):
        cprint(CONFIGURATOR_INVALID_INPUT.format(inputFile), 'red')
        exit(-1)

    # Check if an output directory is defined
    if not outputDirectory:
        return inputFile, path.dirname(inputFile)

    # Get the absolute path to the output directory
    outputDirectory = path.abspath(outputDirectory);

    # Create output folder
    if not path.isdir(outputDirectory):
        try:
            makedirs(outputDirectory)
        except:
            cprint(CONFIGURATOR_INVALID_OUTPUT.format(outputDirectory), 'red')
            exit(-1)

    # Copy xml file to output if the output directory doesn't match the
    # original file directory
    if path.dirname(inputFile) != outputDirectory:
        new_input = path.join(outputDirectory, path.basename(inputFile))

        # Check if the target file already exists
        if path.isfile(new_input) and not \
           promptConfirm(CONFIGURATOR_REPLACE_XML, True):
            cprint(CONFIGURATOR_ABORTED_BY_USER, 'red')
            exit(-1)

        try:
            copy2(inputFile, outputDirectory)
        except:
            cprint(CONFIGURATOR_INVALID_OUTPUT.format(outputDirectory), 'red')
            exit(-1)

        return inputFile, outputDirectory

    return inputFile, outputDirectory

## Run a task and performs a checkpoint over warnings and errors
# @param tool Name of the tool
# @param task Task to perform [ name, taskFunction ]
# @param logger Logger to register the events
# @param abortOnWarnings Flag to abort the checkpoint if warnings here found
# @param abortOnErrors Flag to abort the checkpoint if errors here found
def runTask(tool, task, logger, abortOnWarnings = False, abortOnErrors = True):

    # Run task
    task[1]()

    # Run check point
    errors = (logger.errors > 0 and abortOnErrors) or (logger.warnings > 0 and abortOnWarnings)
    logger.writeTitle(SANITY_CHECK_TITLE.format(tool))
    logger.setIndent('  ')
    if logger.errors   > 0: logger.write(1, SANITY_CHECK_ERRORS.format(logger.errors, task[0]), 'red')
    if logger.warnings > 0: logger.write(1, SANITY_CHECK_WARNINGS.format(logger.warnings, task[0]), 'yellow')
    if not errors: logger.write(1, '{0}: Successful'\
        .format(task[0][0].upper() + task[0][1:]), LOGGER_SUCCESS)

    # Reset errors and warnings count
    logger.errors 	= 0
    logger.warnings = 0

    return not errors

## Gens a Hexadecimal SHA256 hash string of a file
# @param filepath File path
# @return Hexadecimal SHA256 hash string
def genFileS256Hash(filepath):

    # Open file
    try:
        fd = open(filepath, 'r+')
    except:
        return ''

    # Create hash
    hasher = sha256()
    buf = fd.read(65536)
    while len(buf) > 0:
        hasher.update(buf)
        buf = fd.read(65536)
    fd.close()
    return hasher.hexdigest()





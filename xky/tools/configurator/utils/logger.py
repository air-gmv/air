## @package logger
# Logger object thats allows writing to a file, and also allows the logger
# to be echo'ed on the console
#

from time import asctime
from textwrap import TextWrapper
import terminal as terminalutils
import logging

LEVEL_BASE				                    = 4     # Base level indentation
LEVEL_WIDTH				                    = 2     # Indentation increment

## Logger object
class Logger(object):

    ##
    # @brief Constructor
    # @param self object pointer
    def __init__(self, silent=False):
        self.level = 0
        self.errors = 0
        self.warnings = 0
        self.silent = silent
        self.total_errors = 0
        self.total_warnings = 0
        self.logger = logging.getLogger('air_configurator')

    ##
    # @brief Desctructor
    # @param self object pointer
    def __del__(self):

        if not self.silent:
            terminalutils.pprint('\n')

    ##
    # @brief Outputs an event
    # @param self object pointer
    # @param level level of indentation
    # @param fmt format string
    # @param args format arguments
    def event(self, level, fmt, *args):
        self.logger.info(fmt)
        if not self.silent:
            terminalutils.pprint('\n{0}~b{1}~n\n'.format(
                ' ' * (LEVEL_BASE if level == 1 else LEVEL_BASE + (level - 1) * LEVEL_WIDTH), fmt), *args)
        self.level = level + 1

    ##
    # @brief Outputs an event information
    # @param self object pointer
    # @param level level of indentation
    # @param fmt format string
    # @param args format arguments
    def information(self, level, fmt, *args):
        self.logger.debug(fmt)
        if not self.silent:
            color = 'c'
            terminalutils.pprint('{0}~{1}{2}~n'.format(
                ' ' * (LEVEL_BASE if level == 1 else LEVEL_BASE + (level - 1) * LEVEL_WIDTH), color, fmt), *args)
        self.level = level + 1

    ##
    # @brief Outputs a warning
    # @param self object pointer
    # @param fmt format string
    # @param args format arguments
    def warning(self, fmt, *args):
        self.warnings += 1
        self.total_warnings += 1
        self.logger.warning(fmt)
        if not self.silent:
            terminalutils.pprint('{0}~y{1}~n'.format(
                ' ' * (LEVEL_BASE if self.level == 1 else LEVEL_BASE + (self.level - 1) * LEVEL_WIDTH), fmt), *args)

    ##
    # @brief Outputs an error
    # @param self object pointer
    # @param fmt format string
    # @param args format arguments
    def error(self, fmt, *args):
        self.errors += 1
        self.total_errors += 1
        self.logger.error(fmt)
        if not self.silent:
            terminalutils.pprint('{0}~r{1}~n'.format(
                ' ' * (LEVEL_BASE if self.level == 1 else LEVEL_BASE + (self.level - 1) * LEVEL_WIDTH), fmt), *args)

    ##
    # @brief Check for logged errors and clear them
    # @param self object pointer
    # @return true if errors occurred
    def check_errors(self):
        rc = False
        if self.errors > 0: rc = True
        self.errors = self.warnings = 0
        return rc

    ##
    # @brief Clear logged errors
    # @param self object pointer
    # @param level level of indentation
    def clear_errors(self, level = None):
        if level is not None: self.level = level + 1
        self.errors = self.warnings = 0



## Logger
# Logger object that's allows writing to a file, and also allows the logger
# to be echo'ed on the console
class Logger1:

    fd = None			# Logger file descriptor
    to_stdin = True		# Flag to echo on the console
    level = 1			# Level of verbosity
    errors = 0			# Error count
    warnings = 0		# Warning count
    width	= 80

    errorFlag = False	# Flag to provide awareness of multiple errors

    ##
    # @brief Logger constructor
    # @param self The object pointer
    # @param logfile Logfile to store the logs on disc
    # @param to_stdin Flag to enable the logger to echo'ed on the console
    def __init__(self, level = 3, logfile = None, to_stdin = True):

        self.level = level
        self.to_stdin = to_stdin
        self.width = terminalutils.getTerminalSize()[1]
        self.screenWrapper = TextWrapper(initial_indent = '', subsequent_indent = '', width = self.width)
        self.logFileWrapper = TextWrapper(initial_indent = '', subsequent_indent = '', width = 80)

        # Open log file
        if logfile:
            try:
                self.fd = open(logfile, 'w+')
            except IOError, why:
                self.fd = None
                self.to_stdin = True
                self.write(1, '{0}'.format(why), 'warning')

    ##
    # @brief Logger destructor
    # @param self The object pointer
    def __del__(self):

        # Ensure that the file is closed if the logger is claimed
        # by the garbage collector
        self.close()

    def writeHeader(self, msgs, version):

        # Write on file
        if self.fd:
            try:
                self.fd.write('-' * self.logFileWrapper.width + '\r\n')
                for msg in msgs: 	self.fd.write(msg.format(version, asctime()) + '\r\n')
                self.fd.write('-' * self.logFileWrapper.width + '\r\n')
            except:
                self.logger.exception("Exception, See details below")
                self.close()

        # Write on screen
        if self.to_stdin:
            terminalutils.cprint('-' * self.screenWrapper.width)
            for msg in msgs: terminalutils.cprint(msg.format(version, asctime()))
            terminalutils.cprint('-' * self.screenWrapper.width)


    def writeTitle(self, title):

        # Write on file
        if self.fd:
            try:
                self.fd.write('\r\n= {0} {1}\r\n\r\n'.format(title, '=' * (80 - 3 - len(title))))
            except:
                self.logger.exception("Exception, See details below")
                self.close()

        # Write on screen
        if self.to_stdin:
            terminalutils.cprint('')
            terminalutils.cprint('= {0} {1}'.format(title, '=' * (self.width - 3 - len(title))), 'bold')
            terminalutils.cprint('')


    ##
    # @brief Write in the logger
    # @param self The object pointer
    # @param level Level of Verbosity
    # @param msg Message to write on the logger
    # @param color Color of the text
    def write(self, level, msg, color = 'default', indent = None, subindent = None, wrapper = True):

        if indent is not None:
            self.screenWrapper.initial_indent = indent
            self.logFileWrapper.initial_indent = indent
            self.screenWrapper.subsequent_indent = indent
            self.logFileWrapper.subsequent_indent = indent

        # Append warning flag
        if color == 'warning':
            self.warnings += 1
            msg = 'WARNING: {0}'.format(msg)

        # Append error flag
        if color == 'error':
            self.errors += 1
            msg = 'ERROR: {0}'.format(msg)

        # Write on file
        if self.fd and level <= self.level:
            try:
                self.fd.write(('\r\n'.join(self.logFileWrapper.wrap(msg)) if wrapper else msg) + '\r\n')
            except:
                self.logger.exception("Exception, See details below")
                self.close()

        # Write on screen
        if self.to_stdin and level <= self.level:
            terminalutils.cprint('\r\n'.join(self.screenWrapper.wrap(msg)) if wrapper else msg, color)

        if subindent is not None:
            self.screenWrapper.initial_indent = subindent
            self.logFileWrapper.initial_indent = subindent
            self.screenWrapper.subsequent_indent = subindent
            self.logFileWrapper.subsequent_indent = subindent

    def setIndent(self, indent):
        self.screenWrapper.initial_indent = indent
        self.logFileWrapper.initial_indent = indent
        self.screenWrapper.subsequent_indent = indent
        self.logFileWrapper.subsequent_indent = indent

    def haveErrors(self, reset = True):
        ret = self.errorFlag
        if reset: self.errorFlag = False
        return ret

    ##
    # @brief Close the logger
    # @param self The object pointer
    def close(self):

        # Close log file if possible
        if self.fd:
            try:
                self.fd.close()
            except:
                self.logger.exception("Exception, See details below")
                pass

        # Mark file as closed
        self.fd = None

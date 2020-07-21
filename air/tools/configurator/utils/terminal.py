## @package utils.print
#  @author pfnf
#  @brief Print utility functions

__author__ = 'pfnf'

from localization.common import *

PRINT_UTILS_COLORS = True

# List of the available colors (probably UNIX only)
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

## Gets the current terminal size
# @return (Number of rows, Number of columns)
def getTerminalSize():
    # Return default
    return (25, 80)

## Print colored text
# @param string Text string to convert
# @param color Color of the text
def cprint(string, color = 'none'):
    print('{0}{1}{2}'.format(COLORS[color.lower()], string, COLORS['default']))

## Pretty print text similar to C printf, with multi-color support
#  @param fmt formation string
#  @param args input arguments
def pprint(fmt, *args):

    # Apply arguments
    if len(args) > 0:
        fmt = fmt.format(*args)

    # Apply coloring
    if PRINT_UTILS_COLORS:
        fmt = fmt.replace('~b', COLORS['bold'])
        fmt = fmt.replace('~n', COLORS['default'])
        fmt = fmt.replace('~r', COLORS['red'])
        fmt = fmt.replace('~g', COLORS['green'])
        fmt = fmt.replace('~y', COLORS['yellow'])
        fmt = fmt.replace('~p', COLORS['purple'])
        fmt = fmt.replace('~m', COLORS['magenta'])
        fmt = fmt.replace('~c', COLORS['cyan'])
    else:
        fmt = fmt.replace('~b', '')
        fmt = fmt.replace('~n', '')
        fmt = fmt.replace('~r', '')
        fmt = fmt.replace('~g', '')
        fmt = fmt.replace('~y', '')
        fmt = fmt.replace('~p', '')
        fmt = fmt.replace('~m', '')
        fmt = fmt.replace('~c', '')

    # Print to stdout
    print(fmt)

## Prints a line across the screen with a caption
#  @param caption caption of the line
#  @param color of the line and caption
def lprint(caption = '', color = 'default'):

    ts = getTerminalSize()

    if PRINT_UTILS_COLORS:
        if caption: print('{0}  -- {1} {2}{3}'.format(COLORS[color], caption, '-' * (ts[1] - len(caption) - 8), COLORS['default']))
        else:      print('{0}  {1}  {2}'.format(COLORS[color], '-' * (ts[1] -4), COLORS['default']))
    else:
        if caption: print('  -- {0} {1}'.format(caption, '-' * (ts[1] - len(caption) - 8)))
        else:      print('  {0}  '.format('-' * (ts[1] -4)))

## Prints a list of list in a table format
#  @param data data to present
def tprint(data):

    if not data: return
    sizes = [max(len(str(c)) + 5 for c in r) for r in zip(*data)]
    fmt = '     ' + ''.join("{" + "{0}:<{1}".format(i, sizes[i]) + "}" for i in range(0, len(sizes)))
    for row in data: pprint(fmt.format(*row))

## Prints a list in a multicolumn format
#  @param data data to present
def ltprint(data):

    if not data: return
    colLen    = len(max(data, key = len)) + 4
    colsCount = (getTerminalSize()[1] - 8) / colLen
    p = 0
    while p < len(data):
        line = '     '
        for i in range(0, colsCount):
            if p >= len(data): break
            line += ('{0:<' + '{0}'.format(colLen) + '}').format(data[p])
            p += 1
        pprint(line)

## Pretty prints the tool welcome messages
# @param welcomeMsgs List of welcoming messages
# @param version Version of the tool
def printWelcomeBox(welcomeMsgs, version):
    width = getTerminalSize()[1]

    pprint('~g+{0}+~n', '-' * (width - 2))
    for msg in welcomeMsgs:
        msg = msg.format(version)
        pprint('~g|{0}{1}|~n', msg, ' ' * (width - 2 - len(msg)))
    pprint('~g+{0}+~n', '-' * (width - 2))
    print('')

## Asks user for prompt actions
# @param prompt Prompt question
# @param actions List of possible actions
# @param default Default action
# @return ID of the chosen action
def promptActions(prompt, actions, default = -1):

    # Auto continue
    #if followDefault and default >= 0: return default

    # Print prompt and actions
    options = []
    cprint(prompt)
    for i, option in enumerate(actions):

        # Get option letter
        if '&' in option:
            options.append(option.split('&')[1][0].upper())
            cprint('  [{0}|{1}]: {2}{3}' \
                .format(i, options[i], option.replace('&', ''), COMMON_DEFAULT_OPTION if default == i else ''))
        else:
            options.append(str(i))
            cprint('  [{0}]: {1}{2}' \
                .format(i, option, COMMON_DEFAULT_OPTION if default == i else ''))

    # Wait for a valid option
    while True:
        ans = raw_input('').lower()

        # Check if the option is valid
        for i, opt in enumerate(options):
            if ans == str(i) or ans == opt.lower(): return i

        # Check for default option
        if not ans and default >= 0:
            return default

        # Invalid option
        cprint(COMMON_INVALID_OPTION.format(ans))
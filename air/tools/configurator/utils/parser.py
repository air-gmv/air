## @package utils.parser
#  @author pfnf
#  @brief Parsing functions

## Convert string to float (not safe, rounded to 3 decimal places)
# @param string Text string to convert
# @return converted float
def str2float(string):
    return int(float(string) * 1000) / 1000.0

## Convert string to int (not safe)
# @param string Text string to convert
# @param base Base of the number
# @return converted integer
def str2int(string, base = 10):
    if 'x' in string: base = 16
    if 'b' in string: base = 2
    return int(string, base)

## Convert string to list of strings (not safe)
# @param string string to convert
# @return converted list
def str2strList(string):
    return [ s.strip() for s in string.split(";") if s.strip() ]

## Convert string to list of integers (not safe)
# @param string string to convert
# @return converted list
def str2intList(string):
    return [ str2int(s.strip()) for s in string.split(";") if s.strip()]

## Convert string to bool
# @param string Text string to convert
# @param default Default value to return
# @return converted boolean (True, False, Default Value)
def str2bool(string, default = None):
    try:
        return True if ['true', 'false'].index(string.lower()) == 0 else False
    except:
        logger.logger.exception("Exception, See details below")
        return default

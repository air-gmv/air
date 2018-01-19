##
# @package utils
# @author pfnf
# @brief Various utilities functions

__author__ = 'pfnf'

from os import path, getcwd
WORKING_DIRECTORY                   = path.abspath(getcwd())

## Flattens a list of list [of list...] into a single list
# @param lst List to flatten
# @return flattened list
def flatten(lst):
    result = []
    for el in lst:
        if hasattr(el, "__iter__") and not isinstance(el, basestring):
            result.extend(flatten(el))
        else:
            result.append(el)
    return result
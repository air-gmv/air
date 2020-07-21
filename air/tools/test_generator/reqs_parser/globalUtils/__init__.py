"""@package globalUtils
Package which holds some utilities for the entire project
"""
################################################################################
#  Project     : AIRII
#  Date        : 2010/09/09
#  Author      : BRSV
#  Version     : $Revision: 1.4 $
#  Description : This module is used to place definitions for python module.
#  History     : 
#  $Log: __init__.py,v $
#  Revision 1.4  2011/07/04 10:09:41  brsv
#  no message
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
#  Revision 1.1  2010/10/12 16:27:29  brsv
#  First issue
# 
################################################################################

# Version of code
__version__ = 1.61

# All modules that are to be imported loaded
__all__ = ['definitions',
           'utils',
           'xmlUtils',
           'logFile'
           ]

#--------------------------[ IMPORTS ]------------------------------------------

import os
import sys
import string
import definitions
import utils
import xmlUtils
import logFile

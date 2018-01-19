#!/usr/bin/python
# =============================================================================
#  Copyright (C) GMVIS Skysoft S.A., 2014
# =============================================================================
#   This file is part of the XKY Operating System configuration sub-system.
#   The license and distribution terms for this file may be found in the file
#   LICENSE in the root directory of the AIR OS distribution.
# =============================================================================

import os
import xky
import xky.configurations as xky_configuration
import utils.terminal as terminal_utils
import tools.configure_xky as configure_xky
import tools.configure_app as configure_app
import logging

from sys import exit
from utils.logger import Logger


from localization.common import *
from argparse import  ArgumentParser,  RawTextHelpFormatter

__version__ 	= '3.6'
__copyright__ 	= 'Copyright (C) GMVIS Skysoft S.A., 2014'
__author__ 		= 'pfnf'
__app__			= os.path.basename(__file__)

#Uncomment this for full debugging of an error and check configurator.log results
# set level for logging.DEBUG for full info
#logging.basicConfig(filename='configurator.log', format='%(levelname)s:%(filename)s:%(funcName)s:%(lineno)d:%(message)s', filemode='w', level=logging.INFO)
logging.basicConfig(format='%(levelname)s:%(filename)s:%(funcName)s:%(lineno)d:%(message)s', filemode='w', level=logging.INFO)
# we can set more attributes to log see https://docs.python.org/2/library/logging.html#logrecord-attributes
logging.info('Configure log activated')

##
# @brief Configure entry point
if __name__ == "__main__":

    # start event logger
    logger = Logger()

    # display
    terminal_utils.printWelcomeBox(AIR_CONFIG_WELCOME, __version__)

    # Handle input arguments
    arg_parser = ArgumentParser(prog=__file__, description=AIR_DESCRIPTION, formatter_class=RawTextHelpFormatter)

    # get available architectures
    xky_configuration.supported_architectures = xky_configuration.get_available_targets()
    xky_configuration.available_libraries = xky_configuration.get_available_libraries()
    xky_configuration.available_pos = xky_configuration.get_available_pos()

    # check if we are configuring XKY or a partition
    if xky.WORKING_DIRECTORY == xky.ROOT_DIRECTORY:

        args = configure_xky.InputArgs(arg_parser, logger)
        configure_xky.Run(args, logger)

    else:
        # load OS configurations
        os_configuration = xky_configuration.load_configuration(logger)
        if os_configuration is None: exit(-1)

        # run the application configuration
        configure_app.InputArgs(arg_parser, logger)
        configure_app.Run(arg_parser.parse_args(), os_configuration, logger)

    exit(0)


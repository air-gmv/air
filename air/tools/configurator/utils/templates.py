## @package commom.makoutils
#  @author pfnf
#  @brief MAKO templating functions

__author__ = 'pfnf'

import os
import utils
#import file as fileutils
import utils.terminal as terminalutils
from mako import exceptions as mako_exceptions
from mako.template import Template as mako_template
from mako.lookup import TemplateLookup as mako_template_lookup

def applyMAKOTemplate(template_file, output_file, template_args, logger, lookup_dirs = None):

    # Log Action
    logger.information(1, '{0:30} => {1:30}',
                       os.path.basename(template_file),
                       os.path.relpath(output_file, utils.WORKING_DIRECTORY))

    # open template file
    try:
        if lookup_dirs is None: lookup_dirs = []
        lookup = mako_template_lookup(directories=lookup_dirs, default_filters=['decode.utf8'],
                                      input_encoding='utf-8', output_encoding='utf-8')
        template = mako_template(filename = template_file, lookup = lookup)
    except Exception as why:
        logger.error(str(why))
        return

    # add input and output file name to the template parameters
    template_args["input_file"] = os.path.basename(template_file)
    template_args["input_dir"] = os.path.dirname(template_file)
    template_args["output_file"] = os.path.basename(output_file)
    template_args["output_dir"] = os.path.dirname(output_file)

    # apply template
    try:
        file_ctx = template.render(**template_args)
    except:
        logger.logger.exception("Exception, See details below")
        logger.error('Error processing \'{0}\' template', template_file)
        if terminalutils.promptActions('View debug trace?', [ '&Yes', '&No'], 0) == 0:
            print(mako_exceptions.text_error_template().render())
        return

    # create output file
    #fileutils.safeMakedirs(os.path.dirname(output_file))
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    try:
        fd = open(output_file, 'w')
        fd.write(file_ctx)
        fd.close()
    except:
        logger.logger.exception("Exception, See details below")
        logger.error('Error creating \'{0}\' from template', output_file)
        return


def cfileHeader(file_name, purpose):

    return "/*\n" \
           " * ============================================================================\n" \
           " * This file was generated by the GMV's Configurator Tool and is part of an\n" \
           " * AIR - ARINC 653 Interface in RTEMS - application configuration.\n" \
           " * File Purpose  : {0}\n" \
           " * ============================================================================\n" \
           " */".format(purpose)


def makefileGetVarName(var_name):

    return var_name.upper().replace("-", "").replace(" ", "")

def getLibName(lib_name):
    lib_name = lib_name.replace("-", "")
    lib_name = lib_name.replace(" ", "")
    return "{0}.a".format(lib_name)


def getMakoTemplates(lookup_dir):

    templates = []
    for root, dirs, files in os.walk(lookup_dir):
        for f in files:
            if f.endswith('.mako'):
                templates.append((os.path.join(root, f), f.replace('.mako', '')))

    return templates

def getMakefileLib(input_file, lib_name=None):

    # extract name from makefile
    if lib_name is None:
        lib_name = input_file.replace("_makefile.mako", "")

    lib_make = "{0}_LIB".format(lib_name.upper())
    return lib_name, lib_make



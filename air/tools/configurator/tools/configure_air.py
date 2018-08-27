##
# @package tools.configure_air
# @author pfnf
# @brief Tool to configuration a AIR OS instalation

__author__ = "pfnf,$(AIR_RUNNER_USER)"

import os
import air
import sys
import utils.templates as makoutils
import utils.terminal as terminalutils
import utils.file as fileutils
import air.configurations as air_configuration


##
# @brief Setup input arguments to build AIR
def InputArgs(arg_parser, logger):

    arg_parser.add_argument('-i', '--info', dest='info', action='store_const', const=True,
                            default=False, help='Installation information')
    arg_parser.add_argument('-t', '--target', dest='target', default=None,
                            help='Deployment target')
    arg_parser.add_argument('-d', '--dev', dest='dev', action='store_const',
                            const=True, default=False, help='Development symbols')
    return arg_parser.parse_args()


##
# @brief Show installation information
def ShowInstallInfo(logger):

    # load OS configurations
    os_configuration = air_configuration.load_configuration(logger)

    if os_configuration is not None:
        logger.event(0, "AIR configured for {0}", os_configuration.get_target())
    else:
        logger.event(0, "AIR isn't currently configured")

    exit(0)


##
# @brief Runs the configuration tool
def Run(args, logger):

    # show info
    if args.info: ShowInstallInfo(logger)

    #check hardcoded files to replace generated
    fileutils.setHardcodedFiles()

    # parse input args or prompt the user for configuration
    if args.target is None:
        arch, bsp, fpu_enabled, cache_init = prompt_configuration(logger)
    else:
        arch, bsp, fpu_enabled, cache_init = input_configuration(args.target, logger)

    # create the OS configuration object
    os_configuration = air_configuration.Configuration(arch, bsp, fpu_enabled, cache_init)

    logger.event(0, 'Configuring AIR OS:')
    logger.information(1, 'Target: {0} - {1}\n'.format(arch.upper(), bsp))

    # template lookup directories
    template_includes = \
        [
            os.path.join(air.TEMPLATES_DIRECTORY, 'include')
        ]

    # create Makefile rules
    makoutils.applyMAKOTemplate(
        os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'inc_makefile.mako'),
        os.path.join(air.ROOT_DIRECTORY, 'Makefile.inc'),
        dict(os_configuration=os_configuration), logger, template_includes)

    # create kernel Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'pmk_makefile.mako'),
        os.path.join(air.SOURCE_PMK_DIRECTORY, 'Makefile'),
        dict(os_configuration=os_configuration), logger, template_includes)

    # create libraries Makefile
    libraries = os_configuration.get_supported_libraries()
    for lib_name in libraries:
        lib = os_configuration.get_library_config(lib_name)
        makoutils.applyMAKOTemplate(
            os_configuration.get_library_makefile(lib.name),
            os.path.join(air.SOURCE_LIBS_DIRECTORY, lib.name.lower(), 'Makefile'),
            dict(os_configuration=os_configuration, lib_name=lib.name, lib_description=lib.description),
            logger, template_includes)

    # create lib directory Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'dir_makefile.mako'),
        os.path.join(air.SOURCE_LIBS_DIRECTORY, 'Makefile'),
        dict(directories=libraries),
        logger, template_includes)

    # create personalities Makefiles
    personalities = os_configuration.get_supported_pos()
    if len(personalities) > 0:

        directories = []

        # create Makefiles for each personality
        for pos_name in personalities:

            pos = os_configuration.get_pos_config(pos_name)

            #Git checkout of pos
            #os_configuration.git_pos_checkout_and_patch(pos_name, logger)

            # check if it is an alias
#            if pos.alias is not None and pos_name in pos.alias:
#                continue

            makoutils.applyMAKOTemplate(
                os_configuration.get_pos_makefile(pos.name),
                os.path.join(air.SOURCE_POS_DIRECTORY, pos.name.lower(), 'Makefile'),
                dict(os_configuration=os_configuration,
                     name=pos.name, description=pos.description,
                     sources=os_configuration.get_pos_sources(pos.name),
                     headers=os_configuration.get_pos_private_headers(pos.name),
                     library_headers=os_configuration.get_pos_public_headers(pos.name)),
                logger, template_includes)

            # add directory
            directories.append(pos.name)

        # directories Makefile
        makoutils.applyMAKOTemplate(
            os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'dir_makefile.mako'),
            os.path.join(air.SOURCE_POS_DIRECTORY, 'Makefile'),
            dict(directories=directories), logger, template_includes)

    # create assembler Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'pas_makefile.mako'),
        os.path.join(air.SOURCE_TOOLS_DIRECTORY, 'partition_assembler', 'Makefile'),
        dict(os_configuration = os_configuration),
        logger, template_includes)

    # create main Makefile
    makoutils.applyMAKOTemplate(
            os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'air_makefile.mako'),
            os.path.join(air.ROOT_DIRECTORY, 'Makefile'),
            dict(os_configuration=os_configuration),
            logger, template_includes)

    # save configuration definitions for future uses
    air_configuration.save_configuration(os_configuration, logger)


##
# @brief Prompt the user to select the architecture/bsp/fpu
# @return arch, bsp, fpup
def prompt_configuration(logger):

    # get supported architectures
    opts = air_configuration.supported_architectures.keys()
    if len(opts) > 0:
        i = terminalutils.promptActions('Select the target architecture:', opts)
        arch = opts[i]
    else:
        arch = opts[0]

    # get the supported BSP
    opts = air_configuration.supported_architectures[arch].keys()
    if len(opts) > 0:
        i = terminalutils.promptActions('Select the target board support package:', opts)
        bsp = opts[i]
    else:
        bsp = opts[0]

    # get the FPU enabled
    opts = ['Enabled', 'Disabled']
    i = terminalutils.promptActions('Select if FPU is:', opts)
    if i == 0:
        fpu_enabled = True
    else:
        fpu_enabled = False

    # get debug monitor tool
    opts = ['GRMON', 'DMON']
    i = terminalutils.promptActions('Select debug monitor:', opts)
    if i == 0:
        cache_init = 1
    else:
        cache_init = 0

    return arch, bsp, fpu_enabled, cache_init


def input_configuration(target, logger):

    try:

        # get the current target
        args = target.split('-', 1)
        arch = args[0]; bsp  = args[1]; fpu_flag = args[2];

        # check if target is supported
        if arch not in air_configuration.supported_architectures.keys() or \
           bsp not in air_configuration.supported_architectures[arch].keys():
            logger.error("Unsupported target: '{0}'", target)
            arch = bsp = None

    except:
        logger.logger.exception("Exception, See details below")

        # error
        logger.error("Unsupported target: '{0}'", target)
        arch = bsp = None

    # present list of supported targets if an error occurred
    if arch is None or bsp is None:

        # check if arch is supported
        logger.event(0, "Supported targets:")
        for arch in air_configuration.supported_architectures.keys():
            for bsp in air_configuration.supported_architectures[arch].keys():
                logger.information(1, '{0}-{1}', arch, bsp)

        sys.exit(-1)

    return arch, bsp, fpu_flag

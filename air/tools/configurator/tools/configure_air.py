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
    arg_parser.add_argument('-f', '--cfg', dest='config', default=None,
                            help='Insert .air_config file')
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
    if args.config is None:
        arch, bsp, fpu_enabled, debug_monitor, pos = prompt_configuration(logger)
        # create the OS configuration object
        os_configuration = air_configuration.Configuration(arch, bsp, fpu_enabled, debug_monitor, pos)
    else:
        os_configuration = input_configuration(logger, args.config)

    logger.event(0, 'Configuring AIR OS:')
    logger.information(1, 'Target: {0} - {1}\n'.format(os_configuration.arch.upper(), os_configuration.bsp))

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
    opts = ['GRMON', 'DMON', 'NONE']
    i = terminalutils.promptActions('Select debug monitor:', opts)
    if i == 0:
        debug_monitor = 1
    elif i == 2:
        debug_monitor = 2;
    else:
        debug_monitor = 0

    pos = []
    # Prompt to install RTOS
    opts = ['No', 'Yes']
    promptx = 'Install All RTOS ?'
    all_rtos = terminalutils.promptActions(promptx, opts)
    pos_names = [x for x in sorted(os.listdir(air.POS_DIRECTORY))
                 if os.path.isdir(os.path.join(air.POS_DIRECTORY, x)) and x != 'shared']
    for pos_name in pos_names:
        try:
            i = 0
            if all_rtos == 0:
                promptx = 'Install '  + pos_name + '?'
                i = terminalutils.promptActions(promptx, opts)
            if i == 1 or all_rtos == 1:
                pos.append(pos_name)
        except IOError:
            logging.warning ('Missing AIR POS : %s, name: %s', pos_path, pos_name)
            pass

    return arch, bsp, fpu_enabled, debug_monitor, pos


def input_configuration(logger, config):

    __CONFIG_FILE__ = os.path.join(air.ROOT_DIRECTORY, config)

    if not os.path.isfile(__CONFIG_FILE__):
        logger.error("Error config file is missing ", __CONFIG_FILE__)
        sys.exit(-1)

    os_configuration = air_configuration.load_configuration(logger, __CONFIG_FILE__)

    return os_configuration

## @package tools.configure_app
#  @author pfnf
#  @brief Tool to configuration a AIR OS instalation

__author__ = 'pfnf,$(AIR_RUNNER_USER)'

import os
import air
import utils
import utils.file as fileutils
import utils.templates as makoutils
import air.configurations as air_configuration


from localization.logger import *
import parsers.a653.definitions as a653_definitions

## @brief Setup input arguments to configure a AIR application
def InputArgs(arg_parser, logger):

    arg_parser.add_argument('-i', '--info', dest='info', action='store_const', const=True, default=False, help='Installation information')
    arg_parser.add_argument('xml_file',
                        metavar='<xml file> | clean',
                        nargs='?',
                        default=None,
                        help='Input XML configuration')
    arg_parser.add_argument('-dm', '--debug_mode', dest='debug_mode', action='store_const', const=True, default=False)

## @brief Show installation information
def ShowInstallInfo(logger):

    # load OS configurations
    os_configuration = air_configuration.load_configuration(logger)

    if os_configuration is not None:
        logger.event(0, "AIR configured for {0}", os_configuration.get_target())
    else:
        logger.event(0, "AIR isn't currently configured")

    exit(0)

## @brief Run the AIR application configurator
def Run(args, os_configuration, logger):

    # show info
    if args.info: ShowInstallInfo(logger)

    # clean previous configuration files
    if args.xml_file == 'clean' or args.xml_file == 'clear': clearConfiguration(logger)

    # look for a single valid XML configuration
    if args.xml_file is None:
        files = fileutils.findFiles(air.WORKING_DIRECTORY, '.xml', a653_definitions.ARINC_653_MODULE)
        if len(files) == 1:
            args.xml_file = files[0]

    # sanity check
    if args.xml_file is None:
        logger.error(' :: XML configuration not found...')
        exit(-1)

    # set debug mode flag
    os_configuration.debug_mode = args.debug_mode

    # parse module
    from parsers.air.parser import airParser

    print("  PARSING configs")
    app_configuration = airParser(os_configuration, logger)
    app_configuration.parse(args.xml_file)
    #print("  done PARSING configs")
    # sanity check
    if logger.total_errors > 0:
        logger.error(' :: XML configuration contains errors...')
        exit(-1)

    # get arch configurations
    app_configuration.arch = os_configuration.archConfiguration(app_configuration)

    # create temporary file to hold the new files
    temp_directory = os.path.join(air.WORKING_DIRECTORY, '.temp')

    # template lookup directories
    template_includes = \
        [
            os.path.join(air.TEMPLATES_DIRECTORY, 'include')
        ]

    # generation partition Makefiles and glue code
    for i, partition in enumerate(app_configuration.partitions):

        logger.event(0, LOG_EVENT_PARTITION_CONFIG, partition)

        # get partition config and templates
        pos_config = os_configuration.get_pos_config(partition.personality)
        input_files, output_files = os_configuration.get_pos_partition_templates(partition.personality)

        for j in range(0, len(input_files)):
            output_file = os.path.join(temp_directory, partition.directory, output_files[j])
            makoutils.applyMAKOTemplate(
                input_files[j], os.path.join(temp_directory, partition.directory, output_file),
                dict(os_configuration=os_configuration, partition=partition, pos_config=pos_config),
                logger, template_includes)

        # makoutils.applyMAKOTemplate(os.path.join(air.APP_TEMPLATES_DIRECTORY, air.POS_DIR_NAME, 'ports_init.mako'),
        #                            os.path.join(temp_directory, partition.directory, 'ports_init.h'),
        #                            dict(os_configuration=os_configuration, partition=partition, header=True),
        #                            logger, template_includes)

        # makoutils.applyMAKOTemplate(os.path.join(air.APP_TEMPLATES_DIRECTORY, air.POS_DIR_NAME, 'ports_init.mako'),
        #                            os.path.join(temp_directory, partition.directory, 'ports_init.c'),
        #                            dict(os_configuration=os_configuration, partition=partition, header=False),
        #                            logger, template_includes)

        # partition linker inc
        makoutils.applyMAKOTemplate(
            os.path.join(air.APP_TEMPLATES_DIRECTORY, 'inc_linkcmds.mako'),
            os.path.join(temp_directory, partition.directory, 'linkcmds.inc'),
            dict(os_configuration=os_configuration, partition=partition, pos_config=pos_config),
            logger, template_includes)

        # if is an IOP
        if partition.iop is not None:
            configure_iop(os_configuration, app_configuration, partition, temp_directory, logger)

    # generate configuration files
    logger.event(0, LOG_EVENT_APP_CONFIG)

    # common source Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'common_makefile.mako'),
        os.path.join(temp_directory, 'common', 'Makefile'),
        dict(configuration=app_configuration),
        logger, template_includes)

    # generate common source configuration files
    templates_names = ['usr_channels', 'usr_config', 'usr_hm', 'usr_partitions', 'usr_schedules', 'usr_sharedmemory']
    for template in templates_names:
        makoutils.applyMAKOTemplate(
            os.path.join(air.APP_TEMPLATES_DIRECTORY, '{0}.mako'.format(template)),
            os.path.join(temp_directory, 'config', '{0}.c'.format(template)),
            dict(configuration=app_configuration, header=False),
            logger, template_includes)

    # generate configuration header file
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'usr_config.mako'),
        os.path.join(temp_directory, 'config', 'usr_config.h'),
        dict(configuration=app_configuration, header=True),
        logger, template_includes)

    # generate ARCH specific configuration
    makoutils.applyMAKOTemplate(
        os_configuration.get_app_usr_arch_template(),
        os.path.join(temp_directory, 'config', 'usr_arch.c'),
        dict(configuration=app_configuration, header=False),
        logger, template_includes)

    # generate linkcmds file
    makoutils.applyMAKOTemplate(
        os_configuration.get_app_linkcmds_template(),
        os.path.join(temp_directory, 'config', 'linkcmds.ld'),
        dict(configuration=app_configuration, os_configuration=os_configuration),
        logger)

    # generate main Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'air_makefile.mako'),
        os.path.join(temp_directory, 'config', 'Makefile'),
        dict(app_configuration=app_configuration, os_configuration=os_configuration),
        logger, template_includes)

    # generate main Makefile
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'dir_makefile.mako'),
        os.path.join(temp_directory, 'Makefile'),
        dict(app_configuration=app_configuration),
        logger, template_includes)
    
    # load previous cleaning record (if it exists)
    record = fileutils.loadFileRecord(os.path.join(air.WORKING_DIRECTORY, '.config'))
    fileutils.safeMultiFileCopyWithRecord(temp_directory, air.WORKING_DIRECTORY, record)

    # create new record
    record = fileutils.createFileRecord(temp_directory)
    fileutils.saveFileRecord(os.path.join(air.WORKING_DIRECTORY, '.config'), record)
    fileutils.safeRemoveDirectory(temp_directory)

def configure_iop(os_configuration, app_configuration, partition, temp_directory, logger):

    iop_configuration = partition.iop

    # template lookup directories
    template_includes = \
        [
            os.path.join(air.TEMPLATES_DIRECTORY, 'include'),
            os.path.join(air.TEMPLATES_DIRECTORY, 'app_configuration', 'iop', 'include')
        ]

    # physical device files
    for i, device in enumerate(iop_configuration.physical_devices):
        makoutils.applyMAKOTemplate(
            os.path.join(air.APP_TEMPLATES_DIRECTORY, 'iop', device.parser[3]),
            os.path.join(temp_directory, partition.directory, 'iop_physical_device_{0}.c'.format(i)),
            dict(app_configuration=app_configuration, os_configuration=os_configuration,
                 iop_configuration=iop_configuration, device=device),
            logger, template_includes)

    # logical device files
    for i, device in enumerate(iop_configuration.logical_devices):
        makoutils.applyMAKOTemplate(
            os.path.join(air.APP_TEMPLATES_DIRECTORY, 'iop', 'iop_logical_device.mako'),
            os.path.join(temp_directory, partition.directory, 'iop_logical_device_{0}.c'.format(i)),
            dict(app_configuration=app_configuration, os_configuration=os_configuration,
                 iop_configuration=iop_configuration, device=device),
            logger, template_includes)

    # ports
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'iop', 'iop_ports.mako'),
        os.path.join(temp_directory, partition.directory, 'iop_ports.c'),
        dict(app_configuration=app_configuration, os_configuration=os_configuration, iop_configuration=iop_configuration),
        logger, template_includes)

    # generic
    makoutils.applyMAKOTemplate(
        os.path.join(air.APP_TEMPLATES_DIRECTORY, 'iop', 'iop_config.mako'),
        os.path.join(temp_directory, partition.directory, 'iop_config.c'),
        dict(app_configuration=app_configuration, os_configuration=os_configuration, iop_configuration=iop_configuration),
        logger, template_includes)


def convertMemoryPermissions(permissions_str):
    permissions = []
    if 'R' in permissions_str: permissions.append('PMK_MMU_R')
    if 'W' in permissions_str: permissions.append('PMK_MMU_W')
    if 'X' in permissions_str: permissions.append('PMK_MMU_E')
    return permissions

## @brief Clear application configuration
#  @param logger Logger object
def clearConfiguration(logger):
    fileutils.runCleaningRecord(os.path.join(air.WORKING_DIRECTORY, '.config'), logger)
    exit(0)

        

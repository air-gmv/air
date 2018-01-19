
AIR_CONFIG_WELCOME 				= [ ' AIR Configurator v{0}', ' Copyright (C) GMVIS Skysoft S.A., 2014' ]
IOP_CONFIG_WELCOME 				= [ ' IOP Configurator v{0}', ' Copyright (C) GMVIS Skysoft S.A., 2013' ]

AIR_DESCRIPTION 				= 'Tool to configure an AIR application'
IOP_DESCRIPTION 				= 'Tool to configure an I/O Partition'

INPUT_ARG_INPUT_HELP			= 'Input XML file or cleaning command'
INPUT_ARG_SUPPORT_HELP			= 'Creates the support functions to configure the partitions (cannot be used with clean argument)'
INPUT_ARG_OUTPUT_HELP			= 'Output directory (cannot be used with clean argument)'
INPUT_ARG_FORCE_HELP			= 'Force the overwrite/deletion of all files (dangerous)'
INPUT_ARG_MODE_HELP				= 'Validation mode (cannot be used with clean argument)\n'\
                                  '     strict : requires that the configuration validates without errors or warnings\n'\
								  '    relaxed : requires that the configuration validates without errors\n'\
								  '  no-schema : same as relaxed but without performing a XML schema validation\n'
INPUT_ARG_VERBOSE_HELP			= 'Verbose mode (1 - Minimal, 2 - Normal, 3 - Full) (cannot be used with clean argument)'
INPUT_ARG_GRAPH_HELP			= 'Generates Module graphs (experimental)'
INPUT_ARG_DEFAULT				= 'Follows the default action instead of prompting the user'

APP_FAILURE						= '  {0}: Correct the problems and try again'
APP_SUCCESS						= '  {0}: Configuration complete'

COMMON_YES 						= 'y'
COMMON_NO  						= 'n'
COMMON_CONFIRM_ERR 				= 'Please enter {0} or {1}.'.format(COMMON_YES, COMMON_NO)
COMMON_INVALID_OPTION			= 'Invalid Option \'{0}\''
COMMON_DEFAULT_OPTION			= ' \033[1m(default)\033[0m'

COMMON_XMLLINT_ERROR0 			= 'Line {0}'
COMMON_XMLLINT_ERROR1 			= 'Validation process returned an unexpected end signal ({0})'
COMMON_XMLLINT_ERROR2 			= 'Unable to process the XML schema validation'

COMMON_PROCESSING_FILE			= 'Processing file: {0}'

CONFIGURATOR_INVALID_INPUT 		= 'Invalid input file \'{0}\''
CONFIGURATOR_INVALID_OUTPUT 	= 'Invalid output directory \'{0}\''
CONFIGURATOR_ABORTED_BY_USER 	= 'Operation aborted by user'
CONFIGURATOR_REPLACE_XML 		= 'A XML file already exists at the target output, do you want to replace it?'

CLEANING_RECORD_RUN				= 'Cleaning AIR configurations'

FILE_COPY_EXISTS				= 'The file \033[1m\'{0}\'\033[0m already exists and they are different:'
FILE_COPY_REPLACE				= '&Replace old file'
FILE_COPY_KEEP					= '&Keep old file'
FILE_COPY_MERGE					= '&Merge both files'
FILE_COPY_DIFF					= 'View files &diff'
FILE_COPY_MERGE_ERROR			= 'Error calling merge tool (return code: {0})'

FILE_COPY_OK					= 'Original \'{0}\' was replaced by the newer version'
FILE_COPY_OLD_KEEP				= 'WARNING: Original \'{0}\' was kept, the configurations may not be valid'
FILE_COPY_MERGED				= 'WARNING: File \'{0}\' merged with the original, the configurations may not be valid'

SANITY_CHECK_TITLE				= 'Checking whether {0} environment is sane'
SANITY_CHECK_WARNINGS			= '{0} warnings occurred, while {1}'
SANITY_CHECK_ERRORS				= '{0} errors occurred, while {1}'

FILE_REMOVE_INFO				= 'Removing file \'{0}\'...'
FILE_REMOVE_EXISTS				= 'The file \'{0}\' signature doesn\'t match with cleaning record signature:'
FILE_REMOVE_DELETE				= '&Delete file'
FILE_REMOVE_KEEP				= '&Keep file'


DIRECTORY_CANNOT_BE_REMOVED		= '{0} cannot be removed'

COMMON_PARSING_XML				= 'parsing the XML configurations'
COMMON_GENERATOR_XML			= 'generating the configuration files'
COMMON_FINALIZING_XML			= 'finalizing the configuration'

AIR_ERROR_NO_FILES 				= 'No AIR configuration file found in the current directory'
AIR_ERROR_MANY_FILES 			= 'Multiple AIR configuration files found in the current directory'

IOP_ERROR_NO_FILES 				= 'No IOP configuration file found in the current directory'
IOP_ERROR_MANY_FILES 			= 'Multiple IOP configuration files found in the current directory'
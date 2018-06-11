##
# @package air.config
# @author pfnf
# @brief Provides the AIR OS configurations

__author__ = 'pfnf'

import os
import re
import imp
import air
import utils
import pickle
import utils.parser
import logging
import traceback
import utils.terminal as terminalutils
import subprocess

__OS_CONFIG_FILE__ = os.path.join(air.ROOT_DIRECTORY, '.air_config')

AIR_POS = os.path.join(air.CONFIGURATOR_DIRECTORY, 'air', 'pos')
AIR_TARGETS = os.path.join(air.CONFIGURATOR_DIRECTORY, 'air', 'targets')
AIR_LIBRARIES = os.path.join(air.CONFIGURATOR_DIRECTORY, 'air', 'libraries')


##
# @brief Get the list of available targets
# @return List of available targets
def get_available_targets():

    targets = {}
    architectures = [x for x in os.listdir(AIR_TARGETS)
                     if os.path.isdir(os.path.join(AIR_TARGETS, x)) and x != 'shared']

    for arch_name in architectures:

        bsp = {}
        arch_path = os.path.join(AIR_TARGETS, arch_name)
        board_support_packages = [x for x in os.listdir(arch_path)
                                  if os.path.isdir(os.path.join(arch_path, x)) and x != 'shared']
        for bsp_name in board_support_packages:

            target_str = '{0}-{1}'.format(arch_name.lower(), bsp_name.lower())

            try:
                bsp_path = os.path.join(arch_path, bsp_name, 'config.py')
                module = imp.load_source(target_str, bsp_path)
                bsp[bsp_name] = module
            except IOError:
                logging.warning ('Missing AIR target : %s, bsp: %s', bsp_path, bsp_name)        
                pass

        if len(bsp) > 0:
            targets[arch_name.lower()] = bsp

    return targets


def get_available_libraries():

    class LIBAIRConfiguration(object):
        def __init__(self):
            self.name = 'libair'
            self.description = 'Para-virtualization layer with AIR OS'
            self.dependencies = []
            self.makefile = None
            self.requires_pos = None

    libs = {
        'libair': LIBAIRConfiguration(),
    }

    libs_names = [x for x in os.listdir(AIR_LIBRARIES)
                  if os.path.isdir(os.path.join(AIR_LIBRARIES, x)) and x != 'shared']

    for lib_name in libs_names:
        try:
            lib_path = os.path.join(AIR_LIBRARIES, lib_name, 'config.py')
            module = imp.load_source(lib_name, lib_path)
            module.path = lib_path
            libs[module.name.lower()] = module

        except IOError:
            logging.warning ('Missing AIR library : %s, name: %s', lib_path, lib_name)        
            pass

    return libs


def get_available_pos():
    pos = {}
    # Prompt to install RTOS
    #opts = ['No', 'Yes']
    #promptx = 'Install All RTOS ?'
    #all_rtos = terminalutils.promptActions(promptx, opts)
    pos_names = [x for x in os.listdir(AIR_POS)
                 if os.path.isdir(os.path.join(AIR_POS, x)) and x != 'shared']
    for pos_name in pos_names:
        try:
            #i = 0
            #if all_rtos == 0:
            #    promptx = 'Install '  + pos_name + '?'
            #    i = terminalutils.promptActions(promptx, opts)
            #if i == 1 or all_rtos == 1:
            pos_path = os.path.join(AIR_POS, pos_name, 'config.py')
            module = imp.load_source(pos_name, pos_path)
            module.path = pos_path
            pos[module.name.lower()] = module
        except IOError:
            logging.warning ('Missing AIR POS : %s, name: %s', pos_path, pos_name)        
            pass
    return pos


supported_architectures = {}
available_libraries = {}
available_pos = {}


##
# @brief Class to hold the AIR OS current configuration
class Configuration(object):

    ##
    # @brief Constructor
    # @param self object pointer
    # @param arch target architecture
    # @param bsp target board support package
    def __init__(self, arch, bsp, fpu_enabled):

        self.arch = arch.lower()
        self.bsp = bsp.lower()
        self.fpu_enabled = fpu_enabled;
        self.debug_mode = False
        #logging.info ('Initializing Configuration class architecture: %s, bsp: %s', self.arch, self.bsp)

        # get supported pos
        self.supported_pos = {}
        for pos in available_pos:
            pos = available_pos[pos]
            if not hasattr(pos, 'supported_libraries'):
                pos.supported_libraries = []
            if self.arch in pos.source_files and \
               self.arch in pos.private_header_files and self.arch in pos.public_header_files:
                self.supported_pos[pos.name.lower()] = pos
                if pos.alias is not None:
                    for alias in pos.alias:
                        self.supported_pos[alias.lower()] = pos

        # get supported libraries
        self.supported_libraries = {}
        for lib in available_libraries:
            supported = False
            lib = available_libraries[lib]
            if lib.requires_pos is not None:
                for pos_name in lib.requires_pos:
                    try:
                        pos = self.supported_pos[pos_name]
                        pos.supported_libraries.append(lib.name)
                        supported = True
                    except Exception:
                        logging.warning ('the library: %s is not installed because %s is missing', lib.name, pos_name)  
                        pass
            else:
                for pos_name in self.supported_pos:
                    pos = self.supported_pos[pos_name]
                    self.supported_pos[pos_name].supported_libraries.append(lib.name)
                    supported = True

            if supported:
                self.supported_libraries[lib.name.lower()] = lib

        # clear duplicates in partition library list
        for pos_name in self.supported_pos:
            pos = self.supported_pos[pos_name]
            if pos.alias is None or pos_name not in pos.alias:
                pos.supported_libraries = list(set(pos.supported_libraries))

    ##
    # @brief Check if a POS is supported
    # @param pos_name POS name
    # @return True if supported False otherwise
    def is_pos_supported(self, pos_name):
        return pos_name in self.supported_pos

    ##
    # @brief Check if a IOP is supported
    # @return True if supported False otherwise
    def is_iop_supported(self):
        return 'libiop' in self.supported_libraries

    ##
    # @brief Check if a Library is support
    # @param lib_name Library name
    # @return True if supported False otherwise
    def is_library_supported(self, lib_name):
        return lib_name in self.supported_libraries

    ##
    # @brief Gets the configuration current target
    # @param self object pointer
    # @return current target string
    def get_target(self):
        return '{0}-{1}'.format(self.arch, self.bsp)


    ##
    # @brief Implementation of grep file
    # @param filename file to grep
    # @param needle text to search
    # @return current target string
    def grep(self, filename, needle):
        try:
            with open(filename) as f_in:
                for line in f_in:
                    if not needle in line:
                        continue
                    try:
                        return True
                    except IndexError:
                        return False
        except IOError:
            return False

    ##
    # @brief Gets the target compiler
    def get_target_compiler(self):
        msoft_float = self.grep ("Makefile.inc", "msoft")
        if self.fpu_enabled:
            if msoft_float:
                os.system("patch --force -p2 -R -s < tools/configurator/disable_fpu.patch")
            return supported_architectures[self.arch][self.bsp].kernel_compiler
        else:
            if not msoft_float:
               os.system("patch --force -p2 -s < tools/configurator/disable_fpu.patch")
            return supported_architectures[self.arch][self.bsp].kernel_compiler_no_fpu
			
    ##
    # @brief Gets the kernel ASM generator files
    def get_kernel_asm_generator(self):
        return supported_architectures[self.arch][self.bsp].kernel_asm

    ##
    # @brief Get available permissions
    # @return Partition available permissions
    def get_available_permissions(self):
        return supported_architectures[self.arch][self.bsp].permissions

    ##
    # @brief Gets the configuration available cores
    # @param self object pointer
    # @return number of available cores
    def get_available_cores(self):
        return supported_architectures[self.arch][self.bsp].cores

    ##
    # @brief Get Kernel Source file list
    # @return Kernel source files list
    def get_kernel_sources(self):
        return supported_architectures[self.arch][self.bsp].kernel_sources

    ##
    # @brief Get Kernel Header file list
    # @return Kernel header files list
    def get_kernel_headers(self):
        return supported_architectures[self.arch][self.bsp].kernel_headers

    ##
    # @brief Get AIR library Source file list
    # @return Kernel source files list
    def get_libair_sources(self):
        return supported_architectures[self.arch][self.bsp].libair_sources

    ##
    # @brief Get AIR library header files list
    # @return Kernel header files list
    def get_libair_headers(self):
        return supported_architectures[self.arch][self.bsp].libair_headers

    ##
    # @brief Get Library makefile template
    # @param lib_name Name of the library
    # @return Path to the makefile template
    def get_library_makefile(self, lib_name):
        if lib_name == 'libair' or self.supported_libraries[lib_name].makefile is None:
            return os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'lib_makefile.mako')
        return self.supported_libraries[lib_name].makefile

    ##
    # @brief Get Library source files list
    # @param lib_name Name of the library
    def get_library_sources(self, lib_name):
        if lib_name == 'libair':
            return supported_architectures[self.arch][self.bsp].libair_sources
        return self.supported_libraries[lib_name].source_files

    ##
    # @brief Get Library public headers
    # @param lib_name Name of the library
    def get_library_public_headers(self, lib_name):
        if lib_name == 'libair':
            return self.get_libair_headers()
        return set(utils.flatten(self.supported_libraries[lib_name].header_files))

    ##
    # @brief Get Library private headers
    # @param lib_name Name of the library
    def get_library_private_headers(self, lib_name):
        if lib_name == 'libair':
            return self.get_kernel_headers()
        lib_air = self.get_libair_headers()
        dependencies = self.get_library_list_of_dependencies(lib_name)
        return set(utils.flatten([[self.supported_libraries[n].header_files for n in dependencies], lib_air]))

    ##
    # @brief Gets the list of dependencies for a library
    # @param lib_name library name
    def get_library_list_of_dependencies(self, lib_name):
        dependencies = self.supported_libraries[lib_name].dependencies
        if dependencies is None or len(dependencies) == 0:
            return [lib_name]
        return utils.flatten([lib_name, [self.get_library_list_of_dependencies(d) for d in dependencies]])

    ##
    # @brief Get POS makefile template
    # @param pos_name Name of the library
    # @return Path to the makefile template
    def get_pos_makefile(self, pos_name):
        if self.supported_pos[pos_name].makefile is None:
            return os.path.join(air.AIR_TEMPLATES_DIRECTORY, 'pos_makefile.mako')
        return self.supported_pos[pos_name].makefile

    ##
    # @brief Get POS source files list
    # @param pos_name Name of the library
    def get_pos_sources(self, pos_name):
        return self.supported_pos[pos_name].source_files[self.arch]

    ##
    # @brief Get POS public headers
    # @param lib_name Name of the library
    def get_pos_public_headers(self, pos_name):
        return self.supported_pos[pos_name].public_header_files[self.arch]

    ##
    # @brief Get POS private headers
    # @param lib_name Name of the library
    def get_pos_private_headers(self, pos_name):
        return set(utils.flatten([
            self.supported_pos[pos_name].private_header_files[self.arch],
            self.get_kernel_headers()]))

    ##
    # @brief Get target defines
    # @return Target configuration defines
    def get_target_defines(self):
        return supported_architectures[self.arch][self.bsp].defines

    ##
    # @brief Gets a list of supported POS
    def get_supported_pos(self):
        return self.supported_pos.keys()

    ##
    # @brief Gets the configuration of a POS
    # @param pos_name POS Name
    # @return POS configuration
    def get_pos_config(self, pos_name):
        return self.supported_pos[pos_name.lower()]

    ##
    # @brief Get POS partition templates
    # @param pos_name POS Name
    # @return Partition templates
    def get_pos_partition_templates(self, pos_name):

        input_file = []
        output_file = []
        for i, template in enumerate(self.supported_pos[pos_name.lower()].partition_templates):
            input_file.append(template)
            output_file.append('Makefile' if i == 0 else os.path.basename(template).replace('.mako', ''))

        return input_file, output_file

     ##
    # @brief Execute POS git checkout and then patchit
     # @param pos_name Name of the pos
     # @return Path to the makefile template

    #    def pos_checkout_and_patch(self, pos_name, logger):
    #        if self.supported_pos[pos_name].git_id is not "":            
    #            logger.information(1, 'Checking out ' + pos_name + ' from GIT')
    #            FNULL = open(os.devnull, 'w')
    # git stash current files to merge ahead and patch on pos checkout
    #            subprocess.call(['git','stash'], cwd="pos", stdout=FNULL, stderr=subprocess.STDOUT)
    # git checkout call
    #            subprocess.call(['git','checkout', self.supported_pos[pos_name].git_id], cwd="pos/" + pos_name+ "/" + pos_name, stdout=FNULL, stderr=subprocess.STDOUT)
            # patch pos by doing git stash pop
    #            subprocess.call(['git','stash', 'pop'], cwd="pos", stdout=FNULL, stderr=subprocess.STDOUT)



    ##
    # @brief Gets the Partition complete library list
    # @param libraries List of libraries defined by user
    # @param Complete list of libraries (user + dependencies)
    def get_partition_complete_library_list(self, libraries):
        return set(utils.flatten(
            ['libair', [self.get_library_list_of_dependencies(l.lower()) for l in libraries]]
        ))

    ##
    # @brief Gets a list of supported libraries
    def get_supported_libraries(self, pos_name=None):
        return self.supported_libraries.keys() \
            if pos_name is None else self.supported_pos[pos_name.lower()].supported_libraries

    ##
    # @brief Gets the configuration of a library
    # @param lib_name Library Name
    # @return Library configuration
    def get_library_config(self, lib_name):
        return self.supported_libraries[lib_name]

    ##
    # @brief Gets the memory requirements for the current configuration
    # @param self object pointer
    def get_memory_map(self):
        return supported_architectures[self.arch][self.bsp].mmap

    ##
    # @brief Gets the IOP definitions for the current configuration
    # @param self object pointer
    def get_iop_configuration(self):
        return supported_architectures[self.arch][self.bsp].iop

    ##
    # @brief Gets the validation function for the ticks per second configuration
    # @param self object pointer
    def validate_ticks_per_second(self):
        return [utils.parser.str2int, lambda x: 0 < x <= 50000]

    ##
    # @brief Gets the validation function for the core count
    # @param self object pointer
    # @param configured_count currently configured cores
    def validate_core_count(self, configured_count=-1):
        if configured_count == -1:
            configured_count = supported_architectures[self.arch][self.bsp].cores
        return [utils.parser.str2int, lambda x: 0 < x <= configured_count]

    ##
    # @brief Gets the validation function for the core listing
    # @param self object pointer
    # @param configured_count currently configured cores
    def validate_core_list(self, configured_count=1):
        return [utils.parser.str2intList, lambda x: len(x) == len(set(x)) and all(0 <= c < configured_count for c in x)]

    ##
    # @brief Gets the validation function for the system state
    # @param self object pointer
    def validade_state_id(self):
        return [utils.parser.str2int, lambda x: air.MIN_STATE_ID <= x < air.MAX_STATE_ID]

    ##
    # @brief Gets the validation function for the error level
    # @param self object pointer
    def validate_error_id(self):
        return [utils.parser.str2int, lambda x: air.MIN_ERROR_ID <= x < air.MAX_ERROR_ID]

    ##
    # @brief Gets the validation function for the available personalities
    # @param self object pointer
    def validate_personalities(self):
        return [str, lambda x: x.lower() in self.get_supported_pos()]

    ##
    # @brief Gets the validation function for the available libraries
    # @param pos_name POS of the libraries
    def validate_libraries(self, pos_name):
        return [utils.parser.str2strList,
                lambda x: set(map(
                    lambda y: y.lower(), x)) <= set(map(lambda y: y.lower(), self.get_supported_libraries(pos_name)))]

    ##
    # @brief Gets the validation function for memory permissions
    # @param self object pointer
    def validate_memory_permissions(self):
        return [str, lambda x: x.upper() in 'RWX']

    ##
    # @brief Gets the validation function for partition permissions
    # @param self object pointer
    def validate_permissions(self):
        return [utils.parser.str2strList, lambda x: all(p in supported_architectures[self.arch][self.bsp].permissions for p in x)]

    ##
    # @brief Gets the validation function for partition size
    # @param self object pointer
    def validate_size(self):
        return [utils.parser.str2int, lambda x: 0 < x]

    ##
    # @brief Gets the configuration default memory unit
    # @param self object pointer
    def get_default_memory_unit(self):
        return supported_architectures[self.arch][self.bsp].mmap.default_unit

    ##
    # @brief Gets the validation function for memory unit
    # @param self object pointer
    def validate_memory_unit(self):
        return [utils.parser.str2int, lambda x: x in supported_architectures[self.arch][self.bsp].mmap.units]

    ##
    # @brief Gets usr_arch template file
    # @return usr_arch template file
    def get_app_usr_arch_template(self):
        return supported_architectures[self.arch][self.bsp].airapp_usr_arch

    ##
    # @brief Gets linkcmds template file
    # @return linkcmds template file
    def get_app_linkcmds_template(self):
        return supported_architectures[self.arch][self.bsp].airapp_linkcmds

    ##
    # @brief Apply the IOP arch dependent configurations
    # @param self object pointer
    # @param config module configuration
    # @param partition partition configuration
    def applyIOPArchConfiguration(self, config, partition):
        if supported_architectures[self.arch][self.bsp].iop.arch is not None:
            supported_architectures[self.arch][self.bsp].iop.arch(config, partition)

    ##
    # @brief Get library dependencies include files
    # @param self object pointer
    # @param lib library to check
    # @param current Flag if we are top level
    def getLibraryDependenciesIncludes(self, lib, current=True):

        full_list = []
        for d in lib.depends:
            full_list.append(self.getLibraryDependenciesIncludes(d, False))
        if not current: full_list.append(lib.name)
        return set(utils.flatten(full_list))

    def archConfiguration(self, app_configuration):

        if supported_architectures[self.arch][self.bsp].arch_configure is not None:
            return supported_architectures[self.arch][self.bsp].arch_configure(app_configuration)

        return None


##
# @brief Saves the OS configuration in the build directory
# @param os_configuration OS configuration object pointer
# @param logger logger to report errors
def save_configuration(os_configuration, logger):

    try:
        fd = open(__OS_CONFIG_FILE__, 'w+')
        pickle.dump((os_configuration.arch, os_configuration.bsp, os_configuration.fpu_enabled), fd)
        fd.close()

    except Exception, why:
        logger.error("Error saving the OS configuration - {0}".format(str(why)))

##
# @brief Loads the OS configuration from the build directory
# @param logger logger to report errors
# @return OS configuration object pointer
def load_configuration(logger):
    # sanity check
    if not os.path.isfile(__OS_CONFIG_FILE__):
        logger.error("Error POS config file is missing ", __OS_CONFIG_FILE__)
        return None

    # try to load the configuration
    try:
        fd = open(__OS_CONFIG_FILE__, 'r')
        arch, bsp, fpu_enabled = pickle.load(fd)
        os_configuration = Configuration(arch, bsp, fpu_enabled) 
        fd.close()
        return os_configuration
    except: 
        logger.logger.exception("Exception, See details below")
        os_configuration = None

    return os_configuration

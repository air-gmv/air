## @package parsers.a653.parser
#  @author pfnf
#  @brief XKY XML parser base on a653 XML parser

__author__ = 'pfnf'

import sys
import xky
import utils
import utils.parser as parserutils

from definitions import *
from localization.logger import *
from parsers.a653.definitions import *
from parsers.iop.parser import IOParser
from parsers.a653.parser import a653parser


##
#
class xkyParser(a653parser):

    ##
    # @brief xky parser constructor
    # @param self object pointer
    # @param os_configuration current OS configuration
    # @param logger logger
    def __init__(self, os_configuration, logger):

        # initialize base class
        super(xkyParser, self).__init__(logger)

        # xky configuration
        self.os_configuration = os_configuration

        # xky extensions
        self.core_count = 1
        self.ticks_per_second = 100
        self.shared_memory = []

    ##
    # @brief Parse module extensions
    def parse_module_extensions(self, xml_node):

        self.parse_old_module_extension(xml_node)
        self.parse_module_extension(xml_node)

        self.logger.information(1, "XKY (Module Configuration) - Ticks per Second: {0}; Core Count: {1}",
                                self.ticks_per_second, self.core_count)
        return True

    ##
    # @brief Parse old module extensions
    def parse_old_module_extension(self, xml_node):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse extension tag
        xml_node = xml_node.parse_tag(MODULE_EXTENSION_OLD, 0, 1, self.logger)
        if self.logger.check_errors() or len(xml_node) == 0: return False
        xml_node = xml_node[0]

        self.logger.warning("Deprecated AIR extension '{0}' found", MODULE_EXTENSION_OLD)

        # parse attributes
        core_count = xml_node.parse_attr(MODULE_EXTENSION_CORES_OLD, self.os_configuration.validate_core_count(), False,
                                         self.logger, 1)
        ticks_per_second = xml_node.parse_attr(MODULE_EXTENSION_TICKS_OLD, self.os_configuration.validate_ticks_per_second(),
                                               True, self.logger)
        if self.logger.check_errors(): return False

        # apply attributes
        self.ticks_per_second = ticks_per_second
        self.core_count = core_count

    ##
    # @brief Parse new module extensions
    def parse_module_extension(self, xml_node):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse extension tag
        xml_node = xml_node.parse_tag(MODULE_EXTENSION, 0, 1, self.logger)
        if self.logger.check_errors() or len(xml_node) == 0: return False
        xml_node = xml_node[0]

        # parse required cores
        core_count = xml_node.parse_attr(MODULE_EXTENSION_CORES, self.os_configuration.validate_core_count(), False,
                                         self.logger, 1)

        # parse ticks per second
        ticks_per_second = xml_node.parse_attr(MODULE_EXTENSION_TICKS,
                                               self.os_configuration.validate_ticks_per_second(),
                                               True, self.logger)

        # parse supported targets
        targets = xml_node.parse_attr(MODULE_EXTENSION_TARGET,
                                      [utils.parser.str2strList, lambda arg: True],
                                      False, self.logger, [])

        # check if current target is supported
        target = self.os_configuration.get_target()
        targets = [t.lower() for t in targets]
        if len(targets) > 0 and target not in targets:
            self.logger.error(LOG_TARGET_NOT_SUPPORTED, target)

        if self.logger.check_errors(): return False

        # apply attributes
        self.ticks_per_second = ticks_per_second
        self.core_count = core_count

    ##
    # @brief Parse system HM table
    def parse_system_hm(self, xml_node):

        # return code
        rc = True

        # get system state XML nodes
        if len(xml_node) > 0:
            xml_node = xml_node[0]
            xml_system_state_nodes = xml_node.parse_tag(HM_STATE_ENTRY, 0, sys.maxint, self.logger)
            for xml_system_state_node in xml_system_state_nodes:

                # clear previous errors
                self.logger.clear_errors(0)

                # get system state
                system_state = SystemState()
                system_state.id = xml_system_state_node.parse_attr(HM_STATE_ENTRY_ID,
                                                                   self.os_configuration.validade_state_id(), True,
                                                                   self.logger)
                system_state.description = xml_system_state_node.parse_attr(HM_STATE_ENTRY_DESCRIPTION, VALID_NAME_TYPE,
                                                                            False, self.logger, '')
                system_state.xml_line = xml_system_state_node.sourceline

                # sanity check
                if self.logger.check_errors():
                    rc = False
                    continue

                # check for system state redefinition
                if any(system_state == other for other in self.system_hm_table):
                    self.logger.error(LOG_REDEFINITION, xml_system_state_node.sourceline, system_state)
                    continue

                # apply default description
                if system_state.description == '':
                    system_state.description = HMSystemState[system_state.id]['description']

                self.system_hm_table.append(system_state)
                self.logger.information(1, system_state.details())

                # get error level XML nodes
                xml_error_level_nodes = xml_system_state_node.parse_tag(HM_ERROR_LEVEL, 0, sys.maxint, self.logger)
                for xml_error_level_node in xml_error_level_nodes:

                    # clear previous errors
                    self.logger.clear_errors(1)

                    # parse error level attributes
                    error_level = ErrorLevel()
                    error_level.id = xml_error_level_node.parse_attr(HM_ERROR_LEVEL_ID,
                                                                     self.os_configuration.validate_error_id(), True,
                                                                     self.logger)
                    error_level.level = xml_error_level_node.parse_attr(HM_ERROR_LEVEL_TYPE, VALID_ERROR_LEVEL_TYPE, True,
                                                                        self.logger)
                    error_level.description = xml_error_level_node.parse_attr(HM_ERROR_LEVEL_DESCRIPTION, VALID_NAME_TYPE,
                                                                              False, self.logger, '')
                    error_level.xml_line = xml_error_level_node.sourceline
                    if error_level.level == 'PROCESS':
                        error_level.code = xml_error_level_node.parse_attr(HM_ERROR_LEVEL_CODE, VALID_ERROR_CODE_TYPE, True,
                                                                           self.logger)

                    # check if error occurred
                    if self.logger.check_errors():
                        rc = False
                        continue

                    # check for error level redefinition
                    if any(error_level == other for other in system_state.errors):
                        self.logger.error(LOG_REDEFINITION, xml_error_level_node.sourceline, system_state)
                        rc = False
                        continue

                    # apply default code and description
                    error_level.code = HMErrorsDefinitions[error_level.id]['code']
                    if error_level.description == '':
                        error_level.description = HMErrorsDefinitions[error_level.id]['description']

                    # store error level information
                    self.logger.information(2, error_level.details())
                    system_state.errors.append(error_level)

        # apply default system states
        for i in range(xky.MIN_STATE_ID, xky.MAX_STATE_ID):

            # check if state was defined
            system_state = [s for s in self.system_hm_table if s.id == i]
            if len(system_state) == 0:
                system_state = SystemState()
                system_state.id = i
                system_state.description = HMSystemState[system_state.id]['description']
                self.system_hm_table.append(system_state)
            else:
                system_state = system_state[0]

            # apply default error levels
            for j in range(xky.MIN_ERROR_ID, xky.MAX_ERROR_ID):
                if len([e for e in system_state.errors if e.id == j]) == 0:
                    error_level = ErrorLevel()
                    error_level.id = j
                    error_level.code = HMErrorsDefinitions[error_level.id]['code']
                    error_level.description = HMErrorsDefinitions[error_level.id]['description']
                    error_level.level = MODULE_LEVEL if i < 2 or i > 5 else PARTITION_LEVEL
                    system_state.errors.append(error_level)

        return rc

    ##
    # @brief Parse health-monitor table
    # @param self object pointer
    # @param xml_node health-monitor table XML node
    # @param target MODULE_LEVEL or PARTITION_LEVEL
    # @return True if no errors occurred, False otherwise
    def parse_hm_table(self, xml_node, target):

        # return code
        rc = True

        # check if we parsing an HM from a partition
        if target == PARTITION_LEVEL:

            # parse partition attributes
            pid = xml_node.parse_attr(PARTITION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            name = xml_node.parse_attr(PARTITION_NAME, VALID_NAME_TYPE, False, self.logger)
            callback = xml_node.parse_attr(PARTITION_CALLBACK, VALID_NAME_TYPE, False, self.logger, '')

            # sanity check
            if self.logger.check_errors(): return False

            # get partition
            partition = self.get_partition(pid, name)
            if not partition:
                self.logger.error(LOG_UNDEFINED, xml_node.sourceline, PARTITION_STR.format(pid, name))
                return False

            # setup list of valid actions
            valid_actions = VALID_PARTITION_ACTION_TYPE

            # setup HM table
            partition.hm_callback = callback
            hm_table = partition.hm_table

            # mark event
            self.logger.event(0, '{0} Health-Monitor Table', partition)

        else:

            # setup list of valid actions
            valid_actions = VALID_MODULE_ACTION_TYPE

            # setup HM table
            hm_table = self.module_hm_table

            # use only the first node
            if len(xml_node) > 0:
                xml_node = xml_node[0]

        # get system state XML nodes
        if len(xml_node) > 0:
            xml_system_state_nodes = xml_node.parse_tag(HM_STATE_ENTRY, 0, sys.maxint, self.logger)
            for xml_system_state_node in xml_system_state_nodes:

                # clear previous errors
                self.logger.clear_errors(0)

                # get system state
                target_system_state = SystemState()
                target_system_state.id = xml_system_state_node.parse_attr(HM_STATE_ENTRY_ID, VALID_IDENTIFIER_TYPE,
                                                                          True, self.logger)
                target_system_state.description = xml_system_state_node.parse_attr(HM_STATE_ENTRY_DESCRIPTION,
                                                                                   VALID_NAME_TYPE, False, self.logger,
                                                                                   '')
                target_system_state.xml_line = xml_system_state_node.sourceline

                # check if error occurred
                if self.logger.check_errors():
                    rc = False
                    continue

                # check if the system state is defined in system HM table
                system_state = [state for state in self.system_hm_table if state.id == target_system_state.id]
                if not system_state:
                    self.logger.error(LOG_UNDEFINED_SYSTEM_STATE_0, xml_system_state_node.sourceline,
                                      target_system_state)
                    rc = False
                    continue

                # check if the system state is defined for module HM table
                system_state = system_state[0]
                if not any(error_level.level == target for error_level in system_state.errors):
                    self.logger.error(LOG_UNDEFINED_SYSTEM_STATE_1, xml_system_state_node.sourceline, system_state,
                                      target)
                    rc = False
                    continue

                # apply default description
                if target_system_state.description == '':
                    target_system_state.description = HMSystemState[target_system_state.id]['description']

                hm_table.append(target_system_state)
                self.logger.information(1, target_system_state.details())

                # get error action XML nodes
                xml_error_action_nodes = xml_system_state_node.parse_tag(HM_ERROR_ACTION, 0, sys.maxint, self.logger)
                for xml_error_action_node in xml_error_action_nodes:

                    # clear previous errors
                    self.logger.clear_errors(1)

                    # parse error level attributes
                    error_action = ErrorAction()
                    error_action.id = xml_error_action_node.parse_attr(HM_ERROR_ACTION_ID, VALID_IDENTIFIER_TYPE, True,
                                                                       self.logger)
                    error_action.action = xml_error_action_node.parse_attr(HM_ERROR_ACTION_TYPE, valid_actions, True,
                                                                           self.logger)
                    error_action.description = xml_error_action_node.parse_attr(HM_ERROR_ACTION_DESCRIPTION,
                                                                                VALID_NAME_TYPE, False, self.logger, '')
                    error_action.xml_line = xml_error_action_node.sourceline

                    # check if error occurred
                    if self.logger.check_errors():
                        rc = False
                        continue

                    # check if the error level is defined for this target
                    if not any(error_level.id == error_action.id for error_level in system_state.errors):
                        self.logger.error(LOG_UNDEFINED_ERROR_LEVEL, xml_error_action_node.sourceline, error_action.id,
                                          target)
                        rc = False
                        continue

                    # check for error action redefinition
                    if any(error_action == other for other in target_system_state.errors):
                        self.logger.error(LOG_REDEFINITION, xml_error_action_node.sourceline, system_state)
                        rc = False
                        continue

                    # append default action
                    error_action.code = HMErrorsDefinitions[error_action.id]['code']
                    if error_action.description == '':
                        error_action.description = HMErrorsDefinitions[error_action.id]['description']

                    # store error level information
                    self.logger.information(2, error_action.details())
                    target_system_state.errors.append(error_action)

        return rc


    def complete_hm_tables(self):

        # complete Module HM tables
        self.__complete_table(self.module_hm_table, MODULE_LEVEL)

        # complete Partition HM tables
        for partition in self.partitions:
            self.__complete_table(partition.hm_table, PARTITION_LEVEL)

    def __complete_table(self, hm_table, target):

        # apply default HM table
        for i in range(xky.MIN_STATE_ID, xky.MAX_STATE_ID):

            # check if the system state table allows the current combination
            system_state = [state for state in self.system_hm_table if state.id == i][0]
            if any(error_level.level == target for error_level in system_state.errors):

                # check if state was defined
                target_system_state = [state for state in hm_table if state.id == i]
                if len(target_system_state) == 0:
                    target_system_state = SystemState()
                    target_system_state.id = system_state.id
                    target_system_state.description = system_state.description
                    hm_table.append(target_system_state)
                else:
                    target_system_state = target_system_state[0]

                # apply default error actions
                for j in range(xky.MIN_ERROR_ID, xky.MAX_ERROR_ID):
                    if any(error_level.id == j for error_level in system_state.errors) and \
                            not any(error_level.id == j for error_level in target_system_state.errors):
                        error_action = ErrorAction()
                        error_action.id = j
                        error_action.action = 'IGNORE' if i < 5 else ('IDLE' if i == 5 else 'SHUTDOWN')
                        error_action.description = HMErrorsDefinitions[j]['description']
                        target_system_state.errors.append(error_action)

    ##
    # @brief Parse partition extensions
    def parse_partition_extensions(self, xml, partition):

        rc = True

        # clear previous errors
        self.logger.clear_errors(1)

        # get memory requirements
        mmap = self.os_configuration.get_memory_map()

        # split virtual addresses in debug mode
        memory_vaddr = mmap.partition_space[0]
        if self.os_configuration.debug_mode:
            memory_vaddr += len(self.partitions) * 0x1000000

        # append default extension
        partition.core_count = 1
        partition.permissions = [ xky.PERMISSION_GLOBAL_TIME ]
        partition.personality = ''
        partition.cache_state = [False, False]
        partition.memory_size = mmap.units[0] * 1024                    # 1 Mb
        partition.memory_unit = mmap.units[0]                           # 256 kb page
        partition.memory_vaddr = memory_vaddr
        partition.libraries = []
        partition.devices = []

        # parse partition configuration
        xml_config_node = xml.parse_tag(PARTITION_EXTENSION, 1, 1, self.logger)
        if self.logger.check_errors():
            return False

        # get number of cores
        partition.core_count = xml_config_node.parse_attr(PARTITION_EXTENSION_CORES,
                                                          self.os_configuration.validate_core_count(self.core_count),
                                                          False, self.logger, 1)

        # get partition personality
        partition.personality = xml_config_node.parse_attr(PARTITION_EXTENSION_PERSONALITY,
                                                           self.os_configuration.validate_personalities(), True,
                                                           self.logger)


        print("   PARSING iop file attr")
        # get IOP configuration if available
        partition.iop_file = xml_config_node.parse_attr(PARTITION_EXTENSION_IOP,
                                                        VALID_XKY_FILE, False, self.logger, '')

        print("   done PARSING iop file attr")

        # check IOP is supported
        if not self.os_configuration.is_iop_supported() and partition.iop_file != '':
            self.logger.error(LOG_UNSUPPORTED_IOP, xml_config_node.sourceline)

        # sanity check
        if self.logger.check_errors():
            return False

        # get libs
        xml_libs_node = xml_config_node.parse_tag(PARTITION_EXTENSION_LIBS, 0, 1, self.logger)
        if len(xml_libs_node) > 0 and xml_libs_node[0].text is not None:

            # get available libraries and partition libraries
            available_libs = self.os_configuration.get_supported_libraries(partition.personality)
            partition_libs = parserutils.str2strList(xml_libs_node[0].text)

            # remove duplicates and convert to lower case
            available_libs = set(map(lambda x: x.lower(), available_libs))
            partition_libs = set(map(lambda x: x.lower(), partition_libs))

            # remove invalid libraries
            partition.libraries = []
            for lib in partition_libs:
                if lib not in available_libs:
                    self.logger.warning(LOG_PARTITION_LIBRARY_INVALID
                                        .format(xml_libs_node[0].sourceline, lib.upper(), partition))
                elif lib not in partition.libraries:
                    partition.libraries.append(lib)

        # get complete list of libraries
        partition.libraries = self.os_configuration.get_partition_complete_library_list(partition.libraries)

        # get devices
        xml_devices_node = xml_config_node.parse_tag(PARTITION_EXTENSION_DEVICES, 0, 1, self.logger)
        if len(xml_devices_node) > 0:
            partition.devices = xml_devices_node[0].parse_text([parserutils.str2strList, lambda x: True], self.logger, [])

        # get cache
        xml_cache_node = xml_config_node.parse_tag(PARTITION_EXTENSION_CACHE, 0, 1, self.logger)
        if len(xml_cache_node) > 0 and xml_cache_node[0].text is not None:
            initial_cache = xml_cache_node[0].parse_text(VALID_XKY_CACHE, self.logger, PARTITION_EXTENSION_CACHE_NONE)
            if PARTITION_EXTENSION_CACHE_DATA in initial_cache: partition.cache_state[0] = True
            if PARTITION_EXTENSION_CACHE_INST in initial_cache: partition.cache_state[1] = True

        # get permissions
        xml_permissions_node = xml_config_node.parse_tag(PARTITION_EXTENSION_PERMISSIONS, 0, 1, self.logger)
        if len(xml_permissions_node) > 0:

            # get available permissions and partition permissions
            available_permissions = self.os_configuration.get_available_permissions()
            partition_permissions = parserutils.str2strList(xml_permissions_node[0].text)

            # remove invalid permissions
            partition.permissions = []
            for p in partition_permissions:
                if p not in available_permissions:
                    self.logger.warning(LOG_PARTITION_PERMISSION_INVALID
                                        .format(xml_permissions_node[0].sourceline, p, partition))
                elif p not in partition.permissions:
                    partition.permissions.append(p)

        # get memory
        xml_memory_node = xml_config_node.parse_tag(PARTITION_EXTENSION_MEMORY, 0, 1, self.logger)
        if len(xml_memory_node) > 0:
            xml_memory_node = xml_memory_node[0]
            partition.memory_size = xml_memory_node.parse_attr(PARTITION_EXTENSION_MEMORY_SIZE,
                                                               self.os_configuration.validate_size(), True,
                                                               self.logger)
            partition.memory_unit = xml_memory_node.parse_attr(PARTITION_EXTENSION_MEMORY_UNIT,
                                                               self.os_configuration.validate_memory_unit(), False,
                                                               self.logger, self.os_configuration.get_default_memory_unit())

        # sanity check
        if self.logger.check_errors(): rc = False
        return rc


    def parse_window_extensions(self, xml_node, partition_schedule):

        # extensions return code
        rc = True

        # clear previous errors and warnings
        self.logger.clear_errors(3)

        # check if the windows have valid sizes within the ticks per second

        # add extension to the windows
        for window in partition_schedule.windows:
            window.cores = [0]

        # get extensions nodes
        xml_child_nodes = xml_node.parse_tag(WINDOW_EXTENSION, 0, sys.maxint, self.logger)
        for xml_extension_node in xml_child_nodes:

            wid = xml_extension_node.parse_attr(WINDOW_EXTENSION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            cores = xml_extension_node.parse_attr(WINDOW_EXTENSION_CORES,
                                                  self.os_configuration.validate_core_list(self.core_count), True,
                                                  self.logger)
            xml_line = xml_node.sourceline

            if self.logger.check_errors():
                rc = False
                continue

            # check if the extension belongs to a valid window
            window = [window for window in partition_schedule.windows if window.id == wid]
            if not window:
                self.logger.error(LOG_UNDEFINED, xml_line, WINDOW_STR_0.format(wid))
                rc = False
                continue

            # check if the number of cores is valid for the partition
            if partition_schedule.partition.core_count != len(cores):
                self.logger.error(LOG_WINDOW_EXT_CORES_INVALID_1, xml_line, partition_schedule.partition)
                rc = False
                continue

            # add extensions to window
            window[0].cores = cores

        # check if all the windows have the correct number of cores
        for window in partition_schedule.windows:
            if len(window.cores) != partition_schedule.partition.core_count:
                self.logger.error(LOG_WINDOW_EXT_CORES_INVALID_2, window, partition_schedule.partition)
                rc = False

        return rc

    def parse_schedule_extensions(self, xml_node, schedule):

        rc = True

        # add extension to schedule
        schedule.chain_of_execution = []

        # compute core chain of execution
        for i in range(0, self.core_count):
            chain = self.compute_core_schedule(i, schedule)
            schedule.chain_of_execution.append(chain)
            if not chain: rc = False

        return rc

    def compute_core_schedule(self, core_idx, schedule):

        # clear previous errors
        self.logger.clear_errors(1)

        # Get all windows running on core
        core_windows = sorted([window for window in
                               utils.flatten([partition_schedule.windows for partition_schedule in schedule.partitions])
                               if core_idx in window.cores],
                              key=lambda x: x.start)

        # check for window overflow/mtf overflow in the core schedule
        core_schedule_size = len(core_windows)
        for i, window in enumerate(core_windows):

            next_min_offset = window.start + window.duration

            # window overflow
            if i < core_schedule_size - 1:
                if next_min_offset > core_windows[i + 1].start:
                    self.logger.error(LOG_WINDOW_OVERFLOW_1, window.xml_line, window, core_windows[i + 1], core_idx)

            # MTF overflow
            else:
                if next_min_offset > schedule.mtf:
                    self.logger.error(LOG_WINDOW_OVERFLOW_2, window.xml_line, window, schedule, core_idx)

        # check if errors occurred
        if self.logger.check_errors(): return []

        # compute the chain of execution
        offset = 0
        chain_of_execution = []
        if core_schedule_size > 0:
            for i, window in enumerate(core_windows):

                # add a empty window at the beginning
                if window.start > offset:
                    empty_window = Window()
                    empty_window.period_start = False
                    empty_window.start = offset
                    empty_window.duration = window.start - offset
                    empty_window.cores = [core_idx]

                    offset += empty_window.duration
                    chain_of_execution.append(empty_window)

                # add current window
                chain_of_execution.append(window)
                offset += window.duration

                # add a empty window at the end
                if offset < schedule.mtf and i == core_schedule_size - 1:
                    empty_window = Window()
                    empty_window.period_start = False
                    empty_window.start = offset
                    empty_window.duration = schedule.mtf - offset
                    empty_window.cores = [core_idx]

                    offset += empty_window.duration
                    chain_of_execution.append(empty_window)
        else:
            # add an empty window with the size of the mtf
            empty_window = Window()
            empty_window.period_start = False
            empty_window.start = 0
            empty_window.duration = schedule.mtf
            empty_window.cores = [core_idx]
            chain_of_execution.append(empty_window)

        # check if the system resolution allows the current schedule
        for window in chain_of_execution:

            # check window start
            if (window.start / A653_TIMESCALE) * self.ticks_per_second != \
                    int((window.start / A653_TIMESCALE) * self.ticks_per_second):
                self.logger.error(LOG_WINDOW_RESOLUTION, window.xml_line, window, WINDOW_START)

            # check window duration
            if (window.duration / A653_TIMESCALE) * self.ticks_per_second != \
                    int((window.duration / A653_TIMESCALE) * self.ticks_per_second):
                self.logger.error(LOG_WINDOW_RESOLUTION, window.xml_line, window, WINDOW_DURATION)

        # check if errors occurred
        if self.logger.check_errors(): return []
        return chain_of_execution

    def parse_other(self, xml):

        print("   PARSING sm")
        # parse shared memory areas
        self.logger.event(0, LOG_EVENT_SHARED_MEMORY)
        xml_partitions = xml.parse_tag(SHM_EXTENSION, 0, sys.maxint, self.logger)
        for xml_node in xml_partitions: self.parse_shared_memory(xml_node)
        self.compute_shared_memory_addresses()

        print("   PARSING scheduling")
        # set schedule indexes
        for i, schedule in enumerate(self.schedules): schedule.index = i

        print("   PARSING partitions")
        # parse IOP configurations
        for partition in self.partitions:
            partition.iop = None
            if len(partition.iop_file):
                partition.iop = IOParser(self.os_configuration, partition, self, self.logger)
                partition.iop.parse(partition.iop_file)

    def parse_shared_memory(self, xml_node):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse shared memory node
        shm = SharedMemory()
        shm.id = xml_node.parse_attr(SHM_EXTENSION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        shm.name = xml_node.parse_attr(SHM_EXTENSION_NAME, VALID_NAME_TYPE, False, self.logger)
        shm.size = xml_node.parse_attr(SHM_EXTENSION_SIZE, self.os_configuration.validate_size(), False, self.logger)
        shm.unit = xml_node.parse_attr(SHM_EXTENSION_UNIT, self.os_configuration.validate_memory_unit(), False,
                                                           self.logger, self.os_configuration.get_default_memory_unit())
        shm.xml_line = xml_node.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        self.logger.information(1, shm.details())

        # parse partition access nodes
        rc = True
        xml_child_nodes = xml_node.parse_tag(SHM_EXTENSION_PARTITION, 1, sys.maxint, self.logger)
        for xml_child_node in xml_child_nodes:
            rc &= self.parse_shared_memory_partitions(xml_child_node, shm)

        # add shared memory to list
        if rc: self.shared_memory.append(shm)
        return rc

    def parse_shared_memory_partitions(self, xml_node, shm):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        shm_permissions = SharedMemoryPermissions()

        # parse partition attributes
        pid = xml_node.parse_attr(PARTITION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        name = xml_node.parse_attr(PARTITION_NAME, VALID_NAME_TYPE, False, self.logger)
        shm_permissions.permissions = xml_node.parse_attr(SHM_EXTENSION_PARTITION_PERMISSIONS,
                                                          self.os_configuration.validate_memory_permissions(), True,
                                                          self.logger)
        shm_permissions.xml_line = xml_node.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # get partition
        shm_permissions.partition = self.get_partition(pid, name)
        if not shm_permissions.partition:
            self.logger.error(LOG_UNDEFINED, xml_node.sourceline, PARTITION_STR.format(pid, name))
            return False

        # check for partition permissions redefinition
        if any(shm_permissions == other for other in shm.permissions):
            self.logger.error(LOG_REDEFINITION, xml_node.sourceline, shm_permissions)
            return False

        self.logger.information(2, shm_permissions.details())
        shm.permissions.append(shm_permissions)
        return True

    def compute_shared_memory_addresses(self):

        # check if we have any partition defined
        if len(self.partitions) == 0:
            return

        # get the maximum partition end addresses
        end_address = max([alignAddress(p.memory_vaddr + p.memory_size, p.memory_unit) for p in self.partitions])

        # compute the virtual address from each memory section
        for shm in self.shared_memory:
            shm.addr = alignAddress(end_address, shm.unit)
            end_address = alignAddress(shm.addr + shm.size, shm.unit)

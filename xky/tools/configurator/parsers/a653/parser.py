## @package parsers.a653.parser
#  @author pfnf
#  @brief A653 XML parser with support to custom XML extensions

__author__ = "pfnf"

import os
import sys
import utils
import utils.xml_parser as xml_parser

from definitions import *
from localization.logger import *

## A656 XML module parser
class a653parser(object):

    ## Constructor
    # @param self object pointer
    # @param logger logger to announce information
    def __init__(self, logger):
        self.schedules          = []                    # schedules list
        self.partitions         = []                    # partitions list
        self.system_hm_table    = []                    # system health-monitor table
        self.module_hm_table    = []                    # module health-monitor table
        self.channels           = []                    # module channels
        self.logger             = logger

    ## Get partition by Id or by Id and Name
    # @param self object pointer
    # @param pid partition Id
    # @param name partition name
    # @return Partition object if found, None otherwise
    def get_partition(self, pid, name):

        if name:
            matches = [ p for p in self.partitions if p.id == pid and p.name == name]
        else:
            matches = [ p for p in self.partitions if p.id == pid and not p.name]

        return None if not matches else matches[0]

    ## Get schedule by Id or by Id and Name
    # @param self object pointer
    # @param sid schedule Id
    # @param name schedule name
    # @return Schedule object if found, None otherwise
    def get_schedule(self, sid, name = None):

        if name is not None:
            matches = [s for s in self.schedules if s.id == sid and s.name == name]
        else:
            matches = [s for s in self.schedules if s.id == sid ]

        return None if not matches else matches[0]

    ## Parse partition
    # @param self object pointer
    # @param xml_node partition XML node
    # @return True if no errors occurred, False otherwise
    def parse_partition(self, xml_node):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        partition 				= Partition()
        partition.id          	= xml_node.parse_attr(PARTITION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        partition.name        	= xml_node.parse_attr(PARTITION_NAME, VALID_NAME_TYPE, False, self.logger)
        partition.is_system   	= xml_node.parse_attr(PARTITION_IS_SYSTEM, VALID_BOOLEAN_TYPE, False, self.logger)
        partition.criticality 	= xml_node.parse_attr(PARTITION_CRITICALITY, VALID_CRITICALITY_TYPE, False, self.logger)
        partition.entry_point 	= xml_node.parse_attr(PARTITION_ENTRY_POINT, VALID_NAME_TYPE, False, self.logger)
        partition.xml_line    	= xml_node.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # check for partition redefinition
        if any(partition == other for other in self.partitions):
            self.logger.error(LOG_REDEFINITION, xml_node.sourceline, partition)
            return False

        self.logger.information(1, partition.details())
        rc = True

        # parse partition ports
        #  - queuing ports
        xml_ports = xml_node.parse_tag(QUEUING_PORT, 0, sys.maxint, self.logger)
        for xml_port in xml_ports:
            rc &= self.parse_queuing_port(xml_port, partition)

        # - sampling ports
        xml_ports = xml_node.parse_tag(SAMPLING_PORT, 0, sys.maxint, self.logger)
        for xml_port in xml_ports:
            rc &= self.parse_sampling_port(xml_port, partition)

        # parse partition extensions
        rc &= self.parse_partition_extensions(xml_node, partition)

        # add partition to list of partitions
        if rc:
            partition.index = len(self.partitions)
            partition.directory = '{0}'.format(partition.name.lower())
            partition.executable = 'p{0}.exe'.format(partition.index)
            self.partitions.append(partition)

        return rc

    ## Parse queuing port
    # @param self object pointer
    # @param xml_node queuing port XML node
    # @param partition owner partition object pointer
    # @return True if no errors occurred, False otherwise
    def parse_queuing_port(self, xml, partition):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # parse queuing port
        qport                   = QueuingPort()
        qport.name              = xml.parse_attr(QUEUING_PORT_NAME, VALID_NAME_TYPE, True, self.logger)
        qport.direction         = xml.parse_attr(QUEUING_PORT_DIRECTION, VALID_DIRECTION_TYPE, True, self.logger)
        qport.discipline        = ''
        qport.max_nb_message    = xml.parse_attr(QUEUING_PORT_MAX_NB_MSGS, VALID_DECIMAL_TYPE, True, self.logger)
        qport.max_message_size  = xml.parse_attr(QUEUING_PORT_MAX_MSG_SIZE, VALID_DECIMAL_TYPE, True, self.logger)
        qport.xml_line          = xml.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # check for port redefinition
        if any(qport == other for other in partition.ports):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, qport)
            return False

        # Add port to partition port list
        self.logger.information(2, qport.details())
        qport.id = len(partition.ports)
        partition.ports.append(qport)
        return True

    ## Parse sampling port
    # @param self object pointer
    # @param xml_node sampling port XML node
    # @param partition owner partition object pointer
    # @return True if no errors occurred, False otherwise
    def parse_sampling_port(self, xml, partition):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # parse sampling port
        sport                   = SamplingPort()
        sport.name              = xml.parse_attr(SAMPLING_PORT_NAME, VALID_NAME_TYPE, True, self.logger)
        sport.direction         = xml.parse_attr(SAMPLING_PORT_DIRECTION, VALID_DIRECTION_TYPE, True, self.logger)
        sport.refresh_period    = xml.parse_attr(SAMPLING_PORT_REFRESH_RATE, VALID_FLOAT_TYPE, True, self.logger)
        sport.max_message_size  = xml.parse_attr(SAMPLING_PORT_MAX_MSG_SIZE, VALID_DECIMAL_TYPE, True, self.logger)
        sport.xml_line          = xml.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # check for port redefinition
        if any(sport == other for other in partition.ports):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, sport)
            return False

        # convert seconds to a653 time scale
        sport.refresh_period = int(sport.refresh_period * A653_TIMESCALE)

        # Add port to partition port list
        self.logger.information(2, sport.details())
        sport.id = len(partition.ports)
        partition.ports.append(sport)
        return True

    ## Parse module schedule
    # @param self object pointer
    # @param xml_node schedule XML node
    # @param multiple flag of multiple schedules available
    # @return True if no errors occurred, False otherwise
    def parse_schedule(self, xml_node, multiple):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        schedule = Schedule()
        schedule.id         = xml_node.parse_attr(SCHEDULE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        schedule.name       = xml_node.parse_attr(SCHEDULE_NAME, VALID_NAME_TYPE, True, self.logger)
        schedule.is_initial = xml_node.parse_attr(SCHEDULE_IS_INITIAL, VALID_BOOLEAN_TYPE, multiple, self.logger, True)
        schedule.mtf        = xml_node.parse_attr(SCHEDULE_MTF, VALID_FLOAT_TYPE, True, self.logger)
        schedule.xml_line   = xml_node.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # check for schedule redefinition
        if any(schedule == other for other in self.schedules):
            self.logger.error(LOG_REDEFINITION, xml_node.sourceline, schedule)
            return False

        # mark event
        self.logger.information(1, schedule.details())

        # convert MTF to ticks per second
        schedule.mtf = int(schedule.mtf * A653_TIMESCALE)

        # parse partition schedules
        rc = True
        xml_child_nodes = xml_node.parse_tag(PARTITION_SCHEDULE, 1, sys.maxint, self.logger)
        for xml_child_node in xml_child_nodes:
            rc &= self.parse_partition_schedule(xml_child_node, schedule)

        # parse schedule extensions
        rc &= self.parse_schedule_extensions(xml_node, schedule)

        # add schedule to list of schedules
        if rc: self.schedules.append(schedule)
        return rc

    ## Parse partition schedule
    # @param self object pointer
    # @param xml_node partition schedule XML node
    # @param schedule owner schedule object pointer
    # @return True if no errors occurred, False otherwise
    def parse_partition_schedule(self, xml_node, schedule):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # parse partition attributes
        partition_schedule = PartitionSchedule()
        pid  = xml_node.parse_attr(PARTITION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        name = xml_node.parse_attr(PARTITION_NAME, VALID_NAME_TYPE, False,self.logger)

        # sanity check
        if self.logger.check_errors(): return False

        # get partition
        partition_schedule.partition = self.get_partition(pid, name)
        if not partition_schedule.partition:
            self.logger.error(LOG_UNDEFINED, xml_node.sourceline, PARTITION_STR.format(pid, name))
            return False

        # check for partition schedule redefinition
        if any(partition_schedule == other for other in schedule.partitions):
            self.logger.error(LOG_REDEFINITION, xml_node.sourceline, partition_schedule)
            return False

        # parse schedule attributes
        partition_schedule.period        = xml_node.parse_attr(PARTITION_SCHEDULE_PERIOD, VALID_FLOAT_TYPE, True, self.logger)
        partition_schedule.duration      = xml_node.parse_attr(PARTITION_SCHEDULE_DURATION, VALID_FLOAT_TYPE, True, self.logger)
        partition_schedule.set_schedule  = xml_node.parse_attr(PARTITION_SCHEDULE_SET_SCHEDULE, VALID_BOOLEAN_TYPE, False, self.logger, False)
        partition_schedule.change_action = xml_node.parse_attr(PARTITION_SCHEDULE_CHANGE_ACTION, VALID_PARTITION_ACTION_TYPE, False, self.logger, 'IGNORE')
        partition_schedule.xml_line		 = xml_node.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        # Print partition schedule information
        self.logger.information(2, partition_schedule.details())

        # convert seconds to ticks
        partition_schedule.period   = int(partition_schedule.period   * A653_TIMESCALE)
        partition_schedule.duration = int(partition_schedule.duration * A653_TIMESCALE)

        # check if the period is a multiple of the module MTF
        if schedule.mtf % partition_schedule.period != 0.0:
            self.logger.error(LOG_SCHEDULE_INVALID_PERIOD, xml_node.sourceline, schedule)
            return False

        # parse execution windows
        rc = True
        xml_child_nodes = xml_node.parse_tag(WINDOW, 1, sys.maxint, self.logger)
        for xml_child_node in xml_child_nodes:
            rc &= self.parse_execution_window(xml_child_node, schedule, partition_schedule)

        # parse window extensions
        rc &= self.parse_window_extensions(xml_node, partition_schedule)

        # error parsing window
        if not rc: return False
        self.logger.clear_errors(3)

        # check window consistency
        periods_count = int(schedule.mtf / partition_schedule.period)
        periods_start = [ schedule.mtf ] * periods_count
        periods_started = [ -1 ] * periods_count
        periods_duration = [ 0 ] * periods_count
        for window in partition_schedule.windows:

            # check period overflow
            period_s_index = int(window.start / partition_schedule.period)
            if window.start + window.duration == schedule.mtf:
                period_e_index = periods_count - 1
            elif ((window.start + window.duration) - (period_s_index + 1) * partition_schedule.period) == 0:
                period_e_index = period_s_index
            else:
                period_e_index = int((window.start + window.duration) / partition_schedule.period)

            if period_s_index != period_e_index:
                self.logger.error(LOG_WINDOW_OVERFLOW_PERIOD, window.xml_line, window, partition_schedule)
                #return False

            # check if period start is correct
            if window.start < periods_start[period_s_index]:
                periods_start[period_s_index] = window.start

            # Check if the period start is correct
            if window.period_start:

                # Redefinition of period start
                if periods_started[period_s_index] != -1:
                    self.logger.error(LOG_WINDOW_PERIOD_START_REDEFINITION, window.xml_line, window)
                    return False

                # Store the real period start
                periods_started[period_s_index] = window.start

            periods_duration[period_s_index] += window.duration

        # validate the period durations
        for i, duration in enumerate(periods_duration):
            if duration != partition_schedule.duration:
                self.logger.error(LOG_SCHEDULE_INVALID_PERIOD_DURATION, partition_schedule)
                return False

        # add partition schedule to the module schedule
        schedule.partitions.append(partition_schedule)
        return True

    ## Parse execution window schedule
    # @param self object pointer
    # @param xml_node partition schedule XML node
    # @param schedule owner schedule object pointer
    # @param partition_schedule owner partition schedule object pointer
    # @return True if no errors occurred, False otherwise
    def parse_execution_window(self, xml_node, schedule, partition_schedule):

        # clear previous errors and warnings
        self.logger.clear_errors(3)

        # parse attributes
        window 				= Window()
        window.id           = xml_node.parse_attr(WINDOW_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        window.start        = xml_node.parse_attr(WINDOW_START, VALID_FLOAT_TYPE, True, self.logger)
        window.duration     = xml_node.parse_attr(WINDOW_DURATION, VALID_FLOAT_TYPE, True, self.logger)
        window.period_start = xml_node.parse_attr(WINDOW_PERIOD_START, VALID_BOOLEAN_TYPE, False, self.logger, False)
        window.xml_line		= xml_node.sourceline
        window.extensions   = None

        # sanity check
        if self.logger.check_errors(): return False

        # check for window redefinition
        if any(schedule == other for other in partition_schedule.windows):
            self.logger.error(LOG_REDEFINITION, xml_node.sourceline, window)
            return False

        # convert seconds to ticks
        window.start    	= int(window.start * A653_TIMESCALE)
        window.duration 	= int(window.duration * A653_TIMESCALE)
        window.partition 	= partition_schedule.partition

        # show window information
        self.logger.information(3, window.details())

        # add window to the partition schedule
        partition_schedule.windows.append(window)
        return True

    ## Parse channel
    # @param self object pointer
    # @param xml channel XML node
    # @return True if no errors occurred, False otherwise
    def parse_channel(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        channel             = Channel()
        channel.id          = xml.parse_attr(CHANNEL_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        channel.name        = xml.parse_attr(CHANNEL_NAME, VALID_NAME_TYPE, False, self.logger)
        channel.xml_line    = xml.sourceline

        # sanity check
        if self.logger.check_errors(): return False

        rc = True
        self.logger.information(1, channel.details())

        # parse source ports
        xml_src_nodes   = xml.parse_tag(CHANNEL_SRC, 1, sys.maxint, self.logger)
        for xml_node in xml_src_nodes:
            rc &= self.parse_channel_ports(xml_node, channel, PORT_SOURCE)

        # parse destination ports
        xml_dst_nodes = xml.parse_tag(CHANNEL_DST, 1, sys.maxint, self.logger)
        for xml_node in xml_dst_nodes:
            rc &= self.parse_channel_ports(xml_node, channel, PORT_DESTINATION)

        # obtain the channel type
        type_rc = True
        channel.type = ''
        for port in utils.flatten([ channel.src, channel.dst] ):
            if   isinstance(port, QueuingPort):
                if channel.type == SAMPLING_CHANNEL:
                    self.logger.error(LOG_CHANNEL_MIX_PORTS, channel.xml_line, channel)
                    type_rc = False
                    break
                channel.type = QUEUING_CHANNEL
            elif isinstance(port, SamplingPort):
                if channel.type == QUEUING_CHANNEL:
                    self.logger.error(LOG_CHANNEL_MIX_PORTS, channel.xml_line, channel)
                    type_rc = False
                    break
                channel.type = SAMPLING_CHANNEL

        rc &= type_rc

        # enforce one source
        if len(channel.src) > 1:
            self.logger.error(LOG_CHANNEL_ONLY_ONE_SRC, channel.xml_line, channel)
            rc = False

        # enforce one destination for queuing channels
        if type_rc and channel.type == QUEUING_CHANNEL and len(channel.dst) > 1:
            self.logger.error(LOG_CHANNEL_ONLY_ONE_DST, channel.xml_line, channel)
            rc = False

        # check if all ports configuration match
        if rc:
            #  -> channel is queuing
            if channel.type == QUEUING_CHANNEL:
                max_message_size = channel.src[0].max_message_size
                max_nb_message   = channel.src[0].max_nb_message
                discipline       = channel.src[0].discipline
                for port in utils.flatten([ channel.src, channel.dst]):
                    if port.max_message_size != max_message_size or \
                       port.max_nb_message != max_nb_message or \
                       port.discipline != discipline:
                        self.logger.error(LOG_CHANNEL_PORT_MISMATCH, channel.xml_line, channel)
                        rc = False
                        break

            # -> channel is sampling
            if channel.type == SAMPLING_CHANNEL:
                max_message_size = channel.src[0].max_message_size
                refresh_period   = channel.src[0].refresh_period
                for port in utils.flatten([ channel.src, channel.dst]):
                    if port.max_message_size != max_message_size or \
                       port.refresh_period != refresh_period:
                        self.logger.error(LOG_CHANNEL_PORT_MISMATCH, channel.xml_line, channel)
                        rc = False
                        break

        # perform validation
        if rc: self.channels.append(channel)
        return rc

    ## Parse channel port
    # @param self object pointer
    # @param xml port list XML node
    # @param channel current channel
    # @param direction direction of the ports
    # @return True if no errors occurred, False otherwise
    def parse_channel_ports(self, xml, channel, direction):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # get port nodes
        xml_nodes = xml.parse_tag(CHANNEL_STANDARD_PARTITION, 1, sys.maxint, self.logger)

        # sanity check
        if self.logger.check_errors(): return False

        rc = True

        # parse ports
        for xml_node in xml_nodes:

            # clear previous errors and warnings
            self.logger.clear_errors(1)

            # parse partition attributes
            pid = xml_node.parse_attr(PARTITION_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            name = xml_node.parse_attr(PARTITION_NAME, VALID_NAME_TYPE, False, self.logger)
            port = xml_node.parse_attr(CHANNEL_PORT_NAME, VALID_NAME_TYPE, True, self.logger)

            # sanity check
            if self.logger.check_errors():
                rc = False
                continue

            # get partition
            partition = self.get_partition(pid, name)
            if not partition:
                self.logger.error(LOG_UNDEFINED, xml_node.sourceline, PARTITION_STR.format(pid, name))
                rc = False
                continue

            # get port from partition ports
            ports = [ p for p in partition.ports if p.direction == direction and p.name == port ]
            if len(ports) == 0:
                self.logger.error(LOG_UNDEFINED, xml_node.sourceline, '{0} ({1}) in {2}'.format(port, direction, partition))
                rc = False
                continue

            # check if port is already in list
            ports = ports[0]
            if ports.used:
                self.logger.error(LOG_ALREADY_IN_USE_PORT, xml_node.sourceline, ports)
                rc = False
                continue

            # flag port as used
            ports.used = True

            # add port to list
            if   direction == PORT_SOURCE:
                channel.src.append(ports)
            elif direction == PORT_DESTINATION:
                channel.dst.append(ports)

        return rc

    ## Parse file
    # @param self object pointer
    # @param file_name file name of the XML configuration
    def parse(self, file_name):

        # open XML file
        xml = xml_parser.xmlOpen(os.path.abspath(file_name), self.logger, None)
        if xml is None:
            return

        # parse module extensions
        self.logger.event(0, LOG_EVENT_MODULE_CONFIG)
        self.parse_module_extensions(xml)

        # parse system health-monitor configuration
        self.logger.event(0, LOG_EVENT_SYSTEM_HM)
        xml_node = xml.parse_tag(SYSTEM_HM, 0, 1, self.logger)
        self.parse_system_hm(xml_node)

        # parse module health-monitor configuration
        if self.system_hm_table:
            self.logger.event(0, LOG_EVENT_MODULE_HM)
            xml_node = xml.parse_tag(MODULE_HM, 0, 1, self.logger)
            if xml_node is not None: self.parse_hm_table(xml_node, MODULE_LEVEL)

        # parse partitions
        self.logger.event(0, LOG_EVENT_PARTITIONS)
        xml_partitions = xml.parse_tag(PARTITION, 1, sys.maxint, self.logger)
        for xml_node in xml_partitions: self.parse_partition(xml_node)

        # parse schedule
        self.logger.event(0, LOG_EVENT_SCHEDULES)
        xml_schedules = xml.parse_tag(SCHEDULE, 1, sys.maxint, self.logger)
        for xml_node in xml_schedules: self.parse_schedule(xml_node, len(xml_schedules) > 1)

        # parse partitions health-monitor configuration
        if self.system_hm_table:
            self.logger.event(0, LOG_EVENT_PARTITIONS_HM)
            xml_partitions_hm = xml.parse_tag(PARTITION_HM, 0, sys.maxint, self.logger)
            for xml_node in xml_partitions_hm: self.parse_hm_table(xml_node, PARTITION_LEVEL)

        # complete HM tables
        self.complete_hm_tables()

        print("  PARSING connection table")

        # parse connection table
        self.logger.event(0, LOG_EVENT_CONNECTION_TABLE)
        xml_connection_table = xml.parse_tag(CONNECTION_TABLE, 0, 1, self.logger)
        if len(xml_connection_table) > 0:
            xml_connection_table = xml_connection_table[0]
            xml_channels = xml_connection_table.parse_tag(CHANNEL, 0, sys.maxint, self.logger)
            for xml_node in xml_channels: self.parse_channel(xml_node)

        print("  PARSING ports table")
        # assign global index to the ports and check for unused ports
        index = 0
        self.logger.clear_errors(0)
        for partition in self.partitions:
            for port in partition.ports:
                port.index = index
                if not port.used:
                    self.logger.error(LOG_UNUSED_PORT, port.xml_line, port)


        print("  PARSING others")
        # parse other extensions
        self.parse_other(xml)
        print("  done PARSING others")

    def parse_hm_table(self, xml, target): pass
    def complete_hm_tables(self): pass
    def parse_module_extensions(self, xml): pass
    def parse_partition_extensions(self, xml, partition): pass
    def parse_schedule_extensions(self, xml, schedule): pass
    def parse_window_extensions(self, xml, partition_schedule): pass
    def parse_other(self, xml): pass
    def parse_system_hm(self, xml): pass
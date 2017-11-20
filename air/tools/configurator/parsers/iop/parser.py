
import os
import sys
import air
import utils
import utils.xml_parser as xml_parser
from utils.logger import Logger
from re import match
from localization.logger import *

# python
from os import path
from sys import maxint



from devices import *
from definitions import *
from parsers.a653.definitions import *

class IOParser(object):

    ##
    def __init__(self, os_configuration, partition = None, a653parser = None, logger = None):

        self.logical_devices    = []
        self.physical_devices   = []
        self.ports              = []
        self.schedules          = []
        self.schedules_count    = 0
        self.routes             = []
        self.logger             = logger
        self.requests           = 1024
        self.ttl                = 5
        self.partition          = partition
        self.a653parser         = a653parser

        # air & IOP configuration
        self.os_configuration = os_configuration
        self.iop_configuration = os_configuration.get_iop_configuration()

    def get_logical_device(self, ld_id):

        ldevices = [ ld for ld in self.logical_devices if ld.id == ld_id ]
        return ldevices[0] if len(ldevices) == 1 else None

    def get_physical_device(self, pd_id):

        self.logger.event(0, "PHY DEVS", self.physical_devices)
        pdevices = [pd for pd in self.physical_devices if pd.id == pd_id]
        return pdevices[0] if len(pdevices) == 1 else None


    def get_remote_port(self, port_id):

        ports = [ port for port in self.ports if port.name == port_id ]
        return ports[0] if len(ports) > 0 else None

    ## Parse logical device
    # @param self object pointer
    # @param xml logical device xml node
    def parse_logical_device(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        ldevice         = LogicalDevice()
        ldevice.id      = xml.parse_attr(LOGICAL_DEVICE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        ldevice.name    = xml.parse_attr(LOGICAL_DEVICE_NAME, VALID_NAME_TYPE, False, self.logger)

        # sanity check
        if self.logger.check_errors(): return False

        # check for redefinitions
        if any(ldevice == other for other in self.logical_devices):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, ldevice)
            return False

        # store logical device
        self.logger.information(1, ldevice.details())
        ldevice.idx = len(self.logical_devices)
        self.logical_devices.append(ldevice)
        return True

    ## Parse remote ports
    # @param self object pointer
    # @param xml remote ports xml node
    def parse_remote_port(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        name       = xml.parse_attr(REMOTE_PORT_NAME, VALID_NAME_TYPE, True, self.logger)
        if self.logger.check_errors(): return False

        # get port configuration from module
        if self.a653parser is not None and self.partition is not None:

            ports = [ p for p in self.partition.ports if p.name == name ]

            # check if port exists
            if len(ports) == 0:
                self.logger.error(LOG_UNDEFINED, xml.sourceline, REMOTE_PORT_STR.format(name))
                return False

            # get port and set logical device
            port = ports[0]
            port.logical_device = None

            # get logical device if port is a destination port
            if port.direction == PORT_DESTINATION:

                ld_id = xml.parse_attr(REMOTE_PORT_LOGICAL_DEVICE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)

                # sanity check
                if self.logger.check_errors(): return False

                # check if logical device exists
                port.logical_device = self.get_logical_device(ld_id)
                if port.logical_device is None:
                    self.logger.error(LOG_UNDEFINED, xml.sourceline, LOGICAL_DEVICE_STR.format(ld_id, '---'))
                    return False

        else:

            ## @todo implement standalone mode
            self.logger.error('Standalone mode is not currently supported...')
            return False

        # check for port redefinition
        if any(port == other for other in self.ports):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, REMOTE_PORT_STR.format(port.name))
            return False

        # save port
        self.logger.information(2, port.details())
        self.ports.append(port)

    ## Parse physical device
    # @param self object pointer
    # @param xml logical device xml node
    def parse_physical_device(self, xml):

        self.logger.event(0, "     PARSING physical device")

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # parse attributes
        pdevice = PhysicalDevice()
        pdevice.id = xml.parse_attr(PHYSICAL_DEVICE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        pdevice.device = xml.parse_attr(PHYSICAL_DEVICE_NAME, VALID_NAME_TYPE, True, self.logger)

        self.logger.event(0, "dev:", pdevice.device)
        # sanity check
        if self.logger.check_errors(): return False

        # convert to lower case
        pdevice.device = pdevice.device.lower()

        # get device alias
        try:
            self.logger.event(0, "alias0: ", pdevice.device)
            self.logger.event(0, "alias: ", self.iop_configuration.alias[pdevice.device])
            alias = self.iop_configuration.alias[pdevice.device]
            pdevice.device = alias

        except:
            logger.logger.exception("Exception, See details below")
            pass

        self.logger.event(0, "dev: ", pdevice.device)
        # check for redefinitions
        if any(pdevice == other for other in self.physical_devices):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, pdevice)
            return False

        self.logger.event(0, "BEFORE")
        # check if the device is valid
        bsp_devices = self.iop_configuration.devices
        if pdevice.device not in bsp_devices:
            self.logger.error(LOG_UNSUPPORTED_DEVICE, xml.sourceline, pdevice.device)
            return False

        self.logger.event(0, "AFTER", self.iop_configuration.devices)
        # extract the device name
        regex_match = match(PHYSICAL_DEVICE_PATTERN, pdevice.device)
        if not regex_match:
            self.logger.error(LOG_UNSUPPORTED_DEVICE, xml.sourceline, pdevice.device)
            return False

        self.logger.event(0, "BEFORE")
        pdevice.type  = regex_match.group('type').upper()
        pdevice.minor = regex_match.group('minor').upper()

        self.logger.event(0, "AFTER", pdevice.type)
        # check if a parser for that device is available
        if pdevice.type not in iop_supported_devices.keys():
            self.logger.error(LOG_UNSUPPORTED_DEVICE, xml.sourceline, pdevice.device)
            return False

        # get device parser
        pdevice.parser = iop_supported_devices[pdevice.type]

        rc = True
        pdevice.type    = pdevice.parser[0]

        # parse device setup
        if not pdevice.parser[1](self, xml, pdevice):
            return False

        self.logger.information(1, pdevice.setup.details())

        self.logger.event('BEFORE parsing physical routing')

        if ( pdevice.type == RTR ):
            pdevice.idx = len(self.physical_devices)
            self.physical_devices.append(pdevice)
            self.logger.event(0, "     done PARSING SPWRTR device")
            return True

        # Parse physical Routing
        xml_routes = xml.parse_tag(ROUTE_PHYSICAL, 1, maxint, self.logger)
        for xml_route in xml_routes:
            rc &= self.parse_device_routes(xml_route, pdevice)
            self.logger.event(rc)

        self.logger.event('AFTER parsing physical routing and BEFORE logical')
        # Parse Logical Routing
        xml_routes = xml.parse_tag(ROUTE_LOGICAL, 1, maxint, self.logger)
        for xml_route in xml_routes:
            rc &= self.parse_device_routes(xml_route, pdevice)
            self.logger.event(rc)

        self.logger.event(0, "PHYDEV?", rc)

        if rc:
            pdevice.idx = len(self.physical_devices)
            self.physical_devices.append(pdevice)

        self.logger.event(0, "     done PARSING physical device")
        return rc

    ## Parse routes
    # @param self object pointer
    def parse_device_routes(self, xml, pdevice):

        # clear previous errors and warnings
        self.logger.clear_errors(2)

        # parse physical route
        if xml.tag == ROUTE_PHYSICAL:

            route      = RoutePhysical()
            route.id   = xml.parse_attr(ROUTE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            port_id    = xml.parse_attr(ROUTE_PHYSICAL_PORT, VALID_NAME_TYPE, True, self.logger)

            # sanity check
            if self.logger.check_errors(): return False

            # check if remote port exists
            self.logger.event(0, "PHY PORT ID: ", port_id)
            route.port = self.get_remote_port(port_id)
            if route.port is None:
                self.logger.error(LOG_UNDEFINED, xml.sourceline, REMOTE_PORT_STR.format(port_id))
                return False

            pdevice.routes.append(route)

        # parse logical route
        elif xml.tag == ROUTE_LOGICAL:

            route      = RouteLogical()
            route.id   = xml.parse_attr(ROUTE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            ld_id      = xml.parse_attr(ROUTE_LOGICAL_DEVICE, VALID_IDENTIFIER_TYPE, True, self.logger)

            # sanity check
            if self.logger.check_errors(): return False

            # check if logical device exists
            ldevice = self.get_logical_device(ld_id)
            self.logger.event(0, "LOGICAL DEV is ", ldevice)
            if ldevice is None:
                self.logger.error(LOG_UNDEFINED, xml.sourceline, LOGICAL_DEVICE_STR.format(ld_id, '---'))
                return False

            ldevice.routes.append(route)

        # invalid route
        else:
            self.logger.error(LOG_INVALID_TAG, xml.sourceline, xml.tag)
            return  False

        route.device = pdevice

        # check if route already exists
        if any(route == other for other in self.routes):
            self.logger.error(LOG_REDEFINITION, xml.sourceline, route)
            return False

        # parse header
        route.header = self.parse_header(xml, pdevice)
        if route.header is None: return False

        # parse complete
        self.logger.information(2, route.details())
        self.logger.information(3, route.header.details())
        self.routes.append(route)
        return True

    ## Parse router header
    # @param self object pointer
    # @param xml route XML node
    # @param pdevice physical device configuration
    def parse_header(self, xml, pdevice):

        # clear previous errors and warnings
        self.logger.clear_errors(3)

        # device of ethernet type
        if pdevice.type == ETH:
            xml_header = xml.parse_tag(ETHHEADER, 1, 1, self.logger)
            if self.logger.check_errors(): return None
            return self.parse_eth_header(xml_header)
        
        # device of spacewire type
        elif pdevice.type == SPW:
            xml_header = xml.parse_tag(SPWHEADER, 1, 1, self.logger)
            if self.logger.check_errors(): return None
            return self.parse_spw_header(xml_header)
        
        # device of canbus type
        elif pdevice.type == CAN:
            xml_header = xml.parse_tag(CANHEADER, 1, 1, self.logger)
            if self.logger.check_errors(): return None
            return self.parse_can_header(xml_header)
            

        # invalid header
        return None

    ## Parse Ethernet Header
    # @param self object pointer
    # @param xml Ethernet Header xml node
    def parse_eth_header(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(3)

        # parse attributes
        header = EthHeader()
        header.ip = xml.parse_attr(ETHHEADER_IP, VALID_IP, True, self.logger)
        header.mac = xml.parse_attr(ETHHEADER_MAC, VALID_MAC, True, self.logger)
        header.port = xml.parse_attr(ETHHEADER_PORT, VALID_PORT, True, self.logger)

        # sanity check
        if self.logger.check_errors(): return False
        return header

    ## Parse SpaceWire Header
    # @param self object pointer
    # @param xml SpaceWire Header xml node
    def parse_spw_header(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(3)

        # parse attributes
        header = SpwHeader()
        header.address = xml.parse_attr(SPWHEADER_ADDRESS, VALID_SPW_ADDRESS, True, self.logger)

        # sanity check
        if self.logger.check_errors(): return False
        return header
    
    ## Parse CANBUS Header
    # @param self object pointer
    # @param xml SpaceWire Header xml node
    def parse_can_header(self, xml):
        
        # clear previous errors and warning
        self.logger.clear_errors(3)
        
        # parse attributes
        header = CanHeader()
        header.extended = xml.parse_attr(CANHEADER_EXTENDED, VALID_BOOLEAN_TYPE, True, self.logger)
        header.rtr = xml.parse_attr(CANHEADER_RTR, VALID_BOOLEAN_TYPE, True, self.logger)
        header.can_id = xml.parse_attr(CANHEADER_ID, VALID_ID, True, self.logger)

        # sanity check
        if self.logger.check_errors(): return False
        return header
    
    def parse_schedule(self, xml):

        # clear previous errors and warnings
        self.logger.clear_errors(0)

        # get scheduler id
        schedule_id = xml.parse_attr(SCHEDULE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        if self.logger.check_errors(): return None

        # get schedule configuration from module
        if self.a653parser is not None and self.partition is not None:

            # check if schedule exists
            schedule = self.a653parser.get_schedule(schedule_id)
            if schedule is None:
                self.logger.error(LOG_UNDEFINED, xml.sourceline, SCHEDULE_STR.format(schedule_id, '---'))
                return False

            # check if the partition is scheduled
            partition_schedule = schedule.get_partition_schedule(self.partition)
            if partition_schedule is None:
                self.logger.error(LOG_PARTITION_NOT_SCHEDULED_1, xml.sourceline, self.partition, schedule)
                return False

            self.logger.information(1, partition_schedule.details())

            # define routes scheduling
            route_schedule          = RouteSchedule()
            route_schedule.index    = schedule.index
            route_schedule.period   = partition_schedule.period
            route_schedule.duration = partition_schedule.duration

            # check if this route scheduler is already defined
            if any(route_schedule == other for other in self.schedules):
                self.logger.error(LOG_REDEFINITION, xml.sourceline, route_schedule)
                return False

            # parse devices setup
            rc = self.parse_schedule_devices(xml, route_schedule)

            # parse active routes
            xml_routes = xml.parse_tag(SCHEDULING_ROUTE, 0, sys.maxint, self.logger)
            for xml_node in xml_routes:
                rc &= self.parse_schedule_routes(xml_node, route_schedule)

            # sanity check
            if not rc:

                return False

            # add route schedule to list
            self.schedules.append(route_schedule)
            return True

        else:

            ## @todo implement standalone mode
            self.logger.error('Standalone mode is not currently supported...')
            return False

    ## Parse schedule routes
    # @param self object pointer
    # @param xml route XML node
    # @param route_schedule current schedule information
    # @return True if no errors, False otherwise
    def parse_schedule_routes(self, xml, route_schedule):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # parse routes configurations
        rid    = xml.parse_attr(SCHEDULING_ROUTE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
        active = xml.parse_attr(SCHEDULING_ROUTE_ACTIVE, VALID_BOOLEAN_TYPE, False, self.logger, True)
        if self.logger.check_errors(): return False

        # check if route exits
        routes = [ r for r in self.routes if r.id == rid ]
        if len(routes) != 1:
            self.logger.error(LOG_UNDEFINED, xml.sourceline, ROUTE_STR.format(rid))
            return False

        # add route to list of active routes
        route = routes[0]
        if active and not route in route_schedule.routes:
            route_schedule.routes.append(route)
            self.logger.information(2, route.details())

        return True

    def parse_schedule_devices(self, xml, route_schedule):

        # clear previous errors and warnings
        self.logger.clear_errors(1)

        # get devices nodes
        rc = True
        xml = xml.parse_tag(SCHEDULING_DEVICE, 1, maxint, self.logger)

        for xml_device in xml:

            # get device configuration
            pd_id  = xml_device.parse_attr(SCHEDULING_DEVICE_ID, VALID_IDENTIFIER_TYPE, True, self.logger)
            if self.logger.check_errors():
                rc = False
                continue

            # check if device exists
            pdevice  = self.get_physical_device(pd_id)

            self.logger.event(0, "BEFORE PARSING PDEV SCHED", pdevice)
            if pdevice is None:
                self.logger.error(LOG_UNDEFINED, xml_device.sourceline, PHYSICAL_DEVICE_STR.format(pd_id, '---'))
                rc = False
                continue

            # get specific configuration
            device = pdevice.parser[2](self, xml_device, pdevice)
            if device is None:
                rc = False
                continue

            self.logger.information(2, device.details())
            route_schedule.devices.append(device)

        return  rc

    ## Parse file
    # @param self object pointer
    # @param file_name file name of the XML configuration
    def parse(self, file_name):

        # check if we are in standalone mode
        if self.a653parser is None or self.partition is None:
            self.logger.error('Standalone mode is not currently supported...')
            return False

        # open XML file
        xml = xml_parser.xmlOpen(os.path.join(air.WORKING_DIRECTORY, file_name), self.logger, None)
        if xml is None: return False

        # parse logical devices
        self.logger.event(0, LOG_EVENT_IOP_LDEVICES)
        xml_ldevices = xml.parse_tag(LOGICAL_DEVICE, 0, sys.maxint, self.logger)
        for xml_node in xml_ldevices: self.parse_logical_device(xml_node)

        # parse remote ports
        self.logger.event(0, LOG_EVENT_IOP_REMOTE_PORTS)
        xml_rports = xml.parse_tag(REMOTE_PORT, 0, sys.maxint, self.logger)
        for xml_node in xml_rports: self.parse_remote_port(xml_node)

        # parse physical devices
        self.logger.event(0, LOG_EVENT_IOP_PDEVICES)
        xml_pdevices = xml.parse_tag(PHYSICAL_DEVICE, 0, sys.maxint, self.logger)
        for xml_node in xml_pdevices: self.parse_physical_device(xml_node)

        # parse scheduling
        self.logger.event(0, LOG_EVENT_IOP_SCHEDULE)
        xml_schedules = xml.parse_tag(SCHEDULING, 1, sys.maxint, self.logger)
        for xml_node in xml_schedules: self.parse_schedule(xml_node)

        # define IOP port index
        for i, port in enumerate(self.ports): port.iop_index = i

        # apply arch configuration
        self.os_configuration.applyIOPArchConfiguration(self.a653parser, self.partition)

        return True

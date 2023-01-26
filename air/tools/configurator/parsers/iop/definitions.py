
SPW  = 'SPW'
MIL  = 'MIL'
ETH  = 'ETH'
RTR  = 'SPWRTR'
CAN  = 'CAN'
UART = 'UART'
SD   = 'SD'
ADC  = 'ADC'
GPIO = 'GPIO'



SUPPORTED_DEVICES    = {    SPW : ['GRSPW'],
                            MIL : ['GRMIL', 'BRM1553'],
                            ETH : ['GRETH', 'XETH'],
                            RTR : ['SPWRTR'],
                            CAN : ['GRCAN', 'XCAN'],
                            UART: ['XUART'],
                            ADC : ['XADC'],
                            SD  : ['XSD'],
                            GPIO: ['XGPIO']	}



IOP_PORT_TYPE        = { 'SamplingPort' : 'IOP_SAMPLING_PORT',
                         'QueuingPort'  : 'IOP_QUEUING_PORT'   }



LOGICAL_DEVICE                  = 'LogicalDevices/Device'
LOGICAL_DEVICE_ID               = 'Id'
LOGICAL_DEVICE_NAME             = 'Name'


REMOTE_PORT                     = 'RemotePorts/Port'
REMOTE_PORT_NAME                = 'Name'
REMOTE_PORT_LOGICAL_DEVICE_ID   = 'LogicalDeviceId'


PHYSICAL_DEVICE                 = 'PhysicalDevices/Device'
PHYSICAL_DEVICE_ID              = 'Id'
PHYSICAL_DEVICE_NAME            = 'Device'
PHYSICAL_DEVICE_PATTERN         = '(?P<type>[a-zA-Z]{1,})(?P<minor>[0-9]{1})'


ROUTE_PHYSICAL                  = 'PhysicalRoute'
ROUTE_LOGICAL                   = 'LogicalRoute'
ROUTE_ID                        = 'Id'


ROUTE_PHYSICAL_PORT             = 'PortId'
ROUTE_LOGICAL_DEVICE            = 'LogicalDeviceId'



#MIL LIST
MILLIST                         = 'MILList'
MIL_ID                          = 'Id'
MILLISTMJFRAME                  = 'MajorFrame'
#MIL SLOT
MILSLOT                         = 'Slot'
MILSLOTBUS                      = 'Bus'
MILSLOTTYPE                     = 'Type'
MILSLOTWCMODE                   = 'WCMC'
MILSLOTTIME                     = 'Time'
MILSLOADDRTX                    = 'AddrTx'
MILSLOSUBADDRTX                 = 'SubAddrTx'


# Top Level Nodes
IOPARTITION                     = 'IOPartition'


SCHEDULING                      = 'ModuleSchedules/Schedule'
SCHEDULING_ROUTE                = 'RoutesConfiguration/Route'
SCHEDULING_ROUTE_ID             = 'RouteId'
SCHEDULING_ROUTE_ACTIVE         = 'Active'
SCHEDULING_DEVICE               = 'DevicesConfiguration/Device'
SCHEDULING_DEVICE_ID            = 'DeviceId'


#Top level attributes
DATE_ATTR                       = 'date'
NAME_ATTR                       = 'name'
REQUEST_NUMBER_ATTR             = 'request_number'
TIME_TO_LIVE_ATTR               = 'time_to_live'
VERSION_ATTR                    = 'version'



# ETH Header
ETHHEADER                       = 'EthHeader'
ETHHEADER_IP                    = 'Ip'
ETHHEADER_MAC                   = 'MAC'
ETHHEADER_PORT                  = 'Port'


#SPW Header
SPWHEADER                       = 'SpwHeader'
SPWHEADER_ADDRESS               = 'Address'


#CANBUS Header
CANHEADER                       = 'CanHeader'
CANHEADER_EXTENDED              = 'Extended'
CANHEADER_SSHOT                 = 'Sshot'
CANHEADER_RTR                   = 'RTR'
CANHEADER_ID                    = 'CanID'


#MIL Header
MILHEADER                       = 'MILHeader'
MIL_ADDR                        = 'Addr'
MIL_SUBADDR                     = 'SubAddr'


#UART Header
UARTHEADER                      = 'UartHeader'
UARTHEADER_ID                   = 'UartID'


#SD Header
SDHEADER                        = 'SdHeader'
SDHEADER_SIZE                   = 'Size'
SDHEADER_FILE                   = 'File'


#ADC Header
ADCHEADER                      = 'AdcHeader'
ADCHEADER_ID                   = 'AdcId'

#GPIO Header
GPIOHEADER                     = 'GpioHeader'
GPIO_PIN                       = 'Pin'
GPIO_WRITE                     = 'Write'



import utils.parser as parserutils

VALID_STR               = [ str, lambda  x : len(x) ]
VALID_NAME_TYPE         = [ str, lambda x : 0 < len(x) < 31 ]
VALID_DEVICE_ID_TYPE    = [ parserutils.str2int, lambda x : x >= 0 ]
VALID_PORT              = [ parserutils.str2int, lambda x : x > 0]
VALID_IP                = [ lambda x: str(x).strip().split('.'), lambda x: len(x) == 4 ]
VALID_MAC               = [ lambda x: str(x).strip().split(':'), lambda x: len(x) == 6 ]
VALID_SPW_ADDRESS       = [ lambda x: str(x).strip().split(','), lambda x: len(x) <= 32 ]
VALID_FLOAT_TYPE        = [ parserutils.str2float, lambda x : x >= 0 ]
VALID_DECIMAL_TYPE      = [ parserutils.str2int, lambda x : x >= 0 ]
VALID_BOOLEAN_TYPE      = [ parserutils.str2bool, lambda x : isinstance(x, bool) ]
VALID_DIRECTION_TYPE    = [ str, lambda x : x in [ REMOTE_PORT_SRC, REMOTE_PORT_DST] ]
VALID_ID                = [ parserutils.str2int, lambda x: x >= 0]
VALID_SD_FILE_SIZE      = [ parserutils.str2int, lambda x: x >= 0]
VALID_MASK_CODE         = [ lambda x: str(x).strip().split(':'), lambda x: len(x) == 4]
VALID_MILMJFRAME_TYPE   = [ parserutils.str2int, lambda x : 0 <= x <= 10000000 ]
VALID_MILADDR_TYPE      = [ parserutils.str2int, lambda x : 0 <= x <= 31 ]
VALID_MILSUBADDR_TYPE   = [ parserutils.str2int, lambda x : 0 <= x <= 31 ]
VALID_MILADDRTX_TYPE    = [ parserutils.str2int, lambda x : 0 <= x <= 30 ]
VALID_MILSUBADDRTX_TYPE = [ parserutils.str2int, lambda x : 1 <= x <= 30 ]
VALID_MILBUS_TYPE       = [ str, lambda x : x in [ 'A', 'B'] ]
VALID_MILTYPE           = [ str, lambda x : x in [ 'BC_RT', 'RT_BC', 'RT_RT'] ]
VALID_MILWCMODE_TYPE    = [ parserutils.str2int, lambda x : 0 <= x <= 32 ]
VALID_MILTIME_TYPE      = [ parserutils.str2int, lambda x : 0 <= x <= 1000000 ]
VALID_TIME_TO_LIVE      = [ parserutils.str2int, lambda x : 0 <= x <= 10 ]



LOGICAL_DEVICE_STR      = 'Logical Device (Id: {0}, Name: {1})'
PHYSICAL_DEVICE_STR     = 'Physical Device (Id: {0}, Device: {1})'
ETH_HEADER_STR          = 'Ethernet Header (Mac: {0}, Ip: {1}, Port: {2})'
SPW_HEADER_STR          = 'SpaceWire Header (Address: {0})'
CAN_HEADER_STR          = 'Canbus Header (extended: {0}, SShot: {1}, Rtr: {2}, CanId: {3})'
UART_HEADER_STR         = 'Uart Header (UartID: {0})'
SD_HEADER_STR           = 'SD Header (Size: {0}, File: {1})'
ADC_HEADER_STR          = 'Adc Header (AdcId: {0})'
GPIO_HEADER_STR         = 'Gpio Header (Pin: {0}, Write: {1})'
MIL_HEADER_STR          = 'MIL-STD-1553 Header (ADDR: {0}, SUBADDR: {1})'
MIL_LIST_STR            = 'MIL LIST (Id: {0})'
MIL_SLOT_STR            = 'MIL SLOT (Id: {0} Type:{1} ADDR: {2}, SUBADDR: {3})'
REMOTE_PORT_STR         = 'Remote Port (Name: {0})'
ROUTE_STR               = 'Route (Id: {0})'
ROUTE_PHYSICAL_STR      = 'Physical Route (Id: {0})'
ROUTE_LOGICAL_STR       = 'Logical Route (Id: {0})'



## Logical Device
class LogicalDevice(object):

    def __init__(self):
        self.id             = 0
        self.name           = ''
        self.routes         = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return LOGICAL_DEVICE_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## Physical Device
class PhysicalDevice(object):

    def __init__(self):
        self.id             = 0
        self.device         = ''
        self.type           = ''
        self.minor          = 0
        self.routes         = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.device and self.device == other.device)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return PHYSICAL_DEVICE_STR.format(self.id, self.device)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## Physical Route
class RoutePhysical(object):

    def __init__(self):
        self.id     = -1
        self.port   = None
        self.header = None

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_PHYSICAL_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## Logical Route
class RouteLogical(object):

    def __init__(self):
        self.id     = -1
        self.device = None
        self.header = None

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_LOGICAL_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## Route Schedule
class RouteSchedule(object):

    def __init__(self):
        self.index      = -1
        self.period     = 0
        self.duration   = 0
        self.routes     = []
        self.devices    = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.index == other.index

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_PHYSICAL_STR.format(self.index)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## MIL List
class MILList(object):

    def __init__(self):
        self.id     = -1
        self.majorframe = 0
        self.slot       = []

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_LIST_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## MIL Slot
class MILSlot(object):

    def __init__(self):
        self.id     = -1
        self.bus   = 'A'
        self.type  = ''
        self.addr  = 0
        self.subaddr  = 0
        self.addrtx   = 0
        self.subaddrtx  = 0
        self.wcmode     = 0
        self.time     = 0

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_SLOT_STR.format(self.id, self.type, self.addr, self.subaddr)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## Ethernet Header
class EthHeader(object):

    def __init__(self):
        self.ip          = []
        self.mac         = []
        self.port        = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.ip == other.ip and self.mac == other.mac and self.port == other.port

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ETH_HEADER_STR.format(':'.join(self.mac), '.'.join(self.ip), self.port)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## SpaceWire Header
class SpwHeader(object):

    def __init__(self):
        self.address     = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.address == other.address

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SPW_HEADER_STR.format(self.address)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## CANBUS Header
class CanHeader(object):

    def __init__(self):
        self.extended = False
        self.sshot = False
        self.rtr = False
        self.can_id = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.can_id == other.can_id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return CAN_HEADER_STR.format(self.extended, \
                self.sshot, self.rtr, self.can_id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## MIL-STD-1553 Header
class MILHeader(object):

    def __init__(self):
        self.desc = 0
        self.address = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.desc == other.desc and self.address == other.address

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_HEADER_STR.format(self.desc, \
                self.address)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## UARTBUS Header
class UartHeader(object):

    def __init__(self):
        self.uart_id = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.uart_id == other.uart_id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return UART_HEADER_STR.format(self.uart_id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## SD Header
class SdHeader(object):

    def __init__(self):
        self.size = 0
        self.file = ''

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.file == other.file

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SD_HEADER_STR.format(self.size, self.file)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()


## ADC Header
class AdcHeader(object):

    def __init__(self):
        self.id = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ADC_HEADER_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## GPIO Header
class GPIOHeader(object):

    def __init__(self):
        self.id = 0
        self.write = False

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return GPIO_HEADER_STR.format(self.pin, self.write)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()
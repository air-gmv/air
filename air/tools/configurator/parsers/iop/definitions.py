
SPW = 'SPW'
MIL = 'MIL'
ETH = 'ETH'
RTR = 'SPWRTR'

SUPPORTED_DEVICES 	=   {	SPW : ['GRSPW'],
                        	MIL : ['GR1553B', 'BRM1553'],
                        	ETH : ['GRETH'],
                            RTR : ['SPWRTR']   }

IOP_PORT_TYPE		= { 'SamplingPort' : 'IOP_SAMPLING_PORT',
						'QueuingPort'  : 'IOP_QUEUING_PORT'   }



LOGICAL_DEVICE					= 'LogicalDevices/Device'
LOGICAL_DEVICE_ID				= 'Id'
LOGICAL_DEVICE_NAME				= 'Name'


REMOTE_PORT                     = 'RemotePorts/Port'
REMOTE_PORT_NAME                = 'Name'
REMOTE_PORT_LOGICAL_DEVICE_ID   = 'LogicalDeviceId'


PHYSICAL_DEVICE                 = 'PhysicalDevices/Device'
PHYSICAL_DEVICE_ID				= 'Id'
PHYSICAL_DEVICE_NAME			= 'Device'
PHYSICAL_DEVICE_PATTERN         = '(?P<type>[a-zA-Z]{1,})(?P<minor>[0-9]{1})'

ROUTE_PHYSICAL                  = 'PhysicalRoute'
ROUTE_LOGICAL                   = 'LogicalRoute'
ROUTE_ID                        = 'Id'

ROUTE_PHYSICAL_PORT             = 'PortId'
ROUTE_LOGICAL_DEVICE            = 'LogicalDeviceId'

# Top Level Nodes
IOPARTITION						= 'IOPartition'


SCHEDULING                      = 'ModuleSchedules/Schedule'
SCHEDULING_ROUTE                = 'RoutesConfiguration/Route'
SCHEDULING_ROUTE_ID             = 'RouteId'
SCHEDULING_ROUTE_ACTIVE         = 'Active'
SCHEDULING_DEVICE               = 'DevicesConfiguration/Device'
SCHEDULING_DEVICE_ID            = 'DeviceId'

#Top level attributes
DATE_ATTR						= 'date'
NAME_ATTR						= 'name'
REQUEST_NUMBER_ATTR				= 'request_number'
TIME_TO_LIVE_ATTR				= 'time_to_live'
VERSION_ATTR					= 'version'

# ETH Header
ETHHEADER						= 'EthHeader'
ETHHEADER_IP					= 'Ip'
ETHHEADER_MAC					= 'MAC'
ETHHEADER_PORT					= 'Port'

#SPW Header
SPWHEADER                       = 'SpwHeader'
SPWHEADER_ADDRESS               = 'Address'


import utils.parser as parserutils

VALID_STR				= [ str, lambda  x : len(x) ]
VALID_NAME_TYPE			= [ str, lambda x : 0 < len(x) < 31 ]
VALID_DEVICE_ID_TYPE	= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_PORT			    = [ parserutils.str2int, lambda x : x > 0]
VALID_IP			    = [ lambda x: str(x).strip().split('.'), lambda x: len(x) == 4 ]
VALID_MAC			    = [ lambda x: str(x).strip().split(':'), lambda x: len(x) == 6 ]
VALID_SPW_ADDRESS       = [ lambda x: str(x).strip().split(','), lambda x: len(x) <= 32 ]
VALID_FLOAT_TYPE		= [ parserutils.str2float, lambda x : x >= 0 ]
VALID_DECIMAL_TYPE		= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_BOOLEAN_TYPE		= [ parserutils.str2bool, lambda x : isinstance(x, bool) ]
VALID_DIRECTION_TYPE 	= [ str, lambda x : x in [ REMOTE_PORT_SRC, REMOTE_PORT_DST] ]


LOGICAL_DEVICE_STR      = 'Logical Device (Id: {0}, Name: {1})'
PHYSICAL_DEVICE_STR     = 'Physical Device (Id: {0}, Device: {1})'
ETH_HEADER_STR          = 'Ethernet Header (Mac: {0}, Ip: {1}, Port: {2})'
SPW_HEADER_STR          = 'SpaceWire Header (Address: {0})'
REMOTE_PORT_STR         = 'Remote Port (Name: {0})'
ROUTE_STR               = 'Route (Id: {0})'
ROUTE_PHYSICAL_STR      = 'Physical Route (Id: {0})'
ROUTE_LOGICAL_STR       = 'Logical Route (Id: {0})'

## Logical Device
class LogicalDevice(object):

    def __init__(self):
        self.idx            = 0
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
        self.idx            = 0
        self.id             = 0
        self.device         = ''
        self.type           = ''
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
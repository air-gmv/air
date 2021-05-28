## @package parsers.iop.xilinx_devices
#  @author cpse
#  @brief Functions to parse xilinx devices

__author__ = 'trcpse'
__modified__ = 'ttao'


from definitions import *
from utils.parser import str2int
from localization.logger import *



XETH_IP                = 'Ip'
XETH_MAC               = 'MAC'
XETH_TXD               = 'TXD'
XETH_RXD               = 'RXD'
XETH_READS             = 'Reads'

UART_CORE               = 'UartCore'
UART_BAUD               = 'Baud'
UART_PARITY             = 'Parity'
UART_DATABITS           = 'DataBits'
UART_STOPBITS           = 'StopBits'
UART_READS              = 'Reads'
UART_DATABYTES          = 'DataBytes'

CANBUS_BAUD             = 'Baud'

XSD_FORMAT              = 'Format'
XSD_NUMBER_FILES        = 'Number_Files'
XSD_READS               = 'Reads'

XADC_READS              = 'Reads'



VALID_XD                = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_READS             = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_UART_BAUD         = [ parserutils.str2int, lambda x : 0 < x <= 921600 ]
VALID_CAN_BAUD          = [ parserutils.str2int, lambda x : 0 < x <= 1000 ]
VALID_DATABITS          = [ parserutils.str2int, lambda x : 5 < x <= 8 ]
VALID_STOPBITS          = [ parserutils.str2int, lambda x : 0 < x <= 2 ]
VALID_PARITY            = [ str, lambda x : x in ['E', 'O', 'S', 'M', 'N'] ]
VALID_DATABYTES         = [ parserutils.str2int, lambda x : 0 < x <= 64 ]
VALID_FORMAT            = [ parserutils.str2int, lambda x : 0 <= x <= 1 ]
VALID_NUMBER_FILES      = [ parserutils.str2int, lambda x : 0 < x <= 40 ]


# XETH physical device setup
class XETHPhySetup(object):

    def __init__(self):
        self.id         = 0
        self.ip         = ''            # ip address
        self.mac        = ''            # mac address
        self.txd_count  = 0             # number of tx descriptors
        self.rxd_count  = 0             # number of rx descriptors


    def details(self):
        return 'XETH Physical Device Setup (Mac: {0} Ip: {1} TXD - {2} RXD - {3})'\
        .format(':'.join(self.mac), '.'.join(self.ip), self.txd_count, self.rxd_count)

# XETH Schedule device setup
class XETHSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            	# number of reads per period

    def details(self):
        return 'XETH Schedule Setup (Reads - {0})'.format(self.reads)



# UART physical device setup
class UARTPhySetup(object):

    def __init__(self):
        self.uart_core = 0
        self.baud = 0
        self.txd_count  = 0             # number of tx descriptors
        self.rxd_count  = 0             # number of rx descriptors


    def details(self):
        return 'UART Physical Device Setup (Core: {0} Baud: {0} TXD: {1} RXD: {2})'\
        .format( self.uart_core, self.baud, self.txd_count, self.rxd_count )

# UART Schedule device setup
class UARTSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period

    def details(self):
        return 'UART Schedule Setup (Reads - {0})'.format(self.reads)



# CAN physical device setup
class CANPhySetup:

    def __init__(self):
        self.can_core = 0
        self.baud = 1000

    def details(self):
        return 'CAN Physical Device Setup with Baud: {0}'.format(self.baud)

# CAN Schedule device setup
class CANSchSetup:

    def __init__(self):
        self.device = None
        self.reads = 1

    def details(self):
        return 'CAN Schedule Setup with {0} reads per period'.format(self.reads)


# XSD physical device setup
class XSDPhySetup(object):

    def __init__(self):
        self.id = 0
        self.format = 0
        self.number_files = 0

    def details(self):
        return 'XSD Physical Device Setup (Id: {0}, Format: {1}, Number_Files: {2})'\
        .format(self.id, self.format, self.number_files)

# XSD Schedule device setup
class XSDSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            	# number of reads per period

    def details(self):
        return 'XSD Schedule Setup (Reads - {0})'.format(self.reads)


# ADC physical device setup
class XADCPhySetup(object):

    def __init__(self):
        self.id = 0

    def details(self):
        return 'XADC Physical Device Setup (AdcId: {0})'.format(self.id)

# ADC Schedule device setup
class XADCSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            	# number of reads per period

    def details(self):
        return 'XADC Schedule Setup (Reads - {0})'.format(self.reads)



## XETH physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_xeth(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup           = XETHPhySetup()
    setup.id        = pdevice.minor
    setup.ip        = xml.parse_attr(XETH_IP, VALID_IP, True, iop_parser.logger)
    setup.mac       = xml.parse_attr(XETH_MAC, VALID_MAC, True, iop_parser.logger)
    setup.txd_count = xml.parse_attr(XETH_TXD, VALID_XD, True, iop_parser.logger)
    setup.rxd_count = xml.parse_attr(XETH_RXD, VALID_XD, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## XETH schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_xeth(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = XETHSchSetup()
    setup.reads = xml.parse_attr(XETH_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup



## UART physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_xuart(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup                   = UARTPhySetup()
    setup.uart_core         = pdevice.minor
    setup.baud              = xml.parse_attr(UART_BAUD, VALID_UART_BAUD, True, iop_parser.logger)
    setup.parity            = xml.parse_attr(UART_PARITY, VALID_PARITY, True, iop_parser.logger)
    setup.stop_bits         = xml.parse_attr(UART_STOPBITS, VALID_STOPBITS, True, iop_parser.logger)
    setup.data_bits         = xml.parse_attr(UART_DATABITS, VALID_DATABITS, True, iop_parser.logger)
    setup.data_bytes        = xml.parse_attr(UART_DATABYTES, VALID_DATABYTES, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## UART schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_xuart(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = UARTSchSetup()
    setup.reads = xml.parse_attr(UART_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup



## CAN physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_xcan(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup                   = CANPhySetup()
    setup.uart_core         = pdevice.minor
    setup.baud              = xml.parse_attr(CANBUS_BAUD, VALID_CAN_BAUD, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## CAN schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_xcan(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = CANSchSetup()
    setup.reads = xml.parse_attr(UART_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup



## XSD physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_xsd(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup           	= XSDPhySetup()
    setup.id        	= pdevice.minor
    setup.format    	= xml.parse_attr(XSD_FORMAT, VALID_FORMAT, True, iop_parser.logger)
    setup.number_files   = xml.parse_attr(XSD_NUMBER_FILES, VALID_NUMBER_FILES, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## XSD schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_xsd(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = XSDSchSetup()
    setup.reads = xml.parse_attr(XSD_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup



## XADC physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_xadc(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup           	= XADCPhySetup()
    setup.id        	= pdevice.minor

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## XADC schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_xadc(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = XADCSchSetup()
    setup.reads = xml.parse_attr(XADC_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup

## @package parsers.iop.ti_devices
#  @author cpse
#  @brief Functions to parse xilinx devices

__author__ = 'trcpse'

from definitions import *
from utils.parser import str2int
from localization.logger import *

UART_CORE               = 'UartCore'
UART_BAUD               = 'Baud'
UART_PARITY             = 'Parity'
UART_DATABITS           = 'DataBits'
UART_STOPBITS           = 'StopBits'
UART_READS              = 'Reads'
UART_DATABYTES          = 'DataBytes'

VALID_XD                = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_READS             = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_BAUD              = [ parserutils.str2int, lambda x : 0 < x <= 921600 ]
VALID_DATABITS          = [ parserutils.str2int, lambda x : 5 < x <= 8 ]
VALID_STOPBITS          = [ parserutils.str2int, lambda x : 0 < x <= 2 ]
VALID_PARITY            = [ str, lambda x : x in ['E', 'O', 'S', 'M', 'N'] ]
VALID_DATABYTES         = [ parserutils.str2int, lambda x : 0 < x <= 64 ]


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
    setup.baud              = xml.parse_attr(UART_BAUD, VALID_BAUD, True, iop_parser.logger)
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

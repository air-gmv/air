## @package parsers.iop.gaisler_devices
#  @author pfnf
#  @brief Functions to parse gaisler devices

__author__ = 'pfnf'

from definitions import *
from utils.parser import str2int
from localization.logger import *

GRETH_IP                = 'Ip'
GRETH_MAC               = 'MAC'
GRETH_TXD               = 'TXD'
GRETH_RXD               = 'RXD'
GRETH_READS             = 'Reads'

VALID_XD	            = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_READS	            = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]

# GRETH physical device setup
class GRETHPhySetup(object):

    def __init__(self):
        self.ip         = ''            # ip address
        self.mac        = ''            # mac address
        self.txd_count  = 0             # number of tx descriptors
        self.rxd_count  = 0             # number of rx descriptors


    def details(self):
        return 'GRETH Physical Device Setup (Mac: {0} Ip: {1} TXD - {2} RXD - {3})'\
            .format(':'.join(self.mac), '.'.join(self.ip), self.txd_count, self.rxd_count)

# GRETH Schedule device setup
class GRETHSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period

    def details(self):
        return 'GRETH Schedule Setup (Reads - {0})'.format(self.reads)

## Greth physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_greth(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup           = GRETHPhySetup()
    setup.ip        = xml.parse_attr(GRETH_IP, VALID_IP, True, iop_parser.logger)
    setup.mac       = xml.parse_attr(GRETH_MAC, VALID_MAC, True, iop_parser.logger)
    setup.txd_count = xml.parse_attr(GRETH_TXD, VALID_XD, True, iop_parser.logger)
    setup.rxd_count = xml.parse_attr(GRETH_RXD, VALID_XD, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## Greth schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_greth(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = GRETHSchSetup()
    setup.reads = xml.parse_attr(GRETH_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup

## @package parsers.iop.ti_devices
#  @author pfnf
#  @modified by gmvs
#  @brief Functions to parse ti devices

__author__ = 'pfnf'
__modifier__ = 'gmvs'

from parsers.iop.definitions import *
from utils.parser import str2int
from localization.logger import *

CPSW_IP                = 'Ip'
CPSW_MAC               = 'MAC'
CPSW_TXD               = 'TXD'
CPSW_RXD               = 'RXD'
CPSW_READS             = 'Reads'

VALID_XD	            = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]
VALID_READS	            = [ parserutils.str2int, lambda x : 0 < x <= 2048 ]

# CPSW physical device setup
class CPSWPhySetup(object):

    def __init__(self):
        self.ip         = ''            # ip address
        self.mac        = ''            # mac address
        self.txd_count  = 0             # number of tx descriptors
        self.rxd_count  = 0             # number of rx descriptors


    def details(self):
        return 'CPSW Physical Device Setup (Mac: {0} Ip: {1} TXD - {2} RXD - {3})'\
            .format(':'.join(self.mac), '.'.join(self.ip), self.txd_count, self.rxd_count)

# CPSW Schedule device setup
class CPSWSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period

    def details(self):
        return 'CPSW Schedule Setup (Reads - {0})'.format(self.reads)

## CPSW physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_cpsw(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup           = CPSWPhySetup()
    setup.ip        = xml.parse_attr(CPSW_IP, VALID_IP, True, iop_parser.logger)
    setup.mac       = xml.parse_attr(CPSW_MAC, VALID_MAC, True, iop_parser.logger)
    setup.txd_count = xml.parse_attr(CPSW_TXD, VALID_XD, True, iop_parser.logger)
    setup.rxd_count = xml.parse_attr(CPSW_RXD, VALID_XD, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## CPSW schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_cpsw(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = CPSWSchSetup()
    setup.reads = xml.parse_attr(CPSW_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup

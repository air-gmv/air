## @package parsers.iop.gaisler_devices
#  @author pfnf 
# modified by lumm and gmvs
#  @brief Functions to parse gaisler devices
from sqlalchemy.sql.expression import false

__author__ = 'pfnf'
__modified__ = 'lumm'
__modified__ ='gmvs'

from definitions import *
from utils.parser import str2int
from localization.logger import *

GRETH_IP                = 'Ip'
GRETH_MAC               = 'MAC'
GRETH_TXD               = 'TXD'
GRETH_RXD               = 'RXD'
GRETH_READS             = 'Reads'

GRSPW_ADDR				= 'Addr'
GRSPW_MASK				= 'Mask'
GRSPW_DEST				= 'Dest'
GRSPW_CLK				= 'Clk'
GRSPW_RXMAX				= 'RXmax'
GRSPW_PRO				= 'PRO'
GRSPW_RMAP				= 'RMAP'
GRSPW_RMAPBUF			= 'RMAPbuf'
GRSPW_RMPROTID			= 'ProtID'
GRSPW_TXDSIZE			= 'TXDsize'
GRSPW_TXHSIZE			= 'TXHsize'
GRSPW_RXSIZE			= 'RXsize'
GRSPW_TXD				= 'TXD'
GRSPW_RXD				= 'RXD'
GRSPW_RETRY				= 'Retry'
GRSPW_WAIT				= 'Wait'
GRSPW_TIMEOUT			= 'Timeout'
GRSPW_READS             = 'Reads'

SPWRTR_FLAGS            = 'Flags'
SPWRTR_CONFIG           = 'Config'
SPWRTR_IID              = 'Idd'
SPWRTR_IDIV             = 'Idiv'
SPWRTR_PRESCALER        = 'Prescaler'

CANBUS_ID               = 'CanID'
CANBUS_SPEED            = 'Speed'
CANBUS_INTERNAL_QUEUE   = 'InternalQueue'
CANBUS_TXD              = 'TXD'
CANBUS_RXD              = 'RXD'
CANBUS_READS            = 'Reads'
CANBUS_SINGLE_MODE      = 'SingleMode'
CANBUS_CODE             = 'Code'
CANBUS_MASK             = 'Mask'


VALID_EN                    = [ parserutils.str2int, lambda x : 0 <= x <= 1 ]
VALID_XD	                = [ parserutils.str2int, lambda x : 0 <= x <= 2048 ]
VALID_READS	                = [ parserutils.str2int, lambda x : 0 <= x <= 2048 ]
VALID_TIMER                 = [ parserutils.str2int, lambda x : -1 <= x <= 50 ]
VALID_MASK                  = [ parserutils.str2int, lambda x : len(str(x)) <= 32 ]
VALID_BOOL                  = [ parserutils.str2bool, lambda x: type(x) is bool ]

# Insert definition for CANBUS




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
		
# GRSPW physical device setup
class GRSPWPhySetup(object):

    def __init__(self):
        self.nodeaddr       = 0
        self.nodemask       = 0
        self.destkey        = 0
        self.clkdiv         = 1
        self.rxmaxlen       = 0
        self.promiscuous    = 1
        self.rmapen         = 0
        self.rmapbufdis     = 0
        self.rm_prot_id     = 1
        self.txdbufsize     = 0
        self.txhbufsize     = 0
        self.rxbufsize      = 0
        self.txdesccnt      = 0
        self.rxdesccnt      = 0
        self.retry          = 0
        self.wait_ticks     = 0
        self.init_timeout   = 5


    def details(self):
        return 'GRSPW Physical Device Setup (Node Address: {0} Node Mask: {1} Destiny Key - {2} Promiscuous - {3})'\
            .format(self.nodeaddr, self.nodemask, self.destkey, self.promiscuous)

# GRSPW Schedule device setup
class GRSPWSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period

    def details(self):
        return 'GRSPW Schedule Setup (Reads - {0})'.format(self.reads)

# SPWRTR physical device setup
class SPWRTRPhySetup(object):

    def __init__(self):
        self.flags  = 0
        self.config = 0
        self.iid    = 0
        self.idiv   = 0
        self.prescaler = 1

    def details(self):
        return 'SPWRTR Physical Device Setup (Flags: {0} Config: {1} IID: {2} IDIV: {3} PRESCALER: {4})'\
            .format(bin(self.flags), bin(self.config), self.iid, self.idiv, self.prescaler)

# SPWRTR Schedule device setup
class SPWRTRSchSetup(object):

    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period

    def details(self):
        return 'SPWRTR Schedule Setup (Reads - {0})'.format(self.reads)
    
    
# CANBUS physical device
class OCCANPhySetup(object):
    
    def __init__(self):
        self.can_id = 0
        self.speed = 0
        self.txd = 0
        self.rxd = 0
        self.internal_queue = 0
        self.single_mode = False
        self.code = ''
        self.mask = ''
        
    def details(self):
        return 'CAN Physical Device Setup (CanID: {0} Speed: {1} TXD: {2} RXD: {3} InternalQueue: {4} SingleMode: {5} Code: {6} Mask: {7})'\
        .format(bin(self.can_id), bin(self.speed), bin(self.txd), bin(self.rxd), bin(self.internal_queue), bin(int(self.single_mode)), bin(self.code), bin(self.masks))
        
# CANBUS Schedule device setup
class OCCANSchSetup(object):
    
    def __init__(self):
        self.device = None
        self.reads  = ''            # number of reads per period
        
    def details(self):
        return 'CANBUS Schedule Setup (Reads - {0}'.format(self.reads)


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

## Grspw physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_grspw(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup               = GRSPWPhySetup()
    setup.nodeaddr      = xml.parse_attr(GRSPW_ADDR, VALID_SPW_ADDRESS, True, iop_parser.logger)
    setup.nodemask      = xml.parse_attr(GRSPW_MASK, VALID_MASK, True, iop_parser.logger)
    setup.destkey       = xml.parse_attr(GRSPW_DEST, VALID_XD, True, iop_parser.logger)
    setup.clkdiv        = xml.parse_attr(GRSPW_CLK, VALID_XD, True, iop_parser.logger)
    setup.rxmaxlen      = xml.parse_attr(GRSPW_RXMAX, VALID_XD, True, iop_parser.logger)
    setup.promiscuous   = xml.parse_attr(GRSPW_PRO, VALID_EN, True, iop_parser.logger)
    setup.rmapen        = xml.parse_attr(GRSPW_RMAP, VALID_EN, True, iop_parser.logger)
    setup.rmapbufdis    = xml.parse_attr(GRSPW_RMAPBUF, VALID_XD, True, iop_parser.logger)
    setup.rm_prot_id    = xml.parse_attr(GRSPW_RMPROTID, VALID_EN, True, iop_parser.logger)
    setup.txdbufsize    = xml.parse_attr(GRSPW_TXDSIZE, VALID_XD, True, iop_parser.logger)
    setup.txhbufsize    = xml.parse_attr(GRSPW_TXHSIZE, VALID_XD, True, iop_parser.logger)
    setup.rxbufsize     = xml.parse_attr(GRSPW_RXSIZE, VALID_XD, True, iop_parser.logger)
    setup.txdesccnt     = xml.parse_attr(GRSPW_TXD, VALID_XD, True, iop_parser.logger)
    setup.rxdesccnt     = xml.parse_attr(GRSPW_RXD, VALID_XD, True, iop_parser.logger)
    setup.retry         = xml.parse_attr(GRSPW_RETRY, VALID_TIMER, True, iop_parser.logger)
    setup.wait_ticks    = xml.parse_attr(GRSPW_WAIT, VALID_TIMER, True, iop_parser.logger)
    setup.init_timeout  = xml.parse_attr(GRSPW_TIMEOUT, VALID_TIMER, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## Grspw schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_grspw(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = GRSPWSchSetup()
    setup.reads = xml.parse_attr(GRSPW_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup
    
## SPWRTR physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_spwrtr(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup               = SPWRTRPhySetup()
    setup.flags         = xml.parse_attr(SPWRTR_FLAGS, VALID_MASK, True, iop_parser.logger)
    setup.config        = xml.parse_attr(SPWRTR_CONFIG, VALID_MASK, True, iop_parser.logger)
    setup.iid           = xml.parse_attr(SPWRTR_IID, VALID_XD, True, iop_parser.logger)
    setup.idiv          = xml.parse_attr(SPWRTR_IDIV, VALID_XD, True, iop_parser.logger)
    setup.prescaler     = xml.parse_attr(SPWRTR_PRESCALER, VALID_XD, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevice.setup = setup
    return True

## SPWRTR schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_spwrtr(iop_parser, xml, pdevice):

    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)

    # parse setup
    setup       = SPWRTRSchSetup()
    setup.reads = xml.parse_attr(GRSPW_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup


## CANBUS physical device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def phy_occan(iop_parser, xml, pdevices):
    
    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)
    
    # parse setup
    setup                   = OCCANPhySetup()
    setup.speed             = xml.parse_attr(CANBUS_SPEED, VALID_READS, True, iop_parser.logger)
    setup.txd_count         = xml.parse_attr(CANBUS_TXD, VALID_XD, True, iop_parser.logger)
    setup.rxd_count         = xml.parse_attr(CANBUS_RXD, VALID_XD, True, iop_parser.logger)
    setup.internal_queue    = xml.parse_attr(CANBUS_INTERNAL_QUEUE, VALID_READS, True, iop_parser.logger)
    setup.single_mode       = xml.parse_attr(CANBUS_SINGLE_MODE, VALID_BOOL, True, iop_parser.logger)
    setup.code              = xml.parse_attr(CANBUS_CODE, VALID_MASK, True, iop_parser.logger)
    setup.mask              = xml.parse_attr(CANBUS_MASK, VALID_MASK, True, iop_parser.logger)
    
    # sanity check
    if iop_parser.logger.check_errors(): return False
    pdevices.setup = setup
    return True

## CANBUS schedule device setup
# @param iop_parser IOP parser object
# @param xml XML setup node
# @param pdevice current physical device
def sch_occan(iop_parser, xml, pdevice):
    
    # clear previous errors and warnings
    iop_parser.logger.clear_errors(0)
    
    # parse setup
    setup       = CANBUSchSetup()
    setup.reads = xml.parse_attr(CANBUS_READS, VALID_READS, True, iop_parser.logger)

    # sanity check
    if iop_parser.logger.check_errors(): return None

    # parse complete
    setup.device = pdevice
    return setup
    
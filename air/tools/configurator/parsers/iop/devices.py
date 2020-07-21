


from definitions import *
import gaisler_devices as gaisler
import ti_devices as ti
import xilinx_devices as xilinx

## List of devices supported by the IOP configurator
iop_supported_devices = {

    # device        #type   # physical dev setup    # scheduler dev setup   # template file
    'GRETH'     : [ ETH,    gaisler.phy_greth,      gaisler.sch_greth,      'iop_greth.mako' ],
    'CPSW'      : [ ETH,    ti.phy_cpsw,            ti.sch_cpsw,            'iop_cpsw.mako'  ],
    'GRSPW'     : [ SPW,    gaisler.phy_grspw,      gaisler.sch_grspw,      'iop_grspw.mako'],
    'SPWRTR'    : [ RTR,    gaisler.phy_spwrtr,     gaisler.sch_spwrtr,     'iop_spw_router.mako'],
    'GRCAN'     : [ CAN,    gaisler.phy_grcan,      gaisler.sch_grcan,      'iop_grcan.mako'],
    'GRMIL'     : [ MIL,    gaisler.phy_gr1553b,    gaisler.sch_gr1553b,    'iop_gr1553b.mako'],
    'XUART'     : [ UART,   xilinx.phy_xuart,       xilinx.sch_xuart,       'iop_xuart.mako'],
    'XCAN'      : [ CAN,    xilinx.phy_xcan,        xilinx.sch_xcan,        'iop_xcan.mako'],
}

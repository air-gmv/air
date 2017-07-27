


from definitions import *
import gaisler_devices as gaisler
import ti_devices as ti

## List of devices supported by the IOP configurator
iop_supported_devices = {

    # device        #type   # physical dev setup    # scheduler dev setup   # template file
    'GRETH'     : [ ETH,    gaisler.phy_greth,      gaisler.sch_greth,      'iop_greth.mako' ],
    'CPSW'      : [ ETH,    ti.phy_cpsw,            ti.sch_cpsw,            'iop_cpsw.mako'  ],
    'GRSPW'     : [ ETH,    gaisler.phy_grspw,      gaisler.sch_grspw,      'iop_grspw.mako'],
    'SPWRTR'    : [ ETH,    gaisler.phy_spwrtr,     gaisler.sch_spwrtr,     'iop_spw_router.mako'],

}

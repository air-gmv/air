


from definitions import *
import gaisler_devices as gaisler
import ti_devices as ti

## List of devices supported by the IOP configurator
iop_supported_devices = {

    # device        #type   # physical dev setup    # scheduler dev setup   # template file
    'GRETH'     : [ ETH,    gaisler.phy_greth,      gaisler.sch_greth,      'iop_greth.mako' ],
    'CPSW'      : [ ETH,    ti.phy_cpsw,            ti.sch_cpsw,            'iop_cpsw.mako'  ],

}

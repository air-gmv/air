#include <iop.h>
#include <pprintf.h>
#include <taste_api.h>

#include <imaspex.h>

void IOPextern()
{

    unsigned char msg[1024] = "empty\0";
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE port_len;

    unsigned char reply[1024];
    VALIDITY_TYPE val;
    SYSTEM_TIME_TYPE PERIOD = 1000000000ll;

    QUEUING_PORT_ID_TYPE qpid;
    SAMPLING_PORT_ID_TYPE spid;

    unsigned int dev_len;
    int route_id = -1;
    air_status_code_e air_rc;

    /*Create ports*/
    CREATE_QUEUING_PORT("squeuing", 1024, 32, SOURCE, FIFO, &qpid, &rc);
    CREATE_SAMPLING_PORT("dsampling", 1024, DESTINATION, PERIOD, &spid, &rc);

    /*Initialize support features for IOP*/
    init_iopsupport();
    /*Open all devices in IOP config*/
    open_alldevices();

    while (1)
    {

        READ_SAMPLING_MESSAGE(spid, msg, &port_len, &val, &rc);

        /*Write message to first logical device, first logical route*/
        if (rc == NO_ERROR && port_len > 0 && val == VALID)
        {
            if (write_to_device(0, 0, (char *)msg, (unsigned int)port_len) != AIR_SUCCESSFUL)
            {
                pprintf("IOP_TASTE :: write to device failed\n");
            }
        }
        else
        {
            if (rc != NO_ACTION || (rc == NO_ACTION && val == VALID) || (port_len == 0 && val == VALID))
            {
                pprintf("IOP_TASTE :: read port invalid rc=%d len=%d val=%d\n", rc, port_len, val);
            }
        }

        /*Read from first physical device*/
        air_rc = read_from_device(0, &route_id, (char *)reply, &dev_len);

        if (air_rc == AIR_NO_ERROR && dev_len > 0)
        {
            if (route_id == 0)
            {
                SEND_QUEUING_MESSAGE(qpid, reply, (MESSAGE_SIZE_TYPE)dev_len, INFINITE_TIME_VALUE, &rc);
            }

            else
            {
                pprintf("IOP_TASTE :: wrong route route_id=%d\n", route_id);
            }
        }
        else
        {
            if (air_rc != AIR_NO_ACTION)
            {
                pprintf("IOP_TASTE :: read from device failed rc=%d len=%d\n", rc, dev_len);
            }
        }
    }

    return;
}

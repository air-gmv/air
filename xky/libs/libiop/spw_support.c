#include <iop.h>
#include <string.h>
#include <iop_support.h>
#include <spw_support.h>

void spw_prebuild_header(spw_header_t *buf) {

    /* the total length will be inserted when we have the data */
    buf->hlen = 0;
}

void spw_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header) {

    /* get underlying ETH device */
    iop_spw_device_t *spw_dev = (iop_spw_device_t *)iop_dev->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(spw_header_t);
    iop_buf->header_size = sizeof(spw_header_t);

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, iop_buf->header_size);

    /* complete header with the device parameters. TODO depending on how this is used
	 *, the removing bytes might be done here
	 */
    //eth_complete_header();
}

uint32_t spw_compare_header(iop_wrapper_t *wrapper, iop_header_t *header) {

    spw_header_t *src = (spw_header_t *)header;
    spw_header_t *rcv = (spw_header_t *)get_header(wrapper->buffer);

    if (rcv->hdr != src->hdr) {
        return 0;
    }

    return 1;
}

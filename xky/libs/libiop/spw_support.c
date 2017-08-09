#include <iop.h>
#include <string.h>
#include <iop_support.h>
#include <spw_support.h>

void spw_prebuild_header(spw_header_t *buf) {

    /* the total length will be inserted when we have the data */
    //buf->hlen = 0;
}

//void spw_complete_header(iop_buffer_t *buf) {
	
	

void spw_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header) {

    /* get underlying SPW device */
    iop_spw_device_t *spw_dev = (iop_spw_device_t *)iop_dev->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;
	
	uint32_t header_size = 1;
	spw_header_t *spw_header = (spw_header_t *)header;
	int i = 1;
	for (i; i < sizeof(spw_header_t); i++){
		if((*spw_header).hdr[i] != 0) {
			header_size++;
		} else break;
	}
	
    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - header_size;

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, header_size);

    /* complete header with the device parameters. TODO depending on how this is used
	 *, the removing bytes might be done here
	 */
    //spw_complete_header();
}

uint32_t spw_compare_header(iop_wrapper_t *wrapper, iop_header_t *header) {

    spw_header_t *src = (spw_header_t *)header;
    spw_header_t *rcv = (spw_header_t *)get_header(wrapper->buffer);

    if (rcv->hdr != src->hdr) {
        return 0;
    }

    return 1;
}

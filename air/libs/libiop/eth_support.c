/** 
 * @file
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * @author cdcs
 *
 * @defgroup UDP:IP
 */

#include <iop.h>
#include <bsp.h>
#include <iop_support.h>
#include <eth_support.h>
#include <iop_error.h>


/**
 * @brief Computes the packet checksum
 * @param sum pre existing sum
 * @param data data to be checksum'ed
 * @param len length of the data field
 * @return checksum of the data plus previous sum in host order
 */
static uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len){

    uint16_t t;
    const uint8_t *dataptr;
    const uint8_t *last_byte;

    dataptr = data;
    last_byte = data + len - 1;

    while(dataptr < last_byte) {    /* At least two more bytes */
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
        dataptr += 2;
    }

    if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
    }

    /* Return sum in host byte order. */
    return sum;
}

uint16_t eth_ipv4_chksum(uint8_t *buf) {

  uint16_t sum = chksum(0, &buf[ETHII_HDR_SIZE], IPV4_HDR_SIZE);
  return (sum == 0) ? 0xFFFF : HTONS(sum);
}

/**
 * @brief Complete IP header
 * @param src_mac source MAC address
 * @param src_ip source IP address
 * @param data packet payload
 * @param len payload length
 */
static void eth_complete_header(
        void *buf, uint8_t *src_mac, uint8_t *src_ip,
        uint8_t *data, uint16_t len) {

    eth_header_t *eth = (eth_header_t *)buf;
    /* setup source MAC and IPs */
    memcpy(eth->src_mac, src_mac, 6);
    memcpy(eth->src_ip, src_ip, 4);

    /* fill the IPv4 length */
    eth->len = HTONS(len + IPV4_HDR_SIZE + UDP_HDR_SIZE);

    /* force checksums to be zero*/
    eth->ipchksum = 0;

    /* compute IP header checksum */
    eth->ipchksum = ~eth_ipv4_chksum((uint8_t *)buf);

    /*TODO add TCP*/
    udp_header_t *udp = (udp_header_t *)(buf+offsetof(eth_header_t, proto_header.specific_header));

    udp->udplen = HTONS(len + UDP_HDR_SIZE);
    udp->udpchksum = 0;
}

void eth_prebuild_header(iop_header_t *buf) {

    eth_header_t * eth = (eth_header_t *) buf;

    /* the total length will be inserted when we have the data */
    eth->len = 0;

    /* time to Live is defined in UIP_TTL (64) */
    eth->ttl = IPV4_HDR_TTL;

    /* protocol is UDP */
    eth->proto = IPV4_HDR_PROTO;

    /* Version and Header Length. Always the same*/
    eth->vhl = 0x45;

    /* Type of Service: always 0*/
    eth->tos = 0;

    /* We don't fragment packets, so this is zero*/
    eth->ipoffset[0] = eth->ipoffset[1] = 0;

    /* We don't fragment packets, so this can have almost any value*/
    eth->ipid[0] = 0;
    eth->ipid[1] = 1 & 0xff;

    /* checksum needs the user data */
    eth->ipchksum = 0;

    /* set type of packet */
    eth->type = HTONS(ETH_HDR_IP_TYPE);
}


void eth_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header) {

    /* get underlying ETH device */
    iop_eth_device_t *eth_dev = (iop_eth_device_t *)iop_dev->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets TODO: TCP support */
    iop_buf->header_off = iop_buf->header_size - sizeof(eth_header_t);
    iop_buf->header_size = sizeof(eth_header_t);
    iop_buf->payload_off = iop_buf->header_size;

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, iop_buf->header_size);

    /* complete header with the device parameters */
    eth_complete_header(
            iop_buf->v_addr, eth_dev->mac, eth_dev->ip,
            get_payload(iop_buf), get_payload_size(iop_buf));
}


/**
 * @brief Setups an ARP reply packet
 * @param arp_pck current ARP request
 *
 * @note This function assumes that the ARP packet is an ARP request
 */
void eth_send_arp_reply(iop_eth_device_t *eth_device, iop_wrapper_t *wrapper) {

    arp_packet_t *arp_pck = (arp_packet_t *)wrapper->buffer->v_addr;

    /* check if this is an ARP request for us */
    if (arp_pck->oper == HTONS(ARP_HDR_REQUEST) &&
        eth_compare_ip((uint16_t *)arp_pck->tpa, (uint16_t *)eth_device->ip)) {

        /* swap the MACs on the ETH header */
        memcpy(arp_pck->eth_hdr.dst_mac, arp_pck->eth_hdr.src_mac, 6);
        memcpy(arp_pck->eth_hdr.src_mac, eth_device->mac, 6);

        /* swap the target and sender MACs */
        memcpy(arp_pck->tha, arp_pck->sha, 6);
        memcpy(arp_pck->sha, eth_device->mac, 6);

        /* swap the target and sender IPs */
        memcpy(arp_pck->tpa, arp_pck->spa, 4);
        memcpy(arp_pck->spa, eth_device->ip, 4);

        /* change the operation to reply */
        arp_pck->oper = HTONS(ARP_HDR_REPLY);

        /*setup dummy fragment so driver send remains generic*/ 
        /*fetch free fragment*/
        iop_fragment_t *frag = obtain_free_fragment();
        if(frag==NULL){
            iop_raise_error(OUT_OF_MEMORY);
            return;
        }

        frag->header_size = 0;
        frag->payload = get_header(wrapper->buffer);
        frag->payload_size = get_buffer_size(wrapper->buffer);

        /*fetch auxiliar wrapper to swap buffers with
         * wrapper - arp wrapper
         * wrapper_send - wrapper to sent, where arp buffer will be*/
        iop_wrapper_t *wrapper_send=obtain_free_wrapper();
        iop_buffer_t *buffer= wrapper_send->buffer;
        wrapper_send->buffer= wrapper->buffer;
        wrapper->buffer=buffer;

        iop_chain_append(&wrapper_send->fragment_queue, &frag->node);
        iop_debug("ARP Reply Send 0x%06x 0x%06x %d\n", wrapper_send, wrapper_send->buffer, wrapper_send->buffer->payload_size ); 
        /* write packet */
        eth_device->dev.write((iop_device_driver_t *)eth_device, wrapper_send);

        /*swap buffers back and release auxiliary wrapper*/
        wrapper->buffer=wrapper_send->buffer;
        wrapper_send->buffer=buffer;
        release_wrapper(wrapper_send);
    }
}

uint32_t eth_compare_header(iop_wrapper_t *wrapper, iop_header_t *header) {

    eth_header_t *src = (eth_header_t *)header;
    eth_header_t *rcv = (eth_header_t *)get_header(wrapper->buffer);

    if (rcv->proto_header.dst_port != src->proto_header.src_port) {
        return 0;
    }

    if (!eth_compare_ip((uint16_t *)rcv->src_ip, (uint16_t *)src->dst_ip)) {
        return 0;
    }

    return 1;
}

static int eth_handle_fragments(iop_wrapper_t *wrapper)
{
    eth_header_t *packet = (eth_header_t *)get_header(wrapper->buffer);
    unsigned int pack_seq = (((unsigned int)packet->ipoffset[0] & 0x1f)<<8) + packet->ipoffset[1];
    static unsigned int frags;
    static unsigned char buf[14 + 65535]; /*14(eth)+20(IP)+8(UDP)+Data*/
    static unsigned int head;

    /*if is a fragment or there were fragments and this packet has a sequence number*/
    if(packet->ipoffset[0] & 0x20 || (frags && pack_seq))
    {
        /*TODO Handle multiple fragmented packets*/
        if(frags*185 == pack_seq)
        {
            if(!(packet->ipoffset[0] & 0x20))
            {  /*packet is complete*/

                memcpy(buf+head, get_payload(wrapper->buffer), get_payload_size(wrapper->buffer));

                memcpy(wrapper->buffer->v_addr, buf, head + get_payload_size(wrapper->buffer));

                /*Subtract from payload eth+IP header. We still need to subtract TCP/UDP header as well*/
                wrapper->buffer->payload_size = head + get_payload_size(wrapper->buffer) - offsetof(eth_header_t, proto_header);

                head = 0;
                frags = 0;
            }
            else
            {   /*still fragmented*/
                if(!frags)
                {   /*Initial fragment*/
                    memcpy(buf, packet, get_header_size(wrapper->buffer) + get_payload_size(wrapper->buffer));
                    head = get_header_size(wrapper->buffer) + get_payload_size(wrapper->buffer);
                }
                else
                {
                    memcpy(buf+head, get_payload(wrapper->buffer), get_payload_size(wrapper->buffer));
                    head += get_payload_size(wrapper->buffer);
                }
                frags++;
                return 0;
            }
        }
        else
        {   /*The sequence nb is not what we're looking for. restart if this packet has sq nb 0*/
            if(!pack_seq)
            {
                memcpy(buf, packet, get_header_size(wrapper->buffer) + get_payload_size(wrapper->buffer));
                head = get_header_size(wrapper->buffer) + get_payload_size(wrapper->buffer);

                frags = 1; /*restart packet sequence*/
            }
            else
                frags = 0; /*Discard this packet*/

            return 0;
        }
    }
    else
    {
        if(pack_seq)
            return 0; /*it's a out of order last fragment*/
        else{
           iop_debug("Valid non fragged packet 0x%06x 0x%06x %d\n", get_header(wrapper->buffer), get_payload(wrapper->buffer), get_payload_size(wrapper->buffer)); 
        }
    }
    return 1;
}


uint32_t eth_validate_packet(
        iop_eth_device_t *dev, iop_wrapper_t *wrapper) {

    /* get the IP header */
    eth_header_t *packet = (eth_header_t *)get_header(wrapper->buffer);

   /* check if the packet was for us */
    if (!eth_compare_mac((uint16_t*)dev->mac, (uint16_t*)packet->dst_mac) ||
        !eth_compare_ip((uint16_t *)dev->ip, (uint16_t*)packet->dst_ip)){
        iop_debug("invalid packet - wrong address\n");

        return 0;
    }

    /* check the if IP version is valid */
    if (packet->vhl != 0x45) {
        iop_debug("invalid packet - ipv\n");
        return 0;
    }

    /* check if the packet is an UDP packet */
    if (packet->proto != IPV4_HDR_PROTO) {
        iop_debug("invalid packet - not udp\n");
        return 0;
    }

    /* get received length */
    uint32_t received_length = get_buffer_size(wrapper->buffer);

    /* check if the packet is length is correct */
    if (HTONS(packet->len) > received_length) {
        iop_debug("invalid packet - wring length %d packet len %d\n", received_length, packet->len);
        return 0;
    }

    /* fix padding */
    received_length = HTONS(packet->len);

    /* check if the checksum is valid */
    if (eth_ipv4_chksum(
            (uint8_t *)get_header(wrapper->buffer)) != (uint16_t)0xFFFF) {
        iop_debug("invalid packet - wrong checksum\n");
        return 0;
    }

    /*Fragmented packet handler*/
    if(!eth_handle_fragments(wrapper)){
        iop_debug("invalid packet - invalid fragment\n");

        return 0;
    }

    /*TODO Adapt to UDP or TCP*/
    /* setup UDP/TCP offsets */
    wrapper->buffer->header_size = sizeof(eth_header_t);
    wrapper->buffer->payload_off = sizeof(eth_header_t);
    wrapper->buffer->payload_size -= sizeof(udp_header_t);

    iop_debug("PCKT %d %d\n", wrapper->buffer->header_size, wrapper->buffer->payload_size);

    /* packet is fine! */
    return 1;
}

uint32_t eth_fragment_packet(iop_wrapper_t *wrapper)
{
    uint16_t size = (uint16_t)get_buffer_size(wrapper->buffer);
    uint8_t *next_payload = get_payload(wrapper->buffer);
    uint16_t total = 0;


    /*fetch free fragment*/
    iop_fragment_t *frag = obtain_free_fragment();
    if(frag==NULL){
        iop_raise_error(OUT_OF_MEMORY);
        return -1;
    }

    /*set first fragment*/
    /*this is independent on the packet needing fragmentation*/
    /*TODO rethink memcpy*/
    memcpy(&frag->header, get_header(wrapper->buffer), get_header_size(wrapper->buffer));
    iop_debug("FCH 0x%06x 0x%06x %d %d %d\n", &frag->header, wrapper->buffer, get_header_size(wrapper->buffer), frag->header.eth_header.dst_ip[0], wrapper->buffer->payload_size);
    frag->header_size = get_header_size(wrapper->buffer);

    total += frag->header_size;

    frag->payload = get_payload(wrapper->buffer);
    frag->payload_size = get_payload_size(wrapper->buffer);

    /*does the packet need to be fragmented?*/
    if(size <= 1514){ /*TODO define this length somewhere*/
        iop_chain_append(&wrapper->fragment_queue, &frag->node);
        iop_debug("no frag\n");
        return 0;
    }

    frag->header.eth_header.ipoffset[0] |= 0x20; /*set MF flag*/
    frag->header.eth_header.len = 1500; /*TODO 1500 why? define this*/
    frag->header.eth_header.ipchksum = 0;
    frag->header.eth_header.ipchksum = ~eth_ipv4_chksum((uint8_t *)&frag->header);
    frag->payload_size = 1514 - frag->header_size; /*TODO 1514 again..*/

    /*update auxiliars*/
    next_payload += frag->payload_size;
    total += frag->payload_size;

    iop_chain_append(&wrapper->fragment_queue, &frag->node);
    frag=NULL;

    /*continue processing frags*/
    /*create new fragments without UDP header*/
    for(int i=1; size != total; i++)
    {
        frag = obtain_free_fragment();
        if(frag==NULL){
            iop_raise_error(OUT_OF_MEMORY);
            return -1;
        }

        /*TODO rethink this*/
        memcpy(&frag->header, get_header(wrapper->buffer), sizeof(eth_header_t)-sizeof(ethproto_header_t));
   //     iop_debug("FCH 0x%06x 0x%06x %d %d\n", &frag->header, get_header(wrapper->buffer), sizeof(eth_header_t)-sizeof(ethproto_header_t), frag->header.eth_header.dst_ip[0]);

        frag->header_size = sizeof(eth_header_t)-sizeof(ethproto_header_t);
        frag->payload = next_payload;

        /*Update MF flag and sequence number*/
        if(size - total > 1480) /*look for the last fragment TODO more unexplained numbers*/ 
        {
            frag->header.eth_header.ipoffset[0] |= 0x20; /*set MF flag*/
            frag->payload_size = 1514 - frag->header_size; /*TODO 1514 again..*/
        }else{
            iop_debug("last frag\n");
            /*last fragment update paylaod size accordingly*/
            frag->payload_size  = size - total;
        }

        /*Calculate fragment header specifics*/
        frag->header.eth_header.ipoffset[0] |= ((185 * i) & 0x1F00) >> 8;
        frag->header.eth_header.ipoffset[1] |= ((185 * i) & 0x00FF);
        frag->header.eth_header.len = IPV4_HDR_SIZE + frag->payload_size;
        frag->header.eth_header.ipchksum = 0;
        frag->header.eth_header.ipchksum = ~eth_ipv4_chksum((uint8_t *)&frag->header);

        /*update auxiliars*/
        next_payload += frag->payload_size;
        total += frag->payload_size;
//        iop_debug(" F %d size  %d total %d buffer size %d %d\n", i, frag->payload_size, total , size, frag->header.eth_header.ipchksum); 
        /*insert in fragment chain*/
        iop_chain_append(&wrapper->fragment_queue, &frag->node);
        frag=NULL;
    }

    return 0;
}

#if 0
uint32_t eth_fragment_packet(iop_wrapper_t *wrapper, uint8_t *buf)
{

    uint16_t size = (uint16_t)get_buffer_size(wrapper->buffer);

    /*buf idx to copy to*/
    uint16_t tail;
    /*lenght to be copied*/
    uint16_t lenght = sizeof(eth_header_t)+sizeof(udp_header_t);
    /*total bytes copied from wrapper*/
    uint16_t total;

    memmove(buf, get_header(wrapper->buffer), lenght);
    tail = lenght;

    eth_header_t *header = (eth_header_t *)buf;
    header->ipoffset[0] |= 0x20; /*set MF flag*/
    header->len = 1500;
    header->ipchksum = 0;
    header->ipchksum = ~eth_ipv4_chksum((uint8_t *)header);

    lenght = 1514 - tail; /*next cpy lenght is 1472*/

    memmove(buf+tail, get_payload(wrapper->buffer), lenght);
    tail += lenght;
    total = 1514;

    /*create new fragments without UDP header*/
    for(int i=1; size != total; i++)
    {
        /*copy original packet header */
        lenght = offsetof(eth_header_t, src_port);
        memmove(buf+tail, get_header(wrapper->buffer), lenght);
        header = (eth_header_t *)(buf+tail);
        tail += lenght;

        /*Update MF flag and sequence number*/
        if(size - total > 1480) /*look for the last fragment*/
        {
            header->ipoffset[0] |= 0x20; /*set MF flag*/
            lenght = 1514 - lenght;
        }else{
            lenght = size - total;
        }

        /*Calculate fragment offset*/
        header->ipoffset[0] |= ((185 * i) & 0x1F00) >> 8;
        header->ipoffset[1] |= ((185 * i) & 0x00FF);
        header->len = IPV4_HDR_SIZE + lenght;

        header->ipchksum = 0;
        header->ipchksum = ~eth_ipv4_chksum((uint8_t *)header);

        memmove(buf+tail, ((uint8_t*)get_header(wrapper->buffer))+total, lenght);

        tail += lenght;
        total += lenght;
    }

    return tail;
}
#endif

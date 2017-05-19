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
#include <string.h>
#include <iop_support.h>
#include <eth_support.h>


#define ETHII_HDR_SIZE                              (14)
#define IPV4_HDR_SIZE                               (20)
#define UDP_HDR_SIZE                                 (8)

#define IPV4_HDR_TTL                                (64)
#define IPV4_HDR_PROTO                              (17)

/**
 * @brief ARP request operation code
 */
#define ARP_HDR_REQUEST                                     ((uint16_t)0x0001)
/**
 * @brief ARP reply operation code
 */
#define ARP_HDR_REPLY                                       ((uint16_t)0x0002)

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
  
	while(dataptr < last_byte) {	/* At least two more bytes */
		t = (dataptr[0] << 8) + dataptr[1];
		sum += t;
		if(sum < t) {
			sum++;		/* carry */
		}
		dataptr += 2;
	}
  
	if(dataptr == last_byte) {
		t = (dataptr[0] << 8) + 0;
		sum += t;
		if(sum < t) {
			sum++;		/* carry */
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
 * @brief Complete UDP header
 * @param src_mac source MAC address
 * @param src_ip source IP address
 * @param data packet payload
 * @param len payload length
 */
static void eth_complete_header(
        udp_header_t *buf, uint8_t *src_mac, uint8_t *src_ip,
        uint8_t *data, uint16_t len) {

    /* setup source MAC and IPs */
    memcpy(buf->src_mac, src_mac, 6);
    memcpy(buf->src_ip, src_ip, 4);

    /* fill the IPv4 and UDP lengths */
    buf->len = HTONS(len + IPV4_HDR_SIZE + UDP_HDR_SIZE);
    buf->udplen = HTONS(len + UDP_HDR_SIZE);

    /* force checksums to be zero*/
    buf->ipchksum = 0;
    buf->udpchksum = 0;

    /* compute IP header checksum */
    buf->ipchksum = ~eth_ipv4_chksum((uint8_t *)buf);
}

void eth_prebuild_header(udp_header_t *buf) {

    /* the total length will be inserted when we have the data */
    buf->len = 0;

    /* time to Live is defined in UIP_TTL (64) */
    buf->ttl = IPV4_HDR_TTL;

    /* protocol is UDP */
    buf->proto = IPV4_HDR_PROTO;

    /* UDP length is unknown. To Fill when we have data */
    buf->udplen = 0;

    /* checksum needs the data */
    buf->udpchksum = 0;

    /* Version and Header Length. Always the same*/
    buf->vhl = 0x45;

    /* Type of Service: always 0*/
    buf->tos = 0;

    /* We don't fragment packets, so this is zero*/
    buf->ipoffset[0] = buf->ipoffset[1] = 0;

    /* We don't fragment packets, so this can have almost any value*/
    buf->ipid[0] = 0;
    buf->ipid[1] = 1 & 0xff;

    /* checksum needs the user data */
    buf->ipchksum = 0;

    /* set type of packet */
    buf->type = HTONS(ETH_HDR_IP_TYPE);
}


void eth_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header) {

    /* get underlying ETH device */
    iop_eth_device_t *eth_dev = (iop_eth_device_t *)iop_dev->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(eth_header_t);
    iop_buf->header_size = sizeof(eth_header_t);

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, iop_buf->header_size);

    /* complete header with the device parameters */
    eth_complete_header(
            (udp_header_t *)iop_buf->v_addr, eth_dev->mac, eth_dev->ip,
            get_payload(iop_buf), get_payload_size(iop_buf));
}



static inline uint32_t eth_compare_ip(uint16_t *ip1, uint16_t *ip2) {

    return ip1[0] == ip2[0] && ip1[1] == ip2[1];
}

static inline uint32_t eth_compare_mac(uint16_t *mac1, uint16_t *mac2) {

    return mac1[0] == mac2[0] && mac1[1] == mac2[1] && mac1[2] == mac2[2];
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

        /* write packet */
        eth_device->dev.write((iop_device_driver_t *)eth_device, wrapper);
    }
}

uint32_t eth_compare_header(iop_wrapper_t *wrapper, iop_header_t *header) {

    udp_header_t *src = (udp_header_t *)header;
    udp_header_t *rcv = (udp_header_t *)get_header(wrapper->buffer);

    if (rcv->dst_port != src->src_port) {
        return 0;
    }

    if (!eth_compare_ip((uint16_t *)rcv->src_ip, (uint16_t *)src->dst_ip)) {
        return 0;
    }

    return 1;
}

uint32_t eth_validate_packet(
        iop_eth_device_t *dev, iop_wrapper_t *wrapper) {

    /* get the UDP header */
    udp_header_t *packet = (udp_header_t *)get_header(wrapper->buffer);

    /* check if the packet was for us */
    if (!eth_compare_mac((uint16_t*)dev->mac, (uint16_t*)packet->dst_mac) ||
        !eth_compare_ip((uint16_t *)dev->ip, (uint16_t*)packet->dst_ip)){

        return 0;
    }

    /* check the if IP version is valid */
    if (packet->vhl != 0x45) {

        return 0;
    }

    /* check if the packet is an UDP packet */
    if (packet->proto != IPV4_HDR_PROTO) {

        return 0;
    }

    /* get received length */
    uint32_t received_length = get_buffer_size(wrapper->buffer);

    /* check if the packet is length is correct */
    if (HTONS(packet->len) > received_length) {

        return 0;
    }

    /* fix padding */
    received_length = HTONS(packet->len);

    /* check if the packet was fragmented */
    if ((packet->ipoffset[0] & 0x3f) != 0 || packet->ipoffset[1] != 0) {

        return 0;
    }

    /* check if the checksum is valid */
    if (eth_ipv4_chksum(
            (uint8_t *)get_header(wrapper->buffer)) != (uint16_t)0xFFFF) {

        return 0;
    }

    /* packet is fine! */
    return 1;
}

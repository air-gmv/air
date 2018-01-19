/** 
 * @file IOPuip.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * @author Cláudio Silva
 * @brief UDP/IP/ARP stack reduced to the bare minimum \n
 * uIP is an implementation of the UDP/IP protocol stack intended for
 * small 8-bit and 16-bit microcontrollers.
 * 
 * @note some types used in these functions are arrays of 16 and 8 bits
 * in order to maintain the compatibility with the original uip types
 * and functions
 * @note adapted from the uIP stack of Adam Dunkels
 *
 * @defgroup UDP:IP
 */
/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip.c,v 1.65 2006/06/11 21:46:39 adam Exp $
 *
 */

#include <string.h>
#include "IOPuip.h"


#define ARP_REQUEST 1
#define ARP_REPLY   2

#define ARP_HWTYPE_ETH 1
/**
 * @brief The uIP TCP/IP statistics.
 *
 * This is the variable in which the uIP IP statistics are gathered.
 */
struct uip_stats uip_stat;

/* all ones ipaddress */
static const uip_ipaddr_t all_ones_addr = {0xffff,0xffff};

/* all zeros ip address */
static const uip_ipaddr_t all_zeroes_addr = {0x0000,0x0000};




/**
 * @brief structure that defines a ARP header compliant with RFC xxxx
 * See RFC xxx for more information
 */
struct arp_hdr {
	struct uip_eth_hdr ethhdr_;
	u16_t hwtype;
	u16_t protocol;
	u8_t hwlen;
	u8_t protolen;
	u16_t opcode;
	struct uip_eth_addr shwaddr;
	u16_t sipaddr[2];
	struct uip_eth_addr dhwaddr;
	u16_t dipaddr[2];
};

/** Broadcast MAC address*/
static const struct uip_eth_addr broadcast_ethaddr = 
{{0xff,0xff,0xff,0xff,0xff,0xff}};

/** Broadcast ip address */
static const u16_t broadcast_ipaddr[2] = {0xffff,0xffff};

/**
 *  \param [in]  sum: pre existing sum
 * 	\param [in] data: data to be checksum'ed
 *	\param [in]  len: length of the data field
 *	\return checksum of the data plus previous sum in host order	
 *
 *  \brief calculates a chksum
 *  \todo change this to a more optimized way: use 32bit sum
 */
static u16_t chksum(u16_t sum, const u8_t *data, u16_t len){
	u16_t t;
	const u8_t *dataptr;
	const u8_t *last_byte;

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

/**
 *  \param [in]  uip_buf: Buffer containing a ethernet header
 *	\return IP header checksum
 *
 *  \brief calculates the IP header chksum
 *  \note assumes the buffer starts at ethernet frame header.
 */
u16_t uip_ipchksum(u8_t *uip_buf){
  u16_t sum;

  sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
  return (sum == 0) ? 0xffff : htons(sum);
}

/**
 *  \param [in]  buf: pointer to buffer containing a ethernet header (ETH+IP+UDP)
 *  \param [in]  data: pointer to data buffer
 *	\return UDP checksum of the packet
 *
 *  \brief calculates the UDP chksum of an UDP packet
 *  \note assumes that the header starts at IP header
 */
u16_t uip_udpchksum(struct uip_udpip_hdr *buf, u8_t *data){
	u16_t upper_layer_len;
	u16_t sum = 0;
	u8_t *aux;
	  
	upper_layer_len = (((u16_t)(buf->len[0]) << 8) + buf->len[1]) - UIP_IPH_LEN;

	/* First sum pseudoheader. */
	  
	/* IP protocol and length fields. This addition cannot carry. */
	sum = upper_layer_len + UIP_PROTO_UDP;
	
	/* Sum IP source and destination addresses. */
	sum = chksum(sum, (u8_t *)&buf->srcipaddr[0], 8);
	
	/* cast to u8_t so we can index using offset in bytes*/
	aux = (u8_t *)buf;
	
	/* Sum UDP header */
	sum = chksum(sum, &aux[UIP_IPH_LEN],8);
	
	/*Sum data*/
	sum = chksum(sum, data, upper_layer_len-8);
		
	return (sum == 0) ? 0xffff : htons(sum);
}

/**
 *  \param [in]  in: Ethernet route obtained from the configuration
 * 	\param [out] out: Ethernet header abiding with the UDP/IP protocol
 *  \brief prebuilds the ethernet headers based on the route configuration
 *
 *	Some fields will only be appended later. We can only add what we know.
 *	Lengths and Checksums are dependent on the user data, so they can only 
 *  be added latter.
 *
 *  It is assumed that *in and *out are members of the same union.
 *  Therefore if we change one we will be changing the other.
 *  eth_header_t will be unusable when this function is over
 */
void uip_build_header(u8_t *out, eth_header_t *in, uip_ipaddr_t *my_ip, struct uip_eth_addr *my_mac) {


	struct uip_udpip_hdr *buf;
	struct uip_eth_hdr *mac_hdr;
	eth_header_t aux;
	
	
	/* in and out are members of a union, so we must store the values
	 * of *in on another structure so we can change the union
	 */
	 
	uip_ipaddr_copy(&aux.dest_ip[0], &in->dest_ip[0]);
	memcpy(&aux.dest_mac[0], &in->dest_mac[0], 6);
	aux.source_port = in->source_port;
	aux.dest_port = in->dest_port;
	
	/* Start prebuilding UDP/IP packet header
	 * Get the beggining of the IP header 
	 */
	buf = (struct uip_udpip_hdr *)&out[UIP_LLH_LEN];
	
	/* The total length will be inserted when we have the data*/
	buf->len[0] = 0;
	buf->len[1] = 0;
	
	/* Time to Live is defined in UIP_TTL (64)*/
	buf->ttl = UIP_TTL;
	
	/* Protocol is UDP*/
	buf->proto = UIP_PROTO_UDP;
	
	/* UDP length is unknown. To Fill when we have data*/
	buf->udplen = 0;
	
	/* Chksum needs the data*/
	buf->udpchksum = 0;
	
	/* Source and Destination UDP ports*/
	buf->srcport  = in->source_port;
	buf->destport = in->dest_port;
	
	/* copy the ip addresses */
	uip_ipaddr_copy(&buf->srcipaddr[0], my_ip);
	uip_ipaddr_copy(&buf->destipaddr[0], &in->dest_ip[0]);
	
	/* Version and Header Length. Always the same*/
	buf->vhl = 0x45;
	
	/* Type of Service: always 0*/
	buf->tos = 0;
	
	/* We don't fragment packts, so this is zero*/
	buf->ipoffset[0] = buf->ipoffset[1] = 0;
	
	/* We don't fragment packts, so this can have almost any value*/
	buf->ipid[0] = 0;
	buf->ipid[1] = 1 & 0xff;
	
	/* Chksum needs the user data */
	buf->ipchksum = 0;
	
	/* Start prebuilding ethernet frame header
	 * Get the beggining of the mac header 
	 */
	mac_hdr = (struct uip_eth_hdr *) out;
	
	/* Insert destination mac address*/
	memcpy(&mac_hdr->dest.addr[0], &aux.dest_mac[0], 6);
	
	/* insert source mac address (ours) */
	memcpy(&mac_hdr->src.addr[0], my_mac, 6);
	
	/* insert type (Type = IP = 0x0800) */
	mac_hdr->type = HTONS(UIP_ETHTYPE_IP);
	
	return;
}

/**
 *  \param [in] hdr: pointer to ETH:IP:UDP header
 * 	\param [in] data: pointer to frame data
 *  \param [in] data_len: data size un bytes
 *  \brief completes the udp header based on the new data
 *
 */
void uip_complete_header(uint8_t *hdr, uint8_t *data, int data_len){
	struct uip_udpip_hdr *udpip_hdr;
	struct uip_eth_hdr *mac_hdr;
	
	/* Get the beggining of the IP header*/
	udpip_hdr = (struct uip_udpip_hdr *)&hdr[UIP_LLH_LEN];
	
	/* Get the beggining of the mac header */
	mac_hdr = (struct uip_eth_hdr *) hdr;
	
	/* Fill in lengths */
	udpip_hdr->len[0] = ((data_len + UIP_IPUDPH_LEN) >> 8);
	udpip_hdr->len[1] = ((data_len + UIP_IPUDPH_LEN) & 0xff);
	
	/* fill udp length*/
	udpip_hdr->udplen = HTONS(data_len + UIP_UDPH_LEN);
	
	/* force the chksums to be zero*/
	udpip_hdr->udpchksum = 0;
	udpip_hdr->ipchksum = 0;
	
	/* @todo!!! udp checksum!!!*/
	/* Calculate UDP checksum. */
	// udpip_hdr->udpchksum = ~(uip_udpchksum(udpip_hdr, data));
	// if(udpip_hdr->udpchksum == 0) {
		// udpip_hdr->udpchksum = 0xffff;
	// }
	
	/* calculate IP header checksum */
	udpip_hdr->ipchksum = ~(uip_ipchksum(hdr));
	
	return;
}

/**
 *  \param [in]  rcvd_hdr: UDP/IP header received by the HW
 * 	\param [in]  route_hdr: UDP/IP header from a route
 *  \return 1 - headers match, 0 - headers mismatch
 *
 *  \brief compares the received header with the one from the route
 *
 * 	We know that the packet is aimed at our IP and MAC address since we
 * 	have already check it in @see uip_validate_ip_packet	
 * 	The only thing left to check is the destination port and the source
 * 	ip address.
 * 	The source IP address of the packet shall be equal to the dest_ip
 * 	on the route. Destination port of the incoming packet shall be equal
 * 	to the source port on the route.
 */
unsigned int uip_compare_header(u8_t *rcvd_hdr, eth_header_t *route_hdr){
	struct uip_udpip_hdr *header;
	unsigned int rc;
	u16_t *srcipaddr;
	
	header = (struct uip_udpip_hdr *) &rcvd_hdr[UIP_LLH_LEN];
	srcipaddr = (u16_t *)route_hdr->dest_ip;
	
	rc = 1;
	
	/* compare ports*/
	if(header->destport != route_hdr->source_port) {
		rc = 0;
		//printf("PORTS DO NOT MATCH %d != %d\n", route_hdr->source_port, header->destport);
	}
	
	/* compare IP's */
	if((header->srcipaddr[0] != srcipaddr[0]) ||
		(header->srcipaddr[1] != srcipaddr[1])){
		rc = 0;
		//printf("IPs DO NOT MATCH\n");
	}
	
	
	return rc;
}

/**
 *  \param [in]  rcvd_hdr: UDP/IP header received by the HW
 *  \return 2- ARP, 1 - IP, 0 - Other protocol
 *
 *  \brief checks if the ethernet packet is IP or ARP
 */
unsigned int uip_check_type(u8_t *rcvd_hdr){
	struct uip_eth_hdr *mac_hdr;
	unsigned int rc;
	
	mac_hdr = (struct uip_eth_hdr *)rcvd_hdr;
	
	switch(mac_hdr->type){
		case UIP_ETHTYPE_ARP:
			rc = 2;
			break;
		case UIP_ETHTYPE_IP:
			rc = 1;
			break;
		default:
			rc = 0;
			break;
	}

	return rc;
}

/**
 *  \param [in]  rcvd_hdr: pointer to ethernet frame header(eth+IP+UDP header) 
 *  \param [in]  rcvd_data: pointer to data
 *  \param [in]  len: packet length has received by the HW
 *  \return 1-packet is fine; 0- error in packet
 *
 *  \brief validates an incoming IP packet
 */
unsigned int uip_validate_ip_packet(
	u8_t *rcvd_hdr, 
	u8_t *rcvd_data, 
	unsigned int *len,
	uip_ipaddr_t *my_ip)
{

	struct uip_udpip_hdr *hdr;
	unsigned int real_length;
	unsigned int rc;
	
	rc = 1;
	
	/* Get the beggining of the IP header */
	hdr = (struct uip_udpip_hdr *)&rcvd_hdr[UIP_LLH_LEN];
	
	
	/* Check validity of the IP header. */
	/* IP version and header length. */
	if(hdr->vhl != 0x45) {
		uip_stat.ip.drop++;
		uip_stat.ip.vhlerr++;
		rc = 0;
	}
	
	/*Length*/
	real_length = ((hdr->len[0] << 8) + hdr->len[1]) + 14;
	
	/*packet is greater than we received*/
	if(real_length > *len) {
		uip_stat.ip.drop++;
		rc = 0;
	}
	
	/* check if the packet was padded */
	if((real_length < 60) && (*len = 60)){
		
		/* obtain the packet real length */
		*len = real_length;
	}
	
	/* check fragmentation*/
	if((hdr->ipoffset[0] & 0x3f) != 0 || hdr->ipoffset[1] != 0) {
		uip_stat.ip.drop++;
		uip_stat.ip.fragerr++;
		rc = 0;
	}
	
	/*Check if this is a UDP packet*/
	if(hdr->proto != UIP_PROTO_UDP){
		uip_stat.ip.drop++;
		uip_stat.ip.protoerr++;
		rc = 0;
	}
	
	/* lets compare the IP addresses*/
	if(!uip_ipaddr_cmp(hdr->destipaddr, all_ones_addr) &&
	   !uip_ipaddr_cmp(hdr->destipaddr, my_ip)) 
	{
		uip_stat.ip.drop++;
		rc = 0;
	}
	
	/* Compute and check the IP header checksum. */
	if(uip_ipchksum(rcvd_hdr) != 0xffff) { 
		uip_stat.ip.drop++;
		uip_stat.ip.chkerr++;
		rc = 0;
	}
	
	/** @todo udp checksum*/
	/* Compute and check the UDP checksum */
	// if(hdr->udpchksum != 0 && uip_udpchksum(hdr, rcvd_data) != 0xffff) {
		// uip_stat.ip.drop++;
		// uip_stat.ip.chkerr++;
		// rc = 0;
	// }
	
	return rc;

}

u16_t htons(u16_t val){

	return HTONS(val);
}





/**
 * @brief ARP processing for incoming ARP packets.
 * @param [in,out] rcvd_hdr: buffer to an ARP request header
 * @param [in,out] len: length of that same header
 *
 * This function should be called by the device driver when an ARP
 * packet has been received. The function will act differently
 * depending on the ARP packet type. If the incoming ARP packet is an ARP
 * request for our IP address, an ARP reply packet is created and put
 * into same buffer
 *
 * When the function returns, the value of the variable len
 * indicates whether the device driver should send out a packet or
 * not. If len is zero, no packet should be sent. If len is
 * non-zero, it contains the length of the outbound packet that is
 * present in the buffer.
 *
 * This function expects an ARP packet with a prepended Ethernet
 * header in the buffer.
 */
void uip_arp_arpin(u8_t *rcvd_hdr, int *len, u16_t *my_ip, u16_t *my_mac){

	struct arp_hdr *buf;
	
	/* Check the size of the incomming ARP packet */
	if(*len < sizeof(struct arp_hdr)) {
		*len = 0;
		return;
	}
	*len = 0;
	
	buf = (struct arp_hdr *) rcvd_hdr;
	
	switch(buf->opcode) {
		case HTONS(ARP_REQUEST):
			/* ARP request. If it asked for our address, we send out a
			reply. */
		
			if(uip_ipaddr_cmp(buf->dipaddr, my_ip)) {
      
				/* The reply opcode is 2. */
				buf->opcode = HTONS(2);

				memcpy(&buf->dhwaddr.addr[0], &buf->shwaddr.addr[0], 6);
				memcpy(&buf->shwaddr.addr[0], my_mac, 6);
				memcpy(&buf->ethhdr_.src.addr[0], my_mac, 6);
				memcpy(&buf->ethhdr_.dest.addr[0], &buf->dhwaddr.addr[0], 6);
      
				buf->dipaddr[0] = buf->sipaddr[0];
				buf->dipaddr[1] = buf->sipaddr[1];
				buf->sipaddr[0] = my_ip[0];
				buf->sipaddr[1] = my_ip[1];

				buf->ethhdr_.type = HTONS(UIP_ETHTYPE_ARP);
				*len = sizeof(struct arp_hdr);
			}
		break;
		case HTONS(ARP_REPLY):
		default:
			/* 
			 * Something we are not ready to process 
			 * since we don't issue requests, there will be no replies to us
			 */
			break;
	}

	return;
}

/** 
 * @file IOPuip.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * @ingroup UDP:IP
 * @author Cláudio Silva
 *
 * @brief The uIP TCP/IP stack header file contains definitions for a number
 * of C macros that are used by uIP programs as well as internal uIP
 * structures, UDP/IP header structures and function declarations.
 *
 * @note adapted from the uIP stack of Adam Dunkels
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
 * $Id: uip.h,v 1.40 2006/06/08 07:12:07 adam Exp $
 *
 */

#ifndef __UIP_H__
#define __UIP_H__

#include <stdint.h>
#include "IOPgreth.h"
/**
 * 8 bit datatype
 *
 * This typedef defines the 8-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint8_t u8_t;

/**
 * 16 bit datatype
 *
 * This typedef defines the 16-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint16_t u16_t;

/**
 * Statistics datatype
 *
 * This typedef defines the dataype used for keeping statistics in
 * uIP.
 *
 * \hideinitializer
 */
typedef unsigned short uip_stats_t;


/**
 * Representation of an IP address
 *
 */
typedef u16_t uip_ip4addr_t[2];
typedef uip_ip4addr_t uip_ipaddr_t;

/**
 * @brief Representation of a 48-bit Ethernet address.
 */
struct uip_eth_addr {
	u8_t addr[6];	/**< Ethernet Address*/
};

/**
 * @brief structure holding the IP statistics
 *
 */
struct uip_stats {
  struct {
    uip_stats_t drop;     /**< Number of dropped packets at the IP
			     layer. */
    uip_stats_t recv;     /**< Number of received packets at the IP
			     layer. */
    uip_stats_t sent;     /**< Number of sent packets at the IP
			     layer. */
    uip_stats_t vhlerr;   /**< Number of packets dropped due to wrong
			     IP version or header length. */
    uip_stats_t hblenerr; /**< Number of packets dropped due to wrong
			     IP length, high byte. */
    uip_stats_t lblenerr; /**< Number of packets dropped due to wrong
			     IP length, low byte. */
    uip_stats_t fragerr;  /**< Number of packets dropped since they
			     were IP fragments. */
    uip_stats_t chkerr;   /**< Number of packets dropped due to IP
			     checksum errors. */
    uip_stats_t protoerr; /**< Number of packets dropped since they
			     were neither ICMP, UDP nor TCP. */
  } ip;                   /**< IP statistics. */
  struct {
    uip_stats_t drop;     /**< Number of dropped UDP segments. */
    uip_stats_t recv;     /**< Number of recived UDP segments. */
    uip_stats_t sent;     /**< Number of sent UDP segments. */
    uip_stats_t chkerr;   /**< Number of UDP segments with a bad
			     checksum. */
  } udp;                  /**< UDP statistics. */
};


/**
 * @brief The Ethernet header.
 */
struct uip_eth_hdr {
    struct uip_eth_addr dest;	/**< destination ethernet address*/
    struct uip_eth_addr src;	/**< source ethernet address*/
    u16_t type;					/**< next protocol type */
};


/** 
 * @brief The UDP and IP headers as defined by some RFC 
 */
struct uip_udpip_hdr {
    /* Ip header*/
	u8_t vhl;				/**< Version and Header length in 32 bits words */
    u8_t tos;				/**< Type Of Service. Never used */
    u8_t len[2];			/**< Total Lentgh: from 21 to 1499 */
    u8_t ipid[2];			/**< Ip packet identificator. Used in reassemble */
    u8_t ipoffset[2];		/**< Fragment Offset. Used in reassemble */
    u8_t ttl;				/**< Time to Live. Defualts to 64*/
    u8_t proto;				/**< Protocol: UDP = 0x11 */
    u16_t ipchksum;			/**< IP header and data checksum */
    u16_t srcipaddr[2];		/**< Source Ip address */
    u16_t destipaddr[2];	/**< Destination IP address */
  
	/* UDP header. */
	u16_t srcport;			/**< Source UDP port */
	u16_t destport;			/**< Destination UDP port */
	u16_t udplen;			/**< udp packet length */
	u16_t udpchksum;		/**< pseudo header + UDP header + data checksum */ 
};

/**
 * @name IP configuration options
 * @{
 */
 
/**
 * The IP TTL (time to live) of IP packets sent by uIP.
 *
 * This should normally not be changed.
 */
#define UIP_TTL         64

/** @} */

/**
 * \name CPU architecture configuration
 * @{
 *
 * The CPU architecture configuration is where the endianess of the
 * CPU on which uIP is to be run is specified. Most CPUs today are
 * little endian, and the most notable exception are the Motorolas
 * which are big endian. The BYTE_ORDER macro should be changed to
 * reflect the CPU architecture on which uIP is to be run.
 */

/**
 * The byte order of the CPU architecture on which uIP is to be run.
 *
 * This option can be either BIG_ENDIAN (Motorola byte order) or
 * LITTLE_ENDIAN (Intel byte order).
 *
 * \hideinitializer
 */

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

 
#define UIP_CONF_BYTE_ORDER     UIP_BIG_ENDIAN
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER

/** @} */

/**
 * The link level header length.
 *
 * This is the offset into the uip_buf where the IP header can be
 * found. For Ethernet, this should be set to 14.
 * @hideinitializer
 */
#define UIP_LLH_LEN     14


/** @} */

/** IP protocol definition*/
#define UIP_PROTO_ICMP  1
#define UIP_PROTO_TCP   6
#define UIP_PROTO_UDP   17

/** Header Sizes in bytes (octets)*/
#define UIP_IPH_LEN    20    /* Size of IP header */
#define UIP_UDPH_LEN    8    /* Size of UDP header */

/** Size of IP + UDP header */
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)

#define UIP_UDPIP_HLEN UIP_IPUDPH_LEN

/** size remaining for data*/
#define UIP_APPDATA_SIZE (UIP_BUFSIZE - UIP_LLH_LEN - UIP_UDPIP_HLEN)




/*ARP definitions*/

#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_ETHTYPE_IP6 0x86dd



/**
 * Copy an IP address to another IP address.
 *
 * Copies an IP address from one place to another.
 *
 * Example:
 \code
 uip_ipaddr_t ipaddr1, ipaddr2;

 uip_ipaddr(&ipaddr1, 192,16,1,2);
 uip_ipaddr_copy(&ipaddr2, &ipaddr1);
 \endcode
 *
 * \param dest The destination for the copy.
 * \param src The source from where to copy.
 *
 * \hideinitializer
 */
#define uip_ipaddr_copy(dest, src) memcpy(dest, src, 4)


/**
 * Convert 16-bit quantity from host byte order to network byte order.
 *
 * This macro is primarily used for converting constants from host
 * byte order to network byte order. For converting variables to
 * network byte order, use the htons() function instead.
 *
 * \hideinitializer
 */
#ifndef HTONS
#   if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
#      define HTONS(n) (n)
#   else /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#      define HTONS(n) (u16_t)((((u16_t) (n)) << 8) | (((u16_t) (n)) >> 8))
#   endif /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#else
#error "HTONS already defined!"
#endif /* HTONS */

/**
 * Convert 16-bit quantity from host byte order to network byte order.
 *
 * This function is primarily used for converting variables from host
 * byte order to network byte order. For converting constants to
 * network byte order, use the HTONS() macro instead.
 */
#ifndef htons
u16_t htons(u16_t val);
#endif /* htons */
#ifndef ntohs
#define ntohs htons
#endif

#define uip_ipaddr_cmp(addr1, addr2) (((u16_t *)addr1)[0] == ((u16_t *)addr2)[0] && \
                      ((u16_t *)addr1)[1] == ((u16_t *)addr2)[1])


/* UIP Function prototypes*/

static u16_t chksum(u16_t sum, const u8_t *data, u16_t len);

u16_t uip_ipchksum(u8_t *uip_buf);

u16_t uip_udpchksum(struct uip_udpip_hdr *buf, u8_t *data);

void uip_build_header(
    u8_t *out, eth_header_t *in, 
    uip_ipaddr_t *my_ip, 
    struct uip_eth_addr *my_mac);

void uip_complete_header(uint8_t *hdr, uint8_t *data, int data_len);

unsigned int uip_compare_header(u8_t *rcvd_hdr, eth_header_t *route_hdr);

unsigned int uip_check_type(u8_t *rcvd_hdr);

unsigned int uip_validate_ip_packet(
    u8_t *rcvd_hdr, 
    u8_t *rcvd_data, 
    unsigned int *len,
    uip_ipaddr_t *my_ip);

u16_t htons(u16_t val);

void uip_arp_arpin(u8_t *rcvd_hdr, int *len, u16_t *my_ip, u16_t *my_mac);

#endif

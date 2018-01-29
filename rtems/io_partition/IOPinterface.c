#include "iop.h"

uint8_t udpiphdr[42]; /* for recv*/
uint8_t udpiphd[42];  /* for send*/

uint8_t my_ip[4];
uint8_t my_mac[6];



uint8_t *get_my_mac(void) {
	return (struct uip_eth_addr *) my_mac;
}


void build_eth_hdr(uint8_t *ip, uint8_t *mac, 
	uint16_t port, eth_header_t *eth) {

	memcpy(eth->dest_ip, ip, 4);
	memcpy(eth->dest_mac, mac, 6);
	eth->dest_port = port;
	eth->source_port = port;

}


int IOPsend(eth_header_t iphdr, char *datain, int len) {

	greth_args_t wargs;

	memset(&wargs, 0, sizeof(greth_args_t));

	uip_build_header(&(udpiphd[0]), &iphdr, &(my_ip[0]), &(my_mac[0]));

	wargs.data = (char *)datain;
	wargs.hdr = (char *)&(udpiphd[0]);
	wargs.data_len = len;
	wargs.hdr_len = 42;
	wargs.bytes_moved = 0;

	uip_complete_header(wargs.hdr,
		(uint8_t *)wargs.data,
		wargs.data_len);

	if (greth_write(0, 0, (void *) &wargs) != RTEMS_SUCCESSFUL) {
		printf("greth write unsuccessful\n");
		return -1;
	}

	return wargs.bytes_moved;
}


int IOPreceive(char *outbuf) {
	greth_args_t rargs;
	int aux;

	memset(&rargs, 0, sizeof(greth_args_t));

	rargs.data = outbuf;
	rargs.hdr = (uint8_t *)(&(udpiphdr[0]));
	rargs.data_len = 0;
	rargs.hdr_len = 42;
	rargs.bytes_moved = 0;

	if ((greth_read(0, 0, (void *) &rargs)) == RTEMS_SUCCESSFUL) {
		aux = uip_check_type(&(udpiphdr[0])); 


		switch (aux) {
			case 1: /*IP*/
				aux = uip_validate_ip_packet(&(udpiphdr[0]),
										(unsigned char *)&outbuf[0],
										(unsigned int *)&rargs.bytes_moved,
										my_ip);
				/*
				if(aux) printf("RECV: %s\n", outbuf);
				*/
				

				if (aux) {
					return (int) (rargs.bytes_moved - 42);
				} else {
					return -1;
				}
			break;
			case 2: 
			
				uip_arp_arpin(&(udpiphdr[0]), &(rargs.bytes_moved), my_ip, my_mac);
				if(rargs.bytes_moved > 0) {
								
					rargs.hdr_len = rargs.bytes_moved;
					rargs.data_len = 0;
						
					printf("sending arp reply\n");
					
					greth_write(0, 0, (void *) &rargs);
						
				}
				return 0;
			break;
			default:
				printf("received unknown packet type\n");
				return -1;
			break;
		}
	} else {
		/*ToDo: error handling*/
		return -1;
	}
}


rtems_status_code IOPinitialize(uint8_t *myip, uint8_t *mymac) {

	rtems_status_code rc;

	printf("Initizaling IO Partition\n");

	memset(my_ip, 0, 4);
	memset(my_mac, 0, 6);
	memset(udpiphd, 0, 42);
	memset(udpiphdr, 0, 42);

	/*board ip and mac addr*/
	memcpy(my_ip, myip, 4);
	memcpy(my_mac, mymac, 6);

	int j=0;

	rc = greth_initialize(0,0,NULL);
	if (rc != RTEMS_SUCCESSFUL) {
		printf("greth_initialize error\n");
		return rc;
	}
	rc = greth_open(0,0,NULL);
	if (rc != RTEMS_SUCCESSFUL) {
		printf("greth_open error\n");
		return rc;
	}


	return RTEMS_SUCCESSFUL;

}
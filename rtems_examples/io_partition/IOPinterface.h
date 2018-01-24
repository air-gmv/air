uint8_t *get_my_mac(void);

void build_eth_hdr(uint8_t *ip, uint8_t *mac, 
	uint16_t port, eth_header_t *eth);

int IOPsend(eth_header_t iphdr, char *datain, int len);

int IOPreceive(char *outbuf);

rtems_status_code IOPinitialize(uint8_t *myip, uint8_t *mymac);


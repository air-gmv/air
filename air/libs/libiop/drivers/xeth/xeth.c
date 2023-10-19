/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/** 
 * @file xeth.c
 * @author TTAO
 * @brief XETH Ethernet Driver
 *        Based on the Zynq-7000 SoC Technical Reference Manual
 */



#include <xeth.h>



/**
 * Checks if the received Ethernet Packet is valid.
 * @param dev Ethernet Device
 * @param packet Ethernet Header
 * @return 1 If we received an ARP Packet
 *         0 If we received a valid UDP Packet
 *        -1 Otherwise
 */
air_u32_t validate_packet(iop_eth_device_t *dev, eth_header_t *packet) {

    if(packet->type == HTONS(ETH_HDR_ARP_TYPE)){
    	// ARP Ethernet Packet
        return 1;
    }


    if(packet->type == HTONS(ETH_HDR_IP_TYPE)){
    	// IP Ethernet Packet

    	// Check if the Packet was for this Ethernet Device
        if (!eth_compare_mac((uint16_t*)dev->mac, (uint16_t*)packet->dst_mac) ||
                          !eth_compare_ip((uint16_t *)dev->ip, (uint16_t*)packet->dst_ip)){
            return -1;
        }

        // Check if the IP version is Valid
        if (packet->vhl != 0x45) {
            return -1;
        }

        // Check if the Packet is a UDP Packet
        if (packet->proto != IPV4_HDR_PROTO) {
            return -1;
        }

        // Check if the Checksum is Valid
        if (eth_ipv4_chksum((uint8_t *)packet) != (uint16_t)0xFFFF) {
            return -1;
        }

        return 0;
    }

    return -1;
}




air_u32_t iop_xeth_init(iop_device_driver_t *iop_dev, void *arg) {

    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    xeth_struct *xs = (xeth_struct *)(device->dev.driver);


    // Get the Base Address of the Registers of the Ethernet Device
    if(device->id == 0){
        xs->regs = 0xE000B000;
        xs->mac_regs = 0xE000B088;
    }
    else if(device->id == 1) {
        xs->regs = 0xE000C000;
        xs->mac_regs = 0xE000C088;
    }
    else {
        return AIR_DEVICE_NOT_FOUND;
    }


    // Initialize the Controller
    xs->regs->nw_ctrl = 0x20;
    xs->regs->tx_status = 0xFF;
    xs->regs->rx_status = 0x0F;
    xs->regs->int_disable = 0x7FFFEFF;
    xs->regs->rx_base_addr = 0x0;
    xs->regs->tx_base_addr = 0x0;


    // Configure the Controller
    xs->regs->nw_config |= 0x011E2453;
    xs->regs->dma_config = 0x190F10;
    xs->regs->nw_ctrl |= 0x10;


    // Set MAC Address
    xs->mac_regs->mac_addr_msb = device->mac[0] << 8  | device->mac[1];
    xs->mac_regs->mac_addr_lsb = device->mac[2] << 24 | device->mac[3] << 16 | device->mac[4] << 8  | device->mac[5];


    return AIR_SUCCESSFUL;
}




air_u32_t iop_xeth_open(iop_device_driver_t *iop_dev, void *arg) {

    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    xeth_struct *xs = (xeth_struct *)(device->dev.driver);


    // Initialize XETH structure
    xs->tx_ptr = 0;
    xs->rx_ptr = 0;


    // Setup IOP Buffers
    setup_iop_buffers(xs->iop_buffers, xs->iop_buffers_storage, device->rx_count + device->tx_count);


    // Configure Transmit Buffer Descriptor
    xs->tx_bd = (xeth_bd *)(((air_u32_t)xs->tx_bd + 1024) & ~(1024 - 1));
    for (int i = 0; i < device->tx_count; i++){
        xs->tx_bd[i].config = 0x0;
        xs->tx_iop_buffer[i] = &xs->iop_buffers[i];
        xs->tx_bd[i].addr = (uint32_t) xs->tx_iop_buffer[i]->p_addr;
        if(i == device->tx_count-1)
            xs->tx_bd[i].config |= 0x40000000;
    }
    xs->regs->tx_base_addr = (uint32_t) air_syscall_get_physical_addr((air_uptr_t)xs->tx_bd);


    // Configure Receive Buffer Descriptor
    xs->rx_bd = (xeth_bd *)((((air_u32_t)xs->rx_bd + 1024) & ~(1024 - 1)) + 1024);
    for (int i = 0; i < device->rx_count; i++){
        xs->rx_iop_buffer[i] = &xs->iop_buffers[device->tx_count + i];
        xs->rx_bd[i].addr = (uint32_t) xs->rx_iop_buffer[i]->p_addr;
        if(i == device->rx_count-1)
            xs->rx_bd[i].addr |= 0x2;
    }
    xs->regs->rx_base_addr = (uint32_t) air_syscall_get_physical_addr((air_uptr_t)xs->rx_bd);


    // Enable the Ethernet Controller
    xs->regs->nw_ctrl |= 0xc;


    return AIR_SUCCESSFUL;
}




air_u32_t iop_xeth_read(iop_device_driver_t *iop_dev, void *arg) {

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    xeth_struct *xs = (xeth_struct *)(device->dev.driver);


    // Sanity Check
    if (wrapper == NULL || wrapper->buffer == NULL){
        return AIR_INVALID_PARAM;
    }


    if(xs->rx_bd[xs->rx_ptr].addr & 0x1){
    	// Received an Ethernet Packet


    	// Get the Ethernet Packet length
        uint32_t len = 0;
        len = xs->rx_bd[xs->rx_ptr].config & 0x1FFF;


        // Copy the Ethernet Header to a free Fragment
        iop_fragment_t *frag = obtain_free_fragment();
        memcpy(&frag->header, xs->rx_iop_buffer[xs->rx_ptr]->v_addr, sizeof(eth_header_t));


        int valid = -1;
        if((valid = validate_packet(device, &frag->header.eth_header)) == 0){

        	// Received a valid UDP Packet
            unsigned int pack_seq = (((unsigned int)frag->header.eth_header.ipoffset[0] & 0x1f)<<8) + frag->header.eth_header.ipoffset[1];

            if(pack_seq == 0){
            	// The received UDP Packet is the First Fragment or a whole Frame

                if(!iop_chain_is_empty(&wrapper->fragment_queue)){
                    iop_fragment_t *frag_aux;
                    while(!iop_chain_is_empty(&wrapper->fragment_queue)){
                        frag_aux = obtain_fragment(&wrapper->fragment_queue);
                        release_fragment(frag_aux);
                    }
                }

                // Copy the received UDP Frame/Fragment to the IOP Wrapper
                memcpy(wrapper->buffer->v_addr, xs->rx_iop_buffer[xs->rx_ptr]->v_addr, len);

                frag->header_size = sizeof(eth_header_t);
                frag->payload = wrapper->buffer->v_addr + frag->header_size;
                frag->payload_size = len - frag->header_size;

                iop_chain_append(&wrapper->fragment_queue, &frag->node);

                wrapper->buffer->header_off = 0;
                wrapper->buffer->header_size = frag->header_size;
                wrapper->buffer->payload_off = frag->header_size;
                wrapper->buffer->payload_size = frag->payload_size;


            }else{

                if(iop_chain_is_empty(&wrapper->fragment_queue)){
                	// Out of order Fragment

                    release_fragment(frag);
                    len = 0;

                }else{

                    iop_fragment_t *last_frag = (iop_fragment_t *)wrapper->fragment_queue.last;
                    unsigned int last_pack_seq = (((unsigned int)last_frag->header.eth_header.ipoffset[0] & 0x1f)<<8) + last_frag->header.eth_header.ipoffset[1];

                    if(last_pack_seq + 185 != pack_seq){
                    	// Fragment with wrong Sequence Number

                        release_fragment(frag);
                        len=0;

                    }else{
                    	// Fragmented Packet with correct sequence number
                        frag->header_size = sizeof(eth_header_t) - sizeof(ethproto_header_t);
                        frag->payload = last_frag->payload + last_frag->payload_size;
                        frag->payload_size = len - frag->header_size;


                        // Copy the received Ethernet Fragment to the IOP Wrapper
                        memcpy(frag->payload, xs->rx_iop_buffer[xs->rx_ptr]->v_addr+frag->header_size, frag->payload_size);
                        wrapper->buffer->payload_size += frag->payload_size;
                        iop_chain_append(&wrapper->fragment_queue, &frag->node);
                    }
                }
            }
        }else{

            if(valid == 1){
            	// Received an ARP Packet

                static iop_wrapper_t *arp_wrapper=NULL;

                if(arp_wrapper == NULL)
                    arp_wrapper = obtain_free_wrapper();
                else{
                    release_wrapper(arp_wrapper);
                    arp_wrapper = obtain_free_wrapper();
                }

                memcpy(arp_wrapper->buffer->v_addr, xs->rx_iop_buffer[xs->rx_ptr]->v_addr, len);

                arp_wrapper->buffer->header_off = 0;
                arp_wrapper->buffer->header_size = sizeof(eth_header_t);
                arp_wrapper->buffer->payload_off = sizeof(eth_header_t);
                arp_wrapper->buffer->payload_size = len;

                // Copy ARP Packet to the IOP Wrapper
                wrapper->buffer = arp_wrapper->buffer;

                release_fragment(frag);

            }else{
            	// Received an invalid Packet

                release_fragment(frag);
                len=0;
            }

        }

        // Clean Buffer Descriptor Ownership Bit
        xs->rx_bd[xs->rx_ptr].addr &= 0xFFFFFFFE;

        xs->rx_bd[xs->rx_ptr].config &= 0xFFFFFE00;

        // Increment Buffer Descriptor Pointer
        xs->rx_ptr = (xs->rx_ptr + 1) % device->rx_count;

        return AIR_SUCCESSFUL;
    }

    return AIR_NO_ACTION;
}




air_u32_t iop_xeth_write(iop_device_driver_t *iop_dev, void *arg) {

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    xeth_struct *xs = (xeth_struct *)(device->dev.driver);


    // Sanity Check
    if (wrapper == NULL || wrapper->buffer == NULL || wrapper->buffer->payload_size == 0){
        return AIR_INVALID_PARAM;
    }

    if(iop_chain_is_empty(&wrapper->fragment_queue)){
        return AIR_INVALID_SIZE;
    }


    // Copy the Fragment Header and Payload to the Buffer
    iop_fragment_t *frag = obtain_fragment(&wrapper->fragment_queue);
    memcpy(xs->tx_iop_buffer[xs->tx_ptr]->v_addr, &frag->header, frag->header_size);
    memcpy(xs->tx_iop_buffer[xs->tx_ptr]->v_addr + frag->header_size, frag->payload, frag->payload_size);


    // Complete and Enable the Buffer Descriptor
    xs->tx_bd[xs->tx_ptr].config = 0x0;
    if(xs->tx_ptr == device->tx_count-1)
        xs->tx_bd[xs->tx_ptr].config = 0x40000000;
    xs->tx_bd[xs->tx_ptr].config |= frag->header_size + frag->payload_size;
    xs->tx_bd[xs->tx_ptr].config |= 0x8000;
    xs->tx_bd[xs->tx_ptr].config &= ~0x80000000;


    // Enable Transmission
    xs->regs->nw_ctrl |= 0x200;


    // Increment Buffer Descriptor Pointer
    xs->tx_ptr = (xs->tx_ptr + 1) % device->tx_count;
    release_fragment(frag);


    return AIR_SUCCESSFUL;
}




air_u32_t iop_xeth_close(iop_device_driver_t *iop_dev, void *arg) {

    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    xeth_struct *xs = (xeth_struct *)(device->dev.driver);


    // Disable the Ethernet Controller
    xs->regs->nw_ctrl &= ~0xc;


    return AIR_SUCCESSFUL;
}

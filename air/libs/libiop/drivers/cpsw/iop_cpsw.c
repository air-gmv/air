/**
 * @file
 * @author pfnf
 * @brief CPSW Ethernet driver, with pooled send and receive
 */

/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 * Copyright (c) 2010 Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * Copyright (c) 2010 Texas Instruments Incorporated
 *
 * This file is dervied from the "ethernetif.c" skeleton Ethernet network
 * interface driver for lwIP.
 */

#include <iop_cpsw.h>
#include <eth_support.h>

#include <phy.h>
#include <mdio.h>
#include <cpsw.h>
#include <hw_types.h>
#include <soc_AM335x.h>
#include <hw_control_AM335x.h>

extern void CPSWClkEnable(void);
extern void CPSWPinMuxSetup(void);

/**
 * @brief CPSW device setup
 * @param dev Device setup
 */
static void cpsw_setup_eth0(cpsw_device_t *dev)
{

    /* device base registers */
    dev->ss_base = CPSW0_SS_REGS;
    dev->mdio_base = CPSW0_MDIO_REGS;
    dev->wrpr_base = CPSW0_WR_REGS;
    dev->cpdma_base = CPSW0_CPDMA_REGS;
    dev->ale_base = CPSW0_ALE_REGS;
    dev->cppi_ram_base = CPSW0_CPPI_RAM_REGS;
    dev->host_port_base = CPSW0_PORT_0_REGS;

    /* device slave port 1 */
    dev->port[PORT_1].port_base = CPSW0_PORT_1_REGS;
    dev->port[PORT_1].sliver_base = CPSW0_SLIVER_1_REGS;
#ifdef CPSW0_PORT_1_PHY_ADDR
    dev->port[PORT_1].phy_addr = CPSW0_PORT_1_PHY_ADDR;
    dev->port[PORT_1].phy_gbps = CPSW0_PORT_1_PHY_GIGABIT;
#endif

    /* device slave port 2 */
    dev->port[PORT_2].port_base = CPSW0_PORT_2_REGS;
    dev->port[PORT_2].sliver_base = CPSW0_SLIVER_2_REGS;
#ifdef CPSW0_PORT_2_PHY_ADDR
    dev->port[PORT_2].phy_addr = CPSW0_PORT_2_PHY_ADDR;
    dev->port[PORT_2].phy_gbps = CPSW0_PORT_2_PHY_GIGABIT;
#endif
}

/**
 * @brief Gives the index of the ALE entry which is free
 * @param  cpsw_config CPSW configuration structure pointer
 * @return index of the ALE entry which is free
 *         -1 if entry not found
 */
static int32_t cpsw_ale_entry_match_free(cpsw_device_t *cpsw_config)
{

    int32_t idx;
    unsigned int ale_entry[ALE_ENTRY_NUM_WORDS];

    /* check which ALE entry is free starting from 0th entry */
    for (idx = 0; idx < ALE_MAX_ENTRIES; idx++)
    {
        CPSWALETableEntryGet(cpsw_config->ale_base, idx, ale_entry);

        /* break if the table entry is free */
        if (((*(((char *)ale_entry) + ALE_ENTRY_TYPE_IDX)) & ALE_ENTRY_TYPE) == ALE_ENTRY_FREE)
        {
            return idx;
        }
    }

    return -1;
}

/**
 * @brief Sets a unicast entry in the ALE table.
 * @param cpsw_config CPSW configuration structure pointer
 * @param port Slave port number
 * @param eth_addr Ethernet address
 */
static void cpsw_ale_unicastentry_set(cpsw_device_t *cpsw_config, uint32_t port, uint8_t *eth_addr)
{

    volatile uint32_t cnt;
    volatile int32_t idx;
    unsigned int ale_entry[ALE_ENTRY_NUM_WORDS] = {0, 0, 0};

    for (cnt = 0; cnt < ALE_ETHARP_HWADDR_LEN; cnt++)
    {
        *(((char *)ale_entry) + cnt) = eth_addr[ALE_ETHARP_HWADDR_LEN - cnt - 1];
    }

    *(((char *)ale_entry) + ALE_UCAST_ENTRY_TYPE) = ALE_ENTRY_UCAST;
    *(((char *)ale_entry) + ALE_UCAST_ENTRY_DLR_PORT_BLK_SEC) = (port << ALE_UCAST_ENTRY_PORT_SHIFT);

    idx = cpsw_ale_entry_match_free(cpsw_config);

    if (idx < ALE_MAX_ENTRIES)
    {
        CPSWALETableEntrySet(cpsw_config->ale_base, idx, ale_entry);
    }
}

/**
 * @brief Sets a multicast entry in the ALE table
 * @param cpsw_config CPSW configuration structure pointer
 * @param portmask Port mask for the port number
 * @param eth_addr Ethernet Address
 */
static void cpsw_ale_multicastentry_set(cpsw_device_t *cpsw_config, uint32_t portmask, uint8_t *eth_addr)
{

    volatile uint32_t cnt;
    volatile int32_t idx;
    unsigned int ale_entry[ALE_ENTRY_NUM_WORDS] = {0, 0, 0};

    idx = cpsw_ale_entry_match_free(cpsw_config);
    if (idx < ALE_MAX_ENTRIES)
    {
        for (cnt = 0; cnt < ALE_ETHARP_HWADDR_LEN; cnt++)
        {
            *(((uint8_t *)ale_entry) + cnt) = eth_addr[ALE_ETHARP_HWADDR_LEN - cnt - 1];
        }

        *(((uint8_t *)ale_entry) + ALE_MCAST_ENTRY_TYPE_FWD_STATE) = ALE_ENTRY_MCAST;
        *(((uint8_t *)ale_entry) + ALE_MCAST_ENTRY_PORTMASK_SUP) |= (portmask << ALE_MCAST_ENTRY_PORTMASK_SHIFT);

        CPSWALETableEntrySet(cpsw_config->ale_base, idx, ale_entry);
    }
}

/**
 * @brief Reclaim TX descriptors already sent
 * @param eth_ctrl Ethernet device configuration
 */
static void cpsw_reclaim_tx_bd(iop_eth_device_t *eth_ctrl)
{

    /* device configuration */
    cpsw_device_t *cpsw_config = (cpsw_device_t *)eth_ctrl->dev.driver;

    /* transmission channel configuration */
    txch *txch = &cpsw_config->txch;

    volatile cpdma_bd_t *send_head = txch->send_head;
    volatile cpdma_bd_t *curr_bd = send_head;

    /* get the start of packet descriptor */
    while ((curr_bd->flags_pktlen & CPDMA_BD_SOP) != 0)
    {

        /* check if transmission is over */
        if ((curr_bd->flags_pktlen & CPDMA_BD_OWNER) != 0)
        {

            iop_debug("     !!! RETRANSMIT !!!\n");

            CPSWCPDMATxHdrDescPtrWrite(cpsw_config->cpdma_base, (uint32_t)curr_bd, 0);
            return;
        }

        /* free descriptor */
        ++txch->free_num;

        /* traverse till the end of the packet and free its descriptors */
        while ((curr_bd->flags_pktlen & CPDMA_BD_EOP) == 0)
        {

            curr_bd = curr_bd->next;
            ++txch->free_num;

            if (txch->free_num == eth_ctrl->tx_count)
            {
                break;
            }
        }

        /* clear SOP and EOP from the descriptors */
        send_head->flags_pktlen &= ~CPDMA_BD_SOP;
        curr_bd->flags_pktlen &= ~CPDMA_BD_EOP;

        /* set the correct free descriptor for the next transmission */
        if (curr_bd->next == NULL)
        {
            txch->send_head = txch->free_head;
        }
        else
        {
            txch->send_head = curr_bd->next;
        }

        /* acknowledge end of writing */
        CPSWCPDMATxCPWrite(cpsw_config->cpdma_base, 0, (uint32_t)curr_bd);

        /* advance in the list of descriptors */
        send_head = txch->send_head;
        curr_bd = send_head;
    }
}

/**
 * @brief Reclaim RX descriptors already sent
 * @param eth_ctrl Ethernet device configuration
 */
static void cpsw_reclaim_rx_bd(iop_eth_device_t *eth_ctrl)
{

    /* device configuration */
    cpsw_device_t *cpsw_config = (cpsw_device_t *)eth_ctrl->dev.driver;

    rxch *rxch = &(cpsw_config->rxch);

    volatile cpdma_bd_t *curr_bd, *last_bd, *recv_tail, *recv_head;
    uint32_t saved_free_num;

    /* Start from the free head of the chain */
    curr_bd = rxch->free_head;

    /* Note down the current positions */
    recv_head = rxch->free_head;
    recv_tail = rxch->recv_tail;
    saved_free_num = rxch->free_num;
    last_bd = rxch->recv_tail;

    while (rxch->free_num > 0)
    {
        curr_bd->bufoff_len = (uint16_t)IOP_BUFFER_SIZE;
        curr_bd->flags_pktlen = CPDMA_BD_OWNER;
        last_bd = curr_bd;
        curr_bd = curr_bd->next;
        --rxch->free_num;
    }

    /* check if any DB is allocated */
    if (saved_free_num == rxch->free_num)
    {
        return;
    }

    rxch->recv_tail = last_bd;

    /**
     * If the entire ring is traversed, curr_bd will be NULL. In that case,
     * write the Rx HDP in order to continue reception
     */
    if (curr_bd != NULL)
    {
        rxch->free_head = curr_bd;
    }
    else
    {
        CPSWCPDMARxHdrDescPtrWrite(cpsw_config->cpdma_base, (uint32_t)recv_head, 0);
    }

    recv_tail->next = recv_head;

    /* close ring */
    last_bd->next = NULL;

    /* check if the reception it over */
    if ((recv_tail->flags_pktlen & CPDMA_BD_EOQ) != 0)
    {
        CPSWCPDMARxHdrDescPtrWrite(cpsw_config->cpdma_base, (uint32_t)recv_head, 0);
    }
}

/**
 * @brief CPDMA initialization
 * @param eth_ctrl Ethernet device configuration
 */
static void cpsw_cpdma_init(iop_eth_device_t *eth_ctrl)
{

    iop_debug("    CPDMA initialization\n");

    cpsw_device_t *cpsw_config = (cpsw_device_t *)eth_ctrl->dev.driver;

    uint32_t idx, count;
    volatile cpdma_bd_t *curr_bd, *last_bd;
    txch *txch;
    rxch *rxch;

    /* initialize the TX channel */
    txch = &cpsw_config->txch;
    txch->free_head = (volatile cpdma_bd_t *)(cpsw_config->cppi_ram_base);
    txch->send_head = txch->free_head;
    txch->send_tail = NULL;

    /* allocate the TX descriptors */
    count = eth_ctrl->tx_count;
    iop_debug("    setting TX buffers descriptors (%i)\n", count);
    if (count == 0 || count > MAX_DESCRIPTORS)
    {
        eth_ctrl->tx_count = count = MAX_DESCRIPTORS;
        iop_debug("    warning: defaulting to %i descriptors...\n", count);
    }

    idx = 0;

    /* create the TX ring of buffer descriptors */
    txch->free_num = count;
    curr_bd = txch->free_head;
    while (count > 0)
    {
        curr_bd->next = curr_bd + 1;
        curr_bd->iop_buffer = &cpsw_config->iop_buffers[idx++];
        curr_bd->bufptr = (uint32_t)curr_bd->iop_buffer->p_addr;
        curr_bd->flags_pktlen = 0;
        last_bd = curr_bd;
        curr_bd = curr_bd->next;
        --count;
    }
    last_bd->next = txch->free_head;

    /* initialize the RC channel */
    rxch = &cpsw_config->rxch;
    rxch->free_head = (volatile cpdma_bd_t *)(cpsw_config->cppi_ram_base + (SIZE_CPPI_RAM >> 1));
    rxch->recv_head = rxch->free_head;
    rxch->recv_tail = NULL;

    /* allocate the RX descriptors */
    count = eth_ctrl->rx_count;
    iop_debug("    setting RX buffers descriptors (%i)\n", count);
    if (count == 0 || count > MAX_DESCRIPTORS)
    {
        eth_ctrl->rx_count = count = MAX_DESCRIPTORS;
        iop_debug("    warning: defaulting to %i descriptors...\n", count);
    }

    /* create the RX ring of buffer descriptors */
    rxch->free_num = count;
    curr_bd = rxch->free_head;
    while (count > 0)
    {
        curr_bd->next = curr_bd + 1;
        curr_bd->iop_buffer = &cpsw_config->iop_buffers[idx++];
        curr_bd->bufptr = (uint32_t)curr_bd->iop_buffer->p_addr;
        curr_bd->bufoff_len = IOP_BUFFER_SIZE;
        curr_bd->flags_pktlen = CPDMA_BD_OWNER;
        last_bd = curr_bd;
        curr_bd = curr_bd->next;
        --count;
    }
    rxch->recv_tail = last_bd;

    /* reclaim all rx descriptors */
    last_bd->next = rxch->free_head;
    rxch->recv_head = rxch->free_head;
    rxch->recv_tail = last_bd;
    cpsw_reclaim_rx_bd(eth_ctrl);

    /* close the ring  */
    last_bd->next = NULL;
    CPSWCPDMARxHdrDescPtrWrite(cpsw_config->cpdma_base, (uint32_t)(rxch->recv_head), 0);
}

/**
 * @brief PHY ability debug
 * @param adv_val Advertised abilities
 * @param partnr_ablty Partner supported abilities
 * @param gbps_partnr_ablty
 */
static void cpsw_ablty_debug(uint32_t adv_val, uint32_t partnr_ablty, uint32_t gbps_partnr_ablty)
{

    if ((gbps_partnr_ablty & PHY_LINK_PARTNER_1000BT_FD) != 0)
    {
        iop_debug("    transfer mode: 1000 Mbps\n");
    }
    else
    {
        if (((adv_val & partnr_ablty) & PHY_100BTX_FD) != 0)
        {
            iop_debug("    transfer mode: 100 Mbps Full Duplex\n");
        }
        else if (((adv_val & partnr_ablty) & PHY_100BTX) != 0)

        {
            iop_debug("    transfer mode: 100 Mbps Half Duplex\n");
        }
        else if (((adv_val & partnr_ablty) & PHY_10BT_FD) != 0)

        {
            iop_debug("    transfer mode: 10 Mbps Full Duplex\n");
        }
        else if (((adv_val & partnr_ablty) & PHY_10BT) != 0)

        {
            iop_debug("    transfer mode: 10 Mbps Half Duplex\n");
        }
        else
        {
            iop_debug("    no Valid Transfer Mode is detected\n");
        }
    }
}

/**
 * @brief
 * @param cpswinst
 * @param port_num
 * @return
 */
static int32_t cpsw_autoneg_config(cpsw_device_t *cpsw_config, uint32_t port)
{

    uint32_t transfer_mode = 0;
    volatile uint32_t aut_neg_cnt = 200;
    uint16_t adv_val, partnr_ablty, gbps_partnr_ablty, gig_adv_val;

    /* we advertise for 10/100 Mbps both half and full duplex */
    adv_val = (PHY_100BTX | PHY_100BTX_FD | PHY_10BT | PHY_10BT_FD);

    /* Advertise only 1Gbps if the PHY can handle it */
    if (1 == cpsw_config->port[port - 1].phy_gbps)
    {
        gig_adv_val = PHY_1000BT_FD;
        partnr_ablty = 1;
        gbps_partnr_ablty = 1;
    }
    else
    {
        gig_adv_val = 0;
        partnr_ablty = 1;
        gbps_partnr_ablty = 0;
    }

    iop_debug("    auto-negotiating on port %i\n", port);

    /* auto-negotiation */
    if (PhyAutoNegotiate(cpsw_config->mdio_base, cpsw_config->port[port - 1].phy_addr, &adv_val, &gig_adv_val) == 1)
    {

        while (aut_neg_cnt)
        {
            /*TODO add wait routine to substitute the following func*/
            //            rtems_task_wake_after(50);
            if (PhyAutoNegStatusGet(cpsw_config->mdio_base, cpsw_config->port[port - 1].phy_addr) == 1)
            {
                break;
            }
            aut_neg_cnt--;
        }

        if (aut_neg_cnt == 0)
        {
            return -1;
        }

        /* get what the partner supports */
        PhyPartnerAbilityGet(cpsw_config->mdio_base, cpsw_config->port[port - 1].phy_addr, &partnr_ablty,
                             &gbps_partnr_ablty);

        cpsw_ablty_debug(adv_val, partnr_ablty, gbps_partnr_ablty);

        /* determine the transfer mode */
        if ((gbps_partnr_ablty & PHY_LINK_PARTNER_1000BT_FD) != 0)
        {
            transfer_mode = CPSW_SLIVER_GIG_FULL_DUPLEX;
        }
        else
        {
            if (((adv_val & partnr_ablty) & PHY_100BTX_FD) != 0)
            {
                transfer_mode = CPSW_SLIVER_NON_GIG_FULL_DUPLEX;
            }
            else if (((adv_val & partnr_ablty) & PHY_100BTX) != 0)

            {
                transfer_mode = CPSW_SLIVER_NON_GIG_HALF_DUPLEX;
            }
            else if (((adv_val & partnr_ablty) & PHY_10BT_FD) != 0)

            {
                transfer_mode = CPSW_SLIVER_INBAND | CPSW_SLIVER_NON_GIG_FULL_DUPLEX;
            }
            else if (((adv_val & partnr_ablty) & PHY_10BT) != 0)

            {
                transfer_mode = CPSW_SLIVER_INBAND | CPSW_SLIVER_NON_GIG_HALF_DUPLEX;
            }
            else
            {
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }

    CPSWSlTransferModeSet(cpsw_config->port[port - 1].sliver_base, transfer_mode);
    return 0;
}

/**
 * @brief
 * @param cpswinst
 * @param slv_port_num
 * @return
 */
static int32_t cpsw_phylink_config(cpsw_device_t *cpswinst, uint32_t slv_port_num)
{

    iop_debug("    configuring PHY link for port %i\n", slv_port_num);

    /* check if ethernet PHY is present or not */
    if (0 == (MDIOPhyAliveStatusGet(cpswinst->mdio_base) & (1 << cpswinst->port[slv_port_num - 1].phy_addr)))
    {

        iop_debug("    ethernet PHY for port %i not found...\n", slv_port_num);
        return -1;
    }

    /* auto-negotiate the PHY configurations */
    if (cpsw_autoneg_config(cpswinst, slv_port_num) == -1)
    {
        iop_debug("    auto-negotiation failed...\n");
        return -1;
    }

    /* check if PHY link is there or not */
    if (PhyLinkStatusGet(cpswinst->mdio_base, cpswinst->port[slv_port_num - 1].phy_addr, 1000) == 0)
    {
        iop_debug("    PHY link for port %i not found...\n", slv_port_num);
        return -1;
    }

    CPSWSlRGMIIEnable(cpswinst->port[slv_port_num - 1].sliver_base);
    return 0;
}

static void cpsw_device_init(iop_eth_device_t *device)
{

    /* initialize MDIO */
    MDIOInit(CPSW0_MDIO_REGS, MDIO_FREQ_INPUT, MDIO_FREQ_OUTPUT);

    /* Select MII, Internal Delay mode */
    HWREG(SOC_CONTROL_REGS + CONTROL_GMII_SEL) = 0x00;

    /* set CPSW pin setup*/
    CPSWPinMuxSetup();
    CPSWClkEnable();

    /* setup ETH 0  */
    cpsw_device_t *cpswinst = device->dev.driver;
    cpsw_setup_eth0(cpswinst);

    /* reset the device modules */
    CPSWSSReset(cpswinst->ss_base);
    CPSWWrReset(cpswinst->wrpr_base);
    CPSWSlReset(cpswinst->port[PORT_1].sliver_base);
    CPSWSlReset(cpswinst->port[PORT_2].sliver_base);
    CPSWCPDMAReset(cpswinst->cpdma_base);

    /* initialize MDIO */
    MDIOInit(cpswinst->mdio_base, MDIO_FREQ_INPUT, MDIO_FREQ_OUTPUT);

    /*
     * Initializes the ALE (Address lookup network, and
     * set the port 0, 1 and 2 states to FORWARD
     */
    CPSWALEInit(cpswinst->ale_base);
    CPSWALEPortStateSet(cpswinst->ale_base, 0, CPSW_ALE_PORT_STATE_FWD);
    CPSWALEPortStateSet(cpswinst->ale_base, 1, CPSW_ALE_PORT_STATE_FWD);
    CPSWALEPortStateSet(cpswinst->ale_base, 2, CPSW_ALE_PORT_STATE_FWD);

    /* setup the multi-cast entry */
    uint8_t multicast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    cpsw_ale_multicastentry_set(cpswinst, PORT_0_MASK | PORT_1_MASK | PORT_2_MASK, multicast_mac);

    /* setup uni-cast entry */
    cpsw_ale_unicastentry_set(cpswinst, 0, device->mac);
    CPSWPortSrcAddrSet(cpswinst->port[0].port_base, device->mac);
    CPSWPortSrcAddrSet(cpswinst->port[1].port_base, device->mac);

    /* setup IOP buffers */
    setup_iop_buffers(cpswinst->iop_buffers, cpswinst->iop_buffers_storage, device->rx_count + device->tx_count);

    /* initialize the buffer descriptors for CPDMA */
    cpsw_cpdma_init(device);

    /* configure PHYs */
    (void)cpsw_phylink_config(cpswinst, 1);

    (void)cpsw_phylink_config(cpswinst, 2);

    iop_debug("    enable transmission and reception\n");

    cpswinst->started = 1;

    /* enable transmission and reception */
    CPSWCPDMATxEnable(cpswinst->cpdma_base);
    CPSWCPDMARxEnable(cpswinst->cpdma_base);
}

static uint16_t cpsw_transmit(cpsw_device_t *cpswinst, iop_wrapper_t *wrapper)
{

    volatile cpdma_bd_t *curr_bd, *bd_to_send, *bd_end;

    /* transmission channel configuration */
    txch *txch = &cpswinst->txch;

    /* check if we have enough descriptors */
    if (txch->free_num == 0)
    {
        return 0;
    }

    /* set minimum length of the CPSW device */
    uint16_t length = get_buffer_size(wrapper->buffer);
    if (length < 60)
    {
        length = 60;
    }

    /* get the free descriptor to transmit */
    curr_bd = txch->free_head;
    bd_to_send = txch->free_head;

    curr_bd->flags_pktlen = length;
    curr_bd->flags_pktlen |= (CPDMA_BD_SOP | CPDMA_BD_OWNER);

    /*
     * In this part a packet would be split among various descriptors,
     * we aren't providing this functionality
     *
     * Begin of packet to descriptor(s) mapping
     */

    /* swap buffers */
    iop_buffer_t *swap = wrapper->buffer;
    wrapper->buffer = (iop_buffer_t *)curr_bd->iop_buffer;
    curr_bd->iop_buffer = (volatile iop_buffer_t *)swap;

    /* set packet pointers and length */
    curr_bd->bufptr = (uint32_t)swap->p_addr;
    curr_bd->bufoff_len = length & CPDMA_BD_LEN_MASK;
    bd_end = curr_bd;

    /* set as start of the packet (and end of packet) */
    curr_bd = curr_bd->next;

    /* consume descriptor */
    --txch->free_num;

    /*
     * End of packet to descriptor(s) mapping
     */

    bd_end->next = NULL;
    bd_end->flags_pktlen |= CPDMA_BD_EOP;
    bd_end->flags_pktlen &= ~CPDMA_BD_EOQ;

    /* update next free descriptor */
    txch->free_head = curr_bd;

    if (txch->send_tail == NULL)
    {
        CPSWCPDMATxHdrDescPtrWrite(cpswinst->cpdma_base, (uint32_t)bd_to_send, 0);
    }
    else
    {

        curr_bd = txch->send_tail;
        curr_bd->next = bd_to_send;

        if ((curr_bd->flags_pktlen & CPDMA_BD_EOQ) != 0)
        {
            CPSWCPDMATxHdrDescPtrWrite(cpswinst->cpdma_base, (uint32_t)bd_to_send, 0);
        }
    }

    txch->send_tail = bd_end;
    return length;
}

static int16_t cpsw_receive(cpsw_device_t *cpswinst, iop_wrapper_t *wrapper)
{

    int32_t len = 0;

    /* get the receive channel */
    rxch *rxch = &cpswinst->rxch;

    /* get the descriptor of the earliest received data */
    volatile cpdma_bd_t *curr_bd = rxch->recv_head;

    /* check if the descriptor contains data */
    if ((curr_bd->flags_pktlen & CPDMA_BD_OWNER) != CPDMA_BD_OWNER)
    {

        /* get the total length of the packet */
        len = (curr_bd->flags_pktlen & CPDMA_BD_PKTLEN_MASK) - 4;

        /* check if the packet is valid */
        if (len > 0)
        {

            /* swap buffers */
            iop_buffer_t *swap = wrapper->buffer;
            wrapper->buffer = (iop_buffer_t *)curr_bd->iop_buffer;
            curr_bd->iop_buffer = (volatile iop_buffer_t *)swap;
            curr_bd->bufptr = (uint32_t)swap->p_addr;

            /* setup offsets */
            wrapper->buffer->header_off = 0;
            wrapper->buffer->header_size = 42;
            wrapper->buffer->payload_off = 42;
            wrapper->buffer->payload_size = len - 42;

            /* no valid packet received */
        }
        else
        {

            len = 0;
        }

        /* free packet to the DMA engine */
        curr_bd->flags_pktlen = CPDMA_BD_OWNER;

        /* acknowledge that this packet is processed */
        CPSWCPDMARxCPWrite(cpswinst->cpdma_base, 0, (uint32_t)curr_bd);

        /* advance to next descriptor */
        curr_bd = curr_bd->next;
        rxch->free_num++;

        /* check if the DMA engine took the last descriptor */
        if (curr_bd == NULL)
        {
            rxch->recv_head = rxch->free_head;
            return (int16_t)len;
        }
        rxch->recv_head = curr_bd;
    }

    return (int16_t)len;
}

air_status_code_e cpsw_initialize(iop_device_driver_t *iop_dev, void *arg)
{

    cpsw_device_init((iop_eth_device_t *)iop_dev);
    return AIR_SUCCESSFUL;
}

air_status_code_e cpsw_open(iop_device_driver_t *iop_dev, void *arg)
{

    return AIR_SUCCESSFUL;
}

air_status_code_e cpsw_close(iop_device_driver_t *iop_dev, void *arg)
{

    return AIR_SUCCESSFUL;
}

air_status_code_e cpsw_read(iop_device_driver_t *iop_dev, void *arg)
{

    air_status_code_e rc = AIR_SUCCESSFUL;
    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    cpsw_device_t *cpswinst = (cpsw_device_t *)device->dev.driver;

    /* check if driver was initialized */
    if (!cpswinst->started)
    {
        rc = AIR_DEVICE_NOT_FOUND;
    }

    /* sanity check */
    if (wrapper == NULL || wrapper->buffer == NULL)
    {
        rc = AIR_INVALID_PARAM;
    }

    /* read packet */
    if (rc == AIR_SUCCESSFUL && cpsw_receive(cpswinst, wrapper) == 0)
    {
        rc = AIR_NO_ACTION;
    }

    /* free RX descriptors */
    if (rc != AIR_DEVICE_NOT_FOUND)
    {
        cpsw_reclaim_rx_bd(device);
    }

    return rc;
}

air_status_code_e cpsw_write(iop_device_driver_t *iop_dev, void *arg)
{

    air_status_code_e rc = AIR_SUCCESSFUL;
    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    cpsw_device_t *cpswinst = (cpsw_device_t *)device->dev.driver;

    if (!cpswinst->started)
    {
        rc = AIR_DEVICE_NOT_FOUND;
    }

    /* Verify user arguments consistency*/
    if (wrapper == NULL || wrapper->buffer == NULL || wrapper->buffer->payload_size == 0)
    {
        rc = AIR_INVALID_PARAM;
    }

    /* send packet */
    if (rc == AIR_SUCCESSFUL && cpsw_transmit(cpswinst, wrapper) == 0)
    {
        rc = AIR_NO_ACTION;
    }

    /* free TX descriptors */
    if (rc != AIR_DEVICE_NOT_FOUND)
    {
        cpsw_reclaim_tx_bd(device);
    }

    return rc;
}

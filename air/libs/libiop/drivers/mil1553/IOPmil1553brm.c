/*
 * BRM driver
 *
 * COPYRIGHT (c) 2006  Gaisler Research
 * Copyright (C) 2011-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE.RTEMS in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
/** @file
 *  @brief This file contains the MIL-STD-1553 A/B driver for LEON3.
 *
 *  @author Cl�udio Silva
 *
 */

/**
 * @brief basic bus/interface of device,
 * Default to direct accessed AMBA bus.
 */
#ifndef MIL1553BRM_NO_AMBA
#define MIL1553BRM_AMBA
#undef MIL1553BRM_PCI
#endif

/** Address Translation: default to no translation */
#ifndef MIL1553BRM_ADR_TO
#define memarea_to_hw(x) ((unsigned int)(x))
#endif

/** default to 128K memory layout */
#if !defined(DMA_MEM_16K)
#define DMA_MEM_128K
#endif

#include <iop.h>
#include <bsp.h>
#include <iop_error.h>
#include <iop_support.h>
#include <gr1553_support.h>
#include <IOPmil1553brm.h>

/* Uncomment for debug output */
/*#define DEBUG 1
#define FUNCDEBUG 1*/
#undef DEBUG
#undef FUNCDEBUG

/*
 * EVENT_QUEUE_SIZE sets the size of the event queue
 */
#define EVENT_QUEUE_SIZE 1024

#define INDEX(x) (x & (EVENT_QUEUE_SIZE - 1))

#ifdef DEBUG
#define DBG(x...) pprintf(x)
#else
#define DBG(x...)
#endif

#ifdef FUNCDEBUG
#define FUNCDBG(x...) pprintf(x)
#else
#define FUNCDBG(x...)
#endif

#define READ_REG(address) _BRM_REG_READ16((unsigned int)address)
#define READ_DMA(address) _BRM_REG_READ16((unsigned int)address)
static __inline__ unsigned short _BRM_REG_READ16(unsigned int addr)
{
    unsigned short tmp;
    __asm__ __volatile__(" lduh [%1], %0 " : "=r"(tmp) : "r"(addr));
    return tmp;
}

static void check_errors(brm_priv *brm);

void create_write_cmd_shortcut(iop_chain_control *rtaddress_chain, bc_command_t *ul, void *desc);
void *find_shortcut(iop_chain_control *rtaddress_chain, milstd_header_t *hdr, unsigned int size);

/** Calculates the offset to the previous 128Kb aligned address based on the pointer*/
#define OFS(ofs) (((unsigned int)&ofs & 0x1ffff) >> 1)

/**
 * @brief Calculates the parity of an integrer
 *
 * @param [in] data Integrer whose partity is to be verified
 * @return
 *   - 1 if data has a odd parity
 *   - 0 Otherwise
 */
static int odd_parity(unsigned int data)
{
    unsigned int i = 0;

    while (data)
    {
        i++;
        data &= (data - 1);
    }

    return !(i & 1);
}

/**
 * @brief Starts the core operations
 *
 * @param [in] brm internal structure of the device to be initialized
 */
static void start_operation(brm_priv *brm)
{
    unsigned int ctrl = READ_REG(&brm->regs->ctrl);
    brm->regs->ctrl = ctrl | 0x8000;
}

/**
 * @brief Start BC list processing
 *
 * @param [in] brm internal device structure
 */
void brm_bc_start_list(brm_priv *brm)
{
    unsigned int ctrl = READ_REG(&brm->regs->ctrl);
    brm->regs->ctrl = ctrl | 0x8000;
}

/**
 * @brief Stops a BRM core from executing
 *
 * @param [in] brm internal structure of the device to be stoped
 */
static void stop_operation(brm_priv *brm)
{
    unsigned int ctrl = READ_REG(&brm->regs->ctrl);
    brm->regs->ctrl = ctrl & ~0x8000;
}

/**
 * @brief Stops BC list processing
 *
 * @param [in] brm internal device structure
 */
void brm_bc_stop_list(brm_priv *brm)
{
    unsigned int ctrl = READ_REG(&brm->regs->ctrl);

    brm->regs->ctrl = ctrl & ~0x8000;
}

/**
 * @brief Verifies if a milstd device is executing
 *
 * @param [in] brm internal structure of the device checked
 */
static int is_executing(brm_priv *brm)
{
    unsigned int ctrl = READ_REG(&brm->regs->ctrl);
    return ((ctrl >> 15) & 1);
}

/**
 * @brief Validates the user configuration
 *
 * @param [in] config pointer to the user configuration
 */
static void validate_config(milstd_config_t *config)
{

    /*Available operative modes*/
    if ((config->mode != BRM_MODE_BC) && (config->mode != BRM_MODE_RT) && (config->mode != BRM_MODE_BM))
    {

        /*Default to RT*/
        config->mode = BRM_MODE_RT;
    }

    /*Brodcast Activated?*/
    if ((config->broadcast != 1) && (config->broadcast != 0))
    {

        /*Default do Disabled*/
        config->broadcast = 0;
    }

    /*Evaluate specific RT options*/
    if (config->mode == BRM_MODE_RT)
    {

        /*Options: Enabled or Disabled*/
        if ((config->ignore_mode_data != 1) && (config->ignore_mode_data != 0))
        {

            /*Default to ignore mode data activated*/
            config->ignore_mode_data = 1;
        }

        /*Options: None: 0; A = 1; B = 2; A & B = 3 */
        if ((config->bus > 3) != 0)
        {

            /*Default to both buses A and B activated*/
            config->bus = 3;
        }

        /*Options: 0 to 31*/
        if ((config->rt_address > 31) != 0)
        {

            /*Default to 31*/
            config->rt_address = 31;
        }

        /*If broadcast is active, RT address 31 is reserved*/
        if ((config->rt_address == 31) && (config->broadcast))
        {

            /*Default to 30*/
            config->rt_address = 30;
        }

/*If we have memory limitations*/
#if defined(DMA_MEM_16K)

        /*Default number of buffers per address to 2, so we can stay inside 16Kb*/
        config->rt_buf_number = 2;

/*We don't have memory constrains. Occupy all 128Kb*/
#else

        /*If ignore_mode_data is active we can have a higher number of buffers*/
        if (config->ignore_mode_data == 1)
        {

            /*Check limits*/
            if (((config->rt_buf_number > 28) || (config->rt_buf_number < 1)) != 0)
            {

                /*Default to the maximum possible*/
                config->rt_buf_number = 28;
            }
            /*We have to distribute memory evenly, so the limit is lower*/
        }
        else
        {

            /*Check limits*/
            if (((config->rt_buf_number > 14) || (config->rt_buf_number < 1)) != 0)
            {

                /*Default to the maximum possible*/
                config->rt_buf_number = 14;
            }
        }
#endif /*DMA_MEM_16K*/
    }

    /*Options specific for BM and BC*/
    if ((config->mode == BRM_MODE_BC) || (config->mode == BRM_MODE_BM))
    {

        /*Timeout: Enabled or Disabled*/
        if ((config->msg_timeout != 1) && (config->msg_timeout != 0))
        {

            /*Default to enabled*/
            config->msg_timeout = 1;
        }
    }

    /*BC specfic options*/
    if (config->mode == BRM_MODE_BC)
    {

        /*Check limits. Maximum is 1630 due to the 128Kb constrain*/
        if (((config->bc_block_number > 1630) || (config->bc_block_number < 1)) != 0)
        {

            /*Default to the maximum possible*/
            config->bc_block_number = 1630;
        }
    }

    /*BC specfic options*/
    if (config->mode == BRM_MODE_BM)
    {

        /*Check limits. Maximum is 1630 due to the 128Kb constrain*/
        if (((config->bm_block_number > 1630) || (config->bm_block_number < 1)) != 0)
        {

            /*Default to the maximum possible*/
            config->bm_block_number = 1630;
        }
    }

    /*Block or not to block? (on tx)*/
    if ((config->tx_blocking != 1) && (config->tx_blocking != 0))
    {

        /*Default to blocking activated*/
        config->tx_blocking = 1;
    }

    /*Block or not to block? (on rx)*/
    if ((config->rx_blocking != 1) && (config->rx_blocking != 0))
    {

        /*Default to blocking activated*/
        config->rx_blocking = 1;
    }

    /*Clock frequency options: 0=12MHz, 1=16MHz, 2= 20MHz, 3=24MHz*/
    if ((config->brm_freq != 1) && (config->brm_freq != 0) && (config->brm_freq != 2) && (config->brm_freq != 3))
    {

        /*Default to 24Mhz*/
        config->brm_freq = 3;
    }

    config->brm_freq &= BRM_FREQ_MASK;
    /*TODO:*/
    config->clksel &= CLKSEL_MASK;
    config->clkdiv &= CLKDIV_MASK;
}

/**
 * @brief Clears the IRQ log list that is maintained by the HW
 *
 * @param [in] logs pointer to the memory zone containing the list
 *
 * This function iterates over the IRQ log and sets the iiw to all ones
 * and the iaw to 0.
 */
static void clr_int_logs(struct irq_log_list *logs)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        logs[i].iiw = 0xffff;
        logs[i].iaw = 0x0;
    }
}

/**
 * @brief initializes the HW in Remote Terminal mode
 *
 * @param [in] brm internal structure of the device to be initialized
 */
static air_status_code_e rt_init(brm_priv *brm)
{
    unsigned int i, base, buf_count, offset = 0;
    unsigned short ctrl, oper;

    /*Configure blocking behavior*/
    brm->rx_blocking = brm->config->rx_blocking;
    brm->tx_blocking = brm->config->tx_blocking;

    /*Null memory pointer from other modes*/
    brm->bcmem = NULL;

    /*Store memory area as BCMEM*/
    brm->rtmem = (void *)brm->mem;

/*Calculate offset to IRQ log based on memory constrains*/
#if defined(DMA_MEM_128K)
    /*last 64 bytes of 128kb*/
    offset = 64 * 1024 - 32;
#elif defined(DMA_MEM_16K)
    /*last 64 bytes of 16kb*/
    offset = 8 * 1024 - 32;
#endif

    /*Store IRQ log list pointer in private structure*/
    brm->irq_log = (struct irq_log_list *)&brm->mem[offset];

    /* In this section we setup the device's registers
     * enable both buses, circular 1 bufmode, broadcast, interrupt log */
    brm->regs->ctrl = 0x1912;

    /* configure as RT, with addr 1 */
    brm->regs->oper = 0x0900;

    /*Choose what events are stored in the IRQ log*/
    brm->regs->imask = BRM_RT_ILLCMD_IRQ | BRM_TAPF_IRQ | BRM_DMAF_IRQ | BRM_WRAPF_IRQ | BRM_MERR_IRQ;

    /*TODO:*/
    brm->regs->dpoint = 0;

    /*Pointer to the beggining of the IRQ log based on offset*/
    brm->regs->ipoint = offset;

    /* BRM clocked with freq = 12,16,20 or 24MHz */
    brm->regs->enhanced = 0x0000 | brm->config->brm_freq;

    /*Write CLKSEL and CLKDIV*/
    brm->regs->w_ctrl = (brm->config->clksel << 9) | (brm->config->clkdiv << 5) | 1;

    /*Disable all interrupts*/
    brm->regs->w_irqctrl = 0;

    /*Insert memory base address, so HW can access the table based on offset*/
    brm->regs->w_ahbaddr = (unsigned int)memarea_to_hw(brm->memarea_base);

    /*Clear IRQ logs*/
    clr_int_logs(brm->irq_log);

    /* Legalize all commands: iterate all legalize registers */
    for (i = 0; i < 16; i++)
    {

        /*Zero out*/
        brm->regs->rt_cmd_leg[i] = 0;
    }

    /*Set active bus and broadcast mode
     * read control register*/
    ctrl = READ_REG(&brm->regs->ctrl);

    /* Clear bit 12-11 ...      */
    ctrl &= 0xE7EF;

    /* ... OR in new broad cast enable*/
    ctrl |= ((brm->config->broadcast) & 1) << 4;

    /* ... OR in new bus status */
    ctrl |= ((brm->config->bus) & 0x3) << 11;

    /*write control register*/
    brm->regs->ctrl = ctrl;

    /*Set RT address*/
    oper = READ_REG(&brm->regs->oper);

    /* Clear bit 15-10 ...      */
    oper &= 0x03FF;

    /* ... OR in new rt address    */
    oper |= ((brm->config->rt_address) & 0x1f) << 11;

    /* ... OR in parity  */
    oper |= odd_parity((brm->config->rt_address) & 0x1f) << 10;

    /*... Write oper register*/
    brm->regs->oper = oper;

    /*Obtain buf count from configuration*/
    buf_count = brm->config->rt_buf_number;

    /*There are 128 descritors, thus the desc table ends at offset (128*4 words each) = 512*/
    base = 512;

    /**
     * @note Init descriptor table
     * Each circular buffer has room for '$buf_count" messages composed of 34 (32 data + miw + time) words (16b) in
     * each. The buffers must separated by 34 words. Due to the 128 kB limit on the addressable memory and if we split
     * the space evenly, buf_count will be limited to 14. If ignore_mode_data is set then the space will only be
     * distributed for the rx and tx subaddress increasing max buf count to 28 For safety reasons mode code addresses
     * will still have a buffer of one message. Each message has 34 words, thus the buffer memory of each descriptor
     * will be separated by &buf_count*34
     */

    /*Iterate over first 32 subaddresses (RX)*/
    for (i = 0; i < 32; i++)
    {

        /* Activate Interrupts: INTX, IWA, and IBRD */
        brm->rtmem->rxsubs[i].ctrl = 0x0060;

        /*Calculate Top address */
        brm->rtmem->rxsubs[i].top = base + i * buf_count * 34;

        /* Current address: equals top address*/
        brm->rtmem->rxsubs[i].cur = base + i * buf_count * 34;

        /* Calculate Bottom address */
        brm->rtmem->rxsubs[i].bot = base + (i + 1) * buf_count * 34 - 34;

        /*Start reading at the beggining of the buffer*/
        brm->last_read[i] = base + i * buf_count * 34;
    }

    /*New base. This is where the previous buffer area ends (i=32)*/
    base = base + 32 * 34 * buf_count;

    /* Iterate over TX Sub-addresses 0 - 31 */
    for (i = 0; i < 32; i++)
    {

        /* Activate Interrupt: IWA and IBRD */
        brm->rtmem->txsubs[i].ctrl = 0x0060;

        /*Calculate Top address */
        brm->rtmem->txsubs[i].top = base + i * buf_count * 34;

        /* Current address: equals top address*/
        brm->rtmem->txsubs[i].cur = base + i * buf_count * 34;

        /* Calculate Bottom address */
        brm->rtmem->txsubs[i].bot = base + (i + 1) * buf_count * 34 - 34;

        /*Start reading at the beggining of the buffer*/
        brm->last_read[i + 32] = base + i * buf_count * 34;

        /*Start writing at the beggining of the buffer*/
        brm->written[i] = base + i * buf_count * 34;
    }

    /*If ignore_mode_data is activated we focused all memory on previous buffers*/
    if (brm->config->ignore_mode_data == 1)
    {
        buf_count = 1;
    }

    /*New base. This is where the previous buffer area ends (i=32)*/
    base = base + 32 * 34 * buf_count;

    /* Iterate over RX mode code 0 - 31 */
    for (i = 0; i < 32; i++)
    {

        /* Activate Interrupt: INTX, IWA, and IBRD */
        brm->rtmem->rxmodes[i].ctrl = 0x0060;

        /*Calculate Top address */
        brm->rtmem->rxmodes[i].top = base + i * buf_count * 34;

        /* Current address: equals top address*/
        brm->rtmem->rxmodes[i].cur = base + i * buf_count * 34;

        /* Calculate Bottom address */
        brm->rtmem->rxmodes[i].bot = base + (i + 1) * buf_count * 34 - 34;

        /*Start reading at the beggining of the buffer*/
        brm->last_read[i + 64] = base + i * buf_count * 34;
    }

    /*New base. This is where the previous buffer area ends (i=32)*/
    base = base + 32 * 34 * buf_count;

/*Do we have memory contrains?*/
#if defined(DMA_MEM_16K)
    /*We need this so we can stay in the 16KB frontier*/
    buf_count = 1;
#endif

    /* Iterate over TX mode code 0 - 31 */
    for (i = 0; i < 32; i++)
    {

        /* Activate Interrupt: IWA and IBRD */
        brm->rtmem->txmodes[i].ctrl = 0x0060;

        /*Calculate Top address */
        brm->rtmem->txmodes[i].top = base + i * buf_count * 34;

        /* Current address: equals top address*/
        brm->rtmem->txmodes[i].cur = base + i * buf_count * 34;

        /* Calculate Bottom address */
        brm->rtmem->txmodes[i].bot = base + (i + 1) * buf_count * 34 - 34;

        /*Start reading at the beggining of the buffer*/
        brm->last_read[i + 96] = base + i * buf_count * 34;
    }

    /*Store current operating mode*/
    brm->mode = BRM_MODE_RT;

    return AIR_SUCCESSFUL;
}

/**
 * @brief initializes the HW in Bus Controller mode
 *
 * @param [in] brm internal structure of the device to be initialized
 */
static air_status_code_e bc_init(brm_priv *brm)
{

    /*Control Register Contents*/
    unsigned short ctrl;

    /*Offset to the beggining of IRQ log*/
    unsigned int offset;

    /* generic iterator */
    int j;

    /*Store memory area as BCMEM*/
    brm->bcmem = (void *)brm->mem;

    /*Cast memory to BC descriptor*/
    brm->bcmem->descs = (struct cb *)brm->mem;

    /*Copy configuration defined block number*/
    brm->bc_block_number = brm->config->bc_block_number;

    /*Null memory pointer from other modes*/
    brm->rtmem = NULL;

    /*
     * Do we have memory limitations? 204 is the number of blocks inside
     * 16 kb region:(8*1024-32)/(32+8)
     */
    if (brm->bc_block_number <= 204)
    {

        /*offset to the end of the 16k block*/
        offset = 8 * 1024 - 32;

        /*We are allowed to use all memory*/
    }
    else
    {

        /*offset to end of the 128kb block*/
        offset = 64 * 1024 - 32;
    }

    /*Store IRQ log list pointer in private strcuture*/
    brm->irq_log = (struct irq_log_list *)&brm->mem[offset];

    /*Blocking options*/
    brm->rx_blocking = brm->config->rx_blocking;
    brm->tx_blocking = brm->config->tx_blocking;

    /* Let's setup the device's hardware for operating has a BM!
     * In this section we setup the device's registers
     * ping pong enable and (enable interrupt log?) */
    brm->regs->ctrl = 0x0006;

    /* configure as BC */
    brm->regs->oper = 0x0800; /*8????*/

    /*Choose what events are stored in the IRQ log*/
    brm->regs->imask = BRM_BC_ILLCMD_IRQ | BRM_ILLOP_IRQ | BRM_DMAF_IRQ | BRM_WRAPF_IRQ | BRM_MERR_IRQ;

    /*No data is stored in this mode, so no pointer*/
    brm->regs->dpoint = 0;

    /*Pointer to the beggining of the IRQ log based on offset*/
    brm->regs->ipoint = OFS(brm->mem[offset]);

    /* select freq based on configuration option */
    brm->regs->enhanced = 0x0000 | (brm->config->brm_freq & 0x3);

    /*Write CLKSEL and CLKDIV*/
    brm->regs->w_ctrl = (brm->config->clksel << 9) | (brm->config->clkdiv << 5) | 1;

    /*Disable Interrupts*/
    brm->regs->w_irqctrl = 0;

    /*Insert memory base address, so HW can access the table based on offset*/
    brm->regs->w_ahbaddr = (unsigned int)memarea_to_hw(brm->memarea_base);

    /* Set Broadcast Mode and messager timeout
     * read control register*/
    ctrl = READ_REG(&brm->regs->ctrl);

    /* Clear bit 4 ...           */
    ctrl &= 0xFDEF;

    /* ... OR in new Broadcast enable*/
    ctrl |= ((brm->config->broadcast) & 1) << 4;

    /* ... OR in new MSG timeout     */
    ctrl |= ((brm->config->msg_timeout) & 1) << 9;

    /*... write control register with new data*/
    brm->regs->ctrl = ctrl;

    /*Clear IRQ logs*/
    clr_int_logs(brm->irq_log);

    /* iterate over all shortcut chains */
    for (j = 0; j < 32; j++)
    {

        /*  initialize them as empty */
        iop_chain_initialize_empty(&brm->shortcut[j]);
    }

    /*Store new operative mode*/
    brm->mode = BRM_MODE_BC;

    return AIR_SUCCESSFUL;
}

/**
 * @brief initializes the HW in Bus Monitor mode
 *
 * @param [in] *brm internal structure of the device to be initialized
 */
static air_status_code_e bm_init(brm_priv *brm)
{
    unsigned short ctrl;
    unsigned int offset;

    /*Rx blocking behavior*/
    brm->rx_blocking = brm->config->rx_blocking;

    /*Tx blocking behavior*/
    brm->tx_blocking = brm->config->tx_blocking;

    /*Copy number of BM command blocks*/
    brm->bm_block_number = brm->config->bm_block_number;

    /*NULL other modes memory pointers*/
    brm->bcmem = NULL;
    brm->rtmem = NULL;

    /* Do we have memory limitations? 204 is the number of blocks inside
     * 16 kb region:(8*1024-32)/(32+8)
     */
    if (brm->bm_block_number <= 204)
    {

        /*offset to the end of the 16k block*/
        offset = 8 * 1024 - 32;

        /*We are allowed to use all memory*/
    }
    else
    {

        /*offset to the end of the 128kb block*/
        offset = 64 * 1024 - 32;
    }

    brm->bc_last_read = 0;

    /*Pointer to the beggining of the IRQ log based on offset*/
    brm->irq_log = (struct irq_log_list *)&brm->mem[offset];

    /* Let's setup the device's hardware for operating has a BM!
     * In this section we setup the device's registers
     * ping pong enable and enable interrupt log */
    brm->regs->ctrl = 0x0006;

    /* configure as BM */
    brm->regs->oper = 0x0A00;

    /*Choose what events are stored in the IRQ log*/
    brm->regs->imask = BRM_MBC_IRQ | BRM_MERR_IRQ | BRM_DMAF_IRQ | BRM_MERR_IRQ;

    /*No data is stored in this mode, so no pointer*/
    brm->regs->dpoint = 0;

    /*Pointer to the beggining of the IRQ log based on offset*/
    brm->regs->ipoint = OFS(brm->mem[offset]);

    /* Command pointer: memory area start. Offset 0 */
    brm->regs->mcpoint = 0;

    /* Data pointer: memory area end. offset 64K */
    brm->regs->mdpoint = OFS(brm->mem[(brm->bm_block_number) * 8]);

    /* Block count */
    brm->regs->mbc = (brm->bm_block_number - 1);

    /* select freq based on configuration option */
    brm->regs->enhanced = 0x0000 | (brm->config->brm_freq & 0x3);

    /*Write CLKSEL and CLKDIV*/
    brm->regs->w_ctrl = (brm->config->clksel << 9) | (brm->config->clkdiv << 5) | 1;

    /*Disable Interrupts*/
    brm->regs->w_irqctrl = 0;

    /*Insert memory base address, so HW can access the table based on offset*/
    brm->regs->w_ahbaddr = (unsigned int)memarea_to_hw(brm->memarea_base);

    /*Start reading at the beggining of the Command table*/
    brm->bm_last_read = 0;

    /* Set Broadcast Mode and messager timeout
     * read control register*/
    ctrl = READ_REG(&brm->regs->ctrl);

    /* Clear bit 4 ...           */
    ctrl &= 0xFDEF;

    /* ... OR in new BroadCast Enable*/
    ctrl |= ((brm->config->broadcast) & 1) << 4;

    /* ... OR in new MSG TimeOut*/
    ctrl |= ((brm->config->msg_timeout) & 1) << 9;

    /*.. Write control register*/
    brm->regs->ctrl = ctrl;

    /*Clear interrupt logs*/
    clr_int_logs(brm->irq_log);

    /*Store current mode on private*/
    brm->mode = BRM_MODE_BM;

    return AIR_SUCCESSFUL;
}

/**
 *  \brief Initializes the BRM cores
 *
 *  \param [in]  iop_dev : pointer to iop device driver
 *  \param [in]  arg :
 *
 *  \return Status of the operation:
 *	   - AIR_INVALID_PARAM BRM device not found
 *     - AIR_OUT_OF_MEMORY not possible to allocate resource
 *     - AIR_SUCCESSFUL operation completed successfully
 *
 **/
uint32_t brm_initialize(iop_device_driver_t *iop_dev, void *arg)
{

    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    brm_priv *brm = (brm_priv *)(device->dev.driver);

    /* AMBA AHB device*/
    amba_ahb_dev_t ambadev;

    unsigned int allbrm_memarea;

    /* Device's memory area*/
    char *mem;

    FUNCDBG("brm_initialize\n");

/* This is here for compatibility with PCI_RASTA*/
#ifdef MIL1553BRM_LOCAL_MEM
    allbrm_memarea = MIL1553BRM_LOCAL_MEM_ADR;
#else
    allbrm_memarea = 0;
#endif

    /* Amba bus defining structure*/
    amba_confarea_t *amba_bus = (amba_confarea_t *)air_syscall_get_ambaconf();
    ;

    /* Validate user configuration*/
    validate_config(brm->config);

    /* Allocate memory for all device's descriptors*/
    if ((allbrm_memarea) != 0)
    {

        /* Static mem. Maintained here for compatibility */
        mem = (char *)allbrm_memarea;
    }
    else
    {

        /*get driver memory.128k per core + 128k for alignment*/
        mem = (char *)brm->mem;

        /* align memory to 128k boundary */
        mem = (char *)(((unsigned int)mem + 0x1ffff) & ~0x1ffff);
    }

    /* clear the used memory */
    (void)memset(mem, 0, (128 * 1024));

    /* Get AMBA AHB device info from Plug&Play */
    if (amba_get_ahb_slave(amba_bus, VENDOR_GAISLER, GAISLER_B1553BRM, 0, &ambadev) == 0)
    {

        /* Device not found */
        return AIR_INVALID_PARAM;
    }

    /* Copy pointer to device's memory mapped registers */
    brm->regs = (void *)ambadev.start[0];

    /* Copy IRQ*/
    brm->irqno = ambadev.irq;

    /* Copy device's minor*/
    brm->minor = 0;

    /* Clear IRQ since it is not used*/
    brm->irq = 0;

    DBG("Registering BRM core at [0x%x] irq %d, minor %d\n", brm->regs, brm->irqno, minor);
#if 0
	/* Device Semaphore created with count = 1 */
if(( rtems_semaphore_create(rtems_build_name('B', 'M', 'D', '0'+minor) != 0),
				1,
				RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
				0,
				&brm->dev_sem) != AIR_SUCCESSFUL ){
		pprintf("BRM: Failed to create device semaphore\n");
		return AIR_OUT_OF_MEMORY;
	}
#endif

    /* Set base address of the device's memory */
    brm->memarea_base = (unsigned int)&mem[(128 * 1024)];

    /* descriptor table's base address*/
    brm->desc = (struct desc_table *)brm->memarea_base;

    /* Another pointer for the memory area*/
    brm->mem = (volatile unsigned short *)brm->memarea_base;

    /* last 64byte of the memory area for the IRQ log */
    brm->irq_log = (struct irq_log_list *)(brm->memarea_base + (0xFFE0 << 1));

    /* Sel HW clock so that we can write to BRM's registers */
    brm->regs->w_ctrl = (brm->config->clksel << 9) | (brm->config->clkdiv << 5);

    /* Reset BRM core */
    brm->regs->w_ctrl = 1 << 10 | READ_REG(&brm->regs->w_ctrl);

    /* Init device based on choosen mode*/
    switch (brm->config->mode)
    {

    /*Bus Controler*/
    case BRM_MODE_BC:
        bc_init(brm);
        break;

    /*Remote Terminal*/
    case BRM_MODE_RT:
        rt_init(brm);
        break;

    /*Bus Monitor*/
    case BRM_MODE_BM:
        bm_init(brm);
        break;

    /*Default to remote terminal*/
    default:
        rt_init(brm);
        break;

        DBG("BRM: LOG: 0x%lx, 0x%lx\n\r", brm->log, brm);
    }

    DBG("BRM initialisation done.\n");

    return AIR_SUCCESSFUL;
}

/**
 *  \brief prepares the device for communication
 *
 *  \param [in]  brm : driver internal struct
 *
 *  \return Status of the operation:
 *		- AIR_NOT_AVAILABLE if the device is already open
 *		- AIR_SUCCESSFUL if the operation completed sucessfully
 *
 **/
uint32_t brm_open(brm_priv *brm)
{

    FUNCDBG("brm_open\n");

#if 0
	/*obtain device semaphore*/
    if (rtems_semaphore_obtain(brm->dev_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT) != AIR_SUCCESSFUL) {
        DBG("brm_open: resource in use\n");
		
		/* EBUSY */
        return AIR_NOT_AVAILABLE; 
    }
#endif
    /* Set defaults */
    brm->event_id = brm->config->event_id;

    /*start core operation*/
    start_operation(brm);

    return AIR_SUCCESSFUL;
}

/**
 *  \brief stops the device from communicating
 *
 *  \param [in]  brm : driver internal struct
 *
 *  \return Status of the operation:
 *		- AIR_SUCCESSFUL if the operation completed sucessfully
 *
 **/
uint32_t brm_close(brm_priv *brm)
{

    FUNCDBG("brm_close");

    /*stop operation*/
    stop_operation(brm);

    /*release device's semaphore*/
    //	rtems_semaphore_release(brm->dev_sem);

    return AIR_SUCCESSFUL;
}

/**
 *  \brief reads data from the BRM core
 *
 * @param [in] iop_dev iop 1553 support struct
 * @param [out] arg pointer to iop wrapper
 *
 *  \return Status of the operation:
 *		- AIR_SUCCESSFUL if the operation completed sucessfully
 *		- AIR_INVALID_CONFIG: Operation not permited in BC mode
 *		- AIR_NOT_AVAILABLE: There is no data to be read and we can't block
 *
 **/
uint32_t brm_read(iop_device_driver_t *iop_dev, void *arg)
{

    unsigned short descriptor, current, miw, tmp, offset;
    unsigned short wc = 0;
    unsigned short msgadr = 0;
    unsigned short *data = NULL;
    air_status_code_e status = AIR_SUCCESSFUL;
    int len;
    int count = 0, read = 0;

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;

    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    brm_priv *brm = (brm_priv *)(device->dev.driver);

    /*We can only "read" in RT or BM mode*/
    if (!(brm->mode & (BRM_MODE_RT | BRM_MODE_BM)))
    {
        return AIR_INVALID_CONFIG;
    }

    /* Cast to unsigned short so we can index the array */
    data = (unsigned short *)get_payload(wrapper->buffer);

    /* user buffer where to copy the data in BM mode*/
    struct bm_msg *bm_block = (struct bm_msg *)get_payload(wrapper->buffer);

    /* Check for any errors*/
    check_errors(brm);

    /* Check if we are a RT */
    if (((READ_REG(&brm->regs->oper) >> 8) & 3) == 1)
    {

        /* If we can block, wait until we have read something*/
        while (!read)
        {

            /* Iterate all 128 descriptors searching for data to read*/
            for (descriptor = 0; (descriptor < 128 && !read); descriptor++)
            {

                /* Search in the descriptor table for an accessed descriptor*/
                if ((READ_REG(&brm->desc[descriptor].ctrl) & 0x10) != 0)
                {

                    /* This Descriptor was acessed*/
                    current = READ_DMA(&brm->desc[descriptor].cur);

                    /* Check for Buffer overlap: There was a Wrap in the buffer and current > last_read*/
                    if ((((tmp = READ_REG(&brm->desc[descriptor].ctrl) != 0)) & 0x80) &&
                        (current > brm->last_read[descriptor]))
                    {

                        /* There was an overlap. start to read from the beggining*/
                        brm->last_read[descriptor] = brm->desc[descriptor].top;

                        /*Clear overlap bit*/
                        brm->desc[descriptor].ctrl = tmp & ~0x80;
                    }

                    /* Circular buffer start position */
                    msgadr = brm->last_read[descriptor];

                    /*Read message information word*/
                    miw = READ_DMA(&brm->mem[msgadr]);

                    /* Get word count */
                    wc = miw >> 11;

                    /* Data received */
                    if (descriptor < 32)
                    {

                        /* If word count is zero it means 32 words*/
                        wc = wc ? wc : 32;
                    }

                    /* Data transmitted */
                    else if (descriptor < 64)
                    {

                        /* If word count is zero it means 32 words*/
                        wc = wc ? wc : 32;
                    }

                    /* RX Mode code */
                    else if (descriptor < 96)
                    {
                        wc = (wc >> 4);
                    }

                    /* TX Mode code */
                    else if (descriptor < 128)
                    {
                        wc = (wc >> 4);
                    }

                    /*Copy msg to user buffer
                     * copy MIW */
                    wrapper->buffer->payload_size = wc * 2 + 2;

                    /* Copy Time Tag*/
                    data[wc] = READ_DMA(&brm->mem[msgadr + 1]);

                    /* Copy data up to word count*/
                    len = wc;

                    while (len-- > 0)
                    {

                        /*copy data*/
                        data[len] = READ_DMA(&brm->mem[msgadr + 2 + len]);
                    }

                    milstd_header_t *hdr = (milstd_header_t *)get_header(wrapper->buffer);

                    /* Store descriptor from where the data originates */
                    if (descriptor < 32)
                    {

                        /* RX data */
                        hdr->address = descriptor;
                    }
                    else if ((descriptor >= 64) && (descriptor < 96))
                    {

                        /* RX mode code  */
                        hdr->address = descriptor - 64;
                    }

                    /* Header length is always 1 byte*/
                    // rw_args->hdr_len = 1;

                    /* Next message position in buffer*/
                    msgadr += (2 + wc);

                    /*Does the new message exceeds the buffer bounds?*/
                    if (msgadr >= brm->desc[descriptor].bot)
                    {

                        /*Overflow. Return to the beggining of the buffer*/
                        msgadr = brm->desc[descriptor].top;
                    }

                    /* new last read position in the buffer*/
                    brm->last_read[descriptor] = msgadr;

                    /*have we exhausted the descriptor?*/
                    if (msgadr == current)
                    {

                        /*read decriptor control word*/
                        tmp = READ_REG(&brm->desc[descriptor].ctrl);

                        /*Clear acessed bit*/
                        brm->desc[descriptor].ctrl = tmp & ~0x10;
                    }

                    /* Have we read anything? */
                    if (wc > 0)
                    {

                        /* we have read a message, just exit*/
                        read = 1;
                    }
                }
            }

            /*Can we block? did we read anything?*/
            if ((brm->rx_blocking) && (wc == 0))
            {

                /*We can block and we have no data: sleep for a while. TODO:*/
                //				rtems_task_wake_after(1);
            }
            else if (wc > 0)
            { /*< Do we have some data?*/

                /*Tell the user how much did we read*/
                wrapper->buffer->payload_size = wc * 2 + 2;

                /*Now we can exit the read cycle*/
                read = 1;

                status = AIR_SUCCESSFUL;
            }
            else
            { /*< we cannot block*/

                /* break the cycle */
                read = 1;

                /* EWOULDBLOCK */
                status = AIR_NOT_AVAILABLE;
            }
        }
    }
    else
    { /*< we are a BM*/

        /*try to read while we can*/
        while (!read)
        {

            /*get current count*/
            current = READ_REG(&brm->regs->mbc);

            /*How many blocks are we way from the command block table end*/
            tmp = brm->bm_block_number - 1 - brm->bm_last_read;

            /*current pointer has wraped the buffer?*/
            if (tmp < current)
            {

                /*Calculate number of transfered blocks between read calls*/
                wc = tmp + brm->bm_block_number - 1 - current;

                /*Current has wraped*/
            }
            else
            {

                /*Calculate number of transfered blocks between read calls*/
                wc = brm->bm_block_number - 1 - brm->bm_last_read - current;
            }

            /*while the user buffer has space and we have data to read*/
            /*TODO:Extract correct buffer size accordingly with what is allocated*/
            while ((count < IOP_BUFFER_SIZE) && (count < wc))
            {

                /*calculate memory offset based on table offset*/
                offset = brm->bm_last_read * 8;

                /*copy command block contents to the user buffer*/
                bm_block[count].miw = READ_DMA(&brm->mem[offset]);

                bm_block[count].cw1 = READ_DMA(&brm->mem[offset + 1]);
                bm_block[count].cw2 = READ_DMA(&brm->mem[offset + 2]);
                bm_block[count].sw1 = READ_DMA(&brm->mem[offset + 4]);
                bm_block[count].sw2 = READ_DMA(&brm->mem[offset + 5]);
                bm_block[count].time = READ_DMA(&brm->mem[offset + 6]);

                /*Copy data to the user buffer*/
                (void)memcpy((void *)bm_block[count].data, (void *)&brm->mem[READ_DMA(&brm->mem[offset + 3])], 64);

                /*increment number of read packets*/
                count++;

                /*Get to new position in decriptor table*/
                brm->bm_last_read = (brm->bm_last_read + 1) % brm->bm_block_number;
            }

            /*Can we block? did we read anything?*/
            if ((brm->rx_blocking) && (count == 0))
            {

                /*We can block and we have no data: sleep for a while. TODO:*/
                //				rtems_task_wake_after(50);

                /*Do we have some data?*/
            }
            else if (count > 0)
            {

                /*Tell the user how much did we read*/
                wrapper->buffer->payload_size = count;

                /*we have read, so we can now exit */
                read = 1;

                /*we can't block*/
            }
            else
            {
                return AIR_NOT_AVAILABLE;
            }
        }
    }

    return status;
}

/**
 *  \brief writes data to a BRM device
 *
 * @param [in] iop_dev iop 1553 support struct
 * @param [out] arg pointer to iop wrapper
 *
 *  \return Status of the operation:
 *		- AIR_SUCCESSFUL if the operation completed sucessfully
 *		- AIR_INVALID_CONFIG: Operation not permited in BM mode or the requested
 *        descriptor is invalid
 * 		- AIR_INVALID_SIZE: User data exceeds the maximum allowed by MilStd (64 bytes)
 * 		- AIR_INVALID_PARAM: Invalid header or data pointer
 *		- AIR_NOT_AVAILABLE: Write buffers are full and we can't block waiting
 *		- AIR_NO_ACTION: Try Again Later
 *
 **/
uint32_t brm_write(iop_device_driver_t *iop_dev, void *arg)
{

    unsigned int current, next, descriptor, wc, suba;
    unsigned short miw = 0;

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;

    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    brm_priv *brm = (brm_priv *)(device->dev.driver);

    milstd_header_t *hdr = (milstd_header_t *)get_header(wrapper->buffer);

    /*Write can only be used in RT mode*/
    if (!(brm->mode & BRM_MODE_RT))
    {
        return AIR_INVALID_CONFIG;
    }

    /* Verify if user request does not exceed the maximum data size for milstd*/
    if ((get_payload_size(wrapper->buffer) > 64) != 0)
    {
        return AIR_INVALID_SIZE;
    }

    /* Verify if the user correctly provided data and header*/
    if ((get_payload(wrapper->buffer) == NULL) || (hdr == NULL))
    {
        return AIR_INVALID_PARAM;
    }

    /* check for any io errors*/
    check_errors(brm);

    /* Get target descriptor*/
    descriptor = hdr->desc;

    /* obtain corresponding subaddress*/
    suba = hdr->address;

    /* offset to obtain tx subaddress*/
    descriptor += 32;

    /* get word count to write*/
    wc = get_payload_size(wrapper->buffer) / 2;

    /* A word count of 0 corresponds to 32 words*/
    if (wc == 32)
    {

        /* 32 words */
        miw = 0 << 11;
    }
    else
    {

        /* less than 32 words*/
        miw = (wc & 0x1f) << 11;
    }

    /* Only subaddress transmission is allowed with write */
    if (descriptor < 32 || descriptor >= 64)
    {
        return AIR_INVALID_CONFIG;
    }

    /* get first place available to write in the descriptor buffer*/
    current = brm->desc[descriptor].cur;

    /* get last position written*/
    next = brm->written[suba] + 2 + wc;

    /* HW is delayed. It still has to wrap to consume all new data*/
    if ((brm->written[suba] < current) != 0)
    {

        /* There will be an overlap? If we write wc it will pass current*/
        if (next > current)
        {

            /* the buffer is full, can we block?*/
            if ((brm->tx_blocking) != 0)
            {

                /*we can block so wait until enought data was consumed by HW*/
                while (brm->desc[descriptor].cur < next)
                {

                    /*wait a while*/
                    //					rtems_task_wake_after(1);
                }
            }
            else
            { /*< we can't block*/

                /* Translates to posix EBUSY */
                return AIR_NO_ACTION;
            }
        }
    }

    /* copy message information word */
    brm->mem[brm->written[suba]] = miw;

    /* Clear timer */
    brm->mem[brm->written[suba] + 1] = 0;

    /* Copy data from user buffer to DMA buffer*/
    memcpy((void *)&brm->mem[brm->written[suba] + 2], (void *)get_payload(wrapper->buffer), (wc)*2);

    /* If next message exceeds the buffer limits*/
    if (next >= brm->desc[descriptor].bot)
    {

        /*Continue from the beggining*/
        next = brm->desc[descriptor].top;
    }

    /* update next place to write*/
    brm->written[suba] = next;

    /*If we have written something*/
    return AIR_SUCCESSFUL;
}

air_status_code_e brm_control(brm_priv *brm, void *arg)
{

    unsigned int i = 0;
    unsigned short ctrl, oper;
    libio_ioctl_args_t *ioarg = (libio_ioctl_args_t *)arg;
    unsigned int *data = ioarg->buffer;
    struct bc_msg *cmd_list = (struct bc_msg *)ioarg->buffer;
    air_status_code_e ret;

    if (!ioarg)
    {
        DBG("brm_control: invalid argument\n");
        return AIR_INVALID_PARAM;
    }

    ioarg->ioctl_return = 0;
    switch (ioarg->command)
    {

    case BRM_SET_MODE:
        if (data[0] > 2)
        {
            return AIR_INVALID_PARAM;
        }

        stop_operation(brm);
        if (data[0] == 0)
        {
            ret = bc_init(brm);
        }
        else if (data[0] == 1)
        {
            ret = rt_init(brm);
        }
        else if (data[0] == 2)
        {
            ret = bm_init(brm);
        }
        else
        {
            ret = AIR_INVALID_PARAM;
        }

        if (ret != AIR_SUCCESSFUL)
        {
            return ret;
        }

        if ((brm->mode & (BRM_MODE_RT | BRM_MODE_BM)) != 0)
        {
            start_operation(brm);
        }

        break;

    case BRM_SET_BUS: /*RT Only*/
        stop_operation(brm);
        ctrl = READ_REG(&brm->regs->ctrl);
        ctrl &= 0xE7FF;                /* Clear bit 12-11 ...      */
        ctrl |= (data[0] & 0x3) << 11; /* ... OR in new bus status */
        brm->regs->ctrl = ctrl;
        start_operation(brm);
        break;

    case BRM_SET_MSGTO: /*BC or BM*/
        stop_operation(brm);
        ctrl = READ_REG(&brm->regs->ctrl);
        ctrl &= 0xFDFF;             /* Clear bit 9 ...          */
        ctrl |= (data[0] & 1) << 9; /* ... OR in new MSGTO      */
        brm->regs->ctrl = ctrl;
        start_operation(brm);
        break;

    case BRM_SET_RT_ADDR: /*RT only*/
        stop_operation(brm);
        oper = READ_REG(&brm->regs->oper);
        oper &= 0x03FF;                           /* Clear bit 15-10 ...      */
        oper |= (data[0] & 0x1f) << 11;           /* ... OR in new address    */
        oper |= odd_parity(data[0] & 0x1f) << 10; /* ... OR in parity         */
        brm->regs->oper = oper;
        start_operation(brm);
        break;

    case BRM_SET_STD: /*WhY?*/
        stop_operation(brm);
        ctrl = READ_REG(&brm->regs->ctrl);
        ctrl &= 0xFF7F;             /* Clear bit 7 ...           */
        ctrl |= (data[0] & 1) << 7; /* ... OR in new ABSTD (1=A) */
        brm->regs->ctrl = ctrl;
        start_operation(brm);
        break;

    case BRM_SET_BCE: /*Broadcast Enable*/
        stop_operation(brm);
        ctrl = READ_REG(&brm->regs->ctrl);
        ctrl &= 0xFFEF;             /* Clear bit 4 ...           */
        ctrl |= (data[0] & 1) << 4; /* ... OR in new BCE         */
        brm->regs->ctrl = ctrl;
        start_operation(brm);
        break;

    case BRM_TX_BLOCK:
        brm->tx_blocking = data[0];
        break;

    case BRM_RX_BLOCK:
        brm->rx_blocking = data[0];
        break;

    case BRM_DO_LIST:

        // /*This operation is reserved for BC mode*/
        // if ( brm->mode != BRM_MODE_BC ){
        // return AIR_INVALID_PARAM;
        // }

        // /* Check if we are bus controller */
        // if ( ((READ_REG(&brm->regs->oper)>>8) & 3) != 0 ) {
        // return AIR_INVALID_PARAM;
        // }

        // /* Already processing list? */
        // if (is_executing(brm)) {
        // return AIR_NOT_AVAILABLE;
        // }

        // /*No errors yet*/
        // brm->bc_list_fail = 0;

        // /*Copy pointer to user cmd list*/
        // brm->cur_list = cmd_list;

        // /* bc command list starts at list offset zero */
        // brm->regs->dpoint = 0;

        // i = 0;

        // /*
        // * Lets process command block while we don't finish the list, or while
        // * the block count is less than the block count defined in the config
        // */
        // while (((cmd_list[i].ctrl & BC_EOL) == 0) && (i < brm->bc_block_number-1)){

        // /*diassemble user commands*/
        // ctrl = (4<<12) | (((cmd_list[i].ctrl & BC_BUSA) == BC_BUSA)<<9) | (((cmd_list[i].ctrl & BC_RTRT) ==
        // BC_RTRT)<<8);

        // /*If the user command is a RT to RT transmission*/
        // if (cmd_list[i].ctrl & BC_RTRT) {
        // /* receive cw */
        // cw1 = (cmd_list[i].rtaddr[0]<<11) | (0<<10) | (cmd_list[i].subaddr[0]<<5) | (cmd_list[i].wc & 0x1f);

        // /* transmit cw */
        // cw2 = (cmd_list[i].rtaddr[1]<<11) | (1<<10) | (cmd_list[i].subaddr[1]<<5) | (cmd_list[i].wc & 0x1f);
        // }

        // /*Other command than RT RT*/
        // else {

        // cw1 = (cmd_list[i].rtaddr[0]<<11) | (((cmd_list[i].ctrl&BC_TR) == BC_TR)<<10) | (cmd_list[i].subaddr[0]<<5) |
        // (cmd_list[i].wc & 0x1f);

        // /*Second command word is not used except for RT-RT*/
        // cw2 = 0;
        // }

        // /* Set up command block */
        // brm->bcmem->descs[i].ctrl   = ctrl;
        // brm->bcmem->descs[i].cw1    = cw1;
        // brm->bcmem->descs[i].cw2    = cw2;
        // brm->bcmem->descs[i].dptr   = brm->bc_block_number*8+i*32;  /* data pointer: number of bc blocks*8 */
        // brm->bcmem->descs[i].tsw[0] = 0;
        // brm->bcmem->descs[i].tsw[1] = 0;
        // brm->bcmem->descs[i].ba     = 0;
        // brm->bcmem->descs[i].timer  = 0;

        // /*Copy data*/
        // memcpy((void *)&brm->mem[brm->bcmem->descs[i].dptr], &cmd_list[i].data[0], cmd_list[i].wc*2);

        // /*Next command block*/
        // i++;
        // }

        // /* end of comand block list */
        // brm->bcmem->descs[i].ctrl = 0;

        // /*Start operating*/
        // start_operation(brm);

        break;

    case BRM_LIST_DONE:

        /*This operation is reserved for BC mode*/
        if (brm->mode != BRM_MODE_BC)
        {
            return AIR_INVALID_CONFIG;
        }

        /* Check if we are bus controller */
        if (((READ_REG(&brm->regs->oper) >> 8) & 3) != 0)
        {
            return AIR_INVALID_CONFIG;
        }

        // data[0] = 0;

        /*List is still being processed*/
        while (is_executing(brm))
        {

            /*Can we block*/
            if ((brm->tx_blocking) != 0)
            {

                /*Check for any errors*/
                check_errors(brm);

                /*did we find any error?*/
                if ((brm->bc_list_fail) != 0)
                {
                    return AIR_DEVICE_ERROR;
                }

                /*wait a while to see if the list has been completed*/
                //				rtems_task_wake_after(10);
            }
            else
            {
                /*we cannot block so return*/
                return AIR_NOT_AVAILABLE;
            }
        }

        // data[0] = 1; /* done */

        /* copy finished list results back into bc_msg array */
        i = 0;

        /*
         * Lets process command block while we don't finish the list, or while
         * the block count is less than the block count defined in the config
         */
        while (((cmd_list[i].ctrl & BC_EOL) == 0) && (i < brm->bc_block_number - 1))
        {

            /* TODO: ?*/
            if ((READ_DMA(&brm->bcmem->descs[i].ctrl) & 1) != 0)
            {

                /* Set BAME */
                brm->cur_list[i].ctrl |= 0x8000;
            }
            /* if this is a RT Transmit command, copy received data */
            if ((brm->cur_list[i].ctrl & BC_TR) != 0)
            {

                /* obtain word count*/
                // len = brm->cur_list[i].wc;

                /* Copy data to the user buffer*/
                // while( len-- > 0){
                // brm->cur_list[i].data[len] = READ_DMA(&brm->mem[brm->bcmem->descs[i].dptr+len]);
                // }
            }

            /* Update status words*/
            brm->cur_list[i].tsw[0] = READ_DMA(&brm->bcmem->descs[i].tsw[0]);
            brm->cur_list[i].tsw[1] = READ_DMA(&brm->bcmem->descs[i].tsw[1]);

            /* next command*/
            i++;
        }
        break;

    case BRM_CLR_STATUS:
        brm->status = 0;
        break;

    case BRM_GET_STATUS: /* copy status */

        if (!ioarg->buffer)
        {
            return AIR_INVALID_PARAM;
        }

        *(unsigned int *)ioarg->buffer = brm->status;
        break;

    case BRM_SET_EVENTID:
        brm->event_id = (unsigned int)ioarg->buffer;
        break;

    default:
        return AIR_INVALID_PARAM;
    }
    return AIR_SUCCESSFUL;
}

/**
 *  \brief Checks the IRQ log for errors that can be atributed to HW
 *
 *  \param [in] brm Device structure that shall be checked
 *  \param [in] event_status Variable resulting of OR'ing the errors that occured
 *  \param [in] signal_event Should the error handler be signaled that a error happened
 *
 *  This function checks for HW errors and adds them to the event_status that
 *  was already created by check_errors()
 */
static void check_hw_errors(brm_priv *brm, unsigned int event_status, int signal_event)
{
    unsigned short pending;

    /* clear interrupt flags & handle Hardware errors */
    pending = READ_REG(&brm->regs->ipend);

    /*Verify errors*/
    /*DMA fail*/
    if ((pending & BRM_DMAF_IRQ) != 0)
    {
        FUNCDBG("BRM: BRM_DMAF_IRQ\n\r");
        event_status |= BRM_DMAF_IRQ;
        signal_event = 1;
    }

    /*Buffer Wrap fail*/
    if ((pending & BRM_WRAPF_IRQ) != 0)
    {
        FUNCDBG("BRM: BRM_WRAPF_IRQ\n\r");
        event_status |= BRM_WRAPF_IRQ;
        signal_event = 1;
    }

    /*TODO: ?*/
    if ((pending & BRM_TAPF_IRQ) != 0)
    {
        FUNCDBG("BRM: BRM_TAPF_IRQ\n\r");
        event_status |= BRM_TAPF_IRQ;
        signal_event = 1;
    }

    /* Copy current mask to status mask */
    if ((event_status) != 0)
    {
        if ((event_status & 0xffff0000) != 0)
        {
            brm->status &= 0x0000ffff;
        }

        brm->status |= event_status;
    }

    /* signal event once */
    if (signal_event && (brm->event_id != 0))
    {

        /*send event to error handler with event_status has an argument*/
        //		rtems_event_send(brm->event_id, event_status);
    }

    return;
}

/**
 *  \brief Checks the IRQ log for errors
 *
 *  \param [in] brm Device structure that shall be checked
 */
static void check_errors(brm_priv *brm)
{
    unsigned short descriptor, pending, tmp;
    unsigned short iaw, iiw;
    int i;
    int signal_event = 0;
    unsigned int event_status = 0;
#define SET_ERROR_DESCRIPTOR(descriptor) (event_status = (event_status & 0x0000ffff) | descriptor << 16)

    /*Iterate over IRQ log list*/
    for (i = 0; i < 16; i++)
    {

        /*Read IIW: if it is not cleared*/
        if ((iiw = READ_REG(&brm->irq_log[i].iiw)) != 0xffff)
        {

            /*Read IAW*/
            iaw = READ_REG(&brm->irq_log[i].iaw);

            /*Find erroneous descriptor*/
            descriptor = iaw >> 2;

            /*store iiw*/
            pending = iiw;

            /*Bus Controller Illegal Command Error*/
            if ((pending & BRM_BC_ILLCMD_IRQ) != 0)
            {

                // TODO: REMOVE
                brm->bc_list_fail = 1;

                /*set event status*/
                SET_ERROR_DESCRIPTOR(descriptor);

                FUNCDBG("BRM: ILLCMD IRQ\n\r");

                /*Clear error in IRQ log*/
                brm->irq_log[i].iiw &= ~BRM_BC_ILLCMD_IRQ;
            }

            /*RT Illegal Command Error*/
            if ((pending & BRM_RT_ILLCMD_IRQ) != 0)
            {
                FUNCDBG("BRM: BRM_RT_ILLCMD_IRQ\n\r");

                /*Or in new event in status*/
                brm->status |= BRM_RT_ILLCMD_IRQ;

                /*Or in new event*/
                event_status |= BRM_RT_ILLCMD_IRQ;

                /*set event status*/
                SET_ERROR_DESCRIPTOR(descriptor);

                /*This event is to be signaled*/
                signal_event = 1;

                /*Clear error in IRQ log*/
                brm->irq_log[i].iiw &= ~BRM_RT_ILLCMD_IRQ;
            }

            /*Illegal Operation*/
            if ((pending & BRM_ILLOP_IRQ) != 0)
            {
                FUNCDBG("BRM: BRM_ILLOP_IRQ\n\r");
                // TODO: REMOVE
                brm->bc_list_fail = 1;

                /*Or in new event*/
                event_status |= BRM_ILLOP_IRQ;

                /*set event status*/
                SET_ERROR_DESCRIPTOR(descriptor);

                /*This event is to be signaled*/
                signal_event = 1;

                /*Clear error in IRQ log*/
                brm->irq_log[i].iiw &= ~BRM_ILLOP_IRQ;
            }

            /*memory error*/
            if ((pending & BRM_MERR_IRQ) != 0)
            {
                FUNCDBG("BRM: BRM_MERR_IRQ\n\r");

                /*Or in new event*/
                event_status |= BRM_MERR_IRQ;

                /*set event status*/
                SET_ERROR_DESCRIPTOR(descriptor);

                /*This event is to be signaled*/
                signal_event = 1;

                /*Clear error in IRQ log*/
                brm->irq_log[i].iiw &= ~BRM_MERR_IRQ;
            }

            /*Subaddress acessed, TODO:*/
            if (!((pending & BRM_SUBAD_IRQ) || (pending & BRM_EOL_IRQ) || (pending & BRM_MBC_IRQ)))
            {

                /*Clear Log Entry*/
                brm->irq_log[i].iiw = 0xffff;

                /*Read  descriptor control word*/
                tmp = READ_REG(&brm->desc[descriptor].ctrl);

                /*Clear Acessed Bit*/
                brm->desc[descriptor].ctrl = tmp & ~0x10;
            }
        }
    }

    /*Check HW errors*/
    check_hw_errors(brm, event_status, signal_event);

    return;
}

/**
 * @brief Obtains the mode that a milstd device is configured to work on
 * @param [in] brm driver private struct
 * @return Operative mode of a milstd device
 */
unsigned int brm_get_operative_mode(brm_priv *brm)
{

    return brm->config->mode;
}

/**
 * @brief brm_do_list inititates the execution of a BC command list
 * @param [in] minor minor number of the target device
 * @return air_status_code_e, Status of the operation:
 *  	- AIR_INVALID_CONFIG if current mode is not BC
 * 		- AIR_NOT_AVAILABLE if the core is still executing
 * 		- AIR_SUCCESSFUL if the list started being executed
 */
air_status_code_e brm_do_list(brm_priv *brm)
{

    /* This operation is reserved for BC mode*/
    if (brm->mode != BRM_MODE_BC)
    {
        return AIR_INVALID_CONFIG;
    }

    /* Check if we are bus controller */
    if (((READ_REG(&brm->regs->oper) >> 8) & 3) != 0)
    {
        return AIR_INVALID_CONFIG;
    }

    /* Already processing list? */
    if ((is_executing(brm)) != 0)
    {
        return AIR_NOT_AVAILABLE;
    }

    /* No errors yet*/
    brm->bc_list_fail = 0;

    /* list begins at offset 0 */
    brm->regs->dpoint = 0;

    /*Start operating*/
    start_operation(brm);

    return AIR_SUCCESSFUL;
}

/**
 * @brief returns a pointer to the core's DMA memory area
 * @param [in] minor minor number of the core to select
 */
void *brm_get_memarea(brm_priv *brm)
{

    return (void *)brm->mem;
}

/**
 * @brief returns the number of BC command blocks
 * @param [in] minor minor number of the core to select
 */
int brm_get_number_bc_blocks(brm_priv *brm)
{

    return brm->bc_block_number;
}

/**
 * @brief Verifies if a BC command list was completed
 * @param [in] minor minor number of the core to select
 */
air_status_code_e brm_list_done(brm_priv *brm)
{

    /*This operation is reserved for BC mode*/
    if (brm->mode != BRM_MODE_BC)
    {
        return AIR_INVALID_CONFIG;
    }

    /* Check if we are bus controller */
    if (((READ_REG(&brm->regs->oper) >> 8) & 3) != 0)
    {
        return AIR_INVALID_CONFIG;
    }

    /* Is list still being processed*/
    if ((is_executing(brm)) != 0)
    {
        return AIR_NOT_AVAILABLE;
    }

    return AIR_SUCCESSFUL;
}

static uint32_t detect_bc_to_rt(bc_command_t *ul)
{

    uint32_t rc = 0;

    /*  detect transfers which envolve data originating from the BC*/
    if ((((ul->ccw >> 12) & 0x1) == 0) && ((ul->subaddr[1] == 0) || (ul->subaddr[0] == 0) || (ul->subaddr[0] == 31)))
    {

        /* this is a bc to rt transfer */
        rc = 1;
    }

    return rc;
}

static uint32_t detect_rt_to_rt(bc_command_t *ul)
{

    uint32_t rc = 0;

    /* detect if this is a rt to rt transfer */
    if ((ul->subaddr[0] != 0) && (ul->subaddr[0] != 31) && (ul->subaddr[1] != 0) && (ul->subaddr[1] != 31))
    {

        rc = 1;
    }

    return rc;
}

static void brm_translate_user_commands(brm_priv *brm)
{
    uint8_t nret = 0;
    //	uint8_t excl = 0;
    uint8_t ch_ab = 0;
    iop_chain_control *aux = NULL;
    int bc_to_rt = 0;
    int rt_to_rt = 0;
    int i = 0;
    int z = 0;
    unsigned int cl_size = brm->bc_user_list_size;

    /* pointer to user defined command list */
    bc_command_t *user_list = (bc_command_t *)brm->bc_user_list;

    /* pointer to the start of the device's command list */
    struct bc_desc *cmd_entry = (struct bc_desc *)brm->bcmem;

    /* pointer to the first element of the core's list*/
    brm->cur_list = (struct bc_desc *)brm->bcmem;

    /* iterate user command list */
    for (i = 0; i < cl_size; i++)
    {

        /* time slot zero is ignored*/
        if (user_list[i].time_slot != 0)
        {

            /*
             * insert a list entry dedicated to wait for the last timer value
             * and to load the new one.
             * Since we are introducing these timer specific commands, the
             * indexes i and z will drift way. (there is no 1:1 match between
             * the user and the device's list)
             */
            cmd_entry[z].ctrl = (0xE << 12); /*< load minor frame timer opcode */
            cmd_entry[z].cw1 = 0;
            cmd_entry[z].cw2 = 0;
            cmd_entry[z].dptr = 0;
            cmd_entry[z].tsw[0] = 0;
            cmd_entry[z].tsw[1] = 0;
            cmd_entry[z].ba = 0;
            cmd_entry[z].timer = user_list[i].time_slot;

            z++;
        }

        /* Verify if this is a branch command */
        if (((user_list[i].ccw) & TB_BIT) != 0)
        {

            /* branch command */
            cmd_entry[z].ctrl = (0x2 << 12); /*< goto opcode */
            cmd_entry[z].cw1 = 0;
            cmd_entry[z].cw2 = 0;
            cmd_entry[z].dptr = 0; /*<data pointer: number of bc blocks*8 */
            cmd_entry[z].tsw[0] = 0;
            cmd_entry[z].tsw[1] = 0;
            cmd_entry[z].ba = (user_list[i].branch_offset) * 8 * 2;
            cmd_entry[z].timer = 0;
        }
        else if (((user_list[i].ccw) & END_BIT) != 0)

        { /*< End the list */

            cmd_entry[z].ctrl = 0;
            cmd_entry[z].cw1 = 0;
            cmd_entry[z].cw2 = 0;
            cmd_entry[z].dptr = 0;
            cmd_entry[z].tsw[0] = 0;
            cmd_entry[z].tsw[1] = 0;
            cmd_entry[z].ba = 0;
            cmd_entry[z].timer = 0;
        }
        else
        { /*< Transfer */

            /* Get number of retries */
            nret = (((user_list[i].ccw) & NRET_BITMASK) >> 8);

            /* Number of retries is limited to 4 */
            if (nret >= 4)
            {

                /* 0b00 means 4 retries */
                nret = 0;
            }

            /* Exclusive time slot */
            //			excl = (((user_list[i].ccw) & EXCL_BIT) >> 16);

            /* Channel A or B */
            ch_ab = (((user_list[i].ccw) & AB_BIT) >> 13);

            /* detect if this is a rt to rt transfer */
            rt_to_rt = detect_rt_to_rt(&user_list[i]);

            /* detect if this is a bc to rt transfer */
            bc_to_rt = detect_bc_to_rt(&user_list[i]);

            if (bc_to_rt == 1)
            {

                /* Fill command word: since we have no data, use skip opcode */
                cmd_entry[z].ctrl = (0x1 << 12) | (nret << 10) | (ch_ab << 9) | (rt_to_rt << 8);

                aux = &brm->shortcut[(user_list[i].rtaddr[0] & 0x1f)];

                /* this bc command entry needs data, so create a shortcut*/
                create_write_cmd_shortcut(aux, &user_list[i], (void *)&cmd_entry[z]);
            }
            else
            {

                /* Fill command word: command is complete therefore
                 * use retry on condition opcode */
                cmd_entry[z].ctrl = (0x7 << 12) | (nret << 10) | (ch_ab << 9) | (rt_to_rt << 8);
            }

            /* MILSTD command word 1. insert first RT Address */
            cmd_entry[z].cw1 = ((user_list[i].rtaddr[0] & 0x1f) << 11);

            /* or in rt subaddress 1*/
            cmd_entry[z].cw1 |= ((user_list[i].subaddr[0] & 0x1F) << 5);

            /* or in mode code / word count */
            cmd_entry[z].cw1 |= ((user_list[i].ccw & 0x1F));

            /* rt to rt transfers require command word 2*/
            if (rt_to_rt == 1)
            {

                /* MILSTD command word 2. insert transmition RT Address */
                cmd_entry[z].cw2 = ((user_list[i].rtaddr[1] & 0x1f) << 11);

                /* in rt to rt, the rcv/tx bit is 0 for cw1 */
                cmd_entry[z].cw1 |= (0 << 10);

                /* transmit/receive bit: 1 for cw2 rt-rt */
                cmd_entry[z].cw2 |= (1 << 10);

                /* rt subaddress 1*/
                cmd_entry[z].cw2 |= ((user_list[i].subaddr[1] & 0x1F) << 5);

                /* mode code / word count */
                cmd_entry[z].cw2 |= ((user_list[i].ccw & 0x1F));
            }
            else
            {

                /* command word 2 is only used in rt to rt transfers */
                cmd_entry[z].cw2 = 0;

                /* transmit/receive bit from user list: BC-to-RT or RT-to-BC */
                cmd_entry[z].cw1 |= (((user_list[i].ccw >> 12) & 0x1) << 10);
            }

            /** @todo allocate memory using a different interface */
            cmd_entry[z].dptr = 1630 * 8 + i * 32;
            cmd_entry[z].tsw[0] = 0;
            cmd_entry[z].tsw[1] = 0;
            cmd_entry[z].ba = 0;
            cmd_entry[z].timer = 0;
        }

        z++;
    }
}

air_status_code_e brm_bc_init_user_list(brm_priv *brm)
{
    /*This operation is reserved for BC mode*/
    if (brm->mode != BRM_MODE_BC)
    {
        return AIR_INVALID_CONFIG;
    }

    /* Check if we are bus controller */
    if (((READ_REG(&brm->regs->oper) >> 8) & 3) != 0)
    {
        return AIR_INVALID_CONFIG;
    }

    /* 128kb memory constrain limits command list size */
    if ((brm->bc_user_list_size > 1630) != 0)
    {
        return AIR_INVALID_CONFIG;
    }

    /* translate the user list to a specific device interface */
    brm_translate_user_commands(brm);

    return AIR_SUCCESSFUL;
}

static air_status_code_e brm_bc_verify_command_status(uint32_t cw, uint32_t sw)
{
    air_status_code_e status = AIR_SUCCESSFUL;

    /* verify transfer status:  for now detect only ME bit */
    if ((((sw >> 10) & 0x3) != 0) || (cw & 0x1))
    {

        status = AIR_DEVICE_ERROR;

        /* something has gone wrong with the transfer */
        iop_raise_error(HW_PROBLEM);
    }

    return status;
}

static int brm_bc_process_command(volatile unsigned short *mem, struct bc_desc *desc, iop_buffer_t *buf)
{
    /* return code */
    int rc = 0;

    /* transmit receive bit */
    uint8_t trbit = ((desc->cw1 >> 10) & 0x1);

    /* rt subaddress 1 */
    uint8_t subaddress1 = ((desc->cw1 >> 5) & 0x1f);

    /* rt subaddress 2 */
    uint8_t subaddress2 = ((desc->cw2 >> 5) & 0x1f);

    /*word count / mode code */
    uint8_t wcmc = (desc->cw1 & 0x1f);

    /* milstd header */
    milstd_header_t *hdr;

    /*  detect transfers which envolve data originating from the BC*/
    if ((trbit == 0) && ((subaddress2 == 0) || (subaddress1 == 0) || (subaddress1 == 31)))
    {

        /* this is a BC-RT command. Verify if data was sent correctly  */
        if (brm_bc_verify_command_status(desc->ctrl, desc->tsw[0]) == AIR_SUCCESSFUL)
        {

            /* we shall wait for new data from the user. dummy this command */
            desc->ctrl &= 0x0FFF;

            /* insert skip opcode */
            desc->ctrl |= (0x1 << 12);
        }

        /* this descriptor is updated. mark it as not accessed */
        desc->tsw[0] = 0;
        desc->tsw[1] = 0;

        /* clear BAME bit */
        desc->ctrl &= ~0x1;
    }
    else
    { /*< there is data to be read */

        /*  Verify if data was received correctly  */
        if (brm_bc_verify_command_status(desc->ctrl, desc->tsw[0]) == AIR_SUCCESSFUL)
        {

            /* detect mode code */
            if ((subaddress1 == 0) || (subaddress1 == 31))
            {

                /* mode codes 16/18/19 transmit one data word */
                if ((wcmc == 16) || (wcmc == 18) || (wcmc == 19))
                {

                    /* copy received data to user buffer */
                    (void)memcpy(get_payload(buf), (void *)&mem[desc->dptr], 2);

                    /* data length */
                    buf->payload_size = 2;

                    /* copy milstd header */
                    hdr = (milstd_header_t *)get_header(buf);

                    /* RT address 1 */
                    hdr->desc = subaddress1;

                    /* Sub address 1 */
                    hdr->address = ((desc->cw1 >> 11) & 0x1f);

                    /* data was successfuly read*/
                    rc = 1;
                }
            }
            else
            {

                /* word count of zero really means 32 words */
                if (wcmc == 0)
                {

                    wcmc = 32;
                }

                /* copy received data to user buffer */
                (void)memcpy(get_payload(buf), (void *)&mem[desc->dptr], wcmc * 2);

                /* data length */
                buf->payload_size = wcmc * 2;

                /* copy milstd header */
                hdr = (milstd_header_t *)get_header(buf);

                /* RT address 1 */
                hdr->desc = subaddress1;

                /* Sub address 1 */
                hdr->address = ((desc->cw1 >> 11) & 0x1f);

                /* data was successfuly read*/
                rc = 1;
            }
        }

        /* this descriptor is updated. mark it as not accessed */
        desc->tsw[0] = 0;
        desc->tsw[1] = 0;

        /* clear BAME bit */
        desc->ctrl &= ~0x1;
    }

    return rc;
}

/**
 * @brief Iterates through a BC command list, extracts any data a does maintenance operations
 *
 * @param [in] iop_dev iop 1553 support struct
 * @param [out] arg pointer to iop wrapper
 *
 * @return status of the operation:
 *  	- AIR_SUCCESSFUL: Data was sucessfully read.
 * 		- AIR_TIMED_OUT: End of list.
 *
 * This function will obtain the list that is currently being processed by
 * the bc core and will process the already completed commands. Each command is
 * checked for errors and available data. Depending on the command type, some
 * commands will be updated, others will have their data extracted. The function
 * returns when there is data available to be read or when the command list as
 * reached its end. The function starts processing the list in a previously read
 * position. It then follows the branching structure of the list.
 */
uint32_t brm_bc_process_completed_list(iop_device_driver_t *iop_dev, void *arg)
{
    /* return code */
    air_status_code_e status;

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;

    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    brm_priv *bDev = (brm_priv *)(device->dev.driver);

    /* indicates if data was sucessfully read or if the list has ended */
    int end = 0;

    /* current bc descriptor */
    struct bc_desc *current;

    /* Command opcode */
    uint8_t opcode;

    status = AIR_SUCCESSFUL;

    /* @todo place a limit in the number of reads... */
    while (end == 0)
    {

        /* next descriptor/command to be read */
        current = &bDev->cur_list[bDev->bc_last_read];

        /* obtain command opcode */
        opcode = ((current->ctrl >> 12) & 0xf);

        /* detect if this is a branch or a data transfer command or other */
        switch (opcode)
        {

        case 0x7: /* transfer */

            /*
             * verify if this descriptor was acessed by the hw: TSW and BAME
             * we consider it not acessed when the status word is 0 and the
             * BAME bit is not set.
             */
            if ((0 == current->tsw[0]) && !(current->ctrl & 0x1))
            {

                /* this descriptor was not acessed. try again later */
                status = AIR_TIMED_OUT;

                /* we reached the end of the processed command list */
                end = 1;
            }
            else
            {

                /* descriptor was acessed, so obtain any data and check for errors */
                if (brm_bc_process_command(bDev->mem, current, wrapper->buffer) == 1)
                {

                    /* data was successfully read */
                    end = 1;
                }

                /* update last_read */
                bDev->bc_last_read++;
            }

            break;

        case 0x2: /*< branch */

            /* follow branch and update last_read */
            bDev->bc_last_read = current->ba / 8;

            break;

        case 0xe:
        case 0x1:
            /* go for next command block */
            bDev->bc_last_read++;
            break;

        case 0x0:
        default:
            /* default to the beggining of the list. Shall not occur */
            bDev->bc_last_read = 0;

            /* we reached the end of the processed command list */
            end = 1;

            break;
        }
    }

    return status;
}

/**
 * @brief Appends incomming data to a milstd command belonging to a bc command list.
 *
 * @param [in] minor bc minor device number whose data belongs to.
 * @param [in] data input data buffer
 * @param [in] hdr Mil-std header comprising a RT address and a subaddress
 * @param [in] size data buffer length in bytes
 *
 * @return air_status_code_e; status of the operation:
 *  	- RTEMS_SUCESSFULL: Data was sucessfully written to the list
 * 		- RTEMS_NOT_DEFINED: this data is not linked to any command in the list
 *
 * This function inserts incoming user data in the bc command list. For that
 * purpose it uses a shortcut mapping that matches every command in the list
 * with a milstd header.  The header in the incoming data is used as a key
 * to that mapping, which yields the data transfer command affecting that header.
 * This header is composed by one RT address and one subaddress.
 *
 */
air_status_code_e brm_bc_insert_new_data(brm_priv *bDev, uint8_t *data, milstd_header_t *hdr, unsigned int size)
{

    /* return code */
    air_status_code_e status;

    /* Current descriptor */
    struct bc_desc *desc;

    iop_chain_control *aux = &bDev->shortcut[(hdr->address & 0x1f)];

    /* find if this data request can be matched to any descriptor */
    desc = (struct bc_desc *)find_shortcut(aux, hdr, size);

    /* verify if we obtained a valid descriptor */
    if (desc == NULL)
    {

        /* no descriptor was obtained */
        status = AIR_NOT_AVAILABLE;
    }
    else
    {

        /* lets copy the data */
        (void)memcpy((void *)&bDev->mem[desc->dptr], (void *)data, size);

        /* and enable the descriptor: modify the opcode */
        desc->ctrl &= 0x0FFF;

        /* insert retry on condition opcode */
        desc->ctrl |= (0x7 << 12);

        /* this descriptor is updated. mark it as not accessed */
        desc->tsw[0] = 0;
        desc->tsw[1] = 0;

        /* data was appended sucessfully */
        status = AIR_SUCCESSFUL;
    }

    return status;
}

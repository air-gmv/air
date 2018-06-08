/*  RTEMS GR1553B BC and/or BM driver usage example
 *
 *  COPYRIGHT (c) 2010.
 *  Aeroflex Gaisler.
 *
 *
 *  This example may be configured (default) to operate BM and BC interfaces.
 *
 *  Bus Controller (BC) example
 *  ---------------------------
 *  A descriptor list is created using list API (gr1553bc_list.h), then
 *  scheduled for execution. The List has 10 Major frames, where Major Frame
 *    0: Initialization FRAME, find RTs and issue STARTUP command to RTs
 *    1: Wait for External Trigger, Setup Communication Frames (CF) now that
 *       we know which RTs are available.
 *    2 (CF0): Wait For External Trigger. Send/Receive Data to RTs
 *    3 (CF1): Send/Receive Data to RTs
 *    4 (CF2): Send/Receive Data to RTs
 *    5 (CF3): Send/Receive Data to RTs
 *    6 (CF4): Send/Receive Data to RTs
 *    7 (CF5): Send/Receive Data to RTs
 *    8 (CF6): Send/Receive Data to RTs
 *    9 (CF7): Send Time Sync message
 *    10: Final Major Frame, the shutdown frame, jumped to when shutting down
 *        Sending RTs shutdown message over and over again.
 *
 *  Each CF starts with sending a Communication Frame SYNC message.
 *
 *  The example Sends/Receives 1kB/sec data, two 64 blocks are sent and 
 *  received per Major frame. The time delay after Sending the Data is
 *  about 25ms, so the RT got some time to prepare respond.
 *
 *  When running on an AMBA-over-PCI bus the AMBA_OVER_PCI define should be
 *  set. See below.
 *
 *  The BC processing loop prints out the current Major, Minor Frame and 
 *  Message Slot the BC is executing (INDICATION).
 */

#include <rtems.h>
#define CONFIGURE_INIT
#include <bsp.h> /* for device driver prototypes */

/************ CONFIGURATION OPTIONS ************/

/* CONFIG: Define AMBA_OVER_PCI if BC is accessed over PCI, for example on a
 *         GR-RASTA-IO board.
 */
/*#define AMBA_OVER_PCI*/

/* CONFIG: Define SOFT_EXTTRIG_ENABLE if software shall generate the
 *         External Sync Trigger. Define it if no TimeMaster generates the
 *         trigger pulse.
 *
 * Note that the example demonstrates the EXTERNAL TRIGGER function, if that
 * it not wanted, the "Wait For External Sync" bit should be removed from
 * [MAJOR|MINOR|SLOT] : [1|0|0] [2|0|0]
 */
/*#define SOFT_EXTTRIG_ENABLE*/


/* CONFIG: If PRINT_DEVICE_TOPOLOGY is defined the example will print all devices
 *         found by the driver manager, and which bus they are situated on.
 */
/*#define PRINT_DEVICE_TOPOLOGY*/

/* CONFIG: If PRINT_PCI_BUS is defined the example will print PCI configuration
 *         space after being setup by configuration library.
 */
/*#define PRINT_PCI_BUS*/

/* CONFIG: If PRINT_AMBAPP_BUS is defined the example will print the on-chip
 *         AMBA Plug&Play information from the prescanned AMBA structure used
 *         by the driver manager.
 */
/*#define PRINT_AMBAPP_BUS*/


/* CONFIG: Memory setup differ depending on the GR1553BC is located on an
 *         AMBA-over-PCI or an on-chip AMBA bus.
 *
 *         AMBA-over-PCI: Use hard-coded addresses (0x40000000..) located in the
 *                        External Memory (SRAM/SDRAM) on the GR-RASTA-IO board.
 *
 *         on-chip AMBA:  Use dynamically allocated memory using malloc() by
 *                        driver, in CPU's main memory.
 */
#ifdef AMBA_OVER_PCI
  /* Translate Data Pointers from CPU-address into GR-RASTA-XXXX PCI address */
  #define TRANSLATE(adr) (uint16_t *)((unsigned int)(adr) | 0x1)
  /* Use 0x40000000 at the PCI board as base for the descriptor table */
  #define BD_TABLE_BASE TRANSLATE(0x40000000)

#else
  /* No translation of Data buffers needed */
  #define TRANSLATE(adr) adr
  /* Let driver dynamically allocate the descriptor table */
  #define BD_TABLE_BASE NULL

#endif




rtems_task Init( rtems_task_argument argument);	/* forward declaration needed */
/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS             8
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (64 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32
#define CONFIGURE_MAXIMUM_DRIVERS 16
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define RTEMS_PCI_CONFIG_LIB
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

#include <rtems/confdefs.h>


/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) /* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
 #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
 #endif
 #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
 #endif
#endif
//#define CONFIGURE_DRIVER_PCI_GR_RASTA_IO        /* GR-RASTA-IO PCI Target Driver */s

/* CONFIGURE GR-RASTA-IO Board */
#define CONFIGURE_DRIVER_AMBAPP_MCTRL           /* Driver for Memory controller needed when using SRAM on PCI board */
#define RASTA_IO_SRAM

/******** ADD BC DRIVERS **********/
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553BC

#ifdef LEON2
  /* PCI support for AT697 */
  #define CONFIGURE_DRIVER_LEON2_AT697PCI
  /* AMBA PnP Support for GRLIB-LEON2 */
  #define CONFIGURE_DRIVER_LEON2_AMBAPP
#endif

#include <drvmgr/drvmgr_confdefs.h>

#include <stdio.h>
#include <stdlib.h>

//#undef ENABLE_NETWORK_SMC_LEON3

#include "config.c"

int init_bc_list(void);
int init_bc(void);

rtems_task Init(
  rtems_task_argument ignored
)
{

	/* Initialize Driver manager and Networking, in config.c */
	system_init();

#ifdef PRINT_DEVICE_TOPOLOGY
	/* Print device topology */	
	drvmgr_print_topo();
#endif

#ifdef PRINT_PCI_BUS
	/* Print PCI board's configuration */
	printf("\n\n ####### PCI CONFIGURATION #######\n\n");

	/* Print PCI bus resources */
	pci_print();
#endif

#if defined(PRINT_AMBAPP_BUS) && defined(LEON3)
	/* Print AMBA on-chip Plug&Play information */
	printf("\n\n ####### AMBA PnP CONFIGURATION #######\n\n");

	/* Print AMBA Bus */
	ambapp_print(ambapp_root, 10);
#endif

	if ( init_bc() ) {
		printf("Failed to initialize BC\n");

		exit(0);
	}

	exit( 0 );
}

void *bc = NULL;
int bc_in_com_state = 0;
void bc_timesync(void);
void bc_enter_com_state(unsigned int rts_up);

/* For Debnugging only: shut down BC and BM prompt. This will enable us to 
 * debug the current hardware state if an error occurs.
 *
 * Configure base address of GR1553B core
 */
void stop_bc_bm(void)
{
	*(volatile unsigned int *)(0x80000500 + 0x44) = 0x15520004;
	*(volatile unsigned int *)(0x80000500 + 0xC4) = 0;
}

#include <bsp/gr1553bc.h>
#include "pnp1553.h"
#include "bc_list.c"

int bc_irq_cnt=0;

void bc_isr(union gr1553bc_bd *bd, void *data)
{
	/* bd==NULL  : DMA error IRQ
	 * bd!=NULL  : Transfer IRQ
	 */
	bc_irq_cnt++;
}

int init_bc(void)
{
	int status;
	int mid;
	int cnt;

	/* Aquire BC device */
	bc = gr1553bc_open(0);
	if ( !bc ) {
		printf("Failed to open BC[%d]\n", 0);
		return -1;
	}

	if ( init_bc_list() ) {
		printf("Failed to create BC list\n");
		return -2;
	}

	/* Print List:
	 *   gr1553bc_show_list(list, 0);
	 */
	/*gr1553bc_show_list(list, 0);*/

	/* Register standard IRQ handler when an error occur */
	if ( gr1553bc_irq_setup(bc, bc_isr, bc) ) {
		printf("Failed to register standard IRQ handler\n");
		return -2;
	}

	/* Start previously created BC list */
	status = gr1553bc_start(bc, list, NULL);
	if ( status ) {
		printf("Failed to start BC: %d\n", status);
		return -3;
	}

	printf("            MAJOR MINOR SLOT\n");
	cnt = 0;
	while ( 1 ) {
		/* The CPU sharing implmentet is very simple,
		 * note that it would be more appropriate to
		 * implement this using rate-monotonic.
		 *
		 * Task is woken every 1 tick.
		 *
		 * TICK N - CPU ALLOC - WORK
		 *  0     - 10%       - Process BC list and handle startup
		 *                      time management.
		 *  1     - 10%       - BM Log handling
		 *  3,8   - 20%       - Do something
		 *  All   - .         - Time Handling
		 */

		/* Execute every 10 ticks
		 * 
		 */
		if ( cnt == 0 ) {
			if ( gr1553bc_indication(bc, 0, &mid) ) {
				printf("Error getting current MID\n");
				gr1553bc_show_list(list, 0);
				return -1;
			}

			printf("%02x %02x %02x\n",
				GR1553BC_MAJID_FROM_ID(mid),
				GR1553BC_MINID_FROM_ID(mid),
				GR1553BC_SLOTID_FROM_ID(mid));

			/* Talk to RTs */
			bc_list_process();
		}

		/* Execute every 5 ticks  */
		if ( bc_in_com_state && ((cnt == 3) || (cnt == 8)) ) {
			
		}

		cnt ++;
		if ( cnt >= 10 )
			cnt = 0;

		/* Sleep 1 ticks */
		rtems_task_wake_after(1);
	}

	return 0;
}

/* Called each time before a time sync is triggered by software */
void bc_timesync(void)
{

}

/* Called when all RTs have been found and started. The BC can now
 * start requesting data from RTs.
 *
 * rts_up is a bitmask that tells us which RTs are up an running.
 */
void bc_enter_com_state(unsigned int rts_up)
{
	/* Start a task to handle RT5 if available */
	if ( rts_up & (1<<5) ) {
		bc_in_com_state = 1;
	}
	
}

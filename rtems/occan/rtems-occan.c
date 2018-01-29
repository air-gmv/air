/* Simple OC_CAN interface test.
 *
 * Gaisler Research 2007,
 * Daniel Hellstr�m
 *
 */


#define TASK_TX

#define CONFIGURE_INIT


#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(1024*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_INIT_TASK_STACK_SIZE	(64*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	100
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define RTEMS_PCI_CONFIG_LIB
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

///////////// NEW DEFS
   #define CONFIGURE_MAXIMUM_DRIVERS                     16
   #define CONFIGURE_MAXIMUM_DEVICES                     16
   #define CONFIGURE_MAXIMUM_TIMERS                      40

#include <rtems.h>
#include <bsp.h> /* for device driver prototypes */

rtems_task Init( rtems_task_argument argument);	/* forward declaration needed */
/* Configure RTEMS */

#include <rtems/config.h>
#include <rtems/confdefs.h>
#include <drvmgr/drvmgr.h>

/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) /* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
// #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
// #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  //#define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
// #endif
#endif
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF    /* PCI is for GR-701 OCCAN */
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI   /* PCI is for GR-701 OCCAN */
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2  /* PCI is for GR-701 OCCAN */
//#define CONFIGURE_DRIVER_PCI_GR_701             /* GR-701 PCI TARGET has a OCCAN core */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_OCCAN   /* OCCAN Driver */

#include <drvmgr/drvmgr_confdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <occan.h>

#include "occan_lib.h"


/* Include driver configurations and system initialization */
#include "config.c"


#define DO_FILTER_TEST	

/* Loopback mode - both CAN cores are used one for RX and one for TX */
#define OCCAN_DEVICE_TX_NAME "/dev/occan0"
#define OCCAN_DEVICE_RX_NAME "/dev/occan1"


rtems_task task1(rtems_task_argument argument);
rtems_task task2(rtems_task_argument argument);

/*extern int errno;*/

rtems_id   Task_id[3];         /* array of task ids */
rtems_name Task_name[3];       /* array of task names */

/* ========================================================= 
   initialisation */

rtems_task Init(
        rtems_task_argument ignored
)
{
        rtems_status_code status;

        printk("******** Starting Gaisler OCCAN sample ********\n");
printf("******** printf working********\n");
	
        /* Initialize Driver manager, in config.c */
        system_init();


        Task_name[1] = rtems_build_name( 'T', 'S', 'K', 'A' );
        Task_name[2] = rtems_build_name( 'T', 'S', 'K', 'B' );

        status = rtems_task_create(
                Task_name[1], 1, RTEMS_MINIMUM_STACK_SIZE * 2,
                RTEMS_DEFAULT_MODES, 
                RTEMS_DEFAULT_ATTRIBUTES, &Task_id[1]
                );

        status = rtems_task_create( 
                Task_name[2], 1, RTEMS_MINIMUM_STACK_SIZE * 2,
                RTEMS_DEFAULT_MODES,
                RTEMS_DEFAULT_ATTRIBUTES, &Task_id[2]
                );

        /* Starting receiver first */
#ifdef TASK_RX
        status = rtems_task_start(Task_id[2], task2, 2);
#endif
#ifdef TASK_TX
        status = rtems_task_start(Task_id[1], task1, 1);
#endif
        status = rtems_task_delete(RTEMS_SELF);
        
}



void task1_afilter_test(occan_t chan);
void task2_afilter_test(occan_t chan);

#define SPEED_250K 250000

#define BTR0 0x01//0x27	
#define BTR1 0x1c//0x3e


#define TSK1_RX_LEN 32
#define TSK1_TX_LEN 32

#define TSK2_RX_LEN 32
#define TSK2_TX_LEN 32

#define ID_GAISLER 0x2000

/* =========================================================  
   task1 */
	
rtems_task task1(
        rtems_task_argument unused
) 
{
	occan_t chan;
	CANMsg msgs[3];
	int i=0,j=0;
	unsigned int cstatus;
/*
	occan_stats stats;
*/
	
	
	printk("Starting task 1\n");
	/* open device */
	chan=occanlib_open(OCCAN_DEVICE_TX_NAME);
	if ( !chan ){
		printk("Failed to open device driver 0\n");
		rtems_task_delete(RTEMS_SELF);
		return;
	}

	occanlib_get_status(chan, &cstatus);
	printk("Status  %d\n", cstatus);
	
#ifdef DO_FILTER_TESTx
	/* Do filter test */
	task1_afilter_test(chan);
	
	occanlib_stop(chan);
#endif
	/* before starting set up 
	 *  � Speed
	 *  � Buffer length
	 *  � no Filter 
	 *  � blocking mode
	 */
	
	printk("Task1: Setting speed\n"); 
	occanlib_set_speed(chan,SPEED_250K);
        //occanlib_set_btrs(chan,BTR0,BTR1);
	
	printk("Task1: Setting buf len\n"); 
	occanlib_set_buf_length(chan,TSK1_RX_LEN,TSK1_TX_LEN);
	
	/* total blocking mode */
	printk("Task1: Setting blk mode\n"); 
	occanlib_set_blocking_mode(chan,0,0);	
	
	
	/* Set accept all filter */
	/* occanlib_set_filter(occan_t chan, struct occan_afilter *new_filter);*/
	
	/* Start link */
	printk("Task1: Starting\n"); 
	occanlib_start(chan);

	//while(1)
	for(j=0; j<5; j++)
	{
		/* Send 1 STD Message */
		msgs[0].extended = 0;
		msgs[0].rtr = 0;
		msgs[0].sshot = 0;
		msgs[0].id = 10+i;
		msgs[0].len = 4 + (i&1);
		msgs[0].data[0] = 0x2;
		msgs[0].data[1] = 0xc4;
		msgs[0].data[2] = 0x4f;
		msgs[0].data[3] = 0xf2;
		msgs[0].data[4] = 0x23;

	printk("Task1: sending 1 STD msg\n");
	//occanlib_get_stats(chan, &stats);
	//occanlib_stats_print(&stats);

		

	occanlib_send(chan,&msgs[0]);
	printk("Task1: sending COMPLETE\n");
		/* pause for 1 sec */
		//sleep(1);
	}
}

/* ========================================================= 
   task2 */

rtems_task task2(
        rtems_task_argument unused
) 
{
	occan_t chan;
	CANMsg msgs[3];
	int i,cnt,msgcnt;
	struct occan_afilter afilt;
	
	printk("Starting task 2\n");
	
	/* open device */
	chan=occanlib_open(OCCAN_DEVICE_RX_NAME);
	if ( !chan ){
		printk("Failed to open device driver 1\n");
		rtems_task_delete(RTEMS_SELF);
		return;
	}
#ifdef DO_FILTER_TESTx
	/* do filter test 2 */
	task2_afilter_test(chan);
	
	occanlib_stop(chan);
#endif
	/* before starting set up 
	 *  � Speed
	 *  � Buffer length
	 *  � no Filter 
	 *  � blocking mode
	 */
	
	printk("Task2: Setting speed\n"); 
	//occanlib_set_speed(chan,SPEED_250K);
	occanlib_set_btrs(chan,BTR0,BTR1);
  
	printk("Task2: Setting buf len\n"); 
	occanlib_set_buf_length(chan,TSK2_RX_LEN,TSK2_TX_LEN);
	
	/* total blocking mode */
	printk("Task2: Setting blk mode\n"); 
	occanlib_set_blocking_mode(chan,0,1);
	
	/* Set filter to accept all */
	afilt.single_mode = 1;
	afilt.code[0] = 0x00;
	afilt.code[1] = 0x00;
	afilt.code[2] = 0x00;
	afilt.code[3] = 0x00;
	afilt.mask[0] = 0xff; /* don't care */
	afilt.mask[1] = 0xff;
	afilt.mask[2] = 0xff;
	afilt.mask[3] = 0xff;
	occanlib_set_filter(chan,&afilt);
	
	/* Start link */
	printk("Task2: Starting\n"); 
	occanlib_start(chan);
	
	msgcnt=0;
	printk("Task2: Entering rx loop\n");
	while(2){
		/* blocking read */
		cnt = occanlib_recv_multiple(chan,msgs,3);
		if ( cnt > 0 ){
			printk("Task2: Got %d messages\n",cnt);
			for(i=0; i<cnt; i++){
				if ( msgs[i].id & ID_GAISLER ){
					printk("----- GAISLER MESSAGE -----\n");
				}
				print_msg(msgcnt,&msgs[i]);
				if ( msgs[i].id & ID_GAISLER ){
					printk("---------------------------\n");
				}
				msgcnt++;
			}
		}else if ( cnt < 0) {
			printk("Task2: Experienced RX error\n");
		}else{
			/* if in non-blocking mode we work with other stuff here */
			printk("Task2: waiting 1s\n");
			sleep(1);
		}
	}
}

#ifdef DO_FILTER_TESTx	
/************* acceptance filter test *************/
void task1_afilter_test(occan_t chan){
	CANMsg msgs[16];
	int left, sent, i;
	occan_stats stats;

	printk("Task1: Setting speed\n"); 
	occanlib_set_speed(chan,SPEED_250K);
	
	/* total blocking mode */
	printk("Task1: Setting blk mode\n"); 
	occanlib_set_blocking_mode(chan,0,0);
	
	printk("Task1: Setting buf len\n"); 
	occanlib_set_buf_length(chan,TSK1_RX_LEN,TSK1_TX_LEN);
	
	/* Build messages */
	for(i=0; i<16; i++){
		msgs[i].id = 0x1 << i;
		msgs[i].extended = 1;
		msgs[i].rtr = 0;
		msgs[i].sshot = 0;
		msgs[i].len = 1;
		msgs[i].data[0] = i;
		printk("Task1: Message %d: ID: 0x%x\n",i,msgs[i].id);
	}
	
	/* Start */
	occanlib_start(chan);
	
	printk("Task1: sending 16 EXT msgs\n");
	left = 16;
	while ( left > 0 ){
		sent = occanlib_send_multiple(chan,&msgs[16-left],left);
		if ( sent < 0 ){
			printk("Task1: Error, aborting 16 EXT sending\n");
			break;
		}else{
			left -= sent;
		}
	}
	
	printk("Task1: Exiting (%d)\n",left);
	sleep(4);
	printk("---------------- Task1: Printing Stats --------------\n"); 
	if ( !occanlib_get_stats(chan,&stats) ){
		occanlib_stats_print(&stats);
	}
	printk("---------------- Task1: done Printing  --------------\n"); 
}

void task2_afilter_test(occan_t chan){
	int tries, cnt, tot, i;
	CANMsg msgs[16];
	struct occan_afilter filt;
	occan_stats stats;
	
	printk("Task2: Setting speed\n"); 
	occanlib_set_speed(chan,SPEED_250K);
	
	/* total blocking mode */
	printk("Task2: Setting non-blk mode\n"); 
	occanlib_set_blocking_mode(chan,0,0);
	
	printk("Task2: Setting buf len\n"); 
	occanlib_set_buf_length(chan,TSK1_RX_LEN,TSK1_TX_LEN);
	
	/* Set up filter so that odd messages is filtered out 
	 * It can be done with a single filter.
	 * 
	 * Odd Messages: ID = 1<<i (i=odd) 0xaaaaaaa
	 * Even:         ID = 1<<i (i=even 0,2,4) 0x55555
	 *
	 * All odd bits must be zero:
	 *
	 * Mask = 0x aaaa aaaa
	 * Code = 0x0
	 *
	 */
	filt.single_mode = 1;
	filt.code[0] = 0;
	filt.code[1] = 0;
	filt.code[2] = 0;
	filt.code[3] = 0;
	filt.mask[0] = 0xaa;
	filt.mask[1] = 0xaa;
	filt.mask[2] = 0xaa;
	filt.mask[3] = 0xaf;
	occanlib_set_filter(chan,&filt);
	
	/* Start */
	occanlib_start(chan);
	
	tot=tries=0;
	while(tries < 5){
		/* Read 1 message */
		cnt = occanlib_recv_multiple(chan,msgs,16);
		printk("Task2: Got %d Message(s)\n",cnt);
		
		if ( cnt > 0 ){
			for(i=0; i<cnt; i++){
				if ( msgs[i].data[0] & 1 ){
					printk("Task2: ERROR! GOT ODD MESSAGE\n");
				}
				printk("Task2: MSG %d ID: 0x%x, Data[0]: %d(0x%x)\n",tot,msgs[i].id,msgs[i].data[0],msgs[i].data[0]);
				tot++;
			}
		}
		
		tries++;
		sleep(1);
	}
	
	if ( tot != 8 ){
		printk("Task2: Total count is wrong: %d, tries: %d\n",tot,tries);
	}
	
	printk("Task2: Exiting filtering test\n");
	printk("---------------- Task2: Printing Stats --------------\n"); 
	if ( !occanlib_get_stats(chan,&stats) ){
		occanlib_stats_print(&stats);
	}
	printk("---------------- Task2: done Printing  --------------\n"); 
	
}
#endif

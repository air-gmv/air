/* Simple GRCAN interface example.
 *
 * Two tasks are communicating. Task2 (TX) sends data to Task1 (RX) which
 * verify the data content. The RX task and the TX task collect statistics
 * which is printed periodically by a status task.  which calls
 * can_print_stats().
 *
 * In order for the example to work an external board 
 * responding to the messages sent is needed.
 * The example below expects the messages to be sent back
 * with the ID-field decremented once, all other data in 
 * message must be unmodified.
 *
 * The RX task may indicate dropped messages if the external
 * board doesn't send back all sent messages in time.
 * 
 * A software loopback (LOOP) task is also started on the second CAN device.
 * This task will just send back any CAN message is receives. It is useful for
 * testing GRCAN on a system with two GRCAN cores.
 *
 * Gaisler Research 2007,
 * Daniel Hellström
 *
 */
/*
 * NOTE: When running this example on GR740, the following have to be configured:
 * - CAN pin multiplexing
 * - Enable GRCAN clock
 *
 * This can be done in GRMON2 using the following commands before running the
 * example:
 *   grmon2> wmem 0xffa0b000 0x000ffc3c
 *   grmon2> grcg enable 5
 */

#include <rtems.h>
#define CONFIGURE_INIT
#include <bsp.h>		/* for device driver prototypes */
#include <assert.h>

rtems_task Init(rtems_task_argument argument);	/* forward declaration needed */

/* configure RTEMS kernel */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS             16
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (24 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32
#define CONFIGURE_INIT_TASK_PRIORITY	100
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_MAXIMUM_DRIVERS 16

#define RTEMS_PCI_CONFIG_LIB
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

#include <rtems/confdefs.h>

/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3)	/* if --enable-drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
 #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
 #endif /* CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER */
 #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
 #endif /* CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER */
#endif /* defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) */
//~ #define CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF	/* PCI is for GR-RASTA-IO GRCAN */
//~ #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI	/* PCI is for GR-RASTA-IO GRCAN */
//~ #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2	/* PCI is for GR-RASTA-IO GRCAN */
//~ #define CONFIGURE_DRIVER_PCI_GR_RASTA_IO	/* GR-RASTA-IO PCI TARGET has a GRCAN core */

#ifdef LEON2
 /* PCI support for AT697 */
 #define CONFIGURE_DRIVER_LEON2_AT697PCI
 /* AMBA PnP Support for GRLIB-LEON2 */
 #define CONFIGURE_DRIVER_LEON2_AMBAPP
#endif /* LEON2 */

#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN	/* GRCAN Driver */

#include <drvmgr/drvmgr_confdefs.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sched.h>
#include <ctype.h>
#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>

/* Include driver configurations and system initialization */
#include "config.c"

#include <bsp/grcan.h>

/* Select CAN core to be used in sample application.
 *  - 0                        (First CAN core)
 *  - 1                        (Second CAN core)
 *  - etc...
 */
#ifndef GRCAN_DEVICE_NUMBER
 #define GRCAN_DEVICE_NUMBER 0
#endif
/*#define GRCAN_DEVICE_NUMBER 1*/

/* Define this in order to use a single task to handle all
 * CAN communication. This is done by using grcan_read() in non-blocking mode
 * and grcan_write() in blocking mode.
 */
/* #define ONE_TASK */

/* Define this in order to only receive packets, can 
 * be usefull when debugging the receiver. This assumes
 * that the external board sends the messages by it own. 
 */
/* #define CANRX_ONLY */

/* Define this in order to only transmit packets, can 
 * be usefull when debugging the transmitter. No received
 * messages will be processed.
 */
/* #define CANTX_ONLY */

/* If RX_MESSAGES_CHANGED if defined it is assumed that the
 * received messages has been changed from the transmitted
 * message. In that case it is assumed that the ID field of
 * each message has been decremented once.
 *
 * This option is useful when the messages are looped on an
 * external CAN board. CAN is not designed to receive the
 * exact same message as is beeing transmitted:
 *  WE SEND -> CAN_BUS -> External CAN Board changes the ID 
 *  field -> CAN_BUS -> WE RECEIVE and verify message
 */
/* #define RX_MESSAGES_CHANGED_ID
 * #define RX_MESSAGES_CHANGED_DATA
 */

/* Define this to get more statistics printed to console */
#undef PRINT_MORE_STATS

/* CAN Channel select */
enum {
	CAN_CHAN_SEL_A,
	CAN_CHAN_SEL_B,
	CAN_CHAN_SEL_NUM
};
int can_chan_sel = CAN_CHAN_SEL_A;	/* Default to channel A */
int loop_chan_sel = CAN_CHAN_SEL_B;	/* Default to channel B */

#if defined(ONE_TASK) && defined(CANRX_ONLY)
#error not possible to define both ONE_TASK and CANRX_ONLY
#endif

#if defined(CANRX_ONLY) && defined(CANTX_ONLY)
#error not possible to define both CANRX_ONLY and CANTX_ONLY
#endif

#define UNUSED(var) ((void) var)
/* Status printing Task entry point */
rtems_task status_task1(rtems_task_argument argument);

/* CAN routines */
int can_init(int devno, int chan_sel);
void can_start(void);
void can_print_stats(void);

/* Loopback routines */
int loop_init(int devno, int chan_sel);
void loop_start(void);

int status_init(void);
void status_start(void);

/* ========================================================= 
   initialisation */

rtems_task Init(rtems_task_argument ignored)
{
	UNUSED(ignored);
	rtems_status_code status;
	int loop_ret;

	printf("******** Initializing CAN test ********\n");

	/* Initialize Driver manager, in config.c */
	system_init();

	drvmgr_print_devs(0xfffff);
	drvmgr_print_topo();

	/* Initialize RX/TX */
	if (can_init(GRCAN_DEVICE_NUMBER, can_chan_sel)) {
		printf("CAN INITIALIZATION FAILED, aborting\n");
		exit(1);
	}

	/* Initialize Loopback */
	loop_ret = loop_init(!GRCAN_DEVICE_NUMBER, loop_chan_sel);
	if (loop_ret) {
		printf("INFO: LOOP INITIALIZATION FAILED. Loop task deactivated.\n");
	}

	if (status_init()) {
		printf("STATUS INITIALIZATION FAILED, aborting\n");
		exit(3);
	}

	if (loop_ret == 0) {
		loop_start();
	}

	can_start();

	status_start();

	status = rtems_task_delete(RTEMS_SELF);
	assert(status == RTEMS_SUCCESSFUL);
}

rtems_id tstatus;		/* array of task ids */
rtems_name tstatusname;		/* array of task names */

int status_init(void)
{
	rtems_status_code status;

	tstatusname = rtems_build_name('S', 'T', 'S', '0');

	/*
	 * Create a status printing task with the highest priority, this may
	 * result in CAN messages may be dropped. The CAN bus has no flow
	 * control stopping when receiver is full. ==> packets may be dropped
	 * when CAN receive task doesn't get enough CPU time. Packet drop can
	 * be reduced by inserting sleep calls and increasing the receive CAN
	 * buffer size.
	 */
	status = rtems_task_create(
		tstatusname,
		4,
		RTEMS_MINIMUM_STACK_SIZE * 4,
		RTEMS_DEFAULT_MODES | RTEMS_PREEMPT,
		RTEMS_FLOATING_POINT,
		&tstatus
	);
	if (status != RTEMS_SUCCESSFUL) {
		return -1;
	}

	return 0;
}

void status_start(void)
{
	rtems_status_code status;

	printf("Starting status task1\n");

	/* Starting Status task */
	status = rtems_task_start(tstatus, status_task1, 1);
	assert(status == RTEMS_SUCCESSFUL);
}

/* Status Task */
rtems_task status_task1(rtems_task_argument unused)
{
	UNUSED(unused);

	while (1) {
		can_print_stats();
		fflush(stdout);
		sleep(2);
	}
}

/* CAN Implementation */

rtems_task canrx_task(rtems_task_argument argument);
rtems_task cantx_task(rtems_task_argument argument);

static rtems_id tds[2];		/* array of task ids */
static rtems_name tnames[2];	/* array of task names */
/* Arbitration of GRCAN device between RX and TX tasks. */
static rtems_id tx_sem;

/* File descriptors of /dev/grcan0 */
void *candev;

/* Print one CAN message to terminal */
void print_msg(int i, CANMsg * msg);

/* Initializes the 8 CAN messages in the global variable 
 * "CANMsg msgs[8]".
 */
void init_send_messages(void);

/* Verify content of CAN message 'msg' against msgs[index].
 * Returns what went wrong.
 */
int verify_msg(CANMsg * msg, int index);

/* ========================================================= 
   initialisation */

static const struct grcan_timing CAN_TIMING = {
	/* Set baud rate: 250k @ 250MHz */
	.scaler = 0x27,
	.ps1 = 0xf,
	.ps2 = 0x8,
	.rsj = 0x1,
	.bpr = 0x0,
};

static const struct grcan_selection CAN_CHAN_SEL[CAN_CHAN_SEL_NUM] = {
	{
		/* Channel A */
		.selection = 0,
		.enable0 = 0,
		.enable1 = 1,
	},
	{
		/* Channel B */
		.selection = 1,
		.enable0 = 1,
		.enable1 = 0,
	},
};

static int can_init_dev(int devno, int chan_sel, void **dev_new);

int can_init(int devno, int chan_sel)
{
	rtems_status_code status;
	int i;

	printf("******** Initializing GRCAN test ********\n");

	for (i = 0; i < 2; i++) {
		tnames[i] = rtems_build_name('T', 'D', 'C', '0' + i);
	}

  /*** Create, but do not start, CAN RX/TX tasks ***/
#if !defined(CANTX_ONLY)
	/* RX task */
	status = rtems_task_create(tnames[0], 2, RTEMS_MINIMUM_STACK_SIZE * 4,
				   RTEMS_DEFAULT_MODES,
				   RTEMS_FLOATING_POINT, &tds[0]
	    );
	assert(status == RTEMS_SUCCESSFUL);
#endif

#if !defined(CANRX_ONLY) && !defined(ONE_TASK)
	/* TX task */
	status = rtems_task_create(tnames[1], 5, RTEMS_MINIMUM_STACK_SIZE * 4,
				   RTEMS_DEFAULT_MODES,
				   RTEMS_FLOATING_POINT, &tds[1]
	    );
	assert(status == RTEMS_SUCCESSFUL);

	if ( rtems_semaphore_create(rtems_build_name('S', 'T', 'X', '0'),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&tx_sem) != RTEMS_SUCCESSFUL ) {
		return -2;
	}
#endif

	return can_init_dev(devno, chan_sel, &candev);
}

static rtems_id tloop;
static rtems_name tloopname;
rtems_task loop_task1(rtems_task_argument argument);

struct loop_stats {
	unsigned int received;
	unsigned int sent;
	unsigned int rx_err;
};
struct loop_ctx {
	struct loop_stats stats;
	void *dev;
	CANMsg msg[50];
};

struct loop_ctx _loop_ctx;

/* Loopback task */
rtems_task loop_task1(rtems_task_argument arg)
{
	int rxmsgs;
	int txmsgs;
	int rxbytes;
	int txbytes;
	struct loop_ctx *ctx;

	ctx = (struct loop_ctx *) arg;

	iprintf("%s: Entering CAN Loop-Back loop\n", __func__);

	while (1) {
		/* blocking read */
		rxbytes = grcan_read(ctx->dev, &ctx->msg[0], 40);
		if (rxbytes > 0) {
			rxmsgs = rxbytes;
			ctx->stats.received += rxmsgs;
			/* Just try send if possible */
			txbytes = grcan_write(ctx->dev, &ctx->msg[0], rxbytes);
			if (txbytes > 0) {
				txmsgs = txbytes;
				ctx->stats.sent += txmsgs;
			} else {
				grcan_stop(ctx->dev);
				grcan_start(ctx->dev);
			}
		} else if ((rxbytes == -2) || (rxbytes == -4)) {
			iprintf("%s: Experienced RX error (%d) recovering with grcan_start()\n", __func__, rxbytes);
			/* OK to start since this task is the only user of ctx->dev. */
			grcan_stop(ctx->dev);
			grcan_start(ctx->dev);
			ctx->stats.rx_err++;
			sleep(1);
		} else {
			/* if in non-blocking mode we work with other stuff here */
			iprintf("%s: RX blocking not working\n", __func__);
			ctx->stats.rx_err++;
			sleep(1);
		}
	}

	while (1) {
		iprintf("%s: Sleeping loop task\n", __func__);
		sleep(1);
	}
}


int loop_init(int devno, int chan_sel)
{
	rtems_status_code status;

	printf("******** Initializing GRCAN loopback ********\n");

	tloopname = rtems_build_name('L', 'O', 'O', 'P');

	/*** Create, but do not start, CAN loop task ***/

	status = rtems_task_create(
		tloopname,
		/*
		 * Valid task priorities range from a high of 1 to a low of
		 * 255.
		 */
		3,
		RTEMS_MINIMUM_STACK_SIZE * 4,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&tloop
	);
	assert(status == RTEMS_SUCCESSFUL);

	return can_init_dev(devno, chan_sel, &_loop_ctx.dev);
}

/*
 * Open and bring up a can device in a sane configuration.
 *
 * This function does not change global variables, except via dev_new.
 */
static int can_init_dev(int devno, int chan_sel, void **dev_new)
{
        int ret;
	const struct grcan_selection *selection;
	void *dev;

	if (!dev_new) {
		return -1;
	}
	*dev_new = NULL;

	printf("Opening GRCAN device %d\n", devno);
	dev = grcan_open(devno);
	if (NULL == dev) {
		printf("Failed to open GRCAN device %d\n", devno);
		return -1;
	}

	/* Start GRCAN driver */

	/* Select CAN channel */
	if (chan_sel == CAN_CHAN_SEL_A) {
		selection = &CAN_CHAN_SEL[CAN_CHAN_SEL_A];
	} else {
		selection = &CAN_CHAN_SEL[CAN_CHAN_SEL_B];
	}

	/* Set up CAN driver:
	 *  ¤ baud rate
	 *  ¤ Channel
	 *  ¤ TX blocking, and wait for all data to be sent.
	 *  ¤ RX non-blocking depending on ONE_TASK mode
	 */
	/* Set baud */
	ret = grcan_set_btrs(dev, &CAN_TIMING);
	if (ret) {
		printf("grcan_set_btrs() failed: %d\n", ret);
		return -1;
	}
	ret = grcan_set_selection(dev, selection);
	if (ret) {
		printf("grcan_set_selection() failed: %d\n", ret);
		return -1;
	}

	ret = grcan_set_txcomplete(dev, 1);
	if (ret) {
		printf("grcan_set_txcomplete() failed: %d\n", ret);
		return -1;
	}

	ret = grcan_set_rxcomplete(dev, 0);
	if (ret) {
		printf("grcan_set_rxcomplete() failed: %d\n", ret);
		return -1;
	}

#ifdef ONE_TASK
	/* in one task mode, we want TX to block instead */
	ret = grcan_set_rxblock(dev, 0);
#else
	/* in two task mode, we want TX _and_ RX to block */
	ret = grcan_set_rxblock(dev, 1);
#endif
	if (ret) {
		printf("grcan_set_rxblock() failed: %d\n", ret);
		return -1;
	}

	ret = grcan_set_txblock(dev, 1);
	if (ret) {
		printf("grcan_set_txblock() failed: %d\n", ret);
		return -1;
	}

	ret = grcan_clr_stats(dev);
	if (ret) {
		printf("grcan_clr_stats() failed: %d\n", ret);
		return -1;
	}

	/* Start communication */
	ret = grcan_start(dev);
	if (ret) {
		printf("grcan_start() failed: %d\n", ret);
		return -1;
	}

	*dev_new = dev;

	return 0;
}

void can_start(void)
{
	rtems_status_code status;

	/* Starting receiver first */
#ifndef CANTX_ONLY
	status = rtems_task_start(tds[0], canrx_task, 1);
	assert(status == RTEMS_SUCCESSFUL);
#endif
#if !defined(CANRX_ONLY) && !defined(ONE_TASK)
	status = rtems_task_start(tds[1], cantx_task, 1);
	assert(status == RTEMS_SUCCESSFUL);
#endif
}

void loop_start(void)
{
	rtems_status_code status;

	/* Starting loop task */
	status = rtems_task_start(
		tloop,
		loop_task1,
		(rtems_task_argument) &_loop_ctx
	);
	assert(status == RTEMS_SUCCESSFUL);
}

#define ID_GAISLER 0x2000

static CANMsg msgs[8];

void init_send_messages(void)
{
	/* Send 1 STD Message */
	msgs[0].extended = 0;
	msgs[0].rtr = 0;
	msgs[0].unused = 0;
	msgs[0].id = 10;
	msgs[0].len = 4;
	msgs[0].data[0] = 0x2;
	msgs[0].data[1] = 0xc4;
	msgs[0].data[2] = 0x4f;
	msgs[0].data[3] = 0xf2;
	msgs[0].data[4] = 0x23;

	/* Send 3 EXT Message */
	msgs[1].extended = 1;
	msgs[1].rtr = 0;
	msgs[1].unused = 0;
	msgs[1].id = 10;
	msgs[1].len = 4 + 1;
	msgs[1].data[0] = 0x2;
	msgs[1].data[1] = 0xc4;
	msgs[1].data[2] = 0x4f;
	msgs[1].data[3] = 0xf2;
	msgs[1].data[4] = 0x23;
	msgs[1].data[5] = 0xa2;

	msgs[2].extended = 1;
	msgs[2].rtr = 0;
	msgs[2].unused = 0;
	msgs[2].id = 10 + 880;
	msgs[2].len = 8;
	msgs[2].data[0] = 0xaa;
	msgs[2].data[1] = 0xbb;
	msgs[2].data[2] = 0x11;
	msgs[2].data[3] = 0x22;
	msgs[2].data[4] = 'U';
	msgs[2].data[5] = 0x12;
	msgs[2].data[6] = 0xff;
	msgs[2].data[7] = 0x00;

	msgs[3].extended = 1;
	msgs[3].rtr = 0;
	msgs[3].unused = 0;
	msgs[3].id = 0xff | ID_GAISLER;
	msgs[3].len = 7;
	msgs[3].data[0] = 'G';
	msgs[3].data[1] = 'a';
	msgs[3].data[2] = 'i';
	msgs[3].data[3] = 's';
	msgs[3].data[4] = 'l';
	msgs[3].data[5] = 'e';
	msgs[3].data[6] = 'r';

	/* Send 1 STD Message */
	msgs[4].extended = 0;
	msgs[4].rtr = 0;
	msgs[4].unused = 0;
	msgs[4].id = 10;
	msgs[4].len = 4;
	msgs[4].data[0] = 0x2;
	msgs[4].data[1] = 0xc4;
	msgs[4].data[2] = 0x4f;
	msgs[4].data[3] = 0xf2;
	msgs[4].data[4] = 0x23;

	/* Send 3 EXT Message */
	msgs[5].extended = 1;
	msgs[5].rtr = 0;
	msgs[5].unused = 0;
	msgs[5].id = 10;
	msgs[5].len = 4 + 1;
	msgs[5].data[0] = 0x2;
	msgs[5].data[1] = 0xc4;
	msgs[5].data[2] = 0x4f;
	msgs[5].data[3] = 0xf2;
	msgs[5].data[4] = 0x23;
	msgs[5].data[5] = 0xa2;

	msgs[6].extended = 1;
	msgs[6].rtr = 0;
	msgs[6].unused = 0;
	msgs[6].id = 10 + 880;
	msgs[6].len = 8;
	msgs[6].data[0] = 0xaa;
	msgs[6].data[1] = 0xbb;
	msgs[6].data[2] = 0x11;
	msgs[6].data[3] = 0x22;
	msgs[6].data[4] = 'U';
	msgs[6].data[5] = 0x12;
	msgs[6].data[6] = 0xff;
	msgs[6].data[7] = 0x00;

	msgs[7].extended = 1;
	msgs[7].rtr = 0;
	msgs[7].unused = 0;
	msgs[7].id = 0xff | ID_GAISLER;
	msgs[7].len = 7;
	msgs[7].data[0] = 'G';
	msgs[7].data[1] = 'a';
	msgs[7].data[2] = 'i';
	msgs[7].data[3] = 's';
	msgs[7].data[4] = 'l';
	msgs[7].data[5] = 'e';
	msgs[7].data[6] = 'r';
}

/* Verify content of a CAN message */
int verify_msg(CANMsg * msg, int index)
{
	int i;
	CANMsg *src = &msgs[index];

	if (
		(msg->extended && !src->extended) ||
		(!msg->extended && src->extended)
	) {
		printf(
			"Expected extended=%d but got extended=%d\n",
			src->extended,
			msg->extended
		);
		return -1;
	}

	if (msg->rtr != src->rtr) {
		/* Remote Transmission Request does not match */
		return -2;
	}
#ifdef RX_MESSAGES_CHANGED_ID
	/* Decremented the ID once */
	if (msg->id != (src->id - 1)) {
#else
	if (msg->id != src->id) {
#endif
		printf("Expected id=0x%x but got id=0x%x\n", src->id, msg->id);
		return -3;
	}

	if (msg->len != src->len) {
		printf("Expected len=%d but got len=%d\n", src->len, msg->len);
		return -4;
	}
	for (i = 0; i < msg->len; i++) {
#ifdef RX_MESSAGES_CHANGED_DATA
		if (msg->data[i] != (src->data[i] + 1))
#else
		if (msg->data[i] != src->data[i])
#endif
			return -5 - i;
	}
	return 0;
}

/* Staticstics */
static volatile int rxpkts = 0, txpkts = 0;
/*
 * rx_syncs: Number of times an unexpected packet is received.
 * rx_errors: Number of packets not matching a packet known by the example.
 */
static volatile int rx_syncs = 0, rx_errors = 0;

/* RX Task */
rtems_task canrx_task(rtems_task_argument unused)
{
	UNUSED(unused);
	CANMsg rxmsgs[10];
	int i, j, cnt, index = 0, error, e;

#ifdef ONE_TASK
	int wcnt = 0;
	int wofs = 0;
#endif
#ifdef WAIT_AFTER_256_MSGS
	int last = 0;
#endif

#if defined(CANRX_ONLY) || defined(ONE_TASK)
	printf("Initing messages\n");
	init_send_messages();
#endif

#ifdef ONE_TASK
	printf
	    ("************** MESSAGES THAT WILL BE TRANSMITTED *************\n");
	print_msg(1, &msgs[0]);
	print_msg(2, &msgs[1]);
	print_msg(3, &msgs[2]);
	print_msg(4, &msgs[3]);
	printf
	    ("**************************************************************\n");

	printf
	    ("******************* Start of transmission ********************\n");
#endif

#ifdef ONE_TASK
	txpkts = 0;
#endif

	while (1) {
		cnt = grcan_read(candev, rxmsgs, 10);
		if (cnt < 1) {

#ifdef ONE_TASK
			if (cnt < 0) {
				printf("%s: got BUSOFF, doing error recovery with grcan_start()\n", __func__);
				grcan_stop(candev);
				grcan_start(candev);
				continue;
			}
			/* In non-blocking RX mode...
			 * Send messages while instead of blocking read waiting
			 * for new messages to arrive.
			 *
			 * TX is blocking and waiting to complete.
			 */
			wcnt = grcan_write(candev, &msgs[ofs], 4);
			if (wcnt > 0) {
				txpkts += wcnt;
				ofs += wcnt;
				if (ofs > 3) {
					ofs -= 4;
				}
			} else {
				sched_yield();
			}

#ifdef WAIT_AFTER_256_MSGS
			/* Wait a bit after each 256 messages */
			if ((txpkts & 0x100) && !(last & 0x100)) {
				rtems_task_wake_after(4);
			}
			last = txpkts;
#endif

			continue;

#else
			/* blocking mode: should not fail unless CAN errors.
			 * In this example we only handle bus-off errors
			 */
			if (cnt < 0) {
				rtems_status_code status;

				printf("%s: read returned %d, doing error recovery with grcan_start()\n", __func__, cnt);
				status = rtems_semaphore_obtain(tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
				if (status != RTEMS_SUCCESSFUL) {
					printf("%s: FAILURE: rtems_semaphore_obtain(tx_sem) failed\n", __func__);
					exit(1);
				}
				grcan_stop(candev);
				grcan_start(candev);
				rtems_semaphore_release(tx_sem);
			} else {
				printf("CAN grcan_read() failed: %d\n", cnt);
				break;
			}
			continue;
#endif
		}

		/* Statistics */
		rxpkts += cnt;

		/* For every message received we compare the content against
		 * expected content.
		 *
		 * If a message have been dropped we synchronize with the
		 * message stream to avoid getting multiple errors from one
		 * dropped message.
		 *
		 */
		for (i = 0; i < (int) (cnt); i++) {
			error = verify_msg(&rxmsgs[i], index);
			if (error) {
				printf("Message rx error: %d, index: %d\n", error, index);

				/* Print message */
				print_msg(0, &rxmsgs[i]);

				/* Try to sync if one has been lost */
				e = 0;
				for (j = 0; j < 4; j++) {
					if (!verify_msg(&rxmsgs[i], j)) {
						printf ("Synced from message %d to %d\n", index, j);
						rx_syncs++;
						index = j;
						e = 1;
						break;
					}
				}
				if (e != 1) {
					rx_errors++;
				}
			}
			index++;
			if (index > 3) {
				index = 0;
			}
		}
	}

	while (1) {
		printf("Sleeping Task1\n");
		sleep(1);
	}
}

/* TX Task */
rtems_task cantx_task(rtems_task_argument unused)
{
	UNUSED(unused);
	int cnt, ofs;
#ifdef WAIT_AFTER_256_MSGS
	int last = 0;
#endif
	/* Print messages that we be sent to console */
	printf
	    ("************** MESSAGES THAT WILL BE TRANSMITTED *************\n");
	init_send_messages();
	print_msg(1, &msgs[0]);
	print_msg(2, &msgs[1]);
	print_msg(3, &msgs[2]);
	print_msg(4, &msgs[3]);
	printf
	    ("**************************************************************\n");

	printf
	    ("******************* Start of transmission ********************\n");

	txpkts = 0;
	ofs = 0;
	while (1) {
		rtems_status_code status;

		/*
		 * Protect the candev with a semaphore so that canrx_task() can
		 * ensure we are not in the GRCAN driver with it.
		 */
		status = rtems_semaphore_obtain(tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		if (status != RTEMS_SUCCESSFUL) {
			printf("FAILURE: rtems_semaphore_obtain(tx_sem) failed\n");
			exit(1);
		}

		/* Blocking transmit request. Returns when all messages
		 * requested has been scheduled for transmission (not actually
		 * sent, but taken care of by driver).
		 */
		cnt = grcan_write(candev, &msgs[ofs], 4);
		rtems_semaphore_release(tx_sem);
		if (cnt > 0) {
			/* Increment statistics */
			txpkts += cnt;
			ofs += cnt;
			if (ofs > 3) {
				ofs -= 4;
			}
		} else {
			sched_yield();
			printf("TX CAN TASK: write failed: %d\n", cnt);
			/* NOTE: RX task is responsible for handling the error */
		}

#ifdef WAIT_AFTER_256_MSGS
		/* Wait a bit after each 256 messages */
		if ((txpkts & 0x100) && !(last & 0x100)) {
			rtems_task_wake_after(4);
		}
		last = txpkts;
#endif
		rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
	}

	while (1) {
		printf("Sleeping Task 2\n");
		sleep(1);
	}
}

/* CAN HELP DEBUG FUNCTIONS */
char *msgstr_type[2] = { "STD", "EXT" };
char *msgstr_rtr[2] = { "", " RTR" };

/* PRINT A CAN MESSAGE FROM DATA STRUCTURE */
void print_msg(int i, CANMsg * msg)
{
	int j;
	char data_str_buf[64];
	int ofs;

	if (!msg)
		return;

	if (i > 0) {
		printf("MSG[%d]: %s%s length: %d, id: 0x%x\n", i,
		       msgstr_type[(int)msg->extended],
		       msgstr_rtr[(int)msg->rtr], msg->len, msg->id);
		/* print data */
		if (msg->len > 0) {
			ofs = sprintf(data_str_buf, "MSGDATA[%d]: ", i);
			for (j = 0; j < msg->len; j++) {
				ofs +=
				    sprintf(data_str_buf + ofs, "0x%02x ",
					    msg->data[j]);
			}
			printf("%s  ", data_str_buf);
			ofs = 0;
			for (j = 0; j < msg->len; j++) {
				if (isalnum(msg->data[j]))
					ofs +=
					    sprintf(data_str_buf + ofs, "%c",
						    msg->data[j]);
				else
					ofs += sprintf(data_str_buf + ofs, ".");
			}
			printf("%s\n", data_str_buf);
		}
	} else {
		printf("MSG: %s%s length: %d, id: 0x%x\n",
		       msgstr_type[(int)msg->extended],
		       msgstr_rtr[(int)msg->rtr], msg->len, msg->id);
		/* print data */
		if (msg->len > 0) {
			ofs = sprintf(data_str_buf, "MSGDATA: ");
			for (j = 0; j < msg->len; j++) {
				ofs +=
				    sprintf(data_str_buf + ofs, "0x%02x ",
					    msg->data[j]);
			}
			printf("%s  ", data_str_buf);
			ofs = 0;
			for (j = 0; j < msg->len; j++) {
				if (isalnum(msg->data[j]))
					ofs +=
					    sprintf(data_str_buf + ofs, "%c",
						    msg->data[j]);
				else
					ofs += sprintf(data_str_buf + ofs, ".");
			}
			printf("%s\n", data_str_buf);
		}
	}
}

/* Print statistics gathered by RX and TX tasks, also
 * print statistics from driver.
 */
void can_print_stats(void)
{
	struct grcan_stats stats;
	static int cnt = 0;
	int ret;

	/* Get stats from GRCAN driver to print */
	ret = grcan_get_stats(candev, &stats);
	if (ret) {
		printf("grcan_get_stats() failed: %d (continue anyway)\n", ret);
	} else {
		/* Got stats from driver */

#ifdef PRINT_MORE_STATS
		/* Print extra stats */
		printf("CAN PASSV:   %7d\n", stats.passive_cnt);
		rtems_task_wake_after(4);
		printf("CAN OVERRUN: %7d\n", stats.overrun_cnt);
		rtems_task_wake_after(4);
		printf("CAN TXLOSS:  %7d\n", stats.txloss_cnt);
		rtems_task_wake_after(4);
		if (stats.busoff_cnt) {
			printf("CAN BUSOFF:  %7d\n", stats.busoff_cnt);
			rtems_task_wake_after(4);
		}
#endif
		if (stats.ahberr_cnt) {
			printf("CAN AHB:     %7d\n", stats.ahberr_cnt);
		}
		printf("CAN INTS:    %7d\n", stats.ints);
		rtems_task_wake_after(4);
	}
	printf("CAN RXPKTS:  %7d\n", rxpkts);
	rtems_task_wake_after(4);

	/* Print only number of RX syncs every tenth time */
	if (cnt++ >= 10) {
		cnt = 0;
		printf("CAN RXSYNCS: %7d\n", rx_syncs);
		if (rx_errors > 0) {
			printf("CAN RXERRORS: %7d\n", rx_errors);
		}
	}

	printf("CAN TXPKTS:  %7d  (TX-RX: %7d)\n", txpkts, txpkts-rxpkts);
	rtems_task_wake_after(4);
}


/*
 * A collection of hacks, glue, and patches to
 * provide a `UNIX-like' environment to ttcp.
 *
 * Some of the code here may migrate to the libc
 * support routines some day.  Some of the more sleazy
 * hacks should never make it outside this file!
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  rtems_ttcp.c,v 1.7 1999/04/01 18:19:08 joel Exp
 */


/*
 * RTEMS configuration/initialization
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *    1. Leave this author information intact.
 *    2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  init.c,v 1.8 2003/01/28 22:21:21 joel Exp
 */
#define CONFIGURE_INIT
#define GMVNGMP

#include <bsp.h>
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE    (1024*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES    20
#define CONFIGURE_MAXIMUM_TASKS        20

#define CONFIGURE_INIT_TASK_STACK_SIZE    (64*1024)
#define CONFIGURE_INIT_TASK_PRIORITY    100
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define RTEMS_PCI_CONFIG_LIB
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

///////////// NEW DEFS
   #define CONFIGURE_MAXIMUM_DRIVERS                     16
   #define CONFIGURE_MAXIMUM_TIMERS                      40




// LLLG XXX
//rtems_task Init (rtems_task_argument argument);


/* Configure RTEMS */
#include <rtems/confdefs.h>

#include <drvmgr/drvmgr.h>

/* Configure Driver manager */
//#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) /* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
 //#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
 //#endif
// #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  //#define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART // it fails!
 //#endif
//#endif
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF    /* PCI is for RASTA-IO/TMTC/.. GRETH */
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI   /* PCI is for RASTA-IO/TMTC/.. GRETH */
//#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2  /* PCI is for RASTA-IO/TMTC/.. GRETH */
//#define CONFIGURE_DRIVER_PCI_GR_RASTA_IO        /* GR-RASTA-IO has a GRETH network MAC */
//#define CONFIGURE_DRIVER_PCI_GR_RASTA_ADCDAC    /* GR-RASTA-ADCDAC has a GRETH network MAC */
//#define CONFIGURE_DRIVER_PCI_GR_LEON4_N2X       /* GR-CPCI-LEON4-N2X has two GRETH network MACs */


#include <drvmgr/drvmgr_confdefs.h>

/* Application */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>

#include "rtems-ttcp.h"

/* This is used in config.c to set up networking */
#define ENABLE_NETWORK
//#define ENABLE_NETWORK_SMC_LEON2
/* Define this is a SMC91C111 CHIP is available on the I/O bus */
/*#define ENABLE_NETWORK_SMC_LEON3*/

/* Include driver configurations and system initialization */
#include "config.c"


/*
 * RTEMS Startup Task
 */

rtems_task
Init (rtems_task_argument ignored)
{
    rtems_interrupt_level level;

    printk("ttcp: WELLCOME!\n");

    rtems_interrupt_local_disable(level);
    rtems_interrupt_local_enable(level);

#ifdef RTEMS_DRVMGR_STARTUP
    printk("ttcp: driver manager enabled\n");
#endif

#ifdef GTEST /* LLLG XXX */
    int argc = 1;
    int rc = 0;
    char argument[32] = "rtems-ttcp_ut.exe";
    char *argv[3];

printk(">> InitGoogleTest()\n");
    argv[0] = argument;
    ::testing::InitGoogleTest(&argc, argv);
printk(">> RUN_ALL_TESTS()\n");
    rc = RUN_ALL_TESTS();
printk(">> %d = RUN_ALL_TESTS()\n", rc);

#else /* GTEST LLLG XXX */

    /* Initialize Driver manager and Networking, in config.c */
printk(">>system_init()\n");
    system_init();

    /* Run ttcp */
printk(">>test_network()\n");
    test_network();

#endif /* GTEST LLLG XXX */

printk(">>exit()\n");
    exit(0);
}

/*
 * Test network throughput
 */
void test_network(void)
{
    int       len;
    int s;
    int       udp_port = 0;
    //struct in_addr ipaddress;
    struct sockaddr_in sa, myAddr;
    char buf[6] = "ABCDE";
    unsigned long addr_tmp;
    int sockbufsize = 5000;

printk("START test_network()\n");
    udp_port= 55055; // UDP

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &sockbufsize, sizeof sockbufsize);
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof sockbufsize);
    

///// bind    
     memset(&myAddr, 0, sizeof myAddr);
     myAddr.sin_family = AF_INET;
     myAddr.sin_port = htons(5500);
#ifdef GMVNGMP
     myAddr.sin_addr.s_addr = inet_addr("192.168.0.30");
#else
     myAddr.sin_addr.s_addr = inet_addr("10.0.0.40");
#endif    
     
     if (bind(s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
     {
         printk("Can't bind socket: ");
            
     }
/////// end bind    
    
    memset (&sa, 0, sizeof sa);
    sa.sin_port = htons(udp_port);
    sa.sin_family = AF_INET;   
#ifdef GMVNGMP
    addr_tmp= inet_addr("192.168.0.3");
#else
    addr_tmp= inet_addr("10.0.0.190");
#endif
    sa.sin_addr.s_addr = addr_tmp;
   
    while (1)
    {    
#ifdef GMVNGMP
        printk("[ETH(SENDER)] connected UDP to: [%s:%d] socket %d\n", "192.168.0.3", udp_port, s);    
#else
        printk("[ETH(SENDER)] connected UDP to: [%s:%d] socket %d\n", "10.0.0.190", udp_port, s);    
#endif
        printk("[ETH(SENDER)] Sending to socket %d, msg -> %s\n", s, buf);

        len = sendto(s, buf, 5, 0, (struct sockaddr *) &sa, sizeof(struct sockaddr_in));
        if (len <= 1)
        {
            printk("[ETH(SENDER)] Error:");
        }

        rtems_task_wake_after(100);
    }   

    printk ("The end, everything ok\n");
}

#ifdef GTEST /* LLLG XXX */

/*
 * Test network throughput
 */
int yadayadayada(int value)
{
printk(">>\t>>\t>> yadayadayada(%d)\n", value);
    return value;
}

#endif /* GTEST LLLG XXX */

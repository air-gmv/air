#ifdef _rtems_app

//#include <system.h>

/* Application */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <rtems.h>
//#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
//#include <sys/socket.h> 
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
//#include <rtems/dosfs.h>
#include <rtems/bdpart.h>
#include <rtems/libcsupport.h>
#include <rtems/fsmount.h>
#include <sys/select.h>
#include <rtems/rtc.h>
#include <rtems/shell.h>

#include <time.h>
#include <sys/resource.h>

//#include <bsp/grspw_pkt.h>
//#include "grspw_pkt_lib.h"
//#include <bsp/grspw_router.h>
//#include <spwinterface.h>
#include <manag.h>
#include <interface.h>
#include <pthread.h>
#include <rtems/thread.h>
#include <rtems.h>

#include "siria.h"
#include <configuration.h>
#include <dataBuffer.h>
#include <init.h>

#include <a653.h>
#include <imaspex.h>


  #define CONFIGURE_MINIMUM_TASK_STACK_SIZE CPU_STACK_MINIMUM_SIZE

/* DMS-IOP queue (TM/TC) inter-partition port ids */
static SAMPLING_PORT_ID_TYPE QRECV_IOP;
static SAMPLING_PORT_ID_TYPE QSEND_IOP;
/* inter-partition ports maximum size definitions */
#define MAX_MESSAGE_SIZE1 65000// bytes
#define MAX_MESSAGE_SIZE2 7000// bytes

#define MAX_NB_MESSAGES 20 // queue messages
#define REFRESH_RATE 250000000

#ifdef RTEMS_SMP
#warning " SMP support is enabled "
#else
#warning " SMP support is disabled"
#endif

/////////////////////////////////////////
// Socket management
/////////////////////////////////////////
int sd;
//struct sockaddr_in serveraddr;
//int sockaddrlen = (int)sizeof(struct sockaddr_in);
//struct sockaddr_in clientaddr;

int udp_port = 0;
unsigned long addr_tmp;
int sockbufsize = MAX_BUFFER_SIZE;
//
#else

#include <interface.h>

#include <configuration.h>
#include <init.h>
#include <manag.h>
#include <dataBuffer.h>
#include <Defs_nav.h>
#include <fft.h>

/////////////////////////////////////////
// Socket management
/////////////////////////////////////////
int sd;
struct sockaddr_in serveraddr;
int sockaddrlen = (int)sizeof(struct sockaddr_in);
struct sockaddr_in clientaddr;
#endif

//*****************************************************************************
//                        GLOBAL VARIABLES
//*****************************************************************************
S_gnssProcessingStatus	gnssProcessingStatus;	/* GNSS processing status								*/
S_rxProcessingStatus	rxProcessingStatus;	/* GNSS processing status								*/
S_AcqTckAid				AcqTckAid;

T_Config			    Config;					/* Configuration parameters provided by the user		*/
T_ComData			    ComData;					/* Caj�n de sastre, auxiliary constants					*/

T_DAid				    aidingData;				/* Aiding information (for assistance)					*/

/* Signal-buffer related data							*/
circularBufferStruct	Core0DataBuf;        // CPU0
circularBufferStruct	Core1DataBuf;        // CPU1
circularBufferStruct	Core2DataBuf;        // CPU2
circularBufferStruct	Core3DataBuf;        // CPU3

S_MeasProcess		    MeasProcessComb;			/* Measurements to be used for PVT.
													In dual mode, it includes both GPS+GAL measurements:
													GPS = 0:32; GAL = 35:40								*/

S_PVT_Process		    pvtProcess;				/* Structure to manage the PVT computation process		*/

S_TimeStamp			    timeStamp;					/* Time-stamp data for one epoch						*/
subframeAuxStructure    subframeAux[MAXNUMSAT];

/*******************************************************/
/* Structures that must be instanced per constellation */
/*******************************************************/

// Structures that are always declared, in single or dual modes.
// In dual mode, they are assigned to GPS (even if no "GPS" label is in them)
T_GenericSystemData	    genSysData [NSYSTEMS];			 /* Characteristic constants for one constellation	 */
T_SatManager		    satManag   [NSYSTEMS];			 /* Satellite-management for one constellation.		 */
S_MeasProcess		    MeasProcess[NSYSTEMS];			 /* Measurements for one constellation.				 */
T_supportData		    supportData[NSYSTEMS];			 /* Superstructure that manages all magnitudes for one constellation */
T_ChData                chDataAll;				 					/* Array of tracking channels for all constellations.*/
char                    configFileName[200];

/////////////////////////////////////////
// External global structures
/////////////////////////////////////////
extern S_Core        core[NCORES];


extern ioBuffer_t	 ioBuffer;
 
extern int Tck1_finish;
extern int Tck2_finish;
extern int Tck3_finish;
/////////////////////////////////////////
// Data buffer management
/////////////////////////////////////////
E_ExecutionStatus    executionStatus;        // Execution status.

#define TCSIZE_CNF         6584//304
#define TCSIZE_SP_ALMANAC 2920
#define TCSIZE_NF 794

char buffer[MAX_BUFFER_SIZE]; 
static char send_buffer[MAX_BUFFER_SIZE]; /* buffer to send data via udp */

char test_out[MAX_BUFFER_SIZE];
int idx_test_out = 0;

int almanacIndexBuffer = 0;
int kk = 0;
int idx = 0;
int SecondBuff = 0;
int pvt_data_ready = 0;
int nBytesToRead;
 int iii = 0;
int memory_bytes_sum = 0;

#ifndef _rtems_app
    #include <windows.h>
    WSADATA wsaData;
#endif

extern char  configFileName[200];
int pakiet = 0;
int first_pos_fix = 1;
extern int bylazamiana;
#ifdef PROFILER
//
#ifndef _rtems_app
//
LARGE_INTEGER StartingTime, EndingTime; // Total time
LARGE_INTEGER StartingTime_pvt, EndingTime_pvt; // PVT time
//
#else // #ifndef _rtems_app
// RTEMS
struct timeval StartingTime, EndingTime; // Total time
struct timeval StartingTime_pvt, EndingTime_pvt; // PVT time
//
#endif // #ifndef _rtems_app
//

#ifdef _rtems_app
//
struct timeval MainLoop_StartTime;
struct timeval AcquisitionTrackingAid_StartTime;
struct timeval ManageFftAcquisition_StartTime;

struct timeval galManagerSignalProcessing_StartTime;

struct timeval ManageLoopTracking1_StartTime;
struct timeval ManageLoopTracking2_StartTime;
struct timeval ManageLoopTracking3_StartTime;

struct timeval FftAcq_StartTime;
struct timeval gpsVtAcq_StartTime;
struct timeval galVtAcq_StartTime;

struct timeval Tck1_StartTime;
struct timeval Tck2_StartTime;
struct timeval Tck3_StartTime;

struct timeval startReceiverProcessing_StartTime;
struct timeval generalManagerRx_StartTime;
struct timeval ManageFftAcquisition_StartTime;
struct timeval NewVtSatellites_StartTime;
struct timeval manageLoopTracking_StartTime;
struct timeval manageMeasAndPvt_StartTime;
//
#else
//
LARGE_INTEGER startReceiverProcessing_StartTime;
LARGE_INTEGER generalManagerRx_StartTime;
LARGE_INTEGER ManageFftAcquisition_StartTime;
LARGE_INTEGER NewVtSatellites_StartTime;
LARGE_INTEGER manageLoopTracking_StartTime;
LARGE_INTEGER manageMeasAndPvt_StartTime;
#endif
//
double MainLoop_time = 0;
double AcquisitionTrackingAid_time = 0;
double ManageFftAcquisition_time = 0;
double galManagerSignalProcessing_time = 0;

double ManageLoopTracking1_time = 0;
double ManageLoopTracking2_time = 0; 
double ManageLoopTracking3_time = 0; 

double gpsFftAcq_time = 0;
double galFftAcq_time = 0;

double gpsVtAcq_time = 0;
double galVtAcq_time = 0;


Tck_info Tck_info_tab[10][MAXNUMCHAN];

int    gpssatTckSync_tab[12];
int    galsatTckSync_tab[12];
int    gpssatTckNoSync_tab[12];
int    galsatTckNoSync_tab[12];
double gpsTckSync_time_tab[12];
double galTckSync_time_tab[12];
double gpsTckNoSync_time_tab[12];
double galTckNoSync_time_tab[12];

double TckSync_time;
double TckNoSync_time;

double startReceiverProcessing_time = 0;
double generalManagerRx_time = 0;
//double ManageFftAcquisition_time = 0;
double NewVtSatellites_time = 0;
double manageLoopTracking_time = 0;
double manageMeasAndPvt_time = 0;

int acq_svn = 0;
int acq_system_flag = 0;
//
double ElapsedSeconds_pvt=0;

//
#ifdef _rtems_app
//
struct timeval ACQMedFreqFFT_StartTime;
struct timeval ACQFineFreqFFT_StartTime;
struct timeval computeFFT_IQ_StartTime;
struct timeval FftAcqMain_StartTime;
struct timeval searchOneBinOneAccumulation_StartTime;
//
#else
//
LARGE_INTEGER ACQMedFreqFFT_StartTime;
LARGE_INTEGER ACQFineFreqFFT_StartTime;
LARGE_INTEGER computeFFT_IQ_StartTime;
LARGE_INTEGER FftAcqMain_StartTime;
LARGE_INTEGER searchOneBinOneAccumulation_StartTime;
//
#endif
//
double Initialization_time=0;

double ACQMedFreqFFT_time=0;
double ACQFineFreqFFT_time=0;
double computeFFT_IQ_time=0;
double FftAcqMain_time=0;
double searchOneBinOneAccumulation_time=0;
//
double ACQMedFreqFFT_first=0;
double ACQFineFreqFFT_first=0;
double computeFFT_IQ_first=0;
double FftAcqMain_first=0;
double searchOneBinOneAccumulation_first=0;
//
int ACQMedFreqFFT_counter=0;
int ACQFineFreqFFT_counter=0;
int computeFFT_IQ_counter=0;
int FftAcqMain_counter=0;
int searchOneBinOneAccumulation_counter=0;
//
double time_process = 0;
int    time_process_counter = 0;

#endif // #ifdef PROFILER

extern T_Channel Tck1Ch[MAXNUMCHANPERCORE];
extern T_Channel Tck2Ch[MAXNUMCHANPERCORE];
extern T_Channel Tck3Ch[MAXNUMCHANPERCORE];

extern int Tck1ChIdx;
extern int Tck2ChIdx;
extern int Tck3ChIdx;

uint8_t chk=0;


int ii = 0;

task_param_t task_param[6] =
{
/*  name task  id task   id perd_task    expected cpu   cpuset    run     cpu  */
  {   "TCK1"  ,  0x0   ,     0x0      ,       0       , {{0x1}} , false  , -1},  // TCR1
  {   "TCK2"  ,  0x0   ,     0x0      ,       1       , {{0x2}} , false  , -1},  // TCR2 
  {   "TCK3"  ,  0x0   ,     0x0      ,       1       , {{0x2}} , false  , -1},  // TCR3 
  {   "ACQ1"  ,  0x0   ,     0x0      ,       0       , {{0x1}} , false  , -1},  // ACQ1
  {   "ACQ2"  ,  0x0   ,     0x0      ,       1       , {{0x2}} , false  , -1},  // ACQ2
  {   "MAIN"  ,  0x0   ,     0x0      ,       2       , {{0x4}} , false  , -1}   // MAIN
};
    
/******************** SpW variables ********************/ 
int link_run = 0;
int nospw = 0;
int tasks_stop = 0;

int num_pkt_prew = 0;
#if 0
grspw_device devs[DEVS_MAX];

int dma_process(grspw_device *dev);

extern struct router_hw_info router_hw;
extern void *router;
int router_present = 0;		

struct grspw_config dev_def_cfg = 
{
		.adrcfg =
		{
			.promiscuous = 1, /* Detect all packets */
			.def_addr = 32, /* updated bu dev_init() */
			.def_mask = 0,
			.dma_nacfg =
			{
				/* Since only one DMA Channel is used, only
				 * the default Address|Mask is used.
				 */
				{
					.node_en = 0,
					.node_addr = 0,
					.node_mask = 0,
				},
				{
					.node_en = 0,
					.node_addr = 0,
					.node_mask = 0,
				},
				{
					.node_en = 0,
					.node_addr = 0,
					.node_mask = 0,
				},
				{
					.node_en = 0,
					.node_addr = 0,
					.node_mask = 0,
				},
			},
		},
		.rmap_cfg = 0,		/* Disable RMAP */
		.rmap_dstkey = 0,	/* No RMAP DESTKEY needed when disabled */
		.tc_cfg = TCOPTS_EN_TX|TCOPTS_EN_RX,/* Enable TimeCode */
		.tc_isr_callback = NULL,/* No TimeCode ISR */
		.tc_isr_arg = NULL,	/* No TimeCode ISR Argument */
		.enable_chan_mask = 1,	/* Enable only the first DMA Channel */
		.chan =
		{
			{
				.flags = DMAFLAG_NO_SPILL,
				.rxmaxlen = PKT_SIZE+4,
				.rx_irq_en_cnt = 0, /* Disable RX IRQ generation */
				.tx_irq_en_cnt = 0, /* Disable TX IRQ generation */
			},
			/* The other 3 DMA Channels are unused */
			
		},
};
/******************************************************/
#endif
#ifndef _rtems_app
// Windows
#ifdef TCP_interface  
int initializeTCP(void)
{

	/* Configure the PowerPC_monitor as Client
	 * the Server will be the PowerPC target */
	
	/* Open windows connection */
	/* 0x0101 is the version of Winsock to use */
	if (WSAStartup(0x0101, &wsaData) != 0){
		printf("\nCould not open Windows connection.\n");
		return 1;
	}
	
	/* Opend a datagram socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd == INVALID_SOCKET){
		  printf("\nCould not create socket.\n");
		  return 1;
	}
	
	/* Set family and port of the server */
	memset((void *)&serveraddr, '\0', sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_addr.s_addr = inet_addr(IP_SERV);
	
	/* Set family and port of the client */
	memset((void *)&clientaddr, '\0', sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(CLIENTPORT);
	clientaddr.sin_addr.s_addr = inet_addr(IP_CLIENT);

	/* Bind address to socket */
	if(bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
		printf("\nCould not bind name to socket\n");
		return 1;
	}

	return 0;
}
#else
int initializeUDP(void)
{

	/* Configure the PowerPC_monitor as Client
	 * the Server will be the PowerPC target */
	
	/* Open windows connection */
	/* 0x0101 is the version of Winsock to use */
	if (WSAStartup(0x0101, &wsaData) != 0){
		printf("\nCould not open Windows connection.\n");
		return 1;
	}
	
	/* Opend a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd == INVALID_SOCKET){
		  printf("\nCould not create socket.\n");
		  return 1;
	}
	
	/* Set family and port of the server */
	memset((void *)&serveraddr, '\0', sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_addr.s_addr = inet_addr(IP_SERV);
	
	/* Set family and port of the client */
	memset((void *)&clientaddr, '\0', sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(CLIENTPORT);
	clientaddr.sin_addr.s_addr = inet_addr(IP_CLIENT);

	/* Bind address to socket */
	if(bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
		printf("\nCould not bind name to socket\n");
		return 1;
	}

	return 0;
}
#endif



#else

void setup_ports(void)
{
    RETURN_CODE_TYPE rc;
    SYSTEM_TIME_TYPE refreshRate= REFRESH_RATE;
    MESSAGE_SIZE_TYPE size1 = MAX_MESSAGE_SIZE1;
MESSAGE_SIZE_TYPE size2 = MAX_MESSAGE_SIZE2;
    printf("Initializing ports ...\n");

    /*Creating QRECV_TC_IOP Queuing Port*/
    CREATE_SAMPLING_PORT("QRECV_IOP", size2,  DESTINATION, refreshRate, &QRECV_IOP, &rc );
    if(NO_ERROR != rc)
    {
        printf("CREATE_QUEUING_PORT QRECV_IOP error %d\n", rc);
    }
    /*Creating QSEND_TM_IOP Queuing Port*/
    CREATE_SAMPLING_PORT("QSEND_IOP", size1, SOURCE, refreshRate, &QSEND_IOP, &rc );
    if(NO_ERROR != rc)
    {
        printf("CREATE_QUEUING_PORT QSEND_IOP error %d\n", rc);
    }
}
/*
 * RTEMS Startup Task
 */

int entry_point (rtems_task_argument ignored)
{
        rtems_status_code status;    // status of rtems function
        rtems_time_of_day time;      // rtems system time
//        int   i 	  = 0;       // Function return value and increment    
        
        rtems_interrupt_level level;

        rtems_interrupt_local_disable(level);
        rtems_interrupt_local_enable(level);

        /* SET clock & date */ 
        time.year   = 2018;
        time.month  = 02;
        time.day    = 19;
        time.hour   = 14;
        time.minute = 53;
        time.second = 0;
        time.ticks  = 0; 
        float x=((level+3)*200.0)/17.5;
        status = rtems_clock_set( &time );
        printf("floatest %f\n", x);

        if(status == RTEMS_SUCCESSFUL)
        {
            printf(">> RTEMS system time set: %s\n",rtems_status_text(status));          
            printf(">> System time: %d-%d-%d %d:%d:%d \n",time.year, time.month, time.day, time.hour, time.minute, time.second);
        }
        else
        {
            printf(">>ERROR RTEMS system time set: %s\n",rtems_status_text(status));
        }
        
        uint32_t cpu_num;
        cpu_num = rtems_get_current_processor();
        printf(">> Cpu %d running INIT TASK \n", cpu_num);

    #ifdef RTEMS_DRVMGR_STARTUP
        printk("ttcp: driver manager enabled\n");
    #endif      
        
    /* Initialize Driver manager and Networking, in config.c */
    //printk(">> initialize system:\n"); 
    //system_init(); 
    setup_ports();
    rtems_task_wake_after(10);

#ifdef TCP_interface
    printk(">> initialize TCP\n");
    //initializeTCP();
#else
    printk(">> initialize UDP\n");
    //initializeUDP();
#endif
    
    printk(">> initialize SpW\n");
    nospw = 0;
#if 0
    /* Initialize two GRSPW AMBA ports */
   printf(">> Setting up SpaceWire router\n");
   if (router_setup_custom()) 
   {
           printf("Failed router initialization, assuming that it does not exists\n");
   } 
   else 
   {
           /* on-chip router found */
           if (router_hw.nports_amba < 2) 
           {
                   printf("Error. Router with less than 2 AMBA ports not supported\n");
                   exit(0);
           }
           router_present = 1;
   }
    
   printf(">> Setting up SpaceWire router done\n");
   
   nospw = grspw_dev_count();
   if (nospw < 1)
   {
           printf("Found %d SpaceWire cores, aborting \n", nospw);
           exit(0);
   }
   if (nospw > DEVS_MAX) 
   {
           printf(">> Limiting to %d SpaceWire devices\n", DEVS_MAX);
           nospw = DEVS_MAX;
   }
   
        memset(devs, 0, sizeof(devs));
	for (i=0; i<nospw; i++) {
		if (dev_init(i)) {
			printf("Failed to initialize GRSPW%d\n", i);
			exit(0);
		}
		fflush(NULL);
	}
	
	/* Initialize GRSPW */
	init_pkts();

	printf(">> Starting SpW DMA channels\n");
	for (i = 0; i < nospw; i++) {
		printf(">> Starting GRSPW%d: ", i);
		fflush(NULL);
		if (grspw_start(DEV(&devs[i]))) {
			printf("Failed to initialize GRSPW%d\n", i);
			exit(0);
		}
		printf("DMA Started Successfully\n");
	}

	fflush(NULL);  
       
    printk(">> SpaceWire initialize done\n");   
    printk(">> initialize Tasks\n");
#endif
    //  Create tck tasks
    for ( ii = 0 ; ii < 3 ; ii++ ) 
    {
      status =  rtems_task_create(
                            rtems_build_name( task_param[ ii ].name[0],task_param[ ii ].name[1],task_param[ ii ].name[2], task_param[ ii ].name[3]),
                            10,
                            CONFIGURE_MINIMUM_TASK_STACK_SIZE * 32,
                            RTEMS_DEFAULT_MODES,
                            RTEMS_FLOATING_POINT,
                            &task_param[ ii ].id
                        );
    }
    
    //  Create acq tasks
    for ( ii = 3 ; ii < 5 ; ii++ ) 
    {
        rtems_task_create(
                            rtems_build_name( task_param[ ii ].name[0],task_param[ ii ].name[1],task_param[ ii ].name[2], task_param[ ii ].name[3]),
                            10,
                            CONFIGURE_MINIMUM_TASK_STACK_SIZE * 128,
                            RTEMS_DEFAULT_MODES,
                            RTEMS_FLOATING_POINT,
                            &task_param[ ii ].id
                        );
    }
    
    //  Create main task
    rtems_task_create(
                    rtems_build_name( task_param[ 5 ].name[0],task_param[ 5 ].name[1],task_param[ 5 ].name[2], task_param[ 5 ].name[3]),
                    12,
                    2048*1024,
                    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
                    RTEMS_FLOATING_POINT,
                    &task_param[ 5 ].id
                );
    


    for ( ii = 0 ; ii < 6 ; ii++ ) 
    {
       status = rtems_task_set_affinity( task_param[ ii ].id, sizeof(cpu_set_t), &task_param[ii].cpuset );
       if(status == RTEMS_SUCCESSFUL)
       {
            printf(">> Set %s affinity to cpu %d\n", task_param[ii].name, task_param[ii].expected_cpu );
       }
       else
       {
            printf(">ERROR: task_set_affinity(): %s\n",rtems_status_text(status));
       }
    }

#ifdef SEB_FFT
    /* INIT SEB FFT */
    printk(">> initialize fft: %d\n",FFT_SIZE_MAX);   
    Init_FFT_Algorithm(FFT_SIZE_MAX);
#endif
    
    printk("\n>> Main task start, init deleted \n");
    
    /* START GNSSW */
    rtems_task_start( task_param[ 5 ].id, Main_task, 5 );
    
    /* separate SpW communication */
    //rtems_task_start( task_param[ 5 ].id, dma_task, 5 );

    rtems_task_delete( RTEMS_SELF);  
}

#ifdef TCP_interface
int initializeTCP(void)
{
    udp_port= CLIENTPORT; // TCP

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sd, SOL_SOCKET, SO_RCVBUF, &sockbufsize, sizeof sockbufsize);
    setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof sockbufsize);
    

///// bind    
     memset(&serveraddr, 0, sizeof serveraddr);
     serveraddr.sin_family = AF_INET;
     serveraddr.sin_port = htons(SERVPORT);  // board port
     serveraddr.sin_addr.s_addr = inet_addr(IP_SERV);   // board IP
    
     if (bind(sd, (struct sockaddr *)&serveraddr, sizeof serveraddr) < 0)
     {
         printk("Can't bind socket: ");
     }

/////// end bind    
    
    memset (&clientaddr, 0, sizeof clientaddr);
    clientaddr.sin_port = htons(udp_port);
    clientaddr.sin_family = AF_INET;   
    addr_tmp= inet_addr(IP_CLIENT);      // SERVER IP
    clientaddr.sin_addr.s_addr = addr_tmp;
    
    return 0;
}

#else
int initializeUDP(void)
{
#if 0
    udp_port= CLIENTPORT; // UDP

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(sd, SOL_SOCKET, SO_RCVBUF, &sockbufsize, sizeof sockbufsize);
    setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof sockbufsize);
    

///// bind    
     memset(&serveraddr, 0, sizeof serveraddr);
     serveraddr.sin_family = AF_INET;
     serveraddr.sin_port = htons(SERVPORT);  // board port
     serveraddr.sin_addr.s_addr = inet_addr(IP_SERV);   // board IP
     
    
     if (bind(sd, (struct sockaddr *)&serveraddr, sizeof serveraddr) < 0)
     {
         printk("Can't bind socket: ");
     }

/////// end bind    
    
    memset (&clientaddr, 0, sizeof clientaddr);
    clientaddr.sin_port = htons(udp_port);
    clientaddr.sin_family = AF_INET;   
    addr_tmp= inet_addr(IP_CLIENT);      // SERVER IP
    clientaddr.sin_addr.s_addr = addr_tmp;
# endif
    return 0;
}
#endif

#endif

#ifdef TCP_interface
int receiveTCP_TC(void)
#else
int receiveUDP_TC(void)
#endif
{
	int rc;
	int fail = 0;
//	int i = 0, k = 0;
        
 //       grspw_device *dev;

	int nSamplesPerRead = 0;       /* Used to compute # ms per read call. */
	int nSamples;

    #ifdef PROFILER
       double ElapsedSeconds = 0;
    #endif

    //rtems_task_wake_after(100);
//    printf("receiving\n");

#ifdef TCP_interface
	rc = RecvTcp();
#else
    rc = RecvUdp_check(&chk);
    printf("recv %d %d\n", rc, chk);
#endif
    
    if (rc<0) 
    {
        printf("\nError at recvfrom() in receiveUDP_TC.\n");
        return -1;
    }
    else
    {
        if (rc == TCSIZE_CNF)
        {
            #ifdef TCP_interface            
                SendTcpOK();
            #else
                SendUdpOK(chk);  
            #endif 

            printf(">> Initialize ... \n");
            fail = initialize(configFileName); // EXIT_OK; //

            //printConfiguration();
            printf(">> Initialize OK\n");

            if(fail != EXIT_OK)
            {
                printf(">> Error initialize \n");
            }	
            else
            {		                                                        
                if(Config.Com.onspw == 0)
                {
                    nSamples = (int) floor(Config.Com.sampFreq*CODE_PERIOD*supportData[0].genSysData_P->nPrimaryIterations);
                    nSamplesPerRead = 0;
                    do
                    {
                        nSamplesPerRead = nSamplesPerRead + nSamples;
                    } while ((nSamplesPerRead % NBITS_PER_WORD) != 0);

                    /* Compute # integers to read in each read call. */
                    nBytesToRead = nSamplesPerRead * BYTES_PER_WORD / NBITS_PER_WORD;                             
                    
                }

                #ifdef TCP_interface            
                    SendTcpOK();
                #else
                    SendUdpOK(chk); 
                   printf("sned second ok\n"); 
                #endif 
#if 0
                if(Config.Com.onspw == 1)
                {
                    printf(">> Started SpW DMA control task\n");
                    printf(">> Receiver processing start ... \n");

                    while (tasks_stop == 0)
                    {                                                        
                        for (i = 0; i < nospw; i++) 
                        {
                            dev = &devs[i];
                            if (dev->dh == NULL)
                            {
                                continue;
                            }
                            dma_process(dev);
                        }

                        if(ioBuffer.inIndexBytes == 100000)//200000)
                        {
                            ReceiverProcessing();
                            fail = 2;  
                        }
                    }
                }
#endif
            }
        }
        else if(rc == nBytesToRead/25)  // read 4kB for primaryIterations == 100
        //else if(rc == nBytesToRead)  // read 4kB     for primaryIterations == 4
        {
            #ifdef TCP_interface            
                TcpBufferCopy();
            #else
                UdpBufferCopy();   
            #endif

            ioBuffer.inIndexBytes = ioBuffer.inIndexBytes + rc;

            if(ioBuffer.inIndexBytes == nBytesToRead) // read 100kB 
            {
                printf("\n");
                printConfiguration();
                printf("\n");
                
                printf(">> Receiver processing start ... \n");
                ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                ReceiverProcessing();
                fail = 2;
            }

#ifdef TCP_interface            
                    SendTcpOK();
#else
        SendUdpOK(chk);  
#endif
            }
            else if(rc == 5000)  // read 50kB 
            {

#ifdef TCP_interface            
                    SendTcpOK();
#else
        SendUdpOK(chk);  
#endif
            }
            else
            {

                    //fail = 2;
#ifdef TCP_interface            
                    SendTcpOK();
#else
                    printf("send last ok\n");
        SendUdpOK(chk);  
#endif
            }

            return fail;
    }
}

int GetCNF(void)
{
	int i = 0;
        
                Config.Com.onspw				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Com.mode					= *(E_ExecutionMode*)                   (buffer + i); i += sizeof(E_ExecutionMode);
		Config.Com.filterCenterFreq			= *(double *)				(buffer + i); i += sizeof(double);
		Config.Com.systemFlag				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Com.ifreq				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Com.bandwidth				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Com.sampFreq				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Com.outputRate				= *(float *)				(buffer + i); i += sizeof(float);
		Config.Com.simTime				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Com.startTime				= *(double *)				(buffer + i); i += sizeof(double);
                Config.Com.rawSampleBytesFlag			= *(int *)				(buffer + i); i += sizeof(int);
                Config.Com.oneSamplePerByteFlag			= *(int *)				(buffer + i); i += sizeof(int);

		Config.Mng.nChannels				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Mng.nChannelspercore                     = round(Config.Mng.nChannels/NUMTCKTASK);
                printf(" > User set: %d nChannelspercore\n", Config.Mng.nChannelspercore);
                Config.Mng.nChannels                            = Config.Mng.nChannelspercore*NUMTCKTASK;
                printf("             %d nChannels\n", Config.Mng.nChannels);
                Config.Mng.startMode				= *(E_StartMode *)			(buffer + i); i += sizeof(E_StartMode);	
		Config.Mng.fastFirstFixFlag			= *(int *)				(buffer + i); i += sizeof(int);
		Config.Mng.enableVtFlag				= *(int *)				(buffer + i); i += sizeof(int);

		Config.FftAcq.maxFreq				= *(float *)				(buffer + i); i += sizeof(float);
		Config.FftAcq.freqStepCodeRatio                 = *(float *)				(buffer + i); i += sizeof(float);
		Config.FftAcq.maxNbins				= *(int *)				(buffer + i); i += sizeof(int);
		Config.FftAcq.pfa				= *(double *)				(buffer + i); i += sizeof(double);
		Config.FftAcq.acqTh				= *(float *)				(buffer + i); i += sizeof(float);
		Config.FftAcq.maxNonCohInt_GPS			= *(int *)				(buffer + i); i += sizeof(int);
                Config.FftAcq.maxNonCohInt_GAL			= *(int *)				(buffer + i); i += sizeof(int);
		Config.FftAcq.nMedInt				= *(int *)				(buffer + i); i += sizeof(int);
		Config.FftAcq.nFineInt				= *(int *)				(buffer + i); i += sizeof(int);
		Config.FftAcq.freqRefinementFlag                = *(int *)				(buffer + i); i += sizeof(int);
		Config.FftAcq.sp3HighElevN			= *(int *)				(buffer + i); i += sizeof(int);
                Config.FftAcq.maxDopplerDrift			= *(int *)				(buffer + i); i += sizeof(int);

		Config.Aid.refPos[0]				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Aid.refPos[1]				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Aid.refPos[2]				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Aid.refTow				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Aid.refWn				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Aid.lastVisibCompTh			= *(long *)				(buffer + i); i += sizeof(long);
		Config.Aid.clkDriftAid				= *(double *)				(buffer + i); i += sizeof(double);

		Config.Lck.nLock				= *(int *)                              (buffer + i); i += sizeof(int);
		Config.Lck.cnoTh				= *(float *)                            (buffer + i); i += sizeof(float);
		Config.Lck.cliTh				= *(float *)                            (buffer + i); i += sizeof(float);
		Config.Lck.lossCnoTh				= *(float *)                            (buffer + i); i += sizeof(float);
		Config.Lck.vtCnoTh				= *(float *)                            (buffer + i); i += sizeof(float);
		Config.Lck.vtNLock				= *(int *)                              (buffer + i); i += sizeof(int);
		Config.Lck.nCnoTh				= *(int *)                              (buffer + i); i += sizeof(int);

		Config.Pvt.elevMask				= *(double *)				(buffer + i); i += sizeof(double);
		Config.Pvt.smoLength				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Pvt.ionoCorrection			= *(int *)				(buffer + i); i += sizeof(int);
		Config.Pvt.tropoCorrection			= *(int *)				(buffer + i); i += sizeof(int);
		Config.Pvt.cn0outFlag				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Pvt.integerTimeMarkFlag                  = *(int *)				(buffer + i); i += sizeof(int);
		Config.Pvt.clkSteeringFlag			= *(int *)				(buffer + i); i += sizeof(int);

        Config.Ekf.useEkf                               = *(int *)				(buffer + i); i += sizeof(int);
        Config.Ekf.spaceUser                            = *(int *)				(buffer + i); i += sizeof(int);
        Config.Ekf.deltaT                               = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.mahaThr                              = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.measSigmaPr                          = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.measSigmaDp                          = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.initStateSigmaPos                    = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.initStateSigmaVel                    = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.initStateSigmaClkBias                = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.initStateSigmaClkGalGpsBias          = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.initStateSigmaClkDrift               = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.stateNoiseCovPos                     = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.stateNoiseCovVel                     = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.stateNoiseCovClkBias                 = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.stateNoiseCovClkGalGpsBias           = *(double *)				(buffer + i); i += sizeof(double);
        Config.Ekf.stateNoiseCovClkDrift                = *(double *)				(buffer + i); i += sizeof(double);
                        
		Config.Tck.pllOrder				= *(int *)                              (buffer + i); i += sizeof(int);
		Config.Tck.dllOrder				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.decimateFactor			= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.pllBw				= *(float *)				(buffer + i); i += sizeof(float);
		Config.Tck.dllBw				= *(float *)				(buffer + i); i += sizeof(float);
		Config.Tck.nDll					= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.nPll					= *(int *)				(buffer + i); i += sizeof(int);
                Config.Tck.nDll_GAL				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.nPll_GAL				= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.chipDelay				= *(float *)				(buffer + i); i += sizeof(float);
		Config.Tck.resolutionCarrier                    = *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.decimateFactor			= *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.cpInitializationFlag                 = *(int *)				(buffer + i); i += sizeof(int);
		Config.Tck.timeStampOutFlag			= *(int *)				(buffer + i); i += sizeof(int);
                
               //printConfiguration();

                almanacIndexBuffer = i;
	return 0;	
}

#ifdef TCP_interface            
			void SendTcpOK(void)
#else
            void SendUdpOK(uint8_t i)  
#endif
{
    
     char buffer_ok[1] ={i};
    RETURN_CODE_TYPE rc;
   // printf("SendUdpOK %d\n", i);
        unsigned int length=1;
    WRITE_SAMPLING_MESSAGE(QSEND_IOP, (MESSAGE_ADDR_TYPE )buffer_ok, length, &rc );
    if(AIR_NO_ERROR != rc )
        printk("sendok error/n");
}

void SendResultCalculation(char *BufferToSend, int LenToSend)
{
    RETURN_CODE_TYPE rc;
    printf("SendResultCalculation %d\n",LenToSend);

    WRITE_SAMPLING_MESSAGE(QSEND_IOP, (MESSAGE_ADDR_TYPE )BufferToSend, LenToSend, &rc );
}

#ifdef TCP_interface            
			int RecvTcp(void)
#else
            int RecvUdp(void) 
#endif
{
	RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE len = 0;
    VALIDITY_TYPE val;
    unsigned int received=0;

    while(!received){
        READ_SAMPLING_MESSAGE(QRECV_IOP, (MESSAGE_ADDR_TYPE )buffer, &len, &val,  &rc );
       // printk("RPPOL %d %d %d 0x%06x\n", len, val, rc, (MESSAGE_ADDR_TYPE )buffer);
        if(AIR_NO_ERROR != rc  || val != AIR_MESSAGE_VALID){
          //  rtems_task_wake_after(1);
        }else{
            received = 1;
        }
    }
  
    
    return len;
}

#ifdef TCP_interface            
    int RecvTcp(void)
#else
    int RecvUdp_check(uint8_t *chk) 
#endif
{
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE len = 0;
    VALIDITY_TYPE val;
    unsigned int received=0;

    while(!received){
        READ_SAMPLING_MESSAGE(QRECV_IOP, (MESSAGE_ADDR_TYPE )buffer, &len, &val,  &rc );
        // printk("RPPOL %d %d %d 0x%06x\n", len, val, rc, (MESSAGE_ADDR_TYPE )buffer);
        if(AIR_NO_ERROR != rc  || val != AIR_MESSAGE_VALID){
            //  rtems_task_wake_after(1);
        }else{
            received = 1;
        }
    }
    *chk=chksum8(buffer, len);
//   printk(" chk %d\n",  chk); 
    return len;

}
                                      
#ifdef TCP_interface            
    int RecvTcp_timeout(void)
#else
    int RecvUdp_timeout(void) 
#endif
{
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE len = 0;
    VALIDITY_TYPE val;
    unsigned int received=0;
    unsigned int i=0;

    while(!received && i<1500){
        READ_SAMPLING_MESSAGE(QRECV_IOP, (MESSAGE_ADDR_TYPE )buffer, &len, &val,  &rc );
        if(AIR_NO_ERROR != rc  || val != AIR_MESSAGE_VALID){
           // rtems_task_wake_after(2);
        }else {
            received=1;
        }
        i++;
    }
    return len;
}

#ifdef TCP_interface            
			void TcpBufferCopy(void)
#else
            void UdpBufferCopy(void)
#endif
{
	memcpy(&ioBuffer.samples[ioBuffer.inIndexBytes], buffer, nBytesToRead/25);
}

void PrepSendBuffer(void)
{
	idx = 0;
	int k = 0;
			
	double gpstow = (pvtProcess.firstPvtFlag ? timeStamp.gpsTime.TOW : 0.0);

	memcpy(send_buffer+idx,(unsigned char *)&ComData.counterMs,sizeof(int));														idx += sizeof(int);
        
	memcpy(send_buffer+idx,(unsigned char *)&timeStamp.gpsTime.WN,sizeof(int));														idx += sizeof(int);
	memcpy(send_buffer+idx,(unsigned char *)&gpstow,sizeof(double));																idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.firstPvtFlag,sizeof(int));													idx += sizeof(int);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.posFlag,sizeof(int));												idx += sizeof(int);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.G[2],sizeof(double));												idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.ppMeas.NM_ls,sizeof(int));													idx += sizeof(int);
//        printf("SVN: ");

	for (k = 0; k < MAXNUMCHAN; k++)
	{
//                printf("%d \t", pvtProcess.ppMeas.SVN_ls[k]);

		memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.ppMeas.SVN_ls[k],sizeof(int));											idx += sizeof(int);
		
	}
//        printf("%d \n");

	for (k = 0; k < MAXNUMCHAN; k++)
	{
		memcpy(send_buffer+idx,(unsigned char *)&MeasProcessComb.Meas[pvtProcess.ppMeas.SVN_ls[k]-1].CN0,sizeof(double));			idx += sizeof(double);
	}

	for (k = 0; k < MAXNUMCHAN; k++)
	{
		memcpy(send_buffer+idx,(unsigned char *)&MeasProcessComb.Meas[pvtProcess.ppMeas.SVN_ls[k]-1].PR,sizeof(double));			idx += sizeof(double);
	}

	for (k = 0; k < MAXNUMCHAN; k++)
	{
		memcpy(send_buffer+idx,(unsigned char *)&MeasProcessComb.Meas[pvtProcess.ppMeas.SVN_ls[k]-1].CP,sizeof(double));			idx += sizeof(double);
	}

	for (k = 0; k < MAXNUMCHAN; k++)
	{
		memcpy(send_buffer+idx,(unsigned char *)&MeasProcessComb.Meas[pvtProcess.ppMeas.SVN_ls[k]-1].D,sizeof(double));				 idx += sizeof(double);
	}

	for (k = 0; k < MAXNUMCHAN; k++)
	{
		memcpy(send_buffer+idx,(unsigned char *)&MeasProcessComb.Meas[pvtProcess.ppMeas.SVN_ls[k]-1].CLI,sizeof(double));			 idx += sizeof(double);
	}

	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.X,3*sizeof(double));													 idx += 3*sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.V,3*sizeof(double));												     idx += 3*sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.G,3*sizeof(double));													 idx += 3*sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.DOP.GDoP,sizeof(double));											 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.DOP.PDoP,sizeof(double));											 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.DOP.HDoP,sizeof(double));											 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.DOP.VDoP,sizeof(double));											 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.residualPR,MAXNUMCHAN*sizeof(double));								 idx += MAXNUMCHAN*sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.residualDoppler,MAXNUMCHAN*sizeof(double));						     idx += MAXNUMCHAN*sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.clkData.deltaClkBias,sizeof(double));								 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.clkData.clkDrift,sizeof(double));									 idx += sizeof(double);
	memcpy(send_buffer+idx,(unsigned char *)&pvtProcess.lsData.clkData.deltaClkBiasGal,sizeof(double));								 idx += sizeof(double);
#ifdef PROFILER    
    memcpy(send_buffer+idx,(unsigned char *)&MainLoop_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&ManageFftAcquisition_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&AcquisitionTrackingAid_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&ManageLoopTracking1_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&ManageLoopTracking2_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&ManageLoopTracking3_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&gpsVtAcq_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&galVtAcq_time,sizeof(double));								 idx += sizeof(double);
    memcpy(send_buffer+idx,(unsigned char *)&manageMeasAndPvt_time,sizeof(double));								 idx += sizeof(double);
    
    
    memcpy(send_buffer+idx,(unsigned char *)&acq_svn,sizeof(int));							idx += sizeof(int);
    memcpy(send_buffer+idx,(unsigned char *)&acq_system_flag,sizeof(int));							idx += sizeof(int);
    
    for (kk = 0; kk < 10; kk++)
    {
        for (k = 0; k < MAXNUMCHAN; k++)
        {
            memcpy(send_buffer+idx,(unsigned char *)&Tck_info_tab[kk][k].sat,sizeof(int));			 idx += sizeof(int);
        }
    } 
    for (kk = 0; kk < 10; kk++)
    {    
        for (k = 0; k < MAXNUMCHAN; k++)
        {
            memcpy(send_buffer+idx,(unsigned char *)&Tck_info_tab[kk][k].gps_gal,sizeof(int));			 idx += sizeof(int);
        }
    }
    for (kk = 0; kk < 10; kk++)
    {    
        for (k = 0; k < MAXNUMCHAN; k++)
        {
            memcpy(send_buffer+idx,(unsigned char *)&Tck_info_tab[kk][k].sync_nosync,sizeof(int));			 idx += sizeof(int);
        }  
    }  
    for (kk = 0; kk < 10; kk++)
    {    
        for (k = 0; k < MAXNUMCHAN; k++)
        {
            memcpy(send_buffer+idx,(unsigned char *)&Tck_info_tab[kk][k].time,sizeof(double));			 idx += sizeof(double);
        }   
    }
#endif	
    
   #ifdef PROFILER
	time_process = 0;
	time_process_counter = 0;
	//
    
    MainLoop_time = 0;
    //gpsManagerSignalProcessing_time = 0;
    galManagerSignalProcessing_time = 0;
    
    ManageLoopTracking1_time = 0;
    ManageLoopTracking2_time = 0;
    ManageLoopTracking3_time = 0;
    
    gpsFftAcq_time = 0;
    galFftAcq_time = 0;

    gpsVtAcq_time = 0;
    galVtAcq_time = 0;
        
    memset(Tck_info_tab,0,sizeof(Tck_info_tab));
    
    memset(gpssatTckSync_tab,0,sizeof(gpssatTckSync_tab));
    memset(galsatTckSync_tab,0,sizeof(galsatTckSync_tab));
    memset(gpssatTckNoSync_tab,0,sizeof(gpssatTckNoSync_tab));
    memset(galsatTckNoSync_tab,0,sizeof(galsatTckNoSync_tab));
    memset(gpsTckSync_time_tab,0,sizeof(gpsTckSync_time_tab));
    memset(galTckSync_time_tab,0,sizeof(galTckSync_time_tab));
    memset(gpsTckNoSync_time_tab,0,sizeof(gpsTckNoSync_time_tab));
    memset(galTckNoSync_time_tab,0,sizeof(galTckNoSync_time_tab));

    TckSync_time = 0;
    TckNoSync_time = 0;
    
	startReceiverProcessing_time = 0;
    AcquisitionTrackingAid_time = 0;
	generalManagerRx_time = 0;
	ManageFftAcquisition_time = 0;
	NewVtSatellites_time = 0;
	manageLoopTracking_time = 0;
	manageMeasAndPvt_time = 0;
	ElapsedSeconds_pvt = 0;
	//
	ACQMedFreqFFT_time=0;
	ACQFineFreqFFT_time=0;
	computeFFT_IQ_time=0;
	FftAcqMain_time = 0;
	searchOneBinOneAccumulation_time=0;
	//
	ACQMedFreqFFT_first=0;
	ACQFineFreqFFT_first=0;
	computeFFT_IQ_first=0;
	FftAcqMain_first=0;
	searchOneBinOneAccumulation_first=0;
	//
	ACQMedFreqFFT_counter=0;
	ACQFineFreqFFT_counter=0;
	computeFFT_IQ_counter=0;
	FftAcqMain_counter=0;
	searchOneBinOneAccumulation_counter=0;
	//
    #endif // #ifdef PROFILER
}

void SendBuffer(void)
{       
        int recv_len=-1;    
        int val=0;

        uint8_t chk1=chksum8(send_buffer, idx);

        while(!val){
	    SendResultCalculation(send_buffer, idx);
            printf("calc sent %d\n", idx);

#ifdef TCP_interface            
            recv_len=RecvTcp();
#else
            recv_len=RecvUdp_timeout(); 
            if(recv_len>0 && (uint8_t)buffer[0]==chk1)
                val=1;

#endif 
            printf("received %d %d %d\n", recv_len, chk1, (uint8_t)buffer[0]);
        }
        recv_len=RecvUdp_timeout();
        recv_len=-1;
        val=0;
        if(idx_test_out > 0)
        {
            uint8_t chk2=chksum8(test_out, idx_test_out);
            while(!val){

                SendResultCalculation(test_out, idx_test_out); 
                 printf("calc2 sent %d\n", idx);
            
#ifdef TCP_interface            
                recv_len=RecvTcp();
#else
                recv_len=RecvUdp_timeout(); 
                if(recv_len>0 && (uint8_t)buffer[0]==chk2)
                    val=1;

                 printf("received2 %d %d %d\n", recv_len, chk2, (uint8_t)buffer[0]);

#endif 
            }
            idx_test_out = 0;         
            recv_len=RecvUdp_timeout(); 
               printf("received %d %d %d\n", recv_len, chk1, (uint8_t)buffer[0]);

        }
        printf("done \n");
     
        
}
/*
void memory_statistic(int memory_bytes, char *place, int *address, int add_sub)
{  
  if(add_sub)
  {
       memory_bytes_sum = memory_bytes_sum + memory_bytes;
  }
  else
  {
       memory_bytes_sum = memory_bytes_sum - memory_bytes; 
  }
  
   printf("%s: %d sum: %d in address: %p | %d\n",place,memory_bytes,memory_bytes_sum, address, address);
}
*/


/***************************************************************************************
 * SYSTEM TIME FUNCTIONS
 ***************************************************************************************/
#ifdef _rtems_app
//
struct timeval GetStartTime(){

    struct rusage ru;
	struct timeval StartingTime;
	//
    getrusage(RUSAGE_SELF, &ru);
    StartingTime = ru.ru_stime;

	return StartingTime;
}

#else
//
LARGE_INTEGER GetStartTime(){

	LARGE_INTEGER StartingTime;
	QueryPerformanceCounter(&StartingTime);
	return StartingTime;
}

#endif

#ifdef _rtems_app
//
double GetElapsedTime(struct timeval StartingTime)
{

    struct rusage ru;
    struct timeval EndingTime;
    double ElapsedSeconds_local;
	//
    getrusage(RUSAGE_SELF, &ru);
    EndingTime = ru.ru_stime;
    //
    ElapsedSeconds_local = EndingTime.tv_sec+(EndingTime.tv_usec/1000000.0) - (StartingTime.tv_sec+(StartingTime.tv_usec/1000000.0));
	//
	return ElapsedSeconds_local;    
}

#else
//
double GetElapsedTime(LARGE_INTEGER StartingTime)
{
	
	LARGE_INTEGER EndingTime;
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	double ElapsedSeconds_local;

	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	//
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	//
	ElapsedSeconds_local = ElapsedMicroseconds.QuadPart*1e-6;
	
	return ElapsedSeconds_local;
}

#endif



rtems_task Tck1_task( rtems_task_argument unused )
{
//    printf(" :: TCK1\n");
    int i = 0;    
    int ms = 0;
    int auxIndex = 0;

    #ifdef PROFILER
        double ElapsedSeconds = 0;
    #endif

    #ifdef PROFILER
        ManageLoopTracking1_StartTime = GetStartTime();
    #endif	
        
    for (ms = 0 ; ms < 100 ; ms++)
    {     
        ManageLoopTracking(Tck1Ch, 0, core[0].nchan);

        for (i = 0 ; i < core[0].nchan  ; i++)
        {						
                Tck1Ch[i].outIndex.bit	+=  Tck1Ch[i].corrAuxIndex.outSize_P->bit;
                Tck1Ch[i].outIndex.word	+=  Tck1Ch[i].corrAuxIndex.outSize_P->word;

                // Check BIT ROLL-OVER
                if (Tck1Ch[i].outIndex.bit >= NBITS_PER_WORD)
                {
                    Tck1Ch[i].outIndex.bit = Tck1Ch[i].outIndex.bit % NBITS_PER_WORD;
                    Tck1Ch[i].outIndex.word++;
                }

                // Check BUFFER ROLL-OVER
                auxIndex = Tck1Ch[i].outIndex.bit + Tck1Ch[i].outIndex.word * NBITS_PER_WORD;

                if (auxIndex >= ComData.lengthSamples)
                {
                    auxIndex = auxIndex - ComData.lengthSamples;

                    // Reset index.
                    Tck1Ch[i].outIndex.bit  = auxIndex % NBITS_PER_WORD;
                    Tck1Ch[i].outIndex.word  = auxIndex / NBITS_PER_WORD;
                } 

                Tck1Ch[i].chCounterMs++;
        }					
    }
        
    #ifdef PROFILER
        ElapsedSeconds = GetElapsedTime(ManageLoopTracking1_StartTime);
        ManageLoopTracking1_time = ManageLoopTracking1_time + ElapsedSeconds;                   
    #endif  

    rtems_event_send(task_param[ 5 ].id, RTEMS_EVENT_1); 
 //  printf(" :: TCK1 DONE\n"); 
    rtems_task_suspend(task_param[ 0 ].id);
}

rtems_task Tck2_task( rtems_task_argument unused )
{
 //   printf(" :: TCK2\n");

    int i = 0; 
    int ms = 0;
    int auxIndex = 0;
    
    #ifdef PROFILER
        double ElapsedSeconds = 0;
    #endif

    #ifdef PROFILER
        ManageLoopTracking2_StartTime = GetStartTime();
    #endif	

    for (ms = 0; ms < 100; ms++)
    {       
        ManageLoopTracking(Tck2Ch, 1, core[1].nchan);
             
        for (i = 0 ; i < core[1].nchan ; i++)
        {                           
            Tck2Ch[i].outIndex.bit  +=  Tck2Ch[i].corrAuxIndex.outSize_P->bit;
            Tck2Ch[i].outIndex.word +=  Tck2Ch[i].corrAuxIndex.outSize_P->word;
                                                            
             // Check BIT ROLL-OVER
             if (Tck2Ch[i].outIndex.bit >= NBITS_PER_WORD)
             {
                 Tck2Ch[i].outIndex.bit = Tck2Ch[i].outIndex.bit % NBITS_PER_WORD;
                 Tck2Ch[i].outIndex.word++;
             }
                                                                                                                   // Check BUFFER ROLL-OVER
             auxIndex = Tck2Ch[i].outIndex.bit + Tck2Ch[i].outIndex.word * NBITS_PER_WORD;
             if (auxIndex >= ComData.lengthSamples)
             {
                 auxIndex = auxIndex - ComData.lengthSamples;
                 // Reset index.
                 Tck2Ch[i].outIndex.bit  = auxIndex % NBITS_PER_WORD;
                 Tck2Ch[i].outIndex.word = auxIndex / NBITS_PER_WORD;
             }            
             Tck2Ch[i].chCounterMs++;              
        }
#if 1
        ManageLoopTracking(Tck3Ch, 2, core[2].nchan);

        for (i = 0 ; i < core[2].nchan ; i++)
        {					        
            Tck3Ch[i].outIndex.bit	+=  Tck3Ch[i].corrAuxIndex.outSize_P->bit;
            Tck3Ch[i].outIndex.word	+=  Tck3Ch[i].corrAuxIndex.outSize_P->word;
            
            // Check BIT ROLL-OVER
            if (Tck3Ch[i].outIndex.bit >= NBITS_PER_WORD)
            {
                Tck3Ch[i].outIndex.bit = Tck3Ch[i].outIndex.bit % NBITS_PER_WORD;
                Tck3Ch[i].outIndex.word++;
            }

            // Check BUFFER ROLL-OVER
            auxIndex = Tck3Ch[i].outIndex.bit + Tck3Ch[i].outIndex.word * NBITS_PER_WORD;

            if (auxIndex >= ComData.lengthSamples)
            {
                auxIndex = auxIndex - ComData.lengthSamples;

                // Reset index.
                Tck3Ch[i].outIndex.bit  = auxIndex % NBITS_PER_WORD;
                Tck3Ch[i].outIndex.word = auxIndex / NBITS_PER_WORD;
            }            
            
            Tck3Ch[i].chCounterMs++;              
        }
#endif

    } 

    #ifdef PROFILER
        ElapsedSeconds = GetElapsedTime(ManageLoopTracking2_StartTime);
        ManageLoopTracking2_time = ManageLoopTracking2_time + ElapsedSeconds;                   
    #endif 

    rtems_event_send(task_param[ 5 ].id, RTEMS_EVENT_2); 
//printf(" :: TCK2 DONE\n");
    rtems_task_suspend(task_param[ 1 ].id);
}

rtems_task Tck3_task( rtems_task_argument unused )
{
  //  printf(" :: TCK3\n");

    int i = 0;
    int ms = 0;
    int auxIndex = 0;
    
    #ifdef PROFILER
        double ElapsedSeconds = 0;
    #endif    
          
    #ifdef PROFILER
        ManageLoopTracking3_StartTime = GetStartTime();
    #endif	

    for (ms = 0; ms < 100; ms++)
    {     
        ManageLoopTracking(Tck3Ch, 2, core[2].nchan);
    //    ManageLoopTracking(Tck3Ch, 1, core[1].nchan);

        for (i = 0 ; i < core[2].nchan ; i++)
  //   for (i = 0 ; i < core[1].nchan ; i++)

        {					       
            Tck3Ch[i].outIndex.bit	+=  Tck3Ch[i].corrAuxIndex.outSize_P->bit;
            Tck3Ch[i].outIndex.word	+=  Tck3Ch[i].corrAuxIndex.outSize_P->word;
            
            // Check BIT ROLL-OVER
            if (Tck3Ch[i].outIndex.bit >= NBITS_PER_WORD)
            {
                   Tck3Ch[i].outIndex.bit = Tck3Ch[i].outIndex.bit % NBITS_PER_WORD;
                   Tck3Ch[i].outIndex.word++;
            }

            // Check BUFFER ROLL-OVER
            auxIndex = Tck3Ch[i].outIndex.bit + Tck3Ch[i].outIndex.word * NBITS_PER_WORD;

            if (auxIndex >= ComData.lengthSamples)
            {
                   auxIndex = auxIndex - ComData.lengthSamples;

                   // Reset index.
                   Tck3Ch[i].outIndex.bit  = auxIndex % NBITS_PER_WORD;
                   Tck3Ch[i].outIndex.word = auxIndex / NBITS_PER_WORD;
            }            

            Tck3Ch[i].chCounterMs++;
        }            
    } 

    #ifdef PROFILER
        ElapsedSeconds = GetElapsedTime(ManageLoopTracking3_StartTime);
        ManageLoopTracking3_time = ManageLoopTracking3_time + ElapsedSeconds;                   
    #endif 

    rtems_event_send(task_param[ 5 ].id, RTEMS_EVENT_3);
//     printf(" :: TCK3 DONE\n");
    rtems_task_suspend(task_param[ 2 ].id);
}

rtems_task Acq1_task( rtems_task_argument unused )
{
 //   printf(" :: ACQ1\n");

    #ifdef PROFILER
        double ElapsedSeconds = 0;
    #endif 
        
    #ifdef PROFILER
        ManageFftAcquisition_StartTime = GetStartTime();
    #endif  
          
        supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].outIndex = supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].outIndex_init;
                                
        if(rxProcessingStatus.secondFft == 1)
        {       
            int slot_index = supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].slotIndex;
            int DopplerRange = ceil( (Config.FftAcq.maxDopplerDrift * (supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].acqCounterMs - supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].fftMs)/1000 / supportData[rxProcessingStatus.acqGalFlag].genSysData_P->freqStep + 1)*supportData[rxProcessingStatus.acqGalFlag].genSysData_P->freqStep );
            
            setAcqChannelMod(&supportData[rxProcessingStatus.acqGalFlag], supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].svnIndex, -supportData[rxProcessingStatus.acqGalFlag].satManag_P->acqFftSat[slot_index].dopplerFreqHz, DopplerRange, 0);
        }
    //    printf("manage fft id %d data %d\n",(unsigned int)rxProcessingStatus.acqGalFlag, &supportData[rxProcessingStatus.acqGalFlag]) ;
        ManageFftAcquisition(&supportData[rxProcessingStatus.acqGalFlag], 0);   
        
        if(rxProcessingStatus.secondFft == 1 && supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].acqFftStatus == ACQ_MED) // add if 2nd acquisition success
        {        
            // perform doppler refinement
            dopplerRefinementTimeDomain(&supportData[rxProcessingStatus.acqGalFlag], &chDataAll, supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].slot, supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].svnIndex, supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].slotIndex, 0);
        }
    
    #ifdef PROFILER
        ElapsedSeconds = GetElapsedTime(ManageFftAcquisition_StartTime);
        ManageFftAcquisition_time = ElapsedSeconds; //ManageFftAcquisition_time + ElapsedSeconds;  
    #endif	     
 
   if(rxProcessingStatus.secondFft == 0)
    {        
        rtems_event_send(task_param[ 5 ].id, RTEMS_EVENT_4); 
    }
   else if(rxProcessingStatus.secondFft == 1)
   {
        rxProcessingStatus.secondFft = 0;
        rtems_event_send(task_param[ 5 ].id, RTEMS_EVENT_5); 
   }
   //     printf(" :: ACQ1 DONE\n");
   rtems_task_suspend(task_param[ 3 ].id);
}

/* Task not used */
rtems_task Acq2_task( rtems_task_argument unused )
{
    ManageFftAcquisition(&supportData[rxProcessingStatus.acqGalFlag], 1);
       
    rtems_task_suspend(task_param[ 4 ].id);
}


rtems_task Main_task( rtems_task_argument unused )
{
    int   ret	  = 0;               // Function return value.
    
#ifdef TCP_interface    
    int addrlen;
    
    addrlen = sizeof(clientaddr);
    
    g_main_task_id = rtems_task_self();
    
    printk("\n>> waiting for connection ... \n");
    listen(sd,1);
    
    if(( sd = accept (sd, (struct sockaddr *)&clientaddr, &addrlen) ) < 0)
    {
            printf ("Can't accept connection: %s", strerror (errno));
    }
    else
    {
        struct sockaddr_in peer;
        int peerlen = sizeof(peer);
        if (getpeername(sd, (struct sockaddr *) &peer, 
            (socklen_t *)&peerlen) < 0) 
        {
        printf("getpeername");
        }
        fprintf(stderr,">> ttcp: accept from %s\n", 
        inet_ntoa(peer.sin_addr));
    }    
#endif

    printk("\n>> Ready to initialize, waiting ... \n");   
    
    while (1)
    {
#ifdef TCP_interface        
        if ((ret=receiveTCP_TC())!=0)
        {
            break;
        }
#else
       if ((ret=receiveUDP_TC())!=0)
        {
            break;
        }
#endif

    }
    
  printk(">> exit \n");
  exit(0);
}

/************************ SPW INTERFACE ************************/

int dev_init(int idx)
{

#if 0
	grspw_device *dev = &devs[idx];
	int i, ctrl, clkdiv, tc;
    
	printf(">> Initializing SpaceWire device %d\n", idx);

	memset(dev, 0, sizeof(grspw_device));

	dev->index = idx;
	dev->dh = grspw_open(idx);
	if (dev->dh == NULL) {
		printf("Failed to open GRSPW device %d\n", idx);
		return -1;
	}
	grspw_hw_support(dev->dh, &dev->hwsup);
#ifdef PRINT_GRSPW_RESET_CFG
	grspw_config_read(DEV(dev), &dev->cfg);
	printf("\n\n---- DEFAULT CONFIGURATION FROM DRIVER/HARDWARE ----\n");
	grspw_cfg_print(&dev->hwsup, &dev->cfg);
#endif
	dev->cfg = dev_def_cfg;
	dev->cfg.adrcfg.def_addr = 32 + idx;
	dev->cfg.tc_isr_arg = dev;
	tc = TCOPTS_EN_TX | TCOPTS_EN_RX | TCOPTS_EN_RXIRQ;
	grspw_tc_ctrl(dev->dh, &tc);

	if (grspw_cfg_set(DEV(dev), &dev->cfg)) {
		grspw_close(dev->dh);
		return -1;
	}
#ifdef PRINT_GRSPW_RESET_CFG
	printf("\n\n---- APPLICATION CONFIGURATION ----\n");
	grspw_cfg_print(&dev->hwsup, &dev->cfg);
	printf("\n\n");
#endif

	/* This will result in an error if only one port available */
	if (dev->hwsup.nports < 2) {
		int port = 1;
		if ( grspw_port_ctrl(dev->dh, &port) == 0 ) {
			printf("Succeeded to select port1, however only one PORT on dev %d!\n", dev->index);
			return -1;
		}
	}

	/* Try to bring link up at fastest clockdiv but do not touch
	 * start-up clockdivisor */
// 	clkdiv = -1;
// 	grspw_link_ctrl(dev->dh, NULL, NULL, &clkdiv);
	ctrl = LINKOPTS_ENABLE | LINKOPTS_AUTOSTART | LINKOPTS_START;
// 	clkdiv &= 0xff00;
    clkdiv = 0x03;
	grspw_link_ctrl(dev->dh, &ctrl, NULL, &clkdiv);

	if ( (dev->hwsup.hw_version >> 16) == GAISLER_SPW2_DMA )
		printf(">> NOTE: running on SPW-ROUTER DMA SpaceWire link (no link-state available)\n");
	else
		printf(" After Link Start: %d\n", (int)grspw_link_state(dev->dh));
	dev->run = 0;

	grspw_stats_clr(dev->dh);

	for (i=0; i<dev->hwsup.ndma_chans; i++) {
		if (dev->dma[i])
			grspw_dma_stats_clr(dev->dma[i]);
	}
	
	grspw_list_clr(&dev->rx_list);
	grspw_list_clr(&dev->tx_list);
	grspw_list_clr(&dev->tx_buf_list);
	dev->rx_list_cnt = dev->tx_list_cnt = dev->tx_buf_list_cnt = 0;
#endif
	return 0;
}

void init_pkts(void)
{
#if 0
	spwpkt *pkt;
	int i, j;

	memset(&pkts[0][0], 0, sizeof(pkts));

	for (i = 0; i < DEVS_MAX; i++) 
    {
		grspw_list_clr(&devs[i].rx_list);
		grspw_list_clr(&devs[i].tx_list);
		grspw_list_clr(&devs[i].tx_buf_list);
		devs[i].rx_list_cnt = 0;
		devs[i].tx_list_cnt = 0;
		devs[i].tx_buf_list_cnt = 0;
		for (j = 0, pkt = &pkts[i][0]; j < 120+16; j++, pkt = &pkts[i][j]) 
        {
			pkt->p.pkt_id = (i << 8)+ j; /* unused */
			pkt->p.data = &pkt->data[0];
			pkt->p.hdr = &pkt->hdr[0];
            
			if (j < 120+8) 
            {
				/* RX buffer */

				/* Add to device RX list */
				grspw_list_append(&devs[i].rx_list, &pkt->p);
				devs[i].rx_list_cnt++;
			} else 
            {
				/* TX buffer */
				pkt->p.dlen = PKT_SIZE;
// 				memset(pkt->p.data+4, i, PKT_SIZE-4);
                memset(pkt->p.data+4, '*', 1);
                memset(pkt->p.data+5, 'h', 1);
                memset(pkt->p.data+6, 'e', 1);
                memset(pkt->p.data+7, 'l', 1);
                memset(pkt->p.data+8, 'l', 1);
                memset(pkt->p.data+9, '0', 1);
                memset(pkt->p.data+10, '*', 1);
                memset(pkt->p.data+11, 0x0A, 1);

				/* Add to device TX list */
				grspw_list_append(&devs[i].tx_buf_list, &pkt->p);
				devs[i].tx_buf_list_cnt++;
			}
		}
	}
#endif
}

#if 0
rtems_task dma_task(rtems_task_argument unused)
{
    int num_pkt = 0;
	int i;
	grspw_device *dev;

	printf("Started DMA control task\n");

	while (tasks_stop == 0)
        {
		//rtems_semaphore_obtain(dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  double ElapsedSeconds=0;
  num_pkt = 0;
   MainLoop_StartTime = GetStartTime();
		for (i = 0; i < nospw; i++) {
			dev = &devs[i];
			if (dev->dh == NULL)
				continue;

			dma_process(dev);
		}
          MainLoop_time = GetElapsedTime(MainLoop_StartTime);        
        if(ioBuffer.inIndexBytes == 0)//100000)
        {
            num_pkt =  ioBuffer.samples[0];
            num_pkt =  num_pkt << 24;
            num_pkt += ioBuffer.samples[1];
            num_pkt =  num_pkt << 16;
            num_pkt += ioBuffer.samples[2];
            num_pkt =  num_pkt << 8;
            num_pkt += ioBuffer.samples[3];

           printf("%d  %f\n", num_pkt, MainLoop_time);      
        }
        else if(ioBuffer.inIndexBytes == 100004)//200000)
        {
            num_pkt =  ioBuffer.samples[100004];
            num_pkt =  num_pkt << 24;
            num_pkt += ioBuffer.samples[100005];
            num_pkt =  num_pkt << 16;
            num_pkt += ioBuffer.samples[100006];
            num_pkt =  num_pkt << 8;
            num_pkt += ioBuffer.samples[100007];

           printf("%d  %f\n", num_pkt, MainLoop_time);  
        }      
	}

	printf(" DMA task shutdown\n");

	rtems_task_delete(RTEMS_SELF);
}

int dma_process(grspw_device *dev)
{
    int cnt, rc, i;
    struct grspw_list lst;
    struct grspw_pkt *pkt;
    unsigned char *c;

    int rx_ready, rx_sched, rx_recv, rx_hwcnt;

    grspw_dma_rx_count(dev->dma[0], &rx_ready, &rx_sched, &rx_recv, &rx_hwcnt);

    /* Prepare receiver with packet buffers */
    if (dev->rx_list_cnt > 0) {
            rc = grspw_dma_rx_prepare(dev->dma[0], 0, &dev->rx_list,
                                                    dev->rx_list_cnt);
            if (rc != 0) {
                    printf("rx_prep failed %d\n", rc);
                    return -1;
            }
            /*printf("GRSPW%d: Prepared %d RX packet buffers for future "
                   "reception\n", dev->index, dev->rx_list_cnt);*/
            grspw_list_clr(&dev->rx_list);
            dev->rx_list_cnt = 0;
    }

    /* Try to receive packets on receiver interface */
    grspw_list_clr(&lst);
    //cnt = -1; /* as many packets as possible */
    cnt = 1; /* receive only 1 packets  */
    rc = grspw_dma_rx_recv(dev->dma[0], 0, &lst, &cnt);     
    /* Receive at least one packet */

                   
    if (rc != 0) 
    {
            printf("rx_recv failed %d\n", rc);
            return -1;
    }
    if (cnt > 0) 
    {
        //printf("GRSPW%d: Recevied %d packets\n", dev->index, cnt);
        for (pkt = lst.head; pkt; pkt = pkt->next) 
        {
            if ((pkt->flags & RXPKT_FLAG_RX) == 0) 
            {
                printf(" PKT not received.. buf ret\n");
                continue;
            } 
            else if (pkt->flags & (RXPKT_FLAG_EEOP | RXPKT_FLAG_TRUNK)) 
            {
                printf(" PKT RX errors:");
                if (pkt->flags & RXPKT_FLAG_TRUNK) 
                {
                    printf(" truncated");
                }
                
                if (pkt->flags & RXPKT_FLAG_EEOP)   
                {
                    printf(" EEP"); 
                    printf(" (0x%x)", pkt->flags);
                }
            } 
            else
            {
                c = (unsigned char *)pkt->data;

                memcpy(&ioBuffer.samples[ioBuffer.inIndexBytes], &(c[4]), 100000);

                 ioBuffer.inIndexBytes = ioBuffer.inIndexBytes + 100000;   // for primaryIterations == 100

                 if(ioBuffer.inIndexBytes == 100000)//100000)
                 {
                     executionStatus = EXECUTING;
                     ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;  
                 }
                 else if(ioBuffer.inIndexBytes == 100000*2)//200000)
                 {	
                     executionStatus = EXECUTING;
                     ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                     ioBuffer.inIndexBytes = 0;
                 }
            }
        }
        /* Reuse packet buffers by moving packets to rx_list */
        grspw_list_append_list(&dev->rx_list, &lst);
        dev->rx_list_cnt += cnt;
    }
    return 0;
}

#endif


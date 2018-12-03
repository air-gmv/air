
#ifndef INTERFACE_H_
#define INTERFACE_H_

/////////////////////////////////
// DEFINE configuration
/////////////////////////////////
// There are three different options to run the PowerPC_monitor:
// - In Windows:         default, no define is needed
// - In the PPC board: PPC define, the IP configuration is different and the endianness too (big-endian)  
// WARNING!! In the PPC it needs to be defined _rtems_app , but I think this is defined while building....
//
// - In the ZYNQ board:  ZYNQ define, the IP configuration is diferent (the same than with the PPC) and the 
//                       endianness is the same than in Windows (little-endian)
// WARNING!! In the ZYNQ board the _rtems_app is not defined, but there exists new *.c and *.h files
// (i.e. default-network-init.h,init.c, and network-config.h)
//
// There are other defines:
// - NAVFILTER: if the Kalman Navigation Filter is included in the processing
// - PROFILER:  to track execution time of the code
//
// WARNING!! All these defines need to be defined in the same way both in the PowerPC_monitor and SRSA
//           As an example, if NAVFILTER is not defined, the Navigation Filter parameters are not read and not sent, 
//           and the SRSA Kalman filter will not work. In the same way, if PROFILER is not defined the TC/TM will 
//           be diferent in the PowerPC_monitor and SRSA and will not work
//

/////////////////////////////////
// DEFINE configuration
/////////////////////////////////
#define PCMonitor

#define LEON4
//#define ZYNQ
#define PPC
//#define NAVFILTER
#define PROFILER

//#define LOG_PRINT

//#define SEB_FFT

//#define MEMORY_ALLOCATION
/////////////////////////////////
// DEFINE configuration
/////////////////////////////////

#ifdef _rtems_app
#define IP_SERV				"172.19.13.192"//"192.168.56.100"
#define IP_CLIENT			"172.19.13.53"//"192.168.56.1"

#else


#define IP_SERV		"172.19.1.147"	    // SJWI ip number
#define IP_CLIENT	"172.19.1.147"		// SJWI ip number
#include <stdio.h>
#include <winsock2.h>

#include <fft_seb.h>
//#define IP_SERV		"127.0.0.1"	// MMMJ ip number
//#define IP_CLIENT	"127.0.0.1"		// MMMJ ip number
//#define IP_SERV		"172.19.1.28"   // PMBB ip number
//#define IP_CLIENT	"172.19.1.28"   // PMBB ip number

#endif

#define MAX_BUFFER_SIZE 65000 

#define SERVPORT	3333
#define CLIENTPORT	3334

#ifdef TCP_interface
int initializeTCP(void);
void SendTcpOK(void);
int RecvTcp(void);
void TcpBufferCopy(void);
int receiveTCP_TC(void);
#else
int initializeUDP(void);
void SendUdpOK(void);
int RecvUdp(void);
void UdpBufferCopy(void);
int receiveUDP_TC(void);
#endif

int GetCNF(void);

void PrepSendBuffer(void);
void SendBuffer(void);
void SendResultCalculation(char *BufferToSend, int LenToSend);

void memory_statistic(int memory_bytes, char *place, int *address, int add_sub);

#ifdef _rtems_app
//
struct timeval GetStartTime();
double GetElapsedTime(struct timeval StartingTime);
#else
//
LARGE_INTEGER GetStartTime();
double GetElapsedTime(LARGE_INTEGER StartingTime);
#endif

typedef struct
{
    int sat;
    int gps_gal;
    int sync_nosync;
    
    double time;
    
} Tck_info;  

#include <rtems.h>

typedef struct  
{
  char       name[5];
  rtems_id   id;
  rtems_id   perd_id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       ran;
  int        actual_cpu;
}task_param_t;
    
rtems_task Tck1_task( rtems_task_argument unused );
rtems_task Tck2_task( rtems_task_argument unused );
rtems_task Tck3_task( rtems_task_argument unused );
rtems_task Acq1_task( rtems_task_argument unused );
rtems_task Acq2_task( rtems_task_argument unused );
rtems_task Main_task( rtems_task_argument unused );

/************************ SPW INTERFACE ************************/
int dev_init(int idx);
void init_pkts(void);
rtems_task dma_task(rtems_task_argument unused);

#endif // INTERFACE_H_ 

#include <manag.h>
#include <rtems/thread.h>
#include <interface.h>
//#include <spwinterface.h>

#include "siria.h"
#include <configuration.h>
#include <dataBuffer.h>
#include <init.h>
#include <pvtFilter.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************

//*****************************************************************************
//                        GLOBAL EXTERNAL VARIABLES
//*****************************************************************************
extern T_ComData		ComData;
extern T_Config			Config;
extern S_PVT_Process		pvtProcess;
extern T_DAid			aidingData;
extern T_supportData		supportData[NSYSTEMS];
extern S_MeasProcess		MeasProcessComb;
extern S_TimeStamp		timeStamp;
extern T_ChData			chDataAll;
extern S_gnssProcessingStatus	gnssProcessingStatus;
extern S_rxProcessingStatus	rxProcessingStatus;
extern S_AcqTckAid		AcqTckAid;
extern circularBufferStruct	Core3DataBuf;
extern E_ExecutionStatus        executionStatus;        // Execution status.

extern int                      nBytesToRead;

S_Core                          core[NCORES];
int		                core_index = 0;
int		                nsystems_index = 0;

T_Channel                       Tck1Ch[MAXNUMCHANPERCORE];
T_Channel                       Tck2Ch[MAXNUMCHANPERCORE];
T_Channel                       Tck3Ch[MAXNUMCHANPERCORE];

/* Init Tck channel idx */
int                             Tck1ChIdx = 0;
int                             Tck2ChIdx = 0;
int                             Tck3ChIdx = 0;

/* SpW Interface */
#if 0
extern int nospw; 
extern grspw_device devs[DEVS_MAX];
extern struct grspw_config dev_def_cfg;
extern int dma_process(grspw_device *dev);
#endif
#ifdef _rtems_app
extern struct timeval MainLoop_StartTime;
extern struct timeval AcquisitionTrackingAid_StartTime;
extern struct timeval ManageFftAcquisition_StartTime;
extern struct timeval galManagerSignalProcessing_StartTime;
extern struct timeval ManageLoopTracking_StartTime;

extern struct timeval FftAcq_StartTime;
extern struct timeval gpsVtAcq_StartTime;
extern struct timeval galVtAcq_StartTime;

extern struct timeval Tck1_StartTime;
extern struct timeval Tck2_StartTime;
extern struct timeval Tck3_StartTime;

extern struct timeval startReceiverProcessing_StartTime;
extern struct timeval generalManagerRx_StartTime;
extern struct timeval ManageFftAcquisition_StartTime;
extern struct timeval NewVtSatellites_StartTime;
extern struct timeval manageLoopTracking_StartTime;
extern struct timeval manageMeasAndPvt_StartTime;
#else
//
extern LARGE_INTEGER generalManagerRx_StartTime;
extern LARGE_INTEGER ManageFftAcquisition_StartTime;
extern LARGE_INTEGER NewVtSatellites_StartTime;
extern LARGE_INTEGER manageLoopTracking_StartTime;
extern LARGE_INTEGER manageMeasAndPvt_StartTime;
//
#endif
//
extern double MainLoop_time;
extern double AcquisitionTrackingAid_time;
extern double ManageFftAcquisition_time;
extern double galManagerSignalProcessing_time; 

extern double ManageLoopTracking1_time; 
extern double ManageLoopTracking2_time; 
extern double ManageLoopTracking3_time; 

extern double gpsFftAcq_time;
extern double galFftAcq_time;

extern double gpsVtAcq_time;
extern double galVtAcq_time;

extern Tck_info Tck_info_tab[10][MAXNUMCHAN];

extern int    gpssatTckSync_tab[12];
extern int    galsatTckSync_tab[12];
extern int    gpssatTckNoSync_tab[12];
extern int    galsatTckNoSync_tab[12];
extern double gpsTckSync_time_tab[12];
extern double galTckSync_time_tab[12];
extern double gpsTckNoSync_time_tab[12];
extern double galTckNoSync_time_tab[12];

extern double TckSync_time;
extern double TckNoSync_time;

extern double startReceiverProcessing_time;
extern double generalManagerRx_time;
extern double ManageFftAcquisition_time;
extern double NewVtSatellites_time;
extern double manageLoopTracking_time;
extern double manageMeasAndPvt_time;

extern int acq_svn;
extern int acq_system_flag;
//
int bylazamiana = 0;
int        gpstcksync_idx = 0;
int        galtcksync_idx = 0;    
int        gpstcknosync_idx = 0;
int        galtcknosync_idx = 0;  
        
int gps_now = 0;
int tcksync_idx = 0;
int tcknosync_idx = 0;

int managloop_idx = 0;
int iter = 0;                        
double gpsManagerSignalProcessing_time_tab[10]; 
double galManagerSignalProcessing_time_tab[4];
double ManageLoopTracking_time_tab[10];
                 

int count1 = 0;
int counter1 = 0;

int count2 = 0;
int counter2 = 0;

int count3 = 0;
int counter3 = 0;

int initFirstLoop = 1;
//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
static void manageMeasAndPvt(T_supportData *supportData_P, T_ChData *chData_P);
static void pvtManagement   (T_supportData *supportData_P);

//*****************************************************************************


//*****************************************************************************
//                        PUBLIC FUNCTION BODIES
//*****************************************************************************

/*****************************************************************************/
/*																			 */
/* void ReceiverProcessing(void)											 */
/*																			 */
/* This is the highest-level module of SRX (initialization excluded)		 */
/* It manages the full receiver processing, until an EXIT condition is found */
/* In case of dual mode, it is common for both constellations				 */
/*																			 */
/*																			 */
/*****************************************************************************/
extern task_param_t task_param[6];
int rec_ok = 1;
rtems_event_set in = 0;
rtems_event_set out = 0;

void ReceiverProcessing(void)
{        
        int rc = 0;
	// aiding variables
	#define		MAX_SAT_TABLE_SIZE_GPS	MAXNUMGPS	// 32
	#define		MAX_SAT_TABLE_SIZE_GAL	MAXNUMGAL	// 32
        double		gpsDoppler[MAX_SAT_TABLE_SIZE_GPS];
	int		gpsDopplerRange[MAX_SAT_TABLE_SIZE_GPS],
			gpsSampleCode[MAX_SAT_TABLE_SIZE_GPS],
			gpsSampleCodeRange[MAX_SAT_TABLE_SIZE_GPS],
			gpsAcqFlag[MAX_SAT_TABLE_SIZE_GPS];
	double		galDoppler[MAX_SAT_TABLE_SIZE_GAL];
	int		galDopplerRange[MAX_SAT_TABLE_SIZE_GAL],
			galSampleCode[MAX_SAT_TABLE_SIZE_GAL],
			galSampleCodeRange[MAX_SAT_TABLE_SIZE_GAL],
			galAcqFlag[MAX_SAT_TABLE_SIZE_GAL];		
	T_AidData aidData;
	// end of aiding variables

        T_Channel *chanAux;

//        grspw_device *dev;
        
	int i = 0, j = 0, msTh = ComData.lengthMs/2;
        int primaryIterations = 100;//4;
    
    #ifdef PROFILER
    double ElapsedSeconds=0;
     #endif

	/************************************************************************/
	/* Read samples from I/O buffer and transfer to PROCESSING buffer until */
	/* there are sufficient samples for starting processing.				   */
	/************************************************************************/
	while (ComData.counterMs < msTh)
	{
            // Copy samples from IO buffer to PROCESSING buffer.
            copyBuffer();

            for (i = 0; i < Core3DataBuf.nMsPerRead; i++)
            {
                for (j = 0; j < primaryIterations; j++)
                {
                    int cpt = ComData.counterMs;
                    cpt = cpt %4;

                    // Update buffer control index GPS
                    if ( (cpt == 0 && Config.Com.systemFlag == 2) || Config.Com.systemFlag != 2 )
                    {
                        UpdateOutIndex(&supportData[0]);
                    }
                    // Update buffer control index GAL
                    if ( cpt == 0 && (Config.Com.systemFlag == -1 || Config.Com.systemFlag == 2) )
                    {
                        UpdateOutIndex(&supportData[1]);
                    } 
                    // Update buffer control variables.
                    ComData.counterMs++;
                }
            }
	}
        
	ComData.counterMs = 0;
    
	supportData[0].outIndex.word = 0;
	supportData[0].outIndex.bit  = 0;
        
	supportData[1].outIndex.word = 0;//-supportData[1].outSize.word/4*3;
                
	supportData[1].outIndex.bit  = 0;    
    
	for(j = 0; j < NSYSTEMS; j++)
	{        
            for(core_index = 0; core_index < NACQCHAN; core_index++)		
            {
                supportData[0].acqAux_P[core_index].acqCounterMs = 0;
                supportData[1].acqAux_P[core_index].acqCounterMs = 0;
            }      
	}

	/*******************************************************************************************/
	/* Once sufficient samples are available in PROCESSING buffer, actual processing can start */
	/*******************************************************************************************/
    
	// Initialise the cores status
	for(int core_index1 = 0; core_index1 < NACQCHAN; core_index1++)		
	{
            core[core_index1].status         = FREE_C;
            core[core_index1].init           = START_ACQ1;
            core[core_index1].activityprev   = NONE;
            core[core_index1].activity       = ACQ;
            core[core_index1].nchan          = 0;
	}
	
	for(int core_index2 = NACQCHAN; core_index2 < NCORES-1; core_index2++)		
	{
            core[core_index2].status         = FREE_C;
            core[core_index2].init           = START_TASK;
            core[core_index2].activityprev   = NONE;
            core[core_index2].activity       = TCK;
            core[core_index2].nchan          = 0;
	}
	
	core[NCORES-1].status       = BUSY_C;
        core[NCORES-1].init         = DONE;
        core[NCORES-1].activityprev = NONE;
	core[NCORES-1].activity     = MAIN;
	core[NCORES-1].nchan        = 0;

	for(int idx1 = 0; idx1 < Config.Mng.nChannelspercore ; idx1++)
	{	
            resetTckChannel_core( &Tck1Ch[idx1]);
            resetTckChannel_core( &Tck2Ch[idx1]);
            resetTckChannel_core( &Tck3Ch[idx1]);
	}  
               
	/*************/
	/* MAIN LOOP */
	/*************/
    executionStatus = WAIT_FOR_SAMPLES;
    rtems_status_code status;     // status of rtems function
   
#ifdef PCMonitor
while (1)
{
#else
while (continueExecution())
{
#endif     
//printf("execution loop start\n");
    #ifdef PROFILER
        MainLoop_StartTime = GetStartTime();
    #endif  
        
        if(Config.Com.onspw == 1) // Read samples from SpW
        {
#if 0
            for (int idx2 = 0; idx2 < nospw; idx2++) 
            {
                    dev = &devs[idx2];
                    if (dev->dh == NULL)
                            continue;

                    dma_process(dev);
           }
#endif 
        }
        else // Read samples from UDP
        {   
            #ifdef PCMonitor
            if(rec_ok)	
            {
                rec_ok = 0;

                while (rec_ok != 1)
                {
    #ifdef TCP_interface            
                            SendTcpOK();
    #else
                SendUdpOK();  
    #endif  

    #ifdef TCP_interface
            rc = RecvTcp();
    #else
//            printf("receiving udp\n");
        rc = RecvUdp();
    #endif
  //       printf("received %d\n", rc);
                    if(rc == nBytesToRead/25)// for primaryIterations == 100
                    //if(rc == nBytesToRead)     // for primaryIterations == 4
                    {
    #ifdef TCP_interface            
                            TcpBufferCopy();
    #else
                        UdpBufferCopy();   
    #endif
                         ioBuffer.inIndexBytes = ioBuffer.inIndexBytes + nBytesToRead/25;   // for primaryIterations == 100
                         // ioBuffer.inIndexBytes = ioBuffer.inIndexBytes + nBytesToRead; // for primaryIterations == 4
                     //   printf("received bytes %d of %d...\n",ioBuffer.inIndexBytes, nBytesToRead  );
                        if(ioBuffer.inIndexBytes == nBytesToRead)//100000)
                        {
                         //   printf("nbytesread, samples max %d\n",ioBuffer.inIndexBytes );
                            ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                            rec_ok = 1;
                            executionStatus = EXECUTING;
                        }
                        else if(ioBuffer.inIndexBytes == nBytesToRead*2)//200000)
                        {	
                          //  printf("2 * nbytesread, samples max %d\n",ioBuffer.inIndexBytes );
                            ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                            ioBuffer.inIndexBytes = 0;
                            rec_ok = 1;
                            executionStatus = EXECUTING;
                        }
                    }
                    else if(rc == 1)
                    {
                        printf("no more samples\n");
                        executionStatus = NO_MORE_SAMPLES;
                        break;
                    }
                    else if(rc < 0)
                    {
                        executionStatus = NO_MORE_SAMPLES;
                        break;
                    }
                }
            }
            #endif
        }
        
//#ifdef MAIN_LOOP_GO  
        if(executionStatus == EXECUTING)
        {
         //   printf("executing\n");
            executionStatus = WAIT_FOR_SAMPLES;
            // Copy samples from IO buffer to PROCESSING buffer.
            copyBuffer();
 
            /*****************************************************************************************/
            // As many iterations as # of code periods in each read block of samples (from the buffer).
            /*****************************************************************************************/

            int nmscounter;
            for (nmscounter = 0; nmscounter < Core3DataBuf.nMsPerRead; nmscounter++)
            {                         
                /*****************************************************/
                // Code-period rate between GPS and GALILEO is 4-to-1:
                // process 4 GPS iterations per 1 GALILEO iteration.
                /*****************************************************/                               
                
                if(initFirstLoop = 0 && Config.Com.onspw == 1)
                {
                    for (int idx3 = 0; idx3 < core[0].nchan; idx3++)	
                    {
                        chDataAll.Ch[Tck1Ch[idx3].chIndex] = Tck1Ch[idx3];              
                    }

                    for (int idx4 = 0; idx4 < core[1].nchan; idx4++)	
                    {
                       chDataAll.Ch[Tck2Ch[idx4].chIndex] = Tck2Ch[idx4];
                    }                   

                    for (int idx5 = 0; idx5 < core[2].nchan; idx5++)                    
                    {
                        chDataAll.Ch[Tck3Ch[idx5].chIndex] = Tck3Ch[idx5];				
                    } 

                    /*********************************************************************************/
                    /* Check if current iteration corresponds to generation of measurements and PVT. */
                    /*********************************************************************************/                         
                    if(abs(ComData.counterMs-timeStamp.measBlock)<100)                       
                    {                       
                        #ifdef PROFILER
                            manageMeasAndPvt_StartTime = GetStartTime();
                        #endif

                        // Manage generation of measurements and PVT.
                        manageMeasAndPvt(supportData, &chDataAll);

                        #ifdef PROFILER
                            ElapsedSeconds = GetElapsedTime(manageMeasAndPvt_StartTime);
                            manageMeasAndPvt_time = manageMeasAndPvt_time + ElapsedSeconds;
                        #endif

                        #ifdef PCMonitor
                            // Send output data to PCmonitor
                            print("prepping buffer\n");
                            PrepSendBuffer();
                            SendBuffer();
                            print("Sent buffer\n");
                        #endif                    
                    } 
                }

/* setup Tracking if acq new satelite */  
                for(int core_index1 = 0; core_index1 < NACQCHAN; core_index1++)		
                {         
                 //   printf("acq new stellite %d\n", core_index1);
                    status = rtems_event_receive(RTEMS_EVENT_5, RTEMS_EVENT_ALL | RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT, &out);
                    if ( status == RTEMS_SUCCESSFUL ) 
                    {          
                        if(out == RTEMS_EVENT_5)
                        {
                            if (supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].acqFftStatus == ACQ_FINE && core[core_index1].status == BLOCKED_C)
                            {
                                double doppler = -supportData[rxProcessingStatus.acqGalFlag].satManag_P->acqFftSat[supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].slotIndex].dopplerFreqHz;
                                int STR_TTCK_MS = ComData.counterMs;
                                double delta_t = (double) ( STR_TTCK_MS - supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].fftMs)/1000;
                                
                                int samplecode_delta = (int) round((doppler*delta_t * ComData.sampFreqIQ2 / GPSL1FREQ));
                                
                                supportData[rxProcessingStatus.acqGalFlag].satManag_P->acqFftSat[supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].svnIndex].codeSamples += samplecode_delta;                                                
                                setUpTracking(&supportData[rxProcessingStatus.acqGalFlag], &chDataAll, core_index1);

                                if (supportData[rxProcessingStatus.acqGalFlag].genSysData_P->systemFlag == 2)
                                {
                                    gnssProcessingStatus.galProcessingStatus[supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].slotIndex].acqStatus = 1;
//                                    if(rxProcessingStatus.acqInitFlag == 0 && Config.Com.systemFlag != 0)
//                                    {
//                                        rxProcessingStatus.acqGalFlag = 0;
//                                    }
                                }
                                else if (supportData[rxProcessingStatus.acqGalFlag].genSysData_P->systemFlag == 0)
                                {
                                    gnssProcessingStatus.gpsProcessingStatus[supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index1].slotIndex].acqStatus = 1;
//                                    if(rxProcessingStatus.acqInitFlag == 0 && Config.Com.systemFlag != 0)
//                                    {
//                                        rxProcessingStatus.acqGalFlag = 1;
//                                    }
                                }                                           
                            }
                            else
                            {
                                core[core_index1].status = FREE_C;
                                core[core_index1].activity = ACQ;
                            } 
                        }                                            
                    }  
                }

                // Manage VirtualTracking Acquisition GPS
                if (rxProcessingStatus.vtQueueGps.N > supportData[0].satManag_P->vtChannels.N && aidData.pvtStatus == 3 )
                //if (rxProcessingStatus.vtQueueGps.N > supportData[0].satManag_P->vtChannels.N && aidData.pvtStatus >= 0 )
                {
                    #ifdef PROFILER
                        gpsVtAcq_StartTime = GetStartTime();
                    #endif

                    ManageVtAcq(&supportData[0], &chDataAll, &AcqTckAid);

                    #ifdef PROFILER
                        ElapsedSeconds = GetElapsedTime(gpsVtAcq_StartTime);
                        gpsVtAcq_time = gpsVtAcq_time + ElapsedSeconds;                        
                    #endif	                        
                }

                // Manage VirtualTracking Acquisition GAL
                if( rxProcessingStatus.vtQueueGal.N > supportData[1].satManag_P->vtChannels.N && aidData.pvtStatus == 3)
                //if( rxProcessingStatus.vtQueueGal.N > supportData[1].satManag_P->vtChannels.N && aidData.pvtStatus >= 0)
                {
                    #ifdef PROFILER
                        galVtAcq_StartTime = GetStartTime();
                    #endif

                    ManageVtAcq(&supportData[1], &chDataAll, &AcqTckAid);

                    #ifdef PROFILER
                        ElapsedSeconds = GetElapsedTime(galVtAcq_StartTime);
                        galVtAcq_time = galVtAcq_time + ElapsedSeconds;                        
                    #endif	

                }                 
            
                for(int idx6 = 0; idx6 < Config.Mng.nChannelspercore ; idx6++)
                {	
                    resetTckChannel_core( &Tck1Ch[idx6]);
                    resetTckChannel_core( &Tck2Ch[idx6]);
                    resetTckChannel_core( &Tck3Ch[idx6]);
                } 
            
                for (int idx7 = 0; idx7 < Config.Mng.nChannels; idx7++)
                {
                    // Auxiliar pointer to one channel.
                    chanAux = &chDataAll.Ch[idx7];

                    chanAux->outIndex.bit			 = supportData[0].outIndex.bit;
                    chanAux->outIndex.word			 = supportData[0].outIndex.word;
                    chanAux->corrAuxIndex.outSize_P              = &supportData[0].outSize;
                    chanAux->chCounterMs = ComData.counterMs;
                } 

/* split chDataAll to three tck data structures */   
            int nchan;   
            // check if any core is available for tracking
            for(int core_local_index = 1; core_local_index < NCORES-1; core_local_index++)		
            {
                if (core[core_local_index].nchan < Config.Mng.nChannelspercore)
                {
                    for (nchan = 0; nchan < Config.Mng.nChannels; nchan++)
                    {
                        if(chDataAll.Ch[nchan].core_index == 0 && core[core_local_index].nchan < Config.Mng.nChannelspercore)
                        {
                           chDataAll.Ch[nchan].core_index = core_local_index;
                           core[core_local_index].nchan++;
                           core[0].nchan--;

                           if(core[0].nchan <= 0)
                           {
                                core[0].activityprev = TCK;
                                core[0].activity = ACQ;
                                core[0].status   = FREE_C;
                                break;
                           }
                        } 
                    }
                }
            }

            Tck1ChIdx = 0;
            Tck2ChIdx = 0;
            Tck3ChIdx = 0;  
            
            for (nchan = 0; nchan < Config.Mng.nChannels; nchan++)
            {               
                switch(chDataAll.Ch[nchan].core_index)
                {
                    case 0:                            
                            Tck1Ch[Tck1ChIdx] = chDataAll.Ch[nchan];
                            Tck1ChIdx++;
                    break;
                        
                    case 1:             
                            Tck2Ch[Tck2ChIdx] = chDataAll.Ch[nchan];
                            Tck2ChIdx++;
                    break;
                        
                    case 2:                            
                            Tck3Ch[Tck3ChIdx] = chDataAll.Ch[nchan];
                            Tck3ChIdx++;
                    break;
                }						
            }  
                in = 0;      

/* start tck tasks */       

            if(core[0].activity == TCK)
            {              
                in |= (RTEMS_EVENT_1);
                if(core[0].init == START_TCK1 && core[0].activityprev == ACQ)       
                {   
                    core[0].activityprev = NONE;
                    rtems_task_start( task_param[ 0 ].id, Tck1_task, 0 );   
                    core[0].init = DONE; 
                }
                else if(core[0].activityprev == ACQ)
                {
                    core[0].activityprev = NONE;
                    rtems_task_restart(task_param[ 0 ].id, 0 ); 
                }
                else
                {
                    rtems_task_restart(task_param[ 0 ].id, 0 ); 
                }                                
            }

            if(core[1].activity == TCK)
            {           
                in |= (RTEMS_EVENT_2);

                if(core[1].init == START_TASK)       
                {
                    rtems_task_start( task_param[ 1 ].id, Tck2_task, 1 );   
                   core[1].init = DONE; 
                }  
                else
                {
                    rtems_task_restart(task_param[ 1 ].id, 1 );
                }
            }

     /*       if(core[2].activity == TCK)
            {               
                in |= (RTEMS_EVENT_3);

                if(core[2].init == START_TASK)       
                {     
                    rtems_task_start( task_param[ 2 ].id, Tck3_task, 2 );  
                    core[2].init = DONE; 
                }  
                else
                {
                    rtems_task_restart(task_param[ 2 ].id, 2 ); 
                }                
            }*/
        //    printf("tasks started\n");

                for (int idx7 = 0; idx7 < primaryIterations; idx7++)
                {                                                          
                    int cpt = ComData.counterMs%4;
                  //  printf("it %d cpt %d\n", idx7, cpt);                                        
                    // copy data into aiding function structures ( possibly to be modified)
                    if ( pvtProcess.ekfData.solFlag == 0 ) //timeStamp.gpsTime.TOW <= pvtProcess.firstPvtTow + 3 && pvtProcess.firstPvtFlag == 0)
                    {
                            memcpy(&aidData.lsData,&pvtProcess.lsData,sizeof(pvtProcess.lsData));
                            aidData.lsData.V[0] = -aidData.lsData.V[0]; // needed until velocity lsq gives negative velocity
                            aidData.lsData.V[1] = -aidData.lsData.V[1];
                            aidData.lsData.V[2] = -aidData.lsData.V[2];

                            memcpy(&aidData.lsDataPrev,&pvtProcess.lsDataPrev,sizeof(pvtProcess.lsDataPrev));
                            aidData.lsDataPrev.V[0] = -aidData.lsDataPrev.V[0]; // needed until velocity lsq gives negative velocity
                            aidData.lsDataPrev.V[1] = -aidData.lsDataPrev.V[1];
                            aidData.lsDataPrev.V[2] = -aidData.lsDataPrev.V[2];
                    }
                    else
                    {
                            aidData.lsData.X[0] = pvtProcess.ekfData.x[0]; // needed until velocity lsq gives negative velocity
                            aidData.lsData.X[1] = pvtProcess.ekfData.x[1];
                            aidData.lsData.X[2] = pvtProcess.ekfData.x[2];
                            aidData.lsData.V[0] = pvtProcess.ekfData.x[3]; // needed until velocity lsq gives negative velocity
                            aidData.lsData.V[1] = pvtProcess.ekfData.x[4];
                            aidData.lsData.V[2] = pvtProcess.ekfData.x[5];
                            aidData.lsData.clkData.deltaClkBias		= pvtProcess.ekfData.x[6]/LIGHTSPEED; // needed until velocity lsq gives negative velocity
                            aidData.lsData.clkData.deltaClkBiasGal	= pvtProcess.ekfData.x[7]/LIGHTSPEED;
                            aidData.lsData.clkData.clkDrift			= pvtProcess.ekfData.x[8]/LIGHTSPEED;

                            aidData.lsDataPrev.X[0] = pvtProcess.ekfData.prev_x[0]; // needed until velocity lsq gives negative velocity
                            aidData.lsDataPrev.X[1] = pvtProcess.ekfData.prev_x[1];
                            aidData.lsDataPrev.X[2] = pvtProcess.ekfData.prev_x[2];
                            aidData.lsDataPrev.V[0] = pvtProcess.ekfData.prev_x[3]; // needed until velocity lsq gives negative velocity
                            aidData.lsDataPrev.V[1] = pvtProcess.ekfData.prev_x[4];
                            aidData.lsDataPrev.V[2] = pvtProcess.ekfData.prev_x[5];
                            aidData.lsDataPrev.clkData.deltaClkBias		= pvtProcess.ekfData.prev_x[6]/LIGHTSPEED; // needed until velocity lsq gives negative velocity
                            aidData.lsDataPrev.clkData.deltaClkBiasGal	= pvtProcess.ekfData.prev_x[7]/LIGHTSPEED;
                            aidData.lsDataPrev.clkData.clkDrift			= pvtProcess.ekfData.prev_x[8]/LIGHTSPEED;
                    }
                    memcpy(&aidData.gpsAlmEph,&aidingData.gpsAlmEph,sizeof(aidingData.gpsAlmEph));
                    memcpy(&aidData.galAlmEph,&aidingData.galAlmEph,sizeof(aidingData.galAlmEph));
                    memcpy(&aidData.pvtStatus,&aidingData.pvtAcqStatus,sizeof(aidingData.pvtAcqStatus));
                    memcpy(&aidData.refTime,&aidingData.refTime,sizeof(aidingData.refTime));
                    memcpy(&aidData.attQuat,&aidingData.refAttQuat,sizeof(aidingData.refAttQuat));
                    aidData.gpsAlmEphPosCov = 400;
                    aidData.galAlmEphPosCov = 400;	
                    aidData.gpsAlmEphVelCov = 3;
                    aidData.galAlmEphVelCov = 3;

                    // Manage Aiding - Select satellite for FFT or VT
                    #ifdef PROFILER
                        AcquisitionTrackingAid_StartTime = GetStartTime();
                    #endif 
                        
                   AcquisitionTrackingAid(supportData, &chDataAll, &aidData, &AcqTckAid);
                    
                    #ifdef PROFILER
                        ElapsedSeconds = GetElapsedTime(AcquisitionTrackingAid_StartTime);
                        AcquisitionTrackingAid_time = AcquisitionTrackingAid_time + ElapsedSeconds;
                    #endif	
                                                                
                    // perform 1st FFt acquisition
                    if (cpt == 0)
                    {	                        
                        for(int core_index3 = 0; core_index3 < NACQCHAN; core_index3++)		
                        {
                            if (core[core_index3].status == FREE_C && core[core_index3].activity == ACQ)
                            {           
                                core[core_index3].status = BUSY_C;
                                copyBufferAcq(supportData[rxProcessingStatus.acqGalFlag].outIndex.word);
                                        
                                supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index3].outIndex_init.bit  = 0;
                                supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index3].outIndex_init.word = 0;
                                supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index3].acqCounterMs  = ComData.counterMs;
                                acq_svn = supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index3].slot;
                                acq_system_flag = supportData[rxProcessingStatus.acqGalFlag].genSysData_P->systemFlag;
                            //    printf("ACQ core[0] activity: %d core[0] init: %d core[0] activitprev %d\n", core[0].activity, core[0].init, core[0].activityprev);
                                switch(core_index3)
                                {
                                    case 0:
                                        if(core[core_index3].init == START_ACQ1)
                                        {
                                            rtems_task_start( task_param[ 3 ].id, Acq1_task, 3 );  
                                            core[core_index3].init = START_TCK1;
                                        }
                                        else
                                        {
                                            if(core[core_index3].activityprev == TCK)
                                            {                                        
                                                rtems_task_suspend(task_param[ 0 ].id); 
                                                core[core_index3].activityprev = NONE;
                                            }       
                                                rtems_task_restart(task_param[ 3 ].id, 3 );             
                                        } 
                                    break;
                                }
                            }
                        }
                   }                                                                
                    if (cpt == 0)      
                    { 
                        status = rtems_event_receive(RTEMS_EVENT_4, RTEMS_EVENT_ALL | RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT, &out);
                        if ( status == RTEMS_SUCCESSFUL ) 
                        {                                         
                            for(int core_index4 = 0; core_index4 < NACQCHAN; core_index4++)		
                            {                                        
                                if(out == RTEMS_EVENT_4)
                                {
                                    if (supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index4].acqFftStatus == ACQ_MED && core[core_index4].status == BLOCKED_C) // Acquiring medium frequency.
                                    {                                                                                        
                                        // perform 2nd FFT acquisition
                                        core[core_index4].status = BUSY_C;
                                        copyBufferAcq(supportData[rxProcessingStatus.acqGalFlag].outIndex.word);

                                        supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index4].outIndex_init.bit  = 0;
                                        supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index4].outIndex_init.word = 0;
                                        supportData[rxProcessingStatus.acqGalFlag].acqAux_P[core_index4].acqCounterMs  = ComData.counterMs;               
                                        rxProcessingStatus.secondFft = 1;

                                        // Perform the second acquisition
                                  //      printf("ACQ2 core[0] activity: %d core[0] init: %d core[0] activitprev %d\n", core[0].activity, core[0].init, core[0].activityprev);
                                        switch(core_index4)
                                        {
                                            case 0:     
                                                    rtems_task_restart(task_param[ 3 ].id, 3 );             
                                            break;
                                        }

                                    }
                                }
                            }                       
                        }
                    }             
                        
//                    // Manage VirtualTracking Acquisition GPS
//                    if (rxProcessingStatus.vtQueueGps.N > supportData[0].satManag_P->vtChannels.N && aidData.pvtStatus == 3 )
//                    //if (rxProcessingStatus.vtQueueGps.N > supportData[0].satManag_P->vtChannels.N && aidData.pvtStatus >= 0 )
//                    {
//                        #ifdef PROFILER
//                            gpsVtAcq_StartTime = GetStartTime();
//                        #endif
//                            
//                        ManageVtAcq(&supportData[0], &chDataAll, &AcqTckAid);
//
//                        #ifdef PROFILER
//                            ElapsedSeconds = GetElapsedTime(gpsVtAcq_StartTime);
//                            gpsVtAcq_time = gpsVtAcq_time + ElapsedSeconds;                        
//                        #endif	                        
//                    }
// 
//                    // Manage VirtualTracking Acquisition GAL
//                    if( rxProcessingStatus.vtQueueGal.N > supportData[1].satManag_P->vtChannels.N && aidData.pvtStatus == 3)
//                    //if( rxProcessingStatus.vtQueueGal.N > supportData[1].satManag_P->vtChannels.N && aidData.pvtStatus >= 0)
//                    {
//                        #ifdef PROFILER
//                            galVtAcq_StartTime = GetStartTime();
//                        #endif
//
//                        ManageVtAcq(&supportData[1], &chDataAll, &AcqTckAid);
//                        
//                        #ifdef PROFILER
//                            ElapsedSeconds = GetElapsedTime(galVtAcq_StartTime);
//                            galVtAcq_time = galVtAcq_time + ElapsedSeconds;                        
//                        #endif	
//                            
//                    } 
                    
                    // Update buffer control index GPS
                    if ( (cpt == 0 && Config.Com.systemFlag == 2) || Config.Com.systemFlag != 2 )
                    {
                        UpdateOutIndex(&supportData[0]);
                    }
                    // Update buffer control index GAL
                    if ( cpt == 0 && (Config.Com.systemFlag == -1 || Config.Com.systemFlag == 2) )
                    {
                        UpdateOutIndex(&supportData[1]);
                    }
                        
                    ComData.counterMs++;
                }          

                initFirstLoop = 0;                             
               
                if(Config.Com.onspw == 0)
                {
                    /* wait untill tck finish - RTEMS events are used for synchronization */      
                    rtems_event_set non_completed = in;
                   
                    while (non_completed) 
                    {
                        status = rtems_event_receive(in, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &out);
                        if ( status == RTEMS_SUCCESSFUL ) 
                        {
                            non_completed &= ~out;
                        }
                    } 
               //     printf("rtems_event_completed\n"); 
                    for (i = 0; i < core[0].nchan; i++)	
                    {
                        chDataAll.Ch[Tck1Ch[i].chIndex] = Tck1Ch[i];
                    }

                    for (i = 0; i < core[1].nchan; i++)	
                    {                     
                        chDataAll.Ch[Tck2Ch[i].chIndex] = Tck2Ch[i];                       
                    }                   

                    for (i = 0; i < core[2].nchan; i++)
                    {
                        chDataAll.Ch[Tck3Ch[i].chIndex] = Tck3Ch[i];	
                    } 

                    /*********************************************************************************/
                    /* Check if current iteration corresponds to generation of measurements and PVT. */
                    /*********************************************************************************/    
                        if(abs(ComData.counterMs-timeStamp.measBlock-100)<100)                       
                        { 
                            #ifdef PROFILER
                                manageMeasAndPvt_StartTime = GetStartTime();
                            #endif

                            // Manage generation of measurements and PVT.
                            manageMeasAndPvt(supportData, &chDataAll);

                            #ifdef PROFILER
                                ElapsedSeconds = GetElapsedTime(manageMeasAndPvt_StartTime);
                                manageMeasAndPvt_time = manageMeasAndPvt_time + ElapsedSeconds;
                            #endif

                            #ifdef PCMonitor
                                // Send output data to PCmonitor
                                printf("send buffer2\n");

                                PrepSendBuffer();
                                SendBuffer();
                                printf("sent buffer2\n");
                            #endif                    
                        } 
                    }               
                }
        }
        else if(executionStatus == NO_MORE_SAMPLES)
        {
            break;
        }
        //#endif   
#ifdef PROFILER
        ElapsedSeconds = GetElapsedTime(MainLoop_StartTime);
        MainLoop_time = MainLoop_time + ElapsedSeconds;
#endif    
    }
	return;
}

//*********************************************************************************
//                        INTERNAL FUNCTION BODIES
//*********************************************************************************

/**********************************************************************************************/
/*																							  */
/* void ManageLoopTracking( T_supportData *supportData_P)									  */
/*																							  */
/* Manage tracking for all channels of ONE constellation at ONE iteration.					  */
/*																							  */
/* It is organized in three consecutive loops:												  */
/*																							  */
/* - Loop for all channels in SYNC mode:    Bit transition is known and accumulations allowed */
/*																							  */
/* - Loop for all channels in NOSYNC mode:  Bit transition being still sought. Accumulations  */
/*											not possible yet (single correlations only).	  */
/*											GAL is always noSync, since code and bit periods  */
/*											coincide and correlations cannot be  accumulated  */
/* - Loop for all channels in VT mode.														  */
/*																							  */
/**********************************************************************************************/
void ManageLoopTracking(T_Channel *chData_P, int CurrentCpu, int nchan)
{
    int	i = 0;   
    int cur_idx1 = 0;
    int cur_idx2 = 0;
    int cur_idx3 = 0;
    
    #ifdef PROFILER
        double ElapsedSeconds1=0;
        double ElapsedSeconds2=0;
        double ElapsedSeconds3=0;
        
        gpstcksync_idx = 0;
        galtcksync_idx = 0;    
        gpstcknosync_idx = 0;
        galtcknosync_idx = 0;  
    #endif

    #ifdef PROFILER
      if(CurrentCpu == 1)
      {
            cur_idx1 = 0;
                    
      }
      else if(CurrentCpu == 2)
      {
            cur_idx2 = Config.Mng.nChannelspercore ;
      }
      else if (CurrentCpu == 0)
      {
            cur_idx3 = (Config.Mng.nChannelspercore ) * 2;  
      }
      #endif
        
    for (i = 0; i < nchan; i++)
    {                
         chData_P[i].corrAuxIndex.outIndex_P = &chData_P[i].outIndex;

        if (chData_P[i].chStatus > -1 && (chData_P[i].genSysData_P->systemFlag != 2 || (chData_P[i].chCounterMs  % 4 == 0)))
        {                      
                switch (chData_P[i].chStatus)
                {
                case 7:
                        /*************************
                         BIT-SYNCHRONIZED CHANNELS
                         *************************/

                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                              Tck1_StartTime = GetStartTime(); 
                        }
                        else if(CurrentCpu == 2)
                        {
                              Tck2_StartTime = GetStartTime(); 
                        }
                        else if (CurrentCpu == 0)
                        {
                              Tck3_StartTime = GetStartTime();   
                        }
                        #endif

                        TckSyncManagement(&chData_P[i], CurrentCpu);

                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                           ElapsedSeconds1 = GetElapsedTime(Tck1_StartTime);

                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1; 
                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1; 

                            } 
                        }
                        else if(CurrentCpu == 2)
                        {
                           ElapsedSeconds2 = GetElapsedTime(Tck2_StartTime);

                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][i].time + ElapsedSeconds2; 

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][cur_idx2].time + ElapsedSeconds2; 

                            } 
                        }
                        else if(CurrentCpu == 0)
                        {
                           ElapsedSeconds3 = GetElapsedTime(Tck3_StartTime);

                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][cur_idx3].time + ElapsedSeconds3; 

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][cur_idx3].time + ElapsedSeconds3; 

                            } 
                        }
                        #endif  

                        break;

                case 6:
                        /******************************
                         NON BIT-SYNCHRONIZED CHANNELS
                         ******************************/

                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                              Tck1_StartTime = GetStartTime(); 
                        }
                        else if(CurrentCpu == 2)
                        {
                              Tck2_StartTime = GetStartTime(); 
                        }
                        else if (CurrentCpu == 0)
                        {
                              Tck3_StartTime = GetStartTime();   
                        }
                        #endif

                        TckNoSyncManagement(&chData_P[i], CurrentCpu);

                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                            ElapsedSeconds1 = GetElapsedTime(Tck1_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1;                        

                            }   
                        }
                        else if(CurrentCpu == 2)
                        {
                            ElapsedSeconds2 = GetElapsedTime(Tck2_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][cur_idx2].time + ElapsedSeconds2;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][cur_idx2].time + ElapsedSeconds2;                        

                            }   
                        }
                        else if(CurrentCpu == 0)
                        {
                            ElapsedSeconds3 = GetElapsedTime(Tck3_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][i].time + ElapsedSeconds3;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][cur_idx3].time + ElapsedSeconds3;                        

                            }   
                        }
                        #endif                  

                        break;

                case 5:
                        /*************************************
                         VIRTUAL-TRACKING ACQUISITION CHANNELS
                         *************************************/
                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                              Tck1_StartTime = GetStartTime(); 
                        }
                        else if(CurrentCpu == 2)
                        {
                              Tck2_StartTime = GetStartTime(); 
                        }
                        else if (CurrentCpu == 0)
                        {
                              Tck3_StartTime = GetStartTime();   
                        }
                        #endif

                        TckVtManagement(&chData_P[i], CurrentCpu);

                        #ifdef PROFILER
                        if(CurrentCpu == 1)
                        {
                            ElapsedSeconds1 = GetElapsedTime(Tck1_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter1][cur_idx1].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter1][cur_idx1].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter1][cur_idx1].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter1][cur_idx1].time = Tck_info_tab[counter1][cur_idx1].time + ElapsedSeconds1;                        

                            }   
                        }
                        else if(CurrentCpu == 2)
                        {
                            ElapsedSeconds2 = GetElapsedTime(Tck2_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][cur_idx2].time + ElapsedSeconds2;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter2][cur_idx2].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter2][cur_idx2].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter2][cur_idx2].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter2][cur_idx2].time = Tck_info_tab[counter2][cur_idx2].time + ElapsedSeconds2;                        

                            }   
                        }
                        else if(CurrentCpu == 0)
                        {
                            ElapsedSeconds3 = GetElapsedTime(Tck3_StartTime);
                            if(chData_P[i].genSysData_P->systemFlag == 0)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][i].time + ElapsedSeconds3;                         

                            }
                            else if(chData_P[i].genSysData_P->systemFlag == 2)
                            {
                                Tck_info_tab[counter3][cur_idx3].sat = chData_P[i].SVN_true;
                                Tck_info_tab[counter3][cur_idx3].gps_gal = chData_P[i].genSysData_P->systemFlag;
                                Tck_info_tab[counter3][cur_idx3].sync_nosync = chData_P[i].chStatus;
                                Tck_info_tab[counter3][cur_idx3].time = Tck_info_tab[counter3][cur_idx3].time + ElapsedSeconds3;                        

                            }   
                        }
                        #endif                           
                        
                        break;
                }
            }
            #ifdef PROFILER
              if(CurrentCpu == 1)
              {
                    cur_idx1++;

              }
              else if(CurrentCpu == 2)
              {
                    cur_idx2++;
              }
              else if (CurrentCpu == 0)
              {
                    cur_idx3++;  
              }
            #endif         
	}
	
    if(CurrentCpu == 1)
    {
        count1++;
        if(count1 == 100)
        {
            counter1++;
            count1 = 0;
         if(counter1 == 10)
         {
             counter1 = 0;
         }
        }                   
    }
    else if(CurrentCpu == 2)
    {
        count2++;
        if(count2 == 100)
        {
            counter2++;
            count2 = 0;
         if(counter2 == 10)
         {
             counter2 = 0;
         }
        }
    }
    else if(CurrentCpu == 0)
    {
        count3++;
        if(count3 == 100)
        {
            counter3++;
            count3 = 0;
         if(counter3 == 10)
         {
             counter3 = 0;
         }
        }
    }
	return;
}


/******************************************************************************************/
/******************************************************************************************/
/*																					      */
/* void manageMeasAndPvt(T_supportData *supportDataGps_P,T_supportData *supportDataGal_P) */
/*																					      */
/* Tasks that must be executed at the desired output rate (typically once per second):    */
/*																					      */
/*	- Produce measurements (observations: PR, CP, Doppler, C/No...).					  */
/*	- Compute PVT.																		  */
/*	- Update next measurement time.														  */
/*	- Print traces (Line-Of-Commands and trace file).									  */
/*																						  */
/******************************************************************************************/
/******************************************************************************************/
static void manageMeasAndPvt(T_supportData *supportData_P, T_ChData *chData_P)
{
	/***********************************
      MANAGE GENERATION OF MEASUREMENTS
	 ***********************************/
	timeStampMeasManagement(supportData_P, &MeasProcessComb, chData_P);

	/************
      MANAGE PVT
	 ************/
	if (timeStamp.gpsTime.TOW > 0)
	{
		pvtManagement(supportData_P);
	}
	
	// EKF Filter
	pvtProcess.ekfData.solFlag = 0;
	double start_filter = pvtProcess.firstPvtTow + 3; 
	if(Config.Ekf.useEkf && timeStamp.gpsTime.TOW >= start_filter && pvtProcess.firstPvtFlag)
	{
		// initialize EKF
		// Initialize inputs/outputs and internal structures with zeros
		BlockIO_pvtFilter			pvtFilter_B;
		ExternalInputs_pvtFilter	pvtFilter_U;
		ExternalOutputs_pvtFilter	pvtFilter_Y;
		pvtFilter_initialize(	&pvtFilter_U,
								&pvtFilter_Y);
		(void) memset((void *)&pvtFilter_B, 0,sizeof(BlockIO_pvtFilter));
					
		pvtFilter_U.space_user_flag = Config.Ekf.spaceUser;
		pvtFilter_U.freq_L1 = supportData[0].genSysData_P->freqPerSv[0]; // Hz
		pvtFilter_U.speed_of_light = LIGHTSPEED; // m/s
		pvtFilter_U.dt = Config.Ekf.deltaT;
		// State noise 
		pvtFilter_U.state_noise[0] = Config.Ekf.stateNoiseCovPos;			// pos xyz m^2
		pvtFilter_U.state_noise[1] = Config.Ekf.stateNoiseCovVel;			// vel xyz m^2/s^2
		pvtFilter_U.state_noise[2] = Config.Ekf.stateNoiseCovClkBias;		// clk bias m^2
		pvtFilter_U.state_noise[3] = Config.Ekf.stateNoiseCovClkGalGpsBias;	// gal gps clk bias m^2
		pvtFilter_U.state_noise[4] = Config.Ekf.stateNoiseCovClkDrift;		// clk drift m^2/s^2
		pvtFilter_U.meas_noise[0] =	 Config.Ekf.measSigmaPr * Config.Ekf.measSigmaPr;	// PR noise sigma m^2
		pvtFilter_U.meas_noise[1] =	 Config.Ekf.measSigmaDp * Config.Ekf.measSigmaDp;	// doppler noise m^2/s^2
		pvtFilter_U.mu_CB = EARTHMU;
		pvtFilter_U.mahalanobis_threshold = Config.Ekf.mahaThr;
		pvtFilter_U.x_state_init_eci[0] = pvtProcess.lsData.X[0];	// pos x m eci
		pvtFilter_U.x_state_init_eci[1] = pvtProcess.lsData.X[1];	// pos y m eci
		pvtFilter_U.x_state_init_eci[2] = pvtProcess.lsData.X[2];	// pos z m eci
		pvtFilter_U.x_state_init_eci[3] = -pvtProcess.lsData.V[0];	// vel x m/s eci
		pvtFilter_U.x_state_init_eci[4] = -pvtProcess.lsData.V[1];	// vel y m/s eci
		pvtFilter_U.x_state_init_eci[5] = -pvtProcess.lsData.V[2];	// vel z m/s eci
		pvtFilter_U.x_state_init_eci[6] = pvtProcess.lsData.clkData.deltaClkBias*LIGHTSPEED;	// dt clk  m
		pvtFilter_U.x_state_init_eci[7] = pvtProcess.lsData.clkData.deltaClkBiasGal*LIGHTSPEED;	// dt gal gpsclk  m
		pvtFilter_U.x_state_init_eci[8] = pvtProcess.lsData.clkData.clkDrift*LIGHTSPEED;		// dt clk drift m/s
		// init covariance
		pvtFilter_U.p_init[0]  =	Config.Ekf.initStateSigmaPos * Config.Ekf.initStateSigmaPos;	// pos x m^2
		pvtFilter_U.p_init[10]  =	Config.Ekf.initStateSigmaPos * Config.Ekf.initStateSigmaPos;	// pos y m^2
		pvtFilter_U.p_init[20] =	Config.Ekf.initStateSigmaPos * Config.Ekf.initStateSigmaPos;	// pos z m^2
		pvtFilter_U.p_init[30] =	Config.Ekf.initStateSigmaVel * Config.Ekf.initStateSigmaVel;	// vel x m^2/s^2
		pvtFilter_U.p_init[40] =	Config.Ekf.initStateSigmaVel * Config.Ekf.initStateSigmaVel;	// vel y m^2/s^2
		pvtFilter_U.p_init[50] =	Config.Ekf.initStateSigmaVel * Config.Ekf.initStateSigmaVel;	// vel z m^2/s^2
		pvtFilter_U.p_init[60] =	Config.Ekf.initStateSigmaClkBias * Config.Ekf.initStateSigmaClkBias;	// clk bias m^2
		pvtFilter_U.p_init[70] =	Config.Ekf.initStateSigmaClkGalGpsBias * Config.Ekf.initStateSigmaClkGalGpsBias;	// clk gal gps bias m^2
		pvtFilter_U.p_init[80] =	Config.Ekf.initStateSigmaClkDrift * Config.Ekf.initStateSigmaClkDrift;		// clk drift m^2/s^2
		
		if (timeStamp.gpsTime.TOW == start_filter)
		{
			// update outputs for the previous step inputs
			memcpy(&pvtFilter_U.x_priori_t,pvtFilter_U.x_state_init_eci,sizeof(pvtProcess.ekfData.prev_x));
			memcpy(&pvtFilter_U.p_priori_t,pvtFilter_U.p_init,sizeof(pvtProcess.ekfData.prev_cov));
			pvtFilter_U.TOW_in = timeStamp.gpsTime.TOW;		// TOW
		}
		else 
		{
			memcpy(&pvtFilter_U.x_priori_t,&pvtProcess.ekfData.prev_x,sizeof(pvtProcess.ekfData.prev_x));
			memcpy(&pvtFilter_U.p_priori_t,&pvtProcess.ekfData.prev_cov,sizeof(pvtProcess.ekfData.prev_cov));
			pvtFilter_U.TOW_in = pvtProcess.ekfData.prev_tow;		// TOW
		}
		
		pvtFilter_U.TOW_in = timeStamp.gpsTime.TOW;		// TOW

		// set the validity flags and compute GNSS positions
		// Clock bias GPS ans GPST2GST
		double deltaClkBiasPropag_s	=  -pvtFilter_U.x_priori_t[6]/LIGHTSPEED;
		double deltaClkBiasGal_s	=  -pvtFilter_U.x_priori_t[7]/LIGHTSPEED;
			
		double delta_range;
		SSatData	pvt_act;

		// Compute reception time, should be equal to ToW
		double Time_rx		= (double) (timeStamp.measBlock)*MSTOSEC + pvtProcess.lsData.clkData.clkController + deltaClkBiasPropag_s;
		double delta_time_gps	=	0; //(timeStamp.gpsTime.TOW-Time_rx);
		double delta_time_gal	=	-deltaClkBiasGal_s; //(timeStamp.gpsTime.TOW-Time_rx-deltaClkBiasGal_s);

		for (int ii =0; ii<12; ii++)
		{
			pvtFilter_U.meas_valid_flag[ii]		= 0;
			pvtFilter_U.meas_valid_flag[ii+12]	= 0;
		}
			
		for (int ii = 0; ii < 12; ii++)

		{
			if( pvtProcess.lsData.posFlag == 1 && ii<pvtProcess.lsData.NM)
			{						
				pvtFilter_U.meas_valid_flag[ii]		= 1;
				pvtFilter_U.meas_valid_flag[ii+12]	= 1;					
			}
				
			if (pvtProcess.SVN[ii] > 32)
			{
				pvtFilter_U.measurements_const_flag[ii] = 2;
			}
			else
			{
				pvtFilter_U.measurements_const_flag[ii] = 0;
			}
				
			// Compute the position of the GNSS satellites at proper time- according to the TOW and biases TOW - PR/c+dt
			double Time_tx = Time_rx - pvtProcess.ppMeas.PR_ls[pvtProcess.SVN[ii]-1]/LIGHTSPEED 
										+ pvtProcess.SatData[pvtProcess.SVN[ii]-1].dtsv/LIGHTSPEED;
				
			if (pvtProcess.SVN[ii] > 32)
			{
					SatPosGenericSystem(pvtProcess.SVN[ii]-1-GALNAVINDEX, Time_rx, Time_tx, &pvt_act, 0, supportData_P[1].genSysData_P->systemFlag );					
					delta_range= pvtProcess.ppMeas.Dms[pvtProcess.SVN[ii]-1]*(delta_time_gal); //pvtProcess.ppMeas.Dms[pvtProcess.SVN[ii]-1]*(deltaClkBiasPropag+deltaClkBiasGal)/LIGHTSPEED;
			}
			else if (pvtProcess.SVN[ii] > 0 && pvtProcess.SVN[ii] < 32)
			{
					SatPosGenericSystem(pvtProcess.SVN[ii]-1, Time_rx, Time_tx, &pvt_act, 0, supportData_P[0].genSysData_P->systemFlag);
					delta_range = pvtProcess.ppMeas.Dms[pvtProcess.SVN[ii]-1]*(delta_time_gps);
			}
			else
			{
				(void) memset((void *)&pvt_act, 0,sizeof(pvt_act));
			}
				
			pvtFilter_U.GNSS_pos[ii]		=  pvt_act.P[0];
			pvtFilter_U.GNSS_pos[ii + 12]	=  pvt_act.P[1];
			pvtFilter_U.GNSS_pos[ii + 24]	=  pvt_act.P[2];

			pvtFilter_U.GNSS_vel[ii]		= pvt_act.V[0];
			pvtFilter_U.GNSS_vel[ii + 12]	= pvt_act.V[1];
			pvtFilter_U.GNSS_vel[ii + 24]	= pvt_act.V[2];

			pvtFilter_U.GNSS_pos_corr[ii]	= pvt_act.dtsv; // pvtProcess.SatData[pvtProcess.SVN[ii]-1].dtsv; // 
			
			pvtFilter_U.y_meas[ii]			= pvtFilter_U.meas_valid_flag[ii]*(pvtProcess.ppMeas.PR_ls[pvtProcess.SVN[ii]-1]+delta_range);
			pvtFilter_U.y_meas_doppler[ii]	= pvtFilter_U.meas_valid_flag[ii]*(pvtProcess.ppMeas.Dms[pvtProcess.SVN[ii]-1]);							
		}

		// solve EKF 
		pvtFilter_step(		&pvtFilter_B, 
					&pvtFilter_U, 
					&pvtFilter_Y);
                
                pvtProcess.ekfData.solFlag = 1;

		// update outputs for the previous step inputs
		memcpy(&pvtProcess.ekfData.prev_x,&pvtFilter_Y.x_priori_t_plus_dt,sizeof(pvtProcess.ekfData.prev_x));
		memcpy(&pvtProcess.ekfData.prev_cov,&pvtFilter_Y.p_priori_t_plus_dt,sizeof(pvtProcess.ekfData.prev_cov));
		pvtProcess.ekfData.prev_tow = pvtFilter_Y.tow_priori_t_plus_dt;
		memcpy(&pvtProcess.ekfData.x,&pvtFilter_Y.x_est,sizeof(pvtProcess.ekfData.x));
		memcpy(&pvtProcess.ekfData.cov,&pvtFilter_Y.P_est,sizeof(pvtProcess.ekfData.cov));		
	}

	/*************************************************************************
	  UPDATE MEASUREMENT TIME (iteration when next measurement is programmed)
	 *************************************************************************/
	computeNextMeasTime(&supportData_P[0]);

	/*******************
      PRINT  TRACE INFO
	 *******************/
#ifndef PCMonitor
	printOutputDataLine(&pvtProcess);
#endif
	/********************************/
	/* Reset number of measurements */
	/********************************/
	int k;
	for(k = 0; k < NSYSTEMS; k++)
	{
		supportData_P[k].MeasProcess_P->NM = 0;
	}

	/***********************
	 CHECK CLOCK VALIDITY
	 ***********************/

	// Check whether clock estimate is still valid (not too old) for Virtual-Tracking assistance.
	if (Config.Mng.enableVtFlag == 1)
	{
		checkClockValidityForVt(&supportData_P[0]);
		checkClockValidityForVt(&supportData_P[1]);
	}

    /*******************************************************************************
     RECOMPUTE SATELLITE VISIBILITY (if time for it and rough position is available)
     *******************************************************************************/
    for (int ii = 0;ii<NSYSTEMS;ii++)
    {
        if ((supportData_P[ii].satManag_P->lastVisibComp >= Config.Aid.lastVisibCompTh) &&
            (aidingData.pvtAcqStatus > 0) )
        {
            // Reset counter (for next visibility check)
            supportData_P[ii].satManag_P->lastVisibComp = 1;
        }
        else
        {
            supportData_P[ii].satManag_P->lastVisibComp++;
        }
    }

    return;
}

/***************************************************************************************/
/***************************************************************************************/
/*																					   */
/* void pvtManagement(T_supportData *supportDataGps_P,T_supportData *supportDataGal_P) */
/*																					   */
/*	High-level management of the tasks related to computation of PVT				   */
/*	(and auxiliar processes)														   */
/*	It takes place at the desired output rate (typically once per second).			   */
/*																					   */
/***************************************************************************************/
/***************************************************************************************/
static void pvtManagement(T_supportData *supportData_P)
{
	int svnIndex   = 0,
		timeOfDay  = 0,
		epochIndex = 0;

	double isBias = 0;

	/*******************************************************************************************/
	/* IF ASSISTANCE BASED ON BRDC EPHEMERIS FILE, FIND THE PROPER INDEX FROM THE EPHEM TABLE, */
	/* ACCORDING TO THE CURRENT TOW.															  */
	/*******************************************************************************************/
	if(Config.Mng.startMode == 2 || Config.Mng.startMode == 5 || Config.Mng.startMode == NOTPOS_EPH)
	{
		if (supportData_P[0].genSysData_P->systemFlag == 0) // GPS
		{
			timeOfDay  = ((int)(timeStamp.gpsTime.TOW + 7200) % 86400);
			epochIndex = (int)(((double)timeOfDay)/7200.0);

			memcpy(&aidingData.gpsEph,    &aidingData.gpsEphME[epochIndex-1], sizeof(aidingData.gpsEph)); // epoch index was wrongly defined (added -1 to solve in the particular case tested)
			memcpy(&aidingData.gpsAlmEph, &aidingData.gpsEphME[epochIndex-1], sizeof(aidingData.gpsEph));

			memcpy(&aidingData.galEph,    &aidingData.galEphME[0], sizeof(aidingData.galEph));
			memcpy(&aidingData.galAlmEph, &aidingData.galEphME[0], sizeof(aidingData.galEph));
		}
	}

	// Default: posFlag = FALSE; if position computed at this epoch, this will turn to TRUE inside ComputePVT.
	pvtProcess.lsData.posFlag = FALSE;

        memcpy(&pvtProcess.lsDataPrev, &pvtProcess.lsData, sizeof(pvtProcess.lsData));
	/*******************/
	/* PVT computation */
	/*******************/
	pvt_computation(&pvtProcess);

	/************************/
	/* IF POSITION COMPUTED */
	/************************/
	if( pvtProcess.lsData.posFlag == TRUE)
	{
		/*****************************************************************************/
		/* IF POSITION DEEMED ACCURATE FOR VT PURPOSES (IN TERMS OF HEIGTH AND HDoP) */
		/* ALLOW VIRTUAL TRACKING													 */
		/*****************************************************************************/
		if (pvtProcess.lsData.DOP.HDoP < 10)
		{
			/******************************************************************************/
			/* ASSIGN COMPUTED PVT TO AIDING STRUCTURE FOR VIRTUAL-TRACKING REACQUISITION */
			/******************************************************************************/
			memcpy(&aidingData.vtData.hotX,&pvtProcess.lsData.X,sizeof(pvtProcess.lsData.X));
			memcpy(&aidingData.vtData.hotV,&pvtProcess.lsData.V,sizeof(pvtProcess.lsData.V));
			memcpy(&aidingData.vtData.clkData,&pvtProcess.lsData.clkData,sizeof(pvtProcess.lsData.clkData));

			// Virtual tracking possible (position + clock available):
			// show it in pvtAvqStatus ( = 3) and set clock as VALID.

			aidingData.pvtAcqStatus = 3;
			// update rxProcessing PVT status
			rxProcessingStatus.aidingStatus = 3;
			rxProcessingStatus.pvtThread.pvtStatus = 1;
			rxProcessingStatus.pvtThread.pvtTime = ComData.counterMs;
			rxProcessingStatus.pvtThread.svnProcessedNr = pvtProcess.ppMeas.NM_ls;
			memcpy(&rxProcessingStatus.pvtThread.svnProcessedList,&pvtProcess.ppMeas.SVN_ls,sizeof(pvtProcess.ppMeas.SVN_ls));

			aidingData.vtData.clkData.hotClkBiasStatus = VALID_STAT;

			// Store most recent time when position was computed (for virtual-tracking purposes).
			aidingData.vtData.pvtHotMs =  ComData.counterMs;

			/***************************************************************************************************/
			/* At due time, execute "second part" of assistance process  -->								   */
			/*			review all satellites to change their acquisition mode to virtual time (when possible) */
			/*			manage potential change of startMode, now when position is known					   */
			/***************************************************************************************************/
			if(supportData_P[0].MeasProcess_P->assistanceStatusFlag == 1 && Config.Mng.startMode > WARM_SP3 && Config.Mng.startMode < NOTPOS_EPH)
			{
				ProcessAssistancePostposition(&supportData_P[0], &chDataAll);
			}

            if (Config.Mng.startMode == NOTPOS_EPH)
            {
                Config.Mng.startMode = ASSISTED_EPH;
                supportData_P[0].MeasProcess_P->assistanceStatusFlag = 1;
                supportData_P[1].MeasProcess_P->assistanceStatusFlag = 1;
            }
            else if (Config.Mng.startMode == NOTPOS_SP3)
            {
                Config.Mng.startMode = ASSISTED_SP3;
                supportData_P[0].MeasProcess_P->assistanceStatusFlag = 1;
                supportData_P[1].MeasProcess_P->assistanceStatusFlag = 1;
            } // if startMode == NOTPOS_EPH

			/*****************************************************************************/
			/* MANAGE CLOCK-STEERING TASKS BASED ON ESTIMATED CLOCK BIAS (IF CONFIGURED) */
			/*****************************************************************************/
			if (Config.Pvt.clkSteeringFlag == 1)
			{
				// Update clock bias.
				pvtProcess.lsData.clkData.clkController  = pvtProcess.lsData.clkData.clkController - pvtProcess.lsData.clkData.deltaClkBias;
			}
			else
			{
				// Update clock bias with computed ISB.
				pvtProcess.lsData.clkData.clkController  = pvtProcess.lsData.clkData.clkController - isBias;
			}

		} // if (pvtProcess.lsData.DOP.HDoP < 10)
		else if (pvtProcess.lsData.DOP.HDoP >= 10)
		{
			rxProcessingStatus.pvtThread.pvtStatus = 2;
			rxProcessingStatus.pvtThread.pvtTime = ComData.counterMs;
			rxProcessingStatus.pvtThread.svnProcessedNr = pvtProcess.ppMeas.NM_ls;
			memcpy(&rxProcessingStatus.pvtThread.svnProcessedList,&pvtProcess.ppMeas.SVN_ls,sizeof(pvtProcess.ppMeas.SVN_ls));
		}

		if (pvtProcess.firstGalPvtFlag == 0 && pvtProcess.lsData.clkData.deltaClkBiasGal != 0)
		{
			pvtProcess.lsData.clkData.clkControllerGal -= pvtProcess.lsData.clkData.deltaClkBiasGal;
			pvtProcess.firstGalPvtFlag					= 1;
		}

		/*****************************************************************************/
		/* THIS IS EXECUTED ONLY THE FIRST TIME WHEN REASONABLE POSITION IS COMPUTED */
		/*****************************************************************************/
		if (pvtProcess.firstPvtFlag == FALSE)
		{
			pvtProcess.firstPvtFlag = TRUE;
                        pvtProcess.firstPvtTow = pvtProcess.gpsTime.TOW;

			/******************************************************************************************/
			// Set CP to zero for all sats in this first epoch. Trick: just to force that CP ambiguity
			// is recomputed next iteration, to make it smaller.
			/******************************************************************************************/
			for (svnIndex = 0; svnIndex <  MAXNUMSAT; svnIndex++)
			{
				MeasProcessComb.Meas[svnIndex].validCP = 0;
			}

			// Even if clock steering is disabled (i.e. free running configured), correct clock bias
			// after this initial PVT computation, to make the clock bias more reasonable.
			// Afterwards, at later epochs, clock bias will be left freely evolving.
			if (Config.Pvt.clkSteeringFlag == 0)
			{
				// Update clock bias.
				pvtProcess.lsData.clkData.clkController  = pvtProcess.lsData.clkData.clkController - pvtProcess.lsData.clkData.deltaClkBias;
			}

			pvtProcess.lsData.clkData.clkController0 = pvtProcess.lsData.clkData.clkController;

		} // from: if (pvtProcess.firstPvtFlag == FALSE)

		/**************************************************************************/
		// Store position and velocity for warm acquisition (FFT-Acq based, not VT,
		// since it may be not sufficiently accurate, due to large DoP).
		/**************************************************************************/

		memcpy(&aidingData.refPosWarm.X,&pvtProcess.lsData.X,sizeof(pvtProcess.lsData.X)); // Cartesian position
		memcpy(&aidingData.refPosWarm.V,&pvtProcess.lsData.V,sizeof(pvtProcess.lsData.V)); // Cartesian velocity
			
		// Store also in Geodetic format
		ECI2ECEF(aidingData.refPosWarm.X, aidingData.refPosWarm.V, timeStamp.gpsTime.TOW, timeStamp.gpsTime.WN, timeStamp.gpsTime.WN0, aidingData.refPosWarm.X_WGS84, aidingData.refPosWarm.V_WGS84);
		WGSToGeo(aidingData.refPosWarm.X_WGS84,aidingData.refPosWarm.G);

		// Store reference time.
		aidingData.refTime.TOW = (int)timeStamp.gpsTime.TOW;

		aidingData.vtData.clkData.clkMsWarm = ComData.counterMs;

	} // from: if (pvtProcess.lsData.posFlag == TRUE)
	else if ((supportData_P[0].MeasProcess_P->NM > 0) && (aidingData.pvtAcqStatus == 3) &&
			(ComData.counterMs - aidingData.vtData.pvtHotMs) < 20000)
	{
		clkEstimate(&pvtProcess.lsData.clkData, timeStamp.gpsTime.TOW, &supportData_P[0], supportData_P[0].MeasProcess_P);
		memcpy(&aidingData.vtData.clkData,&pvtProcess.lsData.clkData,sizeof(pvtProcess.lsData.clkData));

		if (Config.Pvt.clkSteeringFlag == 1)
		{
			// Update clock bias.
			pvtProcess.lsData.clkData.clkController  = pvtProcess.lsData.clkData.clkController - pvtProcess.lsData.clkData.deltaClkBias;
		}
		else
		{
			// Update clock bias with computed ISB.
			pvtProcess.lsData.clkData.clkController  = pvtProcess.lsData.clkData.clkController - isBias;
		}

		aidingData.vtData.clkData.clkMsWarm = ComData.counterMs;


	} // if (pvtProcess.lsData.posFlag == TRUE)
	
	if (pvtProcess.lsData.posFlag != TRUE)
	{
		rxProcessingStatus.pvtThread.pvtStatus = 0;
		rxProcessingStatus.pvtThread.pvtTime = -1;
		rxProcessingStatus.pvtThread.svnProcessedNr = -1;
	}

// 	if (pvtProcess.lsData.posFlag > 0 || Config.Aid.refPos[0] != 0)
// 	{
// 		int k;
// 		for(k = 0; k < NSYSTEMS; k++)
// 		{
// 			CheckVisibilityAllSlots(&supportData_P[k], &chDataAll);
// 		}
// 	}

	return;
}





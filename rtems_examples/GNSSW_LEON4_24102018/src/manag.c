#include <manag.h>
#include <rtems/score/thread.h>
#include <interface.h>
#include <spwinterface.h>

#include "siria.h"
#include <configuration.h>
#include <dataBuffer.h>
#include <init.h>

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
extern int nospw; 
extern grspw_device devs[DEVS_MAX];
extern struct grspw_config dev_def_cfg;
extern int dma_process(grspw_device *dev);

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

        grspw_device *dev;
        
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

    #ifdef PROFILER
        MainLoop_StartTime = GetStartTime();
    #endif  
        
        if(Config.Com.onspw == 1) // Read samples from SpW
        {           
            for (int idx2 = 0; idx2 < nospw; idx2++) 
            {
                    dev = &devs[idx2];
                    if (dev->dh == NULL)
                            continue;

                    dma_process(dev);
            }
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
        rc = RecvUdp();
    #endif
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

                        if(ioBuffer.inIndexBytes == nBytesToRead)//100000)
                        {
                            ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                            rec_ok = 1;
                            executionStatus = EXECUTING;
                        }
                        else if(ioBuffer.inIndexBytes == nBytesToRead*2)//200000)
                        {	
                            ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
                            ioBuffer.inIndexBytes = 0;
                            rec_ok = 1;
                            executionStatus = EXECUTING;
                        }
                    }
                    else if(rc == 1)
                    {
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
                            PrepSendBuffer();
                            SendBuffer();
                        #endif                    
                    } 
                }
                
/* setup Tracking if acq new satelite */  
                for(int core_index1 = 0; core_index1 < NACQCHAN; core_index1++)		
                {         
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

            if(core[2].activity == TCK)
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
            }
      
                for (int idx7 = 0; idx7 < primaryIterations; idx7++)
                {                                                          
                    int cpt = ComData.counterMs%4;
                                                            
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
                    /* just for now, after add gnss .lib change to: if(abs(ComData.counterMs-timeStamp.measBlock-100)<100) */
                        if(ComData.counterMs%1000 == 0)   
                        //if(abs(ComData.counterMs-timeStamp.measBlock-100)<100)                       
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
                                PrepSendBuffer();
                                SendBuffer();
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

}



/* Must be deleted after add gnss .lib file and */
int setUpTracking(T_supportData *supportData_P, T_ChData *chData_P, int core_index)
{
    
}

void ManageVtAcq(T_supportData *supportData_P, T_ChData *chData_P, S_AcqTckAid *AcqTckAid)
{
    
}

void resetTckChannel_core(T_Channel *chanAux)
{
    
}

void AcquisitionTrackingAid(T_supportData *supportData_P, T_ChData *chData_P, T_AidData *aidData, S_AcqTckAid *AcqTckAid)
{
    
}

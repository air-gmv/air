#include <configuration.h>
//#include <trace.h>

#include <dataBuffer.h>
#include <threads.h>

//#include <tcpsock.h>

#include <init.h>
//#include <date.h>

#include <interface.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************

//*****************************************************************************
//                        GLOBAL EXTERNAL VARIABLES
//*****************************************************************************
extern T_ComData			ComData;
extern T_ChData				chDataAll;
extern T_Config				Config;
extern S_PVT_Process		pvtProcess;
extern T_DAid				aidingData;
extern S_TimeStamp			timeStamp;
extern char buffer[MAX_BUFFER_SIZE]; 
extern T_GenericSystemData	genSysData [NSYSTEMS];
extern T_SatManager			satManag   [NSYSTEMS];
extern S_MeasProcess		MeasProcess[NSYSTEMS];
extern T_supportData		supportData[NSYSTEMS];
extern int almanacIndexBuffer;
extern subframeAuxStructure subframeAux[MAXNUMSAT];


extern S_gnssProcessingStatus	gnssProcessingStatus;
extern S_rxProcessingStatus		rxProcessingStatus;

//*****************************************************************************
//                        MAIN VARIABLES
//*****************************************************************************

//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
static int  initializeChannelData(T_ChData *chData_P);
static void initializeGenSysData (T_GenericSystemData *genSysData_P, T_supportData *supportData_P, int galE1flag);
static int  initializeSupportData(T_supportData *supportData_P, char main_system);
static void initializeComData();
static void finalizeChannelData();
static int  initializeAssistance(T_supportData *supportData_P, T_SatManager *satManag_P, T_ChData *chData_P, char main_system);
static void initializeSatManag(T_supportData *supportData_P);
// void initialize_queue_list(T_ListSat *queue);
void initialize_acq_list(T_ListSat *queue);

//*****************************************************************************/
//                        PUBLIC FUNCTION BODIES
//*****************************************************************************/

/*****************************************/
/*										 */
/*	int initialize(char *configFileName) */
/*		 								 */
/*****************************************/
int initialize(char *configFileName)
{
    	// Fll accumulation table signs
	
	int sign = 1;
	double diffs = 0;
	for(int jj = 0; jj < 6; jj++)
	{
		int kk = 1;
		for(int ii = 0; ii < 32; ii++)
		{
			diffs = pow((double) 2,jj);

			rxProcessingStatus.FllAccumSigns[ii][jj] = sign;

			if (kk == 32/diffs)
			{
				sign = -sign;
				kk = 0;
			}
			kk += 1;
		}
	}
        
	// init GPS week number to 0
	timeStamp.gpsTime.WN0 = -1;

	// Initialize Queues
	initialize_queue_list(&rxProcessingStatus.vtQueueGps);
	initialize_queue_list(&rxProcessingStatus.vtQueueGal);
	initialize_acq_list(&rxProcessingStatus.acqQueue);

	//Initialize rxProcessingStatus
	if( Config.Com.systemFlag == 2)
	{
		rxProcessingStatus.acqGalFlag = 1;
	}
	else
	{
		rxProcessingStatus.acqGalFlag = 0;
	}
	rxProcessingStatus.secondFft = 0;
	rxProcessingStatus.acqInitFlag = 0;
	rxProcessingStatus.aidingStatus = 0;
	rxProcessingStatus.acqSatNr = 0;
	rxProcessingStatus.acqThreadIndicator = 0;
	rxProcessingStatus.vtChannelNr = 0;

	for(int ii = 0; ii < 3 * MAXNUMSAT; ii++)
	{
		rxProcessingStatus.newFileFlag[ii] = 1;
		rxProcessingStatus.pvtThread.svnProcessedList[ii] = 0;
	}
	rxProcessingStatus.pvtThread.pvtStatus = -1;
	rxProcessingStatus.pvtThread.pvtTime = -1;
	rxProcessingStatus.pvtThread.svnProcessedNr = -1;

	rxProcessingStatus.tckThreadIndicator = 0;
	int canal_nr = 0;
	for(int ii = 0; ii < 3; ii++)
	{		
		rxProcessingStatus.acqThreadStatus[ii].acqStatus = -1;
		rxProcessingStatus.acqThreadStatus[ii].svnInProcess = -1;
		rxProcessingStatus.acqThreadStatus[ii].fftAcqFlag = 1;
		rxProcessingStatus.vtAcqStatus[ii].vtStatus = 0;
		rxProcessingStatus.vtAcqStatus[ii].vtAcqFlag = 0;
		rxProcessingStatus.tckThread[ii].tckThreadStatus = -1;
		rxProcessingStatus.tckThread[ii].tckThreadStatus = 0;
		for(int kk = 0; kk < 3; kk++)
		{	
			canal_nr = canal_nr+1;
			rxProcessingStatus.tckThread[ii].tckChannelNr[kk] = canal_nr;
			rxProcessingStatus.tckThread[ii].tckChannelStatus[kk] = 0;
			rxProcessingStatus.tckThread[ii].svnInProcess[kk] = -1;
		}
	}

	for(int ii = 0; ii < MAXNUMCHAN; ii++)
	{
		rxProcessingStatus.vtAcqStatus[ii].vtStatus = 0;
		rxProcessingStatus.vtAcqStatus[ii].vtAcqFlag = 0;
	}

	//Initialize gnssProcessingStatus
	for(int ii = 0; ii < MAXNUMSAT; ii++)
	{
		if(ii<MAXNUMGPS)
		{
			gnssProcessingStatus.gpsProcessingStatus[ii].svn		=  ii+1;
			gnssProcessingStatus.gpsProcessingStatus[ii].acqStatus		=  0;
			gnssProcessingStatus.gpsProcessingStatus[ii].tckStatus		=  0;
			gnssProcessingStatus.gpsProcessingStatus[ii].AlmEphStatus	=  0;
			gnssProcessingStatus.gpsProcessingStatus[ii].satInView		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].satHealthy		= -1;				
			gnssProcessingStatus.gpsProcessingStatus[ii].vtStatus		=  0;			
			gnssProcessingStatus.gpsProcessingStatus[ii].vtPossible		=  0;
			gnssProcessingStatus.gpsProcessingStatus[ii].elevation		= -1;
                        gnssProcessingStatus.gpsProcessingStatus[ii].acqMs		= -1;					
			gnssProcessingStatus.gpsProcessingStatus[ii].acqDoppler		= -1;						
			gnssProcessingStatus.gpsProcessingStatus[ii].acqSampleCode	= -1;			
			gnssProcessingStatus.gpsProcessingStatus[ii].tckMs		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].acqDoppler		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].tckDoppler		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].acqSampleCode	= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].tckSampleCode	= -1;
                        gnssProcessingStatus.gpsProcessingStatus[ii].vtMs		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].vtDoppler		= -1;
			gnssProcessingStatus.gpsProcessingStatus[ii].vtSampleCode	= -1;
		
		}
		if(ii<MAXNUMGAL)
		{
			gnssProcessingStatus.galProcessingStatus[ii].svn		=  ii+1;
			gnssProcessingStatus.galProcessingStatus[ii].acqStatus		=  0;
			gnssProcessingStatus.galProcessingStatus[ii].tckStatus		=  0;
			gnssProcessingStatus.galProcessingStatus[ii].AlmEphStatus	=  0;
			gnssProcessingStatus.galProcessingStatus[ii].satInView		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].satHealthy		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].vtStatus		=  0;
			gnssProcessingStatus.galProcessingStatus[ii].vtPossible		=  0;
			gnssProcessingStatus.galProcessingStatus[ii].elevation		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].acqMs		= -1;				
			gnssProcessingStatus.galProcessingStatus[ii].acqDoppler		= -1;			
			gnssProcessingStatus.galProcessingStatus[ii].acqSampleCode	= -1;			
			gnssProcessingStatus.galProcessingStatus[ii].tckMs		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].tckDoppler		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].tckSampleCode	= -1;
			gnssProcessingStatus.galProcessingStatus[ii].vtMs		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].vtDoppler		= -1;
			gnssProcessingStatus.galProcessingStatus[ii].vtSampleCode	= -1;	
		}
	}

	int ret = EXIT_OK; // Function return value.

	int i, k;

	// Read configuration file and initialize configuration structures.
	if (CNFMain(configFileName) != EXIT_OK)
	{
		ret = EXIT_ER;
	}
	else
	{
       // printConfiguration();
		/***************************/
		/* Initialize channel data */
		/***************************/
		if (initializeChannelData(&chDataAll) != EXIT_OK)
		{
			ret = EXIT_ER;
		}

		for(k = 0; k < NSYSTEMS; k++)
		{ 
			for(i = 0; i < MAXNUMSAT; i++)
			{
				satManag[k].svi2ch   [i]                   = -1;
				satManag[k].sloti2ch [i]                   = -1;
				satManag[k].acqFftSat[i].lastGoodFreq.freq =  0;
				satManag[k].acqFftSat[i].lastGoodFreq.ms   = -100000;
				satManag[k].acqFftSat[i].lastGoodFreq.CLI  =  0;
				satManag[k].acqFftSat[i].elev              = -100;
			}

			supportData[k].genSysData_P  = &genSysData [k];
			supportData[k].satManag_P	 = &satManag   [k];
			supportData[k].MeasProcess_P = &MeasProcess[k];

			// Initialize tables and support data (GPS/GLONASS/Galileo).
			initializeGenSysData(&genSysData[k], &supportData[k], (k != 0 || Config.Com.systemFlag == 2));
			initializeSupportData(&supportData[k], (k == 0));
		}   
          	
printf("> initializeComData \n");
		// Initialize common data
		initializeComData();

		/****************************/
		/* Initialize sample buffer */
		/****************************/
		if (initializeDataBuffer() != EXIT_OK)
		{
            printf("> initializeDataBuffer ERROR \n");
			ret = EXIT_ER;
		}

		for(k = 0; k < NSYSTEMS; k++)
		{
			genSysData[k].auxThCorr	= ComData.lengthSamples - NBITS_PER_WORD * genSysData[k].nWordsPerBlock - 1;
                       // printf("\n\n\n auxThCorr: %d, length samples: %d, nwordsperblock: %d \n\n\n\n\n",
                       //                                               genSysData[k].auxThCorr,ComData.lengthSamples, genSysData[k].nWordsPerBlock);
                        
		}
                
#ifdef LOG_PRINT                
		if(Config.Com.systemFlag == 0)
		{
			printf("\t\tRECEIVER INITIALIZED IN GPS MODE: START PROCESSING [STARTMODE=%d]\n", Config.Mng.startMode);
		}
		else if(Config.Com.systemFlag == 2)
		{
			printf("\t\tRECEIVER INITIALIZED IN GALILEO MODE: START PROCESSING [STARTMODE=%d]\n", Config.Mng.startMode);
		}
		else if(Config.Com.systemFlag == -1)
		{
			printf("\t\tRECEIVER INITIALIZED IN GPS+GALILEO MODE: START PROCESSING [STARTMODE=%d]\n", Config.Mng.startMode);
		}
#endif                
	}
	return(ret);
}

/*************************************************/
/*												 */
/* int initializeChannelData(T_ChData *chData_P) */
/*												 */
/*************************************************/
static int initializeChannelData(T_ChData *chData_P)
{
	int i, ret = EXIT_OK;

	// Creating channels structures.
#ifdef MEMORY_ALLOCATION   
 	if ((chData_P->Ch = (T_Channel *) calloc( Config.Mng.nChannels, sizeof(T_Channel))) == NULL)
#else        
    memset(chData_P->Ch, 0, sizeof(chData_P->Ch));
#endif  
    
#ifdef MEMORY_ALLOCATION    
	{
 		ret = EXIT_ER;
 	}
// 	// Initialize channel data.
 	else
 	{
#endif        
		// Initializing channels structures.
		for(i = 0; (i < Config.Mng.nChannels) && (ret == EXIT_OK); i++)
		{
			// TRACKING channel parameters.
			// Local code structure.
			chData_P->Ch[i].corrAuxIndex.codeIndex = -1;

			// Local carrier structure.
			chData_P->Ch[i].corrAuxIndex.carrierFreq = -1;
            
            // Local code structure.
			chData_P->Ch[i].core_index = -1;

			// LOCK detection channel parameters.
			chData_P->Ch[i].Lck.cntLck		= 0;
			chData_P->Ch[i].Lck.NPk			= 0;
			chData_P->Ch[i].Lck.cliVectork	= 0;

			chData_P->Ch[i].chStatus = -1;

			// Initialize sync-decod data.
			initializeSynchrodecod(&chData_P->Ch[i]);

			chData_P->Ch[i].chIndex   =  i;
			chData_P->Ch[i].SVN	      = -1;
			chData_P->Ch[i].slot	  = -1;
			chData_P->Ch[i].slotIndex = -1;
			chData_P->Ch[i].svnIndex  = -1;
		}
#ifdef MEMORY_ALLOCATION		
 	}
#endif
	chData_P->freeChannels.N = Config.Mng.nChannels;

	for (i = 0; i < Config.Mng.nChannels; i++)
	{
		chData_P->freeChannels.chList[i]    = i;
		chData_P->freeChannels.listIndex[i] = i;
	}
	chData_P->freeChannels.i = 0;

	return(ret);
}

/***********************************/
/*								   */
/* void initializeGenSysData(void) */
/*								   */
/***********************************/
static void initializeGenSysData(T_GenericSystemData *genSysData_P, T_supportData *supportData_P, int galE1flag)
{
	int i = 0, svnIndex = 0;

	double iFreqAfterConversion = Config.Com.ifreq;

	// Set to zero (initial cleaning) genSysData structure
	memset(genSysData_P, 0, sizeof(T_GenericSystemData));

	/* GPS/Galileo/GPS+Galileo */

	genSysData_P->nSlots = (galE1flag ? MAXNUMGAL : MAXNUMGPS);
	genSysData_P->nPrn   = (galE1flag ? MAXNUMGAL : MAXNUMGPS);
	genSysData_P->nSv    = (galE1flag ? MAXNUMGAL : MAXNUMGPS);

	genSysData_P->nFreq					  = 1;
	genSysData_P->deltaIndexSp3			  = 0;
	genSysData_P->leapSeconds			  = 0;
	genSysData_P->deltaIndexSp3			  = DELTAINDEXSP3GAL*galE1flag;
	genSysData_P->deltaSlotIndexPerSystem =-1;		/* slotIndex = slot + deltaSlotIndexPerSystem */

	genSysData_P->codePeriod   = (galE1flag ? 0.004 : 0.001);						// s
	genSysData_P->chipRate     = (galE1flag ?  2046 :  1023)*1e3;					// chips/s
	genSysData_P->chipsPerCode = genSysData_P->codePeriod * genSysData_P->chipRate;	// chips
        
        genSysData_P->maxNonCohInt = (galE1flag ? Config.FftAcq.maxNonCohInt_GAL : Config.FftAcq.maxNonCohInt_GPS);
        
//	genSysData_P->maxNonCohInt = genSysData_P->maxNonCohInt / (genSysData_P->codePeriod / 0.001);
//
//	if (genSysData_P->maxNonCohInt == 0)
//	{
//		genSysData_P->maxNonCohInt = 1;
//	}

	genSysData_P->systemFlag		    = 2*galE1flag;
                
	genSysData_P->nPrimaryIterations	= 100;//(galE1flag ? 1 : 4); //100;
        //genSysData_P->nPrimaryIterations	= (galE1flag ? 1 : 4);

	for (i = 0; i < genSysData_P->nSlots; i++)
	{
		genSysData_P->freqPerSv[i]		   = 1575420000.0;			// Central frequency for each channel (Hz)
		genSysData_P->deltaFreqPerSv[i]	   = 0;						// Frequency relative to central channel (0)
		genSysData_P->ifreqEquiv[i]		   = iFreqAfterConversion;
		genSysData_P->codeCarrierFactor[i] = genSysData_P->freqPerSv[i] / genSysData_P->chipRate;
		genSysData_P->waveLength[i]		   = LIGHTSPEED / genSysData_P->freqPerSv[i];
		genSysData_P->sloti2sviCur[i]	   = i;
		genSysData_P->sloti2sviAll[i][0]   = i;
		genSysData_P->sloti2sviAll[i][1]   = i;
	}

	for (svnIndex = 0; svnIndex < genSysData_P->nSv; svnIndex++)
	{
		genSysData_P->svi2sloti[svnIndex] = svnIndex;
	}

	genSysData_P->secondsPerChip		 = 1.0 / genSysData_P->chipRate;						// s/chip
	genSysData_P->codeLength			 = LIGHTSPEED * genSysData_P->codePeriod;				// meters
	genSysData_P->chipLength			 = LIGHTSPEED / genSysData_P->chipRate;				// meters
	genSysData_P->codePerOverRate		 = genSysData_P->codePeriod / genSysData_P->chipRate;		// chips^(-1)
	genSysData_P->nSamples				 = (int)(Config.Com.sampFreq * genSysData_P->codePeriod);
	genSysData_P->chipsPerSample		 = (1.0/(genSysData_P->secondsPerChip * Config.Com.sampFreq));
	genSysData_P->samplesPerChip		 = (double)((double)genSysData_P->nSamples) / genSysData_P->chipsPerCode;
	genSysData_P->nSamplesDecimatedHalf      = genSysData_P->nSamples / Config.Tck.decimateFactor / 2;

	// Set length of correlation loops.
	genSysData_P->nWordsPerBlock		 = ((int) ceil(genSysData_P->nSamples / (float) NBITS_PER_WORD)) / Config.Tck.decimateFactor;
	genSysData_P->auxThCorr			 = ComData.lengthSamples - NBITS_PER_WORD * genSysData_P->nWordsPerBlock - 1;
	genSysData_P->nWordsPerBlock0		 = (int) ceil(genSysData_P->nSamples / (float) NBITS_PER_WORD);

	genSysData_P->fractCodeThPlus		 = genSysData_P->chipRate / Config.Com.sampFreq / 2;
	genSysData_P->fractCodeThMinus		 =-genSysData_P->fractCodeThPlus;

	genSysData_P->freqStep				 = Config.FftAcq.freqStepCodeRatio / (float)genSysData_P->codePeriod;

	/****************************************/
	/* TIME BETWEEN EPOCHS FROM OUTPUT RATE */
	/****************************************/
	genSysData_P->blocksBtwEpochs = 1/(Config.Com.outputRate * genSysData_P->codePeriod);
        //printf("%f %f %f\n",Config.Com.outputRate, genSysData_P->codePeriod, supportData_P->genSysData_P->blocksBtwEpochs);
	// Compute size of ms of data in bit index format.
	supportData_P->outSize.bit  = genSysData_P->nSamples % NBITS_PER_WORD;
	supportData_P->outSize.word = genSysData_P->nSamples / NBITS_PER_WORD;
        //printf("outSize.word %d genSysData_P->nSamples %d \n", supportData_P->outSize.word, genSysData_P->nSamples);
}

/************************************************************************************/
/*																					*/
/* static int initializeSupportData(T_supportData *supportData_P, char main_system) */
/*																					*/
/************************************************************************************/
static int initializeSupportData(T_supportData *supportData_P, char main_system)
{
	int ret = EXIT_OK;

#if SYSFLAG == -1
	Bpsk2Boc11(genSysData[(Config.Com.systemFlag == -1)].nSlots);
#elif SYSFLAG == 2
	Bpsk2Boc11(genSysData[0].nSlots);
#endif
	printf("> initializeAcquisition \n");
	/*********************************/
	/* Initialize FFT-Acq structures */
	/*********************************/

	if (initializeAcquisition(supportData_P) != EXIT_OK)
	{
		ret = EXIT_ER;
	}

	/**********************************/
	/* Initialize tracking structures */
	/**********************************/
printf("> initializeTracking \n");
	if (initializeTracking(supportData_P, &chDataAll) != EXIT_OK)
	{
		ret = EXIT_ER;
	}
printf("> initializeSatManag  \n");
	/**********************************/
	/* Initialize satManag structures */
	/**********************************/

	initializeSatManag(supportData_P);

	/*************************/
	/* Initialize assistance */
	/*************************/
printf("> initializeAssistance  \n");
	if (initializeAssistance(supportData_P, supportData_P->satManag_P, &chDataAll, main_system) != EXIT_OK)
	{
		ret = EXIT_ER;
	}

	return ret;
}

/****************************/
/*							*/
/* void initializeComData() */
/*						    */
/****************************/
static void initializeComData()
{
	// Set length of execution, in ms (based on configuration, in seconds)
	ComData.executionLengthMs = Config.Com.simTime * 1000;

	if (ComData.executionLengthMs == 0)
	{
		ComData.executionLengthMs = -1;
	}
      
        if(Config.Com.systemFlag == 2)
        {
            ComData.nSamples          = (int) floor(Config.Com.sampFreq * 0.001 * genSysData[0].nPrimaryIterations);
        }
        else
        {
            ComData.nSamples          = (int) floor(Config.Com.sampFreq * genSysData[0].codePeriod * genSysData[0].nPrimaryIterations);
        }
        
        // printf("nSamples %d sampFreq %f codePeriod %f nPrimaryIterations %d\n",ComData.nSamples, Config.Com.sampFreq, genSysData[0].codePeriod, genSysData[0].nPrimaryIterations);
        
	ComData.metersPerSample   = LIGHTSPEED / Config.Com.sampFreq;
	ComData.ts                = (double)(1/Config.Com.sampFreq);

	ComData.cnoThNatural      = pow(10,Config.Lck.cnoTh/10.0);
	ComData.vtCnoThNatural    = pow(10,Config.Lck.vtCnoTh/10.0);
	ComData.lossCnoThNatural  = pow(10,Config.Lck.lossCnoTh/10.0);

	ComData.vtMsResetTh = Config.Tck.nDll * Config.Lck.vtNLock;

	// Compute lozow threshold Factor
	Config.FftAcq.pfa = -(log(2 * Config.FftAcq.pfa));

	ComData.sampFreqIQ2 = Config.Com.sampFreq;
	ComData.nSamplesIQ2 = ComData.nSamples;
}

/**********************************************************************/
/*																	  */
/* static void initializeAssistanceTime(T_supportData *supportData_P) */
/*																	  */
/**********************************************************************/
static void initializeAssistanceTime(T_supportData *supportData_P)
{
	char *pchSrxAux, *pchStereoAux, szMonth[10], partText[29];

	double sec;

	time_t ltime;
	struct tm *gmtTime;

	if(Config.Mng.startMode > COLD_START)
	{
		aidingData.refTime.WN  = Config.Aid.refWn;
		aidingData.refTime.TOW = Config.Aid.refTow;
	}

	// Reference time not provided
	if (aidingData.refTime.WN == -1)
	{
		// Get current time.
		ltime   = time(NULL);
		gmtTime = gmtime(&ltime);

		// Update year.
		gmtTime->tm_year = gmtTime->tm_year + 1900;

		if(Config.Com.mode == POSTPROCESS) // Real time execution: Reference Time taken from the computer.
		{
			//////////////////
			// SRX file
			pchSrxAux = strstr(Config.Com.sampleFile, SRX_BINARY_STR);

			if ((pchSrxAux != NULL) && ((pchSrxAux-Config.Com.sampleFile) >= SRX_TIME_STR_LEN))
			{
				pchSrxAux -= SRX_TIME_STR_LEN;
			}
			if (pchSrxAux != NULL)
			{
				strncpy(partText, pchSrxAux, SRX_TIME_STR_LEN);

				partText[SRX_TIME_STR_LEN] = 0;

				sscanf(partText, "%d_%d_%d_%d_%d_%d", &gmtTime->tm_year, &gmtTime->tm_mon, &gmtTime->tm_mday, &gmtTime->tm_hour, &gmtTime->tm_min, &gmtTime->tm_sec);
			}
		}

		// Compute week number.
		if (aidingData.refTime.WN == -1)
		{
			convertDateToWN(gmtTime, &Config.Aid.refWn);
		}

		// Compute time of week.
		if (aidingData.refTime.TOW == -1)
		{
			convertDateToTOW(gmtTime, &Config.Aid.refTow);
		}

		if(Config.Mng.startMode > COLD_START)
		{
			// Reference week time
			aidingData.refTime.TOW = Config.Aid.refTow;
			aidingData.refTime.WN  = Config.Aid.refWn;
			timeStamp.gpsTime.WN   = Config.Aid.refWn;
		}
	}
}

/*********************************************/
/*                                           */
/* static int initializeAssistancePosition() */
/*											 */
/*********************************************/
static int initializeAssistancePosition()
{
	int ret = EXIT_OK;

	// Any of the warm or assisted modes
	if(Config.Mng.startMode > COLD_START)
	{
		// Reference position is needed: copy it into the aiding structures
		if ((Config.Aid.refPos[0] + Config.Aid.refPos[1] + Config.Aid.refPos[2]) != 0)
		{
			memcpy(&aidingData.refPosWarm.X, &Config.Aid.refPos, sizeof(Config.Aid.refPos));
			memcpy(&aidingData.vtData.hotX,  &Config.Aid.refPos, sizeof(Config.Aid.refPos));
			memcpy(&pvtProcess.lsData.X,     &Config.Aid.refPos, sizeof(Config.Aid.refPos));

			ECI2ECEF(aidingData.refPosWarm.X, aidingData.refPosWarm.V, timeStamp.gpsTime.TOW, timeStamp.gpsTime.WN, timeStamp.gpsTime.WN0, aidingData.refPosWarm.X_WGS84, aidingData.refPosWarm.V_WGS84);
			WGSToGeo(aidingData.refPosWarm.X_WGS84,aidingData.refPosWarm.G);
		}
		else
		{
			Config.Mng.startMode = NOTPOS_EPH;

			ret = EXIT_ER;
		}

		if(Config.Aid.refWn != -1 && Config.Aid.refTow != -1)
		{
			if (Config.Mng.startMode > WARM_EPH && Config.Mng.startMode < NOTPOS_EPH)
			{
				if(Config.Mng.startMode < ASSISTED_ALM)
				{
					aidingData.pvtAcqStatus = 1; // Reference position will be used just to compute visible satellites and rough Doppler
					rxProcessingStatus.aidingStatus = 1;
				}
				else
				{
					// Ephemeris from RinexNAV or SP3 are available.
					// Reference position will be used for virtual tracking (once receiver clock becomes available)
					aidingData.pvtAcqStatus = 2;
					rxProcessingStatus.aidingStatus = 2;
				}
			}
		}
		else
		{
			ret = EXIT_ER;
		}
	}
	else
	{
		aidingData.pvtAcqStatus = 0;
		rxProcessingStatus.aidingStatus = 0;
	}

	return ret;
}

/**************************************************************************/
/*																		  */
/* static int initializeAssistanceEphemeris(T_supportData *supportData_P) */
/*																		  */
/**************************************************************************/
static int initializeAssistanceEphemeris(T_supportData *supportData_P)
{
	int ret = EXIT_OK, timeOfDay = 0, epochIndex = 0;

	// Ephemeris modes
	if(Config.Mng.startMode == WARM_EPH || Config.Mng.startMode == ASSISTED_EPH || Config.Mng.startMode == NOTPOS_EPH)
	{
		if (supportData_P->genSysData_P->systemFlag == 0)
		{
			// Try to read ephemeris file.
			if (readEphemerisGps() == 0)
			{
				timeOfDay   = ((int)(aidingData.refTime.TOW + 7200) % 86400);
				epochIndex  = (int)(((double)timeOfDay)/7200.0);

				memcpy(&aidingData.gpsEph, &aidingData.gpsEphME[epochIndex], sizeof(aidingData.gpsEph));

				// Succeeded, ephemeris read from the .NAV file
				memcpy(aidingData.gpsAlmEph, aidingData.gpsEph, sizeof(aidingData.gpsAlmEph));
			}
			else
			{
				ret = EXIT_ER;
			}
		}
		if (supportData_P->genSysData_P->systemFlag == 2)
		{
			// Try to read ephemeris file.
			if (readEphemerisGal() == 0)
			{
				timeOfDay   = ((int)(aidingData.refTime.TOW + 7200) % 86400);
				epochIndex  = (int)(((double)timeOfDay)/7200.0);

				memcpy(&aidingData.galEph, &aidingData.galEphME[epochIndex], sizeof(aidingData.galEph));

				// Succeeded, ephemeris read from the .NAV file
				memcpy(aidingData.galAlmEph, aidingData.galEph, sizeof(aidingData.galAlmEph));
			}
			else
			{
				ret = EXIT_ER;
			}
		}
	}
	return ret;
}

/********************************************************************************************************/
/*																									    */
/* int initializeAssistance(T_supportData *supportData_P, T_SatManager *satManag_P, T_ChData *chData_P) */
/*																									    */
/********************************************************************************************************/
static int initializeAssistance(T_supportData *supportData_P, T_SatManager *satManag_P, T_ChData *chData_P, char main_system)
{
	int ret = EXIT_OK, i = 0, k = 0;
    int core_index = 0;
    

	if(main_system)
	{
		// When start, receiver clock is always unknown (INVALID).
		pvtProcess.lsData.clkData.hotClkBiasStatus = INVALID;

		// Initialize aiding structures.
		memset (&aidingData, 0, sizeof(T_DAid));
		memset (&pvtProcess, 0, sizeof(S_PVT_Process));

		aidingData.refTime.TOW = -1;
		aidingData.refTime.WN  = -1;

		for (i = 0; i < MAXNUMSAT; i++)
		{
			aidingData.gpsEph[i].svHealth    = -1;
			aidingData.galEph[i].svHealth    =  1;
			aidingData.alm[i].almhlth        = -1;
			aidingData.gpsAlmEph[i].svHealth = -1;
			aidingData.galAlmEph[i].svHealth =  1;
		}

		////////////////////////////////////////////////
		// CODE-SWEEPING COUNTER FOR VIRTUAL TRACKING //
		////////////////////////////////////////////////

		// deltaCodeVector: in samples

		aidingData.vtData.deltaCodeVector[0] = 0;

		for (i = 0; i < 49; i++)
		{
			aidingData.vtData.deltaCodeVector[i*2+1] =   i + 1;
			aidingData.vtData.deltaCodeVector[i*2+2] = -(i + 1);
		}

		////////////////////////////////////////
		// Initialize measurement structures. //
		////////////////////////////////////////
		timeStamp.measBlock = 560;

		for(k = 0; k < NSYSTEMS; k++)
		{
			MeasProcess[k].NM = 0;
			for(i = 0; i < MAXNUMSAT; i++)
			{
				MeasProcess[k].SVN[i] = 0;
			}
		}
 
		// Open file for storing decoded almanacs
		//if (Config.Com.mode != PCMONITOR)
		//{
		//	initializeAlmanacOut();
		//}

		// Almanac modes

//                i = almanacIndexBuffer;       
//                for (k = 0 ; k < MAXNUMSAT ; k++)
//                {
//                    
//                        aidingData.alm[k].ecc	     = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].inclin     = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].rora	     = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].af0	     = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].af1	     = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].sqrta      = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].ratoa      = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].argpg      = *(double *)	        (buffer + i); i += sizeof(double);
//                        aidingData.alm[k].manom      = *(double *)	        (buffer + i); i += sizeof(double);
//                }
//                // After doubles, unsigned
//                for (k = 0 ; k < MAXNUMSAT ; k++)
//                {
//                        aidingData.alm[k].SVN	     = *(int *)	(buffer + i); i += sizeof(int);
//                        aidingData.alm[k].vflg	     = *(int *)	(buffer + i); i += sizeof(int);
//                        aidingData.alm[k].almhlth	 = *(int *)	        (buffer + i); i += sizeof(int);
//                        aidingData.alm[k].refweek	 = *(int*)	(buffer + i); i += sizeof(int);
//                        aidingData.alm[k].toa	     = *(int *)	(buffer + i); i += sizeof(int);
//                } 
//     
//        // Convert available almanacs into ephemeries format.
//	for(i = 0; i < MAXNUMSAT; i++)
//	{
//		if(aidingData.alm[i].SVN == i+1)
//		{
//			// Almanac is available for this SVN
//			if ( aidingData.alm[i].SVN <= 32  && aidingData.gpsAlmEph[i].vflg == 0) // almanac is GPS
//			{
//				Alm2Eph(aidingData.alm[i], &aidingData.gpsAlmEph[i]);
//				gnssProcessingStatus.gpsProcessingStatus[i].AlmEphStatus	=  1;
//			}
//			else  if (aidingData.alm[i].SVN >= 36  && aidingData.galAlmEph[i-35].vflg == 0) // almanac is GAL
//			{
//				aidingData.alm[i].SVN = aidingData.alm[i].SVN - 35;
//				Alm2EphGal_my(&aidingData.alm[i], &aidingData.galAlmEph[i-35]);
//				gnssProcessingStatus.galProcessingStatus[i-35].AlmEphStatus	=  1;
//			}
//		}
//	} 
//       
//       
//        memset(aidingData.refPosWarm.V, 0, sizeof(aidingData.refPosWarm.V));  
//        memset(aidingData.vtData.hotV, 0, sizeof(aidingData.vtData.hotV));
//        
//        memcpy(&aidingData.refPosWarm.X, &Config.Aid.refPos, sizeof(Config.Aid.refPos));
//        memcpy(&aidingData.vtData.hotX,  &Config.Aid.refPos, sizeof(Config.Aid.refPos));
    
        
       // printf("%f %f %f\n",aidingData.refPosWarm.X[0], aidingData.refPosWarm.X[1], aidingData.refPosWarm.X[2] );
       // printf("%f %f %f\n",aidingData.refPosWarm.V[0], aidingData.refPosWarm.V[1], aidingData.refPosWarm.V[2] );    
        
		// Set timestamp if given by assistance.
		initializeAssistanceTime(supportData_P);

		// Set initial user position if given by assistance.
		ret = initializeAssistancePosition();
	}

	// Flag to be more agressive (1) or conservative (0) in the TTFF approach in assisted mode.
	// assistanceEpochsTh is the number of epochs between assistancePre and assistancePost.
	if (Config.Mng.fastFirstFixFlag == 1)
	{
		supportData_P->MeasProcess_P->assistanceEpochsTh = 1;
	}
	else
	{
		supportData_P->MeasProcess_P->assistanceEpochsTh = 4;
	}

	// Set satellite ephemeris if given by assistance. This function is called each time that the function
	// initializeAssitance is called, because it reads one system at a time (GPS/Galileo).
	ret = initializeAssistanceEphemeris(supportData_P);

	/***********************
      MANAGE VISIBILITY MODE
	 ***********************/
	if (aidingData.pvtAcqStatus == 0 || Config.Mng.startMode == COLD_START || Config.Mng.startMode == NOTPOS_EPH)
	{
		for(core_index = 0; core_index < NACQCHAN ; core_index++)		
		{
			// Cold, Position not available or configured by user
			SetColdVisibility(supportData_P, core_index);
		}
	}
	else if (aidingData.pvtAcqStatus > 0 && Config.Mng.startMode > COLD_START)
	{
		// Position available (either for Warm or Virtual-Tracking, to be checked inside).
		// CheckVisibilityAllSlots(supportData_P, chData_P);
	}

	return ret;
}

/*******************************************************/
/*													   */
/* static void initialize_acq_list(T_ListSat *queue) */
/*													   */
/*******************************************************/
void initialize_acq_list(T_ListSat *queue)
{
	int i;
	for(i = 0; i < MAXNUMSAT; i++)
	{
		if(i >= GALNAVINDEX)
		{
			queue->svnList  [i] =  i+1-GALNAVINDEX;
		}
		else
		{
			queue->svnList  [i] =  i+1;
		}
			queue->listIndex[i] = i;
	}
	queue->i = 0;
	if(Config.Com.systemFlag == 0)
	{
		queue->N = MAXNUMGPS;
	}
	else if (Config.Com.systemFlag == 2)
	{
		queue->N = MAXNUMGAL;
	}
	else if (Config.Com.systemFlag == -1)
	{
		queue->N = MAXNUMSAT;
	}
	queue->nextElement = 0;
}

/*******************************************************/
/*													   */
/* static void initialize_queue_list(T_ListSat *queue) */
/*													   */
/*******************************************************/
// void initialize_queue_list(T_ListSat *queue)
// {
// 	int i;
// 	for(i = 0; i < MAXNUMSAT; i++)
// 	{
// 		queue->svnList  [i] =  0;
// 		queue->listIndex[i] = -1;
// 	}
// 	queue->i = 0;
// 	queue->N = 0;
// 	queue->nextElement = 0;
// }

/****************************************************************************

  void initializeSatManag(T_SatManager *satManag_P)

 ****************************************************************************/
static void initializeSatManag(T_supportData *supportData_P)
{
	int i = 0, k = 0;         // Loop index.

	int nForbiddenSats = 0,
		prn			   = 0,
		aux			   = 0;

	if (supportData_P->genSysData_P->systemFlag == 2)
	{
		nForbiddenSats = 0;
	}

	int forbiddenSats[MAXNUMSAT] = {1,2,3,4,5,6,7,8,9,10,13,14,15,16,17,18,21,22,23,24,25,26,27,28,29,30};

	// Initially, FFT-based acquisition is always necessary (in any mode).
	supportData_P->satManag_P->fftAcqFlag = 1;
	rxProcessingStatus.acqThreadStatus[0].fftAcqFlag = 1;

	// Initialize forced satellites structure.
	supportData_P->satManag_P->unhealthy.N = 0;

	// Initialize unknown satellites structure.
	supportData_P->satManag_P->fftAcqUnknownQueue.N = 0;

	// Initialize visible satellites structure.
	supportData_P->satManag_P->nonVisible.N = 0;
	supportData_P->satManag_P->fftAcqVisibleQueue.N = 0;

	memset(&supportData_P->satManag_P->acqFftSat,0,sizeof(T_DataAcqFft));

	// Initialize acqFactor (start on acqFactorVec[0], i.e. no channel in tracking)
	supportData_P->satManag_P->acqFactor = Config.FftAcq.acqFactor;

	// FftAcq Visible Queue
	initialize_queue_list(&supportData_P->satManag_P->fftAcqVisibleQueue);

	// FftAcq Unknown Queue
	initialize_queue_list(&supportData_P->satManag_P->fftAcqUnknownQueue);

	// Non-visible list
	initialize_queue_list(&supportData_P->satManag_P->nonVisible);

	// Unhealthy list
	initialize_queue_list(&supportData_P->satManag_P->unhealthy);

	// Virtual Tracking Queue
	initialize_queue_list(&supportData_P->satManag_P->vtQueue);

	// Initialize management structures.
	for (i = 0; i < MAXNUMSAT; i++)
	{
		// Initially, all satellites are "not processed"
		supportData_P->satManag_P->satProcessingStatus[i] = -1;

		// Initially, all satellites are considered "unknown"
		supportData_P->satManag_P->satVisibilityStatus[i] =  1;
	}

	// SET ALLOWED SATELLITES
	supportData_P->satManag_P->allowedSats.N = MAXNUMSAT - nForbiddenSats;

	for (i = 0; i < MAXNUMSAT; i++)
	{
		prn = i+1;
		aux = 1;

		for (k = 0; k < nForbiddenSats; k++)
		{
			if(forbiddenSats[k] == prn)
			{
				supportData_P->satManag_P->allowedSats.listIndex[i] = 0;
				aux = 0;
			}
		}

		if (aux == 1)
		{
			supportData_P->satManag_P->allowedSlots.listIndex[i] = 1;
			supportData_P->satManag_P->allowedSlots.svnList  [i] = prn;
		}
	}

	// Set initially to zero.
	memset (&supportData_P->satManag_P->vtChannels, 0, sizeof(T_ListChan));

	// INITIALIZE subframeAux structure (used during GPS message decoding, globally visible to procsbf.c).
	memset( &subframeAux, 0, sizeof(subframeAuxStructure)*MAXNUMSAT);

	// Initialize TOE and valid subframes flag per satellite.
	for (i = 0; i < MAXNUMSAT; i++)
	{
		// Initialize subframes 1, 2 and 3 flags.
		subframeAux[i].valid[0] = -1;
		subframeAux[i].valid[1] = -1;
		subframeAux[i].valid[2] = -1;
	}
}

/***************************************

  void finalizeChannelData()

 ***************************************/
static void finalizeChannelData()
{
	int i;
	// Free resources per channel.
	for (i = 0; i < Config.Mng.nChannels; i++)
	{
		//// Free lock parameters.
		//free(chDataAll.Ch[i].Lck.NPk);
		chDataAll.Ch[i].Lck.NPk = 0;
		chDataAll.Ch[i].Lck.cliVectork = 0;
		//free(chDataAll.Ch[i].Lck.cliVectork);
	}

	// Free global channel structures.
	free(chDataAll.Ch);
}

/***********************************************

  void finalize()

 ***********************************************/
void finalize()
{
	// Finalize channels resources.
	finalizeChannelData();

	// Free data buffer data.
	finalizeDataBuffer();

	close_pvt_files();
	close_rinex_files();

	finalizeEphemeris();

	finalizeAlmanacOut();

	int k;
    int core_index;
	for(k = 0; k < NSYSTEMS; k++)
	{
		for(core_index = 0; core_index < NACQCHAN; core_index++)		
		{
				// Free acquisition resources.
				finalizeACQ(&supportData[k], core_index);
		}
		// Free tracking resources.
		finalizeTracking(&supportData[k]);
	}

	// Free threads resources.
	#ifndef PCMonitor
	finalizeThreads();
	#endif
}



/* Must be deleted after add gnss .lib file and */
void initialize_queue_list(T_ListSat *queue)
{
    
}

void initializeSynchrodecod(T_Channel *chanAux)
{
    
}

void Bpsk2Boc11(int nSlots)
{
    
}

int initializeAcquisition(T_supportData *supportData_P)
{
    
}

int initializeTracking(T_supportData *supportData_P, T_ChData *chData_P)
{
    
}

void SetColdVisibility(T_supportData *supportData_P, int core_index)
{
    
}

int readEphemerisGps(void)
{
    
}

int readEphemerisGal(void)
{
    
}

void ECI2ECEF(double *pos_ECI, double *vel_ECI, double TOW, int WN, int WN0, double *pos_ECEF, double *vel_ECEF)
{
    
}

void WGSToGeo(double XWGS84[3], double G[3])
{
    
}

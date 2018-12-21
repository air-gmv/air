/**********************************************************************************************/
/**********************************************************************************************/
/*																							  */
/* This file contains definition for most of the types and structures used along the receiver */
/* The name "siria" comes from the first project in which the software receiver was developed */
/*																							  */
/**********************************************************************************************/
/**********************************************************************************************/

#ifndef  SIRIA_H
#define  SIRIA_H

#include <Defs_nav.h>
#include <interface.h>
#include <complexAPI.h>
#ifndef _rtems_app
#include <windows.h>
#endif

#include <math.h>
#include <time.h>
#include <matrix.h>


//*****************************************************************************
//                     TYPE AND STRUCTURE DEFINITION
//*****************************************************************************
#define FFT_SIZE_MAX 32768
#define I_FFT_LENGTH         (FFT_SIZE_MAX/2)
/*************************************/
/* List structure for all satellites */
/*************************************/
typedef struct
{
	int  N;                    /* Number of satellites in the queue.                 */
	int  svnList[MAXNUMSAT];   /* List of PRNs in the queue.                         */
	int  listIndex[MAXNUMSAT]; /* listSat.SVN[listIndex[svn-1]] = svn                */
	int  nextElement;          /* referred to SVN list [0 to N-1]                    */
	int  i;                    /* Auxiliar counter                                   */

} T_ListSat;

/**********************************************************************************/
/* Processing status for particular SVN number									  */
/*																				  */
/**********************************************************************************/
typedef struct
{
	int				svn;				// GNSS satellite number  (from 1:MAXNUMGPS/MAXNUMGAL;)
	int				acqStatus;			// if satellite is acquired: 
									//				0 - not acquired
									//				1 - acquired
									//				2 - processing/acquiring
	int				tckStatus;			// if satellite is tracked:
									//				0 - not
									//				1 - tracked nonSync
									//				2 - tracked Sync
	int				AlmEphStatus;   		// Almanac/Ephemerides available:
									//				0 - not
									//				1 - Almanac
									//				2 - Ephemerides
	int				satInView;			// Flag indicating if satellite is in view:
									//                             -1 - unknown
									//				0 - not in view
									//				1 - in view
	int				satHealthy;			// Flag indicating if satellite is healthy:
									//                             -1 - unknown
									//				0 - not healthy
									//				1 - healthy
	int				vtStatus;			// Virtual Tracking status:
									//				0 - not in mode
									//				1 - in mode
									//				2 - acquired through VT
	int				vtPossible;			// In this iteration sat is possible for VT (not ACQ or TCK yet; PVT hot; visible and healthy)
									//				1 - possible
									//				0 - not possible/not needed
	double				elevation;			// Elevation when visible (-1 when not visible/unknown)
	int				acqMs;				// Time of the acq Doppler and Sample code
	float				acqDoppler;			// Doppler in Hz from acquisition (modified only once per acquisition)
	int				acqSampleCode;                  // Sample delay form acquisition process (modified only once per acquisition)
	int				tckMs;				// Time of the tck Doppler and Sample code
	float				tckDoppler;			// Actual Doppler from tracking (updated "every step")	
	int				tckSampleCode;                  // Sample delay from tracking (updated "every step")
	int				vtMs;				// Time of the vt Doppler and Sample code
	float				vtDoppler;			// Actual Doppler from virtual tracking (updated "every step")	
	int				vtSampleCode;                   // Sample delay from virtual tracking (updated "every step")
} S_satProcessingStatus;

typedef struct
{
	S_satProcessingStatus		gpsProcessingStatus[MAXNUMGPS];		// GPS satellites number  (from 1:MAXNUMGPS;)
	S_satProcessingStatus		galProcessingStatus[MAXNUMGAL];		// GAL satellites status  (from 1:MAXNUMGAL;)
} S_gnssProcessingStatus;


typedef struct
{
	int			nrOfTrackedSats;		// Number of satellites tracked by the thread
	int			tckChannelNr[4];		// Number of channel:
										//			- in GPS mode: 1:12 (4 SC per thread)
										//			- in GAL mode: 1:6 (3 SC per thread)
										//			- in GPS+GAL mode: 1:8 (2 threads for GAL with 2 SC per thread and 1 thread with 4 SC for GPS)
	int 		tckChannelStatus[4];	// status of the channel:
										//			0 - free
										//			1 - tracking in noSync mode
										//			2 - tracking in Sync mode
										//			3 - Virtual Tracking
	int			svnInProcess[4];		// svn number of a processed satellite (-1 if no tracking)
	int			tckThreadStatus;		// tracking thread status:
										//			   -1 - idle
										//				0 - free
										//				1 - processing
} S_tckThread;

typedef struct
{
	int 		acqStatus;				// status of the fft acq canal:
										//		   -1 - idle/ not running
										//			0 - free
										//			1 - in fft acquisition
	int			svnInProcess;			// svn number of a processed satellite (-1 if no acquisition)
	int			fftAcqFlag;				// flag indicating if fft acq is still needed/possible:
										//			0 - not needed
										//			1 - needed
} S_acqThread;

typedef struct
{
	int 		vtStatus;				// status of the vt acq canal:
										//			0 - free
										//			1 - in vt acquisition
	int			svnInProcess;			// svn number of a processed satellite (-1 if no acquisition)
	int			vtAcqFlag;				// flag indicating if vt acq is still needed/possible:
										//			0 - not possible
										//			1 - possible
} S_vtStatus;

typedef struct
{
	int 		pvtStatus;				// status of the channel:
										//		   -1 - idle/ not running
										//			0 - no pvt solution
										//			1 - pvt solution
										//			2 - pvt solution discarded (due to low accuracy)
	int			pvtTime;				// time of pvt calculation
	int			svnProcessedList[3*MAXNUMSAT];	// list of processed SVN's
	int			svnProcessedNr;			// svn number of a processed satellite (-1 if no solution)
} S_pvtThread;

typedef struct
{
	int				acqGalFlag;				// flag that indicates which const should be aquired next:
											//			    0 - GPS
											//				1 - GAL
	int				waitForGal;				// flag that indicates that there is need to wait until 4th milisecond is reached before starting GAL acq
	int				aidingStatus;			// tracking thread status:
											//			    0 - no aid
											//				1 - pv
											//				2 - ephemerides/almanacs
											//				3 - full aid
	int				acqInitFlag;			// flag indicating the initialization of the Rx
	int				acqSatNr;				// number of acquired satellites
	S_acqThread		acqThreadStatus[3];		// acquisition thread status
	int				acqThreadIndicator;		// indicates in which thread the next satellite should be placed

	S_vtStatus		vtAcqStatus[MAXNUMCHAN];// VT tracking status
	int				vtChannelNr;			// Number of satellites in virtual tracking
	
	S_tckThread		tckThread[3];			// tracking thread status
	int				tckThreadIndicator;		// indicates in which thread the next found satellite should be placed
	//S_tckThread		tckThread02;			// tracking thread status
	//S_tckThread		tckThread03;			// tracking thread status
	
	S_pvtThread		pvtThread;				// PVT thread
        
        double			FllAccumSigns[32][6];

	T_ListSat		vtQueueGps;
	T_ListSat		vtQueueGal;
	T_ListSat		acqQueue;
	T_ListSat		tckList;
	T_ListSat		visibleList;
	T_ListSat		nonVisible;				// List of non visible satellites
	T_ListSat		unhealthy;				// List of unhealthy satellites
	int			newFileFlag[3*MAXNUMSAT];		// flag to indicates to overwrite the file
	int			secondFft;				// flag that indicates that second fft is to be performed (0 - to be performed; 1 - performed)
} S_rxProcessingStatus;

typedef struct
{
	double          gpsAidTime;	
	double          gpsTTrans[MAXNUMGPS];
	double          gpsPseudoRange[MAXNUMGPS];
	double          gpsDoppler[MAXNUMGPS];
	double          gpsDopplerRate[MAXNUMGPS];
	int		gpsDopplerRange[MAXNUMGPS];
	int		gpsSampleCode[MAXNUMGPS];
	int		gpsSampleCodeRange[MAXNUMGPS];
	int		gpsAcqFlag[MAXNUMGPS];
	int		gpsVtStartMs[MAXNUMGPS];
	int		gpsVtStartMsDop[MAXNUMGPS];

	double          galAidTime;
	double          galTTrans[MAXNUMGAL];
	double          galPseudoRange[MAXNUMGAL];
	double          galDoppler[MAXNUMGAL];
	double          galDopplerRate[MAXNUMGAL];
	int		galDopplerRange[MAXNUMGAL];
	int		galSampleCode[MAXNUMGAL];
	int		galSampleCodeRange[MAXNUMGAL];
	int		galAcqFlag[MAXNUMGAL];
	int		galVtStartMs[MAXNUMGAL];
	int		galVtStartMsDop[MAXNUMGAL];
} S_AcqTckAid;


/******************/
/* FFT-Acq status */
/******************/
typedef enum E_FftAcqStatus
{
   ACQ_SEARCH_BIN,     // 0 Acquisition searching bin.
   ACQ_MED,            // 1 Medium step for frequency refinement.
   ACQ_FINE,           // 2 Fine frequency refinement.

} E_FftAcqStatus;

/**************/
/* Start mode.*/
/**************/
typedef enum E_StartMode
{
   COLD_START,	  // 0 Cold -> no aiding data.
   WARM_ALM,	  // 1 Warm -> based on almanac, FFT-based Acq.
   WARM_EPH,	  // 2 Warm -> based on ephemerides (rinex file), FFT-based Acq.
   WARM_SP3,	  // 3 Warm -> based on ephemerides (SP3 file), FFT-based Acq.
   ASSISTED_ALM,  // 4 Assisted -> based on recent almanacs and external position. Initially FFT-based Acq until time synchronization -> then VT.
   ASSISTED_EPH,  // 5 Assisted -> based on ephemerides (rinex file) and external position. Initially FFT-based Acq until time synchronization -> then VT.
   ASSISTED_SP3,  // 6 Assisted -> based on ephemerides (SP3 file) and external position. Initially FFT-based Acq until time synchronization -> then VT.
   NOTPOS_EPH,	  // 7 Assisted with EPH (intended) but not a-priori position available. Initially FFT-based until PVT, then VT.
   NOTPOS_SP3,	  // 8 Assisted with SP3 (intended) but not a-priori position available. Initially FFT-based until PVT, then VT.

} E_StartMode;

/*******************/
/* Execution mode. */
/*******************/
typedef enum E_ExecutionMode
{
   PCMONITOR,			 // 0 PCmonitor UDP data communication
   POSTPROCESS,          // 1 Postprocess samples from file.
} E_ExecutionMode;

/****************************/
/* Threads execution status.*/
/****************************/
typedef enum E_ExecutionStatus
{
   EXECUTING,            // 0 Receiver executing.
   END_USER_REQUEST,     // 1 User stops execution.
   NO_MORE_SAMPLES,      // 2 No more samples in file or received from USB.
   END_EXECUTION,        // 3 EOF, simulation time reached or error in input device.
   WAIT_FOR_SAMPLES      // 4 GNSS have no sample for signal processing.                   
} E_ExecutionStatus;

// Structure that defines the pointer to samples into buffer.
typedef struct 
{
   int bit;               // Bit into word where sample is stored.
   int word;              // Word where sample is stored.
}sampleIndex_t;

/***********************************/
/***********************************/
/*								   */
/* STRUCTURES FOR FFT-ACQ		   */
/* USED ONLY IN WINDOWS (OR LINUX) */
/*								   */
/***********************************/
/***********************************/

/*************************************/
/* Status of the acquisition channel */
/*************************************/
typedef enum E_AcqChStatus { FREE, ACQUIRING, NOT_TO_SEARCH, START} E_AcqChStatus;


/***************************************************/
/* Basic cell for one frequency bin during FFT-Acq */
/***************************************************/
struct freqCellStruct
{
   int     freqBinLutIndex;
#ifdef MEMORY_ALLOCATION
   float   *accumulatedPowerVector;
#else // MEMORY_ALLOCATION
   float   accumulatedPowerVector[I_FFT_LENGTH];
#endif // MEMORY_ALLOCATION
   double  binFreq;
};

/*****************************************/
/* Structure for the acquisition channel */
/*****************************************/
// struct acquisitionChannel
// {
//    int			   SVN,						// Space-Vehicle Number
// 				   svnIndex,				// svnIndex = SVN-1
// 
// 				   slot,					// GLONASS:  -7 :  6
//     			   slotIndex;				//			  0 : 14
// 											// GPS & GAL: SVN = slot; svnIndex = slotIndex.
// 
//    int             freqBins2Search;
//    int             freqBinCounter;
// 
//    E_AcqChStatus   busyStatus;			  // FREE; ACQUIRING; NOT_TO_SEARCH
//    int             acqFftStatus;		  // ACQ_SEARCH_BIN; ACQ_MED; ACQ_FINE
// 
//    int             doFFT;				  // Flag to indicate that FFT needs to be computed on a new block of input signal (1 / 0).
// 
//    float           sum;
// 
//    int             maxCodeIndex;
//    float           maxCodePower;
// 
//    int             nCohInt;				  // Number of non coherent accumulations during acquisition.
// 
// #ifdef MEMORY_ALLOCATION
//    struct freqCellStruct *freqCellAccumulation;
// #else // MEMORY_ALLOCATION
//    struct freqCellStruct freqCellAccumulation[NBINS];
// #endif // MEMORY_ALLOCATION
// 
//    int             waitMsCounter;
// 
//    int             medShifts;
// };

/**********************************************************************************/
/* Auxiliar data to be used during the acquisition process, for one constellation */
/* (pointers, reserved memory for FFT, length)									  */
/**********************************************************************************/
typedef struct
{
#ifdef MEMORY_ALLOCATION
	struct complex_t     *prnExpFftCLut[MAXNUMSAT]; // FFT acqusition LUT.
	struct complex_t     *inFftComplex;					 // Input real vector for acquisition FFT.
	struct complex_t     *outFftComplex;				 // Output complex vector for acquisition FFT.
	struct complex_t        *inFftComplex_ptr;				 // Pointer to Input complex vector for IFFT.
	struct complex_t        *outFftComplex_ptr;
	//fftwf_plan           planFftComplex;				 // Plan for interference-detection FFT.

	struct complex_t     **signalFft;					 // Array for output signal FFT's (non-coherent).

	struct complex_t     *inIfftComplex;				 // Input complex vector for IFFT.
	struct complex_t     *outIfftComplex;				 // Output complex vector for IFFT.
	struct complex_t        *outIfftComplex_ptr;			 // Pointer to Output complex vector for IFFT.
	//fftwf_plan           planIfftComplex;				 // Plan for BACKWARD FFT.

	struct complex_t     *signalFftPrnExpFftC;			 // Input complex vector for IFFT.
	struct complex_t        *signalFftPrnExpFftC_ptr;		 // Pointer to Input complex vector for IFFT.

	struct complex_t     *inFineFftComplex;
	struct complex_t     *outFineFftComplex;			 // Output complex vector for FineFreq FFT.
	struct complex_t        *inFineFftComplex_ptr;
	struct complex_t        *outFineFftComplex_ptr;		 // Pointer to Output complex vector for FineFreq FFT.
	//fftwf_plan           planFftFineComplex;			 // Plan for REFINE FREQ FORWARD FFT.

	int				     **codeMedFineLut;				 // Look-Up Tables for the frequency estimation process in ACQ.
#else

    struct complex_t  inFftComplex[FFT_SIZE_MAX];  // Input real vector for acquisition FFT.
    struct complex_t  outFftComplex[FFT_SIZE_MAX];  // Output complex vector for acquisition FFT.
    
    struct complex_t  signalFftPrnExpFftC[FFT_SIZE_MAX];  // Input complex vector for IFFT.

    struct complex_t  inIfftComplex[I_FFT_LENGTH];  // Input complex vector for IFFT.
    struct complex_t  outIfftComplex[I_FFT_LENGTH];  // Output complex vector for IFFT.

    //struct complex_t  inFineFftComplex[MED_FFT_LENGTH];  
    //struct complex_t  outFineFftComplex[MED_FFT_LENGTH];  // Output complex vector for FineFreq FFT.

    struct complex_t  signalFft[MAXNONCOHINT][FFT_SIZE_MAX];  // Array for output signal FFT's (non-coherent).
		
	//int		 codeMedFineLut[MAXNUMSAT][MED_FFT_LENGTH];	 
//	int				     **codeMedFineLut;				 //estimation process in ACQ.
	
	//struct complex_t  prnExpFftCLut[MAXNUMSAT][I_FFT_LENGTH]; // FFT acqusition LUT.

	
#endif
	
	
	int 			     fftLength,						 // Length (number of samples) of the direct FFT during acquisition.
					     ifftLength,					 // Length (number of samples) of the inverse FFT during acquisition.
					     medFftLength,					 // Length (number of samples) of the FFT used during medium-step for frequency estimation.
					     fineFftLength,					 // Length (number of samples) of the FFT used during finer-step  for frequency estimation.
  					     numberTotalBins;
	
	int					 fftMs;
	int					 acqCounterMs;
	sampleIndex_t		 outIndex;
        sampleIndex_t		 outIndex_init;
        

	   int			   SVN,						// Space-Vehicle Number
				   svnIndex,				// svnIndex = SVN-1

				   slot,					// GLONASS:  -7 :  6
    			   slotIndex;				//			  0 : 14
											// GPS & GAL: SVN = slot; svnIndex = slotIndex.

   int             freqBins2Search;
   int             freqBinCounter;

   E_AcqChStatus   busyStatus;			  // FREE; ACQUIRING; NOT_TO_SEARCH
   int             acqFftStatus;		  // ACQ_SEARCH_BIN; ACQ_MED; ACQ_FINE

   int             doFFT;				  // Flag to indicate that FFT needs to be computed on a new block of input signal (1 / 0).

   float           sum;

   int             maxCodeIndex;
   float           maxCodePower;

   int             nCohInt;				  // Number of non coherent accumulations during acquisition.
      
#ifdef MEMORY_ALLOCATION
   struct freqCellStruct *freqCellAccumulation;  // Structure with ongoing acquisition data for one single cell (frequency bin).
#else // MEMORY_ALLOCATION
   struct freqCellStruct freqCellAccumulation[NBINS];  // Structure with ongoing acquisition data for one single cell (frequency bin).
#endif // MEMORY_ALLOCATION
   //int             waitMsCounter;

   int             medShifts;


} S_acqAux;


/********************************************************/
/********************************************************/
/*														*/
/* CONSTELLATION-DEPENDENT PARAMETERS (GPS / GLO / GAL) */
/*														*/
/********************************************************/
/********************************************************/

typedef struct
{
	double	freqPerSv[MAXNUMSAT];			/* Carrier frequency for each satellite. GPS & GAL -> same for all. GLO -> different for each satellite */
	double	deltaFreqPerSv[MAXNUMSAT];		/* GPS & GAL = 0; In GLONASS: delta frequency of each SVN w.r.t. central GLO SVN						*/
	double	ifreqEquiv[MAXNUMSAT];			/* Equivalent IF for each channel. GPS & GAL -> Constant, = Config.ifreq;
											   In GLONASS, includes delta per channel */
	double	codeCarrierFactor[MAXNUMSAT];	/* Carrier freq / chip rate; GPS = 1540; GAL = 385; GLO = freqPerSv / code rate							*/
	double	waveLength[MAXNUMSAT];			/* Carrier wavelength. GPS & GAL = 0.19...m. GLO -> depends on satellite								*/
	int		nSamples;						/* Number of samples in each code period (GPS & GLO = 1 ms; GAL = 4 ms. Depends on sampling frequency	*/
	int		nSamplesDecimatedHalf;			/* Number of samples per code period after applying the decimation factor								*/
	double	codePeriod;						/* Code period: GPS & GLO = 1 ms; GAL = 4 ms.															*/
	int		chipRate;						/* Chip rate: GPS: 1.023 Mcps; GLO = 0.511 Mcps; GAL = 4.092 Mcps										*/
	int		chipsPerCode;					/* # of chips per code period: GPS = 1023; GLO = 511; GAL = 4092										*/
	double	secondsPerChip;					/* Duration of one chip in seconds: GPS = 0.977 �s; GLO = 1.96 �s; GAL = 0.977 �s						*/
	double	chipLength;						/* Length of one code chip in meters: GPS = 293 m; GLO = 587 m; GAL = 293 m								*/
	double	codeLength;						/* Length of one code period: GPS = 299.792 Km; GLO = 299.792 Km; GAL = 1199.169 Km						*/
	double	codePerOverRate;
	double  chipsPerSample;					/* Number (fractions) of chip per each sample */
	double  samplesPerChip;					/* Number of samples in each GPS C/A chip (for configured sampling frequency)							*/

	int		systemFlag;						/* GPS = 0; GLO = 1; GAL =2																				*/

	int		nSlots;							/*	GPS:32;  GLO: 14;	GAL																				*/
	int		nPrn;							/*	    32         1	  5																				*/
	int		nFreq;							/*		 1		  14	  1																				*/
	int		nSv;							/*		32		  24	  5																				*/

	int		deltaIndexSp3;					/*		 0		  37	 61																				*/

	int		svi2sloti[MAXNUMSAT];			/* Useful in GLONASS; conversion from svnIndex to slotIndex												*/
	int		sloti2sviCur[MAXNUMSAT];		/* Useful in GLONASS; conversion from slotIndex to svnIndex: which svn(Index) is actually involved,
											   out of the two possibilities per slot(Index)															*/
	int		sloti2sviAll[MAXNUMSAT][2];		/* Useful in GLONASS; conversion from slotIndes to svnIndex, considering both possibilities per slot	*/

	int		deltaSlotIndexPerSystem;		/* GPS & GAL: -1;  GLO: +7
											   slotIndex = slot + deltaSlotIndexPerSystem */

	double  leapSeconds;					/* Leap Seconds for GLONASS w.r.t. GPS		  */

	int		nWordsPerBlock;					/* Number of int words (32 bits) for bitwise correlation for one code period, including decimation		*/
	int		nWordsPerBlock0;				/* Number of int words (32 bits) for bitwise correlation for one code period, prior to decimation		*/
	int		auxThCorr;

	double	fractCodeThPlus;				/* Threshold for fractionaCodePhase; when exceeded -> jump to next sample								*/
	double	fractCodeThMinus;				/* fractCodeThMinus = -fractCodeThPlus, just for convenience											*/

	float	freqStep;						/* Freq step between two adjacent search bins,in FftAcq.												*/
	int		nPrimaryIterations;				/* # of iterations in the primary system: i.e. in GPS+GAL, # of GPS per Galileo							*/

	int		maxNonCohInt;

	double  blocksBtwEpochs;	  /* Number of blocks between epochs, in ms: = ( 1 / Output rate * Code period).*/

} T_GenericSystemData;

/*************************************/
/*************************************/
/*									 */
/* CLOCK AND TIME RELATED STRUCTURES */
/*									 */
/*************************************/
/*************************************/

/*****************************************************/
/* Structure for defining one GPS epoch (WN and TOW).*/
/*****************************************************/
typedef struct
{
	int		WN0;				   /* GPS week number at first date fix.		 */
    int		WN;                    /* GPS week number.		 */
    double	TOW;                   /* Second into week.		 */

} gpstimestruct;

/****************************************/
/* Structure for defining one GLO epoch.*/
/****************************************/
typedef struct
{
	int	   year;
    int    DoY;                    /* Day of year.			 */
    double ToD;                    /* Time of Day (seconds). */

} glotimestruct;

/************************************/
/* Structure for one calendar epoch.*/
/************************************/
typedef struct
{
	int      year;					/* Year                   */
	int      month;					/* Month                  */
	int      day;					/* Day                    */
	int      hour;					/* Hour                   */
	int      min;					/* Minute                 */
	double   sec;					/* Second                 */
	int		 DoY;				    /* Day of the year        */

} caltimestruct;

/**********************************************/
/* Structure for the clock-related magnitudes.*/
/**********************************************/
typedef struct
{
	double  clkController;		   /* clock controller (s): added to convert "sampled time" (which is just counter of samples) into "receiver time"
									 (roughly in the order of magnitude of GNSS time (there still can be a significant delta clock-bias)			*/

	double  clkController0;		   /* Clock controller can evolve over the time, in clock-steering mode. This variable preserves
									  the initial value of the clock controller (just after the first PVT solution); it is used in the
									  process of setting the ambiguity value for the Carrier Phases, in order to make it small					    */

	double  deltaClkBias;          /* delta clock bias (s): estimate by PVT, used to convert receiver time into GNSS time							*/

	double  clkDrift;              /* clock drift (s/s)																								*/

	double  clkMsWarm;             /* Receiver instant (in "iterations" = "ms") when receiver clock
									  was computed for the last time (for Virtual-Tracking, hot-assistance purposes)								*/

	char    hotClkBiasStatus;      /* VALID / INVALID, to report whether current delta-clock estimate is suitable or not for hot acquisition (VT)	*/

	double  clkControllerGal;	   /* Galileo clock bias (s)																						*/
	double  deltaClkBiasGal;       /* delta clock bias at LSQ estimation (s)																		*/

} clkStruct;

/****************************/
/****************************/
/*							*/
/* GPS EPHEMERIS STRUCTURES */
/*							*/
/****************************/
/****************************/

/****************************************************************************/
/* Almanac data. The data sizes here match the parameter sizes as received  */
/* from the satellite data message. The data has to be scaled appropriately */
/* (i.e. as defined in GPS ICD) before use.									*/
/****************************************************************************/
typedef struct
{
	unsigned short SVN;          /* Space Vehicle Number                       */
    unsigned short vflg;         /* 0=Invalid, 1=Valid, 2=Not Logged.          */
    signed short   almhlth;      /* Almanac health code.                       */
    unsigned short refweek;      /* Reference time, GPS week.                  */
    unsigned short toa;          /* Reference time, seconds of refweek.        */
    unsigned short ecc;          /* Orbital eccentricity.                      */
    short          inclin;       /* Orbital inclination (radians).             */
    short          rora;         /* Rate of right ascension (radians/sec).     */
    short          af0;          /* Clock correction at ref time (seconds).    */
    short          af1;          /* Clock correction rate (seconds/second).    */
    unsigned long  sqrta;        /* Square-root of semi-major axis (m**1/2).   */
    long           ratoa;        /* Right ascension at ref time (radians).     */
    long           argpg;        /* Argument of perigee at ref time (radians). */
    long           manom;        /* Mean anomaly at ref time (radians).        */

} almstruc_raw;

/*********************************************************/
/* Almanac data structure, converted to "physical" units */
/*********************************************************/
typedef struct
{
   int  SVN;
   int  vflg;         /* 0=Invalid, 1=Valid, 2=Not Logged.          */
   int             almhlth;      /* Almanac health code.                       */
   int  refweek;      /* Reference time, GPS week.                  */
   int   toa;          /* Reference time, seconds of refweek.        */
   double          ecc;          /* Orbital eccentricity.                      */
   double          inclin;       /* Orbital inclination (radians).             */
   double          rora;         /* Rate of right ascension (radians/sec).     */
   double          af0;          /* Clock correction at ref time (seconds).    */
   double          af1;          /* Clock correction rate (seconds/second).    */
   double          sqrta;        /* Square-root of semi-major axis (m**1/2).   */
   double          ratoa;        /* Right ascension at ref time (radians).     */
   double          argpg;        /* Argument of perigee at ref time (radians). */
   double          manom;        /* Mean anomaly at ref time (radians).        */

} almstruc_scaled;

/*****************************************************************************/
/* Ephemeris data. The data sizes here match the parameter sizes as received */
/* from the satellite data message. The data has to be scaled appropriately	 */
/* (i.e. as defined in GPS ICD) before use.									 */
/*****************************************************************************/
typedef struct
{
   int            SVN;		/* Space Vehicle number							 */
   unsigned short vflg;     /* 0=Invalid, 1=Valid, 2=Not Logged.             */
   unsigned short s1hlth;   /* SF 1 health code.                             */
   unsigned short codeL2;   /* Code on L2 flag.                              */
   unsigned short wkn;      /* GPS week at time of SF 1 reception.           */
   unsigned short L2Pdata;  /* L2 P data flag.                               */
   unsigned short ura;      /* Satellite's URA code.                         */
   unsigned short iodc;     /* Issue of data, clock.                         */
   unsigned short tocwk;    /* GPS week corresponding to toc.                */
   unsigned short toc;      /* Reference time of clock data parameter set.   */
   unsigned short iode;     /* Issue of data, ephemeris.                     */
   unsigned short toewk;    /* GPS week corresponding to toe.                */
   unsigned short toe;      /* Reference time of ephemeris data set.         */
   unsigned short fti;      /* Fit interval.                                 */
   short          tgd;      /* Group delay parameter.                        */
   short          af1;      /* Clock correction polynomial coefficient.      */
   short          af2;      /* Clock correction polynomial coefficient.      */
   short          crs;      /* Sine harmonic correction to orbital radius.   */
   short          deltan;   /* Mean motion delta from computed value.        */
   short          cuc;      /* Cosine harmonic correction to orbital radius. */
   short          cus;      /* Sine harmonic corr to argument of latitude.   */
   short          cic;      /* Cosine harmonic corr to inclination.          */
   short          cis;      /* Sine harmonic corr to inclination.            */
   short          crc;      /* Cosine harmonic correction to orbital radius. */
   unsigned long  ecc;      /* Eccentricity.                                 */
   unsigned long  sqrta;    /* Square root of semimajor axis.                */
   long           m0;       /* Mean anomaly at TOE.                          */
   long           af0;      /* Clock correction polynomial coefficient.      */
   long           om0;      /* Right ascension at TOE.                       */
   long           in0;      /* Inclination at TOE.                           */
   long           olc;      /* Argument of perigee at TOE.                   */
   long           omd;      /* Rate of right ascension.                      */
   long           idot;     /* Rate of inclination.                          */
} gpsephstruc_raw;

/***********************************************************/
/* Ephemeris data structure, converted to "physical" units */
/***********************************************************/
typedef struct
{
	int      SVN;
	unsigned short vflg;
	int      year;           /* year										 */
	int      month;          /* month										 */
	int      day;            /* day											 */
	int      hour;           /* hour										 */
	int      min;            /* minute										 */
	double   sec;            /* second										 */
	int      svHealth;       /* Health flag									 */
	int      iodc;           /* Issue of data, clock;						 */
	int      toeWeek;        /* Week number for toe							 */
	int      iode;           /* Issue of data, ephemeris					 */
	double   clockBias;      /* a0											 */
	double   clockDrift;     /* a1											 */
	double   clockDriftRate; /* a2											 */
	double   crs;            /* Osculating parameter						 */
	double   cuc;            /* Osculating parameter						 */
	double   cus;            /* Osculating parameter						 */
	double   cis;            /* Osculating parameter						 */
	double   cic;            /* Osculating parameter						 */
	double   crc;            /* Osculating parameter						 */
	double   tgd;            /* Group delay parameter						 */
	double   deltan;         /* Mean motion delta							 */
	double   m0;             /* Mean anomaly at TOE							 */
	double   eEccen;         /* Eccentricity								 */
	double   sqrtA;          /* Square root of semimajor axis				 */
	double   toe;            /* Reference time of ephemeris data set
								(in Time of Week).			  			     */
	double   bigOmega;       /* Right ascension at TOE						 */
	double   i0;             /* Inclination at TOE							 */
	double   bigOmegaDot;    /* Rate of right ascension						 */
	double   litOmega;       /* Argument of perigee at TOE					 */
	double   idot;           /* Rate of inclination							 */
	double   svAccur;        /* Satellite's URA code						 */

} GPSEPHEMSTRUCT_D;

/****************************/
/* Ionospheric and UTC data.*/
/****************************/
typedef struct
{
	short vflg;                      /* 0 = no valid data.								*/

	/* Ionospheric model parameters.*/
	double a0,a1,a2,a3;              /* AFCRL alpha parameters.							*/
	double b0,b1,b2,b3;              /* AFCRL beta parameters.							*/

	/* UTC conversion parameters. */
	double         A0,A1;            /* Coeffs for determining UTC time.                */
	unsigned long  tot;              /* Reference time for A0 & A1, sec of GPS week.    */
	short          dtls;             /* Cumulative past leap seconds.                   */
	unsigned       wnt;              /* Current UTC reference week number.              */
	unsigned       wnlsf;            /* Week number when dtlsf becomes effective.       */
	short          dn;               /* Day of week (1-7) when dtlsf becomes effective. */
	short          dtlsf;            /* Scheduled future leap second event.             */

} iustruc;

/********************************/
/********************************/
/*								*/
/* GALILEO EPHEMERIS STRUCTURES */
/*								*/
/********************************/
/********************************/

// Almanac data structure for I/NAV, converted to "physical" units
typedef struct
{
   unsigned short  SVN;			 /* SVN identifier							   */
   unsigned short  vflg;         /* 0 = Invalid, 1 = Valid, 2 = Not Logged.    */
   unsigned char   E1B_hst;      /* E1B health status  						   */
   unsigned char   E5b_hst;      /* E5b health status						   */
   unsigned short  refweek;      /* Reference time, GPS week.                  */
   unsigned short  ioda;         /* Issue of data							   */
   unsigned long   toa;          /* Reference time, seconds of refweek.        */
   double          ecc;          /* Orbital eccentricity.                      */
   double          inclin;       /* Orbital inclination (radians).             */
   double          rora;         /* Rate of right ascension (radians/sec).     */
   double          af0;          /* Clock correction at ref time (seconds).    */
   double          af1;          /* Clock correction rate (seconds/second).    */
   double          sqrta;        /* Square-root of semi-major axis (m**1/2).   */
   double          ratoa;        /* Right ascension at ref time (radians).     */
   double          argpg;        /* Argument of perigee at ref time (radians). */
   double          manom;        /* Mean anomaly at ref time (radians).        */
} GALALMSTRUCT_INAV;

// Ephemeris data structure for I/NAV, converted to "physical" units
typedef struct
{
	int SVN;

	unsigned short vflg;     /* Flag indicating when data has been loaded    	*/
    unsigned char  svHealth; /* E1B health status  						   	 	*/
	unsigned char  E5b_hst;  /* E5b health status						   	 	*/
    unsigned char  E1B_dvs;  /* E1B data validity  						   	 	*/
    unsigned char  E5b_dvs;  /* E5b data validity  						   	 	*/
	unsigned char  SISA;     /* Signal-in-Space Accuracy					 	*/
	int      year;           /* year											*/
	int      month;          /* month											*/
	int      day;            /* day												*/
	int      hour;           /* hour											*/
	int      min;            /* minute											*/
	double   sec;            /* second											*/
	int      ToW;            /* GST time of week							 	*/
	int      toeWeek;        /* GST week number								 	*/
	int      toe;            /* Reference time of ephemeris data set (s)     	*/
	int      toc;            /* Reference time of clock data set (s)         	*/
	int      iodc;           /* Issue of data        						 	*/
	double   clockBias;      /* a0	(E1,E5b) (s)							 	*/
	double   clockDrift;     /* a1	(E1,E5b) (s/s)							 	*/
	double   clockDriftRate; /* a2	(E1,E5b) (s/s^2)						 	*/
	double   crs;            /* Sine harmonic of orbit radius (m)			 	*/
	double   crc;            /* Cosine harmonic of orbit radius (m)			 	*/
	double   cus;            /* Sine harmonic of argument of latitude (rad)		*/
	double   cuc;            /* Cosine harmonic of argument of latitude (rad)	*/
	double   cis;            /* Sine harmonic of inclination (rad)				*/
	double   cic;            /* Cosine harmonic of inclination (rad)			*/
	double   tgd;		     /* Group delay parameter (s)					 	*/
	double   bgd_E1E5b;      /* Group delay parameter (s)					 	*/
	double   deltan;         /* Mean motion delta (rad/s)					 	*/
	double   m0;             /* Mean anomaly at TOE (rad)					 	*/
	double   eEccen;         /* Eccentricity								 	*/
	double   sqrtA;          /* Square root of semimajor axis (m^(1/2))		 	*/
	double   bigOmega;       /* Right ascension at TOE (rad)		 			*/
	double   i0;             /* Inclination at TOE (rad)			 			*/
	double   bigOmegaDot;    /* Rate of right ascension	(rad/s)					*/
	double   litOmega;       /* Argument of perigee at TOE (rad)				*/
	double   idot;           /* Rate of inclination	(rad/s)						*/

	unsigned char rx_subframes;

} GALEPHEMSTRUCT_INAV;

// Ionospheric and UTC data structure for I/NAV.
typedef struct
{
	short vflg;                      /* 0 = no valid data.								*/

	/* Ionospheric model parameters */
	double         ai0,ai1,ai2;      /* NeQuick parameters.  							*/
	unsigned char  StormFlags;       /* Ionospheric storm flags for 5 different regions */

	/* UTC conversion parameters. */
	double         A0,A1;            /* Coeffs for determining UTC time.                */
	unsigned long  tot;              /* Reference time for A0 & A1, sec of GPS week.    */
	short          dtls;             /* Cumulative past leap seconds.                   */
	unsigned       wnt;              /* Current UTC reference week number.              */
	unsigned       wnlsf;            /* Week number when dtlsf becomes effective.       */
	short          dn;               /* Day of week (1-7) when dtlsf becomes effective. */
	short          dtlsf;            /* Scheduled future leap second event.             */

	/* GPS to Galileo time offset (GGTO) and conversion parameters */
	double         A0g,A1g;          /* Coefficients for determining GGTO				*/
	unsigned long  tog;              /* Reference time for GGTO, sec of GAL week		*/
	unsigned       wng;              /* Reference week for for GGTO, GAL week number	*/

} GalIonoUTC_INAV;

/***************************/
/*						   */
/* STATE-VECTOR STRUCTURES */
/*						   */
/***************************/

/************************************/
/* Satellite State-Vector structure */
/************************************/
typedef struct
{
	char   Valid;       /* flag to indicate if satellite magnitudes have been calculated */
	double P[3];        /* Satellite position in ECI coordenates (meters)             */
	double V[3];        /* Inertial velocity in ECI coordenates (m/s)                 */
	double P_ECEF[3];   /* Satellite position in WGS-84 coordenates (meters)             */
	double V_ECEF[3];   /* Inertial velocity in WGS-84 coordenates (m/s)                 */
	double dtsv;        /* Satellite clock-bias correction, w.r.t. GPS time (meters!!!)  */
	double clkDrift;    /* Satellite clock-drift                                         */
	double elev;        /* Elevation (radians)                                           */
	double azim;        /* Azimuth (radians)											 */

} SSatData;

/*******************************/
/* Tropospheric-data structure */
/*******************************/
typedef struct
{
	double wetZenith;			 /* Wet Zenith Tropospheric Delay [] (millimeters)					*/
	double dryZenith;			 /* Dry Zenith Tropospheric Delay [] (millimeters)				    */
	double totalZenith;			 /* Total Zenith Tropospheric Delay [] (millimeters)			    */

	int	   nextTropoTow;		 /* Next TOW when tropospheric update is programmed					*/
	double tropoCorr[MAXNUMSAT]; /* Vector of tropospheric corrections to be used at epochs when
									they are not recomputed */
	int towPerSat[MAXNUMSAT];    /* Particularization of tropoTow for each SVN						*/

} S_Tropo;

/**************************************/
/* Preprocessed measurement structure */
/**************************************/
typedef struct
{
	double PR_pp[MAXNUMSAT];  /* Preprocessed pseudorange, (tropo, iono corrected, not smoothed)    */
	double CPc[MAXNUMSAT];    /* Carrier Phase measurements (in cycles)                             */
	double Dms[MAXNUMSAT];    /* Doppler measurements (m/s).						                */

	int    kSmo[MAXNUMSAT];   /* Vector of counters (number of epochs) for the smoothing filter     */

	int    NM;				  /* Number of available measurements (sufficiently somoothed or not)   */
	int	   SVN[MAXNUMSAT];    /* List of NM SVNs												    */

	double PR_ls[MAXNUMSAT];  /* Pseudorange to be used for LeastSquare (preprocessed, smoothed...  */
	double sigmaPR_ls[MAXNUMSAT];  /* Pseudorange to be used for LeastSquare (preprocessed, smoothed...  */
	int    NM_ls;			  /* Number of available measurements (sufficiently somoothed or not)   */
	int    SVN_ls[MAXNUMSAT]; /* List of NM SVNs to be used for LeastSquare						    */

	double ionoCorr[MAXNUMSAT];

} CPreProMeas;

/************************************/
/* Dillution-of-precision structure */
/************************************/
typedef struct
{
	double GDoP;   /* Global DoP              */
	double PDoP;   /* Position DoP            */
	double HDoP;   /* Horizontal position DoP */
	double VDoP;   /* Vertical position DoP   */

} S_DOP;

/**********************************/
/* Receiver statevector structure */
/**********************************/
typedef struct
{
	double     X[3];     /* Position in ECI coordinates (meters)					  */
	double     V[3];     /* Velocity in ECI coordinates (meters)					  */
	double     X_WGS84[3];     /* Position in WGS-84 coordinates (meters)					  */
	double     V_WGS84[3];     /* Velocity in WGS-84 coordinates (meters)					  */
	double     G[3];     /* Position in geodetid coordinates: lat,lon (rad),height(m) */
	double     LH[3];    /* Local-Horizon: East, North, Up (meters)					  */
} SUserSV;

typedef struct
{
	double		X[3];     /* Position propagated (meters)					  */
	double		V[3];     /* Velocity propagated (meters/s)					  */
	double		clkBias;/* Clock bias propagated	(s)						 */
	double		clkBiasGal;  /* Clock bias for Gal propagated (s/s)		*/
	Matrix4x4	covX;  /* Position covariance propagated (meters2, s2)		*/
	Matrix4x4	covV;  /* Velocity covariance propagated (meters2/s2, s2/s2)		*/
	double		sigmaX[3];  /* Position sigma propagated (meters)		*/
	double		sigmaV[3];  /* Velocity sigma propagated (meters/s)		*/
	double		sigmaClkBias;  /* Clock bias sigma propagated (s)		*/
	double		sigmaClkBiasDrift;  /* Clock bias drift sigma propagated (s/s)		*/
	
} S_pvtPropagData;

typedef struct
{
	double		x[9];				/* EKF State: pos, vel, bias, drift (m, m/s, s, s/s)	*/
	double		cov[81];			/* State covariance (m2, m2/s2, s2, s2/s2)				*/
	double		prev_x[9];			/* EKF previous state: pos, vel, bias, drift (m, m/s, s, s/s)	*/
	double		prev_cov[81];		/* Previous state covariance (m2, m2/s2, s2, s2/s2)				*/
	double		prev_tow;			/* Previous time of week				*/
	double		sigmaX[3];			/* Position sigma (meters)								*/
	double		sigmaV[3];			/* Velocity sigma (meters/s)							*/
	double		sigmaClkBias;		/* Clock bias sigma (s)									*/
	double		sigmaClkBiasDrift;  /* Clock bias drift sigma (s/s)							*/	
	int		solFlag;			/* Indicate if solution computed (1 vs. 0 )									*/
} S_pvtEkfData;

typedef struct
{
	int         NM;							 /* Number of measurements														*/
	int         SVN[MAXNUMSAT];				 /* Array of SVN used to compute this navigation solution						*/

	Vector		residualPR;					 /* vector of PR residuals w.r.t. aiding reference position						*/
	Vector		residualDoppler;			 /* vector of Doppler residuals w.r.t. aiding reference position				*/
	double		residualPR_B[MAXNUMSAT];	 /* vector of PR residuals w.r.t. GARLIC Kalman PVT (for PVT_B, IGNSSRx)		*/
	double		residualDoppler_B[MAXNUMSAT];/* vector of Doppler residuals w.r.t. GARLIC Kalman PVT (for PVT_B, IGNSSRx)	*/

	double      X[3];						 /* Position in ECI coordinates (meters)										*/
	double      V[3];						 /* Velocity in ECI coordinates (meters)										*/											 
	double      X_WGS84[3];					 /* Position in WGS-84 coordinates (meters)										*/
	double      V_WGS84[3];					 /* Velocity in WGS-84 coordinates (meters)										*/
	double      G[3];						 /* Position in geodetid coordinates: lat,lon (rad),height(m)					*/
	double	    v;

	Matrix4x4	covX;						/* Covariance Position in WGS-84 coordinates (meters2, s2)							*/
	Matrix4x4	covV;						/* Covariance Velocity in WGS-84 coordinates (meters2/s2, s2/s2)							*/
	double		sigmaX[3];					/* Covariance clock bias (s);													*/
	double		sigmaV[3];					/* Covariance clock drift (s/s);												*/
	double		sigmaClkBias;				/* Covariance clock bias (s);													*/
	double		sigmaClkDrift;				/* Covariance clock drift (s/s);												*/

	clkStruct	clkData;					 /* Structure for receiver clock estimates										*/

	S_DOP       DOP;						 /* DoP structure									  							*/

	int         posFlag;					 /* Indicate if position computed (1 vs. 0 )									*/

	Matrix		H;							 /* Observation matrix; it is defined globally within the state-vector variable
												because it is used in two different computation modules: position & velocity*/
	S_pvtPropagData	pvtPropag;				 /* Propagated pvt data															*/
} SLSData;


/******************************************/
/******************************************/
/*										  */
/* AIDING AND VIRTUAL TRACKING STRUCTURES */
/*										  */
/******************************************/
/******************************************/

/************************************/
/* Data related to virtual tracking */
/************************************/
typedef struct
{
	int			deltaCodeVector[1000];	   /* Vector for additional code-delay sweeping, to extend the range for VT search		   */
	int			deltaCodeCounter[MAXNUMSAT];
	double		hotX[3];				   /* "Hot" reference position for virtual tracking										   */
	double		hotV[3];				   /* "Hot" reference velocity for virtual tracking										   */
	int			pvtHotMs;				   /* Receiver time (in ms) for latest estimate of reference position for virtual tracking */
	int			vtSearchThSamples;		   /* Upper threshold for code sweeping, for virtual tracking. It depends on PVT age	   */
	clkStruct	clkData;				   /* Structure for receiver clock estimates											   */

}T_VtData;


/************************************************************/
/* Structures that hold the Aid data					    */
/************************************************************/
typedef struct
{
	gpstimestruct		refTime;				 /* Reference Time structure (TOW and WN)							 */
	SLSData				lsData;					 /* Least Square output data structure								 */
        SLSData				lsDataPrev;				 /* Previous step Least Square output data structure								 */
	int					pvtStatus;				 /* 0: not available; 1 = approx position (warm);					 */
												 /* 2: accurate position (for VT) but no clock: VT not possible		 */
												 /* 3: accurate position + clock: VT possible						 */
	double				attQuat[4];				 // attitude quaternion
	GPSEPHEMSTRUCT_D	gpsAlmEph[MAXNUMSAT];    /* Mixture of almanacs and ephemeris (best possible among them).   */
	GALEPHEMSTRUCT_INAV galAlmEph[MAXNUMSAT];	 /* Mixture of almanacs and ephemeris (best possible among them).   */
	double				gpsAlmEphPosCov;		 // GPS almanac/ephemerides accuracy
	double				galAlmEphPosCov;		 // GPS almanac/ephemerides accuracy
	double				gpsAlmEphVelCov;		 // GPS almanac/ephemerides accuracy
	double				galAlmEphVelCov;		 // GPS almanac/ephemerides accuracy
} T_AidData;


/************************************************************/
/* Structures that hold the channel related data parameters */
/************************************************************/
typedef struct
{
	gpstimestruct		refTime;				 /* Reference Time structure (TOW and WN)							 */
	SUserSV				refPosWarm;				 /* Reference position in WGS84 [x,y,x].							 */
	double				refAttQuat[4];			//attitude quaternion
	int					pvtAcqStatus;			 /* 0: not available; 1 = approx position (warm);					 */
												 /* 2: accurate position (for VT) but no clock: VT not possible		 */
												 /* 3: accurate position + clock: VT possible						 */
	iustruc				ionoUtc;				 /* Ionospheric & UTC information									 */

	almstruc_scaled		alm[MAXNUMSAT];			  /* Almanac structure												 */

	GPSEPHEMSTRUCT_D	gpsEph[MAXNUMSAT];		  /* GPS Broadcast Ephemeris structure                               */
	GPSEPHEMSTRUCT_D	gpsAlmEph[MAXNUMSAT];     /* Mixture of almanacs and ephemeris (best possible among them).   */
	GPSEPHEMSTRUCT_D	gpsEphME[12][MAXNUMSAT];  /* GPS Broadcast Ephemeris structure								 */
	GALEPHEMSTRUCT_INAV galEphME[12][MAXNUMSAT];  /* GAL Broadcast Ephemeris structure								 */

	int					rawIode[MAXNUMSAT];
	int					rawIodc[MAXNUMSAT];

	GALEPHEMSTRUCT_INAV galEph[MAXNUMSAT];
	GALEPHEMSTRUCT_INAV galEphTmp[MAXNUMSAT];
	GALEPHEMSTRUCT_INAV galAlmEph[MAXNUMSAT];	  /* Mixture of almanacs and ephemeris (best possible among them).   */
	GalIonoUTC_INAV     IonoUTC[MAXNUMSAT];

	T_VtData			vtData;					  /* Structure of virtual-tracking related data						 */

} T_DAid;


/************************************/
/************************************/
/*									*/
/*		SATELLITE MANAGEMENT		*/
/*									*/
/* LISTS OF CHANNELS AND SATELLITES */
/*									*/
/************************************/
/************************************/

/***********************************/
/* List structure for channel-size */
/***********************************/
typedef struct
{
	int N;                     /* Number of channels either in tracking or in virtual tracking */
	int chList[MAXNUMCHAN];    /* List of channels either in tracking or in virtual tracking   */
	int listIndex[MAXNUMCHAN]; /* Index of each channel                                        */
	int i;                     /* Auxiliar counter                                             */

} T_ListChan;

///*************************************/
///* List structure for all satellites */
///*************************************/
//typedef struct
//{
//	int  N;                    /* Number of satellites in the queue.                 */
//	int  svnList[MAXNUMSAT];   /* List of PRNs in the queue.                         */
//	int  listIndex[MAXNUMSAT]; /* listSat.SVN[listIndex[svn-1]] = svn                */
//	int  nextElement;          /* referred to SVN list [0 to N-1]                    */
//	int  i;                    /* Auxiliar counter                                   */
//
//} T_ListSat;

/********************************************************************************************/
/* Structure to store most-recent acceptable frequency estimation, for acquisition purposes */
/********************************************************************************************/
typedef struct
{
	float   freq;     /* Frequency (Hz)         */
	int     ms;       /* Receiver time (in ms)  */
	float   CLI;      /* Carrier-Lock Indicator */

} T_FreqEst;

/*************************************/
/* Cell for FFT-Acq on ONE satellite */
/*************************************/
typedef struct
{
	// Searching-range parameters
	int        bins2Search;           /* # max frequency bins to search for each satellite.     */
	int        binCounter;            /* Bin index in bins2Search.                              */

	// Priori information
	double     almFreq;               /* Almanac-estimated frequency for each satellite.        */

	// Elevation (degrees)
	double     elev;                  /* Elevation (degrees)                                    */

	// A-posteriori estimates
	int        codeSamples;           /* code delay estimated by acq [samples]                  */
	double     dopplerFreqHz;         /* Doppler freq estimated by acq [translated to Hz]       */
	double     codeFreqCps;           /* Doppler freq estimated by ACQ (translated to chip/s)	*/
	double     carrierFreqHz;         /* Freq estimated by ACQ [translated to Hz]: Doppler + IF */

	// Acquisition time
    int        acqMs;                 /* Acquisition when the channel acquired.                 */
	T_FreqEst  lastGoodFreq;          /* Structure to store latest suitable frequency estimate  */

} T_DataAcqFft;


/*********************************************************************/
/* Satellite manager structure. Contains lists and variables to		 */
/* organize the status of the satellites from several points of view.*/
/* In the case of dual GPS+GAL, there are two instances				 */
/*********************************************************************/
typedef struct
{
	int          lastVisibComp;                  /* Last time when visibility was checked (in receiver seconds)		  */
	double       initialTow;                     /* Initial time of week.											  */
	T_ListSat	 allowedSats;					 /* List of allowed satellites */
	T_ListSat	 allowedSlots;					 /* List of allowed slots	   */
	T_ListSat    fftAcqVisibleQueue;             /* Queue of satellites (expected to be visible) for Fft Acquisition. */
	T_ListSat    fftAcqUnknownQueue;             /* Queue of satellites (unknown visibility) for Fft Acquisition.     */
	int          counterSwitchFftAcq;            /* Counter for modulating the assignement between the different
                                                    cateogories for Fft Acquisition                                   */

	T_DataAcqFft acqFftSat[MAXNUMSAT];           /* Structure of data for FFT acquisition (all satellites)			  */
	T_ListSat    vtQueue;                        /* Queue of satellites for virtual-tracking acquisition.			  */
	T_ListSat    nonVisible;                     /* List of non visible satellites.									  */
	T_ListSat    unhealthy;                      /* List of unhealthy satellites.									  */

	int          satProcessingStatus[MAXNUMSAT]; /* 0 = FftAcqUnknownQueue;											  */
												 /* 2 = FftAcqVisibleQueue;  3 = FftAcqOngoing;					      */
                                                 /* 4 = vtQueue;             5 = virtualTracking;				  	  */
                                                 /* 6 = noSyncTracking;      7 = syncTracking;						  */

	int          satVisibilityStatus[MAXNUMSAT]; /* 0 = Not visible;    1 = Unknown;     2 = Predetected;			  */
                                                 /* 3 = Visible;        4 = Unhealthy;   5 = High elevation;		  */

	int          fftAcqFlag;                     /* 1 if FFT-based acquisition still needed; 0 otherwise.			  */

	int          acqFactor;                      /* Parameter for controlling FFT ACQ intensity						  */

    int			 sbfFlag[MAXNUMSAT];			 /* Flag to indicate that at least one (any) subframe has been decoded for this SVN.*/

	int			 svi2ch[MAXNUMSAT];				 /* Correspondence between channel number (in *Ch) and SVN index
													(if it is in ANY tracking mode; otherwise: -1).					   */

	int			 sloti2ch[MAXNUMSAT];			 /* Correspondence between channel number (in *Ch) and slot index
													(if it is in ANY tracking mode; otherwise: -1).
													This makes sense in GLONASS; for GPS and GAL, svi2ch would suffice */

	T_ListChan	 vtChannels;           /* List of channels (in *Ch) currently in virtual tracking.						*/

} T_SatManager;

/**************************/
/**************************/
/*						  */
/* MEASUREMENT STRUCTURES */
/*						  */
/**************************/
/**************************/

/******************************************/
/* Cell of measurements for ONE satellite */
/******************************************/
typedef struct
{
	/* Observables */
	double PR;                         /* Pseudorange (m)							*/
	double CP;                         /* Carrier phase (cycles)					*/
	double D;                          /* Doppler (Hz)								*/
	double CN0;                        /* C/No (dB-Hz)								*/
	double CLI;                        /* Carrier-Phase lock indicator				*/
	int	   N0;						   /* To control the value of integer ambiguity */

	/* Validity flags for current epoch */
	int    validPR;                    /* Code validity flag for current epoch		*/
	int    validCP;					   /* Carrier validity flag for current epoch	*/

	double deltaTimeSec;			   /* DeltaTimeSec = to solve code ambiguity	*/

} SMeasPerSat;

/********************************************************/
/* Array of measurements for all satellites				*/
/* In the case of dual GPS+GAL, there are two instances */
/********************************************************/
typedef struct
{
	int			  NM;                   /* Number of Satellites														  */
	int			  SVN[MAXNUMSAT];       /* SVN of Satellites														  */
	int			  NM_garlic;

	SMeasPerSat   Meas[MAXNUMSAT];		/* Vector of measurement structures (for all satellites)					  */

	int			  assistanceStatusFlag; /* Flag to show assistance status											  */
	int			  assistanceEpochsTh;   /* Number of epochs between assistancePre and assistancePost (for convergence)*/

} S_MeasProcess;

/************************************************/
/* Structure to manage time stamp for one epoch */
/************************************************/
typedef struct
{
	int measBlock;				/* Counter: ms iteration when next measurement and position is programmed	  */
	int	measSample;				/* Sample (within the measurement block) to which the measurement is referred */
	int deltaMeasSample;		/* Estimated drift of the measurement sample between two epochs				  */
	int timeStampFlag;			/* Initially = 0, but turns to 1 as soon as time stamp has been computed	  */
	int timeSynchronizedFlag; 	/* Flag to show that time has been synchronized (at least one SBF decoded)	  */

	gpstimestruct	gpsTime;		/* GPS time structure														  */
	caltimestruct	calDateTime;	/* Calendar date and time													  */

} S_TimeStamp;


/*******************************/
/*******************************/
/*							   */
/* LOOK-UP TABLES FOR TRACKING */
/*							   */
/*******************************/
/*******************************/

/***************************************************/
/* Structure that stores the code and carrier LUTs.*/
/***************************************************/
typedef struct
{
   int	  nWordsPerBlock;							/* Number of 32-bit integer words to cover one block of samples */

   // Code LUT array.
   double         minusCodeTh;						/* Negative of codeTh (-codeTh); just for convenience.			*/
#ifdef MEMORY_ALLOCATION   
   double         *codeVectorChips;					/* Array of possible code values.								*/
#else   
   double codeVectorChips[NBINS_CODE];
#endif
   // Carrier LUT array.
   int            carrierLutSize;					/* # possible carrier values.									*/
   double         carrierTh;						/* Threshold to change carrier.									*/
   double         minusCarrierTh;					/* Threshold to change carrier.									*/
#ifdef MEMORY_ALLOCATION   
   double         *carrierVector;					/* Array of carrier frequency values.							*/
   unsigned int	  **cos;
   unsigned int	  **sin;
   
   unsigned int   ***prompt;						/* Prompt.	*/	
   unsigned int   ***prompt2;
   unsigned int   ***nZerosEmlMagn;					/* Number of zeros in the EML Magnitude.			 */
   unsigned int   ***Pdata;					/* Prompt. data when pilot											 */
   unsigned int   ***VEarly;						/* Very Early for Galileo											 */
   unsigned int   ***VLate;							/* Very Late for Galileo	 */   
#else
    double		  carrierVector[CARRIER_LENGTH];					/* Array of possible carrier values.  */
    unsigned int	cos[NBINS_CARRIER][1000];
    unsigned int	sin[NBINS_CARRIER][1000];
    
    unsigned int    prompt[40][NBINS_CODE][1000];
    unsigned int    prompt2[40][NBINS_CODE][1000];
    unsigned int    nZerosEmlMagn[40][NBINS_CODE][1000];
    unsigned int    Pdata[40][NBINS_CODE][1000];
    unsigned int    VEarly[40][NBINS_CODE][1000];
    unsigned int    VLate[40][NBINS_CODE][1000];
#endif

   int			  nSamplesChipDelay;
#ifdef MEMORY_ALLOCATION
   unsigned char  *onesLUT;							/* Look-up table that stores
   the # 1s in an element. */
#else
    unsigned char onesLUT[2048];
#endif

} trackingLUT_t;


/***************************************/
/***************************************/
/*									   */
/* CORRELATION MANAGEMENT FOR TRACKING */
/*									   */
/***************************************/
/***************************************/

/***********************************************/
/* Structure with proper index to the signals  */
/* involved in correlation: input and replicas */
/***********************************************/
typedef struct
{
	// Index to input signal in the buffer.
	int    bitStart;       /* Bit (i.e. sample within bitwise struct) where tracking starts.*/
	int	   wordStart;      /* Word (into bitwise struct) where tracking starts.				*/

	//Index to local replicas in the LUTs.
	int	   codeIndex;      /* Index of the code replicas (P, E & L) in the LUTs				*/
	int	   carrierIndex;   /* Index of the carrier replicas (sin & cos) in the LUTs			*/
	double carrierFreq;	   /* Carrier frequency being used (consistent with index)			*/

	sampleIndex_t *outIndex_P,			 /* Pointer to sample in the buffer.														  */
						 *outSize_P;			 /* Size of one code period in the buffer.													  */
}T_CorrAuxIndex;

/***************************************************************************/
/* Type for a complex number in double format, used for correlation result.*/
/***************************************************************************/
typedef struct
{
   int  I;  /* Real part */
   int  Q;  /* Imag part */
} complex_td_int;



/***************************************************************************/
/* Type for a complex number in double format, used for correlation result.*/
/***************************************************************************/
typedef struct
{
   double  I;  /* Real part */
   double  Q;  /* Imag part */
} complex_td;

/********************************/
/* Correlation-result structure */
/********************************/
typedef struct
{
	complex_td		PCorr;		 /* Prompt correlation result (instantaneous, for one iteration)*/
	complex_td_int          PdataCorr;
	int			corr_best[3];
	complex_td		EMLCorr;		 /* EML correlation result (instantaneous, for one iteration)				    */
	complex_td		PAccumPLL;	 /* Accumulated Prompt correlation (for PLL, accumulation over nPll iterations) */
	complex_td		PAccumDLL;    /* Accumulated Prompt correlation (for DLL, accumulation over nDll iterations) */
	complex_td		EMLAccumCode; /* Accumulatd EML correlation (for DLL, accumulation over nDll iterations)		*/
	complex_td		ECorr;		 /* Early correlation result (instantaneous, for one iteration)					*/
	complex_td		LCorr;		 /* Early correlation result (instantaneous, for one iteration)					*/
        complex_td_int          VECorr;		 /* Early correlation result (instantaneous, for one iteration)					*/
	complex_td_int          VLCorr;		 /* Early correlation result (instantaneous, for one iteration)					*/
} T_CorrResults;

/***********************/
/***********************/
/*					   */
/* LOOP-FILTER RELATED */
/*					   */
/***********************/
/***********************/

/********************************/
/* Filter-coefficient structure */
/********************************/
typedef struct
{
	// Loop coefficients.
	double k1;
	double k2;
	double k3;

} T_TckFtrStruct;


/***********************************************************************/
/* Battery of filter coefficients for the different status of tracking */
/***********************************************************************/
typedef struct
{
	// Bit already synchronized (accumulated correlations).
	T_TckFtrStruct dllFtrCoeffTckSync;
	T_TckFtrStruct pllFtrCoeffTckSync;
	T_TckFtrStruct dllFtrCoeffTckSync_GAL;
	T_TckFtrStruct pllFtrCoeffTckSync_GAL;

	// During bit synchronization (single correlations).
	T_TckFtrStruct dllFtrCoeffTckNoSync;
	T_TckFtrStruct pllFtrCoeffTckNoSync;
} T_FilterData;


/***********************************/
/***********************************/
/*								   */
/* TRACKING AND AUXILIAR PROCESSES */
/* (SYNCDECOD, LOCK...)			   */
/*								   */
/***********************************/
/***********************************/

/***********************************/
/* Tracking structure per channel. */
/* Variables to manage DLL and PLL.*/
/***********************************/
typedef struct
{
	int				roundedSample;				/* Rounded sample, closest to estimated code border					  */

	/* PLL */
	int				pllCnt;						/* Iteration counter for the PLL									  */
	int				nPll;						/* Integer number of ms for accumulation in PLL tracking			  */
	double			carrierErrorCycles;			/* Carrier error (output of the DLL discriminator)					  */
	double			accumDopplerPhaseCycles;	/* Accumulated Doppler Phase (from carrier), in Cycles				  */
	int				freqTckFlag;				/* Flag about type of loop for Doppler estimation. 0 = FLL; 1 = PLL	  */
	int				NcoTckFlag;					/* Flag about NCO update. 0 = FLL; 1 = PLL	  */
	double			deltaFreqFll_v[50];			/* */
	int				fllCnt;						/* */
	int				fllCntUsed;					// Used as the FLL averaging (20 is for 20ms averaging
	int				fllCntUsedConv;				// Used for the FLL converging (how many 20 ms used to converge)
	double			prePhaseRadiansFll;
	double			deltaFreqAccumHz;
	double			dopplerFreqAcqHz;
	double			carrierFreqAcqHz;

	T_TckFtrStruct	pllFilter;					/* PLL Filter coefficients											  */
	T_TckFtrStruct	dllFilter;					/* DLL Filter coefficients											  */

	double			carrierFreqHz;				/* Carrier frequency estimated by the PLL (IF included), Hz			  */
	double			dopplerHz;					/* Doppler frequency estimated by the PLL (IF excluded), Hz			  */
	double			dopplerRateHzps;			/* Doppler rate estimated by the PLL, Hz/s							  */
	double			deltaCarrierPhaseCycles;	/* PLL-Doppler accumulation over one code period (= phase, cycles)
												   (includes IF)													  */
	double			deltaCodePhaseChips;		/* DLL-Doppler accumulation over one code period (= delay, chips)	  */
	double			deltaCodePhaseChipsPLL;		/* PLL-Doppler accumulation over one code period (= delay, chips)	  */
	double			carrierPhaseCycles;			/* Accumulated carrier phase (Doppler + IF) in cycles				  */
	int				validCarrier;				/* Validity flag for carrier tracking estimate						  */
	int				msgValidatedCpFlag;			/* Flag to show that half-ambiguity is solved (preamble found)		  */
	int				pllTypeFlag;				/* 0 = ATAN  (default);
												   1 = ATAN2 (when so configured and wipe-off is possible)			  */
	double			pllAccumTime;				/* Accumulation time for PLL = code period * nPll					  */

	/* DLL */
	int				dllCnt;						/* Iteration counter for the DLL									  */
	double			codeErrorChips;				/* Code error (output of the DLL discriminator), in chips			  */
	int				nDll;						/* Integer number of ms for accumulation in DLL tracking			  */
	int				nDllLock;
	double			codeFreqCps;				/* Frequency estimated by DLL (Doppler alone, no CODE RATE), chip/s	  */
	double			fractionalCodePhaseChips;   /* Distance from code border to rounded sample in the block (in chips)*/
	double			codeFreqKalmanCps;			/* Frequency feedback from the Kalman PVT (if possible).			  */
	double			rangeKalmanMeters;			/* Range estimated from Kalman PVT (if possible).					  */
	int				validCode;					/* Validity flag for code tracking estimate							  */
	int				resetFlag;					/* Flag to indicate a jump to adjacent block:
																-1 = backward; +1 = forward; 0 = no jump			  */
    int				tckStartMs;					/* Receiver time (in ms) when current channel started tracking        */
    int				resetCpMs;					/* Number of ms (block...) when CP has been reset (to set  CP value)  */
	int				extraPeriodCounter;
    int				vtRecovery;					/* Flag to indicate recovery of a satellite through virtual tracking  */
	int				vtRecoveryMs;				/* Iteration ("ms") when VT is successsful and tracking is recovered  */
	int				bitSycnMs;					/* Iteration ("ms") when Bit synchronization is achieved.
												   After 2 seconds from it, loop filter gets narrower				  */
	int			    toBeForcedLockFlag;			/*  It is used as a way to force loss of lock. Default = 0.
													If '1', indicates that the channel must be declared "lost"
													at the time of next measurement.								  */
	double			dllAccumTime;				/* Accumulation time for DLL = code period * nDll					  */

	int				galValidationFlag;
	int				galConvergenceFlag;
	/*int				chIndexGalVector[2];
	/*int				mainGalChannelFlag;*/
	int syncMs;

} T_DTck;


/****************************************/
/* Lock-detector structure per channel: */
/* only used after bit synchronization  */
/****************************************/
typedef struct
{
	int			cntLck;					/* Counter for the lock iterations							 */
	int			cnoCnt;					/* Counter for the CNO lock detector						 */

	double		CN0;					/* CN0 estimate (every ms iteration)						 */
	double		CLI;					/* Phase lock estimate (every ms iteration)					 */

	double		NPk;	/* Normalized power used in lock detection					 */
	double		cliVectork;				/* Carrier-Lock Indicator vector							 */

	double		WBP_k;			/* Power computed over single correlations (one code period) */
	double		NBPpow_k;		/* Power computed over accumulated correlations				 */
	complex_td	NBP_k;	/* Complex ower (I,Q) computed over accumulated correlations */

	double		NPAccum;			/* Normalized-Power average									 */
	double		cliAccum;				/* Carrier-Lock Indicator average							 */

	T_FreqEst	lastGoodFreq;			/* Most-recent estimate of acceptable frequency,
										   to be used as Doppler assitance for reacquisition		 */

	double		accumI;
	double		accumQ;

	double		cn0Gal;
	int			cn0GalCnt;
	int			lckCheckNow;	// flag indicating that lock check can be now performed (whole range of sample code was checked)
	//double		CN0_all[50];	// consecutive CN0 used to compare different sample code checks in the VT
	double		vtSearchResults[50][3]; // [:][1]:	ms of the setting
										// [:][2]:	sample code set
										// [:][2]:	ms of the settingconsecutive CN0 used to compare different sample code checks in the VT	
	int			finalVtFlag;

} T_DLck;

/********************************************************/
/* Synchro & Decod structure per channel, related to	*/
/* the process of bit synchronization and decoding.		*/
/* Each constellation shows significant specificities	*/
/* It has been decided to keep a common structure, with */
/* all the variables, which can be used or not.			*/
/********************************************************/
typedef struct
{
	int				  status;					  /* Bit-synchronization status for the channel:

														0 = NOT_SYNCHRO
														1 = BIT_SYNCHRO
														2 = FRAME_SYNCHRO */

	/* BitSync() data (DURING synchronization process) */
	complex_td 		  promptPre;				  /* Correlation value for previous iteration, for bit syncrho process	   */
	int				  transCntr[20];			  /* Array of transition counters for each ms, during bit synchro		   */
	int				  bestSum;					  /* Maximum sum value: best of the 20 bins within a bit				   */
	int				  bestBin;					  /* Bin with the maximum sum up										   */
	int				  bitMs;					  /* Counter of ms within one bit										   */
	int				  localCpInvertionFlag;
	int				  subcode_gal[25];		 /* Galileo subcode non synchronised										*/
	int				  subcode_ref[25];
	/* ONCE synchronized*/
	unsigned int	  dataBit;					  /* Pseudosymbol (estimated during the accumulated interval)			   */

	/* Subframe and message data */
	unsigned long     wordBuffer[GPS_FRAME_SIZE]; /* Buffer for 10 GPS words.											   */
	unsigned short    bitCounter;
	unsigned short    wordCounter;				  /* 0 to 9  for 1 subframe												   */

	/* GLONASS */
	unsigned int	  string;					  /* String of 30 bits, used during the synchronization of GLONASS message */
	unsigned int	  stringCnt;
	unsigned int	  frameCnt;
	int				  numberSuccessTimeMarks;
	int				  timeMarkMs[2];

	signed int		  timeMarkHeaderSeq[30];
	int				  subBitCntMeander;

	int				  cnt;

	unsigned int	  bitCnt;

	/* GALILEO */
	unsigned int	  dataSymbol;
	unsigned char	  galSubframe[250];
	unsigned char     galMessage[120];
	int				  cnt2;
	int				  writeSymbolFlag;
        
        signed int		  Psign;

} T_DSynDecod;


typedef struct T_ChData T_ChData;

/*****************************************************/
/* Basic cell for management of ONE tracking channel */
/*****************************************************/
typedef struct
{
	T_CorrAuxIndex	corrAuxIndex;		/* Store index for correlations, related to the input and replica signal	*/
	T_CorrResults   corrResults;		/* Store correlation results (Prompt, EML, instantaneous and accumulated	*/
	T_DTck			Tck;                /* Tracking channel data.													*/
	T_DLck			Lck;                /* Lock channel data.														*/
	T_DSynDecod		SynDec;             /* Synchronization/decod channel data.										*/
	int				chStatus;           /* Channel status.															*/
	int				chIndex;			/* Channel index			 												*/

	int				SVN;                /* Current SVN assigned to channel.											*/
	int				svnIndex;           /* SVN-1 (index for access to SVN lists)									*/
	int				SVN_true;
	int				svnValidFlag;

	int				slot;				/* Slot (frequency) assigned to channel (relevant in GLONASS mode)			*/
	int				slotIndex;		    /* slotIndex = slot - 1														*/

	int				slotIndexCarrierLut;/* Index of the freq slot in the carrier LUTs: carrierLutAllSlots[slotIndexCarrierLut]
										   In CDMA systems (GPS & GAL), slotIndexCarrierLut = 0.
										   In GLONASS (FDMA), there is only one PRN code and slotIndexCarrierLut = slotIndex.*/
	int				core_index;

	sampleIndex_t outIndex;			 /* Pointer to sample in the buffer.														  */
	//					 outSize;			 /* Size of one code period in the buffer.													  */

	int chCounterMs;
	/* Pointers to the three structures. This is useful in the case of dual GPS+GAL, where there are difference instances of   */
	/* genSysData, satManag and MeasProcess (for each system). In this way, each channel has access to the required magnitudes,*/
	/* regardless of its system (GPS or GAL)																				   */
	T_GenericSystemData	*genSysData_P;
	T_SatManager		*satManag_P;
	S_MeasProcess		*MeasProcess_P;
	trackingLUT_t		*tckLUT_P;		 /* Look-Up Tables for tracking.													   */
	T_FilterData		*filterData_P;
	T_ChData			*chData_P;

} T_Channel;

/***************************************************************/
/* Array of channels for one constellation (GPS or GLO or GAL) */
/* In the case of dual GPS+GAL, there are two instances		   */
/***************************************************************/
struct T_ChData
{
#ifdef MEMORY_ALLOCATION       
	T_Channel  *Ch; /* This is an array of channels. The number of channels is configurable */
#else
    T_Channel  Ch[MAXNUMCHAN]; /* This is an array of channels. The number of channels is configurable */
#endif
	T_ListChan freeChannels;         /* List of channels (in *Ch) available for tracking or virtual tracking.			*/
};

/**************************************************************************************/
/**************************************************************************************/
/*																					  */
/* SUPERSTRUCTURE THAT MANAGES ALL MAGNITUDES FOR ONE CONSTELLATION (GPS, GLO or GAL) */
/* IN THE CASE OF DUAL GPS+GAL, THERE ARE TWO INSTANCES TO THIS STRUCTURE:			  */
/*				GPS -> supportData;		GAL -> supportDataGal						  */
/*																					  */
/**************************************************************************************/
/**************************************************************************************/

typedef struct
{
	//struct acquisitionChannel acqChannel;	 /* Acquisition channel structure. */
#ifdef MEMORY_ALLOCATION
	S_acqAux			 *acqAux_P;
#else
     S_acqAux			 acqAux_P[NACQCHAN];
#endif
    
    struct complex_t  prnExpFftCLut[MAXNUMSAT][I_FFT_LENGTH]; // FFT acqusition LUT.
     
	T_GenericSystemData	 *genSysData_P; 	 /* Pointer to generic-system structure (data and constants dependent of the specific system).*/
	T_SatManager		 *satManag_P;		 /* Pointer to management structure.														  */
	S_MeasProcess		 *MeasProcess_P;	 /* Pointer to vector of measurement structures.											  */
#ifdef MEMORY_ALLOCATION
	T_FilterData		 *filterData_P;
#else
    T_FilterData		 filterData_P[1];
#endif
	trackingLUT_t		 tckLUT;			 /* Look-Up Tables for tracking.															  */
	sampleIndex_t		outIndex,			 /* Pointer to sample in the buffer.														  */
						 outSize;			 /* Size of one code period in the buffer.													  */

	int					 chIndexList[MAXNUMSAT];

}T_supportData;

/**************************************************************************************/
/**************************************************************************************/
/*																					  */
/* Structure to manage the multicore sattus											  */
/*																					  */
/**************************************************************************************/
/**************************************************************************************/
typedef struct
{
    int status;
    int init;
    int activityprev;
    int activity;
    int nchan;

}S_Core;

/******************/
/* Core status */
/******************/
typedef enum init_status
{
   START_ACQ1,            // 0 Start task
   START_TCK1,
   START_TASK,
   DONE,			 // 1 Init done
} init_status;

/******************/
/* Core status */
/******************/
typedef enum core_activity
{
   NONE,                        // 0 None - Init
   ACQ,				// 1 Acquisition 
   TCK,				// 2 Tracking
   MAIN,			// 3 Main and IO
} core_activity;

/*************************************/
/* Status of the core channel */
/*************************************/
typedef enum core_status 
{ 
  FREE_C,		// 0 FREE
  BUSY_C,		// 1 BUSY
  BLOCKED_C,	        // 2 Blocked
} core_status;

 
/***************/
/***************/
/*			   */
/* PVT RELATED */
/*			   */
/***************/
/***************/

/*************************************************/
/* Structure for PVT process computed within SRX */
/*************************************************/
typedef struct
{
	gpstimestruct	gpsTime;			/* GPS time structure													  */
	caltimestruct	calDateTime;		/* Calendar date and time												  */

	CPreProMeas		ppMeas;             /* Pre-processed measurement structure for current epoch				  */
	CPreProMeas		ppMeasPre;          /* Pre-processed measurement structure for previous epoch				  */

	S_Tropo			tropoData;          /* Troposheric-related data												  */

	SSatData		SatData[MAXNUMSAT]; /* Satellite state vector												  */
	SLSData			lsData;             /* PVT results															  */
        SLSData			lsDataPrev;             /* PVT results															  */
	S_pvtEkfData	ekfData;			/* EKF data structure*/

	int				NM;                 /* Number of Satellites													  */
	int				SVN[MAXNUMSAT];     /* SVN of Satellites													  */
	int			    systemFlagVector[MAXNUMSAT];

	int				pvtTypeFlag;		/* 0: SINGLE SYSTEM; 1: DUAL SYSTEM (TWO CLOCKS)						  */

        double                          firstPvtTow;		// TOW of the first computed PVT, used for the EKF init
	int				firstPvtFlag;       /* First-Time PVT is computed (just for trace purposes)					  */
	int				firstGalPvtFlag;    /* First-Time PVT is computed (just for trace purposes)					  */
	int				firstMeasFlag;      /* First-Time Meas is computed											  */
	int				smoPvtFlag;

} S_PVT_Process;

/***********************************/
/* Common data = "cajon de sastre" */
/***********************************/
typedef struct
{
	int			executionLengthMs;
	int         nSamples;               /* Number of samples read													  */
	double		metersPerSample;
	double      ts;                     /* Sampling time (in seconds)												  */
	double	    cnoThNatural;
	double	    vtCnoThNatural;
	double		lossCnoThNatural;
	int			vtMsResetTh;

	int			loopCoefFlag;

	double		sampFreqIQ2;
	int			nSamplesIQ2;

	double		pvtWeight;

	int			counterMs;
	int			lengthSamples;        /* Buffer size (in samples).													*/
    int			lengthMs;			  /* Buffer length (in ms).														*/
    int			lengthWords;
	int			msBack;				  /* Ms back in buffer where ACQ and TCK process.								*/

} T_ComData;

#ifndef _rtems_app
/******************/
/******************/
/*				  */
/* TRHEAD RELATED */
/*				  */
/******************/
/******************/
typedef struct
{
	/** Server Port **/
	int serverPort;
	/** Server Host */
	char ServerHost [1024];

	/* Synchronization structures */
	#ifdef LINUX
	pthread_mutex_t   TCPLockMutex;
	#else
	CRITICAL_SECTION  TCPLockMutex;
	#endif
	/* I/O Buffer Length */
	int  TCPbufferLen;

	/* SISNeT Server is slightly different from the Observation and Nagigation servers */
	/* Therefore, this flag is used to identify the server type: 0.-Obs/Nav; 1.-SISNeT */
	int sisnetGeoPrn;

	/* Flag to indicate whether a client is or not connected to the server */
	char clientConnected;

   /* Semaphore to signal when data is ready to be sent through the TCP */
   HANDLE  hDataReadyToSend;

} T_Thread_Data;
#endif

// Structure that stores info per satellite.
typedef struct
{
   short valid[3];             /* Subframe valid data [subframes 1,2 and 3].*/
   short iodc;                 /* Issue of SF 1 data in SF1words[].			*/
   short iode2;                /* Issue of SF 2 data in SF2words[].			*/
   short iode3;                /* Issue of SF # data in SF3words[].			*/

   // Unparsed message words in subframes [1,2 and 3];
   unsigned long subFrameWords[3][10];
} subframeAuxStructure;

#endif

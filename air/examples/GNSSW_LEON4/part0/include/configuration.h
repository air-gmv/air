//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  configuration.h
// Version:                  1.3
// Date (YY/MM/DD):            2008 / 11 / 01
// Component:
// Purpose:   This header file implements the interface for the parser of the
//         configuration file. It parses each line of the configuration file
//         and validates the lines.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/05/05   -   JJNC   -   R1.3_PFM_READ_LUT
//   1.2   -   2009/10/01   -   JJNC   -   R1.14_FUN_USB_AND_GENERAL
//   1.3   -   2009/12/03   -   JJNC   -   R1.17_FUN_INPUT_FORMATS
//*****************************************************************************
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <siria.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************
// Directory delimiter
#ifndef LINUX
#define DIR_DELIMITER_CH  '\\'
#else
#define DIR_DELIMITER_CH  '/'
#endif

// Minimum, maximum and default values used in the configuration parameters
#define DEF_PORT_OBS    (5000)
#define DEF_PORT_NAV    (5001)
#define DEF_PORT_EXTPVT (5555)
#define MAX_NCORRPOINTS (10)


//*****************************************************************************
//                     STRUCTURES DEFINITION
//*****************************************************************************
// Structures that hold the configuration parameters

// Configuration common parameters.
typedef struct
{
   char        sampleFile[MAX_FILE_NAME];  // Samples file name.
   int         rawSampleBytesFlag;         // Flag to indicate the format of the input sample file (raw or reversed bytes in each words).
   int         oneSamplePerByteFlag;       // Flag to indicate the format of the input sample file.
   int         simTime;                    // Total simulation time in seconds.
   double      sampFreq;                   // Sampling frequency.
   double      bandwidth;                  // Front end bandwidth.
   double      ifreq;                      // Intermediate frequency.
   double	   filterCenterFreq;		   // 0: Low-pass;   1: Bandpass.
   int		   fePllMode;				   // 0: Fractional; 1: Integer. 
   double      startTime;                  // Starting simulation time.
   E_ExecutionMode mode;                   // Execution mode (see E_ExecutionMode definition).
   float	   outputRate;                 // Rate to generate measurements and navigation (Hz).
   char		   resulPath[MAX_PATH_LEN];	   // PVT result file.
   int		   systemFlag;				   // Flag to indicate the satellite constellation (0: GPS / 1: GLO).
   int         sampleWritingRate;		   // Rate used to write the received samples into the file (in ms).
   int         splitSampleFileRate;		   // Rate used to split the received samples into different files (in minutes).
   int           onspw;
} T_CCom;

// Configuration acquisition parameters.
typedef struct
{
   float    maxFreq;					   // Maximum doppler freq to search for.
   float	freqStepCodeRatio;
   int      maxNbins;					   // For warm/ hot acq, # of bins to search before quitting.
   double   pfa;						   // Probability of False Alarm in the acquisition process.
   float    acqTh;						   // Factor to consider acquisition
   int      maxNonCohInt_GPS;				   // Maximum number of 1 ms in which to perform acquisition.+
   int      maxNonCohInt_GAL;				   // Maximum number of 1 ms in which to perform acquisition.
   int      nMedInt;					   // Number of iterations for medium freq estimation.
   int      nFineInt;					   // Number of iterations for fine freq estimation.
   int		freqRefinementFlag;			   // Flag for the frequency-refinement method (0 = FFT; 1 = FLL).
   int      acqFactor;					   // Time between acquisition calls.
   char     acqLUTFile[MAX_FILE_NAME];     // Acquisition FFT LUT file name.
   int      sp3HighElevN;				   // Number of high-elevation satellites for sp3 start.
   int      maxDopplerDrift;
} T_CFftAcq;

// Configuration aiding parameters
typedef struct
{
   int    refWn;             // Reference week number
   double refTow;            // Reference Time of Week (s)
   double refPos[3];         // Reference position in WGS84 [x,y,x].
   char   almFile[120];       // Path and name of the almanac file (YUMA format)
   char   ephFile[120];       // Path and name of the ephemeris file (Rinex nav format)
   char   galEphFile[120];    // Path and name of the GALILEO ephemeris file (Rinex nav format)
   char   sp3File[120];
   long   lastVisibCompTh;   // Time threshold for new visibility computation.
   double clkDriftAid;		 // A-priori value for the clock drift (Hz)
} T_CAid;

// Configuration lock parameters.
typedef struct
{
   int      nLock;	       // # of accumulations for lock check.
                           // narrow-band noise power estimations (obtained
                           // with M integration periods) which are combined
                           // to produce the final estimation.
   int		vtNLock;
   int      nCnoTh;        // # times to consider loss (CN0)
   float    cnoTh;         // CN0 thresholds [dBHz]
   float    vtCnoTh;       // CNO threshold for virtual tracking [dB-Hz]
   float    lossCnoTh;     // CNO threshold for declaring loss of lock
   float    cliTh;         // Carrier-Locl Indicator threshold
} T_CLck;

// Configuration management parameters.
typedef struct
{
   int      nChannels;          // Number of channels allowed in the receiver
   int      nChannelspercore;   // Number of channels allowed in the receiver
   int      nPrnFbd;            // Number of forbidden PRNs
   int      prnFbd[MAXNUMSAT];  // List of forbidden PRNs
   E_StartMode startMode;		// Acquisition mode (see E_AcquisitionMode definition).
   int		fastFirstFixFlag;	// Flag to be more agressive (1) or conservative (0) in the TTFF approach in assisted mode.
   int		enableVtFlag;
} T_CMng;

// Configuration tracking parameters.
typedef struct
{
   int        pllOrder;					 // PLL order
   int        dllOrder;					 // DLL order
   int        decimateFactor;			 // Decimation factor to apply in correlations.
   float      pllBw;					 // PLL bandwidth.
   float      dllBw;					 // DLL bandwidth.
   int        nDll;						 // Number of accumulations for the DLL
										 // before updating the code loop ; equivalent
										 // to pre-detection integration time
   int        nPll;						 // Number of accumulations for the PLL
										 // integrations before updating the carrier
										 // loop. This parameter is equivalent to
										 // pre-detection integration time
   int        nDll_GAL;						 // Number of accumulations for the DLL
										 // before updating the code loop ; equivalent
										 // to pre-detection integration time
   int        nPll_GAL;						 // Number of accumulations for the PLL
										 // integrations before updating the carrier
										 // loop. This parameter is equivalent to
										 // pre-detection integration time
   float      chipDelay;				 // Correlators space between E and L replicas : [chips]
   int        resolutionCarrier;		 // Resolution of the carrier vector (Hz).
   char       tckLUTFile[MAX_FILE_NAME]; // Sine and cosine tracking LUT.
   int        timeStampOutFlag;			 // Flag to activate  (1 vs. 0) file printing of time stamps.
   int        nCorrPoints;				 // Total number of E-L correlation PAIRS (1 = only default E-L pair).
   int        cpInitializationFlag;		 // Flag to indicate that CP needs to be initialized at SBF.
   int        msgBitWipeOffMode;		 // WIPE-OFF MODE:
										 //					0 : NO WIPE OFF (ATAN).
   										 //					1 : WIPE OFF (ATAN2 WHEN POSSIBLE).

   int        nSuperFrames;				 // Each of them contains 37500 bits (750 s).

} T_CTck;

// Parameter structure
typedef   struct
{
   char         nmeaTCPinfo[80];          // NMEA output TCP/IP information.
   double       elevMask;                 // Elevation masking angle.
   int          smoLength;                // Number of epochs for Smoothing .
   int          ionoCorrection;           // Ionosphere correction.
   int          tropoCorrection;          // Troposphere correction.
   int		cn0outFlag;
   int		integerTimeMarkFlag;	   // Flag to force integer time mark (1 vs. 0)
   int		pvtVtFlag;				   // Flag to activate feedback from external PVT (Garlic) to VT.
   int		clkSteeringFlag;		   // Flag to control clock management: 1 = Clock steering; 0 = Free running.
   int		isbSteeringFlag;		   // Flag to control ISB management: 1 = ISB steering; 0 = Free running.
} T_CPvt;

// Parameter structure
typedef   struct
{
    int		useEkf;						// if 1 use EKF, if 0 do not use EKF
    int		spaceUser;					// if 1 user is in orbit, if 0 user is on Earth
    double	deltaT;						// EKF time step [s]
    double	mahaThr;					// Mahalanobis check threshold
    double	measSigmaPr;				// Pseudorange measurements sigma [m]
    double	measSigmaDp;				// Doppler measurements sigma [Hz]
    double	initStateSigmaPos;			// Init state Position sigma xyz [m]
    double	initStateSigmaVel;			// Init state Velocity sigma xyz [m/s]
    double	initStateSigmaClkBias;		// Init state clock bias sigma [m]
    double	initStateSigmaClkGalGpsBias;// Init state clock GAL-GPS bias sigma [m]
    double	initStateSigmaClkDrift;		// Init state clock drift sigma [m/s]
    double	stateNoiseCovPos;			// State Noise Position cov xyz [m^2]
    double	stateNoiseCovVel;			// State Noise Velocity cov xyz [m^2/s^2]
    double	stateNoiseCovClkBias;		// State Noise Clock bias cov [m^2]
    double	stateNoiseCovClkGalGpsBias;	// State Noise Clock GAL-GPS bias cov [m^2]
    double	stateNoiseCovClkDrift;		// State Noise Clock drift cov [m^2/s^2]
} T_CEkf;

// Configuration main structure.
typedef struct
{
   T_CFftAcq   FftAcq;
   T_CCom      Com;
   T_CLck      Lck;
   T_CMng      Mng;
   T_CAid      Aid;
   T_CTck      Tck;
   T_CPvt      Pvt;
   T_CEkf      Ekf;
} T_Config;

//*****************************************************************************
//                     GLOBAL VARIABLES
//*****************************************************************************

//*****************************************************************************
//                     FUNCTIONS INTERFACE
//*****************************************************************************
int       CNFMain(char *configFileName); // Configuration main function.

void   initializeConfig(void);
void   finalizeDefaultConfiguration(void);    // Free configuration resources for default

void      setDefaultValues(void);        // Set default values to structures.

void   save_to_file_Configuration(void);
void   printConfiguration(void);
#endif

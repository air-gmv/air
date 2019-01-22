#include <configuration.h>

//#include <trace.h>
#include <parserCNF.h>

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <interface.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************
//#define WRITE_CONFIGURATION						// Flag to write configuration values.
#define NUMBER_SECTIONS     9						// Number of different sections.

// Paramters array or matrix zise.
#define   REFPOS_LENGTH     3						// REFPOS parameter array size.

// Sections name.
#define ACQ_SECTION          "ACQ"					// Acquisition section name.
#define INT_SECTION          "INT"					// Interference section name.
#define AID_SECTION          "AID"					// Aid section name.
#define COM_SECTION          "COM"					// Common section name.
#define LCK_SECTION          "LCK"					// Lock section name.
#define MNG_SECTION          "MNG"					// Management section name.
#define PVT_SECTION          "PVT"					// Navigation section name.
#define EKF_SECTION          "EKF"					// Navigation section name
#define TCK_SECTION          "TCK"					// Tracking section name.

// Common section parameters name.
#define SAMPLEFILE           "SAMPLEFILE"			// Samples file name.
#define RAWSAMPLEBYTESFLAG   "RAWSAMPLEBYTESFLAG"   // Flag to indicate the format of the input sample file (raw or reversed bytes in each words).
#define ONESAMPLEPERBYTEFLAG "ONESAMPLEPERBYTEFLAG" // Flag to indicate the format of the input sample file.
#define SIMTIME              "SIMTIME"				// Running Time.
#define SAMPFREQ             "SAMPFREQ"				// Sampling frequency.
#define BANDWIDTH            "BANDWIDTH"			// Front end bandwidth.
#define IFREQ                "IFREQ"				// Intermediate frequency.
#define FILTERCENTERFREQ	 "FILTERCENTERFREQ"		//  0: Low-pass;  1: Band-pass.
#define FEPLLMODE			 "FEPLLMODE"			// 0: Fractional; 1: Integer.
#define STARTTIME            "STARTTIME"			// Starting simulation time.
#define DECIMATIONFACTOR     "DECIMATIONFACTOR"		// Decimation factor to apply to input samples.
#define MODE                 "MODE"					// Execute SRX in postprocess mode.
#define TOOLMODE			 "TOOLMODE"				// Tool mod: Interference Detector (0); Interference Detector + Receiver (1); Receiver only (2).
#define OUTPUTRATE           "OUTPUTRATE"			// Measurements and navigation rate.
#define RESULTPATH           "RESULTPATH"			// Result file.
#define SYSTEMFLAG			 "SYSTEMFLAG"			// Flag to indicate satellite constellation (0: GPS  / 1: GLO / 2: GAL).
#define SAMPLEWRITINGRATE    "SAMPLEWRITINGRATE"	// Rate used to write the received samples into the file (in ms).
#define SPLITSAMPLEFILERATE  "SPLITSAMPLEFILERATE"	// Rate used to split the received samples into different files (in minutes).

// Acquisition section parameters name.
#define MAXFREQ              "MAXFREQ"				// Maximum doppler freq.
#define FREQSTEPCODERATIO	 "FREQSTEPCODERATIO"
#define MAXNBINS             "MAXNBINS"				// Max # bins before quitting search
#define PFA                  "PFA"					// Probability of False Alarm in the acquisition process.
#define ACQTH                "ACQTH"				// Factor to consider acquisition successful
#define NACQINT              "NACQINT"				// Number of ms for acquisition.
#define MAXNONCOHINTGPS      "MAXNONCOHINT_GPS"			// Maximum number of ms for non coherent accumulation.
#define MAXNONCOHINTGAL      "MAXNONCOHINT_GAL"			// Maximum number of ms for non coherent accumulation.
#define NMEDINT              "NMEDINT"				// Number of ms for med freq.
#define NFINEINT             "NFINEINT"				// Number of ms for fine freq.
#define FREQREFINEMENTFLAG   "FREQREFINEMENTFLAG"	// Flag for the frequency-refinement method (0 = FFT; 1 = FLL).
#define ACQFACTOR            "ACQFACTOR"			// Time between acquisition calls.
#define ACQLUTFILE           "ACQLUTFILE"			// Acquisition FFT LUT file name.
#define SP3HIGHELEVN         "SP3HIGHELEVN"

// Aid section parameters name.
#define REFWN                "REFWN"				// Reference week number.
#define REFTOW               "REFTOW"				// Reference Time of Week (s).
#define REFPOS               "REFPOS"				// Reference position in WGS84 [x,y,x].
#define ALMFILE              "ALMFILE"				// Path and name of the almanac file (YUMA format).
#define EPHFILE              "EPHFILE"				// Path and name of the ephemeris file (Rinex nav format).
#define GALEPHFILE           "GALEPHFILE"			// Path and name of the ephemeris file (Rinex nav format).
#define SP3FILE              "SP3FILE"				// Path and name of the sp3 file (Rinex nav format).
#define LASTVISIBCOMPTH      "LASTVISIBCOMPTH"		// Time threshold for new visibility computation.
#define CLKDRIFTAID			 "CLKDRIFTAID"			// A-priori value for the clock drift (Hz)

// Lock section parameters name.
#define PRIMCODEMCO          "PRIMCODEMCO"			// Number of code integrations.
#define NOISEBANDKCO         "NOISEBANDKCO"			// Number of wide-band noise power (code)
#define PRIMCODEMCA          "PRIMCODEMCA"			// Number of carrier integrations.
#define NOISEBANDKCA         "NOISEBANDKCA"			// Number of wide-band noise power (carrier).
#define NCNOTH               "NCNOTH"				// # times to consider loss (CN0).
#define NC2PHITH             "NC2PHITH"				// # times to consider loss (cos2phi).
#define CNOTH                "CNOTH"				// CN0 thresholds [dBHz].
#define C2PHITH              "C2PHITH"				// Cos(2phi) thresholds.
#define VTCNOTH              "VTCNOTH"				// CN0 threshold for virtual tracking [dBHz]
#define LOSSCNOTH            "LOSSCNOTH"			// CNO threshold for declaring loss of lock
#define	NLOCK				 "NLOCK"
#define	VTNLOCK				 "VTNLOCK"

// Management section parameters name.
#define NCHANNELS            "NCHANNELS"			// Number of channels.
#define NPRNFORCED           "NPRNFORCED"			// Number of forced PRNs for acquisition.
#define PRNFORCED            "PRNFORCED"			// List of forced PRNs for acquisition.
#define NPRNFBD              "NPRNFBD"				// Number of forbidden PRNs.
#define PRNFBD               "PRNFBD"				// List of forbidden PRNs.
#define STARTMODE	         "STARTMODE"			// Start mode.
#define FASTFIRSTFIXFLAG	 "FASTFIRSTFIXFLAG"		// Flag to be more agressive (1) or conservative (0) in the TTFF approach in assisted mode.
#define ENABLEVTFLAG		 "ENABLEVTFLAG"

// Navigation section parameters name.
#define ELEVMASK             "ELEVMASK"				// Elevation masking angle.
#define SMOLENGTH            "SMOLENGTH"			// Number of epochs for Smoothing.
#define IONOCORRECTION       "IONOCORRECTION"		// Ionosphere correction.
#define TROPOCORRECTION      "TROPOCORRECTION"		// Troposphere correction.
#define NMEATCPINFO          "NMEATCPINFO"			// NMEA output TCP IP and port information.
#define CN0OUTFLAG			 "CN0OUTFLAG"			// Flag to trace C/No for used satellite (screen and trace file).
#define INTEGERTIMEMARKFLAG	 "INTEGERTIMEMARKFLAG"  // Flag to force integer time mark (1 vs. 0)
#define PVTVTFLAG			 "PVTVTFLAG"			// Flag to activate feedback from external PVT (Garlic) to VT.
#define CLKSTEERINGFLAG		 "CLKSTEERINGFLAG"		// Flag to control clock management: 1 = Clock steering; 0 = Free running.
#define ISBSTEERINGFLAG		 "ISBSTEERINGFLAG"		// Flag to control ISB management: 1 = ISB steering; 0 = Free running.

// Navigation EKF section parameters name.
#define	USEEKF					"USEEKF"				// if 1 use EKF, if 0 do not use EKF
#define	SPACEUSER				"SPACEUSER"			// if 1 user is in orbit, if 0 user is on Earth
#define	DELTAT					"DELTAT"
#define	MAHATHR					"MAHATHR"
#define	MEASSIGMAPR				"MEASSIGMAPR"
#define	MEASSIGMADP				"MEASSIGMADP"
#define	INITSTATESIGMAPOS		"INITSTATESIGMAPOS"
#define	INITSTATESIGMAVEL		"INITSTATESIGMAVEL"
#define	INITSTATESIGMACLKBIAS	"INITSTATESIGMACLKBIAS"
#define	INITSTATESIGMACLKGALGPSBIAS "INITSTATESIGMACLKGALGPSBIAS"
#define	INITSTATESIGMACLKDRIFT	"INITSTATESIGMACLKDRIFT"
#define	STATENOISECOVPOS		"STATENOISECOVPOS"
#define	STATENOISECOVVEL		"STATENOISECOVVEL"
#define	STATENOISECOVCLKBIAS	"STATENOISECOVCLKBIAS"
#define	STATENOISECOVCLKGALGPSBIAS "STATENOISECOVCLKGALGPSBIAS"
#define	STATENOISECOVCLKDRIFT	"STATENOISECOVCLKDRIFT"

// Tracking section parameters name.
#define DECIMATEFACTOR       "DECIMATEFACTOR"		// Decimation factor to apply in correlation.
#define PLLORDER             "PLLORDER"				// PLL order per channel.
#define DLLORDER             "DLLORDER"				// DLL order per channel.
#define PLLBW				 "PLLBW"				// Carrier loop filter bandwidth per channel.
#define DLLBW				 "DLLBW"				// Code loop filter bandwidth per channel.
#define NDLL	             "NDLL"					// Integrations before updating the code.
#define NPLL	             "NPLL"					// Integrations before updating the carrier.
#define NDLL_GAL	         "NDLL_GAL"					// Integrations before updating the code.
#define NPLL_GAL	         "NPLL_GAL"					// Integrations before updating the carrier.
#define RESOLUTIONCARRIER    "RESOLUTIONCARRIER"	// Resolution of the carrier LUTs.
#define CHIPDELAY            "CHIPDELAY"			// Correlators space between E and L.
#define TCKLUTFILE           "TCKLUTFILE"			// Sine and cosine tracking LUT.
#define TCKLOOPOUTFLAG		 "TCKLOOPOUTFLAG"		// Flag to activate printing tracking-loop data.
#define TIMESTAMPOUTFLAG	 "TIMESTAMPOUTFLAG"		// Flag to activate  (1 vs. 0) file printing of time stamps.
#define NCORRPOINTS			 "NCORRPOINTS"			// Total number of E-L correlation PAIRS (1 = only default E-L pair).
#define CPINITIALIZATIONFLAG "CPINITIALIZATIONFLAG" // Flag to indicate that CP needs to be initialized at SBF.
#define MSGBITWIPEOFFMODE	 "MSGBITWIPEOFFMODE"	//			0 : NO WIPE OFF (ATAN).
   													//			1 : WIPE OFF (ATAN2 WHEN POSSIBLE).

#define NSUPERFRAMES		 "NSUPERFRAMES"		// Each of them contains 37500 bits (750 s).

//*****************************************************************************
//                        STRUCTURES DEFINITION
//*****************************************************************************
// Struct for int parameters.
struct intParameter
{
   int      min;
   int      max;
   int      def;
};

// Struct for float parameters.
struct floatParameter
{
   float   min;
   float   max;
   float   def;
};

// Struct for double parameters.
struct doubleParameter
{
   double  min;
   double  max;
   double  def;
};

// Struct for long parameters.
struct longParameter
{
   long   min;
   long   max;
   long   def;
};

// Struct for char parameters.
struct charParameter
{
   char   name[MAX_FILE_NAME];
};

// Common parameters validation structure.
struct commonParameters
{
   struct intParameter    simTime;
   struct doubleParameter sampFreq;
   struct intParameter	  rawSampleBytesFlag;
   struct intParameter	  oneSamplePerByteFlag;
   struct doubleParameter bandwidth;
   struct doubleParameter iFreq;
   struct doubleParameter filterCenterFreq;
   struct intParameter	  fePllMode;
   struct charParameter   samplesFile;
   struct doubleParameter startTime;
   struct intParameter    decimationFactor;
   struct intParameter    mode;
   struct floatParameter  outputRate;
   struct charParameter   resulPath;
   struct intParameter	  systemFlag;
   struct intParameter	  sampleWritingRate;
   struct intParameter	  splitSampleFileRate;
   struct intParameter    onspw;
} commonParams;//*commonParams=NULL;

// Acquisition parameters validation structure.
struct acquisitionParameters
{
   struct floatParameter	maxFreq;
   struct floatParameter	freqStepCodeRatio;
   struct intParameter		maxNbins;
   struct doubleParameter	pfa;
   struct floatParameter	acqTh;
   struct intParameter		nAcqInt;
   struct intParameter		maxNonCohInt_GPS;
   struct intParameter		maxNonCohInt_GAL;
   struct intParameter		nMedInt;
   struct intParameter		nFineInt;
   struct intParameter		freqRefinementFlag;
   struct intParameter		acqFactor;
   struct charParameter		acqLUTFile;
   struct intParameter		sp3HighElevN;
   struct intParameter		maxDopplerDrift; 
   
} acquisitionParams;//*acquisitionParams=NULL;

// Aid parameters validation structure.
struct aidParameters
{
   struct intParameter    refWn;
   struct doubleParameter refTow;
   struct doubleParameter refPos;
   struct charParameter   almFile;
   struct charParameter   ephFile;
   struct charParameter   galEphFile;
   struct charParameter   sp3File;
   struct longParameter   lastVisibCompTh;
   struct doubleParameter clkDriftAid;
} aidParams;//*aidParams=NULL;

// Lock parameters validation structure.
struct lockParameters
{
   struct intParameter    nLock;
   struct intParameter	  vtNLock;
   struct intParameter    nCnoTh;
   struct intParameter    nC2PhiTh;
   struct floatParameter  cnoTh;
   struct floatParameter  c2PhiTh;
   struct floatParameter  vtCnoTh;
   struct floatParameter  lossCnoTh;
} lockParams;//*lockParams=NULL;

// Management parameters validation structure.
struct managementParameters
{
   struct intParameter     nChannels;
   struct intParameter     nPrnForced;
   struct intParameter     prnForced;
   struct intParameter     nPrnFbd;
   struct intParameter     prnFbd;
   struct intParameter     startMode;
   struct intParameter     fastFirstFixFlag;
   struct intParameter	   enableVtFlag;
} managementParams;//*managementParams=NULL;

// Navigation parameters validation structure.
struct pvtParameters
{
   struct doubleParameter elevMask;
   struct intParameter    smoLength;
   struct intParameter    ionoCorrection;
   struct intParameter    tropoCorrection;
   struct charParameter   nmeaTCPinfo;
   struct intParameter	  cn0outFlag;
   struct intParameter	  integerTimeMarkFlag;
   struct intParameter	  pvtVtFlag;
   struct intParameter	  clkSteeringFlag;
   struct intParameter	  isbSteeringFlag;
} pvtParams;//*pvtParams=NULL;

// Navigation parameters validation structure.
struct ekfParameters
{
	struct intParameter			useEkf;
	struct intParameter			spaceUser;
	struct doubleParameter		deltaT;
	struct doubleParameter		mahaThr;
	struct doubleParameter		measSigmaPr;
	struct doubleParameter		measSigmaDp;
	struct doubleParameter		initStateSigmaPos;
	struct doubleParameter		initStateSigmaVel;
	struct doubleParameter		initStateSigmaClkBias;
	struct doubleParameter		initStateSigmaClkGalGpsBias;
	struct doubleParameter		initStateSigmaClkDrift;
	struct doubleParameter		stateNoiseCovPos;
	struct doubleParameter		stateNoiseCovVel;
	struct doubleParameter		stateNoiseCovClkBias;
	struct doubleParameter		stateNoiseCovClkGalGpsBias;
	struct doubleParameter		stateNoiseCovClkDrift;
} ekfParams;

// Tracking parameters validation structure.
struct trackingParameters
{
   struct intParameter     dllOrder;
   struct floatParameter   dllBw;
   struct floatParameter   chipDelay;
   struct intParameter     nDll;
   struct intParameter     nDll_GAL;
   struct intParameter     pllOrder;
   struct intParameter     decimateFactor;
   struct floatParameter   pllBw;
   struct intParameter     nPll;
   struct intParameter     nPll_GAL;
   struct intParameter     resolutionCarrier;
   struct charParameter    tckLUTFile;
   struct intParameter	   tckLoopOutFlag;
   struct intParameter	   timeStampOutFlag;
   struct intParameter	   nCorrPoints;
   struct intParameter	   cpInitializationFlag;
   struct doubleParameter  gloHwBias;
   struct intParameter	   msgBitWipeOffMode;
   struct intParameter	   nSuperFrames;
} trackingParams;//*trackingParams=NULL;


//*****************************************************************************
//                        GLOBAL INTERNAL VARIABLES
//*****************************************************************************
int  alreadySet=FALSE;               // Flag to free configuration memory.
char sectionName[MAX_LINE_LENGTH][NUMBER_SECTIONS] = {COM_SECTION, // Sections names.
                                                      ACQ_SECTION,
                                                      INT_SECTION,
                                                      AID_SECTION,
                                                      TCK_SECTION,
                                                      MNG_SECTION,
                                                      PVT_SECTION,
						      EKF_SECTION,
                                                      LCK_SECTION};


//*****************************************************************************
//                        GLOBAL EXTERNAL VARIABLES
//*****************************************************************************
extern T_Config Config;

//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
//void   initializeConfig(void);                // Initialize values.
void   setConfigurationChannelsData(void);    // Set configuration channel depending data.
void   startSection(void);                    // Check if line is a valid header section.
void   endSection(void);                      // Check if line is a valid end section.
void   validateParameter(void);               // Call validateXXX parameter.
void   validateCOMParameter(void);            // Validate common parameter.
void   validateACQParameter(void);            // Validate acquisition parameter.
void   validateAIDParameter(void);            // Validate aiding parameter.
void   validateLCKParameter(void);            // Validate lock parameter.
void   validateMNGParameter(void);            // Validate management parameter.
void   validatePVTParameter(void);            // Validate navigation parameter.
void   validateEKFParameter(void);            // Validate navigation EKF parameter.
void   validateTCKParameter(void);            // Validate tracking parameter.
void   finalizeDefaultConfiguration(void);    // Free configuration resources for default
                                              // values during initialization.
int    LoadInputFilesIntoConfig(char *inputFilesFileName); // Load input files into the configuration
void   setInputFileStartMode(int siAssistedModeFlag, int siSp3AssistanceFlag);

#ifndef LINUX
void   FindSampleFile(void);
char   FindFirstNonEmptyFile(char *szSearchStr, char *szFirstNonEmptyFile);
#endif

//*****************************************************************************
//                        PUBLIC FUNCTION BODIES
//*****************************************************************************
//*****************************************************************************
//   Name:         CNFMain
//   Purpose:    main function of the configuration unit. Open the configuration
//               file if exists and parses it validating the parameters and
//               values. If it does not exist apply default values. Free
//               resources before end.
//   Arguments:
//      I:         configFileName         configuration file name,
//      I/0:       NONE
//      O:         NONE
//      Global:      parserStatus         status of the file parser.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int CNFMain(char *configFileName)
{
   int     ret = EXIT_OK;          // Function return value.
   int k = 0;
   // Initialize configuration unit.
   initializeConfig();

   // Set default values to configuration structures.
   setDefaultValues();

   // If configuration file supplied by the user apply new configuration.
#ifndef PCMonitor
   if ((ret = parserCNFInit(configFileName)) == EXIT_OK)
   {
      // Read line from file.
      readLine();

      // Check if line read is a valid header section.
      startSection();

      // Loop to read the file.
      while (parserStatusOk())
      {
         // Read line from file.
         readLine();

         // Check if line read is a valid end section.
         endSection();

         // Loop to read parameters until valid end of section.
         while ((parserStatus == NEXT_PARAM_END) &&
               (parserStatusOk()))
         {
            // Check if parameter is valid and update it if so.
            validateParameter();

            // Read line from file.
            readLine();

            // Check if line read is a valid end section.
            endSection();
         }

         // Read line.
         readLine();

         // Check if line read is a valid header section.
         startSection();
      }
   }
#else
GetCNF();
#endif

   // Check if an error ocurred while parsing the configuration file.
   if (getFailStatus() != FALSE)
   {
      ret = EXIT_ER;
   }

   // Free resoruces.
   finalizeDefaultConfiguration();

   // Check warnings while parsing configuration file.
   checkWarning();

   // Close configuration file.
   finalizeParserCNF();

   // In processing mode check the configured input sample file string in case a search is needed
   if (Config.Com.mode == POSTPROCESS)
   {
#ifndef _rtems_app       
#ifndef LINUX
      FindSampleFile();	// Falta arreglar esto para poder procesar ficheros consecutivos en linux
#endif
#endif
   }

   // Check whether the SYSTEMFLAG value matches with the compilation flag.
#ifndef COMPILE_RECORD_ONLY
#if SYSFLAG != 1
#else
   if (Config.Com.systemFlag != 1)
   {
      ret = EXIT_ER;
   }
#endif
#endif


#ifdef COMPILE_RECORD_ONLY
   // When compiled only for recording measurements ensure that the execution mode is USB_RECORD_ONLY
  // if ((Config.Com.mode != USB_RECORD_ONLY) && (Config.Com.mode != SOCKET_RECORD_ONLY))
  // {
  //    ret = EXIT_ER;
  // }
#endif

#ifdef   WRITE_CONFIGURATION
   // Write configuration values read.
   printConfiguration();
#endif

   return (ret);
}

//*****************************************************************************
//   Name:       LoadInputFilesIntoConfig
//   Purpose:    Opens the InputFiles file if exists and parses it validating
//               the parameters and values.
//               Closes the file before end.
//   Arguments:
//      I:         configFileName         configuration file name,
//      I/0:       NONE
//      O:         NONE
//      Global:      parserStatus         status of the file parser.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int    LoadInputFilesIntoConfig(char *inputFilesFileName)
{
   int    ret	    = EXIT_OK;          // Function return value.
   FILE  *fdInFiles = NULL;
   char   szParameter[MAX_LINE_LENGTH];  // Parameter name.
   char   szValue[MAX_LINE_LENGTH];      // Parameter value.
   char   szLine[MAX_LINE_LENGTH];       // Read line buffer.
   char   chSeparator='=';               // Parameters separator (char).
   char   szSeparator[2]="=";            // Parameters separator (string).
   char  *pchValue;
   char  *pchAux;
   int    i = 0;
   int    j = 0;
   char   bInitCopy;
   int    siAssistedModeFlag  = -1;
   int    siSp3AssistanceFlag = -1;

   // Open InputFiles file in reading mode.
   if ((fdInFiles = fopen(inputFilesFileName,"rb")) == NULL)
   {
      // Error opening samples file.
      ret = EXIT_ER;
   }

   // Read the content of the InputFiles file
   if (ret == EXIT_OK)
   {
      // Initialize separators.
      chSeparator = '=';
      strcpy( szSeparator, "=");
      while ((fgets(szLine, MAX_LINE_LENGTH, fdInFiles)) != NULL)
      {
         // Loop to skip comments lines.
         if (szLine[0] != '%')
         {
            // Check the presence of the separator in the line
            pchAux = strstr( szLine, szSeparator);
            if (pchAux != NULL)
            {
               // Clean parameter variables
               memset( szParameter, 0, MAX_LINE_LENGTH);
               memset( szValue,     0, MAX_LINE_LENGTH);

               // Get parameter name
               ////strncpy(szParameter,szLine,pchAux-szLine);
               ////szParameter[pchAux-szLine] = '\0';
               i=0;
               j=0;
               while (szLine[i] != chSeparator)
               {
                  if (szLine[i] != ' ')
                  {
                     szParameter[j] = szLine[i];
                     j++;
                  }
                  i++;
               }
               szParameter[j] = '\0';

               // Get parameter value
               pchValue = pchAux+1;
               i = 0;
               j = 0;
               bInitCopy = FALSE;
               while ((pchValue[i] != '%')&&(pchValue[i] != '\0'))
               {
                  if (pchValue[i] != ' ')
                  {
                     bInitCopy = TRUE;
                  }
                  if ((bInitCopy == TRUE) && (pchValue[i] != '\'') && (pchValue[i] != '\r') && (pchValue[i] != '\n'))
                  {
                     szValue[j] = pchValue[i];
                     j++;
                  }
                  i++;
               }

               // Check and Load Parameters
               if ((strcmp(szParameter, "SampleFile{1}") == 0) ||
                   (strcmp(szParameter, "SampleFile") == 0))
               {
                  strcpy(currentParameter, SAMPLEFILE);
                  strcpy(currentValue,     szValue);
                  validateCOMParameter();
               }
               else if (strcmp(szParameter, "WaveFeFlag") == 0)
               {
                  strcpy(currentParameter, ONESAMPLEPERBYTEFLAG);
                  strcpy(currentValue,     szValue);
                  validateCOMParameter();
                  strcpy(currentParameter, RAWSAMPLEBYTESFLAG);
                  strcpy(currentValue,     szValue);
                  validateCOMParameter();
               }
               else if (strcmp(szParameter, "AssistedModeFlag") == 0)
               {

                  if ((strstr(szValue,"true") != NULL) ||
                      (strstr(szValue,"True") != NULL) ||
                      (strstr(szValue,"TRUE") != NULL) ||
                      (strstr(szValue,"1") != NULL) ||
                      (strstr(szValue,"5") != NULL) ||
                      (strstr(szValue,"6") != NULL))
                  {
                     siAssistedModeFlag = 1;
                  }
                  else
                  {
                     siAssistedModeFlag = 0;
                  }
                  setInputFileStartMode(siAssistedModeFlag, siSp3AssistanceFlag);
               }
               else if ((strcmp(szParameter, "Assistance") == 0) ||
                        (strcmp(szParameter, "AssistanceGps") == 0) ||
                        (strcmp(szParameter, "AssistanceGlo") == 0) )
               {
                  if (strstr(szValue, ".sp3") != NULL)
                  {
                     siSp3AssistanceFlag = 1;
                     strcpy(currentParameter, SP3FILE);
                     strcpy(currentValue,     szValue);
                     validateAIDParameter();
                     strcpy(currentValue,     "1");
                     validateAIDParameter();
                     setInputFileStartMode(siAssistedModeFlag, siSp3AssistanceFlag);
                  }
                  else
                  {
                     if ( ((Config.Com.systemFlag == 0) && (strstr(strstr(szValue+strlen(szValue)-4,"."), "n") != NULL)))
                     {
                        siSp3AssistanceFlag = 0;
                        strcpy(currentParameter, EPHFILE);
                        strcpy(currentValue,     szValue);
                        validateAIDParameter();
                        strcpy(currentValue,     "0");
                        validateAIDParameter();
                        setInputFileStartMode(siAssistedModeFlag, siSp3AssistanceFlag);
                     }
                  }
               }
               else if (strcmp(szParameter, "RefPos") == 0)
               {
                  strcpy(currentParameter, REFPOS);
                  strcpy(currentValue,     szValue);
                  validateAIDParameter();
               }
/* #########################################################___DELETE_START___##################################################################### */
    /*           else if (strcmp(szParameter, "MACalib") == 0)
               {
                  strcpy(currentParameter, szParameter);
                  sprintf(currentValue, "%s%c%s", gszBaseDirForRelativePaths, DIR_DELIMITER_CH, szValue);
                  // Not used yet
               }
               else if (strcmp(szParameter, "WnTOW") == 0)
               {
                  strcpy(currentParameter, szParameter);
                  sprintf(currentValue, "%s%c%s", gszBaseDirForRelativePaths, DIR_DELIMITER_CH, szValue);
                  // Not used yet
               }*/
/* ###########################################################___DELETE_END___##################################################################### */
               else if (strcmp(szParameter, "StartTime") == 0)
               {
                  strcpy(currentParameter, STARTTIME);
                  strcpy(currentValue,     szValue);
                  validateCOMParameter();
               }
               else if (strcmp(szParameter, "SimTime") == 0)
               {
                  strcpy(currentParameter, SIMTIME);
                  strcpy(currentValue,     szValue);
                  validateCOMParameter();
               }
            }
         }
      }
   }

   // Close configuration file if open.
   if (fdInFiles != NULL)
   {
      // Close configuration file.
      fclose(fdInFiles);
      // Set reference to NULL.
      fdInFiles = NULL;
   }

   return (ret);
}


//*****************************************************************************
//   Name:       setInputFileStartMode
//   Purpose:    Sets the STARTMODE configuration parameter according to the
//               parameters provided by the InputFile.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      Config.Mng.startMode
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//*****************************************************************************
void   setInputFileStartMode(int siAssistedModeFlag, int siSp3AssistanceFlag)
{
   if (siAssistedModeFlag == 0)
   {
      strcpy(currentParameter, STARTMODE);
      strcpy(currentValue,     "0");
      validateMNGParameter();
   }

   if (siAssistedModeFlag == 1)
   {
      if (siSp3AssistanceFlag == 0)
      {
         strcpy(currentParameter, STARTMODE);
         strcpy(currentValue,     "5");
         validateMNGParameter();
      }
      if (siSp3AssistanceFlag == 1)
      {
         strcpy(currentParameter, STARTMODE);
         strcpy(currentValue,     "6");
         validateMNGParameter();
      }
   }

   return;
}

#ifndef _rtems_app
#ifndef LINUX
//*****************************************************************************
//   Name:       FindSampleFile
//   Purpose:    Checks if the input sample file is a search string and
//               finds the first non-empty file matching the string.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      Config.Com.sampleFile
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//*****************************************************************************
void FindSampleFile(void)
{
   char *pchFindAux   = NULL;
   char *pchPrefixAux = NULL;
   char *pchSamplesAux= NULL;
   char *pchStereoAux = NULL;
   char  szTailAux[MAX_FILE_NAME];
   char bFileFound;

   // Checking samples file name.
   if ((Config.Com.sampleFile != NULL) && (strcmp( Config.Com.sampleFile, "") != 0))
   {
      // Check whether the file needs to be found
      pchFindAux = strstr(Config.Com.sampleFile, "*");
      if (pchFindAux != NULL)
      {
         // Copy the tail of the input string
         szTailAux[0] = '\0';
         sprintf(szTailAux, "%s", pchFindAux+1);

         // Find the first non-empty file according to the configured search string.
         bFileFound = FindFirstNonEmptyFile(Config.Com.sampleFile, Config.Com.sampleFile);
      }
   }

   return;
}

//*****************************************************************************
//   Name:       FindFirstNonEmptyFile
//   Purpose:    Finds the first non-empty file matching the input string.
//   Arguments:
//      I:         szSearchStr
//      I/0:       NONE
//      O:         szFirstNonEmptyFile
//      Global:      NONE
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//   Return:       char   A non empty file has been found
//*****************************************************************************
char FindFirstNonEmptyFile(char *szSearchStr, char *szFirstNonEmptyFile)
{
   char bSearchRes = FALSE;
   char szSearchStrAux[MAX_FILE_NAME];
   char bFoundFile;
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char szDrive[MAX_PATH_LEN];
   char szDir[MAX_PATH_LEN];

   // Copy the search string
   strcpy(szSearchStrAux, szSearchStr);

   // Find the first non-empty file according to the search string.   
   hFind = FindFirstFile(szSearchStrAux, &ffd);
   if (INVALID_HANDLE_VALUE == hFind)
   {
      // Not enter into the while loop to check the files
      bFoundFile = TRUE;
   }
   else
   {
      // Check the files.
      bFoundFile = FALSE;
   }
   while (bFoundFile == FALSE)
   {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {

      }
      else
      {
         filesize.LowPart  = ffd.nFileSizeLow;
         filesize.HighPart = ffd.nFileSizeHigh;
			
         if (filesize.QuadPart > 0)
         {	
            bFoundFile = TRUE;
            _splitpath(szSearchStrAux, szDrive, szDir, NULL, NULL);
            sprintf(szSearchStrAux, "%s%s%s", szDrive, szDir, ffd.cFileName);
            strcpy(szFirstNonEmptyFile, szSearchStrAux);
            bSearchRes = TRUE;
         }
      }
      if (bFoundFile == FALSE)
      {
         if (FindNextFile(hFind, &ffd) == 0)
         {
            // End while loop
            bFoundFile = TRUE;
         }
      }
   }
   FindClose(hFind);

   return bSearchRes;
}
#endif
#endif
//*****************************************************************************
//   Name:         setDefaultValues
//   Purpose:      Apply default values to the configuration structures.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      Config         configuration structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
void   setDefaultValues(void)
{
   int i = 0;                  // Loop counters.

   // Setting common configuration data.
   Config.Com.rawSampleBytesFlag = commonParams.rawSampleBytesFlag.def;
   Config.Com.oneSamplePerByteFlag = commonParams.oneSamplePerByteFlag.def;
   Config.Com.simTime			   = commonParams.simTime.def;
   Config.Com.sampFreq			   = commonParams.sampFreq.def;
   Config.Com.bandwidth			   = commonParams.bandwidth.def;
   Config.Com.ifreq				   = commonParams.iFreq.def;
   Config.Com.startTime			   = commonParams.startTime.def;
   Config.Com.mode				   = (E_ExecutionMode) commonParams.mode.def;
   Config.Com.outputRate		   = commonParams.outputRate.def;
   Config.Com.filterCenterFreq     = commonParams.filterCenterFreq.def;
   Config.Com.fePllMode			   = commonParams.fePllMode.def;
   Config.Com.sampleWritingRate	   = commonParams.sampleWritingRate.def;
   Config.Com.splitSampleFileRate  = commonParams.splitSampleFileRate.def;
   Config.Com.systemFlag		   = commonParams.systemFlag.def;
   Config.Com.onspw		   = commonParams.onspw.def;

   strcpy( Config.Com.sampleFile, commonParams.samplesFile.name);

   // Setting management configuration data.
   
   Config.Mng.nChannelspercore     = round(managementParams.nChannels.def/NUMTCKTASK);
   Config.Mng.nChannels		   = Config.Mng.nChannelspercore*NUMTCKTASK;
   Config.Mng.startMode		   = (E_StartMode) managementParams.startMode.def;
   Config.Mng.nPrnFbd		   = managementParams.nPrnFbd.def;
   Config.Mng.fastFirstFixFlag     = managementParams.fastFirstFixFlag.def;
   Config.Mng.enableVtFlag	   = managementParams.enableVtFlag.def;

   // Update array of forbidden satellites.
   for (i = 0; i < Config.Mng.nPrnFbd; i++)
   {
      Config.Mng.prnFbd[i] = managementParams.prnFbd.def;
   }

   // Tracking configuration data.
   Config.Tck.chipDelay = trackingParams.chipDelay.def;

#ifndef PCMonitor
   if ((strlen(commonParams.resulPath.name)>0) && (commonParams.resulPath.name[strlen(commonParams.resulPath.name)-1] != DIR_DELIMITER_CH))
   {
      sprintf( Config.Com.resulPath, "%s%c", commonParams.resulPath.name, DIR_DELIMITER_CH);
   }
   else
   {
      sprintf( Config.Com.resulPath, "%s", commonParams.resulPath.name);
   }
#else
   sprintf( Config.Com.resulPath, ".//data//output//");
#endif
   strcpy(Config.Pvt.nmeaTCPinfo, pvtParams.nmeaTCPinfo.name);

   Config.Pvt.smoLength		      = pvtParams.smoLength.def;
   Config.Pvt.elevMask			  = pvtParams.elevMask.def;
   Config.Pvt.ionoCorrection	  = pvtParams.ionoCorrection.def;
   Config.Pvt.tropoCorrection	  = pvtParams.tropoCorrection.def;
   Config.Pvt.cn0outFlag		  = pvtParams.cn0outFlag.def;
   Config.Pvt.integerTimeMarkFlag = pvtParams.integerTimeMarkFlag.def;
   Config.Pvt.clkSteeringFlag	  = pvtParams.clkSteeringFlag.def;
   Config.Pvt.isbSteeringFlag	  = pvtParams.isbSteeringFlag.def;

      // EKF configuration data.
    Config.Ekf.useEkf		      = ekfParams.useEkf.def;
    Config.Ekf.spaceUser              = ekfParams.spaceUser.def;
    Config.Ekf.deltaT		      = ekfParams.deltaT.def;
    Config.Ekf.mahaThr                = ekfParams.mahaThr.def;
    Config.Ekf.measSigmaPr            = ekfParams.measSigmaPr.def;
    Config.Ekf.measSigmaDp            = ekfParams.measSigmaDp.def;
    Config.Ekf.initStateSigmaPos      = ekfParams.initStateSigmaPos.def;
    Config.Ekf.initStateSigmaVel      = ekfParams.initStateSigmaVel.def;
    Config.Ekf.initStateSigmaClkBias  = ekfParams.initStateSigmaClkBias.def;
    Config.Ekf.initStateSigmaClkGalGpsBias= ekfParams.initStateSigmaClkGalGpsBias.def;
    Config.Ekf.initStateSigmaClkDrift	= ekfParams.initStateSigmaClkDrift.def;
    Config.Ekf.stateNoiseCovPos			= ekfParams.stateNoiseCovPos.def;
    Config.Ekf.stateNoiseCovVel			= ekfParams.stateNoiseCovVel.def;
    Config.Ekf.stateNoiseCovClkBias		= ekfParams.stateNoiseCovClkBias.def;
    Config.Ekf.stateNoiseCovClkGalGpsBias= ekfParams.stateNoiseCovClkGalGpsBias.def;
    Config.Ekf.stateNoiseCovClkDrift		= ekfParams.stateNoiseCovClkDrift.def;
   
   // Aiding configuration data.
   Config.Aid.refWn  = aidParams.refWn.def;
   Config.Aid.refTow = aidParams.refTow.def;

   strcpy( Config.Aid.almFile, aidParams.almFile.name);
   strcpy( Config.Aid.ephFile, aidParams.ephFile.name);
   strcpy( Config.Aid.galEphFile, aidParams.galEphFile.name);
   strcpy( Config.Aid.sp3File, aidParams.sp3File.name);

   // Update array of reference positions.
   for (i = 0; i < REFPOS_LENGTH; i++)
   {
      Config.Aid.refPos[i] = aidParams.refPos.def;
   }

   Config.Aid.lastVisibCompTh = aidParams.lastVisibCompTh.def;

   // Set configuration data that depends on number of channels.
   setConfigurationChannelsData();
}

//*****************************************************************************
//                        INTERNAL FUNCTION BODIES
//*****************************************************************************
//*****************************************************************************
//   Name:         initializeConfig
//   Purpose:      Initialize unit resources.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  acquisitionParams  acquisition internal structure.
//               aidParams          aiding internal structure.
//               commonParams       common internal structure.
//               lockParams         lock internal structure.
//               managementParams   management internal structure.
//               pvtParams          navigation internal structure.
//               trackingParams     tracking internal structure.
//               alreadySet         flag for default values already initialzied.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   initializeConfig(void)
{
   alreadySet = FALSE;

   //**************************************************************
   //                  COMMON PARAMETERS
   //**************************************************************
   //commonParams = (struct commonParameters *) malloc(sizeof(struct commonParameters));

   // SAMPLEFILE parameter.
   strcpy( commonParams.samplesFile.name, "");

   // RAWSAMPLEBYTESFLAG parameter.
   commonParams.rawSampleBytesFlag.def = 0;
   commonParams.rawSampleBytesFlag.min = 0;
   commonParams.rawSampleBytesFlag.max = 1;

   // ONESAMPLEPERBYTEFLAG parameter.
   commonParams.oneSamplePerByteFlag.def = 0;
   commonParams.oneSamplePerByteFlag.min = 0;
   commonParams.oneSamplePerByteFlag.max = 1;

   // SIMTIME parameter.
   commonParams.simTime.def = 0;
   commonParams.simTime.min = 0;
   commonParams.simTime.max = INT_MAX;

   // SAMPFREQ parameter.
   commonParams.sampFreq.def = 4092000;
   commonParams.sampFreq.min = 0;
   commonParams.sampFreq.max = DBL_MAX;

   // BANDWIDTH parameter.
   commonParams.bandwidth.def = 2500000.0;
   commonParams.bandwidth.min = 0;
   commonParams.bandwidth.max = DBL_MAX;

   // IFREQ parameter.
   commonParams.iFreq.def = -2;
   commonParams.iFreq.min =-DBL_MAX;
   commonParams.iFreq.max = DBL_MAX;

   // FILTERCENTERFREQ parameter.
   commonParams.filterCenterFreq.def = 0;
   commonParams.filterCenterFreq.min = 0;
   commonParams.filterCenterFreq.max = 12000000;

   // FEPLLMODE parameter.
   commonParams.fePllMode.def = -1;
   commonParams.fePllMode.min = -1;
   commonParams.fePllMode.max = 1;

   // STARTTIME parameter.
   commonParams.startTime.def = 0;
   commonParams.startTime.min = 0;
   commonParams.startTime.max = DBL_MAX;

   // DECIMATIONFACTOR parameter.
   commonParams.decimationFactor.def = 0;
   commonParams.decimationFactor.min = 0;
   commonParams.decimationFactor.max = 7;

   // MODE parameter.
   commonParams.mode.def = 0;
   commonParams.mode.min = 0;
   commonParams.mode.max = 1;
   
   // ON SPW parameter.
   commonParams.onspw.def = 0;
   commonParams.onspw.min = 0;
   commonParams.onspw.max = 1;
   
#ifdef COMPILE_RECORD_ONLY
  // commonParams.mode.def = USB_RECORD_ONLY;
#endif

   // OUTPUTRATE parameter.
   commonParams.outputRate.def = (float) 1;
   commonParams.outputRate.min = (float) 0.1;
   commonParams.outputRate.max = FLT_MAX;

   // RESULTPATH parameter.
   strcpy(commonParams.resulPath.name, "");

   // SYSTEMFLAG parameter.
   commonParams.systemFlag.def = 0;
   commonParams.systemFlag.min =-1;
   commonParams.systemFlag.max = 2;

   // SAMPLEWRITTINGRATE parameter.v
   commonParams.sampleWritingRate.def = 1000;
   commonParams.sampleWritingRate.min = 32;
   commonParams.sampleWritingRate.max = 98208;

   // SPLITSAMPLEFILERATE parameter.
   commonParams.splitSampleFileRate.def = 0;
   commonParams.splitSampleFileRate.min = 0;
   commonParams.splitSampleFileRate.max = 60;

   //**************************************************************
   //                  ACQUISITION PARAMETERS
   //**************************************************************
  // acquisitionParams = (struct acquisitionParameters *) malloc(sizeof(struct acquisitionParameters));
 //  memory_statistic(sizeof(struct acquisitionParameters), "init fft acquisitionParams",acquisitionParams, 1);
   // MAXFREQ parameter.
   acquisitionParams.maxFreq.def = 6000.0;
   acquisitionParams.maxFreq.min = 0.0;
   acquisitionParams.maxFreq.max = 50000.0;

   // FREQSTEPCODERATIO parameter.
   acquisitionParams.freqStepCodeRatio.def = 1;
   acquisitionParams.freqStepCodeRatio.min = 0;
   acquisitionParams.freqStepCodeRatio.max = 1;

   // MAXNBINS parameter.
   acquisitionParams.maxNbins.def = 10;
   acquisitionParams.maxNbins.min = 1;
   acquisitionParams.maxNbins.max = 10;

   // PFA parameter.
   acquisitionParams.pfa.def = 1e-4;
   acquisitionParams.pfa.min = 0.0;
   acquisitionParams.pfa.max = 1.0;

   // ACQTH parameter.
   acquisitionParams.acqTh.def = (float)1.8;
   acquisitionParams.acqTh.min = 0.5;
   acquisitionParams.acqTh.max = 10.0;

   // NACQINT parameter.
   acquisitionParams.nAcqInt.def = 1;
   acquisitionParams.nAcqInt.min = 0;
   acquisitionParams.nAcqInt.max = 10;

   // MAXNONCOHINTGPS parameter.
   acquisitionParams.maxNonCohInt_GPS.def = 3;
   acquisitionParams.maxNonCohInt_GPS.min = 0;
   acquisitionParams.maxNonCohInt_GPS.max = 500;
   
   // MAXNONCOHINTGAL parameter.
   acquisitionParams.maxNonCohInt_GAL.def = 3;
   acquisitionParams.maxNonCohInt_GAL.min = 0;
   acquisitionParams.maxNonCohInt_GAL.max = 500;

   // NMEDINT parameter.
   acquisitionParams.nMedInt.def = 10;
   acquisitionParams.nMedInt.min = 0;
   acquisitionParams.nMedInt.max = 1000;

   // NFINEINT parameter.
   acquisitionParams.nFineInt.def = 10;
   acquisitionParams.nFineInt.min = 2;
   acquisitionParams.nFineInt.max = 1000;

   // FREQREFINEMENTFLAG
   acquisitionParams.freqRefinementFlag.def = 0;
   acquisitionParams.freqRefinementFlag.min = 0;
   acquisitionParams.freqRefinementFlag.max = 2;

   // ACQFACTOR parameter.
   acquisitionParams.acqFactor.def = 1;
   acquisitionParams.acqFactor.min = 0;
   acquisitionParams.acqFactor.max = INT_MAX;

   // SP3HIGHELEVN parameter.
   acquisitionParams.sp3HighElevN.def = 9;
   acquisitionParams.sp3HighElevN.min = 0;
   acquisitionParams.sp3HighElevN.max = MAXNUMCHAN;
   
   // MAXDOPPLERDRIFT parameter.
   acquisitionParams.maxDopplerDrift.def = 70;
   acquisitionParams.maxDopplerDrift.min = 10;
   acquisitionParams.maxDopplerDrift.max = 100;
   
   // ACQLUTFILE parameter.
   strcpy(acquisitionParams.acqLUTFile.name, "");

   //**************************************************************
   //                  AIDING PARAMETERS
   //**************************************************************
   //aidParams = (struct aidParameters *) malloc(sizeof(struct aidParameters));
     //memory_statistic(sizeof(struct aidParameters), "init fft aidParams",aidParams, 1);
   aidParams.refWn.def = -1;
   aidParams.refWn.min = 0;
   aidParams.refWn.max = 3000;

   aidParams.refTow.def = -1;
   aidParams.refTow.min = 0;
   aidParams.refTow.max = 604800;

   aidParams.refPos.def = 0;
   aidParams.refPos.min = -1e08;
   aidParams.refPos.max = 1e08;

   strcpy( aidParams.almFile.name, "");
   strcpy( aidParams.ephFile.name, "");
   strcpy( aidParams.galEphFile.name, "");
   strcpy( aidParams.sp3File.name, "");

   aidParams.lastVisibCompTh.def = 100;
   aidParams.lastVisibCompTh.min = 0;
   aidParams.lastVisibCompTh.max = 86400;

   aidParams.clkDriftAid.def = 0;
   aidParams.clkDriftAid.min = -100000000.0;
   aidParams.clkDriftAid.max =  100000000.0;

   //**************************************************************
   //                     LOCK PARAMETERS
   //**************************************************************
  // lockParams = (struct lockParameters *) malloc(sizeof(struct lockParameters));
     //memory_statistic(sizeof(struct lockParameters), "init fft lockParams",lockParams, 1);
   // NOISEBANDKCO parameter.
   lockParams.nLock.def = 50;
   lockParams.nLock.min = 1;
   lockParams.nLock.max = 500;

   // NOISEBANDKCO parameter.
   lockParams.vtNLock.def = 50;
   lockParams.vtNLock.min = 1;
   lockParams.vtNLock.max = 500;

   // NCNOTH parameter.
   lockParams.nCnoTh.def = 3;
   lockParams.nCnoTh.min = 1;
   lockParams.nCnoTh.max = INT_MAX;

   // NC2PHITH parameter.
   lockParams.nC2PhiTh.def = 3;
   lockParams.nC2PhiTh.min = 1;
   lockParams.nC2PhiTh.max = INT_MAX;

   // CNOTH parameter.
   lockParams.cnoTh.def = 30.0;
   lockParams.cnoTh.min = 0.0;
   lockParams.cnoTh.max = 100.0;

   // C2PHITH parameter.
   lockParams.c2PhiTh.def = (float) 0.8;
   lockParams.c2PhiTh.min = -1.0;
   lockParams.c2PhiTh.max = 1.0;

   // VTCNOTH parameter.
   lockParams.vtCnoTh.def = (float) 20;
   lockParams.vtCnoTh.min = 0;
   lockParams.vtCnoTh.max = 100;

   // LOSSCNOTH parameter.
   lockParams.lossCnoTh.def = (float) 15;
   lockParams.lossCnoTh.min = 0;
   lockParams.lossCnoTh.max = 100;

   //**************************************************************
   //                  MANAGEMENT PARAMETERS
   //**************************************************************
 //  managementParams = (struct managementParameters *) malloc(sizeof(struct managementParameters));
     //memory_statistic(sizeof(struct managementParameters), "init fft managementParams",managementParams, 1);
   // NCHANNELS parameter.
   managementParams.nChannels.def = 12;
   managementParams.nChannels.min = 5;
   managementParams.nChannels.max = 24;

   // NPRNFORCED parameter.
   managementParams.nPrnForced.def = 0;
   managementParams.nPrnForced.min = 0;
   managementParams.nPrnForced.max = 12;

   // PRNFORCED parameter.
   managementParams.prnForced.def = 0;
   managementParams.prnForced.min = 0;
   managementParams.prnForced.max = 12;

   // NPRNFBD parameter.
   managementParams.nPrnFbd.def = 0;
   managementParams.nPrnFbd.min = 0;
   managementParams.nPrnFbd.max = 32;

   // PRNFBD parameter.
   managementParams.prnFbd.def = 0;
   managementParams.prnFbd.min = 0;
   managementParams.prnFbd.max = 32;

   // STARTMODE parameter.
   managementParams.startMode.def = COLD_START_M;
   managementParams.startMode.min = COLD_START_M;
   managementParams.startMode.max = ASSISTED_SP3;

   // PRNFBD parameter.
   managementParams.fastFirstFixFlag.def = 0;
   managementParams.fastFirstFixFlag.min = 0;
   managementParams.fastFirstFixFlag.max = 1;

   // ENABLEVTFLAG parameter.
   managementParams.enableVtFlag.def = 1;
   managementParams.enableVtFlag.min = 0;
   managementParams.enableVtFlag.max = 1;

   //**************************************************************
   //                  NAVIGATION PARAMETERS
   //**************************************************************
  // pvtParams = (struct pvtParameters *) malloc(sizeof(struct pvtParameters));
  //memory_statistic(sizeof(struct pvtParameters), "init fft pvtParams",pvtParams, 1);
   // NMEATCPINFO parameter.
   strcpy(pvtParams.nmeaTCPinfo.name, "");
   
   // ELEVMASK parameter.
   pvtParams.elevMask.def = 10;
   pvtParams.elevMask.min = 0;
   pvtParams.elevMask.max = 90;

   // SMOLENGTH parameter.
   pvtParams.smoLength.def = 0;
   pvtParams.smoLength.min = 0;
   pvtParams.smoLength.max = 1000;

   // IONOCORRECTION parameter.
   pvtParams.ionoCorrection.def = 1;
   pvtParams.ionoCorrection.min = 0;
   pvtParams.ionoCorrection.max = 1;

   // TROPOCORRECTION parameter.
   pvtParams.tropoCorrection.def = 1;
   pvtParams.tropoCorrection.min = 0;
   pvtParams.tropoCorrection.max = 1;

   // CNO0OUTFLAG parameter
   pvtParams.cn0outFlag.def = 0;
   pvtParams.cn0outFlag.min = 0;
   pvtParams.cn0outFlag.max = 1;

   // INTEGERTIMEMARKFLAG
   pvtParams.integerTimeMarkFlag.def = 0;
   pvtParams.integerTimeMarkFlag.min = 0;
   pvtParams.integerTimeMarkFlag.max = 1;

   // PVTVTFLAG
   pvtParams.pvtVtFlag.def = 0;
   pvtParams.pvtVtFlag.min = 0;
   pvtParams.pvtVtFlag.max = 1;

   // CLKSTEERINGFLAG
   pvtParams.clkSteeringFlag.def = 0;
   pvtParams.clkSteeringFlag.min = 0;
   pvtParams.clkSteeringFlag.max = 1;

   // ISBSTEERINGFLAG
   pvtParams.isbSteeringFlag.def = 1;
   pvtParams.isbSteeringFlag.min = 0;
   pvtParams.isbSteeringFlag.max = 1;

   //**************************************************************
   //                  NAVIGATION EKF PARAMETERS
   //**************************************************************

   // USEEKF parameter.
   ekfParams.useEkf.def = 1;
   ekfParams.useEkf.min = 0;
   ekfParams.useEkf.max = 1;

   // SPACEUSER parameter.
   ekfParams.spaceUser.def = 1;
   ekfParams.spaceUser.min = 0;
   ekfParams.spaceUser.max = 1;

   // DELTAT parameter.
   ekfParams.deltaT.def = 1;
   ekfParams.deltaT.min = 1;
   ekfParams.deltaT.max = 10;

   //MAHATHR
   ekfParams.mahaThr.def = 5;
   ekfParams.mahaThr.min = 1;
   ekfParams.mahaThr.max = 1000;

   //MEASSIGMAPR
   ekfParams.measSigmaPr.def = 5;
   ekfParams.measSigmaPr.min = 1;
   ekfParams.measSigmaPr.max = 100;

   //MEASSIGMADP
   ekfParams.measSigmaDp.def = 0.5;
   ekfParams.measSigmaDp.min = 0.05;
   ekfParams.measSigmaDp.max = 5;

   //INITSTATESIGMAPOS
   ekfParams.initStateSigmaPos.def = 20;
   ekfParams.initStateSigmaPos.min = 0.1;
   ekfParams.initStateSigmaPos.max = 10000;

   //INITSTATESIGMAVEL
   ekfParams.initStateSigmaVel.def = 0.5;
   ekfParams.initStateSigmaVel.min = 0.01;
   ekfParams.initStateSigmaVel.max = 1000;

   //INITSTATESIGMACLKBIAS
   ekfParams.initStateSigmaClkBias.def = 10;
   ekfParams.initStateSigmaClkBias.min = 0.01;
   ekfParams.initStateSigmaClkBias.max = 1000;

   //INITSTATESIGMACLKGALGPSBIAS
   ekfParams.initStateSigmaClkGalGpsBias.def = 10;
   ekfParams.initStateSigmaClkGalGpsBias.min = 0.01;
   ekfParams.initStateSigmaClkGalGpsBias.max = 1000;

   //INITSTATESIGMACLKDRIFT
   ekfParams.initStateSigmaClkDrift.def = 1;
   ekfParams.initStateSigmaClkDrift.min = 0.001;
   ekfParams.initStateSigmaClkDrift.max = 100;

   //STATENOISECOVPOS
   ekfParams.stateNoiseCovPos.def = 1e-9;
   ekfParams.stateNoiseCovPos.min = 1e-18;
   ekfParams.stateNoiseCovPos.max = 1e0;

   //STATENOISECOVVEL
   ekfParams.stateNoiseCovVel.def = 1e-6;
   ekfParams.stateNoiseCovVel.min = 1e-18;
   ekfParams.stateNoiseCovVel.max = 1e0;

   //STATENOISECOVCLKBIAS
   ekfParams.stateNoiseCovClkBias.def = 1e-4;
   ekfParams.stateNoiseCovClkBias.min = 1e-18;
   ekfParams.stateNoiseCovClkBias.max = 1e0;

   //STATENOISECOVCLKGALGPSBIAS
   ekfParams.stateNoiseCovClkGalGpsBias.def = 1e-4;
   ekfParams.stateNoiseCovClkGalGpsBias.min = 1e-18;
   ekfParams.stateNoiseCovClkGalGpsBias.max = 1e0;

   //STATENOISECOVCLKDRIFT
   ekfParams.stateNoiseCovClkDrift.def = 1e-2;
   ekfParams.stateNoiseCovClkDrift.min = 1e-18;
   ekfParams.stateNoiseCovClkDrift.max = 1e0;

   //**************************************************************
   //                  TRACKING PARAMETERS
   //**************************************************************
   //trackingParams = (struct trackingParameters *) malloc(sizeof(struct trackingParameters));
  //memory_statistic(sizeof(struct trackingParameters), "init fft trackingParams",trackingParams, 1);
   // DLLORDER parameter.
   trackingParams.dllOrder.def = 2;
   trackingParams.dllOrder.min = 1;
   trackingParams.dllOrder.max = 3;

   // COLOOPBW parameter.
   trackingParams.dllBw.def = 1.0;
   trackingParams.dllBw.min = 0.0;
   trackingParams.dllBw.max = 1000.0;

   // CHIPDELAY parameter.
   trackingParams.chipDelay.def = (float) 1.0;
   trackingParams.chipDelay.min = (float) 0;
   trackingParams.chipDelay.max = (float) 1.0;

   // NCODEINT parameter.
   trackingParams.nDll.def = 20;
   trackingParams.nDll.min = 1;
   trackingParams.nDll.max = 20;

    // NCODEINT parameter.
   trackingParams.nDll_GAL.def = 5;
   trackingParams.nDll_GAL.min	= 1;
   trackingParams.nDll_GAL.max	= 1000;


   // PLLORDER parameter.
   trackingParams.pllOrder.def = 2;
   trackingParams.pllOrder.min = 1;
   trackingParams.pllOrder.max = 3;

   // DECIMATEFACTOR parameter.
   trackingParams.decimateFactor.def = 1;
   trackingParams.decimateFactor.min = 1;
   trackingParams.decimateFactor.max = INT_MAX;

   // CALOOPBW parameter.
   trackingParams.pllBw.def = 15.0;
   trackingParams.pllBw.min = 0.0;
   trackingParams.pllBw.max = 1000.0;

   // NPLL parameter.
   trackingParams.nPll.def = 5;
   trackingParams.nPll.min = 1;
   trackingParams.nPll.max = 20;   
   
   // NPLL parameter.
   trackingParams.nPll_GAL.def = 1;
   trackingParams.nPll_GAL.min = 1;
   trackingParams.nPll_GAL.max = 1000;

   // RESOLUTIONCARRIER parameter.
   trackingParams.resolutionCarrier.def = 20;
   trackingParams.resolutionCarrier.min = 1;
   trackingParams.resolutionCarrier.max = 2000;

   // TCKLOOPOUTFLAG parameter.
   trackingParams.tckLoopOutFlag.def = 0;
   trackingParams.tckLoopOutFlag.min = 0;
   trackingParams.tckLoopOutFlag.max = 1;

   // TIMESTAMPOUTFLAG parameter.
   trackingParams.timeStampOutFlag.def = 0;
   trackingParams.timeStampOutFlag.min = 0;
   trackingParams.timeStampOutFlag.max = 1;

   // NCORRPOINTS parameter.
   trackingParams.nCorrPoints.def = 1;
   trackingParams.nCorrPoints.min = 1;
   trackingParams.nCorrPoints.max = MAX_NCORRPOINTS;

   // CPINITIALIZATIONFLAG parameter.
   trackingParams.cpInitializationFlag.def = 1;
   trackingParams.cpInitializationFlag.min = 0;
   trackingParams.cpInitializationFlag.max = 1;

   // GLOHWBIAS parameter.
   trackingParams.gloHwBias.def = 0;
   trackingParams.gloHwBias.min =-10000;
   trackingParams.gloHwBias.max = 10000;

   // MSGBITMODE parameter.
   trackingParams.msgBitWipeOffMode.def = 0;
   trackingParams.msgBitWipeOffMode.min = 0;
   trackingParams.msgBitWipeOffMode.max = 1;

   // NSUPERFRAMES parameter.
   trackingParams.nSuperFrames.def = 0;
   trackingParams.nSuperFrames.min = 0;
   trackingParams.nSuperFrames.max = 12;

   // TCKLUTFILE parameter.
   strcpy(trackingParams.tckLUTFile.name, "");
}

//*****************************************************************************
//   Name:         setConfigurationChannelsData
//   Purpose:      Initialize the configuration fields that depends on the
//               number of channels to their default values.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  Config            configuration structure.
//               alreadySet        flag for default values already initialized.
//               acquisitionParams acquisition internal structure.
//               aidParams         aiding internal structure.
//               commonParams      common internal structure.
//               lockParams        lock internal structure.
//               managementParams  management internal structure.
//               pvtParams         navigation internal structure.
//               trackingParams    tracking internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   setConfigurationChannelsData(void)
{
   // This function set the default values of all the configuration parameters.

   // Set flag to TRUE to indicate that the default value has been set.
   alreadySet = TRUE;

   // Setting configuration data for acquisition to their default values.
   Config.FftAcq.pfa				= acquisitionParams.pfa.def;
   Config.FftAcq.acqTh				= acquisitionParams.acqTh.def;
   Config.FftAcq.freqStepCodeRatio	= acquisitionParams.freqStepCodeRatio.def;
   Config.FftAcq.maxNbins			= acquisitionParams.maxNbins.def;
   Config.FftAcq.maxFreq			= acquisitionParams.maxFreq.def;
   Config.FftAcq.maxNonCohInt_GPS		= acquisitionParams.nAcqInt.def;
   Config.FftAcq.maxNonCohInt_GAL		= acquisitionParams.nAcqInt.def;
   Config.FftAcq.nFineInt			= acquisitionParams.nFineInt.def;
   Config.FftAcq.nMedInt			= acquisitionParams.nMedInt.def;
   Config.FftAcq.freqRefinementFlag = acquisitionParams.freqRefinementFlag.def;
   Config.FftAcq.acqFactor			= acquisitionParams.acqFactor.def;
   Config.FftAcq.sp3HighElevN		= acquisitionParams.sp3HighElevN.def;
   Config.FftAcq.maxDopplerDrift		= acquisitionParams.maxDopplerDrift.def;
   
   // Setting tracking configuration data to their default values.
   Config.Tck.pllBw					= trackingParams.pllBw.def;
   Config.Tck.dllBw					= trackingParams.dllBw.def;
   Config.Tck.dllOrder				= trackingParams.dllOrder.def;
   Config.Tck.nPll					= trackingParams.nPll.def;
   Config.Tck.resolutionCarrier		= trackingParams.resolutionCarrier.def;
   Config.Tck.nDll					= trackingParams.nDll.def;
   Config.Tck.pllOrder				= trackingParams.pllOrder.def;
   Config.Tck.decimateFactor		= trackingParams.decimateFactor.def;
   Config.Tck.timeStampOutFlag	    = trackingParams.timeStampOutFlag.def;
   Config.Tck.nCorrPoints		    = trackingParams.nCorrPoints.def;
   Config.Tck.cpInitializationFlag  = trackingParams.cpInitializationFlag.def;
   Config.Tck.msgBitWipeOffMode		= trackingParams.msgBitWipeOffMode.def;
   Config.Tck.nSuperFrames			= trackingParams.nSuperFrames.def;
   Config.Tck.nDll_GAL				= trackingParams.nDll_GAL.def;
   Config.Tck.nPll_GAL				= trackingParams.nPll_GAL.def;

   // Setting lock configuration data to their default values .
   Config.Lck.cliTh					= lockParams.c2PhiTh.def;
   Config.Lck.cnoTh					= lockParams.cnoTh.def;
   Config.Lck.nCnoTh				= lockParams.nCnoTh.def;
   Config.Lck.nLock					= lockParams.nLock.def;
   Config.Lck.vtCnoTh				= lockParams.vtCnoTh.def;
   Config.Lck.lossCnoTh				= lockParams.lossCnoTh.def;
   Config.Lck.vtNLock				= lockParams.vtNLock.def;
}

//*****************************************************************************
//   Name:         startSection
//   Purpose:      Checks if the current line is a valid header section.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  parserStatus        configuration file parser status.
//               sectionName         array of valid section name.
//               currentSectionName  name of the current section parsed.
//               currentSection      current section ID.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/14   -   JJNC   -   Created.
//*****************************************************************************
void	startSection(void)
{
   int      i	  = 0;                      // Loop counter.
   int      found = 0;                  // Section found flag.

   // Check parser internal status.
   if (parserStatusOk())
   {
      // Loop for section names.
      while ((i<NUMBER_SECTIONS) && (!found))
      {
         // Compare section name read with i-element.
         if (strcmp( sectionName[i], currentSection) == 0)
         {
            // Update section name.
            strcpy( currentSectionName, sectionName[i]);

            // Update loop flag and parser status.
            found = 1;
            parserStatus = NEXT_PARAM_END;
         }
         // Not match.
         else
         {
            // Next section.
            i++;
            // Update unit fail flag if not found.
            if (i == NUMBER_SECTIONS)
            {
               // Update fail status.
               updateFailStatus( SECTION_NAME_NOT_EXIST, currentSection);
            }
         }
      }
   }
}

//*****************************************************************************
//   Name:         endSection
//   Purpose:      Checks if the current line matches current open section.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  parserStatus       configuration file parser status.
//               currentParameter   current parameter name.
//               currentSectionName name of the current section parsed.
//               currentSection     current section ID.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/14   -   JJNC   -   Created.
//*****************************************************************************
void endSection(void)
{
   // Check fail in last read or eof reached.
   if (parserStatusOk())
   {
      // Check if current line is an end of section.
      if (strcmp(currentParameter, "") == 0)
      {
         // Compare section name read with actual open section.
         if (strcmp( currentSectionName, currentSection) == 0)
         {
            // Update
            parserStatus = NEXT_HEADER;

            // Clean current section name.
            strcpy( currentSectionName , "");
         }
         else
         {
            updateFailStatus( END_SECTION_NOT_MATCH, currentSection);
         }
      }
   }
}

//*****************************************************************************
//   Name:         validateParameter
//   Purpose:      Select the correct validateXXX function to check and update
//               parameter.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      currentSectionName   name of the current section parsed.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/14   -   JJNC   -   Created.
//*****************************************************************************
void validateParameter(void)
{
   // Common section parameter.
   if (strcmp(currentSectionName, COM_SECTION) == 0)
   {
      validateCOMParameter();
   }
   // Acquisition section parameter.
   else if (strcmp(currentSectionName, ACQ_SECTION) == 0)
   {
	   validateACQParameter();
   }
   // Aiding section parameter.
   else if (strcmp(currentSectionName, AID_SECTION) == 0)
   {
	   validateAIDParameter();
   }
   // Lock section parameter.
   else if (strcmp(currentSectionName, LCK_SECTION) == 0)
   {
	   validateLCKParameter();
   }
   // Management section parameter.
   else if (strcmp(currentSectionName, MNG_SECTION) == 0)
   {
	   validateMNGParameter();
   }
   // Navigation LSQ section parameter.
   else if (strcmp(currentSectionName, PVT_SECTION) == 0)
   {
	   validatePVTParameter();
   }
   // Navigation EKF section parameter.
   else if (strcmp(currentSectionName, EKF_SECTION) == 0)
   {
	   validateEKFParameter();
   }
   // Tracking section parameter.
   else if (strcmp(currentSectionName, TCK_SECTION) == 0)
   {
	   validateTCKParameter();
   }
   // Wrong section name.
   else
   {
	   updateFailStatus( SECTION_NAME_NOT_EXIST, "");
   }
}

//*****************************************************************************
//   Name:         validateCOMParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter  name of the current parameter parsed.
//               currentValue      current value parsed.
//               Config            configuration structure.
//               aidParams         aiding internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateCOMParameter(void)
{
   int  mode=0;               // Auxiliar variable to get execution mode.

   // SAMPLEFILE parameter.
   if (strcmp( currentParameter, SAMPLEFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Com.sampleFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
         warningFlag = TRUE;
      }
   }
   // RAWSAMPLEBYTESFLAG parameter.
   else if (strcmp( currentParameter, RAWSAMPLEBYTESFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.rawSampleBytesFlag, FALSE, 0, commonParams.rawSampleBytesFlag.min,
																	  commonParams.rawSampleBytesFlag.max,
																	  commonParams.rawSampleBytesFlag.def);
   }
   // ONESAMPLEPERBYTEFLAG parameter.
   else if (strcmp( currentParameter, ONESAMPLEPERBYTEFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.oneSamplePerByteFlag, FALSE, 0, commonParams.oneSamplePerByteFlag.min,
																	  commonParams.oneSamplePerByteFlag.max,
																	  commonParams.oneSamplePerByteFlag.def);
   }
   // SIMTIME parameter.
   else if (strcmp( currentParameter, SIMTIME) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.simTime, FALSE, 0, commonParams.simTime.min,
														 commonParams.simTime.max,
														 commonParams.simTime.def);
   }
   // SAMPFREQ parameter.
   else if (strcmp( currentParameter, SAMPFREQ) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Com.sampFreq, FALSE, 0, commonParams.sampFreq.min,
															 commonParams.sampFreq.max,
															 commonParams.sampFreq.def);
   }
   // BANDWIDTH parameter.
   else if (strcmp( currentParameter, BANDWIDTH) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Com.bandwidth, FALSE, 0, commonParams.bandwidth.min,
															  commonParams.bandwidth.max,
															  commonParams.bandwidth.def);
   }
   // IFREQ parameter.
   else if (strcmp( currentParameter, IFREQ) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Com.ifreq, FALSE, 0, commonParams.iFreq.min,
														  commonParams.iFreq.max,
														  commonParams.iFreq.def);
   }

   // FILTERCENTERFREQ parameter.
   else if (strcmp( currentParameter, FILTERCENTERFREQ) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Com.filterCenterFreq, FALSE, 0, commonParams.filterCenterFreq.min,
																	 commonParams.filterCenterFreq.max,
																	 commonParams.filterCenterFreq.def);
   }
   // FEPLLMODE parameter.
   else if (strcmp( currentParameter, FEPLLMODE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.fePllMode, FALSE, 0, commonParams.fePllMode.min,
														   commonParams.fePllMode.max,
														   commonParams.fePllMode.def);
   }
   // STARTTIME parameter.
   else if (strcmp( currentParameter, STARTTIME) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Com.startTime, FALSE, 0, commonParams.startTime.min,
															  commonParams.startTime.max,
															  commonParams.startTime.def);
   }
   // MODE parameter.
   else if (strcmp( currentParameter, MODE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &mode, FALSE, 0, commonParams.mode.min,
                                 commonParams.mode.max,
                                 commonParams.mode.def);

      Config.Com.mode = (E_ExecutionMode) mode;
   }
   // OUTPUTRATE parameter.
   else if (strcmp( currentParameter, OUTPUTRATE) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Com.outputRate, FALSE, 0, commonParams.outputRate.min,
															  commonParams.outputRate.max,
															  commonParams.outputRate.def);
   }
   // SYSTEMFLAG parameter.
   else if (strcmp( currentParameter, SYSTEMFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.systemFlag, FALSE, 0, commonParams.systemFlag.min,
							 							    commonParams.systemFlag.max,
														    commonParams.systemFlag.def);
   }
   // SAMPLEWRITINGRATE parameter.
   else if (strcmp( currentParameter, SAMPLEWRITINGRATE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.sampleWritingRate, FALSE, 0, commonParams.sampleWritingRate.min,
							 									   commonParams.sampleWritingRate.max,
																   commonParams.sampleWritingRate.def);
   }
   // SPLITSAMPLEFILERATE parameter.
   else if (strcmp( currentParameter, SPLITSAMPLEFILERATE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Com.splitSampleFileRate, FALSE, 0, commonParams.splitSampleFileRate.min,
							 									   commonParams.splitSampleFileRate.max,
																   commonParams.splitSampleFileRate.def);
   }
   // RESULTPATH parameter.
   else if (strcmp( currentParameter, RESULTPATH) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         if ((strlen(currentValue) > 0) && (currentValue[strlen(currentValue)-1] != DIR_DELIMITER_CH))
         {
            sprintf( Config.Com.resulPath, "%s%c", currentValue, DIR_DELIMITER_CH);
         } else {
            sprintf( Config.Com.resulPath, "%s", currentValue);
         }
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
         warningFlag = TRUE;
      }
   }

   // Error.
   else
   {
      warningFlag = TRUE;
   }
}

//*****************************************************************************
//   Name:         validateACQParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter   name of the current parameter parsed.
//               Config             configuration structure.
//               acquisitionParams  acquisition internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateACQParameter(void)
{
   // MAXFREQ parameter.
   if (strcmp( currentParameter, MAXFREQ) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.FftAcq.maxFreq, FALSE, Config.Mng.nChannels, acquisitionParams.maxFreq.min,
																				 acquisitionParams.maxFreq.max,
																				 acquisitionParams.maxFreq.def);
   }
   // FREQSTEPCODERATIO parameter.
   else if (strcmp( currentParameter, FREQSTEPCODERATIO) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.FftAcq.freqStepCodeRatio, FALSE, Config.Mng.nChannels, acquisitionParams.freqStepCodeRatio.min,
																				  acquisitionParams.freqStepCodeRatio.max,
							                                                      acquisitionParams.freqStepCodeRatio.def);
   }
   // MAXNBINS parameter.
   else if (strcmp( currentParameter, MAXNBINS) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.maxNbins, FALSE, Config.Mng.nChannels, acquisitionParams.maxNbins.min,
													                            acquisitionParams.maxNbins.max,
						                                                        acquisitionParams.maxNbins.def);
   }
   // PFA parameter.
   else if (strcmp( currentParameter, PFA) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.FftAcq.pfa, FALSE, Config.Mng.nChannels, acquisitionParams.pfa.min,
						                                                      acquisitionParams.pfa.max,
												                              acquisitionParams.pfa.def);
   }
   // ACQTH parameter.
   else if (strcmp( currentParameter, ACQTH) == 0)
   {
      // Update the double field of config structure.
      updateFloatStructure( &Config.FftAcq.acqTh, FALSE, Config.Mng.nChannels, acquisitionParams.acqTh.min,
																			   acquisitionParams.acqTh.max,
						                                                       acquisitionParams.acqTh.def);
   }
   // MAXNONCOHINTGPS parameter.
   else if (strcmp( currentParameter, MAXNONCOHINTGPS) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.maxNonCohInt_GPS, FALSE, Config.Mng.nChannels, acquisitionParams.maxNonCohInt_GPS.min,
																					acquisitionParams.maxNonCohInt_GPS.max,
																					acquisitionParams.maxNonCohInt_GPS.def);
   }
   // MAXNONCOHINTGAL parameter.
   else if (strcmp( currentParameter, MAXNONCOHINTGAL) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.maxNonCohInt_GAL, FALSE, Config.Mng.nChannels, acquisitionParams.maxNonCohInt_GAL.min,
																					acquisitionParams.maxNonCohInt_GAL.max,
																					acquisitionParams.maxNonCohInt_GAL.def);
   }
   // NMEDINT parameter.
   else if (strcmp( currentParameter, NMEDINT) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.nMedInt, FALSE, Config.Mng.nChannels, acquisitionParams.nMedInt.min,
																			   acquisitionParams.nMedInt.max,
						                                                       acquisitionParams.nMedInt.def);
   }
   // NFINEINT parameter.
   else if (strcmp( currentParameter, NFINEINT) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.nFineInt, FALSE, Config.Mng.nChannels, acquisitionParams.nFineInt.min,
																				acquisitionParams.nFineInt.max,
																				acquisitionParams.nFineInt.def);
   }
   // FREQREFINEMENTFLAG parameter.
   else if (strcmp( currentParameter, FREQREFINEMENTFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.freqRefinementFlag, FALSE, Config.Mng.nChannels, acquisitionParams.freqRefinementFlag.min,
																						  acquisitionParams.freqRefinementFlag.max,
																						  acquisitionParams.freqRefinementFlag.def);
   }
   // ACQFACTOR parameter.
   else if (strcmp( currentParameter, ACQFACTOR) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.acqFactor, FALSE, Config.Mng.nChannels, acquisitionParams.acqFactor.min,
                                                      acquisitionParams.acqFactor.max,
                                                      acquisitionParams.acqFactor.def);
   }
   // SP3HIGHELEVN parameter.
   else if (strcmp( currentParameter, SP3HIGHELEVN) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.FftAcq.sp3HighElevN, FALSE, Config.Mng.nChannels, acquisitionParams.sp3HighElevN.min,
																					acquisitionParams.sp3HighElevN.max,
																					acquisitionParams.sp3HighElevN.def);
   }
   // ACQLUTFILE parameter.
   else if (strcmp( currentParameter, ACQLUTFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.FftAcq.acqLUTFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
         warningFlag = TRUE;
      }
   }
   else
   {
      warningFlag = TRUE;
   }
}

//*****************************************************************************
//   Name:         validateAIDParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter  name of the current parameter parsed.
//               Config            configuration structure.
//               aidParams         aiding internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateAIDParameter(void)
{
   // REFWN parameter.
   if (strcmp( currentParameter, REFWN) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Aid.refWn, FALSE, 0, aidParams.refWn.min,
													   aidParams.refWn.max,
													   aidParams.refWn.def);
   }
   // REFTOW parameter.
   else if (strcmp( currentParameter, REFTOW) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Aid.refTow, FALSE, 0, aidParams.refTow.min,
														   aidParams.refTow.max,
														   aidParams.refTow.def);
   }
   // REFPOS parameter.
   else if (strcmp( currentParameter, REFPOS) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( Config.Aid.refPos, TRUE, REFPOS_LENGTH, aidParams.refPos.min,
																	 aidParams.refPos.max,
																	 aidParams.refPos.def);
   }
   // ALMFILE parameter.
   else if (strcmp( currentParameter, ALMFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Aid.almFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {

      }
   }
   // EPHFILE parameter.
   else if (strcmp( currentParameter, EPHFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Aid.ephFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
      }
   }
   // GALEPHFILE parameter.
   else if (strcmp( currentParameter, GALEPHFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Aid.galEphFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {

      }
   }
   // SP3FILE parameter.
   else if (strcmp( currentParameter, SP3FILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Aid.sp3File, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {	
      }
   }
   // LASTVISIBCOMPTH parameter.
   else if (strcmp( currentParameter, LASTVISIBCOMPTH) == 0)
   {
      // Update the int field of config structure.
      updateLongStructure( &Config.Aid.lastVisibCompTh, FALSE, 0, aidParams.lastVisibCompTh.min,
																  aidParams.lastVisibCompTh.max,
																  aidParams.lastVisibCompTh.def);
   }
   else if (strcmp( currentParameter, CLKDRIFTAID) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Aid.clkDriftAid, FALSE, 0, aidParams.clkDriftAid.min,
																aidParams.clkDriftAid.max,
																aidParams.clkDriftAid.def);
   }
   else
   {
      warningFlag = TRUE;
   }
}


//*****************************************************************************
//   Name:         validateLCKParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter   name of the current parameter parsed.
//               Config             configuration structure.
//               lockParams         lock internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateLCKParameter(void)
{
   // NCNOTH parameter.
   if (strcmp( currentParameter, NCNOTH) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Lck.nCnoTh, FALSE, 1, lockParams.nCnoTh.min,
                                                        lockParams.nCnoTh.max,
                                                        lockParams.nCnoTh.def);
   }
   // NLOCK parameter.
   else if (strcmp( currentParameter, NLOCK) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Lck.nLock, FALSE, 1, lockParams.nLock.min,
													   lockParams.nLock.max,
													   lockParams.nLock.def);
   }
   // VTNLOCK parameter.
   else if (strcmp( currentParameter, VTNLOCK) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Lck.vtNLock, FALSE, 1, lockParams.vtNLock.min,
														 lockParams.vtNLock.max,
														 lockParams.vtNLock.def);
   }
   // CNOTH parameter.
   else if (strcmp( currentParameter, CNOTH) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Lck.cnoTh, FALSE, 1, lockParams.cnoTh.min,
														 lockParams.cnoTh.max,
														 lockParams.cnoTh.def);
   }
   // VTCNOTH parameter.
   else if (strcmp( currentParameter, VTCNOTH) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Lck.vtCnoTh, FALSE, 1, lockParams.vtCnoTh.min,
                                                           lockParams.vtCnoTh.max,
														   lockParams.vtCnoTh.def);
   }
   // C2PHITH parameter.
   else if (strcmp( currentParameter, C2PHITH) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Lck.cliTh, FALSE, 1, lockParams.c2PhiTh.min,
                                                         lockParams.c2PhiTh.max,
                                                         lockParams.c2PhiTh.def);
   }
   // LOSSCNOTH parameter.
   else if (strcmp( currentParameter, LOSSCNOTH) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Lck.lossCnoTh, FALSE, 1, lockParams.lossCnoTh.min,
															 lockParams.lossCnoTh.max,
															 lockParams.lossCnoTh.def);
   }
   else
   {
      warningFlag = TRUE;
   }
}



//*****************************************************************************
//   Name:         validateMNGParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter   name of the current parameter parsed.
//               Config             configuration structure.
//               managementParams   management internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateMNGParameter(void)
{
   int aux=0;                // Aux variable.
   int mode=0;               // Acquisition mode.

   // NCHANNELS parameter.
   if (strcmp( currentParameter, NCHANNELS) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Mng.nChannels, FALSE, 0, managementParams.nChannels.min,
														   managementParams.nChannels.max,
														   managementParams.nChannels.def);
      // Reset parameters that depends on number of channels.
      setConfigurationChannelsData();
   }
   // NPRNFBD parameter.
   else if (strcmp( currentParameter, NPRNFBD) == 0)
   {
      // Update max parameter, depending on available satellites
      aux = MAXNUMSAT - Config.Mng.nChannels;
      if (aux < managementParams.nPrnFbd.max)
      {
         managementParams.nPrnFbd.max = aux;

         if (aux < managementParams.nPrnFbd.def)
         {
            managementParams.nPrnFbd.def = aux;
         }
      }

      // Update the int field of config structure.
      updateIntStructure( &Config.Mng.nPrnFbd, FALSE, 0, managementParams.nPrnFbd.min,
														 managementParams.nPrnFbd.max,
														 managementParams.nPrnFbd.def);
   }
   // PRNFBD parameter.
   else if (strcmp( currentParameter, PRNFBD) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( Config.Mng.prnFbd, TRUE, Config.Mng.nPrnFbd, managementParams.prnFbd.min,
																	   managementParams.prnFbd.max,
																	   managementParams.prnFbd.def);
   }
   // ENABLEVTFLAG parameter.
   else if (strcmp( currentParameter, ENABLEVTFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Mng.enableVtFlag, FALSE, 0, managementParams.enableVtFlag.min,
						   managementParams.enableVtFlag.max,
						   managementParams.enableVtFlag.def);
   }
   // STARTMODE parameter.
   else if (strcmp( currentParameter, STARTMODE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &mode, FALSE, 0, managementParams.startMode.min,   managementParams.startMode.max,
                                 managementParams.startMode.def);
      Config.Mng.startMode = (E_StartMode) mode;
   }
   // FASTFIRSTFIXFLAG parameter.
   else if (strcmp( currentParameter, FASTFIRSTFIXFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &mode, FALSE, 0, managementParams.fastFirstFixFlag.min,   managementParams.fastFirstFixFlag.max,
                                 managementParams.fastFirstFixFlag.def);
      Config.Mng.fastFirstFixFlag = (E_StartMode) mode;
   }
   else
   {
      warningFlag = TRUE;
   }
}



//*****************************************************************************
//   Name:         validatePVTParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter  name of the current parameter parsed.
//               currentValue      current value parsed.
//               Config            configuration structure.
//               managementParams  management internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validatePVTParameter(void)
{
   // NMEATCPINFO parameter.
   if (strcmp( currentParameter, NMEATCPINFO) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Pvt.nmeaTCPinfo, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
         warningFlag = TRUE;
      }
   }
   // ELEVMASK parameter.
   else if (strcmp( currentParameter, ELEVMASK) == 0)
   {
      // Update the int field of config structure.
      updateDoubleStructure( &Config.Pvt.elevMask, FALSE, 0, pvtParams.elevMask.min,
															 pvtParams.elevMask.max,
															 pvtParams.elevMask.def);
   }
   // SMOLENGTH parameter.
   else if (strcmp( currentParameter, SMOLENGTH) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.smoLength, FALSE, 0, pvtParams.smoLength.min,
														   pvtParams.smoLength.max,
														   pvtParams.smoLength.def);
   }
   // IONOCORRECTION parameter.
   else if (strcmp( currentParameter, IONOCORRECTION) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.ionoCorrection, FALSE, 0, pvtParams.ionoCorrection.min,
																pvtParams.ionoCorrection.max,
																pvtParams.ionoCorrection.def);
   }
   // TROPOCORRECTION parameter.
   else if (strcmp( currentParameter, TROPOCORRECTION) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.tropoCorrection, FALSE, 0, pvtParams.tropoCorrection.min,
																 pvtParams.tropoCorrection.max,
																 pvtParams.tropoCorrection.def);
   }
   // CN0OUTFLAF parameter.
   else if (strcmp( currentParameter, CN0OUTFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.cn0outFlag, FALSE, 0, pvtParams.cn0outFlag.min,
														    pvtParams.cn0outFlag.max,
														    pvtParams.cn0outFlag.def);
   }
   // INTEGERTIMEMARKFLAG parameter.
   else if (strcmp( currentParameter, INTEGERTIMEMARKFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.integerTimeMarkFlag, FALSE, 0, pvtParams.integerTimeMarkFlag.min,
																	 pvtParams.integerTimeMarkFlag.max,
																	 pvtParams.integerTimeMarkFlag.def);
   }
   // PVTVTFLAG parameter.
   else if (strcmp( currentParameter, PVTVTFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.pvtVtFlag, FALSE, 0, pvtParams.pvtVtFlag.min,
																	 pvtParams.pvtVtFlag.max,
																	 pvtParams.pvtVtFlag.def);
   }
   // CLKSTEERINGFLAG parameter.
   else if (strcmp( currentParameter, CLKSTEERINGFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.clkSteeringFlag, FALSE, 0, pvtParams.clkSteeringFlag.min,
																 pvtParams.clkSteeringFlag.max,
																 pvtParams.clkSteeringFlag.def);
   }
   // ISBSTEERINGFLAG parameter.
   else if (strcmp( currentParameter, ISBSTEERINGFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Pvt.isbSteeringFlag, FALSE, 0, pvtParams.isbSteeringFlag.min,
																 pvtParams.isbSteeringFlag.max,
																 pvtParams.isbSteeringFlag.def);
   }
   else
   {
      warningFlag = TRUE;
   }
}

//*****************************************************************************
//   Name:         validateEKFParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter  name of the current parameter parsed.
//               currentValue      current value parsed.
//               Config            configuration structure.
//               managementParams  management internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2018/09/14   -   AKKK   -   Created.
//*****************************************************************************
void   validateEKFParameter(void)
{
   // USEEKF parameter.
   if (strcmp( currentParameter, USEEKF) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Ekf.useEkf, FALSE, 0, ekfParams.useEkf.min,
                                                        ekfParams.useEkf.max,
                                                        ekfParams.useEkf.def);
   }
   // SPACEUSER parameter.
   else if (strcmp( currentParameter, SPACEUSER) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Ekf.spaceUser, FALSE, 0,  ekfParams.spaceUser.min,
                                                            ekfParams.spaceUser.max,
                                                            ekfParams.spaceUser.def);
   }
   // DELTAT parameter.
   else if (strcmp( currentParameter, DELTAT) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.deltaT, FALSE, 0,	ekfParams.deltaT.min,
                                                                ekfParams.deltaT.max,
                                                                ekfParams.deltaT.def);
   }
   // MAHATHR parameter.
   else if (strcmp( currentParameter, MAHATHR) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.mahaThr, FALSE, 0,	ekfParams.mahaThr.min,
                                                                ekfParams.mahaThr.max,
                                                                ekfParams.mahaThr.def);
   }
   // MEASSIGMAPR parameter.
   else if (strcmp( currentParameter, MEASSIGMAPR) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.measSigmaPr, FALSE, 0,	ekfParams.measSigmaPr.min,
                                                                ekfParams.measSigmaPr.max,
                                                                ekfParams.measSigmaPr.def);
   }
   // MEASSIGMADP parameter.
   else if (strcmp( currentParameter, MEASSIGMADP) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.measSigmaDp, FALSE, 0,	ekfParams.measSigmaDp.min,
                                                                ekfParams.measSigmaDp.max,
                                                                ekfParams.measSigmaDp.def);
   }
   // INITSTATESIGMAPOS parameter.
   else if (strcmp( currentParameter, INITSTATESIGMAPOS) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.initStateSigmaPos, FALSE, 0,	ekfParams.initStateSigmaPos.min,
                                                                        ekfParams.initStateSigmaPos.max,
                                                                        ekfParams.initStateSigmaPos.def);
   }
   // INITSTATESIGMAVEL parameter.
   else if (strcmp( currentParameter, INITSTATESIGMAVEL) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.initStateSigmaVel, FALSE, 0,	ekfParams.initStateSigmaVel.min,
                                                                        ekfParams.initStateSigmaVel.max,
                                                                        ekfParams.initStateSigmaVel.def);
   }
   // INITSTATESIGMACLKBIAS parameter.
   else if (strcmp( currentParameter, INITSTATESIGMACLKBIAS) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.initStateSigmaClkBias, FALSE, 0,	ekfParams.initStateSigmaClkBias.min,
                                                                                ekfParams.initStateSigmaClkBias.max,
                                                                                ekfParams.initStateSigmaClkBias.def);
   }
   // INITSTATESIGMACLKGALGPSBIAS parameter.
   else if (strcmp( currentParameter, INITSTATESIGMACLKGALGPSBIAS) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.initStateSigmaClkGalGpsBias, FALSE, 0,	ekfParams.initStateSigmaClkGalGpsBias.min,
                                                                                        ekfParams.initStateSigmaClkGalGpsBias.max,
                                                                                        ekfParams.initStateSigmaClkGalGpsBias.def);
   }
   // INITSTATESIGMACLKDRIFT parameter.
   else if (strcmp( currentParameter, INITSTATESIGMACLKDRIFT) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.initStateSigmaClkDrift, FALSE, 0,	ekfParams.initStateSigmaClkDrift.min,
                                                                                ekfParams.initStateSigmaClkDrift.max,
                                                                                ekfParams.initStateSigmaClkDrift.def);
   }
   // STATENOISECOVPOS parameter.
   else if (strcmp( currentParameter, STATENOISECOVPOS) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.stateNoiseCovPos, FALSE, 0,	ekfParams.stateNoiseCovPos.min,
                                                                        ekfParams.stateNoiseCovPos.max,
                                                                        ekfParams.stateNoiseCovPos.def);
   }
   // STATENOISECOVVEL parameter.
   else if (strcmp( currentParameter, STATENOISECOVVEL) == 0)
   {
      // Update the double field of config structure.
      updateDoubleStructure( &Config.Ekf.stateNoiseCovVel, FALSE, 0,	ekfParams.stateNoiseCovVel.min,
                                                                        ekfParams.stateNoiseCovVel.max,
                                                                        ekfParams.stateNoiseCovVel.def);
   }
   // STATENOISECOVCLKBIAS parameter.
   else if (strcmp( currentParameter, STATENOISECOVCLKBIAS) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.stateNoiseCovClkBias, FALSE, 0,	ekfParams.stateNoiseCovClkBias.min,
                                                                                ekfParams.stateNoiseCovClkBias.max,
                                                                                ekfParams.stateNoiseCovClkBias.def);
   }
   // STATENOISECOVCLKGALGPSBIAS parameter.
   else if (strcmp( currentParameter, STATENOISECOVCLKGALGPSBIAS) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.stateNoiseCovClkGalGpsBias, FALSE, 0,	ekfParams.stateNoiseCovClkGalGpsBias.min,
                                                                                        ekfParams.stateNoiseCovClkGalGpsBias.max,
                                                                                        ekfParams.stateNoiseCovClkGalGpsBias.def);
   }
   // STATENOISECOVCLKDRIFT parameter.
   else if (strcmp( currentParameter, STATENOISECOVCLKDRIFT) == 0)
   {
      // Update the double field of config structure.
	   updateDoubleStructure( &Config.Ekf.stateNoiseCovClkDrift, FALSE, 0,	ekfParams.stateNoiseCovClkDrift.min,
                                                                                ekfParams.stateNoiseCovClkDrift.max,
                                                                                ekfParams.stateNoiseCovClkDrift.def);
   }
   else
   {
      warningFlag = TRUE;
   }
}

//*****************************************************************************
//   Name:         validateTCKParameter
//   Purpose:      Check and update the parameter if no error in parameter line.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  currentParameter  name of the current parameter parsed.
//               Config            configuration structure.
//               trackingParams    management internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   validateTCKParameter(void)
{
   // DLLORDER parameter.
   if (strcmp( currentParameter, DLLORDER) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.dllOrder, FALSE, 1, trackingParams.dllOrder.min,
                                                          trackingParams.dllOrder.max,
                                                          trackingParams.dllOrder.def);
   }
   // PLLORDER parameter.
   else if (strcmp( currentParameter, PLLORDER) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.pllOrder, FALSE, 1, trackingParams.pllOrder.min,
                                                          trackingParams.pllOrder.max,
                                                          trackingParams.pllOrder.def);
   }
   // DECIMATEFACTOR parameter.
   else if (strcmp( currentParameter, DECIMATEFACTOR) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.decimateFactor, FALSE, 1, trackingParams.decimateFactor.min,
																trackingParams.decimateFactor.max,
																trackingParams.decimateFactor.def);
   }
   // PLLBW parameter.
   else if (strcmp( currentParameter, PLLBW) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Tck.pllBw, FALSE, 1, trackingParams.pllBw.min,
														 trackingParams.pllBw.max,
														 trackingParams.pllBw.def);
   }
   // DLLBW parameter.
   else if (strcmp( currentParameter, DLLBW) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Tck.dllBw, FALSE, 1, trackingParams.dllBw.min,
														 trackingParams.dllBw.max,
														 trackingParams.dllBw.def);
   }
   // NDLL parameter.
   else if (strcmp( currentParameter, NDLL) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nDll, FALSE, 1, trackingParams.nDll.min,
                                                      trackingParams.nDll.max,
                                                      trackingParams.nDll.def);
   }
   // NCARRIERINT parameter.
   else if (strcmp( currentParameter, NPLL) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nPll, FALSE, 1, trackingParams.nPll.min,
                                                      trackingParams.nPll.max,
                                                      trackingParams.nPll.def);
   }// NDLL parameter.
   else if (strcmp( currentParameter, NDLL_GAL) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nDll_GAL, FALSE, 1,	trackingParams.nDll_GAL.min,
															trackingParams.nDll_GAL.max,
															trackingParams.nDll_GAL.def);
   }
   // NCARRIERINT parameter.
   else if (strcmp( currentParameter, NPLL_GAL) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nPll_GAL, FALSE, 1, trackingParams.nPll_GAL.min,
                                                      trackingParams.nPll_GAL.max,
                                                      trackingParams.nPll_GAL.def);
   }
   // RESOLUTIONCARRIER parameter.
   else if (strcmp( currentParameter, RESOLUTIONCARRIER) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.resolutionCarrier, FALSE, 1, trackingParams.resolutionCarrier.min,
																   trackingParams.resolutionCarrier.max,
																   trackingParams.resolutionCarrier.def);
   }
   // CHIPDELAY parameter.
   else if (strcmp( currentParameter, CHIPDELAY) == 0)
   {
      // Update the int field of config structure.
      updateFloatStructure( &Config.Tck.chipDelay, FALSE, 1, trackingParams.chipDelay.min,
															 trackingParams.chipDelay.max,
															 trackingParams.chipDelay.def);
   }
   // TIMESTAMPOUTFLAG parameter.
   else if (strcmp( currentParameter, TIMESTAMPOUTFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.timeStampOutFlag, FALSE, 1, trackingParams.timeStampOutFlag.min,
																  trackingParams.timeStampOutFlag.max,
																  trackingParams.timeStampOutFlag.def);
   }
   // NCORRPOINTS parameter.
   else if (strcmp( currentParameter, NCORRPOINTS) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nCorrPoints, FALSE, 1, trackingParams.nCorrPoints.min,
															 trackingParams.nCorrPoints.max,
															 trackingParams.nCorrPoints.def);
   }
   // CPINITIALIZATIONFLAG parameter.
   else if (strcmp( currentParameter, CPINITIALIZATIONFLAG) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.cpInitializationFlag, FALSE, 1, trackingParams.cpInitializationFlag.min,
															 trackingParams.cpInitializationFlag.max,
															 trackingParams.cpInitializationFlag.def);
   }
   // MSGBITMODE parameter.
   else if (strcmp( currentParameter, MSGBITWIPEOFFMODE) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.msgBitWipeOffMode, FALSE, 1, trackingParams.msgBitWipeOffMode.min,
																   trackingParams.msgBitWipeOffMode.max,
																   trackingParams.msgBitWipeOffMode.def);
   }
   // NSUPERFRAMES parameter.
   else if (strcmp( currentParameter, NSUPERFRAMES) == 0)
   {
      // Update the int field of config structure.
      updateIntStructure( &Config.Tck.nSuperFrames, FALSE, 1, trackingParams.nSuperFrames.min,
															  trackingParams.nSuperFrames.max,
															  trackingParams.nSuperFrames.def);
   }
   // TCKLUTFILE parameter.
   else if (strcmp( currentParameter, TCKLUTFILE) == 0)
   {
      // Checking new value belongs to parameter valid range.
      if (strlen(currentValue) <= MAX_FILE_NAME)
      {
         strcpy( Config.Tck.tckLUTFile, currentValue);
      }
      // Print a warning message because value does not belong to parameter range.
      else
      {
         warningFlag = TRUE;
      }
   }
   else
   {
      warningFlag = TRUE;
   }
}

//*****************************************************************************
//   Name:         printConfiguration
//   Purpose:      Write configuration values into file.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      Config   configuration global structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2009/11/16   -   JJNC   -   Created.
//*****************************************************************************
void      save_to_file_Configuration(void)
{
   char   fileName[MAX_FILE_NAME]; // File name.
   FILE   *fd=NULL;                // Output file desriptor.

   // Build file name.
#ifndef LINUX
   sprintf(fileName, "%sconfigValues.txt", Config.Com.resulPath);
#else
   sprintf(fileName, "%sconfigValues.txt", Config.Com.resulPath);
#endif

//    // Open file.
//    if ((fd = fopen( fileName, "w")) == NULL)
//    {
//    }
//    // Normal execution.
//    else
//    {
      // Print common data header.
//       fprintf( fd, "********************************\n");
//       fprintf( fd, "COMMON SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print common data values.
//       fprintf( fd, "SAMPLEFILE %s\n", Config.Com.sampleFile);
//       fprintf( fd, "SIMTIME %d\n", Config.Com.simTime);
//       fprintf( fd, "SAMPFREQ %lf\n", Config.Com.sampFreq);
//       fprintf( fd, "BANDWIDTH %lf\n", Config.Com.bandwidth);
//       fprintf( fd, "IFREQ %lf\n", Config.Com.ifreq);
//       fprintf( fd, "STARTTIME %lf\n", Config.Com.startTime);
//       fprintf( fd, "MODE %d\n", Config.Com.mode);
//       fprintf( fd, "OUTPUTRATE %lf\n", Config.Com.outputRate);
//       fprintf( fd, "RESULTPATH %s\n", Config.Com.resulPath);

      // Print acquisition data header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "ACQUISITION SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print acquisition data values.
//       fprintf( fd, "MAXFREQ %lf\n", Config.FftAcq.maxFreq);
// 	  fprintf( fd, "FREQSTEPCODERATIO %lf\n", Config.FftAcq.freqStepCodeRatio);
//       fprintf( fd, "MAXNBINS %d\n", Config.FftAcq.maxNbins);
//       fprintf( fd, "PFA %lf\n", Config.FftAcq.pfa);
//       fprintf( fd, "ACQTH %lf\n", Config.FftAcq.acqTh);
//       fprintf( fd, "MAXNONCOHINT %d\n", Config.FftAcq.maxNonCohInt);
//       fprintf( fd, "NMEDINT %d\n", Config.FftAcq.nMedInt);
//       fprintf( fd, "NFINEINT %d\n", Config.FftAcq.nFineInt);
//       fprintf( fd, "ACQFACTOR %d\n", Config.FftAcq.acqFactor);
//       fprintf( fd, "ACQLUTFILE %s\n", Config.FftAcq.acqLUTFile);

      // Print aiding data header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "AIDING SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print aiding data values.
//       fprintf( fd, "REFWN %d\n", Config.Aid.refWn);
//       fprintf( fd, "REFTOW %d\n", (int)Config.Aid.refTow);
//       fprintf( fd, "REFPOS %lf %lf %lf\n", Config.Aid.refPos[0],
//                                  Config.Aid.refPos[1],
//                                  Config.Aid.refPos[2]);
//       fprintf( fd, "ALMFILE %s\n", Config.Aid.almFile);
//       fprintf( fd, "EPHFILE %s\n", Config.Aid.ephFile);
// 	  fprintf( fd, "EPHFILE %s\n", Config.Aid.galEphFile);
//       fprintf( fd, "SP3FILE %s\n", Config.Aid.sp3File);
//       fprintf( fd, "LASTVISIBCOMPTH %ld\n", Config.Aid.lastVisibCompTh);

      // Print lock data header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "LOCK SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print lock data values.
//       fprintf( fd, "NOISEBANDKCO %d\n", Config.Lck.nLock);
//       fprintf( fd, "NCNOTH %d\n", Config.Lck.nCnoTh);
//       fprintf( fd, "CNOTH %lf\n", Config.Lck.cnoTh);
//       fprintf( fd, "CLI %lf\n", Config.Lck.cliTh);

      // Print management data header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "MANAGEMENT SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print management data values.
//       fprintf( fd, "NCHANNELS %d\n", Config.Mng.nChannels);
//       fprintf( fd, "\n");
//       fprintf( fd, "NPRNFBD %d\n", Config.Mng.nPrnFbd);
//       fprintf( fd, "PRNFBD: ");
//       fprintf( fd, "\n");
//       fprintf( fd, "STARTMODE %d\n", Config.Mng.startMode);

      // Print navigation data header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "NAVIGATION SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print navigation data values.
//       fprintf( fd, "NMEATCPINFO %s\n", Config.Pvt.nmeaTCPinfo);
//       fprintf( fd, "ELEVMASK %lf\n", Config.Pvt.elevMask);
//       fprintf( fd, "SMOLENGTH %d\n", Config.Pvt.smoLength);
//       fprintf( fd, "IONOCORRECTION %d\n", Config.Pvt.ionoCorrection);
//       fprintf( fd, "TROPOCORRECTION %d\n", Config.Pvt.tropoCorrection);

      // Print trackingdata header.
//       fprintf( fd, "\n********************************\n");
//       fprintf( fd, "TRACKING SECTION\n");
//       fprintf( fd, "********************************\n");

      // Print tracking data values.
//       fprintf( fd, "DECIMATEFACTOR %d\n", Config.Tck.decimateFactor);
//       fprintf( fd, "PLLORDER %d\n", Config.Tck.pllOrder);
//       fprintf( fd, "DLLORDER %d\n", Config.Tck.dllOrder);
//       fprintf( fd, "PLLBW %lf\n", Config.Tck.pllBw);
//       fprintf( fd, "DLLBW %lf\n", Config.Tck.dllBw);
//       fprintf( fd, "NDLL %d\n", Config.Tck.nDll);
//       fprintf( fd, "NPLL %d\n", Config.Tck.nPll);
// 	  fprintf( fd, "NDLL_GAL %d\n", Config.Tck.nDll_GAL);
//       fprintf( fd, "NPLL_GAL %d\n", Config.Tck.nPll_GAL);
//       fprintf( fd, "RESOLUTIONCARRIER %d\n", Config.Tck.resolutionCarrier);
//       fprintf( fd, "CHIPDELAY %lf\n", Config.Tck.chipDelay);
//       fprintf( fd, "TCKLUTFILE %s\n", Config.Tck.tckLUTFile);

      // Close file.
//       fclose(fd);
//    }
}

//*****************************************************************************
//   Name:         finalizeDefaultConfiguration
//   Purpose:      Free internal configuration unit resources used during
//               initialization.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  acquisitionParams acquisition internal structure.
//               aidParams         aiding internal structure.
//               commonParams      common internal structure.
//               lockParams        lock internal structure.
//               managementParams  management internal structure.
//               pvtParams         navigation internal structure.
//               trackingParams    tracking internal structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/15   -   JJNC   -   Created.
//*****************************************************************************
void   finalizeDefaultConfiguration(void)
{
  /* // Deallocate acquisition.
   free(acquisitionParams);
   acquisitionParams = 0;

   // Deallocate aiding.
   free(aidParams);
   aidParams = 0;

   // Deallocate common.
  // free(commonParams);
  // commonParams = 0;

   // Deallocate lock detection.
   free(lockParams);
   lockParams = 0;

   // Deallocate management.
   free(managementParams);
   managementParams = 0;

   // Deallocate navigation.
   free(pvtParams);
   pvtParams = 0;

   // Deallocate tracking.
   free(trackingParams);
   trackingParams = 0;*/
}


void	printConfiguration(void)
{


	  // Print common data header.
      printf("\n********************************\n");
      printf("COMMON SECTION\n");
      printf("********************************\n");

      // Print common data values.
          printf("ONSPW %d \n",Config.Com.onspw);
	  printf("MODE %d \n",Config.Com.mode);
          printf("SYSTEMFLAG %d \n",Config.Com.systemFlag);
          printf("SAMPFREQ %lf\n", Config.Com.sampFreq);
          printf("IFREQ %lf\n", Config.Com.ifreq);
	  printf("FILTERCENTERFREQ %lf \n",Config.Com.filterCenterFreq);	  	 
	  printf("BANDWIDTH %lf\n", Config.Com.bandwidth);	 
	  printf("RESULTPATH %s\n", Config.Com.resulPath);
	  printf("OUTPUTRATE %lf\n", Config.Com.outputRate);
	  printf("SIMTIME %d \n",Config.Com.simTime);
	  printf("STARTTIME %lf\n", Config.Com.startTime);
          printf("SAMPLEFILE %s\n", Config.Com.sampleFile);
          printf("RAWSAMPLEBYTESFLAG %d\n", Config.Com.rawSampleBytesFlag);
          printf("ONESAMPLEPERBYTEFLAG %d\n", Config.Com.oneSamplePerByteFlag);
      

	   // Print management data header.
      printf("\n********************************\n");
      printf("MANAGEMENT SECTION\n");
      printf("********************************\n");

      // Print management data values.
          printf("NCHANNELS %d\n", Config.Mng.nChannels);
	  printf("STARTMODE %d\n", Config.Mng.startMode);
	  printf("FASTFIRSTFIXFLAG %d\n", Config.Mng.fastFirstFixFlag);
	  printf("ENABLEVTFLAG %d\n", Config.Mng.enableVtFlag);

	  // Print acquisition data header.
      printf("\n********************************\n");
      printf("ACQUISITION SECTION\n");
      printf("********************************\n");

      // Print acquisition data values.
      printf("MAXFREQ %lf\n", Config.FftAcq.maxFreq);
      printf("FREQSTEPCODERATIO %lf\n", Config.FftAcq.freqStepCodeRatio);
      printf("MAXNBINS %d\n", Config.FftAcq.maxNbins);
      printf("PFA %lf\n", Config.FftAcq.pfa);
      printf("ACQTH %lf\n", Config.FftAcq.acqTh);
      printf("MAXNONCOHINTGPS %d\n", Config.FftAcq.maxNonCohInt_GPS);
      printf("MAXNONCOHINTGAL %d\n", Config.FftAcq.maxNonCohInt_GAL);
      printf("NMEDINT %d\n", Config.FftAcq.nMedInt);
      printf("NFINEINT %d\n", Config.FftAcq.nFineInt);
	  printf("FREQREFINEMENTFLAG %d\n", Config.FftAcq.freqRefinementFlag);
      printf("ACQFACTOR %d\n", Config.FftAcq.acqFactor);
      printf("SP3HIGHELEVN %d\n", Config.FftAcq.sp3HighElevN);

	  // Print aiding data header.
      printf("\n********************************\n");
      printf("INT SECTION\n");
      printf("********************************\n");

	  printf("NULL\n");

	  // Print aiding data header.
      printf("\n********************************\n");
      printf("AIDING SECTION\n");
      printf("********************************\n");

      // Print aiding data values.
      printf("REFPOS %lf %lf %lf\n", Config.Aid.refPos[0],
                                 Config.Aid.refPos[1],
                                 Config.Aid.refPos[2]);
	  printf("REFTOW %d\n", (int)Config.Aid.refTow);
      printf("REFWN %d\n", Config.Aid.refWn);
      printf("EPHFILE %s\n", Config.Aid.ephFile);
	  printf("GALEPHFILE %s\n", Config.Aid.galEphFile);

	  // Print lock data header.
      printf("\n********************************\n");
      printf("LOCK SECTION\n");
      printf("********************************\n");

      // Print lock data values.
	  printf("NLOCK %d\n", Config.Lck.nLock);
	  printf("CNOTH %lf\n", Config.Lck.cnoTh);
	  printf("C2PHITH %lf\n", Config.Lck.cliTh);
	  printf("LOSSCNOTH %lf\n", Config.Lck.lossCnoTh);
	  printf("VTCNOTH %lf\n", Config.Lck.vtCnoTh);
	  printf("VTNLOCK %d\n", Config.Lck.vtNLock);

	  // Print trackingdata header.
      printf("\n********************************\n");
      printf("PVT SECTION\n");
      printf("\n********************************\n");

	  printf("ELEVMASK %lf\n", Config.Pvt.elevMask);
	  printf("SMOLENGTH %d\n", Config.Pvt.smoLength);
	  printf("IONOCORRECTION %d\n", Config.Pvt.ionoCorrection);
	  printf("TROPOCORRECTION %d\n", Config.Pvt.tropoCorrection);
	  printf("CN0OUTFLAG %d\n", Config.Pvt.cn0outFlag);
	  printf("INTEGERTIMEMARKFLAG %d\n", Config.Pvt.integerTimeMarkFlag);
	  printf("CLKSTEERINGFLAG %d\n", Config.Pvt.clkSteeringFlag);
          
        	  // Print EKF header.
      printf("\n********************************\n");
      printf("EKF SECTION\n");
      printf("\n********************************\n");  

      printf("USEEKF %d\n", Config.Ekf.useEkf);
      printf("SPACEUSER %d\n", Config.Ekf.spaceUser);
      printf("DELTAT %f\n", Config.Ekf.deltaT);
      printf("MAHATHR %f\n", Config.Ekf.mahaThr);
      printf("MEASSIGMAPR %f\n", Config.Ekf.measSigmaPr);
      printf("MEASSIGMADP %f\n", Config.Ekf.measSigmaDp);
      printf("INITSTATESIGMAPOS %f\n", Config.Ekf.initStateSigmaPos);
      printf("INITSTATESIGMAVEL %f\n", Config.Ekf.initStateSigmaVel);
      printf("INITSTATESIGMACLKBIAS %f\n", Config.Ekf.initStateSigmaClkBias);
      printf("INITSTATESIGMACLKGALGPSBIAS %f\n", Config.Ekf.initStateSigmaClkGalGpsBias);
      printf("INITSTATESIGMACLKDRIFT %f\n", Config.Ekf.initStateSigmaClkDrift);
      printf("STATENOISECOVPOS %lf\n", Config.Ekf.stateNoiseCovPos);
      printf("STATENOISECOVVEL %f\n", Config.Ekf.stateNoiseCovVel);
      printf("STATENOISECOVCLKBIAS %f\n", Config.Ekf.stateNoiseCovClkBias);
      printf("STATENOISECOVCLKGALGPSBIAS %f\n", Config.Ekf.stateNoiseCovClkGalGpsBias);
      printf("STATENOISECOVCLKDRIFT %f\n", Config.Ekf.stateNoiseCovClkDrift);          
      
      // Print trackingdata header.
      printf("\n********************************\n");
      printf("TRACKING SECTION\n");
      printf("\n********************************\n");

      // Print tracking data values.
      printf("PLLORDER %d\n", Config.Tck.pllOrder);
      printf("DLLORDER %d\n", Config.Tck.dllOrder);
      printf("PLLBW %lf\n", Config.Tck.pllBw);
      printf("DLLBW %lf\n", Config.Tck.dllBw);
      printf("NDLL %d\n", Config.Tck.nDll);
      printf("NPLL %d\n", Config.Tck.nPll);
      printf("NDLL_GAL %d\n", Config.Tck.nDll_GAL);
      printf("NPLL_GAL %d\n", Config.Tck.nPll_GAL);
      printf("CHIPDELAY %lf\n", Config.Tck.chipDelay);
      printf("RESOLUTIONCARRIER %d\n", Config.Tck.resolutionCarrier);      
      printf("DECIMATEFACTOR %d\n", Config.Tck.decimateFactor);
      printf("CPINITIALIZATIONFLAG %d\n", Config.Tck.cpInitializationFlag);
  
	  printf("\n ");
}

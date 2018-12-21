
#include <stdlib.h>
#include <complexAPI.h>
#include <configuration.h>
#include <dataBuffer.h>
#include <date.h>
#include <threads.h>
//#include <trace.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#ifndef _rtems_app
#include <windows.h>
#endif

//#include <process.h>
#include <sys/timeb.h>

#include <interface.h>

//*****************************************************************************
//         MACROS DEFINITION
//*****************************************************************************
#define ALIVE_TICK_RATE  (100)

#define TIME_STAMP_RATE         (2000) // Works over the times data is written into the buffer
#define FFLUSH_TIME_STAMP_RATE  (1)    // Works over the times data is written into the file

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
//*****************************************************************************
//                     STRUCTURES DEFINITION
//*****************************************************************************
typedef struct
{
   char*        pchDataTwinBuff[2];
   unsigned int psiWrittenBytes[2];
   int          siWriteBuffId;
   int          siReadBuffId;
   unsigned int uiSize;

} SampleFileBuffer_st;

//*****************************************************************************
//                        GLOBAL EXTERNAL VARIABLES
//*****************************************************************************
extern T_ComData			ComData;
extern T_Config				Config;
extern S_rxProcessingStatus		rxProcessingStatus;                                        
extern circularBufferStruct		Core0DataBuf;        // CPU0
extern circularBufferStruct		Core1DataBuf;        // CPU1
extern circularBufferStruct		Core2DataBuf;        // CPU2
extern circularBufferStruct	        Core3DataBuf;        // CPU3

extern T_GenericSystemData	genSysData [NSYSTEMS];
extern T_supportData		supportData[NSYSTEMS];

extern S_Core                          core[NCORES];

//*****************************************************************************
//                        GLOBAL INTERNAL VARIABLES
//*****************************************************************************
// # samples needed in acquisition operations.
#ifndef _rtems_app
// Synchronization structures.
   CRITICAL_SECTION  ioBuffLock;			  // I/O buffer lock.
   HANDLE            semFull=NULL;			  // I/O buffer full semaphore.
   HANDLE            semEmpty=NULL;			  // I/O buffer empty semaphore.
#endif

ioBuffer_t	 ioBuffer;				  // IO buffer.

#ifndef _rtems_app
static LPVOID        pSharedMemory;			  // Pointer to FileMapping's shared memory to implement heart beat
static HANDLE        mappedFile;
#endif
static int           aliveTick;				  // Integer value whose changes serve as heart beat

static SampleFileBuffer_st gstFileBuff;		  // Pointer to the Buffer used to write samples into the file

static int  gsiWrittingFactor = 1;            // Factor between the buffer writting rate and the sampling rate
static int  gsiSampleFileCounter = 0;         // Counter of the splitted sample files.
static char gszRecordFileName[MAX_PATH_LEN];  // File name where record samples.

#ifndef _rtems_app
// Sample File Buffer Synchronization structures.
   CRITICAL_SECTION  ioSampFileBuffLock;      // I/O buffer lock.
   HANDLE            semReadyToWrite=NULL;    // I/O buffer full semaphore.
   HANDLE            ioInterProcessLock;      // I/O buffer lock.
#endif
//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
int  initializeIOBuffer(void);
int  initializeSampleFileBuffer(void);
void copyBuffer(void);
void StoreSamplesAndSwitchBuffer(unsigned char *refData, unsigned int *puiCounter);

//*****************************************************************************
//                        PUBLIC FUNCTIONS
//*****************************************************************************
/*****************************************************************

  int initializeDataBuffer(void)

 *****************************************************************/
int initializeDataBuffer(void)
{
   int ret			   = EXIT_OK; // Return code.

   int nSamplesPerRead     = 0;       // Used to compute # ms per read call.

   double auxDouble	   = 0;
   
   // Get # ms of data read in each call to read/receive samples.
   // Least Common Multiple between nSamples and 32 (number of bits per int word).
   nSamplesPerRead = 0;
   do
   {
      nSamplesPerRead = nSamplesPerRead + ComData.nSamples;
   } while ((nSamplesPerRead % NBITS_PER_WORD) != 0);

   auxDouble = ((double)(ComData.nSamplesIQ2)) / NBITS_PER_WORD;

   while (auxDouble - (int)(auxDouble) != 0)
   {
	   auxDouble = auxDouble * 2;
   }
   
   Core0DataBuf.resetCntTh = (int)auxDouble;
   Core1DataBuf.resetCntTh = (int)auxDouble;
   Core2DataBuf.resetCntTh = (int)auxDouble;
   Core3DataBuf.resetCntTh = (int)auxDouble;


   // Compute # integers to read in each read call.
   Core0DataBuf.nWordsToRead = nSamplesPerRead / NBITS_PER_WORD;
   Core1DataBuf.nWordsToRead = nSamplesPerRead / NBITS_PER_WORD;
   Core2DataBuf.nWordsToRead = nSamplesPerRead / NBITS_PER_WORD;
   Core3DataBuf.nWordsToRead = nSamplesPerRead / NBITS_PER_WORD;

   // Update # fields per read.   
   Core0DataBuf.accBytesExtra		  = 0;
   Core0DataBuf.nBytesToRead		  = Core0DataBuf.nWordsToRead*BYTES_PER_WORD; // BYTES_PER_WORD = 4
   Core0DataBuf.nMsPerRead		  = (int) ((Core0DataBuf.nBytesToRead*NBITS_PER_BYTE) / ComData.nSamplesIQ2);
   Core0DataBuf.nBytesExtra		  = ((Core0DataBuf.nBytesToRead*NBITS_PER_BYTE) - (ComData.nSamplesIQ2*Core0DataBuf.nMsPerRead)) / NBITS_PER_BYTE;

   Core1DataBuf.accBytesExtra		  = 0;
   Core1DataBuf.nBytesToRead		  = Core1DataBuf.nWordsToRead*BYTES_PER_WORD; // BYTES_PER_WORD = 4
   Core1DataBuf.nMsPerRead		  = (int) ((Core1DataBuf.nBytesToRead*NBITS_PER_BYTE) / ComData.nSamplesIQ2);
   Core1DataBuf.nBytesExtra		  = ((Core1DataBuf.nBytesToRead*NBITS_PER_BYTE) - (ComData.nSamplesIQ2*Core1DataBuf.nMsPerRead)) / NBITS_PER_BYTE;

   Core2DataBuf.accBytesExtra		  = 0;
   Core2DataBuf.nBytesToRead		  = Core2DataBuf.nWordsToRead*BYTES_PER_WORD; // BYTES_PER_WORD = 4
   Core2DataBuf.nMsPerRead		  = (int) ((Core2DataBuf.nBytesToRead*NBITS_PER_BYTE) / ComData.nSamplesIQ2);
   Core2DataBuf.nBytesExtra		  = ((Core2DataBuf.nBytesToRead*NBITS_PER_BYTE) - (ComData.nSamplesIQ2*Core2DataBuf.nMsPerRead)) / NBITS_PER_BYTE;   

   Core3DataBuf.accBytesExtra		  = 0;
   Core3DataBuf.nBytesToRead		  = Core3DataBuf.nWordsToRead*BYTES_PER_WORD; // BYTES_PER_WORD = 4
   Core3DataBuf.nMsPerRead                = (int) ((Core3DataBuf.nBytesToRead*NBITS_PER_BYTE) / ComData.nSamplesIQ2);
   Core3DataBuf.nBytesExtra		  = ((Core3DataBuf.nBytesToRead*NBITS_PER_BYTE) - (ComData.nSamplesIQ2*Core3DataBuf.nMsPerRead)) / NBITS_PER_BYTE;
   
   
   //printf("nSamplesPerRead %d nWordsToRead %d nBytesToRead %d nMsPerRead %d\n",nSamplesPerRead, Core3DataBuf.nWordsToRead, Core3DataBuf.nBytesToRead, Core3DataBuf.nMsPerRead);
   // Set buffer length as 2 times max( nAcqInt, nFineInt, nMedInt) and multiply of # ms per read call.
   ComData.lengthMs = 2;
   // Set length as multiple of # ms per read call.
   while (ComData.lengthMs % Core3DataBuf.nMsPerRead != 0)
   {
      ComData.lengthMs++;
   }
   
   // Set size in other useful units.
   ComData.lengthSamples = (ComData.lengthMs/Core3DataBuf.nMsPerRead)*Core3DataBuf.nBytesToRead*NBITS_PER_BYTE;
   ComData.lengthWords   =  ComData.lengthSamples / NBITS_PER_WORD;
     
   //printf("lengthMs %d lengthSamples %d lengthWords %d \n",ComData.lengthMs, ComData.lengthSamples, ComData.lengthWords);

#ifdef MEMORY_ALLOCATION    
   DataBuf.samplesWords  = (unsigned int *) calloc(ComData.lengthSamples / NBITS_PER_WORD, sizeof(unsigned int));
   DataBuf.samplesI		 = (unsigned int *) calloc(ComData.lengthSamples / NBITS_PER_WORD, sizeof(unsigned int));
   DataBuf.samplesQ		 = (unsigned int *) calloc(ComData.lengthSamples / NBITS_PER_WORD, sizeof(unsigned int));
#else  
    memset(Core0DataBuf.samplesWords ,0, sizeof(Core0DataBuf.samplesWords ));
    memset(Core0DataBuf.samplesI     ,0, sizeof(Core0DataBuf.samplesI     ));
    memset(Core1DataBuf.samplesWords ,0, sizeof(Core1DataBuf.samplesWords ));
    memset(Core2DataBuf.samplesWords ,0, sizeof(Core2DataBuf.samplesWords )); 
    memset(Core3DataBuf.samplesWords ,0, sizeof(Core3DataBuf.samplesWords )); 
#endif
    
   Core0DataBuf.byteVectorLength = ComData.lengthSamples / NBITS_PER_WORD*2;
   Core1DataBuf.byteVectorLength = ComData.lengthSamples / NBITS_PER_WORD*2;
   Core2DataBuf.byteVectorLength = ComData.lengthSamples / NBITS_PER_WORD*2;
   Core3DataBuf.byteVectorLength = ComData.lengthSamples / NBITS_PER_WORD*2;
 //printf("byteVectorLength %d\n",Core3DataBuf.byteVectorLength);
#ifdef MEMORY_ALLOCATION     
  if (DataBuf.samplesWords == NULL)
  {
      // Error allocating memory in data buffer.
      ret = EXIT_ER;
      printf("Error allocating memory in data buffer. \n");
  }
   else
   {
#endif       
      // Initializing data buffer variables.
      ComData.counterMs    = 0;
      Core0DataBuf.inIndexWords = 0;
      Core1DataBuf.inIndexWords = 0;
      Core2DataBuf.inIndexWords = 0;
      Core3DataBuf.inIndexWords = 0;

#ifdef MEMORY_ALLOCATION       
   }
#endif
   // Initialize IO buffer.
   if (ret == EXIT_OK)
   {
      ret = initializeIOBuffer();
   }

   return(ret);
}

/*****************************************

  void copyBuffer(void)

 *****************************************/
void copyBuffer(void)
{  
   // If # extra bytes accumulated are a block then skip reception from I/O buffer.
   if (Core3DataBuf.accBytesExtra >= Core3DataBuf.nBytesToRead)
   {
      // Reset # accumulated extra bytes.
      Core3DataBuf.accBytesExtra = Core3DataBuf.accBytesExtra - Core3DataBuf.nBytesToRead;
   }
   else
   {
	////////////////////////////////////////////
        // TRANSFER FROM IO BUFFER TO DATA BUFFER. /
	////////////////////////////////////////////
        //                              DEST                                        SRC                                    length
          //      printf("CPY1st: %d 0x%02x 0x%02x ... 0x%02x 0x%02x 0x%02x\n"   ,ComData.counterMs , Core3DataBuf.samplesWords[0]                        , Core3DataBuf.samplesWords[1]  , Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)-2]                        , Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)-1] , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4]);
          //      printf("CPY2st: %d 0x%02x 0x%02x ... 0x%02x 0x%02x 0x%02x\n\n" ,ComData.counterMs , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4], Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+1], Core3DataBuf.samplesWords[51150-2], Core3DataBuf.samplesWords[51150-1],Core3DataBuf.samplesWords[51150]);
             
             
        memcpy(&Core3DataBuf.samplesWords[Core3DataBuf.inIndexWords], &ioBuffer.samples[ioBuffer.outIndexBytes], Core3DataBuf.nBytesToRead);
        memcpy(&Core2DataBuf.samplesWords[Core2DataBuf.inIndexWords], &ioBuffer.samples[ioBuffer.outIndexBytes], Core2DataBuf.nBytesToRead);
        memcpy(&Core1DataBuf.samplesWords[Core1DataBuf.inIndexWords], &ioBuffer.samples[ioBuffer.outIndexBytes], Core1DataBuf.nBytesToRead);
        memcpy(&Core0DataBuf.samplesWords[Core0DataBuf.inIndexWords], &ioBuffer.samples[ioBuffer.outIndexBytes], Core0DataBuf.nBytesToRead);
        
        
        ////////////////////////////
        // UPDATE "OUT" IO BUFFER //
        ////////////////////////////
	 ioBuffer.outIndexBytes = ioBuffer.outIndexBytes + Core3DataBuf.nBytesToRead;

         // Check IO-BUFFER ROLL-OVER (in "OUT" bytes).
         if (ioBuffer.outIndexBytes == ioBuffer.lengthBytes)
         {
//                printf("CPY1st: %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",ComData.counterMs , refData_CPU3[0], refData_CPU3[1] , refData_CPU3[2], refData_CPU3[3], refData_CPU3[4], refData_CPU3[5] , refData_CPU3[6], refData_CPU3[7]);
//                printf("CPY2st: %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\n" ,ComData.counterMs, refData_CPU3[Core3DataBuf.nBytesToRead], refData_CPU3[Core3DataBuf.nBytesToRead+1] , refData_CPU3[Core3DataBuf.nBytesToRead+2], refData_CPU3[Core3DataBuf.nBytesToRead+3], refData_CPU3[Core3DataBuf.nBytesToRead +4], refData_CPU3[Core3DataBuf.nBytesToRead+5] , refData_CPU3[Core3DataBuf.nBytesToRead+6], refData_CPU3[Core3DataBuf.nBytesToRead+7]); 
//             
             //   printf("CPY1st: %d 0x%02x 0x%02x ... 0x%02x 0x%02x 0x%02x\n"   ,ComData.counterMs , Core3DataBuf.samplesWords[0]                        , Core3DataBuf.samplesWords[1]  , Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)-2]                        , Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)-1] , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4]);
             //   printf("CPY2st: %d 0x%02x 0x%02x ... 0x%02x 0x%02x 0x%02x\n\n\n" ,ComData.counterMs , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4], Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+1], Core3DataBuf.samplesWords[51150-2], Core3DataBuf.samplesWords[51150-1], Core3DataBuf.samplesWords[51150]);
             
           
             // Reset out index.
            ioBuffer.outIndexBytes = 0;
         }
         else
         {
//             printf("CPY1st: %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",ComData.counterMs , refData_CPU3[0], refData_CPU3[1] , refData_CPU3[2], refData_CPU3[3], refData_CPU3[4], refData_CPU3[5] , refData_CPU3[6], refData_CPU3[7]); 
//             printf("CPY2st: %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\n",ComData.counterMs, refData_CPU3[Core3DataBuf.nBytesToRead], refData_CPU3[Core3DataBuf.nBytesToRead+1] , refData_CPU3[Core3DataBuf.nBytesToRead+2], refData_CPU3[Core3DataBuf.nBytesToRead+3], refData_CPU3[Core3DataBuf.nBytesToRead +4], refData_CPU3[Core3DataBuf.nBytesToRead+5] , refData_CPU3[Core3DataBuf.nBytesToRead+6], refData_CPU3[Core3DataBuf.nBytesToRead+7]); 
//             
//               printf("CPY1st: %d 0x%02x 0x%02x \n"   ,ComData.counterMs , Core3DataBuf.samplesWords[0]                        , Core3DataBuf.samplesWords[1]                          );
//               printf("CPY2st: %d 0x%02x 0x%02x \n\n" ,ComData.counterMs , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4], Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+1]);
         }

         /////////////////////////////
         // UPDATE "IN" DATA BUFFER //
         /////////////////////////////

         // Update Data-Buffer index
         Core3DataBuf.inIndexWords = Core3DataBuf.inIndexWords + Core3DataBuf.nWordsToRead;
         // Check DATA-BUFFER ROLL-OVER (in "IN" ms).
         if (Core3DataBuf.inIndexWords == ComData.lengthWords)
         {
            Core3DataBuf.inIndexWords = 0;
         }
         
         Core2DataBuf.inIndexWords = Core2DataBuf.inIndexWords + Core2DataBuf.nWordsToRead;
         // Check DATA-BUFFER ROLL-OVER (in "IN" ms).
         if (Core2DataBuf.inIndexWords == ComData.lengthWords)
         {
            Core2DataBuf.inIndexWords = 0;
         }
         
         Core1DataBuf.inIndexWords = Core1DataBuf.inIndexWords + Core1DataBuf.nWordsToRead;
         // Check DATA-BUFFER ROLL-OVER (in "IN" ms).
         if (Core1DataBuf.inIndexWords == ComData.lengthWords)
         {
            Core1DataBuf.inIndexWords = 0;
         }

        Core0DataBuf.inIndexWords = Core0DataBuf.inIndexWords + Core0DataBuf.nWordsToRead;
        // Check DATA-BUFFER ROLL-OVER (in "IN" ms).
        if (Core0DataBuf.inIndexWords == ComData.lengthWords)
        {
           Core0DataBuf.inIndexWords = 0;
        }      
   
         // Update extra bytes read/received.
         Core3DataBuf.accBytesExtra = Core3DataBuf.accBytesExtra + Core3DataBuf.nBytesExtra;

         // Signal if buffer was full.
         signalIfFull();
    //  }
   }
}

void copyBufferAcq(int word)
{
    int ioBufferIdx = 0,
        samplesToEnd = 0,
        samplesAtStart = 0;    
    //printf("  %d %d stat %d act %d idx %d %d %d %d %d\n", ComData.counterMs, supportData[rxProcessingStatus.acqGalFlag].acqAux_P[0].acqCounterMs, core[0].status, core[0].activity, ioBuffer.outIndexBytes, Core3DataBuf.inIndexWords, Core2DataBuf.inIndexWords, Core1DataBuf.inIndexWords, Core0DataBuf.inIndexWords);

   // refData_CPU0  = (unsigned char *) Core0DataBuf.samplesI;      

//    if(word - 25575 < 0)
//    {
//        word = 25575*2+(word - 25575);
//    }
//    else
//    {
//        word = word - 25575;
//    }
    ioBufferIdx = word * BYTES_PER_WORD;
 //   printf("ioBufferIdx %d \n", ioBufferIdx);
    
    if(ioBufferIdx + Core0DataBuf.nBytesToRead >= ioBuffer.lengthBytes)
    {
       // printf("sum %d \n", ioBufferIdx + Core0DataBuf.nBytesToRead);
      //  printf("n ok\n");
       samplesToEnd = ioBuffer.lengthBytes - ioBufferIdx;
       memcpy(&Core0DataBuf.samplesI[0], &ioBuffer.samples[ioBufferIdx], samplesToEnd);
     
  //  printf("IO    : %d %d %d 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x\n",ComData.counterMs, word, ioBufferIdx, ioBuffer.samples[ioBufferIdx], ioBuffer.samples[ioBufferIdx+1] , ioBuffer.samples[ioBufferIdx+2], ioBuffer.samples[ioBufferIdx+3], ioBuffer.samples[ioBufferIdx+4], ioBuffer.samples[ioBufferIdx+5] , ioBuffer.samples[ioBufferIdx+6], ioBuffer.samples[ioBufferIdx+7],
   //                                                                                                                           ioBuffer.samples[ioBufferIdx+8], ioBuffer.samples[ioBufferIdx+9] , ioBuffer.samples[ioBufferIdx+10], ioBuffer.samples[ioBufferIdx+11], ioBuffer.samples[ioBufferIdx+12], ioBuffer.samples[ioBufferIdx+13] , ioBuffer.samples[ioBufferIdx+14], ioBuffer.samples[ioBufferIdx+15]);                 
  //  printf("CPYACQ: %d %d %d 0x%x 0x%x 0x%x 0x%x \n\n" ,ComData.counterMs , word, ioBufferIdx, Core0DataBuf.samplesI[0], Core0DataBuf.samplesI[1], Core0DataBuf.samplesI[2], Core0DataBuf.samplesI[3]);   
          
       ioBufferIdx = 0;
       samplesAtStart = Core0DataBuf.nBytesToRead - samplesToEnd;
       memcpy(&Core0DataBuf.samplesI[samplesToEnd / BYTES_PER_WORD], &ioBuffer.samples[ioBufferIdx], samplesAtStart);  
      // printf("SamplesAtStart %d \n", SamplesAtStart);
       
       
    }
    else
    {
       // printf("ioBufferIdx %d\n", ioBufferIdx);
        memcpy(&Core0DataBuf.samplesI[0], &ioBuffer.samples[ioBufferIdx], Core0DataBuf.nBytesToRead);
   
   //    printf("IO    : %d %d %d 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x\n",ComData.counterMs, word, ioBufferIdx, ioBuffer.samples[ioBufferIdx], ioBuffer.samples[ioBufferIdx+1] , ioBuffer.samples[ioBufferIdx+2], ioBuffer.samples[ioBufferIdx+3], ioBuffer.samples[ioBufferIdx+4], ioBuffer.samples[ioBufferIdx+5] , ioBuffer.samples[ioBufferIdx+6], ioBuffer.samples[ioBufferIdx+7],
   //                                                                                                                           ioBuffer.samples[ioBufferIdx+8], ioBuffer.samples[ioBufferIdx+9] , ioBuffer.samples[ioBufferIdx+10], ioBuffer.samples[ioBufferIdx+11], ioBuffer.samples[ioBufferIdx+12], ioBuffer.samples[ioBufferIdx+13] , ioBuffer.samples[ioBufferIdx+14], ioBuffer.samples[ioBufferIdx+15]);                 
  //      printf("CPYACQ: %d %d %d 0x%x 0x%x 0x%x 0x%x \n\n" ,ComData.counterMs , word, ioBufferIdx, Core0DataBuf.samplesI[0], Core0DataBuf.samplesI[1], Core0DataBuf.samplesI[2], Core0DataBuf.samplesI[3]);   
        
//        printf("IO1st : %d 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x\n",ComData.counterMs , ioBuffer.samples[0], ioBuffer.samples[1] , ioBuffer.samples[2], ioBuffer.samples[3], ioBuffer.samples[4], ioBuffer.samples[5] , ioBuffer.samples[6], ioBuffer.samples[7],
//                                                                                                                              ioBuffer.samples[8], ioBuffer.samples[9] , ioBuffer.samples[10], ioBuffer.samples[11], ioBuffer.samples[12], ioBuffer.samples[13] , ioBuffer.samples[14], ioBuffer.samples[15]);
//        printf("IO2nd : %d 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x\n",ComData.counterMs, ioBuffer.samples[Core3DataBuf.nBytesToRead], ioBuffer.samples[Core3DataBuf.nBytesToRead+1] , ioBuffer.samples[Core3DataBuf.nBytesToRead+2], ioBuffer.samples[Core3DataBuf.nBytesToRead+3], ioBuffer.samples[Core3DataBuf.nBytesToRead+4], ioBuffer.samples[Core3DataBuf.nBytesToRead+5] , ioBuffer.samples[Core3DataBuf.nBytesToRead+6], ioBuffer.samples[Core3DataBuf.nBytesToRead+7],
//                                                                                                                             ioBuffer.samples[Core3DataBuf.nBytesToRead + 8], ioBuffer.samples[Core3DataBuf.nBytesToRead+9] , ioBuffer.samples[Core3DataBuf.nBytesToRead+10], ioBuffer.samples[Core3DataBuf.nBytesToRead+11], ioBuffer.samples[Core3DataBuf.nBytesToRead+12], ioBuffer.samples[Core3DataBuf.nBytesToRead+13] , ioBuffer.samples[Core3DataBuf.nBytesToRead+14], ioBuffer.samples[Core3DataBuf.nBytesToRead+15]);
//          printf("CPY1st: %d 0x%02x 0x%02x 0x%02x 0x%02x\n"   ,ComData.counterMs , Core3DataBuf.samplesWords[0]                        , Core3DataBuf.samplesWords[1],  Core3DataBuf.samplesWords[2] , Core3DataBuf.samplesWords[3]                           );
//          printf("CPY2st: %d 0x%02x 0x%02x 0x%02x 0x%02x\n" ,ComData.counterMs , Core3DataBuf.samplesWords[Core3DataBuf.nBytesToRead/4], Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+1],Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+2] , Core3DataBuf.samplesWords[(Core3DataBuf.nBytesToRead/4)+3]);                
        
       //printf("CPYACQ: %d 0x%x 0x%x 0x%x 0x%x ... 0x%x\n\n" ,ComData.counterMs , Core0DataBuf.samplesI[0], Core0DataBuf.samplesI[1], Core0DataBuf.samplesI[2], Core0DataBuf.samplesI[3], Core0DataBuf.samplesI[51150]);                                                                                     //             , refData_CPU0[4], refData_CPU0[5] , refData_CPU0[6], refData_CPU0[7]); 
    }
}

/******************************************

 StoreSamplesAndSwitchBuffer

 ******************************************/
#ifndef _rtems_app
void StoreSamplesAndSwitchBuffer(unsigned char *refData, unsigned int *puiCounter)
{
   unsigned int uiBytesToWrite	  = 0;
   char *pchBuff				  = NULL;
   int   siBytesWrittenInBuff	  = 0;
   static int siAccumWrittenBytes = 0;
#ifndef LINUX
   struct tm     *timeStruct;           // Struct information time.
   struct _timeb  tbThisTime;           // Struct system time with ms.
   FILETIME       ftCurrentTime;
   clock_t        tProcClock;
   LARGE_INTEGER  liPerformanceCount;
   int            resQuery;
#endif

   if ((*puiCounter)%TIME_STAMP_RATE == 0)
   {
      // Get time stamp just after it finishes waiting
      _ftime(&tbThisTime);
      timeStruct = gmtime(&(tbThisTime.time));

      GetSystemTimeAsFileTime(&ftCurrentTime);

	  tProcClock = clock();
      resQuery   = QueryPerformanceCounter(&liPerformanceCount);
   }

   // Store samples into the sample buffer to be written into the sample file
   uiBytesToWrite       = BYTES_PER_WORD * DataBuf.nWordsToRead;
   pchBuff              = gstFileBuff.pchDataTwinBuff[gstFileBuff.siWriteBuffId];
   siBytesWrittenInBuff = gstFileBuff.psiWrittenBytes[gstFileBuff.siWriteBuffId];
   if ((siBytesWrittenInBuff + uiBytesToWrite) <= gstFileBuff.uiSize) {
      memcpy(&(pchBuff[siBytesWrittenInBuff]),refData, uiBytesToWrite);
      siBytesWrittenInBuff = siBytesWrittenInBuff + uiBytesToWrite;
      siAccumWrittenBytes  = siAccumWrittenBytes + uiBytesToWrite;
   }
   gstFileBuff.psiWrittenBytes[gstFileBuff.siWriteBuffId] = siBytesWrittenInBuff;

   if ((*puiCounter)%TIME_STAMP_RATE == 0)
   {
      // Store time stamp
//       fprintf(ioBuffer.fdTStamp,"%7u %5u %8u %10u %10u %10u %04d/%02d/%02d %02d:%02d:%02d.%03hu\n",
//          (*puiCounter)/TIME_STAMP_RATE,TIME_STAMP_RATE, siAccumWrittenBytes, ftCurrentTime.dwHighDateTime, ftCurrentTime.dwLowDateTime, tProcClock,
//          timeStruct->tm_year+1900,timeStruct->tm_mon+1,timeStruct->tm_mday,timeStruct->tm_hour,timeStruct->tm_min,timeStruct->tm_sec,
//          tbThisTime.millitm);

      siAccumWrittenBytes = 0;
   }

   // Increment counter
   *puiCounter = (*puiCounter) + 1;
   // Make the thread to write samples into the file
   if (((*puiCounter)%(Config.Com.sampleWritingRate) == 0) &&
       (gstFileBuff.pchDataTwinBuff[0] != NULL) &&
       (gstFileBuff.pchDataTwinBuff[1] != NULL))
   {

      // Lock sample buffer mutex
      EnterCriticalSection( &ioSampFileBuffLock);

      // Switch the Twin buffer
      gstFileBuff.siWriteBuffId = (gstFileBuff.siWriteBuffId + 1) % 2;
      gstFileBuff.siReadBuffId  = (gstFileBuff.siReadBuffId + 1) % 2;
      gstFileBuff.psiWrittenBytes[gstFileBuff.siWriteBuffId] = 0;

      // Flush time stamps
      if (((*puiCounter)/(Config.Com.sampleWritingRate)%FFLUSH_TIME_STAMP_RATE == 0))
      {
         fflush(ioBuffer.fdTStamp);
      }

      // Send a signal to the thread writing sample file and unnlock the sample buffer mutex
      ReleaseSemaphore( semReadyToWrite, 1, NULL);
      LeaveCriticalSection( &ioSampFileBuffLock);

   }

   // Keep-Alive signal:
   if (((*puiCounter)%ALIVE_TICK_RATE == 0) && (pSharedMemory != NULL))
   {
	   aliveTick++;
      CopyMemory(pSharedMemory, &aliveTick, sizeof(int));
   }

   return;
}
#endif
/************************************************************

  void UpdateOutIndex(T_supportData *supportData_P)

 ************************************************************/
void UpdateOutIndex(T_supportData *supportData_P)
{
   int auxIndex = 0;
   //printf("przed + word %d " , supportData_P->outIndex.word);
   supportData_P->outIndex.bit  = supportData_P->outIndex.bit  + supportData_P->outSize.bit;
   supportData_P->outIndex.word = supportData_P->outIndex.word + supportData_P->outSize.word;

   // Check BIT ROLL-OVER
   if (supportData_P->outIndex.bit >= NBITS_PER_WORD)
   {
      supportData_P->outIndex.bit = supportData_P->outIndex.bit % NBITS_PER_WORD;
      supportData_P->outIndex.word++;
   }

   // Check BUFFER ROLL-OVER
   auxIndex = supportData_P->outIndex.bit + supportData_P->outIndex.word*NBITS_PER_WORD;
 //printf(" %d %d %d \n" ,supportData_P->outIndex.word,  auxIndex, ComData.lengthSamples);
   if (auxIndex >= ComData.lengthSamples)
   {
      auxIndex = auxIndex - ComData.lengthSamples;

      // Reset index.
      supportData_P->outIndex.bit  = auxIndex % NBITS_PER_WORD;
      supportData_P->outIndex.word = auxIndex / NBITS_PER_WORD;
   }
}

//*****************************************************************************
//   Name:         finalizeDataBuffer
//   Purpose:      Free all module resources.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      DataBuf         samples buffer.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/21   -   JJNC   -   Created.
//*****************************************************************************
#ifdef MEMORY_ALLOCATION 
void finalizeDataBuffer(void)
{
#ifdef WRITE_SAMPLE_FILE_ASYNCHRONOUSLY
   DWORD dwNumBytesWrittenInFile;
#endif

   // Check if buffer already deallocated,
   if (DataBuf.samplesWords != NULL)
   {
      // Free memory.
      free(DataBuf.samplesWords);

      // Dereference data buffer.
      DataBuf.samplesWords = NULL;
   }

   if (DataBuf.samplesI != NULL)
   {
      // Free memory.
      free(DataBuf.samplesI);
      free(DataBuf.samplesQ);

      // Dereference data buffer.
      DataBuf.samplesI     = NULL;
      DataBuf.samplesQ     = NULL;
   }

   if (DataBuf.byteVectorI != NULL)
   {
      // Free memory.
      free(DataBuf.byteVectorI);
      free(DataBuf.byteVectorQ);

      // Dereference data buffer.
      DataBuf.byteVectorI = NULL;
      DataBuf.byteVectorQ = NULL;
   }

   // Free IO buffer.
   if (ioBuffer.samples != NULL)
   {
      free(ioBuffer.samples);
   }
#ifndef _rtems_app
    #ifdef LINUX
    // Destroy lock.
    pthread_mutex_destroy( &ioBuffLock);
    pthread_mutex_destroy( &ioSampFileBuffLock);

    // Destroy conditions.
    pthread_cond_destroy( &fullCond);
    pthread_cond_destroy( &emptyCond);
    pthread_cond_destroy( &readyToWriteCond);
    #else
    // Destroy IO buffer lock.
    DeleteCriticalSection( &ioBuffLock);
    DeleteCriticalSection( &ioSampFileBuffLock);

    // Destroy io buffer semaphores.
    CloseHandle(semFull);
    CloseHandle(semEmpty);
    CloseHandle(semReadyToWrite);
    CloseHandle(ioInterProcessLock);
    #endif
#endif  

}
#endif
/**************************************************************************************************

  void getAcquisitionData(struct complex_t *target, int lengthNeeded, T_supportData *supportData_P)

 **************************************************************************************************/
void getAcquisitionData(struct complex_t *target, int lengthNeeded, T_supportData *supportData_P, int core_index)
{
   int      i = 0,
			j = 0,           // Loop indexes.
			sampleIndex = 0, // Sample index.
			nItemsStart = 0,
			nItemsEnd   = 0; // Number of items to read from acquisition buffer.

   unsigned int mask = 0;    // Mask to select sample from buffer.

   sampleIndex_t index;        // Index to reference samples into buffer.

   // Compute index of first sample needed.
   sampleIndex = supportData_P->acqAux_P[core_index].outIndex.word*NBITS_PER_WORD + supportData_P->acqAux_P[core_index].outIndex.bit;
//printf("%d %d %d \n", sampleIndex, supportData_P->acqAux_P[core_index].outIndex.word, supportData_P->acqAux_P[core_index].outIndex.bit);
   // Index out of bounds (low side).
   if (sampleIndex < 0)
   {
      sampleIndex = ComData.lengthSamples + sampleIndex;
   }
   // Index out of bounds (high side).
   else if (sampleIndex >= ComData.lengthSamples)
   {
      sampleIndex = sampleIndex % ComData.lengthSamples;
   }

   // Translate sample into bit format index.
   index.bit  = sampleIndex % NBITS_PER_WORD;
   index.word = sampleIndex / NBITS_PER_WORD;

   //printf("GETACQ: %d %d %d 0x%x 0x%x 0x%x 0x%x \n\n" ,ComData.counterMs , index.word, sampleIndex, Core0DataBuf.samplesI[index.word], Core0DataBuf.samplesI[index.word+1], Core0DataBuf.samplesI[index.word+2], Core0DataBuf.samplesI[index.word+3]);   
          
   // Check if needed data from the beginning of the buffer.
   if ((sampleIndex + lengthNeeded) > ComData.lengthSamples)
   {
      // Number of items to copy from the end.
      nItemsEnd = ComData.lengthSamples - sampleIndex;

      // Number of items to copy from the beginning.
      nItemsStart = lengthNeeded - nItemsEnd;

      // Initialize mask.
      mask = 0x80000000;
      mask = mask >> index.bit;

      // 1st copy from current sample until end of buffer.
      i = 0;
      while (i<nItemsEnd)
      {
         // Copy sample into buffer.
         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].x =  1;
         }
         else
         {
            target[i].x = -1;
         }

         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].y =  1;
         }
         else
         {
            target[i].y = -1;
         }

         // Compute index value of next sample to copy.
         index.bit++;
         if (index.bit == NBITS_PER_WORD)
         {
            index.bit = 0;
            index.word++;
            mask = (unsigned int) 0x80000000;
         }
         else
         {
            // Update mask.
            mask = mask >> 1;
         }

         // Next sample.
         i++;
      }

      // Set index to zero to access first sample from buffer.
      index.bit  = 0;
      index.word = 0;

      // Initialize mask.
      mask = 0x80000000;
      mask = mask >> index.bit;

      // 2nd copy from the beginning of the buffer.
      j = 0;
      while (j<nItemsStart)
      {
         // Copy sample into buffer.
         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].x =  1;
         }
         else
         {
            target[i].x = -1;
         }

         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].y =  1;
         }
         else
         {
            target[i].y = -1;
         }

         // Compute index value of next sample to copy.
         index.bit++;
         if (index.bit == NBITS_PER_WORD)
         {
            index.bit = 0;
            index.word++;
            mask = (unsigned int) 0x80000000;
         }
         else
         {
            // Update mask.
            mask = mask >> 1;
         }

         // Next sample.
         i++;
         j++;
      }
   }
   else
   {
      // Initialize mask.
      mask = 0x80000000;
      mask = mask >> index.bit;

      // Copy samples from buffer to target.
      i = 0;
      while (i<lengthNeeded)
      {
         // Copy sample into buffer.
         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].x =  1;
         }
         else
         {
            target[i].x = -1;
         }

         if (Core0DataBuf.samplesI[index.word] & mask)
         {
            target[i].y =  1;
         }
         else
         {
            target[i].y = -1;
         }

         // Compute index value of next sample to copy.
         index.bit++;
         if (index.bit == NBITS_PER_WORD)
         {
            index.bit = 0;
            index.word++;
            mask = 0x80000000;
         }
         else
         {
            // Update mask.
            mask = mask >> 1;
         }

         // Next sample.
         i++;
      }
   }
}

//*****************************************************************************
//   Name:         readData
//   Purpose:      this function reads data from file.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         0 if succes
//                 -1 i.o.c.
//      Global:  error               communications error flag.
//               msgText             trace error extra data structure
//               sockDesc            socket descriptor.
//               address             socket address structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/08/26   -   JJNC   -   Created.
//*****************************************************************************
#ifndef _rtems_app
unsigned int WINAPI readData(LPVOID lpParam)
{
   int endFlag = FALSE;      // Loop control flag.

   // Set thread pirority.
   HANDLE hProcess = GetCurrentProcess();
   SetPriorityClass( hProcess , NORMAL_PRIORITY_CLASS );
   SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_NORMAL);

   // Check error in previous call.
   while (!endFlag)
   {
      // Wait if buffer is full.
      waitIfFull();

      // Read samples from file.
      if (readSamples() == EXIT_ER)
      {
         // Release semaphore to synchronize receiver thread.
         ReleaseSemaphore( semEmpty, 1, NULL);

         // Update loop control flag.
         endFlag = TRUE;
      }
      else
      {
         // Signal if buffer was empty.
         signalEmpty();

         // Set up next reception.
         setUpReception();
      }
   }

   // Exit thread.
   _endthreadex(0);

   return(0);
}
#endif
//*****************************************************************************
//   Name:         writeSampleFile
//   Purpose:      this function writes sample data into file.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         0 if succes
//                 -1 i.o.c.
//      Global:  error               communications error flag.
//               msgText             trace error extra data structure
//               sockDesc            socket descriptor.
//               address             socket address structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//*****************************************************************************
#ifndef _rtems_app
unsigned int WINAPI writeSampleFile(LPVOID lpParam)
{
   int endFlag				     = FALSE;      // Loop control flag.
   int siWriteRes			     = FALSE;
   char *pchBuff				 = NULL;
   int   siBytesWrittenInBuff = 0;
   char  recordFileName[MAX_PATH_LEN];   // File name where record samples.

   __int64 slWriteSampleBytesCounter = 0;

#ifdef WRITE_SAMPLE_FILE_ASYNCHRONOUSLY
   DWORD dwLastError;
#endif

   // Set thread pirority.
   HANDLE hProcess = GetCurrentProcess();
   SetPriorityClass( hProcess , NORMAL_PRIORITY_CLASS );
   SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_NORMAL);

   // Check error in previous call.
   while (!endFlag)
   {
      // Wait until buffer is ready to write samples into the file and then lock sample buffer mutex
      WaitForSingleObject( semReadyToWrite, INFINITE);
      EnterCriticalSection( &ioSampFileBuffLock);

      // Wait until interprocess mutex is released (to synchronize multiple writes into disk)
      if (ioInterProcessLock != NULL) {
         WaitForSingleObject( ioInterProcessLock, INFINITE);
      }

      // Write the sample data into the file
      // Write the data contained in the read buffer into the file
      pchBuff              = gstFileBuff.pchDataTwinBuff[gstFileBuff.siReadBuffId];
      siBytesWrittenInBuff = gstFileBuff.psiWrittenBytes[gstFileBuff.siReadBuffId];
#ifdef WRITE_SAMPLE_FILE_ASYNCHRONOUSLY
      DWORD dwNumBytesWrittenInFile = 0;

      siWriteRes = WriteFile( ioBuffer.hFile, pchBuff, siBytesWrittenInBuff,
                              NULL, &(ioBuffer.overlapped));
      // Check that the previous write has finished
      int siGetRes = GetOverlappedResult(ioBuffer.hFile, &(ioBuffer.overlapped), &dwNumBytesWrittenInFile, FALSE);
      if (siGetRes == FALSE) {
        dwLastError = GetLastError();
        // Wait for the previous write to finish
        siGetRes = GetOverlappedResult(ioBuffer.hFile, &(ioBuffer.overlapped), &dwNumBytesWrittenInFile, TRUE);
      }
      if (siGetRes == FALSE) {
         // Update loop control flag.
         endFlag = TRUE;
      }
#else

      if (Config.Com.splitSampleFileRate > 0)
      {
         // Increment the writting counter
         slWriteSampleBytesCounter += siBytesWrittenInBuff;
         // Check whether a new sample file has to be created
         if ((ioBuffer.sampleFileDescriptor != NULL) &&
             ((slWriteSampleBytesCounter / (Config.Com.sampFreq/NBITS_PER_BYTE)) >= (Config.Com.splitSampleFileRate * 60)))
         {
            // Reset writing counter
            slWriteSampleBytesCounter -= ((int) (Config.Com.splitSampleFileRate * 60 * (Config.Com.sampFreq/NBITS_PER_BYTE)));
            // Increment file counter
            gsiSampleFileCounter++;
            // Close previous file
            fclose(ioBuffer.sampleFileDescriptor);
            // Create the new file
            sprintf(recordFileName,"%s%s%03d", gszRecordFileName, CHUNK_STR, gsiSampleFileCounter);
            if ((ioBuffer.sampleFileDescriptor = fopen( recordFileName,"wb")) == NULL)
            {
               endFlag = TRUE;
            }
         }
      }

      siWriteRes = fwrite( pchBuff, 1, siBytesWrittenInBuff, ioBuffer.sampleFileDescriptor);
      if (siWriteRes != siBytesWrittenInBuff) {
         // Update loop control flag.
         endFlag = TRUE;
      }
#endif


      // Release interprocess mutex
      if (ioInterProcessLock != NULL) {
         ReleaseMutex( ioInterProcessLock);
      }

      // Release sample buffer mutex
      LeaveCriticalSection( &ioSampFileBuffLock);
   } // End while

   // Exit thread.
   _endthreadex(0);

   return(0);
}
#endif
//*****************************************************************************
//   Name:         initializeSampleFileBuffer
//   Purpose:      Initializes sample file buffer.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         0               if success
//                -1               i.o.c.
//      Global:  ioBuffer          IO buffer.
//               DataBuf            Data buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//*****************************************************************************
#ifndef _rtems_app
int initializeSampleFileBuffer(void)
{
   int  ret=EXIT_OK;                    // Function return value.

   // Create critical section.
   InitializeCriticalSection( &ioSampFileBuffLock);

   // Create semaphores.
   if ((semReadyToWrite = CreateSemaphore( NULL, 0, 1, NULL)) == NULL)
   {
      // Error creating semaphore.
      ret = EXIT_ER;
   }

   // Open inter-process mutex
   ioInterProcessLock = OpenMutex( MUTEX_ALL_ACCESS, FALSE, TEXT("SRX_TRITON_WRITE_FILE_MUTEX"));
   return(ret);
}
#endif
//*****************************************************************************
//   Name:         initializeIOBuffer
//   Purpose:      Initializes IO buffer.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         0               if success
//                -1               i.o.c.
//      Global:  ioBuffer          IO buffer.
//               DataBuf            Data buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/08/13   -   JJNC   -   Created.
//*****************************************************************************
int initializeIOBuffer(void)
{
   int  ret=EXIT_OK;                    // Function return value.
   char recordFileName[MAX_PATH_LEN];   // File name where record samples.
   time_t stThisTime=0;                 // Standard time in seconds.
   struct tm *timeStruct;               // Struct information time.
   char szAuxTag[MAX_PATH_LEN];

   time(&stThisTime);

   timeStruct = gmtime(&stThisTime);

   // Initialize reception buffer variables.
   if (Config.Com.mode <= POSTPROCESS)
   {
      ioBuffer.maxRecp     = FILE_N_RECEPTIONS;
      ioBuffer.packPerRecp = FILE_N_PACKAGES;
   }
   else
   {
      ioBuffer.maxRecp     = RT_N_RECEPTIONS;
      ioBuffer.packPerRecp = RT_N_PACKAGES;
   }
   ioBuffer.totalPack = ioBuffer.maxRecp*ioBuffer.packPerRecp;

#ifdef MEMORY_ALLOCATION   
   // Allocate buffer.
   ioBuffer.samples = (unsigned char *) malloc(DataBuf.nBytesToRead*ioBuffer.totalPack);
   if (ioBuffer.samples == NULL)
   {
     ret = EXIT_ER;
      
   }
   else
   {
      // Initialize io buffer control variables.
      ioBuffer.inIndexBytes    = 0;
      ioBuffer.outIndexBytes   = 0;
      ioBuffer.lengthBytes     = DataBuf.nBytesToRead*ioBuffer.totalPack;
      ioBuffer.nBytesToReceive = DataBuf.nBytesToRead*ioBuffer.packPerRecp;
      ioBuffer.nFree           = ioBuffer.totalPack;
    #ifndef _rtems_app
        // Create semaphores.
        InitializeCriticalSection( &ioBuffLock);

        if ((semFull = CreateSemaphore( NULL, 0, 1, NULL)) == NULL)
        {
            // Error creating semaphore.
            ret = EXIT_ER;
        }
        else if ((semEmpty = CreateSemaphore( NULL, 0, 1, NULL)) == NULL)
        {
            // Error creating semaphore.
            ret = EXIT_ER;
        }
    }

    // Open samples file if user request to record samples.
    ioBuffer.sampleFileDescriptor = NULL;
    ioBuffer.fdTStamp			 = NULL;
    
        #ifndef LINUX
        ioBuffer.hFile				 = INVALID_HANDLE_VALUE;
        #endif
    #else       //_rtems_app
        }       
    #endif      //_rtems_app
#else           // MEMORY_ALLOCATION
// printf("%d %d %d %d \n",N_BITES_TOTAL_PACK, DataBuf.nBytesToRead, ioBuffer.totalPack, DataBuf.nBytesToRead*ioBuffer.totalPack );
   if ( N_BITES_TOTAL_PACK < (Core3DataBuf.nBytesToRead*ioBuffer.totalPack) )
   {
		// MEMORY problems, stop process and inform the user
		ret = EXIT_ER;
   }
   
   memset(ioBuffer.samples,0,sizeof(ioBuffer.samples));
   // Initialize io buffer control variables.
   ioBuffer.inIndexBytes    = 0;
   ioBuffer.outIndexBytes   = 0;
   ioBuffer.lengthBytes     = Core3DataBuf.nBytesToRead*ioBuffer.totalPack;
   ioBuffer.nBytesToReceive = Core3DataBuf.nBytesToRead*ioBuffer.packPerRecp;
   ioBuffer.nFree           = ioBuffer.totalPack;
#endif          // MEMORY_ALLOCATION
   return(ret);
}

//*****************************************************************************
//   Name:         waitIfFull
//   Purpose:      Check if IO buffer is full and blocks thread if so.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      USBDeviceGlobal         IO buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
#ifndef _rtems_app
void waitIfFull(void)
{
   // Get IO buffer lock.
   EnterCriticalSection( &ioBuffLock);

   // Check if buffer is full.
   while (ioBuffer.nFree < ioBuffer.packPerRecp)
   {
      // Release IO buffer lock.
      LeaveCriticalSection( &ioBuffLock);

      // Block IO thread.
      WaitForSingleObject( semFull, INFINITE);

      // Get IO buffer lock.
      EnterCriticalSection( &ioBuffLock);
   }

   // Release IO buffer lock.
   LeaveCriticalSection( &ioBuffLock);
}
#endif
//*****************************************************************************
//   Name:         signalIfFull
//   Purpose:      Signal blocked thread if buffer is empty.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:  semFull               wait condition
//               ioBuffLock            I/O buffer.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
void signalIfFull(void)
{
#ifndef _rtems_app
   // Get IO buffer lock.
   EnterCriticalSection( &ioBuffLock);
#endif
   // Update # free positions in buffer.
   ioBuffer.nFree++;
#ifndef _rtems_app
   // Check if buffer was full.
   if (ioBuffer.nFree == ioBuffer.packPerRecp)
   {
      // Signal blocked thread.
      ReleaseSemaphore( semFull, 1, NULL);
   }

   // Release IO buffer lock.
   LeaveCriticalSection( &ioBuffLock);
#endif
}

//*****************************************************************************
//   Name:         waitIfEmpty
//   Purpose:      Check if IO buffer is full or the status changes to
//                 END_EXECUTION or NO_MORE_SAMPLES, and then blocks thread if so.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      USBDeviceGlobal         IO buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
#ifndef _rtems_app
void waitIfEmpty(void)
{
   // Get Execution Status
   int status = getExecutionStatus();

   // Get IO buffer lock.
   EnterCriticalSection( &ioBuffLock);

   // Check if buffer is empty.
   while ((ioBuffer.nFree == ioBuffer.totalPack) && (status != END_EXECUTION) && (status != NO_MORE_SAMPLES))
   {
      // Release IO buffer lock.
      LeaveCriticalSection( &ioBuffLock);

      // Block IO thread.
      WaitForSingleObject( semEmpty, 2*1000);

      // Get Execution Status
      status = getExecutionStatus();

      // Get IO buffer lock.
      EnterCriticalSection( &ioBuffLock);
   }

   // Release IO buffer lock.
   LeaveCriticalSection( &ioBuffLock);
}
#endif
//*****************************************************************************
//   Name:         signalEmpty
//   Purpose:      Signal blocked thread if buffer was empty.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      USBDeviceGlobal         IO buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
void signalEmpty(void)
{
#ifndef _rtems_app    
   // Get IO buffer lock.
   EnterCriticalSection( &ioBuffLock);
#endif
   // Check if buffer is empty
   if (ioBuffer.nFree == ioBuffer.totalPack)
   {
      // Decrease # free positions.
      ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
#ifndef _rtems_app
      // Signal blocked thread in empty condition.
      ReleaseSemaphore( semEmpty, 1, NULL);
#endif
   }
   else
   {
      // Decrease # free positions.
      ioBuffer.nFree = ioBuffer.nFree - ioBuffer.packPerRecp;
   }
#ifndef _rtems_app
   // Release IO buffer lock.
   LeaveCriticalSection( &ioBuffLock);
#endif
}

//*****************************************************************************
//   Name:         setUpReception
//   Purpose:      Update IO buffer control variables.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      usbBuffer         IO buffer structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
void setUpReception(void)
{
   // Update in index.
   ioBuffer.inIndexBytes = ioBuffer.inIndexBytes + ioBuffer.nBytesToReceive;
   if (ioBuffer.inIndexBytes == ioBuffer.lengthBytes)
   {
      // Reset input index.
      ioBuffer.inIndexBytes = 0;
   }
}

//*****************************************************************************
//   Name:         releaseEmpty
//   Purpose:      Signals receiver thread to terminate execution.
//   Arguments:
//      I:         NONE
//      I/0:       NONE
//      O:         NONE
//      Global:      semEmpty      empty semaphore
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//   1.0   -   2009/10/07   -   JJNC   -   Created.
//*****************************************************************************
void releaseEmpty(void)
{
#ifndef _rtems_app
   // Signal blocked thread.
   ReleaseSemaphore( semEmpty, 1, NULL);
#endif
}

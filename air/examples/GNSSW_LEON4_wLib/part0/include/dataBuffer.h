//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  dataBuffer.h
// Version:                  1.4
// Date (YY/MM/DD):            2008 / 02 / 21
// Component:
// Purpose:   This file is the header file for the buffer that stores the samples
//         to be processes by the SIRIA application.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/04/28   -   JJNC   -   R1.1_PFM_SINGLE_FFTW
//   1.2   -   2009/07/14   -   JJNC   -   R1.8_CMP_COMPLEXITY
//   1.3   -   2009/08/13   -   JJNC   -   R1.9_PFM_BIT_FORMAT
//   1.4   -   2009/12/03   -   JJNC   -   R1.17_FUN_INPUT_FORMATS
//*****************************************************************************
#ifndef DATABUFFER_H
#define DATABUFFER_H

#include <Defs_nav.h>
#include <interface.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************
#ifdef COMPILE_RECORD_ONLY
  #define RT_N_PACKAGES      64      // # packages per reception in real time.
  #define RT_N_RECEPTIONS    1024    // IO buffer # receptions in real time.
#else
  #define RT_N_PACKAGES      64      // # packages per reception in real time.
  #define RT_N_RECEPTIONS    32      // IO buffer # receptions in real time.
#endif

#define FILE_N_PACKAGES     1      // # packages per reception in postprocess.
#define FILE_N_RECEPTIONS   2      // IO buffer # receptions in postprocess.

#define TOTAL_PACKAGES   N_PACKAGES*N_RECEPTIONS      // I/O buffer # packages.

// Constants to control the generation and the identification of splitted files
#define CHUNK_STR        ".chunk"
#define PART_STR         ".part"
#define SRX_SAMPLES_STR  "_samples_"
#define SRX_BINARY_STR   ".bin"
#define SRX_TIME_STR_LEN (19)
#define STEREO_SAMPLES_STR      "STEREO_"
#define STEREO_PADDING_LEN      (4)
#define STEREO_TIME_STR_LEN     (15)
#define STEREO_STORING_RATE_MIN (10)
#define RAW_SAMPLE_BYTES_STR      "srx_samples_raw_"
#define REVERSED_SAMPLE_BYTES_STR "srx_samples_"

//*****************************************************************************
//                        STRUCTURES DEFINITION
//*****************************************************************************

// Structure data defines the circular buffer for samples data.
typedef struct
{
   int						inIndexWords;		  // Next position for input data.

   int						nWordsToRead;		  // # words to insert into buffer in each read.
   int						nBytesToRead;		  // # bytes to insert into buffer in each read.
   int						nMsPerRead;			  // # ms read in each read call.
   int						nBytesExtra;		  // # bytes reads by excess in each read call.
   int						accBytesExtra;		  // # extra bytes accumulated.
#ifdef MEMORY_ALLOCATION   
   unsigned int				*samplesWords;        // Samples buffer.
   unsigned int				*samplesI;
   unsigned int				*samplesQ;
#else                                                
   unsigned int				samplesWords[260000];        // Samples buffer.
   unsigned int				samplesI[130000];
  // unsigned int				samplesI[260000];
//   unsigned int				samplesQ[260000];   
#endif                                                      
   
   unsigned int				sampleCounter;
   
//#ifdef MEMORY_ALLOCATION    
//   unsigned char			*byteVectorI;
//   unsigned char			*byteVectorQ;
//#else
//   unsigned char			byteVectorI[5200000];
//   unsigned char			byteVectorQ[5200000];   
//#endif  
   
   unsigned int				byteCounterI;
   unsigned int				byteCounterQ;
   int						wordCounterI;
   int						wordCounterQ;
   unsigned int				byteVectorLength;
   int						cntIfLut;
   int						resetCntTh;
} circularBufferStruct;

// Structure that stores data written by IO thread.
typedef struct ioBuffer_t
{
   int      inIndexBytes;           // Input index.
   int      outIndexBytes;          // Output index.
   int      lengthBytes;            // Length (in bytes).
   long     nFree;                  // # free positions in buffer.
   int      nBytesToReceive;        // Length of data to receive (bytes).
   int      maxRecp;                // Max # receptions to fill buffer.
   int      packPerRecp;            // # packages per reception.
   int      totalPack;              // Total packages in buffer.
   
#ifdef MEMORY_ALLOCATION
   unsigned char *samples;
#else // MEMORY_ALLOCATION
   unsigned char samples[N_BITES_TOTAL_PACK];
#endif // MEMORY_ALLOCATION
   
   FILE     *sampleFileDescriptor;  // Samples record file descriptor.
   FILE     *fdTStamp;				// Time stamp record file descriptor.
#ifndef _rtems_app
    #ifdef LINUX
    // TBD
    #else
    HANDLE     hFile;				// Samples record file handler.
    OVERLAPPED overlapped;			// Overlaped info needed for asynchronous write
    #endif
#endif
} ioBuffer_t;

#ifndef _rtems_app
    #ifdef LINUX
    extern pthread_mutex_t    ioBuffLock;   // I/O buffer lock.
    #else
    extern CRITICAL_SECTION   ioBuffLock;   // I/O buffer lock.
    #endif
#endif


//*********************************************************************************
//                        GLOBAL VARIABLES
//*********************************************************************************
extern ioBuffer_t   ioBuffer;   // IO buffer.

//*****************************************************************************
//                        FUNCTIONS INTERFACE
//*****************************************************************************
int    initializeDataBuffer(void);
void   getAcquisitionDataIQ(struct complex_t *target, int lengthNeeded);
void   getAcquisitionData(struct complex_t *target, int lengthNeeded, T_supportData *supportData_P, int core_index);
void   getAcquisitionDataI(float *target);
void   synchronizeIQdata();
void   UpdateOutIndex(T_supportData *supportData_P);
void   finalizeDataBuffer(void);
#ifndef _rtems_app
    unsigned int WINAPI readData(LPVOID lpParam);

    #ifdef LINUX
    void *writeSampleFile(void *ptr);
    #else
    unsigned int WINAPI writeSampleFile(LPVOID lpParam);
    #endif
#endif
// IO buffer API.
void   setUpReception(void);
void   waitIfFull(void);
void   signalIfFull(void);
void   waitIfEmpty(void);
void   signalEmpty(void);
void   releaseEmpty(void);
void   copyBuffer(void);
void copyBufferAcq(int word);
void   copyBufferWriteSamples(void);

#endif

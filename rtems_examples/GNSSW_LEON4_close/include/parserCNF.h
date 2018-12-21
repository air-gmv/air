//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  parserCNF.h
// Version:                  1.1
// Date (YY/MM/DD):            2008 / 02 / 25
// Component:
// Purpose:   This header file implements the interface for the parser of the
//         configuration file. It parses each line of the configuration file
//         and validates the lines.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/11/03   -   JJNC   -   R.16_FUN_GENERAL_AND_ARM
//*****************************************************************************
#ifndef PARSERCNF_H
#define PARSERCNF_H



//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************
// Errors parsing file.
#define SECTION_NOT_CLOSED      1      // Section already open not closed.
#define FAIL_HEADER_FORMAT      2      // Error in header format.
#define FAIL_END_OR_PARAMETER   3      // Error in header or parameter.
#define PARSER_STATUS           4      // Error in configuration parser status.
#define EXCEEDED_LINE_LENGTH    5      // Line in configuration file too long.
#define SECTION_NAME_NOT_EXIST  6      // Header section name not exists in config.
#define END_SECTION_NOT_MATCH   7      // End section name does not match open section.

#define MAX_LINE_LENGTH         MAX_FILE_NAME      // Maximum line length.



//*****************************************************************************
//                        STRUCTURES DEFINITION
//*****************************************************************************
typedef enum configStatus{ NEXT_HEADER, NEXT_PARAM_END} configStatus;   // Parser status.



//*****************************************************************************
//                        VARIABLES INTERFACE
//*****************************************************************************
extern   int    warningFlag;                        // Warning found during execution.
extern   char   currentParameter[MAX_LINE_LENGTH];  // Current parameter name.
extern   char   currentValue[MAX_LINE_LENGTH];      // Current parameter value.
extern   char   currentSection[MAX_LINE_LENGTH];    // Current section read.
extern   char   currentSectionName[MAX_LINE_LENGTH];// Current section name.
extern   char   currentLine[MAX_LINE_LENGTH];       // Read line buffer.
extern   char   channelSeparatorString[2];          // Values separator (string).
extern   configStatus   parserStatus;               // Parser status.



//*****************************************************************************
//                        FUNCTIONS INTERFACE
//*****************************************************************************
int       parserCNFInit(char *configFile);
void      readLine(void);
void      updateIntStructure(int *array, int isArray, int numberItems, 
                        int min, int max, int def);
void      updateFloatStructure(float *array, int isArray, int numberItems, 
                         float min, float max, float def);
void      updateDoubleStructure(double *array, int isArray, int numberItems, 
                          double min, double max, double def);
void      updateLongStructure(long *array, int isArray, int numberItems, 
                        long min, long max, long def);
int       parserStatusOk(void);
void      updateFailStatus(int failId, char additionalInfo[MAX_LINE_LENGTH]);
int       getFailStatus(void);
void      checkWarning(void);
void      finalizeParserCNF(void);

#endif

#include   <configuration.h>
//#include   <trace.h>

#include   <parserCNF.h>
#include   <siria.h>
#include   <stdlib.h>

//*****************************************************************************
//                        GLOBAL EXTERNAL VARIABLES
//*****************************************************************************
extern T_Config Config;

//*****************************************************************************
//                        GLOBAL INTERNAL VARIABLES
//*****************************************************************************
int    eof=0;                         // End of file flag.
int    fail=0;                        // Critical fail flag.
int    warningFlag=0;                 // Warning found during execution.
char   separatorChar;                 // Parameters separator (char).
char   separatorString[2];            // Parameters separator (string).
char   channelSeparatorChar;          // Channel separator value (char).
FILE   *fdConfig = NULL;              // Configuration file descriptor.
char   currentParameter[MAX_LINE_LENGTH];  // Current parameter name.
char   currentValue[MAX_LINE_LENGTH];      // Current parameter value.
char   currentSection[MAX_LINE_LENGTH];    // Current section read.
char   currentSectionName[MAX_LINE_LENGTH];// Current section name.
char   currentLine[MAX_LINE_LENGTH];       // Read line buffer.
char   channelSeparatorString[2];          // Values separator (string).
configStatus   parserStatus;               // Parser status.


//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
int       openConfigFile(char   *configFile);
int       validLineFormat(int   lineType);
void      getNewLine(void);
int       numberOfSeparators(void);
int       validateIntRange(int newValue,
                           int min,
                           int max,
                           int def);
long      validateLongRange(long newValue,
                            long min,
                            long max,
                            long def);
float     validateFloatRange(float newValue,
                             float min,
                             float max,
                             float def);
double    validateDoubleRange(double newValue,
                              double min,
                              double max,
                              double def);
void      updateFailStatus(int failId,
                           char additionalInfo[MAX_LINE_LENGTH]);


//*****************************************************************************
//                        PUBLIC FUNCTION BODIES
//*****************************************************************************
//*****************************************************************************
//   Name:         parserCNFInit
//   Purpose:      check how many values to be set in current line.
//   Arguments:
//      I:         configFile            configuration file name.
//      I/0:       NONE
//      O:         number of values per channel set in current line.
//      Global:  currentValue         new value/s read from file.
//               eof                  end of file flag.
//               fail                 parser fail flag.
//               separatorChar        parameter value separator (char format).
//               channelSeparatorChar channel separator (char format).
//               separatorString      parameter value separator (string format).
//               channelSeparatorString   channel separator (string format).
//               parserStatus             parser status
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int parserCNFInit(char *configFile)
{
   int ret=EXIT_OK;                  // Function return value.

   // Open configuration file.
   if ((ret = openConfigFile(configFile)) == EXIT_OK)
   {
      // Initialize internal variables.
      eof = FALSE;
      fail = FALSE;
      warningFlag = FALSE;

      // Initialize separators.
      separatorChar = '=';
      channelSeparatorChar = '|';
      strcpy( separatorString, "=");
      strcpy( channelSeparatorString, "|");

      // Set initial parser status.
      parserStatus = NEXT_HEADER;
   }

   return(ret);
}

//*****************************************************************************
//   Name:         openConfigFile
//   Purpose:      Open the configuration file and initializes parser.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE
//      O:         0 if no error occurs.
//                 -1 i.o.c.
//      Global:  fdConfig            configuration file descriptor.
//               msgText             trace text.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int openConfigFile(char *configFile)
{
   int ret=EXIT_OK;                  // Function return value.

   // Open configuration file in reading mode.
   if ((fdConfig = fopen(configFile,"rb")) == NULL)
   {
      // Error opening samples file.
      ret = EXIT_ER;
   }

   return (ret);
}

//*****************************************************************************
//   Name:         numberOfSeparators
//   Purpose:      check how many values to be set in current line.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE
//      O:         number of values per channel set in current line.
//      Global:      currentValue         new value/s read from file.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int numberOfSeparators(void)
{
   int i=0;                  // Loop counter.
   int counter=0;            // Number of "|" character ocurrences.

   // Loop for the whole string.
   while (currentValue[i] != '\0')
   {
      // If charater is '|' then increase counter.
      if (currentValue[i] == channelSeparatorChar)
      {
         counter++;
      }
      i++;
   }

   return (counter);
}

//*****************************************************************************
//   Name:         validLineFormat
//   Purpose:      Check if the current line has a valid format depending on type
//                 of line.
//   Arguments:
//      I:       lineType      Type of line: head, end or parameter.
//      I/0:     NONE
//      O:       TRUE if valid line format.
//               FALSE i.o.c.
//      Global:      currentLine         new line read from file.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int validLineFormat(int lineType)
{
   int ret=FALSE;                  // Return function value.
   int lineLength=0;               // Current line length.

   // Check type of line.
   switch (lineType)
   {
      // Header section.
      case 0:
      {
         // Check header format.
         if ((currentLine[0] == '<') &&
            (currentLine[1] != '/') &&
            (currentLine[4] == '>'))
         {
            ret = TRUE;
         }
         // Fail in format.
         else
         {
            ret = FALSE;
            updateFailStatus( FAIL_HEADER_FORMAT, currentLine);
         }
         break;
      }
      // End section.
      case 1:
      {
         // Check end format.
         if ((currentLine[0] == '<') &&
            (currentLine[1] == '/') &&
            (currentLine[5] == '>'))
         {
            ret = TRUE;
         }
         break;
      }
      // Parameter.
      default:
      {
         // Check paramater format.
         lineLength = strlen(currentLine);
         if ((strstr( currentLine, separatorString) != NULL) &&
            (currentLine[0] != separatorChar) &&
            (currentLine[lineLength-2] != separatorChar))
         {
            ret = TRUE;
         }
         // Fail in format.
         else
         {
            ret = FALSE;
            updateFailStatus( FAIL_END_OR_PARAMETER, "");
         }
      }
   }

   return (ret);
}

//*****************************************************************************
//   Name:         getNewLine
//   Purpose:      Get a new line and checks that eof was not reached and
//                 a not empty and not too long line was read.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:  eof               end of file reached.
//               currentLine       last line read from file.
//               parserStatus      status of the parser.
//               currentParameter   parameter to update.
//   Files I/O:      fdConfig       configuration file.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
void getNewLine(void)
{
   int lineLength = 0;            // Line read length.

   // Loop to skip comments lines.
   do
   {
      // Read new line from configuration file.
      memset(currentLine, 0 , sizeof(char)*MAX_LINE_LENGTH);
      if ((fgets(currentLine, MAX_LINE_LENGTH, fdConfig)) == NULL)
      {
         // EOF found. Check if last was end section.
         if (parserStatus == NEXT_HEADER)
         {
            // Set end of file flag.
            eof = 1;

            // Clean variable.
            strcpy( currentParameter, "");
         }
         // Last parameter or header section -> FAIL.
         else
         {
            // Clean variable.
            strcpy( currentParameter, "");

            // Update fail status.
            updateFailStatus( SECTION_NOT_CLOSED, "");
         }
      }
      // Parse line read.
      else
      {
         // Checks line length.
         lineLength = strlen(currentLine);
         if ((lineLength <= 0) || (lineLength > MAX_LINE_LENGTH))
         {
            // Clean variable.
            strcpy( currentParameter, "");

            // Update fail status.
            updateFailStatus( EXCEEDED_LINE_LENGTH, "");
         }
      }
   } while (currentLine[0] == '%');

   return;
}

//*****************************************************************************
//   Name:         readLine
//   Purpose:      Read new line from configuration file.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:  currentLine        last line read from file.
//               parserStatus       status of the parser.
//               currentParameter   parameter to update.
//               currentSection     section to update.
//               currentValue       value/s to set parameter.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
void readLine(void)
{
   unsigned int   i=0;                  // Loop index.
   int            j=0;                  // Loop index.
   char           *stringAux;           // Auxiliar string variable.

   // Check fail in last read or eof reached.
   if (parserStatusOk())
   {
      // Gets a new line from configuration file.
      getNewLine();

      // Checks not errors during read.
      if (parserStatusOk())
      {
         // Parse depending on status of the parser.
         switch (parserStatus)
         {
            // Parser out of section. Header section expected.
            case NEXT_HEADER:
            {
               strcpy( currentParameter, "");
               // Line start with header section characters.
               if (validLineFormat(0))
               {
                  // Gets section name from read line.
                  j = 0;
                  for (i=1; i<4 ;i++)
                  {
                     currentSection[j] = currentLine[i];
                     j++;
                  }
                  currentSection[j] = '\0';
               }
               break;
            }
            // Parser out of section. End section or configuration expected.
            case NEXT_PARAM_END:
            {
               // Line start with end section characters.
               if (validLineFormat(1))
               {
                  strcpy( currentParameter, "");
                  // Gets section name from read line.
                  j = 0;
                  for (i=2; i<5 ;i++)
                  {
                     currentSection[j] = currentLine[i];
                     j++;
                  }
                  currentSection[j] = '\0';
                     // Update parser status.
                  parserStatus = NEXT_HEADER;
               }
               // Configuration parameter line.
               else if (validLineFormat(2))
               {
                  // Gets parameter value.
                  stringAux = strstr( currentLine, separatorString);
                  memset( currentValue, 0, sizeof(char)*MAX_LINE_LENGTH);
                  strncpy( currentValue, stringAux, strlen(stringAux)-1);
                  i=0;
#ifndef ARM_LINUX
                  while (currentValue[i+1] != 13 && currentValue[i+1] != 0)
#else
                  while (i < (strlen(currentValue)-1))
#endif
                  {
                     currentValue[i] = currentValue[i+1];
                     i++;
                  }
                  currentValue[i] = '\0';

                  // Gets parameter name.
                  i=0;
                  while (currentLine[i] != separatorChar)
                  {
                     currentParameter[i] = currentLine[i];
                     i++;
                  }
                  currentParameter[i] = '\0';
               }
               break;
            }
            // Wrong configuration parser status.
            default:
            {
               strcpy( currentParameter, "");
               updateFailStatus( PARSER_STATUS, "");
            }
         }
      }
   }

   return;
}

//*****************************************************************************
//   Name:         validateIntRange
//   Purpose:      Check if new int value is between limits.
//   Arguments:
//      I:       newValue             new value to set.
//               min                  minum value allowed.
//               max                  maximum value allowed.
//               def                  default value to set.ly.
//      I/0:     NONE.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/11   -   JJNC   -   Created.
//*****************************************************************************
int validateIntRange(int newValue, int min, int max, int def)
{
   int ret=0;                  // Value to set.

   // Set default value.
   ret = def;

   // Check new value.
   if ((newValue >= min) && (newValue <= max))
   {
      ret = newValue;
   }
   // Print a warning message because value does not belong to parameter range.
   else
   {
      warningFlag = TRUE;
   }

   return (ret);
}

//*****************************************************************************
//   Name:         validateLongRange
//   Purpose:      Check if new long value is between limits.
//   Arguments:
//      I:       newValue             new value to set.
//               min                  minum value allowed.
//               max                  maximum value allowed.
//               def                  default value to set.ly.
//      I/0:     NONE.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/11   -   JJNC   -   Created.
//*****************************************************************************
long validateLongRange(long newValue, long min, long max, long def)
{
   long ret=0;                  // Value to set.

   // Set default value.
   ret = def;

   // Check new value.
   if ((newValue >= min) && (newValue <= max))
   {
      ret = newValue;
   }
   // Print a warning message because value does not belong to parameter range.
   else
   {
      warningFlag = TRUE;
   }

   return (ret);
}

//*****************************************************************************
//   Name:         validateFloatRange
//   Purpose:      Check if new float value is between limits.
//   Arguments:
//      I:       newValue             new value to set.
//               min                  minum value allowed.
//               max                  maximum value allowed.
//               def                  default value to set.ly.
//      I/0:     NONE.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/11   -   JJNC   -   Created.
//*****************************************************************************
float validateFloatRange(float newValue, float min, float max, float def)
{
   float ret=0;                  // Value to set.

   // Update return value.
   ret = def;

   // Check if new value is between limits.
   if ((newValue >= min) && (newValue <= max))
   {
      ret = newValue;
   }
   // Print a warning message because value does not belong to parameter range.
   else
   {
      warningFlag = TRUE;
   }

   return (ret);
}

//*****************************************************************************
//   Name:         validateDoubleRange
//   Purpose:      Check if new double value is between limits.
//   Arguments:
//      I:       newValue             new value to set.
//               min                  minum value allowed.
//               max                  maximum value allowed.
//               def                  default value to set.ly.
//      I/0:     NONE.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/11   -   JJNC   -   Created.
//*****************************************************************************
double validateDoubleRange(double newValue, double min, double max, double def)
{
   double ret=0;                  // Value to set.

   // Update return value.
   ret = def;

   // Check if new value is between limits.
   if ((newValue >= min) && (newValue <= max))
   {
      ret = newValue;
   }
   // Print a warning message because value does not belong to parameter range.
   else
   {
      warningFlag = TRUE;
   }

   return (ret);
}

//*****************************************************************************
//   Name:         updateIntStructure
//   Purpose:      Parse values from last read line and updates integer structure.
//   Arguments:
//      I:       isArray           flag for arrays field.
//               numberItems       structure max length.
//               min               minimum value allowed.
//               max               maximum value allowed.
//               def               default value to apply.
//      I/0:     array             structure to update.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/18   -   JJNC   -   Created.
//*****************************************************************************
void updateIntStructure( int *array, int isArray, int numberItems, int min, int max, int def)
{
   int      i=0;                     // Index.
   int      intNewValue=0;           // New integer value.
   int      nSeparators=0;           // Number of value separators.
   char     *stringValue;            // Value per channel in string format.

   // Structure to update is an array.
   if (isArray)
   {
      // Allocate memory.
      nSeparators = numberOfSeparators();

      // Apply same value to all elements of structure.
      if (nSeparators == 0)
      {
         intNewValue = atoi(currentValue);
         for (i=0; i<numberItems ;i++)
         {
            array[i] = validateIntRange(intNewValue, min, max, def);
         }
      }
      // Apply values read from file.
      else if (nSeparators == (numberItems-1))
      {
         // Get the value for the i-channel in string format.
         stringValue = strtok( currentValue, channelSeparatorString);

         // Get the integer value.
         intNewValue = atoi(stringValue);

         // Apply first value.
         array[0] = validateIntRange(intNewValue, min, max, def);

         // Get the values for each channel.
         for (i=1; i<numberItems ;i++)
         {
            // Get the value for the i-channel in string format.
            stringValue = strtok( NULL, channelSeparatorString);

            // Get the integer value.
            intNewValue = atoi(stringValue);

            // Apply new value for i-element.
            array[i] = validateIntRange(intNewValue, min, max, def);
         }
      }
      // Fail in line read.
      else if (numberItems != 0)
      {
         updateFailStatus( FAIL_END_OR_PARAMETER, "");
      }
   }
   // Structure to update is a number.
   else
   {
      // Get integer format of new value.
      intNewValue = atoi(currentValue);
      array[0] = validateIntRange(intNewValue, min, max, def);
   }

   return;
}

//*****************************************************************************
//   Name:         updateFloatStructure
//   Purpose:      Parse values from last read line and updates float structure.
//   Arguments:
//      I:       isArray           flag for arrays field.
//               numberItems       structure max length.
//               min               minimum value allowed.
//               max               maximum value allowed.
//               def               default value to apply.
//      I/0:     array             structure to update.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/18   -   JJNC   -   Created.
//*****************************************************************************
void updateFloatStructure( float *array, int isArray, int numberItems, float min, float max, float def)
{
   int     i=0;                    // Index.
   float   floatNewValue=0;        // New float value.
   int     nSeparators=0;          // Number of value separators.
   char    *stringValue;           // Value per channel in string format.

   // Structure to update is an array.
   if (isArray)
   {
      // Allocate memory.
      nSeparators = numberOfSeparators();

      // Apply same value to all elements of structure.
      if (nSeparators == 0)
      {
         floatNewValue = (float) atof(currentValue);
         for (i=0; i<numberItems ;i++)
         {
            array[i] = validateFloatRange(floatNewValue, min, max, def);
         }
      }
      // Apply values read from file.
      else if (nSeparators == (numberItems-1))
      {
         // Get the value for the i-channel in string format.
         stringValue = strtok( currentValue, channelSeparatorString);

         // Get the integer value.
         floatNewValue = (float) atof(stringValue);

         // Apply first value.
         array[0] = validateFloatRange(floatNewValue, min, max, def);

         // Get the values for each channel.
         for (i=1; i<numberItems ;i++)
         {
            // Get the value for the i-channel in string format.
            stringValue = strtok( NULL, channelSeparatorString);

            // Get the integer value.
            floatNewValue = (float) atof(stringValue);

            // Apply new value for i-element.
            array[i] = validateFloatRange(floatNewValue, min, max, def);
         }
      }
      // Fail in line read.
      else
      {
         updateFailStatus( FAIL_END_OR_PARAMETER, "");
      }
   }
   // Structure to update is a number.
   else
   {
      // Get integer format of new value.
      floatNewValue = (float) atof(currentValue);
      array[0] = validateFloatRange(floatNewValue, min, max, def);
   }

   return;
}

//*****************************************************************************
//   Name:         updateDoubleStructure
//   Purpose:      Parse values from last read line and updates double structure.
//   Arguments:
//      I:       isArray           flag for arrays field.
//               numberItems       structure max length.
//               min               minimum value allowed.
//               max               maximum value allowed.
//               def               default value to apply.
//      I/0:     array             structure to update.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/18   -   JJNC   -   Created.
//*****************************************************************************
void updateDoubleStructure( double *array, int isArray, int numberItems, double min, double max, double def)
{
   int      i=0;                    // Index.
   double   doubleNewValue=0;       // New double value.
   int      nSeparators=0;          // Number of value separators.
   char     *stringValue;           // Value per channel in string format.

   // Structure to update is an array.
   if (isArray)
   {
      // Allocate memory.
      nSeparators = numberOfSeparators();

      // Apply same value to all elements of structure.
      if (nSeparators == 0)
      {
         doubleNewValue = atof(currentValue);
         for (i=0; i<numberItems ;i++)
         {
            array[i] = validateDoubleRange(doubleNewValue, min, max, def);
         }
      }
      // Apply values read from file.
      else if (nSeparators == (numberItems-1))
      {
         // Get the value for the i-channel in string format.
         stringValue = strtok( currentValue, channelSeparatorString);

         // Get the integer value.
         doubleNewValue = atof(stringValue);

         // Apply first value.
         array[0] = validateDoubleRange(doubleNewValue, min, max, def);

         // Get the values for each channel.
         for (i=1; i<numberItems ;i++)
         {
            // Get the value for the i-channel in string format.
            stringValue = strtok( NULL, channelSeparatorString);

            // Get the integer value.
            doubleNewValue = atof(stringValue);

            // Apply new value for i-element.
            array[i] = validateDoubleRange(doubleNewValue, min, max, def);
         }
      }
      // Fail in line read.
      else
      {
         updateFailStatus( FAIL_END_OR_PARAMETER, "");
      }
   }
   // Structure to update is a number.
   else
   {
      // Get integer format of new value.
      doubleNewValue = atof(currentValue);
      array[0] = validateDoubleRange(doubleNewValue, min, max, def);
   }

   return;
}


//*****************************************************************************
//   Name:         updateLongStructure
//   Purpose:      Parse values from last read line and updates long structure.
//   Arguments:
//      I:       isArray           flag for arrays field.
//               numberItems       structure max length.
//               min               minimum value allowed.
//               max               maximum value allowed.
//               def               default value to apply.
//      I/0:     array             structure to update.
//      O:       NONE.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/02/18   -   JJNC   -   Created.
//*****************************************************************************
void updateLongStructure( long *array, int isArray, int numberItems, long min, long max, long def)
{
   int    i=0;                     // Index.
   long   longNewValue=0;          // New long value.
   int    nSeparators=0;           // Number of value separators.
   char   *stringValue;            // Value per channel in string format.

   // Structure to update is an array.
   if (isArray)
   {
      // Allocate memory.
      nSeparators = numberOfSeparators();

      // Apply same value to all elements of structure.
      if (nSeparators == 0)
      {
         longNewValue = atol(currentValue);
         for (i=0; i<numberItems ;i++)
         {
            array[i] = validateLongRange(longNewValue, min, max, def);
         }
      }
      // Apply values read from file.
      else if (nSeparators == (numberItems-1))
      {
         // Get the value for the i-channel in string format.
         stringValue = strtok( currentValue, channelSeparatorString);

         // Get the integer value.
         longNewValue = atol(stringValue);

         // Apply first value.
         array[0] = validateLongRange(longNewValue, min, max, def);

         // Get the values for each channel.
         for (i=1; i<numberItems ;i++)
         {
            // Get the value for the i-channel in string format.
            stringValue = strtok( NULL, channelSeparatorString);

            // Get the integer value.
            longNewValue = atol(stringValue);

            // Apply new value for i-element.
            array[i] = validateLongRange(longNewValue, min, max, def);
         }
      }
      // Fail in line read.
      else
      {
         updateFailStatus( FAIL_END_OR_PARAMETER, "");
      }
   }
   // Structure to update is a number.
   else
   {
      // Get integer format of new value.
      longNewValue = atol(currentValue);
      array[0] = validateLongRange(longNewValue, min, max, def);
   }

   return;
}

//*****************************************************************************
//   Name:         updateFailStatus
//   Purpose:      Set fail variable status and prints error message description.
//   Arguments:
//      I:       failId            fail id.
//               additionalInfo    additional info to print.
//      I/0:      NONE.
//      O:        NONE.
//      Global:  fail              fail while parsing file.
//               Config            configuration structure.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
void updateFailStatus(int failId, char additionalInfo[MAX_LINE_LENGTH])
{
   // Update fail value.
   fail = failId;

   // Prints fail message description.
   switch (fail)
   {
      // A section was open but not closed when reached end of file.
      case SECTION_NOT_CLOSED:
      {
         break;
      }
      case FAIL_HEADER_FORMAT:
      {
         break;
      }
      // Error in end of section or parameter line.
      case FAIL_END_OR_PARAMETER:
      {
         break;
      }
      // Internal error. Parser status in invalid status.
      case PARSER_STATUS:
      {
         break;
      }
      // Line read exceed maximum line length.
      case EXCEEDED_LINE_LENGTH:
      {
         break;
      }
      // Header section contains a name that is not a section.
      case SECTION_NAME_NOT_EXIST:
      {
         break;
      }
      // End section name does not match open section.
      case END_SECTION_NOT_MATCH:
      {
         break;
      }
      default:
      {
         fail = -1;
      }
   }

   // Reser the default values.
   setDefaultValues();

   return;
}

//*****************************************************************************
//   Name:         parserStatusOk
//   Purpose:      Checks if eof or a parser failure.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         TRUE   if succes
//                 FALSE  i.o.c.
//      Global:  fail            fail while parsing file.
//               eof             end of configuration file.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
int parserStatusOk(void)
{
   // Checks if eof reached or parser failure.
   return ((!fail) && (!eof));
}

//*****************************************************************************
//   Name:         checkWarning
//   Purpose:      Checks if a warning was written to trace file to print a
//                 message in screen.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE
//      Global:      warningFlag            warning flag.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2009/03/03   -   JJNC   -   Created.
//*****************************************************************************
void checkWarning(void)
{
   // Check if a warning was found while parsing configuration file.
   if (warningFlag == TRUE)
   {
      // Print warning message in console.
      printf("\n");
      printf("Warning during ROMA initialization.........check trace file.\n");
      printf("\n");
   }

   return;
}

//*****************************************************************************
//   Name:         finalizeParserCNF
//   Purpose:      Free configuration parser resources.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:    NONE.
//   Files I/O:      fdConfig         configuration file.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/01/11   -   JJNC   -   Created.
//*****************************************************************************
void finalizeParserCNF(void)
{
   // Close configuration file if open.
   if (fdConfig != NULL)
   {
      // Close configuration file.
      fclose(fdConfig);

      // Set reference to NULL.
      fdConfig = NULL;
   }

   return;
}

//*****************************************************************************
//   Name:         getFailStatus
//   Purpose:      Check if an errro ocurred while parsing configuration file.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:      fail         fail while parsing file.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/08/04   -   JJNC   -   Created.
//*****************************************************************************
int getFailStatus(void)
{
   // Return parser internal status.
   return (fail);
}

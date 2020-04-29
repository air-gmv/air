/*------------------------------------------------------------------------+
 * +xxxxxxxxxxxxxxxxxxxxxx SKYSOFT PORTUGAL SA xxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | LEGAL NOTICE: The source code contained herein is not intended to  | *
 * | allow the development of source code or software for use or        | *
 * | distribution. This file contains SKYSOFT PORTUGAL, SA proprietary  | *
 * | information and is supplied on the condition that it shall solely  | *
 * | be used within the terms of the AVT License Agreement. Except as   | *
 * | otherwise provided thereof, you may not,without prior written      | *
 * | permission from SKYSOFT PORTUGAL, S.A. provide, or otherwise       | *
 * | disclose information regarding any discovered bugs, errors,        | *
 * | architecture issues or problems with the Source Code or Compiled   | *
 * | Code to any other party.                                           | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *                                                                        *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | FILE: MasterTest.c                                                 | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: MASTERTEST                                              | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterTestMain                              | *
 * |                        ExecuteTest                                 | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    19/09/2002                                                | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
*------------------------------------------------------------------------*/

#include "MasterTest.h"

extern TEST_RESULT_TYPE TabResults[LAST_TEST];
extern int NrTestId;
extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: PrepareTestEnvironmentOfMasterPartition             | *
 * |                                                                    | *
 * | DESCRIPTION: Puts all general purpose ARINC 653 objects declared   | *
 * |              on initialization state and ready to be used          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   13/01/2005                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void PrepareTestEnvironmentOfMasterPartition ( )
{


/*Does nothing - used to initialize semaphores and buffers*/

  CleanQueuingPort ( "QP_RESULT_RECEIVE" );
  CleanQueuingPort ( "QP_MASTER_RECEIVE" );

  return;

} /* PrepareTestEnvironmentOfMasterPartition */ 


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ExecuteTest                                         | *
 * |                                                                    | *
 * | DESCRIPTION: Receive the test id, and call de respective function. | *
 * |              Return the result of the test. Ok or FAIL.            | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | int TestId                      X            Test identification   | *
 * |                                                                    | *
 * | RETURN VALUE: int TestResult                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void ExecuteTest( void )
{
  printf("ExecuteTest\n");

	
		
#ifdef  EXECUTE_T_API_INTER_560_0011                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_560_0011
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_560_0011;
      MasterProcedure_T_API_INTER_560_0011 (&(TabResults[T_API_INTER_560_0011]) );   
#endif
		
#ifdef  EXECUTE_T_API_INTER_560_0012                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_560_0012
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_560_0012;
      MasterProcedure_T_API_INTER_560_0012 (&(TabResults[T_API_INTER_560_0012]) );   
#endif
		
#ifdef  EXECUTE_T_API_INTER_570_0013                                 
      printf("EXECUTE_T_API_INTER_570_0013 defined\n");
  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_570_0013
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_570_0013;
      MasterProcedure_T_API_INTER_570_0013 (&(TabResults[T_API_INTER_570_0013]) );   
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0010  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0010
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0010;
      MasterProcedure_T_API_INTER_580_0010( &(TabResults[T_API_INTER_580_0010]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0020  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0020
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0020;
      MasterProcedure_T_API_INTER_580_0020( &(TabResults[T_API_INTER_580_0020]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0025                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0025
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0025;
      MasterProcedure_T_API_INTER_580_0025( &(TabResults[T_API_INTER_580_0025]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0028                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0028
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0028;
      MasterProcedure_T_API_INTER_580_0028 ( &(TabResults[T_API_INTER_580_0028]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0030

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0030
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0030;
      MasterProcedure_T_API_INTER_580_0030( &(TabResults[T_API_INTER_580_0030]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0032                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0032
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0032;
      MasterProcedure_T_API_INTER_580_0032( &(TabResults[T_API_INTER_580_0032]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0033                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0033
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0033;
      MasterProcedure_T_API_INTER_580_0033( &(TabResults[T_API_INTER_580_0033]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0035  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0035
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0035;
      MasterProcedure_T_API_INTER_580_0035( &(TabResults[T_API_INTER_580_0035]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0038                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0038
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0038;
      MasterProcedure_T_API_INTER_580_0038( &(TabResults[T_API_INTER_580_0038]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0039                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0039
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0039;
      MasterProcedure_T_API_INTER_580_0039( &(TabResults[T_API_INTER_580_0039]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0040  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0040
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0040;
      MasterProcedure_T_API_INTER_580_0040(&(TabResults[T_API_INTER_580_0040]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0042

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0042
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0042;
      MasterProcedure_T_API_INTER_580_0042(&(TabResults[T_API_INTER_580_0042]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0045                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0045
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0045;
      MasterProcedure_T_API_INTER_580_0045( &(TabResults[T_API_INTER_580_0045]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0050  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0050
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0050;
      MasterProcedure_T_API_INTER_580_0050( &(TabResults[T_API_INTER_580_0050]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0052                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0052
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0052;
      MasterProcedure_T_API_INTER_580_0052( &(TabResults[T_API_INTER_580_0052]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0055                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0055
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0055;
      MasterProcedure_T_API_INTER_580_0055( &(TabResults[T_API_INTER_580_0055]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0060  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0060
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0060;
      MasterProcedure_T_API_INTER_580_0060( &(TabResults[T_API_INTER_580_0060]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0062                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0062
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0062;
      MasterProcedure_T_API_INTER_580_0062( &(TabResults[T_API_INTER_580_0062]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0065                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0065
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0065;
      MasterProcedure_T_API_INTER_580_0065( &(TabResults[T_API_INTER_580_0065]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0070  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0070
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0070;
      MasterProcedure_T_API_INTER_580_0070( &(TabResults[T_API_INTER_580_0070]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0080  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0080
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0080;
      MasterProcedure_T_API_INTER_580_0080( &(TabResults[T_API_INTER_580_0080]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_580_0090  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_580_0090
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_580_0090;
      MasterProcedure_T_API_INTER_580_0090( &(TabResults[T_API_INTER_580_0090]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0010  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0010
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0010;
      MasterProcedure_T_API_INTER_590_0010( &(TabResults[T_API_INTER_590_0010]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0020  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0020
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0020;
      MasterProcedure_T_API_INTER_590_0020( &(TabResults[T_API_INTER_590_0020]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0030  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0030
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0030;
      MasterProcedure_T_API_INTER_590_0030( &(TabResults[T_API_INTER_590_0030]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0040  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0040
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0040;
      MasterProcedure_T_API_INTER_590_0040( &(TabResults[T_API_INTER_590_0040]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0045  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0045
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0045;
      MasterProcedure_T_API_INTER_590_0045( &(TabResults[T_API_INTER_590_0045]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0050  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0050
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0050;
      MasterProcedure_T_API_INTER_590_0050( &(TabResults[T_API_INTER_590_0050]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0060  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0060
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0060;
      MasterProcedure_T_API_INTER_590_0060( &(TabResults[T_API_INTER_590_0060]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0066                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0066
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0066;
      MasterProcedure_T_API_INTER_590_0066( &(TabResults[T_API_INTER_590_0066]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0070  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0070
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0070;
      MasterProcedure_T_API_INTER_590_0070( &(TabResults[T_API_INTER_590_0070]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0080  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0080
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0080;
      MasterProcedure_T_API_INTER_590_0080( &(TabResults[T_API_INTER_590_0080]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_590_0090  

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_590_0090
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_590_0090;
      MasterProcedure_T_API_INTER_590_0090( &(TabResults[T_API_INTER_590_0090]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_600_0011                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_600_0011
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_600_0011;
      MasterProcedure_T_API_INTER_600_0011 (&(TabResults[T_API_INTER_600_0011]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_600_0012                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_600_0012
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_600_0012;
      MasterProcedure_T_API_INTER_600_0012 (&(TabResults[T_API_INTER_600_0012]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_610_0011                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_610_0011
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_610_0011;
      MasterProcedure_T_API_INTER_610_0011 (&(TabResults[T_API_INTER_610_0011]) );
#endif
		
#ifdef  EXECUTE_T_API_INTER_610_0012                                 

  TraceActive = AVT_NO_ACTIVE;
  PrepareTestEnvironmentOfMasterPartition ( );
  
  #ifdef  TRACE_T_API_INTER_610_0012
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif
	  
      NrTestId = T_API_INTER_610_0012;
      MasterProcedure_T_API_INTER_610_0012 (&(TabResults[T_API_INTER_610_0012]) );
#endif
		
	
		
  TraceActive = AVT_NO_ACTIVE;

printf("end ExecuteTest\n");
} /* ExecuteTest */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME  SaveTestResults                                     | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   01/02/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
 
void SaveTestResults ()
{
/*	int fd;
	unsigned int SizeToWrite = 0;
		
	if ( ( fd = creat ("AVT_results", PERMS ) ) == -1 )	{
      fprintf( stdout,"Unable to create file for result data\n");
      printf("Unable to create file for result data\n");
		return;
	}
	
	SizeToWrite = sizeof (TEST_RESULT_TYPE)*LAST_TEST;
   if ( write ( fd, (char *) ( TabResults ), SizeToWrite ) !=  SizeToWrite )
	{
      //fprintf( stdout,"Error writing file of result data\n");
      printf("Error writing file of result data\n");
	}

	if ( close ( fd ) != 0)
	{
	   printf ("Failed closing AVT_results file\n");
	}*/

} /* SaveTestResults */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME  SynchronizeWithAuxPartition                                      | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   23/07/2007                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void SynchronizeWithAuxPartition ()
{
	APEX_BYTE    Message[QUEUING_PORT_MAX_STARTUPINFO_SIZE];

	printf("SynchronizeWithAuxPartition\n");

	ReceiveQueuingMessage ( "QP_AUXPARTITONSTARTUP_RECEIVE", 
							INFINITE_TIME_VALUE, Message );


    CleanQueuingPort ( "QP_AUXPARTITONSTARTUP_RECEIVE" );
}


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME  MasterTestMain                                      | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void MasterTestMain( void )
{
  int rc = -1;
  printf("MasterTestMain\n");

  SynchronizeWithAuxPartition ();
  ExecuteTest ();
  //SaveTestResults ();
  /* makes aux partition to STOP */
  SendSubtestIdToAuxPartition( LAST_TEST );

  
  print_test_results(&TabResults);
  printf("TESTS DONE\n");
  
  pmk_system_halt();

  //STOP_SELF ();
  
} /* MasterTestMain */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: MasterTest.c                       | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */


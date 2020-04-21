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
 * | FILE: AVT_LoadTestName.c                                           | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTLOADTESTNAME.C                                       | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): LoadTestsName                               | *
 * |                        GetTestName                                 | *
 * |                                                                    | *
 * | DESCRIPTION: LOAD NAME OF TEST IN A ARRAY AND RETURN A POINTER WITH| *
 * |              THE ADREES OF THE ARRAY                               | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    30/11/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED:                                                       | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "AVT_LoadTables.h"
#include <string.h>
char TabTestNames[LAST_TEST][TESTNAMESIZE];

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  LoadTestsName                                      | *
 * |                                                                    | *
 * | DESCRIPTION: As described above                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   10/11/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                    Input  Output  Description          | *
 * |                                                                    | *
 * | RETURN VALUE: char* with de adrees of the array of names.          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void LoadTestsName( void )
{

  /* clean array */
  memset( (void *) TabTestNames,'\0', sizeof( TabTestNames ));

  
    
  strcpy( TabTestNames[T_API_INTER_520_0011],  "T_API_INTER_520_0011 " );
  strcpy( TabTestNames[T_API_INTER_520_0012],  "T_API_INTER_520_0012 " );
  strcpy( TabTestNames[T_API_INTER_520_0013],  "T_API_INTER_520_0013 " );
  strcpy( TabTestNames[T_API_INTER_530_0011],  "T_API_INTER_530_0011 " );
  strcpy( TabTestNames[T_API_INTER_530_0012],  "T_API_INTER_530_0012 " );
  strcpy( TabTestNames[T_API_INTER_540_0011],  "T_API_INTER_540_0011 " );
  strcpy( TabTestNames[T_API_INTER_540_0013],  "T_API_INTER_540_0013 " );  
  strcpy( TabTestNames[T_API_INTER_540_0014],  "T_API_INTER_540_0014 " );  
  strcpy( TabTestNames[T_API_INTER_550_0011],  "T_API_INTER_550_0011 " );
  strcpy( TabTestNames[T_API_INTER_550_0012],  "T_API_INTER_550_0012 " );
  
} /* LoadTables */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetTestName                                         | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
char *GetTestName( unsigned int NrTest  )
{
  static char  TestName[TESTNAMESIZE];

  memset ( (void *) TestName, '\0', sizeof (TestName)); 
  strcpy ( TestName, TabTestNames[NrTest] );
  return ( (char *) TestName );

} /* GetTestName */


void print_test_results(TEST_RESULT_TYPE *TestResult){


  rtems_task_wake_after(40);

  int i = 0;

  for(i = 0; i < LAST_TEST; i++){

    printf("%s : ", TabTestNames[i]);
    if(TestResult[i] == FAILED){
      printf("FAILED\n");
    }else if(TestResult[i] == NOT_PERFORMED){
      printf("NOT_PERFORMED\n");
    }else printf("SUCCESS\n");
  }

}

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: AVT_LoadTestName.c                 | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

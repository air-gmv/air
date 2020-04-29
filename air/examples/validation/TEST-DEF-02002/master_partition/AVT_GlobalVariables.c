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
 * | FILE: AVT_GlobalVariables.c                                        | *
 * | VERSION:                                                           | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME:                                                         | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): InitGlobalVariables                    		| *
 * |                        ResetVariable											| *
 * |                        SetVariable											| *
 * |                        GetVariable											| *
 * |                                                                    | *
 * | DESCRIPTION: This AVTTool module ensures platform independent      | *
 * /              communication of test results between different       / *
 * /              AVTTool processes running on the same partition.      / *
 * /              It provides three boolean variables (Global Variables)/ *
 * /              and the methods required to initialize them and set,  / *
 * /              reset or get its internal values.                     / *
 * /                                                                    / *
 * |                                                                    | *
 * | AUTHOR:      ssantos                                               | *
 * | DATE:        2006/11/17                                            | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_GlobalVariables.h"


static BOOLEAN GV1;
static BOOLEAN GV2;
static BOOLEAN GV3;


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: InitGlobalVariables                                 | *
 * |                                                                    | *
 * | DESCRIPTION:   Initializes the declared Global Variables  	      | *
 * /                GV1, GV2 and GV3                                    / *
 * |                                                                    | *
 * | AUTHOR: 		  ssantos                                             | *
 * | DATE:          2006/11/17                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char* err_mesg                  	     x     error message         | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:  An ARINC653 RETURN_CODE_TYPE which is either        | *
 * /                NO_ERROR or INVALID_PARAM                           / *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE InitGlobalVariables(char *err_msg)
{
	memset(err_msg, 0, INIT_ERRORMSG_SIZE);
	
	GV1 = FALSE;
	GV2 = FALSE;
	GV3 = FALSE;
		
	/* (no error condition for this platform) */
	return NO_ERROR;
	
}/* InitGlobalVariables */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: SetVariable                                         | *
 * |                                                                    | *
 * | DESCRIPTION:   Set the internal value of the variable  	         | *
 * /                identified by "var" to TRUE                         / *
 * |                                                                    | *
 * | AUTHOR: 		  ssantos                                             | *
 * | DATE:          2006/11/17                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char*   var                     x	   	     Variable name         | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:  An ARINC653 RETURN_CODE_TYPE which is either        | *
 * /                NO_ERROR or INVALID_PARAM                           / *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE SetVariable (char* var)
{
	RETURN_CODE_TYPE ret_code = NO_ERROR;
	
	/* Put GV1 value to TRUE */
	if (strcmp (var, "GV1") == 0)
	{
		GV1 = TRUE;
	}
	
   /* Put GV2 value to TRUE */	
	else if (strcmp(var, "GV2") == 0)
	{
		GV2 = TRUE;
	}

   /* Put GV3 value to TRUE */		
	else if (strcmp(var, "GV3") == 0)
	{
		GV3 = TRUE;
	}
	
	/* error condition: no var with the specified name*/
	else
	{
		ret_code = INVALID_PARAM;
	}
	
	return ret_code;
	
}/* SetVariable */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ResetVariable                                       | *
 * |                                                                    | *
 * | DESCRIPTION:   Set the internal value of the variable  	         | *
 * /                identified by "var" to FALSE                        / *
 * |                                                                    | *
 * | AUTHOR: 		  ssantos                                             | *
 * | DATE:          2006/11/17                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char*   var                     x	   	     Variable name         | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:  An ARINC653 RETURN_CODE_TYPE which is either        | *
 * /                NO_ERROR or INVALID_PARAM                           / *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE ResetVariable (char* var)
{
	RETURN_CODE_TYPE ret_code = NO_ERROR;

	/* put GV1 value to FALSE */		
	if (strcmp(var, "GV1") == 0)
	{
		GV1 = FALSE;
	}
	
	/* put GV2 value to FALSE */	
	else if (strcmp(var, "GV2") == 0)
	{
		GV2 = FALSE;
	}
	
	/* put GV3 value to FALSE */
	else if (strcmp(var, "GV3") == 0)
	{
		GV3 = FALSE;
	}

	/* error condition: no var with the specified name*/	
	else
	{
		ret_code = INVALID_PARAM;
	}
	
	return ret_code;
	
}/* ResetVariable */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetVariable                                         | *
 * |                                                                    | *
 * | DESCRIPTION:   Get the current value of the global variable  	   | *
 * /                identified by the "var" parameter filling the       / *
 * /                "value" parameter with the internal value of        | *
 * /                that variable                                       / * 
 * |                                                                    | *
 * | AUTHOR: 		  ssantos                                             | *
 * | DATE:          2006/11/17                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char*   var                     x	   	     Variable name         | *
 * / BOOLEAN value                          x     Current value         / *   
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:  An ARINC653 RETURN_CODE_TYPE which is either        | *
 * /                NO_ERROR or INVALID_PARAM                           / *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE GetVariable (char* var, BOOLEAN *value)
{
				
	RETURN_CODE_TYPE ret_code = NO_ERROR;
	
	*value = FALSE;
		

	/* get GV1 value */			
	if (strcmp (var, "GV1") == 0)
	{
		if (GV1 == TRUE)
		{
			*value = TRUE;
		}
	}
	
	/* or GV2 value */
	else if (strcmp (var, "GV2") == 0)	
	{
		if (GV2 == TRUE)
		{
			*value = TRUE;
		}
	}
	
	/* or GV3 value */
	else if (strcmp (var, "GV3") == 0)
	{
		if (GV3 == TRUE)
		{
			*value = TRUE;
		}
	}
	
	
	/* error condition: no var with the specified name*/
	else
	{
		ret_code = INVALID_PARAM;
	}
	
	return ret_code;
	
} /* GetVariable */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: AVT_GlobalVariables.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

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
 * | FILE: AVT_Stdlib_replacer.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTFUNCS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S):  stdlib_replacer                            | *
 * |                                                                    | *
 * | DESCRIPTION: Auxiliary functions                                   | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    25/09/2002                                                | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_Stdlib_replacer.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: atoi                                                | *
 * |                                                                    | *
 * | DESCRIPTION: Make atoi by name                                     | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | EVENT_NAME_TYPE   EvName        X            Event name            | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE  RetCode                           | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

int atoi ( char s[] )
{
	int i    = 0;
	int n    = 0;
	n        = 0;
	
	for ( i = 0; s[i] >= '0' && s[i] <= '9'; i++ )
	{
		n = 10 * n + ( s[i] - '0' );
	}
	return ( n );
}  /*atoi*/



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |             END OF FILE: AVT_Stdlib_replacer.c                     | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */



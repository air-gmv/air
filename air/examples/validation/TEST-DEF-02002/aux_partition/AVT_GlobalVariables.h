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
 * | FILE: AVT_GlobalVariables.h                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: ................                                        | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concerning AVT_GlobalVariables.c source.              | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:      ssantos                                               | *
 * | DATE:        2006/11/17                                            | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_GLOBALVARIABLES_H_
#define _AVT_GLOBALVARIABLES_H_

#include <string.h>

#include <a653.h>
#include <imaspex.h>
#include "AVT_TARGET_Boolean.h"


#define INIT_ERRORMSG_SIZE 128


RETURN_CODE_TYPE InitGlobalVariables(char *);

RETURN_CODE_TYPE ResetVariable(char *);

RETURN_CODE_TYPE SetVariable(char *);

RETURN_CODE_TYPE GetVariable(char *, BOOLEAN *);

#endif

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                       END OF FILE: AVT_GlobalVariables.h           | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

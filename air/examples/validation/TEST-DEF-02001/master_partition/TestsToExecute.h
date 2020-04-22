#ifndef _TESTS_TO_EXECUTE_H_
#define _TESTS_TO_EXECUTE_H_



// /* SUCCESS without the trace*/
#define EXECUTE_T_API_INTER_520_0011
// //#define TRACE_T_API_INTER_520_0011

// /* FAILED  CREATE_SAMPLING_PORT - fails the exaustion test*/
//#define EXECUTE_T_API_INTER_520_0012
// //#define TRACE_T_API_INTER_520_0012

// /* SUCCESS CREATE_SAMPLING_PORT - I guess PARTITION_MODE verification is made even before getting the port */
#define EXECUTE_T_API_INTER_520_0013
// //#define TRACE_T_API_INTER_520_0013

/* SUCCESS WRITE_SAMPLING_MESSAGE  - blocks the test sequence */
#define EXECUTE_T_API_INTER_530_0011
//#define TRACE_T_API_INTER_530_0011

// /* SUCCESS WRITE_SAMPLING_MESSAGE */
#define EXECUTE_T_API_INTER_530_0012
// //#define TRACE_T_API_INTER_530_0012

// /* SUCCESS - This test depends on the value given as refreshRate */
// //93
#define EXECUTE_T_API_INTER_540_0011
// //#define TRACE_T_API_INTER_540_0011

// /* This test is not implemented by AVT*/
// //#define EXECUTE_T_API_INTER_540_0012

// /* SUCCESS */
// /* This test is tricky. If it is executed alone, it passes. But if it is executed
//  * after the other test, it fails because avt expects the port to be empty. A653
//  * do not specify that a read message must be removed from the port. */
//#define EXECUTE_T_API_INTER_540_0013
// //#define TRACE_T_API_INTER_540_0013

// /* SUCCESS READ_SAMPLING_MESSAGE */
#define EXECUTE_T_API_INTER_540_0014

// /* SUCCESS */
#define EXECUTE_T_API_INTER_550_0011
// //#define EXECUTE_T_API_INTER_550_0012

#define EXECUTE_T_API_INTER_550_0012


#endif




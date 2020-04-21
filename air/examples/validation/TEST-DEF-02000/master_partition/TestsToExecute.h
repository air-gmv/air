#ifndef _TESTS_TO_EXECUTE_H_
#define _TESTS_TO_EXECUTE_H_

/* FAILED SET_PARTITION_MODE WARM_START - master partition hangs in unsent message */
/* SUCCESS without the trace. Trace function blocks in getting the PERIOD value for the partition */
//#define EXECUTE_T_API_PART_230_0010
//#define TRACE_T_API_PART_230_0010

/*pending uses the second process of aux partition */
//#define EXECUTE_T_API_PART_230_0020
//#define TRACE_T_API_PART_230_0020

/*FAILED SET_PARTITION_MODE WARM_START -Function ReceiveQueuingMessage. Error TIMED_OUT while attempting to do RECEIVE_QUEUING_MESSAGE */
/* This test fails because it expects the aux partition to be restarted and unblock the master partition by resenting the synchronization start up message */
//#define EXECUTE_T_API_PART_230_0030
//#define TRACE_T_API_PART_230_0030

/*FAILED SET_PARTITION_MODE to IDLE expected message TIME_OUT*/
/* This test fails because it expects the aux partition to be restarted and unblock the master partition by resenting the synchronization start up message */
//#define EXECUTE_T_API_PART_230_0035
//#define TRACE_T_API_PART_230_0035

// /* SUCCESS SET_PARTITION_MODE UNDEFINED fail with trace */
#define EXECUTE_T_API_PART_230_0040
// //#define TRACE_T_API_PART_230_0040

// /* SUCCESS SET_PARTITION_MODE UNDEFINED fail with trace */
#define EXECUTE_T_API_PART_230_0050
// //#define TRACE_T_API_PART_230_0050

// /* SET_PARTITION_MODE UNDEFINED master partition hangs in the middle of the test expecting an unsent message */
//#define EXECUTE_T_API_PART_230_0060
// //#define TRACE_T_API_PART_230_0060

// /* SUCCESS without the trace*/
#define EXECUTE_T_API_INTER_520_0011
// //#define TRACE_T_API_INTER_520_0011

#endif




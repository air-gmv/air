/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file rvs.h
 *  @author cdcs
 *
 *  @brief Integration function declarations for RAPITA tools
 */

//#define TEST_SCHEDULING_CORR        /* correctness of scheduling          */
//#define TEST_CONTEXT_SWITCH_PERF    /* context switch performance         */
//#define TEST_CLOCK_TICK_PERF        /* clock tick performance             */
//#define TEST_SYSCALL_PERF           /* system call performance            */
//#define TEST_POS_CLOCK_TICK_PERF    /* clock tick announce performance    */
//#define TEST_PMK_CRITICAL_PERF      /* PMK critical section performance */

#ifndef __RVS_H__
#define __RVS_H__

#ifndef ASM
/**
 * @brief Initialise the instrumentation library
 */
void RVS_Init(void);

/**
 * @brief Output the current coverage data
 */
void RVS_Output(void);

/**
 * @brief Set the build id for the instrumented source code
 */
void RVS_Build_Id(const char *build_id);

/**
 * @brief Set the start of a new test
 */
// void RVS_Begin_Test (unsigned int test_id, char* test_description);
void RVS_instrument_init(int i_n, void *i_p);

#endif /* ASM */
#endif /* __RVS_H__ */

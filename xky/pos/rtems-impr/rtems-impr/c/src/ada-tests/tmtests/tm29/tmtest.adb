--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 29 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;

package body TMTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      ID     : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 29 ***" );

      TMTEST.PERIOD_NAME := RTEMS.BUILD_NAME( 'P', 'R', 'D', ' ' );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_CREATE( TMTEST.PERIOD_NAME, ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_CREATE",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_CREATE
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_PERIOD( ID, 10, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_PERIOD (initial)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_PERIOD
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_PERIOD( ID, RTEMS.PERIOD_STATUS, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_PERIOD (STATUS)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_PERIOD
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_CANCEL( ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_CANCEL",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_CANCEL
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_DELETE( ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_DELETE (cancelled)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_DELETE
      );

      RTEMS.RATE_MONOTONIC_CREATE( TMTEST.PERIOD_NAME, ID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_CREATE" );

      RTEMS.RATE_MONOTONIC_PERIOD( ID, 10, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_PERIOD" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.RATE_MONOTONIC_DELETE( ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_DELETE (active)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_DELETE
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'E', 'S', 'T' ),
            128, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASK_START( ID, TMTEST.TASKS'ACCESS, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'L', 'O', 'W', ' ' ),
         200, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

      RTEMS.TASK_START( ID, TMTEST.LOW_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      TMTEST.TASK_COUNT := 0;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TASKS
--

   procedure TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      ID     : RTEMS.NAME;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.RATE_MONOTONIC_CREATE( TMTEST.PERIOD_NAME, ID, STATUS );
      RTEMS.RATE_MONOTONIC_PERIOD( ID, 10, STATUS );

      -- Give up the processor to allow all tasks to actually
      -- create and start their period timer before the benchmark
      -- timer driver is initialized.

      RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );

      TMTEST.TASK_COUNT := TMTEST.TASK_COUNT + 1;

      if TMTEST.TASK_COUNT = 1 then
         TIMER_DRIVER.INITIALIZE;
      end if;

      RTEMS.RATE_MONOTONIC_PERIOD( ID, 100, STATUS );

   end TASKS;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      OVERHEAD : RTEMS.UNSIGNED32;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_PERIOD (blocking)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_PERIOD
      );
   
      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 29 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;

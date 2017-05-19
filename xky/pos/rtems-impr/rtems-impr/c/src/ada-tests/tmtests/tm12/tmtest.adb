--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 12 of the RTEMS
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
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 12 ***" );

      RTEMS.TASK_CREATE( 
         1,
         251, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START( 
         TASK_ID, 
         TMTEST.TEST_INIT'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TASK_ENTRY     : RTEMS.TASK_ENTRY;
      PRIORITY       : RTEMS.TASK_PRIORITY;
      TASK_ID        : RTEMS.ID;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      RTEMS.MESSAGE_QUEUE_CREATE(
         RTEMS.BUILD_NAME( 'M', 'Q', '1', ' ' ),
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         16,
         RTEMS.DEFAULT_OPTIONS,
         TMTEST.QUEUE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_CREATE" );

      PRIORITY := 250;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT - 1
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TASK_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         PRIORITY := PRIORITY - 1;

         if INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT - 1 then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.LOW_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( TASK_ID, TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

   end TEST_INIT;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      OVERHEAD       : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT 
      loop
      
         RTEMS.MESSAGE_QUEUE_SEND(
            TMTEST.QUEUE_ID,
            BUFFER_POINTER, 
            16,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" ); --XXX

      end loop;

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_SEND (readying)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_SEND 
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 12 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32 := 0;
      STATUS         : RTEMS.STATUS_CODES;
   begin
 
      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE_RECEIVE( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_RECEIVE" ); --XXX
 
   end LOW_TASKS;

end TMTEST;

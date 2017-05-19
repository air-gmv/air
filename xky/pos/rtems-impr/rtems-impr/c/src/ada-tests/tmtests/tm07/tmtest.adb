--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 7 of the RTEMS
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
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 7 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      TASK_ENTRY : RTEMS.TASK_ENTRY;
      PRIORITY   : RTEMS.TASK_PRIORITY;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 250;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         PRIORITY := PRIORITY - 1;

         if INDEX = 0 then
            TASK_ENTRY := TMTEST.LOW_TASK'ACCESS;
         elsif INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( TMTEST.TASK_ID( INDEX ), TASK_ENTRY, 0, STATUS );
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
      STATUS       : RTEMS.STATUS_CODES;
   begin

      if ARGUMENT > 0 then
         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_RESTART suspended/preempt",
            TMTEST.END_TIME, 
            TIME_TEST_SUPPORT.OPERATION_COUNT, 
            0,
            RTEMS_CALLING_OVERHEAD.TASK_RESTART
         );
      else
         RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      end if;

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 07 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS       : RTEMS.STATUS_CODES;
   begin
 
      TMTEST.TASK_INDEX := TMTEST.TASK_INDEX + 1;

      if ARGUMENT > 0 then
         RTEMS.TASK_RESTART( 
            TMTEST.TASK_ID( TMTEST.TASK_INDEX ),
            16#7FFFFFFF#,
            STATUS
         );
      else
         RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      end if;

 
   end MIDDLE_TASKS;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_INDEX := 1;
      TIMER_DRIVER.INITIALIZE;

      RTEMS.TASK_RESTART( 
         TMTEST.TASK_ID( TMTEST.TASK_INDEX ),
         16#7FFFFFFF#,
         STATUS
      );

   end LOW_TASK;

end TMTEST;

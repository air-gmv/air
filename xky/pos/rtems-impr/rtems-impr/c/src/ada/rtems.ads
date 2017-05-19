--
--  RTEMS / Specification
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--  DEPENDENCIES:
--
--  NOTES:
--    RTEMS initialization and configuration are called from
--    the BSP side, therefore should never be called from ADA.
--
--  COPYRIGHT (c) 1997-2008.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with System;
with System.Storage_Elements; use System.Storage_Elements;
with Interfaces;
with Interfaces.C;

package RTEMS is
pragma Elaborate_Body (RTEMS);

   Structure_Alignment : constant := 8;

   --
   --  RTEMS Base Types
   --

   subtype Unsigned8  is Interfaces.Unsigned_8;
   subtype Unsigned16 is Interfaces.Unsigned_16;
   subtype Unsigned32 is Interfaces.Unsigned_32;
   subtype Signed32   is Interfaces.Integer_32;

   type Unsigned32_Pointer     is access all RTEMS.Unsigned32;
   type Unsigned16_Pointer     is access all RTEMS.Unsigned16;
   type Unsigned8_Pointer      is access all RTEMS.Unsigned8;
   type Signed32_Pointer       is access all RTEMS.Signed32;

   subtype Boolean             is RTEMS.Unsigned8;
   subtype Address             is System.Address;
   subtype Single              is Interfaces.C.C_float;
   subtype Double              is Interfaces.C.Double;

   --
   --  The following define the size of each of the base types in
   --  both bits and system units.
   --

   Unsigned8_Bits   : constant := 7;
   Unsigned16_Bits  : constant := 15;
   Unsigned32_Bits  : constant := 31;
   Boolean_Bits     : constant := 7;
   Address_Bits     : constant := 31;
   Single_Bits      : constant := 31;
   Double_Bits      : constant := 63;

   Unsigned8_Units  : constant := 1;
   Unsigned16_Units : constant := 2;
   Unsigned32_Units : constant := 4;
   Boolean_Units    : constant := 1;
   Address_Units    : constant := 4;
   Single_Units     : constant := 4;
   Double_Units     : constant := 8;

   Null_Address     : constant RTEMS.Address :=
      System.Storage_Elements.To_Address(0);

   True  : constant RTEMS.Boolean := 1;
   False : constant RTEMS.Boolean := 0;

   --
   --  More Types
   --

   subtype Name                is RTEMS.Unsigned32;
   subtype ID                  is RTEMS.Unsigned32;
   subtype Interval            is RTEMS.Unsigned32;
   subtype Attribute           is RTEMS.Unsigned32;
   subtype Mode                is RTEMS.Unsigned32;
   subtype Option              is RTEMS.Unsigned32;
   subtype Task_Priority       is RTEMS.Unsigned32;
   subtype Notepad_Index       is RTEMS.Unsigned32 range 0 .. 15;

   subtype Event_Set           is RTEMS.Unsigned32;
   subtype Signal_Set          is RTEMS.Unsigned32;
   subtype Debug_Set           is RTEMS.Unsigned32;
   subtype Device_Major_Number is RTEMS.Unsigned32;
   subtype Device_Minor_Number is RTEMS.Unsigned32;
   subtype ISR_Level           is RTEMS.Unsigned32;

   subtype Node                is RTEMS.Unsigned32;

   type Driver_Name_t is
      record
         Device_Name        : RTEMS.Address;
         Device_Name_Length : RTEMS.Unsigned32;
         Major              : RTEMS.Device_Major_Number;
         Minor              : RTEMS.Device_Minor_Number;

      end record;


   --
   --  Task Related Types
   --

   subtype Task_Argument       is RTEMS.Unsigned32;
   type Task_Argument_PTR      is access all Task_Argument;

   type Task_Entry is access procedure (
      Argument : RTEMS.Unsigned32
   );
   pragma Convention (C, Task_Entry);

   subtype TCB                 is RTEMS.Unsigned32;
   type    TCB_Pointer         is access all RTEMS.TCB;

   --
   --  Clock and Time of Day Types
   --

   type Time_Of_Day is
      record
         Year    : RTEMS.Unsigned32; -- year, A.D.
         Month   : RTEMS.Unsigned32; -- month, 1 .. 12
         Day     : RTEMS.Unsigned32; -- day, 1 .. 31
         Hour    : RTEMS.Unsigned32; -- hour, 0 .. 23
         Minute  : RTEMS.Unsigned32; -- minute, 0 .. 59
         Second  : RTEMS.Unsigned32; -- second, 0 .. 59
         Ticks   : RTEMS.Unsigned32; -- elapsed ticks between seconds
      end record;

   type Clock_Time_Value is
      record
         Seconds      : RTEMS.Unsigned32;
         Microseconds : RTEMS.Unsigned32;
      end record;

   type Clock_Get_Options is (
      Clock_Get_TOD,
      Clock_Get_Seconds_Since_Epoch,
      Clock_Get_Ticks_Since_Boot,
      Clock_Get_Ticks_Per_Second,
      Clock_Get_Time_Value
   );

   type Time_T is new Interfaces.C.Long;

   type Timespec is record
      TV_Sec  : Time_T;
      TV_Nsec : Interfaces.C.Long;
   end record;
   pragma Convention (C, Timespec);

   --
   --  Ident Options
   --

   Search_All_Nodes   : constant RTEMS.Node := 0;
   Search_Other_Nodes : constant RTEMS.Node := 16#7FFFFFFE#;
   Search_Local_Node  : constant RTEMS.Node := 16#7FFFFFFF#;
   Who_Am_I           : constant RTEMS.Node := 0;

   --
   --  Options
   --

   Default_Options    : constant RTEMS.Option    := 16#0000#;

   Wait      : constant RTEMS.Option := 16#0000#;
   No_Wait   : constant RTEMS.Option := 16#0001#;

   Event_All : constant RTEMS.Option := 16#0000#;
   Event_Any : constant RTEMS.Option := 16#0002#;

   --
   --  Mode constants
   --

   Default_Modes      : constant RTEMS.Mode      := 16#0000#;

   All_Mode_Masks     : constant RTEMS.Mode := 16#0000_ffff#;
   Current_Mode       : constant RTEMS.Mode := 16#0000_0000#;
   Preempt_Mask       : constant RTEMS.Mode := 16#0000_0100#;
   Timeslice_Mask     : constant RTEMS.Mode := 16#0000_0200#;
   ASR_Mask           : constant RTEMS.Mode := 16#0000_0400#;
   Interrupt_Mask     : RTEMS.Mode;
   Preempt            : constant RTEMS.Mode := 16#0000_0000#;
   No_Preempt         : constant RTEMS.Mode := 16#0000_0100#;
   No_Timeslice       : constant RTEMS.Mode := 16#0000_0000#;
   Timeslice          : constant RTEMS.Mode := 16#0000_0200#;
   ASR                : constant RTEMS.Mode := 16#0000_0000#;
   No_ASR             : constant RTEMS.Mode := 16#0000_0400#;

   pragma Import (C, Interrupt_Mask, "rtems_interrupt_mask");

   --
   --  Attribute constants
   --

   Default_Attributes      : constant RTEMS.Attribute := 16#00000000#;
   No_Floating_Point       : constant RTEMS.Attribute := 16#00000000#;
   Floating_Point          : constant RTEMS.Attribute := 16#00000001#;
   Local                   : constant RTEMS.Attribute := 16#00000000#;
   Global                  : constant RTEMS.Attribute := 16#00000002#;
   FIFO                    : constant RTEMS.Attribute := 16#00000000#;
   Priority                : constant RTEMS.Attribute := 16#00000004#;
   Counting_Semaphore      : constant RTEMS.Attribute := 16#00000000#;
   Binary_Semaphore        : constant RTEMS.Attribute := 16#00000010#;
   Simple_Binary_Semaphore : constant RTEMS.Attribute := 16#00000020#;
   No_Inherit_Priority     : constant RTEMS.Attribute := 16#00000000#;
   Inherit_Priority        : constant RTEMS.Attribute := 16#00000040#;
   No_Priority_Ceiling     : constant RTEMS.Attribute := 16#00000000#;
   Priority_Ceiling        : constant RTEMS.Attribute := 16#00000080#;

   function Interrupt_Level (
      Level : in     RTEMS.Unsigned32
   ) return RTEMS.Attribute;
   pragma Import (C, Interrupt_Level, "rtems_interrupt_level_attribute");

   Minimum_Stack_Size : RTEMS.Unsigned32;
   pragma Import (C, Minimum_Stack_Size, "rtems_minimum_stack_size");

   --
   --  Notepad index constants
   --

   Notepad_0  : constant RTEMS.Unsigned32 := 0;
   Notepad_1  : constant RTEMS.Unsigned32 := 1;
   Notepad_2  : constant RTEMS.Unsigned32 := 2;
   Notepad_3  : constant RTEMS.Unsigned32 := 3;
   Notepad_4  : constant RTEMS.Unsigned32 := 4;
   Notepad_5  : constant RTEMS.Unsigned32 := 5;
   Notepad_6  : constant RTEMS.Unsigned32 := 6;
   Notepad_7  : constant RTEMS.Unsigned32 := 7;
   Notepad_8  : constant RTEMS.Unsigned32 := 8;
   Notepad_9  : constant RTEMS.Unsigned32 := 9;
   Notepad_10 : constant RTEMS.Unsigned32 := 10;
   Notepad_11 : constant RTEMS.Unsigned32 := 11;
   Notepad_12 : constant RTEMS.Unsigned32 := 12;
   Notepad_13 : constant RTEMS.Unsigned32 := 13;
   Notepad_14 : constant RTEMS.Unsigned32 := 14;
   Notepad_15 : constant RTEMS.Unsigned32 := 15;

   --
   --  Miscellaneous
   --

   No_Timeout       : constant RTEMS.Interval := 0;
   Self             : constant RTEMS.ID       := 0;
   Period_Status    : constant RTEMS.Interval := 0;
   Yield_Processor  : constant RTEMS.Interval := 0;
   Current_Priority : constant RTEMS.Task_Priority := 0;
   No_Priority      : constant RTEMS.Task_Priority := 0;

   --
   --  Extension Callouts and Table
   --

   type Thread_Create_Extension is access function (
      Current_Task : in     RTEMS.TCB_Pointer;
      New_Task     : in     RTEMS.TCB_Pointer
   ) return RTEMS.Boolean;
   pragma Convention (C, Thread_Create_Extension);

   type Thread_Start_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Started_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Start_Extension);

   type Thread_Restart_Extension is access procedure (
      Current_Task   : in     RTEMS.TCB_Pointer;
      Restarted_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Restart_Extension);

   type Thread_Delete_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Deleted_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Delete_Extension);

   type Thread_Switch_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Heir_Task    : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Switch_Extension);

   type Thread_Post_Switch_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Post_Switch_Extension);

   type Thread_Begin_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Begin_Extension);

   type Thread_Exitted_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Exitted_Extension);

   type Fatal_Error_Extension is access procedure (
      Error : in     RTEMS.Unsigned32
   );
   pragma Convention (C, Fatal_Error_Extension);

   type Extensions_Table is
      record
         Thread_Create      : RTEMS.Thread_Create_Extension;
         Thread_Start       : RTEMS.Thread_Start_Extension;
         Thread_Restart     : RTEMS.Thread_Restart_Extension;
         Thread_Delete      : RTEMS.Thread_Delete_Extension;
         Thread_Switch      : RTEMS.Thread_Switch_Extension;
         Thread_Post_Switch : RTEMS.Thread_Post_Switch_Extension;
         Thread_Begin       : RTEMS.Thread_Begin_Extension;
         Thread_Exitted     : RTEMS.Thread_Exitted_Extension;
         Fatal              : RTEMS.Fatal_Error_Extension;
      end record;

   type Extensions_Table_Pointer is access all Extensions_Table;

   --
   --  The following type define a pointer to a watchdog/timer service routine.
   --

   type Timer_Service_Routine is access procedure (
      ID          : in     RTEMS.ID;
      User_Data   : in     RTEMS.Address
   );
   pragma Convention (C, Timer_Service_Routine);

   --
   --  The following type define a pointer to a signal service routine.
   --

   type ASR_Handler is access procedure (
      Signals : in     RTEMS.Signal_Set
   );
   pragma Convention (C, ASR_Handler);

   --
   --  The following type defines the status information returned
   --  about a period.
   --

   type Rate_Monotonic_Period_States is (
     Inactive,               -- off chain, never initialized
     Owner_Is_Blocking,      -- on chain, owner is blocking on it
     Active,                 -- on chain, running continuously
     Expired_While_Blocking, -- on chain, expired while owner was was blocking
     Expired                 -- off chain, will be reset by next
                             --   rtems_rate_monotonic_period
   );

   for Rate_Monotonic_Period_States'Size use 32;

   for Rate_Monotonic_Period_States use (
     Inactive                => 0,
     Owner_Is_Blocking       => 1,
     Active                  => 2,
     Expired_While_Blocking  => 3,
     Expired                 => 4
   );

   type Rate_Monotonic_Period_Status is
      record
         Owner                            : RTEMS.ID;
         State                            : RTEMS.Rate_Monotonic_Period_States;
         Ticks_Since_Last_Period          : RTEMS.Unsigned32;
         Ticks_Executed_Since_Last_Period : RTEMS.Unsigned32;
      end record;

   --
   --  Method Completions Status Codes
   --

   type Status_Codes is (
      Successful,               -- successful completion
      Task_Exitted,             -- returned from a task
      MP_Not_Configured,        -- multiprocessing not configured
      Invalid_Name,             -- invalid object name
      Invalid_ID,               -- invalid object id
      Too_Many,                 -- too many
      Timeout,                  -- timed out waiting
      Object_Was_Deleted,       -- object deleted while waiting
      Invalid_Size,             -- specified size was invalid
      Invalid_Address,          -- address specified is invalid
      Invalid_Number,           -- number was invalid
      Not_Defined,              -- item has not been initialized
      Resource_In_Use,          -- resources still outstanding
      Unsatisfied,              -- request not satisfied
      Incorrect_State,          -- task is in wrong state
      Already_Suspended,        -- task already in state
      Illegal_On_Self,          -- illegal on calling task
      Illegal_On_Remote_Object, -- illegal for remote object
      Called_From_ISR,          -- called from wrong environment
      Invalid_Priority,         -- invalid task priority
      Invalid_Clock,            -- invalid date/time
      Invalid_Node,             -- invalid node id
      Not_Configured,           -- directive not configured
      Not_Owner_Of_Resource,    -- not owner of resource
      Not_Implemented,          -- directive not implemented
      Internal_Error,           -- RTEMS inconsistency detected
      No_Memory,                -- no memory left in heap
      IO_Error,                 -- driver IO error
      Proxy_Blocking            -- internal multiprocessing only
   );

   for Status_Codes'Size use 32;

   for Status_Codes use (
      Successful                  =>  0,
      Task_Exitted                =>  1,
      MP_Not_Configured           =>  2,
      Invalid_Name                =>  3,
      Invalid_ID                  =>  4,
      Too_Many                    =>  5,
      Timeout                     =>  6,
      Object_Was_Deleted          =>  7,
      Invalid_Size                =>  8,
      Invalid_Address             =>  9,
      Invalid_NumbeR              => 10,
      Not_Defined                 => 11,
      Resource_In_Use             => 12,
      Unsatisfied                 => 13,
      Incorrect_State             => 14,
      Already_Suspended           => 15,
      Illegal_On_Self             => 16,
      Illegal_On_Remote_Object    => 17,
      Called_From_ISR             => 18,
      Invalid_Priority            => 19,
      Invalid_Clock               => 20,
      Invalid_Node                => 21,
      Not_Configured              => 22,
      Not_Owner_Of_Resource       => 23,
      Not_ImplementeD             => 24,
      Internal_Error              => 25,
      No_Memory                   => 26,
      IO_Error                    => 27,
      Proxy_Blocking              => 28
   );

   --
   --  RTEMS Events
   --

   Pending_Events : constant RTEMS.Event_Set := 16#0000_0000#;
   All_Events     : constant RTEMS.Event_Set := 16#FFFF_FFFF#;
   Event_0        : constant RTEMS.Event_Set := 16#0000_0001#;
   Event_1        : constant RTEMS.Event_Set := 16#0000_0002#;
   Event_2        : constant RTEMS.Event_Set := 16#0000_0004#;
   Event_3        : constant RTEMS.Event_Set := 16#0000_0008#;
   Event_4        : constant RTEMS.Event_Set := 16#0000_0010#;
   Event_5        : constant RTEMS.Event_Set := 16#0000_0020#;
   Event_6        : constant RTEMS.Event_Set := 16#0000_0040#;
   Event_7        : constant RTEMS.Event_Set := 16#0000_0080#;
   Event_8        : constant RTEMS.Event_Set := 16#0000_0100#;
   Event_9        : constant RTEMS.Event_Set := 16#0000_0200#;
   Event_10       : constant RTEMS.Event_Set := 16#0000_0400#;
   Event_11       : constant RTEMS.Event_Set := 16#0000_0800#;
   Event_12       : constant RTEMS.Event_Set := 16#0000_1000#;
   Event_13       : constant RTEMS.Event_Set := 16#0000_2000#;
   Event_14       : constant RTEMS.Event_Set := 16#0000_4000#;
   Event_15       : constant RTEMS.Event_Set := 16#0000_8000#;
   Event_16       : constant RTEMS.Event_Set := 16#0001_0000#;
   Event_17       : constant RTEMS.Event_Set := 16#0002_0000#;
   Event_18       : constant RTEMS.Event_Set := 16#0004_0000#;
   Event_19       : constant RTEMS.Event_Set := 16#0008_0000#;
   Event_20       : constant RTEMS.Event_Set := 16#0010_0000#;
   Event_21       : constant RTEMS.Event_Set := 16#0020_0000#;
   Event_22       : constant RTEMS.Event_Set := 16#0040_0000#;
   Event_23       : constant RTEMS.Event_Set := 16#0080_0000#;
   Event_24       : constant RTEMS.Event_Set := 16#0100_0000#;
   Event_25       : constant RTEMS.Event_Set := 16#0200_0000#;
   Event_26       : constant RTEMS.Event_Set := 16#0400_0000#;
   Event_27       : constant RTEMS.Event_Set := 16#0800_0000#;
   Event_28       : constant RTEMS.Event_Set := 16#1000_0000#;
   Event_29       : constant RTEMS.Event_Set := 16#2000_0000#;
   Event_30       : constant RTEMS.Event_Set := 16#4000_0000#;
   Event_31       : constant RTEMS.Event_Set := 16#8000_0000#;

   --
   --  RTEMS Signals
   --

   All_Signals : constant RTEMS.Signal_Set := 16#7FFFFFFF#;
   Signal_0    : constant RTEMS.Signal_Set := 16#00000001#;
   Signal_1    : constant RTEMS.Signal_Set := 16#00000002#;
   Signal_2    : constant RTEMS.Signal_Set := 16#00000004#;
   Signal_3    : constant RTEMS.Signal_Set := 16#00000008#;
   Signal_4    : constant RTEMS.Signal_Set := 16#00000010#;
   Signal_5    : constant RTEMS.Signal_Set := 16#00000020#;
   Signal_6    : constant RTEMS.Signal_Set := 16#00000040#;
   Signal_7    : constant RTEMS.Signal_Set := 16#00000080#;
   Signal_8    : constant RTEMS.Signal_Set := 16#00000100#;
   Signal_9    : constant RTEMS.Signal_Set := 16#00000200#;
   Signal_10   : constant RTEMS.Signal_Set := 16#00000400#;
   Signal_11   : constant RTEMS.Signal_Set := 16#00000800#;
   Signal_12   : constant RTEMS.Signal_Set := 16#00001000#;
   Signal_13   : constant RTEMS.Signal_Set := 16#00002000#;
   Signal_14   : constant RTEMS.Signal_Set := 16#00004000#;
   Signal_15   : constant RTEMS.Signal_Set := 16#00008000#;
   Signal_16   : constant RTEMS.Signal_Set := 16#00010000#;
   Signal_17   : constant RTEMS.Signal_Set := 16#00020000#;
   Signal_18   : constant RTEMS.Signal_Set := 16#00040000#;
   Signal_19   : constant RTEMS.Signal_Set := 16#00080000#;
   Signal_20   : constant RTEMS.Signal_Set := 16#00100000#;
   Signal_21   : constant RTEMS.Signal_Set := 16#00200000#;
   Signal_22   : constant RTEMS.Signal_Set := 16#00400000#;
   Signal_23   : constant RTEMS.Signal_Set := 16#00800000#;
   Signal_24   : constant RTEMS.Signal_Set := 16#01000000#;
   Signal_25   : constant RTEMS.Signal_Set := 16#02000000#;
   Signal_26   : constant RTEMS.Signal_Set := 16#04000000#;
   Signal_27   : constant RTEMS.Signal_Set := 16#08000000#;
   Signal_28   : constant RTEMS.Signal_Set := 16#10000000#;
   Signal_29   : constant RTEMS.Signal_Set := 16#20000000#;
   Signal_30   : constant RTEMS.Signal_Set := 16#40000000#;
   Signal_31   : constant RTEMS.Signal_Set := 16#80000000#;

   --
   --  Utility Functions
   --

   function From_Ada_Boolean (
      Ada_Boolean : Standard.Boolean
   ) return RTEMS.Boolean;

   function To_Ada_Boolean (
      RTEMS_Boolean : RTEMS.Boolean
   ) return Standard.Boolean;

   function Milliseconds_To_Microseconds (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Unsigned32;

   function Microseconds_To_Ticks (
      Microseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval;

   function Milliseconds_To_Ticks (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval;

   procedure Name_To_Characters (
      Name : in     RTEMS.Name;
      C1   :    out Character;
      C2   :    out Character;
      C3   :    out Character;
      C4   :    out Character
   );

   function Get_Node (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32;

   function Get_Index (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32;

   function Are_Statuses_Equal (
      Status  : in     RTEMS.Status_Codes;
      Desired : in     RTEMS.Status_Codes
   ) return Standard.Boolean;

   function Is_Status_Successful (
      Status  : in     RTEMS.Status_Codes
   ) return Standard.Boolean;

   function Subtract (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return RTEMS.Unsigned32;

   function Are_Equal (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return Standard.Boolean;


   --
   --  RTEMS API
   --

   --
   --  Initialization Manager -- Shutdown Only
   --
   procedure Shutdown_Executive (
      Status : in     RTEMS.Unsigned32
   );

   --
   --  Task Manager
   --

   procedure Task_Create (
      Name             : in     RTEMS.Name;
      Initial_Priority : in     RTEMS.Task_Priority;
      Stack_Size       : in     Unsigned32;
      Initial_Modes    : in     RTEMS.Mode;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Task_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Node;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Task_Start (
      ID          : in     RTEMS.ID;
      Entry_Point : in     RTEMS.Task_Entry;
      Argument    : in     RTEMS.Task_Argument;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Task_Restart (
      ID       : in     RTEMS.ID;
      Argument : in     RTEMS.Task_Argument;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Task_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Task_Suspend (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Task_Resume (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Task_Is_Suspended (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Task_Set_Priority (
      ID           : in     RTEMS.ID;
      New_Priority : in     RTEMS.Task_Priority;
      Old_Priority :    out RTEMS.Task_Priority;
      Result       :    out RTEMS.Status_Codes
   );

   procedure Task_Mode (
      Mode_Set          : in     RTEMS.Mode;
      Mask              : in     RTEMS.Mode;
      Previous_Mode_Set :    out RTEMS.Mode;
      Result            :    out RTEMS.Status_Codes
   );

   procedure Task_Get_Note (
      ID      : in     RTEMS.ID;
      Notepad : in     RTEMS.Notepad_Index;
      Note    :    out RTEMS.Unsigned32;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Task_Set_Note (
      ID      : in     RTEMS.ID;
      Notepad : in     RTEMS.Notepad_Index;
      Note    : in     RTEMS.Unsigned32;
      Result  :    out RTEMS.Status_Codes
   );

   type Task_Variable_Dtor is access procedure (
      Argument : in     RTEMS.Address
   );
   pragma Convention (C, Task_Variable_Dtor);

   procedure Task_Variable_Add (
      ID            : in     RTEMS.ID;
      Task_Variable : in     RTEMS.Address;
      Dtor          : in     RTEMS.Task_Variable_Dtor;
      Result        :    out RTEMS.Status_Codes
   );

   procedure Task_Variable_Get (
      ID                  : in     RTEMS.ID;
      Task_Variable       :    out RTEMS.Address;
      Task_Variable_Value :    out RTEMS.Address;
      Result              :    out RTEMS.Status_Codes
   );

   procedure Task_Variable_Delete (
      ID                  : in     RTEMS.ID;
      Task_Variable       :    out RTEMS.Address;
      Result              :    out RTEMS.Status_Codes
   );

   procedure Task_Wake_When (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Task_Wake_After (
      Ticks  : in     RTEMS.Interval;
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Interrupt Manager
   --

   function Interrupt_Disable return RTEMS.ISR_Level;
   pragma Interface (C, Interrupt_Disable);
   pragma Interface_Name (Interrupt_Disable, "rtems_interrupt_disable");

   procedure Interrupt_Enable (
      Level : in     RTEMS.ISR_Level
   );
   pragma Interface (C, Interrupt_Enable);
   pragma Interface_Name (Interrupt_Enable, "rtems_interrupt_enable");

   procedure Interrupt_Flash (
      Level : in     RTEMS.ISR_Level
   );
   pragma Interface (C, Interrupt_Flash);
   pragma Interface_Name (Interrupt_Flash, "rtems_interrupt_flash");

   function Interrupt_Is_In_Progress return RTEMS.Boolean;
   pragma Interface (C, Interrupt_Is_In_Progress);
   pragma Interface_Name
     (Interrupt_Is_In_Progress, "rtems_interrupt_is_in_progress");

   --
   --  Clock Manager
   --

   procedure Clock_Set (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Clock_Get (
      Option      : in     RTEMS.Clock_Get_Options;
      Time_Buffer : in     RTEMS.Address;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Clock_Get_TOD (
      Time   :    out RTEMS.Time_Of_Day;
      Result :    out RTEMS.Status_Codes
   );

   procedure Clock_Get_TOD_Time_Value (
      Time   :    out RTEMS.Clock_Time_Value;
      Result :    out RTEMS.Status_Codes
   );

   procedure Clock_Get_Seconds_Since_Epoch(
      The_Interval :    out RTEMS.Interval;
      Result       :    out RTEMS.Status_Codes
   );

   function Clock_Get_Ticks_Per_Second
   return RTEMS.Interval;
   pragma Import (
      C,
      Clock_Get_Ticks_Per_Second,
      "rtems_clock_get_ticks_per_second"
   );

   function Clock_Get_Ticks_Since_Boot
   return RTEMS.Interval;
   pragma Import (
      C,
      Clock_Get_Ticks_Since_Boot,
      "rtems_clock_get_ticks_since_boot"
   );

   procedure Clock_Get_Uptime (
      Uptime :    out RTEMS.Timespec;
      Result :    out RTEMS.Status_Codes
   );

   procedure Clock_Tick (
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Extension Manager
   --

   procedure Extension_Create (
      Name   : in     RTEMS.Name;
      Table  : in     RTEMS.Extensions_Table_Pointer;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Extension_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Extension_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Timer Manager
   --

   procedure Timer_Create (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Timer_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Timer_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Timer_Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Timer_Server_Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Timer_Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Timer_Server_Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Timer_Reset (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Timer_Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Timer_Initiate_Server (
      Server_Priority : in     RTEMS.Task_Priority;
      Stack_Size      : in     Unsigned32;
      Attribute_Set   : in     RTEMS.Attribute;
      Result          :    out RTEMS.Status_Codes
   );

   --
   --  Semaphore Manager
   --

   procedure Semaphore_Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      Priority_Ceiling : in     RTEMS.Task_Priority;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Semaphore_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Semaphore_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Semaphore_Obtain (
      ID         : in     RTEMS.ID;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Semaphore_Release (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Semaphore_Flush (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Message Queue Manager
   --

   procedure Message_Queue_Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Max_Message_Size : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Send (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Urgent (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Broadcast (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Receive (
      ID         : in     RTEMS.ID;
      Buffer     : in     RTEMS.Address;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Size       : in out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Get_Number_Pending (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Message_Queue_Flush (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Event Manager
   --

   procedure Event_Send (
      ID       : in     RTEMS.ID;
      Event_In : in     RTEMS.Event_Set;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Event_Receive (
      Event_In   : in     RTEMS.Event_Set;
      Option_Set : in     RTEMS.Option;
      Ticks      : in     RTEMS.Interval;
      Event_Out  :    out RTEMS.Event_Set;
      Result     :    out RTEMS.Status_Codes
   );

   --
   --  Signal Manager
   --

   procedure Signal_Catch (
      ASR_Handler : in     RTEMS.ASR_Handler;
      Mode_Set    : in     RTEMS.Mode;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Signal_Send (
      ID         : in     RTEMS.ID;
      Signal_Set : in     RTEMS.Signal_Set;
      Result     :    out RTEMS.Status_Codes
   );

   --
   --  Partition Manager
   --

   procedure Partition_Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Buffer_Size      : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Partition_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Partition_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Partition_Get_Buffer (
      ID     : in     RTEMS.ID;
      Buffer :    out RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   );

   procedure Partition_Return_Buffer (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   );

   --
   --  Region Manager
   --

   procedure Region_Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Page_Size        : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Region_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Region_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Region_Extend (
      ID               : in     RTEMS.ID;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Region_Get_Segment (
      ID         : in     RTEMS.ID;
      Size       : in     RTEMS.Unsigned32;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Segment    :    out RTEMS.Address;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Region_Get_Segment_Size (
      ID         : in     RTEMS.ID;
      Segment    : in     RTEMS.Address;
      Size       :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Region_Return_Segment (
      ID      : in     RTEMS.ID;
      Segment : in     RTEMS.Address;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Region_Resize_Segment (
      ID         : in     RTEMS.ID;
      Segment    : in     RTEMS.Address;
      Size       : in     RTEMS.Unsigned32;
      Old_Size   :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

   --
   --  Dual Ported Memory Manager
   --

   procedure Port_Create (
      Name           : in     RTEMS.Name;
      Internal_Start : in     RTEMS.Address;
      External_Start : in     RTEMS.Address;
      Length         : in     RTEMS.Unsigned32;
      ID             :    out RTEMS.ID;
      Result         :    out RTEMS.Status_Codes
   );

   procedure Port_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Port_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Port_External_To_Internal (
      ID       : in     RTEMS.ID;
      External : in     RTEMS.Address;
      Internal :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Port_Internal_To_External (
      ID       : in     RTEMS.ID;
      Internal : in     RTEMS.Address;
      External :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   );

   --
   --  Input/Output Manager
   --

   procedure IO_Register_Name (
      Name   : in     String;
      Major  : in     RTEMS.Device_Major_Number;
      Minor  : in     RTEMS.Device_Minor_Number;
      Result :    out RTEMS.Status_Codes
   );

   procedure IO_Lookup_Name (
      Name         : in     String;
      Device_Info  :    out RTEMS.Driver_Name_t;
      Result       :    out RTEMS.Status_Codes
   );

   procedure IO_Open (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (IO_Open);

   procedure IO_Close (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (IO_Close);

   procedure IO_Read (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (IO_Read);

   procedure IO_Write (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (IO_Write);

   procedure IO_Control (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (IO_Control);

   --
   --  Fatal Error Manager
   --

   procedure Fatal_Error_Occurred (
      The_Error : in     RTEMS.Unsigned32
   );

   --
   --  Rate Monotonic Manager
   --

   procedure Rate_Monotonic_Create (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Period (
      ID      : in     RTEMS.ID;
      Length  : in     RTEMS.Interval;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Get_Status (
      ID      : in     RTEMS.ID;
      Status  :    out RTEMS.Rate_Monotonic_Period_Status;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Reset_Statistics (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Rate_Monotonic_Reset_All_Statistics;
   pragma Import (
      C,
      Rate_Monotonic_Reset_All_Statistics,
      "rtems_rate_monotonic_reset_all_statistics"
   );

   procedure Rate_Monotonic_Report_Statistics;
   pragma Import (
      C,
      Rate_Monotonic_Report_Statistics,
      "rtems_rate_monotonic_report_statistics"
   );

   --
   --  Barrier Manager
   --

   procedure Barrier_Create (
      Name            : in     RTEMS.Name;
      Attribute_Set   : in     RTEMS.Attribute;
      Maximum_Waiters : in     RTEMS.Unsigned32;
      ID              :    out RTEMS.ID;
      Result          :    out RTEMS.Status_Codes
   );

   procedure Barrier_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Wait (
      ID         : in     RTEMS.ID;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Barrier_Release (
      ID       : in     RTEMS.ID;
      Released :    out RTEMS.Unsigned32;
      Result   :    out RTEMS.Status_Codes
   );

   --
   --  Stack Bounds Checker
   --

   function Stack_Checker_Is_Blown return RTEMS.Boolean;
   pragma Interface (C, Stack_Checker_Is_Blown);
   pragma Interface_Name
     (Interrupt_Is_In_Progress, "rtems_stack_checker_is_blown");

   procedure Stack_Checker_Report_Usage;
   pragma Import (
      C, Stack_Checker_Report_Usage, "rtems_stack_checker_report_usage"
   );

   --
   --  CPU Usage Statistics
   --

   procedure CPU_Usage_Report;
   pragma Import (C, CPU_Usage_Report, "rtems_cpu_usage_report");

   procedure CPU_Usage_Reset;
   pragma Import (C, CPU_Usage_Reset, "rtems_cpu_usage_reset");

   --
   --  Debug Manager
   --

   Debug_All_Mask : constant RTEMS.Debug_Set := 16#ffffffff#;
   Debug_Region   : constant RTEMS.Debug_Set := 16#00000001#;

   procedure Debug_Enable (
      To_Be_Enabled : in     RTEMS.Debug_Set
   );

   procedure Debug_Disable (
      To_Be_Disabled : in     RTEMS.Debug_Set
   );

   function Debug_Is_Enabled (
      Level : in     RTEMS.Debug_Set
   ) return RTEMS.Boolean;

   --
   --  Object Services
   --

   function Build_Name (
      C1 : in     Character;
      C2 : in     Character;
      C3 : in     Character;
      C4 : in     Character
   ) return RTEMS.Name;

   procedure Object_Get_Classic_Name(
      ID     : in     RTEMS.ID;
      Name   :    out RTEMS.Name;
      Result :    out RTEMS.Status_Codes
   );

   procedure Object_Get_Name(
      ID     : in     RTEMS.ID;
      Name   :    out String;
      Result :    out RTEMS.Address
   );

   procedure Object_Set_Name(
      ID     : in     RTEMS.ID;
      Name   : in     String;
      Result :    out RTEMS.Status_Codes
   );

   procedure Object_Id_Get_API(
      ID  : in     RTEMS.ID;
      API :    out RTEMS.Unsigned32
   );

   procedure Object_Id_Get_Class(
      ID        : in     RTEMS.ID;
      The_Class :    out RTEMS.Unsigned32
   );

   procedure Object_Id_Get_Node(
      ID   : in     RTEMS.ID;
      Node :    out RTEMS.Unsigned32
   );

   procedure Object_Id_Get_Index(
      ID    : in     RTEMS.ID;
      Index :    out RTEMS.Unsigned32
   );

   function Build_Id(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      The_Node  : in     RTEMS.Unsigned32;
      The_Index : in     RTEMS.Unsigned32
   ) return RTEMS.Id;

   function Object_Id_API_Minimum return RTEMS.Unsigned32;

   function Object_Id_API_Maximum return RTEMS.Unsigned32;

   procedure Object_API_Minimum_Class(
      API     : in     RTEMS.Unsigned32;
      Minimum :    out RTEMS.Unsigned32
   );

   procedure Object_API_Maximum_Class(
      API     : in     RTEMS.Unsigned32;
      Maximum :    out RTEMS.Unsigned32
   );

   procedure Object_Get_API_Name(
      API  : in     RTEMS.Unsigned32;
      Name :    out String
   );

   procedure Object_Get_API_Class_Name(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      Name      :    out String
   );

   type Object_API_Class_Information is
     record
        Minimum_Id    : RTEMS.Id;
        Maximum_Id    : RTEMS.Id;
        Maximum       : RTEMS.Unsigned32;
        AutoExtend    : RTEMS.Boolean;
        Unallocated   : RTEMS.Unsigned32;
     end record;

   procedure Object_Get_Class_Information(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      Info      :    out RTEMS.Object_API_Class_Information;
      Result    :    out RTEMS.Status_Codes
   );

end RTEMS;

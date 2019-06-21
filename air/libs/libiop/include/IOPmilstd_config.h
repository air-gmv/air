/**
 *  @file 
 *  @brief This file contains the user configuration interface for the milstd bus
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cláudio Silva
 *	
 *  This is the generic part of the Bus Controller configuration interface and is
 *  independent of the target mil-std core.
 */

#ifndef __MILSTD_CONFIG_H__
#define __MILSTD_CONFIG_H__

/**
 *  @brief Bus Controller command block interface
 *
 *  This interface is used to define a mil-std command list. Commands defined
 *  in this list are executed by the mil-std core. It is possible to branch 
 *  between commands and to create dummy commands. The following options are
 *  available:
 *
 *  ccw, bit 31  (T/B)		: 0-Command is a transfer. 1-Command is a branch.
 *		 bit 30:20 (N/A)	: Reserved for future use
 *		 bit 19 (DUM)		: Dummy command. Do nothing but wait for timeslot.
 *		 bit 18 (END)		: End of list. Stop Transfers.
 *		 bit 17	(ETRIG)		: Wait for external trigger
 *	     bit 16	(EXCL)		: Time Slot is exclusive
 *		 bit 15:14 (N/A)	: Reserved
 *		 bit 13 (AB)		: Bus Selection 1 - Bus B, 0 - Bus A  
 *		 bit 12 (T/R)		: Transmit or Receive
 * 		 bit 11:8 (NRET)	: Number of Retries
 *		 bit 7:5 (N/A)	 	: Reserved
 *		 bit 4:0 (Mode Code): Word Count/Mode Code Value
 *
 *  rtaddr[0] and subaddr[0] :  RT address and subaddress (for rt-rt receive addresses)
 *  rtaddr[1] and subaddr[1] :  Only for RT-RT. Transmit addresses.
 *  subaddr[1] must be zero for non RT-RT transfers.
 *
 */
typedef struct {
	unsigned int 	ccw;			/**< Command Control Word */
    unsigned char  	rtaddr[2];   	/**< RT address */
    unsigned char  	subaddr[2];		/**< RT subaddress */
    unsigned int 	branch_offset;	/**< Branch offset */
	unsigned int 	time_slot;		/**< Time this command will use */
} bc_command_t;


#define TB_BIT 0x80000000
#define DUMMY_BIT 0x00080000
#define END_BIT 0x00040000
#define ETRIG_BIT 0x00020000
#define EXCL_BIT 0x00010000
#define BSEL_BIT 0x00002000
#define TR_BIT 0x00001000
#define AB_BIT 0x00002000
#define NRET_BITMASK 0x00000F00
#define MC_BITMASK 0x0000001F

#define CCW_BC_RT_A 0x00000400
#define CCW_BC_RT_B 0x00002400
#define CCW_RT_BC_A 0x00001400
#define CCW_RT_BC_B 0x00003400
#define CCW_RT_RT_A 0x00000400
#define CCW_RT_RT_B 0x00002400
#define CCW_DUMMY DUMMY_BIT
#define CCW_LOOP TB_BIT
#define CCW_END END_BIT


#endif

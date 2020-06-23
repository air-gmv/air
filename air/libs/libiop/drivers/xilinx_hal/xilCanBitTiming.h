/*
 * =====================================================================
 *  FILE:  $URL$
 * =====================================================================
 *  PROJECT:             :  AVIOAR
 *  VERSION              :  $Revision$
 *  LANGUAGE             :  C
 *  AUTHOR               :  $LastChangedBy$
 *  COPYRIGHT            :  AVOS - GMV,S.A.
 *  COMPILER             :  GNU GCC
 *  CREATED              :  $CreationDate$
 *  CLASS                :  xilCan
 *  LAST MODIFIED        :  $LastChangedDate$
 *  GENERATED FROM MODEL :  07-Mar-2018 08:02:00
 *  ORIGINAL MODEL AUTHOR:  fdpo
 *
 *..................................................................
 *  DESCRIPTION
 *  This configuration files provides CAN bit timing configuration in
 *  a simple way. Just one rate can be selected by macro declaration.
 *  Then, initialization software will take the BRPR (prescaler) and
 *  bit time segment values related to the rate.
 *..................................................................
 *  HISTORY
 *      DATE            VERSION         AUTHOR              REASONS
 *      3/02/2020       1               LUMM                ADAPTED for INFANTE project
 *
 *======================================================================
 */

#if !defined(XILCANBITTIMING_H_INCLUDED)
#define XILCANBITTIMING_H_INCLUDED

/* Selected Rate */
//#define CAN_BIT_RATE_1000KBPS
#define CAN_BIT_RATE_500KBPS

/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * works with a 40 MHz CAN clock. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 *
 * A good approach can be found at Xilinx UG585 document at CAN Controller chapter:
 *
 *                                        (freqCANController)
 * bitRate = --------------------------------------------------------------------------------
 *           (1 + BRPR_BAUD_PRESCALAR) * (3 + BTR_FIRST_TIMESEGMENT + BTR_SECOND_TIMESEGMENT)
 *
 */

#define CAN_STB_PIN  49u

#ifdef CAN_BIT_RATE_1000KBPS
#define BRPR_BAUD_PRESCALAR      1
#define BTR_SYNCJUMPWIDTH        3
#define BTR_SECOND_TIMESEGMENT   2
#define BTR_FIRST_TIMESEGMENT    15
#endif

#ifdef CAN_BIT_RATE_500KBPS
#define BRPR_BAUD_PRESCALAR      3
#define BTR_SYNCJUMPWIDTH        3
#define BTR_SECOND_TIMESEGMENT   2
#define BTR_FIRST_TIMESEGMENT    15
#endif

#endif /*!defined(XILCANBITTIMING_H_INCLUDED)*/

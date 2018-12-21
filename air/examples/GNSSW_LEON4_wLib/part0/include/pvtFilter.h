/*
 * File: pvtFilter.h
 *
 * Code generated for Simulink model 'pvtFilter'.
 *
 * Model version                  : 1.3856
 * Simulink Coder version         : 8.3 (R2012b) 20-Jul-2012
 * TLC version                    : 8.3 (Jul 21 2012)
 * C/C++ source code generated on : Wed Sep 12 13:34:31 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Generic->32-bit Embedded Processor
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_pvtFilter_h_
#define RTW_HEADER_pvtFilter_h_
#ifndef pvtFilter_COMMON_INCLUDES_
# define pvtFilter_COMMON_INCLUDES_
#include <math.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* pvtFilter_COMMON_INCLUDES_ */

#include "pvtFilter_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((void*) 0)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((void) 0)
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T meas_noise_matrix_full[576];
  real_T Py_full[576];
  real_T Y[576];
  real_T b_A[576];
  real_T Pycopy[576];
  real_T Pycopy_m[576];
} BlockIO_pvtFilter;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T TOW_in;                       /* '<Root>/TOW_in' */
  real_T GNSS_pos[36];                 /* '<Root>/GNSS_pos' */
  real_T GNSS_vel[36];                 /* '<Root>/GNSS_vel' */
  real_T GNSS_pos_corr[12];            /* '<Root>/GNSS_pos_corr' */
  real_T measurements_const_flag[12];  /* '<Root>/selected_sats_const' */
  real_T u_ctrl[3];                    /* '<Root>/u_ctrl' */
  real_T y_meas[12];                   /* '<Root>/y_meas' */
  real_T y_meas_doppler[12];           /* '<Root>/y_meas_doppler' */
  real_T freq_L1;                      /* '<Root>/freq_L1' */
  real_T speed_of_light;               /* '<Root>/speed_of_light' */
  real_T dt;                           /* '<Root>/dt' */
  real_T state_noise[5];               /* '<Root>/state_noise' */
  real_T meas_noise[2];                /* '<Root>/meas_noise' */
  real_T mu_CB;                        /* '<Root>/mu_CB' */
  real_T space_user_flag;              /* '<Root>/space_user_flag' */
  real_T meas_valid_flag[24];          /* '<Root>/meas_valid_flag' */
  real_T mahalanobis_threshold;        /* '<Root>/mahalanobis_threshold' */
  real_T x_state_init_eci[9];          /* '<Root>/x_init' */
  real_T p_init[81];                   /* '<Root>/p_init' */
  real_T x_priori_t[9];                /* '<Root>/x_priori_t' */
  real_T p_priori_t[81];               /* '<Root>/p_priori_t' */
} ExternalInputs_pvtFilter;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T x_est[9];                     /* '<Root>/x_est' */
  real_T P_est[81];                    /* '<Root>/P_est' */
  real_T maha_dist_full;               /* '<Root>/maha_dist_full' */
  real_T Py_full[576];                 /* '<Root>/Py_full' */
  real_T Pxy_full[216];                /* '<Root>/Pxy_full' */
  real_T y_residual_full[24];          /* '<Root>/y_residual_full' */
  real_T x_priori_t_plus_dt[9];        /* '<Root>/x_priori_t_plus_dt' */
  real_T p_priori_t_plus_dt[81];       /* '<Root>/p_priori_t_plus_dt' */
  real_T tow_priori_t_plus_dt;         /* '<Root>/tow_priori_t_plus_dt' */
} ExternalOutputs_pvtFilter;

/* Model entry point functions */
extern void pvtFilter_initialize(ExternalInputs_pvtFilter *pvtFilter_U,
  ExternalOutputs_pvtFilter *pvtFilter_Y);
extern void pvtFilter_step(BlockIO_pvtFilter *pvtFilter_B,
  ExternalInputs_pvtFilter *pvtFilter_U, ExternalOutputs_pvtFilter *pvtFilter_Y);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('GNSS_vis_sim_eci_GNSSW_EKF/GNSS_receiver1/Navigation_processing/GNSSW_prtEKF/GNSSW_EKF_preparation/pvtFilter')    - opens subsystem GNSS_vis_sim_eci_GNSSW_EKF/GNSS_receiver1/Navigation_processing/GNSSW_prtEKF/GNSSW_EKF_preparation/pvtFilter
 * hilite_system('GNSS_vis_sim_eci_GNSSW_EKF/GNSS_receiver1/Navigation_processing/GNSSW_prtEKF/GNSSW_EKF_preparation/pvtFilter/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'GNSS_vis_sim_eci_GNSSW_EKF/GNSS_receiver1/Navigation_processing/GNSSW_prtEKF/GNSSW_EKF_preparation'
 * '<S1>'   : 'GNSS_vis_sim_eci_GNSSW_EKF/GNSS_receiver1/Navigation_processing/GNSSW_prtEKF/GNSSW_EKF_preparation/pvtFilter'
 */

/*-
 * Requirements for '<Root>': pvtFilter
 */
#endif                                 /* RTW_HEADER_pvtFilter_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

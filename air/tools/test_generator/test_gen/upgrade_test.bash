#!/bin/bash
# 
# File:   upgrate_tests.bash
# Author: $(AIR_RUNNER_USER)
#
# Script to automatically upgrade unit test created in the old days for
# during Multima and IMA-SP
#
# Created on 10-Apr-2020, 11:15:49
#
find . -name '*.c' -print -exec sed -i 's/#include <pal_test.h>//g' {} \;
find . -name '*.c' -print -exec sed -i 's/#include <pal.h>/#include <air.h>/g' {} \;
find . -name '*.c' -print -exec sed -i 's/#include <pal_pprintf.h>/#include <air_test.h>/g' {} \;
find . -name '*.c' -print -exec sed -i 's/#include <pmk_hm.h>//g' {} \;
find . -name '*.c' -print -exec sed -i 's/hm_part_callback (pmk_hm_err_id_e i_error,void \*i_state)/partition_HM_callback(air_state_e state_id,air_error_e i_error)/g' {} \;
find . -name '*.c' -print -exec sed -i 's/pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;//g' {} \;
find . -name '*.c' -print -exec sed -i 's/test_step_report( /test_report(__FILE__, __LINE__,    /g' {} \;
find . -name '*.c' -print -exec sed -i 's/pal_schedule_change/air_syscall_set_schedule/g' {} \;
find . -name '*.c' -print -exec sed -i 's/pal_pprintf_process()/control_partition_init(1, TEST_MEMSHARE)/g' {} \;

find . -name '*.h' -print -exec sed -i 's/#include <pal_test.h>//g' {} \;
find . -name '*.h' -print -exec sed -i 's/#include <pal.h>/#include <air.h>/g' {} \;
find . -name '*.h' -print -exec sed -i 's/#include <pal_pprintf.h>/#include <air_test.h>/g' {} \;
find . -name '*.h' -print -exec sed -i 's/#include <pmk_hm.h>//g' {} \;
find . -name '*.h' -print -exec sed -i 's/hm_part_callback (pmk_hm_err_id_e i_error,void \*i_state)/partition_HM_callback(air_state_e state_id,air_error_e i_error)/g' {} \;
find . -name '*.h' -print -exec sed -i 's/pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;//g' {} \;
find . -name '*.h' -print -exec sed -i 's/test_step_report( /test_report(__FILE__, __LINE__,    /g' {} \;
find . -name '*.h' -print -exec sed -i 's/pal_schedule_change/air_syscall_set_schedule/g' {} \;
find . -name '*.h' -print -exec sed -i 's/pal_pprintf_process()/control_partition_init(1, TEST_MEMSHARE)/g' {} \;

find . -name '*.xml' -print -exec sed -i 's/RTEMSIMPR/RTEMS5/g' {} \;


## sed examples
##find . -name '*.c' -print -exec sed -i 's/AAA/BBB/g' {} \;
## find . -name '*.set_extension_here' -print -exec sed -i 's/write_here_what_to_search/write_here_replace_text/g' {} \;
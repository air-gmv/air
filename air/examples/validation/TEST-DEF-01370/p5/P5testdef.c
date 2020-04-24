
/* ***********************************************************************    */
/* ** TEST-DEF-01370 / 1370 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0140                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define two schedules: 1.P0, P1, P2, P3 2.P2, P3, P4, P5 Each partition
	should execute an MD5 of its own memory areas (data, bss and code). Then, it
	should keep repeating this MD5 calculation to check that the MD5 has not been
	altered. Partition 0 should, after one MD5 cycle, change the module schedule to the
	second schedule. The partitions that are common to both schedules (P2 and P3)
	should report no MD5 change. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P5testdef.h>
#include <md5.h>

/* Test external definitions **********************************************	*/
#define SECTIONS (8)

/* Test type definitions **************************************************	*/
typedef md5_byte_t md5_t[16];
typedef md5_t md5_pair[2];
typedef struct {
	char *p;
	int size;
} section_t;

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 50;

/* Test auxiliary functions    ********************************************	*/
void fill_section(section_t* section, void* start, void* end) {
	section->p = start;
	section->size = (((int) ((char*)end - (char*)start)) + 4095) & ~4095;
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
	/* signal error ocurrence	*/
    unexp_error	= i_error;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/       
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	  int i,j;
    int status = 0;

    section_t sections	[SECTIONS];
    md5_pair  md5s		[SECTIONS];

    int di;
    md5_state_t state;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 16 
    	Calculate the initial MD5 of the curren partition data, bss and code
	memory zones. Sleep until next MTF. */
    test_step_announce(16,1);

    /* Test step 16 code */
    fill_section(&(sections[0]),&air_memmap_part_text5_start    , &air_memmap_part_text5_end);
    fill_section(&(sections[1]),&air_memmap_part_data5_start    , &air_memmap_part_data5_end);
    fill_section(&(sections[2]),&air_memmap_part_bss5_start     , &air_memmap_part_bss5_end );
    fill_section(&(sections[3]),&air_memmap_part_init_code_start, &air_memmap_part_init_text0_end);
    fill_section(&(sections[4]),&air_memmap_part_init_data_start, &air_memmap_part_init_data0_end);
    fill_section(&(sections[5]),&air_memmap_part_init_bss_start , &air_memmap_part_init_bss0_end);
    fill_section(&(sections[6]),&air_memmap_pos_code_start   	  , &air_memmap_pos_code_end	);
    fill_section(&(sections[7]),&air_memmap_pos_data_start   	  , &air_memmap_pos_data0_end	);
    for (j=0;j < SECTIONS; j += 1) {
      md5_init	(&state);
      md5_append	(&state, (const md5_byte_t *)sections[j].p, sections[j].size);
      md5_finish	(&state,md5s[j][0]);
    }
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	rtems_task_wake_after(mtf_ticks>>1);

    /* Test Step 20 
    	Repeat the MD5 calculations, and check that no change as occured.
	Sleep until next MTF. */
    test_step_announce(20,1);

    /* Test step 20 code */
	for (j=0;j < SECTIONS; j += 1) {

		md5_init	(&state);
		md5_append	(&state, (const md5_byte_t *)sections[j].p, sections[j].size);
		md5_finish	(&state, md5s[j][1]);
		
		for (di = 0; di < 16; ++di) {
			if (md5s[j][0][di] != md5s[j][1][di]) {
				ret =  RTEMS_INTERNAL_ERROR;
				break;
			}
		}
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
        test_exit(TEST_FAILURE,mtf_ticks>>1);
    }
	rtems_task_wake_after(mtf_ticks>>1);

    
    /* Test End */
    test_return();
    return 0;
}




/* ***********************************************************************    */
/* ** TEST-DEF-00620 / 620 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test if the execution of a jump instruction into the data area, for both
	valid and invalid opcodes, in a user mode (non-previledged) partition is
	captured by the LEON and handled by the HM, and the associated HM action is called.
	Check that the remaining partitions are not affected. */

#include <rtems.h>

#include <pmk_linkcmd_vars.h>
#include <pmk_segregation.h>
#include <air.h>
#include <air_test.h>

#include <P1testdef.h>
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
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
void fill_section(section_t* section, void* start, void* end) {
	section->p = start;
	section->size = (((int) ((char*)end - (char*)start)) + 4095) & ~4095;
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= 1;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* repeatition iteration counter	*/ 
	int repeat     = 0;                
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

	/* Test Step 0 
	   Calculate the initial MD5 of the relevant partition 1 memory zones */
    test_step_announce(0,1);
	fill_section(&(sections[0]),&air_memmap_part_text1_start 	, &air_memmap_part_text1_end);
	fill_section(&(sections[1]),&air_memmap_part_data1_start	, &air_memmap_part_data1_end);
	fill_section(&(sections[2]),&air_memmap_part_bss1_start 	, &air_memmap_part_bss1_end );
	fill_section(&(sections[3]),&air_memmap_part_init_code_start, &air_memmap_part_init_text0_end);
	fill_section(&(sections[4]),&air_memmap_part_init_data_start, &air_memmap_part_init_data0_end);
	fill_section(&(sections[5]),&air_memmap_part_init_bss_start , &air_memmap_part_init_bss0_end);
	fill_section(&(sections[6]),&air_memmap_pos_code_start		, &air_memmap_pos_code_end	);
	fill_section(&(sections[7]),&air_memmap_pos_data_start		, &air_memmap_pos_data0_end	);
	// fill_section(&(sections[3]),&air_memmap_pos_bss_start		, &air_memmap_pos_bss0_end	);

	for (j=0;j < SECTIONS; j += 1) {
		md5_init	(&state);
		md5_append	(&state, (const md5_byte_t *)sections[j].p, sections[j].size);
		md5_finish	(&state,md5s[j][0]);
	}
    ret |= RTEMS_SUCCESSFUL;

    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

	rtems_task_wake_after(mtf_ticks >> 1);

    /* Test Step 8 
    	Check if MD5 was not altered */
    //test_step_announce(8,1);

    /* Test step 8 code */
    ret |= RTEMS_SUCCESSFUL;

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

    test_step_announce(8,1);
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

    
	rtems_task_wake_after(mtf_ticks >> 1);
    /* Test End */
    test_return(res);
    return 0;
}


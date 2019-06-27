/**
 * @file
 * @author pfnf
 * @brief GR1553 function definitions
 */

#ifndef __GR1553_SUPPORT_H__
#define __GR1553_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

typedef struct {
	/* Generic device configuration */
	iop_device_driver_t dev;

} iop_1553_device_t;

/* These do nothing */
uint32_t gr1553_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);

void gr1553_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);

void gr1553_read_task(air_uptr_t arg);
void gr1553_write_task(air_uptr_t arg);

#endif /* __GR1553_SUPPORT_H__ */

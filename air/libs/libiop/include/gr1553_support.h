/**
 * @file
 * @author pfnf
 * @brief GR1553 function definitions
 */

#ifndef __GR1553_SUPPORT_H__
#define __GR1553_SUPPORT_H__

#include <iop.h>
#include <rtems.h>
#include <iop_support.h>

/* These do nothing */
uint32_t gr1553_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);

void gr1553_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);

rtems_task gr1553bc_read_task(iop_physical_device_t *pdev);
rtems_task gr1553bc_write_task(iop_physical_device_t *pdev);

rtems_task gr1553rt_read_task(iop_physical_device_t *pdev);
rtems_task gr1553rt_write_task(iop_physical_device_t *pdev);

#endif /* __GR1553_SUPPORT_H__ */

/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xsd.c
 * @author TTAO
 * @brief XSD SD Driver
 */

#include <xsd.h>

air_u32_t iop_xsd_init(iop_device_driver_t *iop_dev, void *arg)
{

    iop_sd_device_t *device = (iop_sd_device_t *)iop_dev;
    xsd_struct *xsd = (xsd_struct *)(device->dev.driver);

    char *Path;
    FRESULT Res;

    if (device->id == 0)
    {
        Path = "0:/";
    }

    else if (device->id == 1)
    {
        Path = "1:/";
    }

    else
    {
        return AIR_NOT_AVAILABLE;
    }

    Res = f_mount(&xsd->fat, Path, device->format == 1);
    if (Res != FR_OK)
    {
        return AIR_DEVICE_ERROR;
    }

    if (device->format == 1)
    {
        unsigned char work[FF_MAX_SS];

        Res = f_mkfs(Path, FM_FAT32, 0, work, sizeof work);
        if (Res != FR_OK)
        {
            return AIR_DEVICE_ERROR;
        }
    }

    for (int i = 0; i < device->number_files; i++)
    {
        xsd->file_opened[i] = 0;
        xsd->append_byte[i] = 0;
    }

    device->all_read = 0;
    xsd->skip_open = 1;
    xsd->open_for_read = 0;

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xsd_open(iop_device_driver_t *iop_dev, void *arg)
{

    iop_sd_device_t *device = (iop_sd_device_t *)iop_dev;
    xsd_struct *xsd = (xsd_struct *)(device->dev.driver);

    if (xsd->skip_open == 0)
    {

        FRESULT Res;
        unsigned char mode;
        int file_id = (int)arg;

        if (xsd->open_for_read == 1)
        {
            mode = FA_OPEN_ALWAYS | FA_READ;
        }
        else
        {
            mode = FA_CREATE_ALWAYS | FA_WRITE;
        }

        Res = f_open(&xsd->fil[file_id], xsd->filename[file_id], mode);
        if ((Res) != 0)
        {
            return AIR_DEVICE_ERROR;
        }

        xsd->skip_open = 1;
        xsd->open_for_read = 0;
    }

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xsd_read(iop_device_driver_t *iop_dev, void *arg)
{

    iop_sd_device_t *device = (iop_sd_device_t *)iop_dev;
    xsd_struct *xsd = (xsd_struct *)(device->dev.driver);

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    FRESULT Res;
    int file_id = -1;
    unsigned int NumBytesRead;

    for (int i = 0; i < device->number_files; i++)
    {
        if (strcmp(device->header_read->file, xsd->filename[i]) == 0)
        {
            file_id = i;
            break;
        }
    }

    if (file_id == -1)
    {
        for (int i = 0; i < device->number_files; i++)
        {
            if (xsd->file_opened[i] == 0)
            {
                strcpy(xsd->filename[i], device->header_read->file);
                xsd->file_opened[i] = 1;
                file_id = i;
                break;
            }
        }
    }

    xsd->skip_open = 0;
    xsd->open_for_read = 1;

    Res = iop_xsd_open(iop_dev, file_id);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    Res = f_lseek(&xsd->fil[file_id], 0);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    char *msg_ptr = (char *)iop_buffer->v_addr + iop_buffer->payload_off;

    Res = f_read(&xsd->fil[file_id], (void *)msg_ptr, device->header_read->size, &NumBytesRead);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    xsd_copy_header(iop_dev, wrapper, device->header_read);
    iop_buffer->payload_size = NumBytesRead;

    Res = iop_xsd_close(iop_dev, file_id);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xsd_write(iop_device_driver_t *iop_dev, void *arg)
{

    iop_sd_device_t *device = (iop_sd_device_t *)iop_dev;
    xsd_struct *xsd = (xsd_struct *)(device->dev.driver);

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    sd_header_t *header = (sd_header_t *)get_header(iop_buffer);

    FRESULT Res;
    int file_id = -1;
    unsigned int NumBytesWritten;

    for (int i = 0; i < device->number_files; i++)
    {
        if (strcmp(header->file, xsd->filename[i]) == 0)
        {
            file_id = i;
            break;
        }
    }

    if (file_id == -1)
    {
        for (int i = 0; i < device->number_files; i++)
        {
            if (xsd->file_opened[i] == 0)
            {
                strcpy(xsd->filename[i], header->file);
                xsd->file_opened[i] = 1;
                file_id = i;
                break;
            }
        }
    }

    xsd->skip_open = 0;

    Res = iop_xsd_open(iop_dev, file_id);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    Res = f_lseek(&xsd->fil[file_id], xsd->append_byte[file_id]);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    char *msg_ptr = (char *)iop_buffer->v_addr + iop_buffer->payload_off;

    Res = f_write(&xsd->fil[file_id], (const void *)msg_ptr, iop_buffer->payload_size, &NumBytesWritten);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    xsd->append_byte[file_id] = xsd->append_byte[file_id] + NumBytesWritten;

    Res = iop_xsd_close(iop_dev, file_id);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xsd_close(iop_device_driver_t *iop_dev, void *arg)
{

    iop_sd_device_t *device = (iop_sd_device_t *)iop_dev;
    xsd_struct *xsd = (xsd_struct *)(device->dev.driver);

    FRESULT Res;
    int file_id = (int)arg;

    Res = f_close(&xsd->fil[file_id]);
    if ((Res) != 0)
    {
        return AIR_DEVICE_ERROR;
    }

    return AIR_SUCCESSFUL;
}

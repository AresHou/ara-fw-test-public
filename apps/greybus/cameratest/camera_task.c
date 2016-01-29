/**
 * Copyright (c) 2016 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "camera_task.h"

/**
 * @brief function to get camera module capabilities
 *
 * @param dbgfs_attr_para debugfs's parameters.
 * @param info camera tested items, including getting capabilities,
 * configurating stream, and stop/flush stream.
 * @param camera_test_id
 * @return 0 on success, error code on failure
 */
int get_capabilities(char *dbgfs_attr_para, struct inquiry_info *info,
                     unsigned int camera_test_id)
{
    int ret = 0;

    if ((info == NULL) || (dbgfs_attr_para == NULL) ||
        (camera_test_id != id_capabilities))
        return -EINVAL;

    info->cap_info = malloc(sizeof(struct capabilities_info));
    if (info->cap_info == NULL)
        return -ENOMEM;

    memset(info->cap_info, 0, sizeof(struct capabilities_info));

    /** Setup capabilities parameters */
    //TODO

    free(info->cap_info);
    return ret;
}

/**
 * @brief Setup camera module streams
 *
 * @param dbgfs_attr_para debugfs's parameters
 * @param info camera tested items, including getting capabilities,
 * configurating stream, and stop/flush stream.
 * @param camera_test_id An ID of camera tested items.
 * @return 0 on success, error code on failure
 */
int set_stream(char *dbgfs_attr_para, struct inquiry_info *info,
               unsigned int camera_test_id)
{
    char pick;
    int ret = 0;

    if ((info == NULL) || (dbgfs_attr_para == NULL) ||
        (camera_test_id != id_stream))
        return -EINVAL;

    info->strm_info = malloc(sizeof(struct stream_info));
    if (info->strm_info == NULL)
        return -ENOMEM;

    memset(info->strm_info, 0, sizeof(struct stream_info));

    /** Setup stream parameters */
    info->strm_info->flags = 0;
    info->strm_info->num_streams = 1;
    info->strm_info->img_fmt_code = 1;

    /** List all of resolutions white camera sensor support to screen */
    printf("Pick one mode you want to test:\n");
    printf("1.VGA   - 640*480\n");
    printf("2.QVGA  - 320*240\n");
    printf("3.720P  - 1280*720\n");
    printf("4.1080P - 1920*1080\n");
    printf("5.QSXGA - 2592*1944\n");
    printf("6.SXGA  - 1280*960\n");
    printf("7.XGA   - 1024*768\n\n");

    //GET_SINGLE_CHAR(pick);

    /** Pick one out for applying */
    do {
        GET_SINGLE_CHAR(pick);
        switch (pick) {
        case '1':
            info->strm_info->width = VGA_WIDTH;
            info->strm_info->height  = VGA_HEIGHT;
           break;
        case '2':
            info->strm_info->width   = QVGA_WIDTH;
            info->strm_info->height  = QVGA_HEIGHT;
           break;
        case '3':
            info->strm_info->width   = _720P_WIDTH;
            info->strm_info->height  = _720P_HEIGHT;
           break;
        case '4':
            info->strm_info->width   = _1080P_WIDTH;
            info->strm_info->height  = _1080P_HEIGHT;
           break;
        case '5':
            info->strm_info->width   = QSXGA_WIDTH;
            info->strm_info->height  = QSXGA_HEIGHT;
           break;
        case '6':
            info->strm_info->width   = SXGA_WIDTH;
            info->strm_info->height  = SXGA_HEIGHT;
           break;
        case '7':
            info->strm_info->width   = XGA_WIDTH;
            info->strm_info->height  = XGA_HEIGHT;
           break;
        default:
            printf("Wrong number:%c. Try again...\n", pick);
        }/** End of switch() */
    } while ((pick >= '8') || (pick == '0'));/** End of while() */

    /** Assign attr */
    ret = reallocate_attr(dbgfs_attr_para, info->strm_info, camera_test_id);
    if(ret)
        printf("***ERROR*** Re-allocate memory failure.\n");

    free(info->strm_info);
    return ret;
}

/**
 * @brief Start camera sensor streaming
 *
 * @param dbgfs_attr_para debugfs's parameters.
 * @param info camera tested items, including getting capabilities,
 * configurating stream, and stop/flush stream.
 * @param camera_test_id An ID of camera tested items.
 * @return 0 on success, error code on failure
 */
int set_capture(char *dbgfs_attr_para, struct inquiry_info *info,
                unsigned int camera_test_id)
{
    int ret = 0;

    if ((info == NULL) || (dbgfs_attr_para == NULL) ||
        (camera_test_id != id_capture))
        return -EINVAL;

    info->capt_info = malloc(sizeof(struct capture_info));
    if (info->capt_info == NULL) {
        ret = -ENOMEM;
        return ret;
    }

    memset(info->capt_info, 0, sizeof(struct capture_info));

    /** Setup capture parameters */
    info->capt_info->request_id = 1;
    info->capt_info->streams_mask = 1;
    info->capt_info->num_framems = 0;

    ret = reallocate_attr(dbgfs_attr_para, info->capt_info, camera_test_id);
    if(ret)
        printf("***ERROR*** Re-allocate memory failure.\n");

    free(info->capt_info);
    return ret;
}

/**
 * @brief Stop camera sensor streaming
 *
 * @param dbgfs_attr_para debugfs's parameters.
 * @param info camera tested items, including getting capabilities,
 * configurating stream, and stop/flush stream.
 * @param camera_test_id An ID of camera tested items.
 * @return 0 on success, error code on failure
 */
int set_flush(char *dbgfs_attr_para, struct inquiry_info *info,
              unsigned int camera_test_id)
{
    int ret = 0;

    if ((info == NULL) || (dbgfs_attr_para == NULL) ||
        (camera_test_id != id_flush))
        return -EINVAL;

    info->fh_info = malloc(sizeof(struct flush_info));
    if (info->fh_info == NULL) {
        ret = -ENOMEM;
        return ret;
    }

    memset(info->fh_info, 0, sizeof(struct flush_info));

    /** Setup flush parameters */
    info->fh_info->request_id = 1;

    ret = reallocate_attr(dbgfs_attr_para, info->fh_info, camera_test_id);
    if(ret)
        printf("***ERROR*** Re-allocate memory failure.\n");

    free(info->fh_info);
    return ret;
}

/**
 * @brief Setup attribute parameters
 *
 * @param dbgfs_attr debugfs's parameters.
 * @param re_attr A pointer to debugfs's parameters.
 * @param cam_test_id An ID of camera tested items.
 * @return 0 on success, error code on failure
 */
int reallocate_attr(char *dbgfs_attr_para, void *re_attr,
                    unsigned int cam_test_id)
{
    int ret = 0;
    unsigned int attr_para_size = 0;

    if ((re_attr == NULL) || (dbgfs_attr_para == NULL))
        return -EINVAL;

    memset(dbgfs_attr_para, '\0', sizeof(char) * TEMP_ATTR_SIZE);

    switch (cam_test_id) {
    case id_capabilities:
        /** capabilities info */
        //TODO
        break;
    case id_stream:
        /** stream info */
        /** attribute format: "num_streams;flags;width;height;img_fmt_code" */
        snprintf(dbgfs_attr_para, ATTR_PARA_SIZE, "%d;%d;%d;%d;%d",
                ((struct stream_info*)re_attr)->num_streams,
                ((struct stream_info*)re_attr)->flags,
                ((struct stream_info*)re_attr)->width,
                ((struct stream_info*)re_attr)->height,
                ((struct stream_info*)re_attr)->img_fmt_code);
       break;
    case id_capture:
        /** capturing info */
        snprintf(dbgfs_attr_para, ATTR_PARA_SIZE, "%d;%d;%d",
                ((struct capture_info*)re_attr)->request_id,
                ((struct capture_info*)re_attr)->streams_mask,
                ((struct capture_info*)re_attr)->num_framems);
       break;
    case id_flush:
        /** flush info */
        snprintf(dbgfs_attr_para, ATTR_PARA_SIZE, "%d;",
                ((struct flush_info*)re_attr)->request_id);
       break;
    default:
        printf("No camera test ID is selected. cam_test_id:%d\n", cam_test_id);
        return -EINVAL;
    }

    attr_para_size = strlen(dbgfs_attr_para);

    if(attr_para_size > ATTR_PARA_SIZE) {
        printf("***ERROR*** The size of attribute parameters doesn't match.\n");
        return -EINVAL;
    }

    if(attr_para_size > TEMP_ATTR_SIZE) {
        dbgfs_attr_para = (char*) realloc(dbgfs_attr_para,
                                          attr_para_size * sizeof(char));
        if (NULL == dbgfs_attr_para) {
            printf("***ERROR*** Fail to re-allocate memory.\n");
            return -ENOMEM;
        }
    }

    return ret;
}


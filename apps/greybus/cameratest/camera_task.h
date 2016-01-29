/*
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
#ifndef __CAMERA_TASK_H__
#define __CAMERA_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get a single character users input without character '/n'
 */
#define GET_SINGLE_CHAR(x) \
    do{ \
        x = getchar(); \
    } while (x == '\n');

/**
 * Attribute path, /sys/kernel/debug/greybus/camera-x.x,
 * is the default path of camera debugfs
 */
#define DEBUGFS_PATH "/sys/kernel/debug/greybus/camera-5.1"

/**
 * Attribute
 */
#define DEBUGFS_ATTR_CAPTABILITIES "capabilities"
#define DEBUGFS_ATTR_CFG_STREAMS "configure_streams"
#define DEBUGFS_ATTR_CAPTURE "capture"
#define DEBUGFS_ATTR_FLUSH "flush"
#define DEBUGFS_ATTR_METADATA "metadata"

#define TEMP_ATTR_SIZE 8
#define ATTR_PARA_SIZE 16

#define MODE_MAX 7

/**
 * Image sizes
 */
/** VGA */
#define VGA_WIDTH           640
#define VGA_HEIGHT          480
/** QVGA */
#define QVGA_WIDTH          320
#define QVGA_HEIGHT         240
/** 720P */
#define _720P_WIDTH         1280
#define _720P_HEIGHT        720
/** 1080P */
#define _1080P_WIDTH        1920
#define _1080P_HEIGHT       1080
/** QSXGA */
#define QSXGA_WIDTH         2592
#define QSXGA_HEIGHT        1944
/** SXGA */
#define SXGA_WIDTH          1280
#define SXGA_HEIGHT         960
/* XGA */
#define XGA_WIDTH           1024
#define XGA_HEIGHT          768

/**
 * @brief camera test ID for the camera tested items
 */
enum CAMERA_TEST_ID {
    id_capabilities = 1,
    id_stream,
    id_capture,
    id_flush,
};

/**
 * Struct of the items which wants to be verified.
 */
struct inquiry_info {
    /** capabilities */
    struct capabilities_info *cap_info;

    /** stream information */
    struct stream_info *strm_info;

    /** capture information */
    struct capture_info *capt_info;

    /** flush information */
    struct flush_info *fh_info;
};

/**
 * [TBD]
 * Struct of the capabilities information.
 */
struct capabilities_info {

    //TODO
    /**
     * Because capabilities function in firmware side doesn't complete yet,
     * place a dummy field here temporarily
     */
    int dummy_field;
};

/**
 * Struct of the stream information.
 */
struct stream_info {
    /** Image width */
    unsigned int    width;

    /** Image height */
    unsigned int    height;

    /** The number of stream */
    unsigned int    num_streams;

    /** Data Format */
    unsigned int    img_fmt_code;

    /** Flags set in the response by camera module */
    unsigned int    flags;
};

/**
 * Struct of the capture information.
 */
struct capture_info {
    /** An incrementing integer to uniquely identify the capture request */
    unsigned int    request_id;

    /** Bitmask of the streams included in the capture request */
    unsigned int    streams_mask;

    /** Number of frames to capture */
    unsigned int    num_framems;
};

/**
 * Struct of the flush information.
 */
struct flush_info {
    /** The request id set by capture */
    unsigned int    request_id;
};

/**
 * Function Prototype
 */
int get_capabilities(char *dbgfs_attr, struct inquiry_info *info, unsigned int camera_test_id);
int set_stream(char *dbgfs_attr, struct inquiry_info *info, unsigned int camera_test_id);
int set_capture(char *dbgfs_attr, struct inquiry_info *info, unsigned int camera_test_id);
int set_flush(char *dbgfs_attr, struct inquiry_info *info, unsigned int camera_test_id);

int reallocate_attr(char *dbgfs_attr, void *re_attr, unsigned int cam_test_id);

#ifdef __cplusplus
}
#endif

#endif /** __CAMERA_TASK_H__ */

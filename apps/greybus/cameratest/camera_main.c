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
#include <libfwtest.h>
#include "camera_task.h"

/**
 * @brief print usage.
 */
void print_usage(void)
{
    printf("\nUsage: cameratest < c | s | t | p | m | u | q >\n");
    printf("This test tool only support white camera:\n");
    printf("Camera Tests Options:\n");
    printf("    c : Get camera capabilities\n");
    printf("    s : Set streams configuration\n");
    printf("    t : Start to capture\n");
    printf("    p : Stop to capture\n");
    printf("    u : Check data transmission on CSI-2 TX/RX\n");
    printf("    q : Exit camera test application\n");
    printf("\n");
}

/**
 * @brief The main function of the camera test
 *
 * @param Command-line Arguments. This tool doesn't pass these arguments.
 * @return 0 on success, error code on failure
 */
int main(int argc, char **argv)
{
    struct inquiry_info *info = NULL;
    unsigned int attr_path_size = 0;
    int ret = 0;
    char cmd;
    char *dbgfs_attr_para = NULL;

    info = malloc(sizeof(*info));
    if (info == NULL) {
        return -ENOMEM;
    }

    memset(info, 0, sizeof(*info));

    dbgfs_attr_para = malloc(sizeof(char) * TEMP_ATTR_SIZE);
    if (dbgfs_attr_para == NULL) {
        printf("***ERROR*** Fail to allocate memory.\n");
        goto free_inquiry;
        ret = -ENOMEM;
    }

    memset(dbgfs_attr_para, '\0', sizeof(char) * TEMP_ATTR_SIZE);

    attr_path_size = strlen(DEBUGFS_PATH);

    print_usage();

    while((cmd = getchar()) != 'q')
    {
        switch (cmd) {
        case 'c':
            /** capabilities */
            printf("TBD\n");
#if 0
            ret = get_capabilities(dbgfs_attr_para, info, id_capabilities);
            if(ret) {
                printf("***ERROR*** Fail to setup capabilities.\n");
                ret = -ENOMEM;
                goto free_attr;
            }

            /** Get camera capabilities */
            ret = debugfs_set_attr(DEBUGFS_PATH, DEBUGFS_ATTR_CAPTABILITIES,
                                   dbgfs_attr_para, strlen(dbgfs_attr_para));
#endif
            break;
        case 's':
            /** configure stream */
            ret = set_stream(dbgfs_attr_para, info, id_stream);
            if(ret) {
                printf("***ERROR*** Fail to setup stream.\n");
                ret = -ENOMEM;
                goto free_attr;
            }

            /** configure camera stream */
            ret = debugfs_set_attr(DEBUGFS_PATH, DEBUGFS_ATTR_CFG_STREAMS,
                                   dbgfs_attr_para, strlen(dbgfs_attr_para));
            break;
        case 't':
            /** capture */
            ret = set_capture(dbgfs_attr_para, info, id_capture);
            if(ret) {
                printf("***ERROR*** Fail to setup capture.\n");
                ret = -ENOMEM;
                goto free_attr;
            }

            /** Start Capture */
            ret = debugfs_set_attr(DEBUGFS_PATH, DEBUGFS_ATTR_CAPTURE,
                                   dbgfs_attr_para, strlen(dbgfs_attr_para));
           break;
        case 'p':
            /** flush */
            ret = set_flush(dbgfs_attr_para, info, id_flush);
            if(ret) {
                printf("***ERROR*** Fail to setup flush.\n");
                ret = -ENOMEM;
                goto free_attr;
            }

            /** Stop Capture */
            ret = debugfs_set_attr(DEBUGFS_PATH, DEBUGFS_ATTR_FLUSH,
                                   dbgfs_attr_para, strlen(dbgfs_attr_para));
           break;
        case 'u':
            /** Read CSI-2 RX via attribute 0xd092 */
            printf("TBD\n");
            //TODO
           break;
        default:
            print_usage();
        }
    }//End of while loop

    free(dbgfs_attr_para);
    free(info);

    return 0;

free_attr:
    free(dbgfs_attr_para);
free_inquiry:
    free(info);

    return ret;
}

/*
 * Copyright (c) 2015 Google, Inc.
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
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include <libfwtest.h>
#include "commsteps.h"

struct gpio_app_info {
    uint16_t    case_id;
    uint16_t    base_pin;
    uint16_t    max_count;
    uint16_t    gpio_pin1;
    uint16_t    gpio_pin2;
    uint16_t    gpio_pin3;
    char        num_type[2];
};

/**
 * @brief Print usage of this GPIO test application
 *
 * @return None
 */
static void print_usage()
{
    printf("\nUsage: gpiotest [-c case-id] [-t number-type] [-1 gpio-pin1]"
           "[-2 gpio-pin2] [-3 gpio-pin3]\n");
    printf("    -c: TestLink test case ID.\n");
    printf("    -t: 's' for Single pin test or 'm' for Multiple pins test.\n");
    printf("    -1: GPIO pin1 number for single pin or multiple pins test\n");
    printf("    -2: GPIO pin2 number for multiple pins test\n");
    printf("    -3: GPIO pin3 number for multiple pins test\n");
    printf("Example : case ARA-270 use SDB board, GPIO had 3 pins can\n");
    printf("     test(GPIO0 GPIO8 GPIO9)\n");
    printf("     ./gpiotest -c 270 -t m -1 0 -2 8 -9\n");
 }

/**
 * @brief Set SDIO default parameters
 *
 * This is function to set GPIO parameters using in this application to default
 * value
 *
 * @param info The GPIO info from user
 * @return None
 */
static void default_params(struct gpio_app_info *info)
{
    info->case_id = 0;
    info->base_pin = 0;
    info->max_count = 0;
    snprintf(info->num_type, sizeof("") + 1, "%s", "");
    info->gpio_pin1 = 0;
    info->gpio_pin2 = 0;
    info->gpio_pin3 = 0;
}

/**
 * @brief Command parser
 *
 * Parse the command input from console
 *
 * @param info The GPIO info from user
 * @param argc Indicate how many input parameters
 * @param argv Commands input from console
 * @return 0 on success, negative errno on error
 */
static int command_parse(struct gpio_app_info *info, int argc, char **argv)
{
    int option;

    while ((option = getopt(argc, argv, "c:C:t:T:1:2:3:")) != ERROR) {
        switch(option) {
            case 'c':
                info->case_id = (uint16_t)atoi(optarg);
                break;
            case 'C':
                info->case_id = (uint16_t)atoi(optarg);
                break;
            case 't':
                snprintf(info->num_type, strlen(optarg) + 1, "%s", optarg);
                break;
            case 'T':
                snprintf(info->num_type, strlen(optarg) + 1, "%s", optarg);
                break;
            case '1':
                info->gpio_pin1 = (uint16_t)atoi(optarg);
                break;
            case '2':
                info->gpio_pin2 = (uint16_t)atoi(optarg);
                break;
            case '3':
                info->gpio_pin3 = (uint16_t)atoi(optarg);
                break;
            default:
                print_usage();
                return -EINVAL;
        }
    }

    return 0;
}

/**
 * @brief TestLink test case ARA-263
 *
 * ARA-263: GPIO line count response contains the number of GPIO lines used by
 * the GPIO Controller. This test case verifies that the GPIO Line Count
 * Response payload contains a one-byte value corresponding to the number of
 * lines managed by the GPIO Controller
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_263_get_count(struct gpio_app_info *info)
{
    int ret = 0;
    char gpiostr[PATH_MAX];
    /* Get debugfs ngpio buffer string */
    char countbuf[4];

    /* Read debugfs "ngpio to set GPIO_MAX_COUNT" */
    ret = get_greybus_gpio_count(info->base_pin, countbuf,
                                 sizeof(countbuf));
    info->max_count = atoi(countbuf);

    snprintf(gpiostr, sizeof(gpiostr), "GPIO count: %d", info->max_count);
    print_log(LOG_TAG, info->case_id, gpiostr);

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief TestLink test case ARA-264
 *
 * ARA-264: Generate multiple GPIO activate Request. This test case verifies
 * that multiple GPIO Activate Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_264_multiple_activate(struct gpio_app_info *info)
{
    int ret = 0;

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate GPIO multiple pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-267
 *
 * ARA-267: Generate multiple GPIO Deactivate Request. This test case verifies
 * that multiple GPIO Deactivate Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_267_multiple_deactivate(struct gpio_app_info *info)
{
    int ret = 0;

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief TestLink test case ARA-270
 *
 * ARA-270: Generate multiple GPIO Direction Request. This test case verifies
 * that multiple GPIO Direction Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_270_multiple_direction(struct gpio_app_info *info)
{
    int ret = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get multiple GPIO direction */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin1),
                                 directbuf, sizeof(directbuf));
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin2),
                                 directbuf, sizeof(directbuf));
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin3),
                                 directbuf, sizeof(directbuf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin1),
                                     directbuf, sizeof(directbuf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-271
 *
 * ARA-271: GPIO Direction Request multiple times for the same GPIO line. This
 * test case verifies that multiple GPIO Direction Request operations for the
 * same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_271_multiple_times_direction(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];
    /* TestLink test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Read GPIO direction 10 times */
    for(i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            ret = get_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), directbuf,
                                     sizeof(directbuf));
        } else {
            ret = -EINVAL;
        }
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-272
 *
 * ARA-272: GPIO Direction Request for all the GPIO lines. This test case
 * verifies that GPIO Direction Request Operations can be initiated for all the
 * GPIO lines including lines that have not been activated
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_272_all_direction(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];

    /* Activate all GPIO pins */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = activate_gpio_pin(info->case_id, (info->base_pin + i));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = get_gpio_direction(info->case_id, (info->base_pin + i),
                                     directbuf, sizeof(directbuf));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), directbuf,
                                 sizeof(directbuf));
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), directbuf,
                                 sizeof(directbuf));
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), directbuf,
                                 sizeof(directbuf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), directbuf,
                                 sizeof(directbuf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate all GPIO pins */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = deactivate_gpio_pin(info->case_id, (info->base_pin + i));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-273
 *
 * ARA-273: Generate multiple GPIO Direction Input Request. This test case
 * verifies that multiple GPIO Direction Input Request operations can be
 * executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_273_multiple_input(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "in";

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-274
 *
 * ARA-274: GPIO direction input request multiple times for the same GPIO line.
 * This test case verifies that multiple GPIO Direction Input Request operations
 * for the same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_274_multiple_times_input(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "in";
    /* TestLink test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input and set 10 times. */
    for (i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            strcpy(buf, directbuf);
            ret = set_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), buf, sizeof(buf));
        } else {
            ret = -EINVAL;
        }
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-276
 *
 * ARA-276: Generate multiple GPIO direction output request. This test case
 * verifies that multiple GPIO Direction Output Request operations can be
 * executed successfully.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_276_multiple_output(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "out";

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-277
 *
 * ARA-277: GPIO direction output request multiple times for the same line. This
 * test case verifies that multiple GPIO Direction Output Request operations for
 * the same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_277_multiple_times_output(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* TestLink test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output and set 10 times. */
    for (i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            strcpy(buf, directbuf);
            ret = set_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), buf, sizeof(buf));
        } else {
            ret = -EINVAL;
        }
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-279
 *
 * ARA-279: GPIO get response payload returns GPIO line current value. This test
 * case verifies that the GPIO Get Response payload contains a 1-byte value
 * indicating the GPIO Line Value
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_279_get_value(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "in";

    /* activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin2),
                             buf, sizeof(buf));
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin3),
                             buf, sizeof(buf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-281
 *
 * ARA-281: Set GPIO line to high. This test case verifies that a given GPIO
 * Line can be set to HIGH using the GPIO Set Request operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_281_set_value_high(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value, and verify GPIO value is 1 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, valuebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-282
 *
 * ARA-282: Set GPIO line to low. This test case verifies that a given GPIO Line
 * can be set to LOW using the GPIO Set Request operation
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_282_set_value_low(struct gpio_app_info *info)
{
   int ret = 0;
    char buf[4];                /* GPIO debugfs buffer */
    char directbuf[] = "out";   /* Set GPIO direction string */
    char valuebuf[] = "0";      /* Set GPIO value string */

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 0(low) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, directbuf);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

   /* Get GPIO value, and verify GPIO value is 0 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, valuebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-286
 *
 * ARA-286: GPIO IRQ type can be set to EDGE_RISING. This test case verifies
 * that the GPIO IRQ Type Response doesn't return an error when setting the GPIO
 * IRQ Type is set to IRQ_TYPE_EDGE_RISING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_286_set_edge_rising(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf[] = "rising";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-287
 *
 * ARA-287: GPIO IRQ type can be set to EDGE_FALLING. This test case verifies
 * that the GPIO IRQ Type Response doesn't return an error when setting the GPIO
 * IRQ Type is set to IRQ_TYPE_EDGE_FALLING.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_287_set_edge_falling(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf[] = "falling";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-288
 *
 * ARA-288: GPIO IRQ type can be set to EDGE_BOTH. This test case verifies that
 * the GPIO IRQ Type Response doesn't return an error when setting the GPIO IRQ
 * Type is set to IRQ_TYPE_EDGE_BOTH.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_288_set_edge_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-409
 *
 * ARA-409: Change input line to output line. This test case verifies that a
 * previously configure GPIO line as an Input Line can be reconfigured to an
 * Output Line
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_409_input_to_output(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf1[] = "in";
    /* Set GPIO direction string */
    char directbuf2[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf1);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf2);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value, and verify GPIO value is 1 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, valuebuf);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-410
 *
 * ARA-410: Change output line to input line. This test case verifies that a
 * previously configure GPIO line as an output line can be reconfigured to an
 * input line
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_410_output_to_input(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char directbuf1[] = "out";
    /* Set GPIO direction string */
    char directbuf2[] = "in";
    /* Set GPIO value string */
    char valuebuf[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf1);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf2);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value*/
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-411
 *
 * ARA-411: Change IRQ type from falling edge to rising edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_FALLIN to
 * IRQ_TYPE_EDGE_RISING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_411_falling_to_rising(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf1[] = "falling";
    /* Set GPIO edge string */
    char edgebuf2[] = "rising";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf1);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf1);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf2);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf2);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-412
 *
 * ARA-412: Change IRQ type from rising edge to falling edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_RISING to
 * IRQ_TYPE_EDGE_FALLING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_412_rising_to_falling(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf1[] = "rising";
    /* Set GPIO edge string */
    char edgebuf2[] = "falling";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf1);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf1);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf2);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf2);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-413
 *
 * ARA-413: Change IRQ type from rising edge to falling edge triggered. This
 * test case verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_RISING
 * to IRQ_TYPE_EDGE_BOTH
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_413_rising_to_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf1[] = "rising";
    /* Set GPIO edge string */
    char edgebuf2[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf1);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf1);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf2);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf2);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-416
 *
 * ARA-416: Change IRQ type from none to rising and falling edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_NONE to
 * IRQ_TYPE_EDGE_BOTH
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_416_none_to_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf1[] = "none";
    /* Set GPIO edge string */
    char edgebuf2[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf1);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf1);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf2);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf2);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief TestLink test case ARA-417
 *
 * ARA-417: Change IRQ type from rising and falling edge to none. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_BOTH to
 * IRQ_TYPE_NONE.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_417_both_to_none(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char directbuf[] = "out";
    /* Set GPIO value string */
    char valuebuf[] = "1";
    /* Set GPIO edge string */
    char edgebuf1[] = "both";
    /* Set GPIO edge string */
    char edgebuf2[] = "none";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, directbuf);
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, valuebuf);
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf1);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf1);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        strcpy(buf, edgebuf2);
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, edgebuf2);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Switch case number and check case number is correct.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int switch_case_number(struct gpio_app_info *info)
{
        switch (info->case_id) {
            case 263:
                return ARA_263_get_count(info);
            case 264:
                return ARA_264_multiple_activate(info);
            case 267:
                return ARA_267_multiple_deactivate(info);
            case 270:
                return ARA_270_multiple_direction(info);
            case 271:
                return ARA_271_multiple_times_direction(info);
            case 272:
                return ARA_272_all_direction(info);
            case 273:
                return ARA_273_multiple_input(info);
            case 274:
                return ARA_274_multiple_times_input(info);
            case 276:
                return ARA_276_multiple_output(info);
            case 277:
                return ARA_277_multiple_times_output(info);
            case 279:
                return ARA_279_get_value(info);
            case 281:
                return ARA_281_set_value_high(info);
            case 282:
                return ARA_282_set_value_low(info);
            case 286:
                return ARA_286_set_edge_rising(info);
            case 287:
                return ARA_287_set_edge_falling(info);
            case 288:
                return ARA_288_set_edge_both(info);
            case 409:
                return ARA_409_input_to_output(info);
            case 410:
                return ARA_410_output_to_input(info);
            case 411:
                return ARA_411_falling_to_rising(info);
            case 412:
                return ARA_412_rising_to_falling(info);
            case 413:
                return ARA_413_rising_to_both(info);
            case 416:
                return ARA_416_none_to_both(info);
            case 417:
                return ARA_417_both_to_none(info);
            default:
                print_log(LOG_TAG, 0, "Error: The command had error case_id.");
                return -EINVAL;
        }
}

/**
 * @brief The gpiotest main function
 *
 * @param argc The gpiotest main arguments count
 * @param argv The gpiotest main arguments data
 * @return 0 on success, error code on failure
 */
int main(int argc, char **argv)
{
    struct gpio_app_info info;
    int ret = 0, base_pin = 0, max_count;

    if (argc < 3) {
        print_usage();
        ret = -EINVAL;
    }

    default_params(&info);

    if (!ret) {
        if (command_parse(&info, argc, argv)) {
            ret = -EINVAL;
        }
    }

    /* 1. Check Greybus GPIO controller */
    if (!ret) {
        ret = check_greybus_gpio(&base_pin, &max_count);
        info.base_pin = base_pin;
        info.max_count = max_count;
        check_step_result(info.case_id, ret);
    }

    /* 2. Switch test case */
    if (!ret) {
        ret = switch_case_number(&info);
        check_step_result(info.case_id, ret);
    }

    return ret;
}

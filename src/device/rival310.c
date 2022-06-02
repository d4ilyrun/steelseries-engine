/**
 * \file rival310.c
 * \authors Léo Duboin
 *
 * Handlers and API definition for the Steelseries Rival 310 series.
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>

#include "device.h"
#include "driver.h"
#include "log.h"
#include "utils.h"

#define DEVICE_NAME rival310
#define DEVICE_NAME_STR "Steelseries Rival 310"

static bool rival310_handle_rgb(u8 red, u8 green, u8 blue);

/**
 * \see DEVICE_INFO
 */
struct driver_device_info *DEVICE_INFO(rival310)
{
    static struct driver_device_info device = {
        STEELSERIES_VENDOR_ID,    .product_id = 0x1720,
        .interface_number = 1,    .max_packet_size = 298,
        .endpoint_address = 0x80, .rgb_event_handler = rival310_handle_rgb};

    return &device;
}

static bool rival310_handle_rgb(u8 red, u8 green, u8 blue)
{
    char *msg;

    asprintf(&msg, DEVICE_NAME_STR ": Switching RGB to rgb(%d,%d,%d)", red,
             green, blue);

    log_print(msg, LOG_NONE);

    return true;
}

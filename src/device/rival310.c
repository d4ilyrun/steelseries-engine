/**
 * \file rival310.c
 * \authors Léo Duboin
 *
 * Handlers and API definition for the Steelseries Rival 310 series.
 *
 * \see https://github.com/FFY00/rival310-re
 */

#define _GNU_SOURCE

#include "device/rival310.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "device/device.h"
#include "driver.h"
#include "log.h"
#include "utils.h"

#define DEVICE_NAME rival310
#define DEVICE_NAME_STR "Steelseries Rival 310"

/**
 * \struct rival310_usb_protocol
 * \brief The 310's USB protocol structure
 *
 * \warning The protocol's structure is different when configuring LEDs and
 * buttons. Please use `rival310_btn_protocol` or `rival310_led_protocol`
 *
 * \see rival310_usb_led_protocol
 * \see rival310_usb_btn_protocol
 */
struct rival310_usb_protocol {
    u8 command;
    u8 data[RIVAL310_PROTOCOL_LENGTH - 1];
};

/// \brief The 310 USB protocol total length when configuring LEDs.
/// \see rival310_usb_protocol
struct rival310_led_protocol {
    u8 command;
    u8 data[RIVAL310_LED_PROTOCOL_LENGTH - 1];
};

/// \brief The 310 USB protocol total length when configuring buttons.
/// \see rival310_usb_led_protocol
#define rival310_btn_protocol rival310_usb_led_protocol

/**
 * \brief Checks the validity of a USB protocol request before sending it over.
 *
 * Some ill-formed requests may break the device entirely, it is important to
 * call this function to ensure you don't completely break the mouse.
 *
 * \warning THIS FUNCTION SHALL BE CALLED BEFORE SENDING ANY REQUEST.
 */
bool rival310_check_led_btn_request(struct rival310_led_protocol req)
{
    switch (req.command) {
    default:
        return true;
    }

    return true;
}

/**
 * \function rival310_handle_rgb
 * \brief construct and send a valid LED event request
 * \see https://github.com/FFY00/rival310-re/blob/master/5B.md
 *
 * \return `true` if the request was send, `false` otherwise
 */
static bool rival310_handle_rgb(void *raw_params)
{
    struct r310_rgb_params *params = raw_params;
    struct rival310_led_protocol request = {0};
    u8 *data = (u8 *)&request;

    request.command = 0x5B; // LED command number

    // led id number
    data[2] = params->led;

    if (params->cycle_duration.milliseconds < 1980 ||
        params->cycle_duration.milliseconds > 30000) {
        log_print("Rival 310: Invalid cycle duration. Set to 5000ms.",
                  LOG_WARNING);
        params->cycle_duration.milliseconds = 5000;
    }

    // Set cycle duration, in BIG ENDIAN order.
    data[3] = params->cycle_duration.lsb;
    data[4] = params->cycle_duration.msb;

    // Wether a cycle repeats, 0 if it repeats 1 if not
    data[19] = !params->repeat;

    // Set trigger
    // Trigger: When one of the selected button is pressed, the cycle restarts
    // Structure: 8 bits, each representing a btn, set to 1 if btn is a trigger.
    data[23] = params->triggers & 0x3F; // ignore bits 6-7

    // Set color or first color of a cycle
    data[28] = params->color.color.red;
    data[29] = params->color.color.green;
    data[30] = params->color.color.blue;

    // The first color in cycle should always be the same as in data[28-30].
    if (params->color.color.rgba != params->cycle[0].rgba) {
        log_print("Rival 310: First color in color_cycle should be the same as "
                  "the main one. Replacing it.",
                  LOG_WARNING);
        params->cycle[0] = params->color.color;
        params->cycle[0].alpha = true;
    }

    u8 last_point = 0;
    u8 nb_points = 0;
    size_t data_index = 31;
    union color *cycle = params->cycle;
    for (u8 i = 0; i < 255; ++i) {
        // We check if a point has been set using alpha. 0 if not set.
        if (!cycle[i].alpha)
            continue;

        printf("%X: %X\n", i, cycle[i].rgba);

        data[data_index++] = cycle[i].red;
        data[data_index++] = cycle[i].green;
        data[data_index++] = cycle[i].blue;

        // Replace alpha value by the offset since the last point
        data[data_index++] = (i - last_point) & 0xFF;
        last_point = i;
        nb_points += 1;
    }

    // number of "points" in a cycle, max 255 (u8).
    // A cycle has a duration of 255 ticks
    data[27] = nb_points;

    for (int i = 0; i < RIVAL310_LED_PROTOCOL_LENGTH; ++i)
        printf("%02X ", data[i]);
    // printf("%d: %02X\n", i, data[i]);
    fflush(stdout);

    if (!rival310_check_led_btn_request(request)) {
        log_add("Rival 310: Invalid LED request ! Stopped.", LOG_ERROR);
        return false;
    }

    return true;
}

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

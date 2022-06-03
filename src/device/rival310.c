/**
 * \file rival310.c
 * \authors Léo Duboin
 *
 * Handlers and API definition for the Steelseries Rival 310 series.
 *
 * \see https://github.com/FFY00/rival310-re
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

/**
 * \brief The 310's USB protocol total length
 * \warning The size changes when configuring LEDs and buttons.
 * \see RIVAL310_LED_PROTOCOL_LENGTH
 * \see RIVAL310_BTN_PROTOCOL_LENGTH
 */
#define RIVAL310_PROTOCOL_LENGTH 64

/// \see RIVAL310_PROTOCOL_LENGTH
#define RIVAL310_LED_PROTOCOL_LENGTH 262
#define RIVAL310_BTN_PROTOCOL_LENGTH RIVAL310_LED_PROTOCOL_LENGTH

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

/// \brief Available mouse buttons.
enum r310_trigger
{
    L_CLICK = 0x00,
    R_CLICK = 0x02,
    W_CLICK = 0x04,
    BACK = 0x10,
    FWD = 0x20,
    DPI = 0x40,
    NONE = 0x80
};

/**
 * \function rival310_handle_rgb
 * \brief construct and send a valid LED event request
 * \see https://github.com/FFY00/rival310-re/blob/master/5B.md
 *
 * \return `true` if the request was send, `false` otherwise
 */
static bool rival310_handle_rgb(u8 r, u8 g, u8 b, enum led_id led)
{
    struct rival310_led_protocol request = {0};
    u8 *data = (u8 *)&request;

    request.command = 0x5B; // LED command number

    // led id number
    data[2] = led;

    // Set cycle duration, in BIG ENDIAN order.
    union driver_duration duration = {0};

    if (duration.miliseconds < 1980 || duration.miliseconds > 30000) {
        log_print("Rival 310: Invalid cycle duration. Set to 5000ms.",
                  LOG_WARNING);
        duration.miliseconds = 5000;
    }

    data[3] = duration.lsb;
    data[4] = duration.msb;

    // Wether a cycle repeats, 0 if it repeats 1 if not
    bool repeat = false; // TODO: add choice
    data[19] = !repeat;

    // Set trigger
    // Trigger: When one of the selected button is pressed, the cycle restarts
    // Structure: 8 bits, each representing a btn, set to 1 if btn is a trigger.
    data[23] = DPI | BACK;
    data[23] |= 0x3F; // Ignore bits 6-7

    // number of "points" in a cycle, max 255 (u8).
    // A cycle has a duration of 255 ticks
    data[27] = 1; // TODO: add color cycle

    // Set color or first color of a cycle
    data[28] = r;
    data[29] = g;
    data[30] = b;

    // TODO: color-cycle: 31-261

    for (int i = 0; i < RIVAL310_LED_PROTOCOL_LENGTH; ++i)
        printf("%02X ", data[i]);
    printf("\n");

    if (!rival310_check_led_btn_request(request)) {
        log_add("Invalid LED request ! Stopped request.", LOG_ERROR);
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

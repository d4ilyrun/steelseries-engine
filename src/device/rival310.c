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

static bool rival310_handle_rgb(u8 red, u8 green, u8 blue);

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

bool rival310_handle_rgb(u8, u8, u8)
{
    struct rival310_led_protocol request = {0};
    return rival310_check_led_btn_request(request);
}

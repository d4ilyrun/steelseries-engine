#pragma once

#include "device.h"
#include "utils.h"

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
 * \union r310_driver_duration
 * \brief Represents a duration in miliseconds.
 *
 * The duration spans over 2 bytes and can later be separated into bytes.
 */
union r310_cycle_duration {
    u16 milliseconds;

    struct {
#ifdef __LITTLE_ENDIAN__
        u8 msb;
        u8 lsb;
#else
        u8 lsb;
        u8 msb;
#endif
    };
};

/// \brief Available mouse buttons.
enum r310_buttons
{
    L_CLICK = 0x00,
    R_CLICK = 0x02,
    W_CLICK = 0x04,
    BACK = 0x10,
    FWD = 0x20,
    DPI = 0x40,
    NONE = 0x80
};

struct r310_rgb_params {
    ///< The led to apply the config on
    enum mouse_led_id led;
    ///< The color and mode to apply
    struct device_color_mode color;
    ///< The duration of a cycle in milliseconds
    union r310_cycle_duration cycle_duration;
    ///< Wether a cycle should repeat
    bool repeat;
    ///< Buttons that should restart a cycle when clicked
    enum r310_buttons triggers;
    ///< The number of points in a cycle
    u8 points;
};

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

/// The mouse model supported by the driver.
/// Currently set to 'Rival 310' since it is the model I am using.
/// \warning should be changed when implementing support for different models
#define DEFAULT_DEVICE rival310

/// Steelseries' vendor id as shown by `lsusb`
#define STEELSERIES_VENDOR_ID 0x1038

/**
 * \brief Fetch information about the device.
 * \param _device the model of the device
 *
 * Currently available names are:
 * - rival310
 *
 * \see driver_device_info
 */
#define DEVICE_INFO(_device) device_info_##_device()

/**
 * \brief DEVICE_INFO function declaration.
 * \param _device the model of the device
 *
 * \see DEVICE_INFO
 */
#define DEVICE_INFO_DECL(_device) \
    struct driver_device_info *device_info_##_device()

/**
 * \enum mouse_led_id
 */
enum mouse_led_id
{
    LOGO,
    WHEEL
};

/**
 * \struct driver_device_info
 *
 * Holds information about a model of mouse.
 * This information is needed in order to communicate with it
 * and is unique to a model.
 *
 * Information about a device's USB interface and model
 * can be found using the `lsusb` command
 */
struct driver_device_info {
    /**
     * \brief The device's vendor's unique ID
     * \see `STEELSERIES_VENDOR_ID`
     */
    uint16_t vendor_id;
    uint16_t product_id; ///< The unique ID of the device's model

    /// USB interface related data

    uint16_t interface_number; ///< marked as `bInterfaceNumber` in `lsusb`
    uint16_t max_packet_size;  ///< marked as `wMaxPacketSize` in `lsusb`

    /**
     * \brief The address of one of the device's OUTPUT endpoints.
     *
     * Marked as `bEndpointAddress` in `lsusb` (should be marked OUT).
     * If no endpoint marked OUT is present, default to the endpoint 0.
     *
     * Example:
     *
     *  bEndpointAddress     0x82  EP 2 OUT
     *
     * The output endpoint is 2 (EP 2) and its address is 0x82.
     */
    uint8_t endpoint_address;

    /** \brief EVENT HANDLERS
     *
     * They are function pointers pointing to functions that handle certain
     * events related to the device. For example, changing the RGB
     * values will call the `rgb_event_handler` handler.
     *
     * If a device model doesn't handle a particular event, set the related
     * handler function pointer to NULL.
     */

    bool (*rgb_event_handler)(void *);
};

/**
 * \struct device_color_mode
 */
struct device_color_mode {
    /**
     * \enum color_mode
     * \brief The different existing RGB modes
     */
    enum color_mode
    {
        OFF,
        STATIC,
        CYCLE
    } mode;

    /**
     * \union color
     * \brief The actual color.
     *
     * Can be set using an `{R, G, B}` struct or its rgba integer equivalent.
     *
     * \warning When in cycle mode, this shoud also be the first color of the
     * cycle.
     */
    union color {
        struct {
#ifdef __LITTLE_ENDIAN__
            u8 red;
            u8 green;
            u8 blue;
            u8 alpha;
#else
            u8 alpha; // We need it here but it is ignored
            u8 blue;
            u8 green;
            u8 red;
#endif
        };

        u32 rgba;
    } color;
};

DEVICE_INFO_DECL(rival310);

#ifdef DEVICE_NAME
#undef DEVICE_NAME
#endif

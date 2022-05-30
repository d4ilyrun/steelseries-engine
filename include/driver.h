#pragma once

#include <stdint.h>

/// The driver's unique usb context pointer
/// \see https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html
/// extern libusb_context *driver_usb_ctx;

/// The mouse model supported by the driver.
/// Currently set to 'Rival 310' since it is the model I am using.
/// \warning should be changed when implementing support for different models
#define RIVAL_310 0
#define DEVICE RIVAL_310

/// Steelserie's vendor id as shown by `lsusb`
#define STEELSERIES_VENDOR_ID 0x1038

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
    uint16_t product_id; ///< The unique ID of the model

    /// USB interface related data

    uint16_t interface_number; ///< marked as `bInterfaceNumber` in `lsusb`
    uint16_t max_packet_size;  ///< marked as `wMaxPacketSize` in `lsusb`

    /**
     * The address of one of the device's OUTPUT endpoints.
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
};

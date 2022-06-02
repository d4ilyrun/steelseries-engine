#pragma once

#include <hidapi/hidapi.h>
#include <stdint.h>

/// A pointer to the currently opened USB-HID device
extern hid_device *driver_hid_device;

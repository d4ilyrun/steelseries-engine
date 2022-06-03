#include <hidapi/hidapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "device.h"
#include "driver.h"
#include "log.h"

/// Connect to a device by name
/// \see DEVICE_INFO
#define DRIVER_CONNECT_DEVICE(_device) driver_connect(DEVICE_INFO(_device))

#ifndef DRIVER_LOG_LEVEL
#define DRIVER_LOG_LEVEL LOG_MSG
#endif

bool driver_init()
{
    if (hid_init() == -1)
        return false;

    log_init(DRIVER_LOG_LEVEL);

    return true;
}

/**
 * \brief Connect to a known device
 * \return The device's HID-API struct
 */
hid_device *driver_connect(struct driver_device_info *device)
{
    return hid_open(device->vendor_id, device->product_id, NULL);
}

/// Safely exit the driver, freeing all memory and stopping all services.
void driver_exit(int exit_code)
{
    log_exit();
    _exit(exit_code);
}

hid_device *driver_hid_device;

int main(void)
{
    if (!driver_init()) {
        log_add("Couldn't init driver", LOG_ERROR);
        driver_exit(-1);
    }

    struct driver_device_info *rival310_info = DEVICE_INFO(rival310);
    rival310_info->rgb_event_handler(0x12, 0x12, 0x12, LOGO);

    driver_hid_device = DRIVER_CONNECT_DEVICE(DEFAULT_DEVICE);
    if (driver_hid_device == NULL) {
        log_add_hid("Unable to connect to device", LOG_ERROR);
        driver_exit(-2);
    }

    wchar_t name[512];
    hid_get_manufacturer_string(driver_hid_device, name, 512);
    printf("Serial Number String: (%d) %ls", name[0], name);

    driver_exit(0);
}

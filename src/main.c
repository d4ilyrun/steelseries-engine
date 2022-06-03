#include <hidapi/hidapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "device/device.h"
#include "device/rival310.h"
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

    driver_hid_device = DRIVER_CONNECT_DEVICE(DEFAULT_DEVICE);
    if (driver_hid_device == NULL) {
        log_add_hid("Unable to connect to device", LOG_ERROR);
        driver_exit(-2);
    }

    wchar_t name[512];
    hid_get_manufacturer_string(driver_hid_device, name, 512);
    printf("Serial Number String: (%d) %ls", name[0], name);

    struct driver_device_info *rival310_info = DEVICE_INFO(rival310);
    struct r310_rgb_params params = {
        LOGO, {CYCLE, .color.rgba = 0x3151FFFF}, {5000}, true, 0, {{{0}}}};

    params.cycle[0x21].alpha = true;
    params.cycle[0x44].rgba = 0x58FF36FF;
    params.cycle[0x65].alpha = true;
    params.cycle[0x88].rgba = 0xFF1CE5FF;
    params.cycle[0xA4].alpha = true;
    params.cycle[0xC0].rgba = 0xFFC13BFF;
    params.cycle[0xDE].alpha = true;
    params.cycle[0xFC].rgba = 0x3151FFFF;

    rival310_info->rgb_event_handler(&params);

    driver_exit(0);
}

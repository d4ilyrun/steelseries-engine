#pragma once

#include <hidapi/hidapi.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "utils.h"

/// A pointer to the currently opened USB-HID device
extern hid_device *driver_hid_device;

/**
 * \function driver_send_report
 * \brief Send report over to the connected device
 *
 * \param report Report ID, 0x0 for device which only support a single report.
 * \param data The bytes to send
 * \param size The size of data
 *
 * \return `true` on success, `false` on error.
 */
inline bool driver_send_report(u8 report, void *data, size_t size)
{
    if (driver_hid_device == NULL) {
        log_add("Trying to send report but no device but no device connected",
                LOG_ERROR);
        return false;
    }

    u8 raw_data[size + 1];

    raw_data[0] = report;
    memcpy(raw_data + 1, data, size);

    return hid_send_feature_report(driver_hid_device, raw_data, size + 1) != -1;
}

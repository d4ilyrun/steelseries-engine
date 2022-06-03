#pragma once

#include <stdbool.h>

#include "driver.h"

/**
 * \enum log_error_level
 * \brief The degree of importance of a log message
 */
enum log_error_level
{
    LOG_MSG = 0,
    LOG_WARNING,
    LOG_ERROR
};

/**
 * \brief Initialize the driver's logger.
 * \param level The minimum level needed for a log to be logged
 */
bool log_init(enum log_error_level level);

/*
 * \brief Print all saved logs.
 *
 * Write into the logger's file if given one.
 * All log messages will be free'd on display.
 */
void log_dump();

/// \brief Safely stop the logger
void log_exit();

/**
 * \brief Immediatly print a log line, without saving it first.
 *
 * The content of `hid_error` is automatically added to the message.
 *
 * \param msg The log message
 * \param size the message's size
 */
void log_print(char *msg, enum log_error_level level);

/**
 * \brief Store a log to display later.
 *
 * \warning Memory is allocated to store the new message.
 * You may want to free the given message after a call to this function.
 *
 * \param msg The log message
 * \param size The messae's size
 */
void log_add(char *msg, enum log_error_level level);

/**
 * \brief Similar to `log_add`. Add the content of `strerror`.
 * \see log_add
 * \see strerror(3)
 */
void log_add_errno(char *msg, enum log_error_level level);

/**
 * \brief Similar to `log_add`. Add the content of `hid_error`.
 * \see log_add
 */
void log_add_hid(char *msg, enum log_error_level level);

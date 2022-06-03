#define _GNU_SOURCE

#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * \struct log_message
 * \brief A log message to store for later use
 *
 * Messages are stored in a linked list, each message being a node.
 */
struct log_message {
    char *message;
    int size;

    /// The timestamp at which the message has been emitated
    time_t timestamp;

    enum log_error_level level;

    ///< The next message in the list
    struct log_message *next;
};

/// String representations for all log levels
static const char *level_str[] = {"MESSAGE", "WARNING", "ERROR"};

char *log_time_str();

/// Generate the arguments to pass to an output function
/// to ouput a log with the correct format
#define LOG_FORMAT(_msg, _time) "[%s] - %s\n", log_time_str((_time)), (_msg)

struct driver_logger {
    /**
     * \struct driver_logger
     * \brief File descriptor of the output file.
     * When using a file to store logs, set `has_log_file` to true.
     * \see has_log_file
     */
    int log_file;

    /// The minimum level of log displayed
    enum log_error_level level;

    ///< Wether the user asked to store logs in a file.
    bool has_log_file;

    ///< Linked list of stored messages
    struct log_message *messages;
};

/// The driver's logger
static struct driver_logger driver_logger = {0};

bool log_init(enum log_error_level level)
{
    driver_logger.level = level;
    return true;
}

void log_dump()
{
    while (driver_logger.messages != NULL) {
        struct log_message *message = driver_logger.messages;
        driver_logger.messages = message->next;

        // Error when allocating memory during storing
        if (message->size < 0)
            continue;

        // Filter out unwanted logs
        if (message->level >= driver_logger.level && message->size) {
            // Write message into log_file if given one
            if (driver_logger.has_log_file)
                write(driver_logger.log_file, message->message, message->size);

            fprintf(stderr, LOG_FORMAT(message->message, message->timestamp));
        }

        free(message->message);
        free(message);
    }
}

void log_exit()
{
    log_dump();

    if (driver_logger.has_log_file)
        close(driver_logger.log_file);

    while (driver_logger.messages != NULL) {
        struct log_message *message = driver_logger.messages;
        driver_logger.messages = message->next;
        free(message->message);
        free(message);
    }
}

void log_print(char *msg, enum log_error_level level)
{
    if (level < driver_logger.level)
        return;

    char *new_msg;
    int size = asprintf(&new_msg, "[%s]: %s", level_str[level], msg);

    // Negative on error
    if (size < 0)
        return;

    if (driver_logger.has_log_file)
        write(driver_logger.log_file, msg, size);

    fprintf(stderr, LOG_FORMAT(new_msg, time(NULL)));
    free(new_msg);
}

void log_add(char *msg, enum log_error_level level)
{
    struct log_message *new_log = malloc(sizeof(struct log_message));

    if (new_log != NULL) {
        *new_log = (struct log_message){NULL, 0, time(NULL), level,
                                        driver_logger.messages};

        driver_logger.messages = new_log;

        // Size is negative on error. Checked later in `log_dump`
        new_log->size =
            asprintf(&new_log->message, "[%s]: %s", level_str[level], msg);
    }
}

void log_add_errno(char *msg, enum log_error_level level)
{
    char *new_msg;
    int ret = asprintf(&new_msg, "%s: %s", msg, strerror(errno));

    log_add((ret < 0) ? msg : new_msg, level);

    // new_msg must be freed since log_add allocates memory to store its content
    free(new_msg);
}

void log_add_hid(char *msg, enum log_error_level level)
{
    char *new_msg;
    int ret = asprintf(&new_msg, "%s: %ls", msg, hid_error(driver_hid_device));

    log_add((ret < 0) ? msg : new_msg, level);

    // new_msg must be freed since log_add allocates memory to store its content
    free(new_msg);
}

/**
 * \brief Get string representation from a timestamp.
 * \param timestamp The timestamp to parse
 * Formated as: HH:MM:SS
 */
char *log_time_str(time_t timestamp)
{
    static char time_str[9];

    struct tm *current_time = localtime(&timestamp);

    if (current_time == NULL)
        snprintf(time_str, 9, "INVALID");
    else
        snprintf(time_str, 9, "%02d:%02d:%02d", current_time->tm_hour,
                 current_time->tm_min, current_time->tm_sec);

    return time_str;
}

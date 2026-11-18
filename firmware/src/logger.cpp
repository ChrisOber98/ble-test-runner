#include <Arduino.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"

static const size_t LOG_BUFFER_SIZE = 128;

void log_message(const char *message, ...)
{
    char message_buffer[LOG_BUFFER_SIZE];
    int prefix_size = snprintf(message_buffer, sizeof(message_buffer), "[%lu] ", millis());

    va_list args;
    va_start(args, message);
    int wrote_size = vsnprintf(message_buffer + prefix_size, sizeof(message_buffer) - prefix_size, message, args);
    va_end(args);

    if (wrote_size < 0)
    {
        Serial.println("FAILED TO LOG MESSAGE");
        return;
    }

    if ((size_t)wrote_size >= sizeof(message_buffer) - prefix_size)
    {
        size_t postfix_index = strlen(message_buffer) - 4;

        message_buffer[postfix_index] = ' ';
        message_buffer[postfix_index + 1] = '.';
        message_buffer[postfix_index + 2] = '.';
        message_buffer[postfix_index + 3] = '.';
    }

    Serial.println(message_buffer);
}

#pragma once

/**
 * @file slog.h
 * @brief Simple buffered logging API.
 */

/**
 * @brief Append a formatted message to the log buffer.
 *
 * Works like printf(), but writes into an internal buffer instead of
 * directly to stdout.
 *
 * If there is insufficient space remaining in the buffer, the message
 * is silently truncated or dropped. This function never overflows the
 * buffer and never reports errors.
 *
 * @param fmt printf-style format string
 * @param ... Format arguments
 *
 * @note Thread-safe.
 * @note Failure is silent by design.
 */
void slog(const char* fmt, ...);

/**
 * @brief Flush the log buffer to stdout.
 *
 * Writes the entire contents of the internal buffer to stdout using
 * printf(), then resets the buffer to empty.
 *
 * After calling this function, logging starts again from the beginning
 * of the buffer.
 *
 * @note Thread-safe.
 */
void flushlog(void);

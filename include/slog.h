#pragma once
#include <stdio.h>

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
 * Writes the entire contents of the internal buffer to stdout, then
 * resets the buffer to empty.
 *
 * Any I/O errors encountered during flushing are reported to stderr.
 *
 * @note Thread-safe.
 */
void flushlog(void);

/**
 * @brief Drop the log buffer.
 *
 * Resets the log buffer to the start, and drops all logged messages.
 *
 * @note Thread-safe.
 */
void droplog(void);

/**
 * @brief Flush the log buffer to a FILE stream.
 *
 * The caller retains ownership of @p fileptr; this function does not
 * close the stream.
 *
 * Any I/O errors encountered during flushing are reported to stderr.
 *
 * @param fileptr Open FILE stream to write to.
 *
 * @note Thread-safe.
 */
void flushlog_fp(FILE* fileptr);

/**
 * @brief Flush the log buffer to a file path.
 *
 * Opens the file in append mode, flushes the buffer, and closes the file.
 *
 * Any I/O errors encountered during flushing are reported to stderr.
 *
 * @param file Path to the log file.
 *
 * @note Thread-safe.
 */
void flushlog_file(const char* file);

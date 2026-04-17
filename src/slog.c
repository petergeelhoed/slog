/**
 * @file slog.c
 * @brief Simple buffered logging implementation.
 *
 * This module provides a very small, safe logging facility that
 * accumulates formatted log messages in a fixed-size internal buffer.
 *
 * - Logging silently truncates when the buffer is full.
 * - No dynamic memory allocation is used.
 * - Thread-safe (C11).
 *
 * @note This file is intended to be built into a static library.
 */

#include "slog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <threads.h>
#include <time.h>

#ifndef SLOG_SIZE
#define SLOG_SIZE 4096
#endif

static char g_slog[SLOG_SIZE];
static char* g_slogpos = g_slog;

static mtx_t g_slog_mutex;
static once_flag g_slog_once = ONCE_FLAG_INIT;

static void slog_init(void) { (void)mtx_init(&g_slog_mutex, mtx_plain); }

static void slog_timestamp(char* buf, size_t buflen)
{
    struct timeval time_val;
    struct tm time;

    gettimeofday(&time_val, NULL);
    localtime_r(&time_val.tv_sec, &time);

    // NOLINTBEGIN(readability-magic-numbers)
    /* YYYY-MM-DD HH:MM:SS.mmm */
    (void)snprintf(buf,
                   buflen,
                   "%04d-%02d-%02dT%02d:%02d:%02d.%03ld",
                   time.tm_year + 1900,
                   time.tm_mon + 1,
                   time.tm_mday,
                   time.tm_hour,
                   time.tm_min,
                   time.tm_sec,
                   time_val.tv_usec / 1000);
    // NOLINTEND(readability-magic-numbers)
}

void slog(const char* fmt, ...)
{
    call_once(&g_slog_once, slog_init);
    va_list aplist;
    const int ts_buf_len = 32;
    char timestamp[ts_buf_len];

    (void)mtx_lock(&g_slog_mutex);

    size_t remaining = SLOG_SIZE - (size_t)(g_slogpos - g_slog);
    if (remaining <= 1)
    {
        (void)mtx_unlock(&g_slog_mutex);
        return;
    }

    slog_timestamp(timestamp, ts_buf_len);
    int ts_written = snprintf(g_slogpos, remaining, "[%s] ", timestamp);
    if (ts_written <= 0 || (size_t)ts_written >= remaining)
    {
        (void)mtx_unlock(&g_slog_mutex);
        return;
    }

    g_slogpos += ts_written;
    remaining -= (size_t)ts_written;

    va_start(aplist, fmt);
    int written = vsnprintf(g_slogpos, remaining, fmt, aplist);
    va_end(aplist);

    if (written > 0)
    {
        if ((size_t)written >= remaining)
        {
            /* Output truncated */
            g_slogpos += remaining - 1;
        }
        else
        {
            g_slogpos += written;
        }
    }

    (void)mtx_unlock(&g_slog_mutex);
}

void flushlog(void) { flushlog_fp(stdout); }

void flushlog_file(const char* file)
{
    FILE* fileptr = fopen(file, "a");
    if (!fileptr)
    {
        return;
    }

    flushlog_fp(fileptr);

    (void)fclose(fileptr);
}

void flushlog_fp(FILE* fileptr)
{
    if (!fileptr)
    {
        return;
    }

    (void)mtx_lock(&g_slog_mutex);

    size_t len = (size_t)(g_slogpos - g_slog);
    (void)fwrite(g_slog, 1, len, fileptr);
    (void)fflush(fileptr);

    /* reset buffer */
    g_slogpos = g_slog;
    *g_slog = '\0';

    (void)mtx_unlock(&g_slog_mutex);
}

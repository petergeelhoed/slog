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
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#ifndef SLOG_SIZE
#define SLOG_SIZE 4096
#endif

static char g_slog[SLOG_SIZE];
static char* g_slogpos = g_slog;

static mtx_t g_slog_mutex;
static once_flag g_slog_once = ONCE_FLAG_INIT;

static void slog_cleanup(void) { mtx_destroy(&g_slog_mutex); }

static void slog_init(void)
{
    if (thrd_success != mtx_init(&g_slog_mutex, mtx_plain))
    {
        abort();
    }

    if (atexit(slog_cleanup) != 0)
    {
        abort();
    }
}

static void slog_timestamp(char* buf, size_t buflen)
{
    struct timespec time_spec;
    struct tm time;

    if (timespec_get(&time_spec, TIME_UTC) != TIME_UTC)
    {
        buf[0] = '\0';
        return;
    }

    /* localtime() is NOT thread-safe → protected by slog mutex */
    struct tm* tmp = localtime(&time_spec.tv_sec);
    if (tmp == NULL)
    {
        buf[0] = '\0';
        return;
    }
    time = *tmp; /* copy immediately */

    // NOLINTBEGIN(readability-magic-numbers)
    /* YYYY-MM-DDTHH:MM:SS.mmm */
    (void)snprintf(buf,
                   buflen,
                   "%04d-%02d-%02dT%02d:%02d:%02d.%03ld",
                   time.tm_year + 1900,
                   time.tm_mon + 1,
                   time.tm_mday,
                   time.tm_hour,
                   time.tm_min,
                   time.tm_sec,
                   time_spec.tv_nsec / 1000000L);
    // NOLINTEND(readability-magic-numbers)
}

void slog(const char* fmt, ...)
{
    call_once(&g_slog_once, slog_init);
    va_list aplist;
    const int ts_buf_len = 32;
    char timestamp[ts_buf_len];

    if (thrd_success != mtx_lock(&g_slog_mutex))
    {
        return;
    }
    size_t remaining = SLOG_SIZE - (size_t)(g_slogpos - g_slog);
    if (remaining <= 1)
    {
        if (thrd_success != mtx_unlock(&g_slog_mutex))
        {
            abort();
        }
        return;
    }
    slog_timestamp(timestamp, ts_buf_len);
    int ts_written = snprintf(g_slogpos, remaining, "[%s] ", timestamp);
    if (ts_written <= 0 || (size_t)ts_written >= remaining)
    {
        if (thrd_success != mtx_unlock(&g_slog_mutex))
        {
            abort();
        }
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

    if (thrd_success != mtx_unlock(&g_slog_mutex))
    {
        abort();
    }
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

    if (thrd_success != mtx_lock(&g_slog_mutex))
    {
        return;
    }

    size_t len = (size_t)(g_slogpos - g_slog);
    (void)fwrite(g_slog, 1, len, fileptr);
    (void)fflush(fileptr);

    /* reset buffer */
    g_slogpos = g_slog;
    *g_slog = '\0';

    if (thrd_success != mtx_unlock(&g_slog_mutex))
    {
        abort();
    }
}

/**
 * @file slog.c
 * @brief Simple buffered logging implementation.
 *
 * This module provides a very small, safe logging facility that
 * accumulates formatted log messages in a fixed-size internal buffer.
 *
 * - Logging silently truncates when the buffer is full.
 * - No dynamic memory allocation is used.
 * - Thread-safe (POSIX).
 *
 * @note This file is intended to be built into a static library.
 */

#include "slog.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef SLOG_SIZE
#define SLOG_SIZE 4096
#endif

static char g_slog[SLOG_SIZE];
static char* g_slogpos = g_slog;

static pthread_mutex_t g_slog_mutex = PTHREAD_MUTEX_INITIALIZER;

void slog(const char* fmt, ...)
{
    va_list aplist;

    pthread_mutex_lock(&g_slog_mutex);

    size_t remaining = SLOG_SIZE - (size_t)(g_slogpos - g_slog);
    if (remaining <= 1)
    {
        pthread_mutex_unlock(&g_slog_mutex);
        return;
    }

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

    pthread_mutex_unlock(&g_slog_mutex);
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

    pthread_mutex_lock(&g_slog_mutex);

    size_t len = (size_t)(g_slogpos - g_slog);
    (void)fwrite(g_slog, 1, len, fileptr);
    (void)fflush(fileptr);

    /* reset buffer */
    g_slogpos = g_slog;
    *g_slog = '\0';

    pthread_mutex_unlock(&g_slog_mutex);
}

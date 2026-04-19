
#include "slog.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_drop_logging(void)
{
    const char* log_path = "test_drop_logging.log";
    slog("Test message: %d\n", 2);
    droplog();
    flushlog_file(log_path);

    FILE* file_ptr = fopen(log_path, "r");
    assert(file_ptr != NULL && "Log file should exist");
    int is_empty = fgetc(file_ptr) == EOF;
    (void)fclose(file_ptr);
    assert(is_empty && "Log file should be empty after droplog");
}

void test_basic_logging(void)
{
    slog("Test message: %d\n", 3);
    flushlog();
}

void test_flush_to_file(void)
{
    const int BUF_LEN = 128;
    const char* log_path = "test_slog.log";
    slog("File log: %s\n", "abc");
    flushlog_file(log_path);
    FILE* file_ptr = fopen(log_path, "r");
    assert(file_ptr != NULL && "Log file should exist");
    char buffer[BUF_LEN];
    int found = 0;
    while (fgets(buffer, sizeof(buffer), file_ptr) != NULL)
    {
        if (strstr(buffer, "File log: abc") != NULL)
        {
            found = 1;
            break;
        }
    }
    (void)fclose(file_ptr);
    assert(found && "Log message should be in file");
}

int main(void)
{
    test_basic_logging();
    test_flush_to_file();
    printf("All slog tests passed.\n");
    return EXIT_SUCCESS;
}

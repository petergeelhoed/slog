cmake_minimum_required(VERSION 3.10)

project(slog C)

add_executable(slog_example
    main.c
)

target_link_libraries(slog_example
    PRIVATE slog
)


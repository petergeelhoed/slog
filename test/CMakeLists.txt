cmake_minimum_required(VERSION 3.10)

project(slog C)

add_executable(test_slog test_slog.c)

target_link_libraries(test_slog PRIVATE slog)
add_test(NAME test COMMAND test_slog)

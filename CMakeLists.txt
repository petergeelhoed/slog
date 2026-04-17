cmake_minimum_required(VERSION 3.10)

project(slog C)

add_compile_definitions(_POSIX_C_SOURCE=200112L)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Doxygen REQUIRED)

include(cmake/ClangTools.cmake)

add_library(slog STATIC
    src/slog.c
)

target_include_directories(slog
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(slog PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
)



install(TARGETS slog
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)


add_custom_target(install_all
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}
    COMMENT "Installing slog"
)


add_subdirectory(example)


add_custom_target(doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating Doxygen documentation"
)


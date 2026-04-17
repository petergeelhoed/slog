find_program(CLANG_TIDY_EXE NAMES clang-tidy)
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_TIDY_EXE)
  set(CMAKE_C_CLANG_TIDY
    ${CLANG_TIDY_EXE};
    -warnings-as-errors=*;
    -p=${CMAKE_BINARY_DIR}
  )
endif()

if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_C_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/example/*.c
  )

  add_custom_target(clang-format
    COMMAND ${CLANG_FORMAT_EXE}
            -i
            ${ALL_C_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format"
  )

  add_custom_target(clang-format-check
    COMMAND ${CLANG_FORMAT_EXE}
            --dry-run
            --Werror
            ${ALL_C_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking clang-format"
  )
endif()

project(filecheck)
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${fmt_INCLUDE_DIRS})
include_directories(filecheck)

# Source Files
set(
  sources
  filecheck/checksum.h
  filecheck/checksum.cc
  filecheck/clock.h
  filecheck/clock.cc
  filecheck/index.h
  filecheck/index.cc
  filecheck/index_read.cc
  filecheck/index_write.cc
  filecheck/cmd_search.cc
  filecheck/output_progress.h
  filecheck/output_progress.cc
  filecheck/output_text.h
  filecheck/output_text.cc
  filecheck/output_tty.h
  filecheck/output_tty.cc
  filecheck/progress.h
  filecheck/update.h
  filecheck/update.cc
  filecheck/verify.h
  filecheck/verify.cc
)

set(
  test_sources
  filecheck-test/assert.h
  filecheck-test/environment.h
  filecheck-test/environment.cc
  filecheck-test/generate.h
  filecheck-test/generate.cc
)

# Linker Flags
set(
  linklibs
  OpenSSL::SSL
  fmt::fmt
)

# Compile-time warnings in debug mode
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")

# Runtime warnings in debug mode
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")

# Test Coverage
option(TEST_COVERAGE "Enable code coverage reports" OFF)
if(TEST_COVERAGE)
  find_program(GCOVR gcovr REQUIRED)
  add_compile_options(--coverage)
  add_link_options(--coverage)
endif(TEST_COVERAGE)

# cppcheck
option(TEST_CPPCHECK "Enable cppcheck reports" OFF)
if(TEST_CPPCHECK)
  find_program(CPPCHECK cppcheck REQUIRED)
  find_program(CPPCHECKHTMLREPORT cppcheck-htmlreport REQUIRED)
endif(TEST_CPPCHECK)

# Build main executable
add_executable(filecheck ${sources} filecheck/main.cc)
target_link_libraries(filecheck ${linklibs})

# Install the main executable
install(TARGETS filecheck)

# Add the test target
add_custom_target(
  test
  COMMAND ${CMAKE_COMMAND}
    -E env "TEST_COVERAGE=${TEST_COVERAGE}" "TEST_CPPCHECK=${TEST_CPPCHECK}"
    ${PROJECT_SOURCE_DIR}/filecheck-test/test.sh
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Run all tests"
)

add_dependencies(test filecheck)

# Build unit tests
file(GLOB test_paths "filecheck-test/test-*")

foreach(test_path ${test_paths})
  get_filename_component(test_name ${test_path} NAME)

  if(NOT EXISTS "${test_path}/test.cc")
    continue()
  endif()

  add_executable(
    ${test_name}
    ${test_path}/test.cc
    ${test_sources}
    ${sources}
  )

  target_link_libraries(
    ${test_name}
    ${linklibs}
  )

  add_dependencies(test "${test_name}")
endforeach()

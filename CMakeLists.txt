project(filecheck)
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${fmt_INCLUDE_DIRS})

# Source Files
set(
  sources
  checksum.h
  checksum.cc
  clock.h
  clock.cc
  index.h
  index.cc
  index_read.cc
  index_write.cc
  cmd_search.cc
  output_progress.h
  output_progress.cc
  output_text.h
  output_text.cc
  output_tty.h
  output_tty.cc
  progress.h
  update.h
  update.cc
  verify.h
  verify.cc
)

set(
  test_sources
  test/assert.h
  test/environment.h
  test/environment.cc
  test/generate.h
  test/generate.cc
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
add_executable(filecheck ${sources} main.cc)
target_link_libraries(filecheck ${linklibs})

# Install the main executable
install(TARGETS filecheck)

# Add the test target
add_custom_target(
  test
  COMMAND ${CMAKE_COMMAND}
    -E env "TEST_COVERAGE=${TEST_COVERAGE}" "TEST_CPPCHECK=${TEST_CPPCHECK}"
    ${PROJECT_SOURCE_DIR}/test/test.sh
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Run all tests"
)

add_dependencies(test filecheck)

# Build unit tests
file(GLOB test_paths "test/test-*")

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

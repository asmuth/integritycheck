project(flix)
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
  op_verify.cc
  cmd_search.cc
  output_tty.h
  output_tty.cc
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

# Build main executable
add_executable(flix ${sources} main.cc)
target_link_libraries(flix ${linklibs})

# Install the main executable
install(TARGETS flix)

# Add the test target
add_custom_target(
  test
  COMMAND ${PROJECT_SOURCE_DIR}/test/test.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run all tests"
)

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

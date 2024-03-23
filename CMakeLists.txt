cmake_minimum_required(VERSION 3.8)

find_package(OpenSSL REQUIRED)

add_executable(
  flix
  checksum.h
  checksum.cc
  cmd_search.cc
  main.cc
)

target_link_libraries(flix OpenSSL::SSL)

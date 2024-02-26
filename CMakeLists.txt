cmake_minimum_required(VERSION 3.8)

find_package(OpenSSL REQUIRED)

add_executable(
  flix
  cmd_search.cc
  main.cc
)

target_link_libraries(flix OpenSSL::SSL)

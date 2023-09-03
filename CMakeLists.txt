cmake_minimum_required(VERSION 3.8)

find_package(OpenSSL REQUIRED)

add_executable(
  index_search
  cmd_search.cc
)

target_link_libraries(index_search OpenSSL::SSL)

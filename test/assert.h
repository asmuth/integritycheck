#pragma once
#include <iostream>

#define EXPECT(cond) \
  { \
    if (!(cond)) { \
      std::cerr \
        << "EXPECT: " << #cond << std::endl \
        << "AT: " << __FILE__ << ":" << __LINE__ << std::endl; \
      exit(EXIT_FAILURE); \
    } \
  }

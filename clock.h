#pragma once
#include <cstdint>
#include <string>

uint64_t clock_monotonic();

uint64_t clock_elapsed(uint64_t at, uint64_t since);

std::string clock_isodate();

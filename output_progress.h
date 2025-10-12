#pragma once
#include "progress.h"

namespace output_progress {

void print(const Progress& progress);
void bind(ProgressFn* fn);

} // namespace output_progress

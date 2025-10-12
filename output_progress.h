#pragma once
#include "verify.h"

void output_progress(const VerifyResult& result);
void output_progress_bind(VerifyOp* op);
void output_progress_flush();

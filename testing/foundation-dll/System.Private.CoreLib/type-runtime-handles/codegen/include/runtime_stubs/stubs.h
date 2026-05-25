#pragma once

// ── Meta-include: all per-domain stub declarations ───────────────
// Included by generated_code_compat.h to replace the old monolithic
// extern "C" declaration block.

#include "stub_common.h"

// Include exception helpers before extern "C" so inline stubs
// (e.g. ChaosConvertToInt32FromDouble) can use RaiseManagedException.
#include "exception_helpers.h"

extern "C" {

#include "array_stubs.h"
#include "math_stubs.h"
#include "datetime_stubs.h"
#include "char_stubs.h"
#include "convert_stubs.h"
#include "guid_stubs.h"
#include "random_stubs.h"
#include "hashcode_stubs.h"
#include "interlocked_stubs.h"
#include "string_stubs.h"
#include "stream_stubs.h"
#include "exception_stubs.h"
#include "object_stubs.h"
#include "threading_stubs.h"
#include "collection_stubs.h"
#include "misc_stubs.h"
#include "mutex_stubs.h"
#include "enum_stubs.h"

}  // extern "C"
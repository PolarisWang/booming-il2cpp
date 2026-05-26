"""Generate supplemental dispatch symbols for families without MethodN pattern.

Handwritten entrypoint families (e.g. interface-dispatch) don't have
MethodN forward declarations, so patch_bypass can't generate the dispatch
table loop. This script creates a supplemental .cpp with RunNativeAotAll
and empty/sentinel patchdata symbols.

Usage:
    python _gen_supplemental_dispatch.py <generated-dir> <method-count>
"""

from __future__ import annotations

import sys
from pathlib import Path


def generate_supplemental(genuine_out: Path, method_count: int) -> bool:
    """Generate supplemental dispatch symbols."""
    supplemental = """// Auto-generated supplemental dispatch symbols (handwritten entrypoint)
// Provides RunNativeAotAll and sentinel patchdata symbols.
// Uses setjmp/longjmp recovery (references variables injected by patch_bypass
// in native-aot.generated.cpp) so CHAOS_IL2CPP_FAIL (longjmp bounce-back)
// doesn't crash the process.

#include <cstddef>
#include <cstdint>
#include <csetjmp>

// thread_local variables defined in native-aot.generated.cpp (injected by patch_bypass)
extern thread_local std::jmp_buf g_chaos_abort_jmp;
extern thread_local int g_chaos_abort_flag;

// ── Dispatch table ─────────────────────────────────────────────
// Note: for handwritten entrypoints, all methods are dispatched
// through a single RunNativeAot(entry_index) call.
// RunNativeAotAll simply calls RunNativeAot(i) for each method.
extern "C" std::int32_t RunNativeAot(std::int32_t chaos_entry_index);

extern "C" std::int32_t RunNativeAotAll()
{
    std::int32_t failed_count = 0;
    for (int i = 0; i < %d; i++)
    {
        g_chaos_abort_flag = 0;
        if (std::setjmp(g_chaos_abort_jmp) == 0)
        {
            std::int32_t r = RunNativeAot(i);
            if (r) ++failed_count;
        }
        if (g_chaos_abort_flag)
        {
            ++failed_count;  // Mark this method as failed
        }
    }
    return failed_count;
}

// ── Sentinel patchdata (empty — hotpatch disabled) ─────────────
static const char* const kPatchDataHostClassName = "";
static const uint8_t kPatchData[] = { 0x00 };
static constexpr size_t kPatchDataSize = 0u;

// ── Method count for runtime-entry.cpp ────────────────────────
extern "C" const int kAotMethodCount = %d;
""" % (method_count, method_count)
    out_path = genuine_out / "supplemental-dispatch.cpp"
    out_path.write_text(supplemental, encoding="utf-8")
    print(f"  [gen_supplemental] wrote {method_count}-method dispatch to {out_path}")
    return True


def main():
    if len(sys.argv) < 3:
        print("Usage: python _gen_supplemental_dispatch.py <genuine-out-dir> <method-count>")
        sys.exit(1)

    genuine_out = Path(sys.argv[1])
    method_count = int(sys.argv[2])

    if not genuine_out.exists():
        print(f"Directory not found: {genuine_out}")
        sys.exit(1)

    success = generate_supplemental(genuine_out, method_count)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
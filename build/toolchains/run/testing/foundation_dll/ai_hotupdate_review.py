"""AI HotUpdate Review — thin wrapper around ai_codegen_reviewer.py with --type hotupdate preset.

Reviews generated patch C++ code against C# IL dumps to verify:
  - Patch function correctness (sentinel values, fallthrough logic)
  - Signature alignment between C# and C++
  - Metadata side effect safety

Usage:
    python ai_hotupdate_review.py <family-slug> --csil <cs-il-dump> --cpp <generated-cpp>

Output:
    - Writes review verdict to review/<family>/hotupdate-review-v<N>.md
    - Exit code 0 (pass), 1 (warn), 2 (fail)
"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))  # for testing.trace

from ai_codegen_reviewer import perform_review


def main() -> None:
    parser = argparse.ArgumentParser(description="AI HotUpdate Review for C#/C++ alignment")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--csil", required=True, help="Path to C# IL dump file")
    parser.add_argument("--cpp", required=True, help="Path to generated C++ patch file")
    parser.add_argument("--trace", action="store_true", help="Enable JSONL trace logging")
    args = parser.parse_args()

    trace_enabled = args.trace
    if trace_enabled:
        try:
            from testing.trace import trace as _trace, trace_init
            trace_init(_REPO_ROOT, stage="hotupdate-review")
            print("[trace] JSONL trace enabled")
        except ImportError:
            trace_enabled = False

    csil_path = Path(args.csil)
    cpp_path = Path(args.cpp)

    if not csil_path.exists():
        print(f"ERROR: C# IL file not found: {csil_path}", file=sys.stderr)
        sys.exit(2)
    if not cpp_path.exists():
        print(f"ERROR: C++ file not found: {cpp_path}", file=sys.stderr)
        sys.exit(2)

    csil_content = csil_path.read_text(encoding="utf-8")
    cpp_content = cpp_path.read_text(encoding="utf-8")

    t0 = time.time()
    result = perform_review(args.family_slug, csil_content, cpp_content, review_type="hotupdate")
    elapsed = time.time() - t0

    if trace_enabled:
        _trace("hotupdate_review", stage="hotupdate-review", family=args.family_slug,
               verdict=result["verdict"], issues=len(result["issues"]),
               dur_ms=round(elapsed * 1000, 1))

    print(f"Review written to: {result['review_path']}")
    print(f"Verdict: {result['verdict']}")
    print(f"Issues: {len(result['issues'])}")
    for issue in result["issues"]:
        print(f"  - {issue}")

    exit_code = {"PASS": 0, "WARN": 1, "FAIL": 2}
    sys.exit(exit_code.get(result["verdict"], 2))


if __name__ == "__main__":
    main()

"""Self-test for 3 cross-platform workaround fixes."""
import sys
import json
import re

sys.path.insert(0, "testing/foundation-dll/verification")

passed = 0
failed = 0

def check(name, condition, detail=""):
    global passed, failed
    if condition:
        passed += 1
        print(f"  PASS: {name}")
    else:
        failed += 1
        print(f"  FAIL: {name} {detail}")


# ── Test #11: DOTNET_ROOT auto-detection (build.py) ──
print("[#11] DOTNET_ROOT auto-detection")

with open("tests/e2e/verification/stages/build.py", encoding="utf-8") as f:
    content = f.read()

check("no hardcoded C:/ path", "C:/Program Files" not in content)
check("dotnet_root=None guard present", "if dotnet_root:" in content)
check("Base Path parser exists", "Base Path" in content)

# Verify the old default is gone
check("no 'C:/Program Files/dotnet/shared' default",
      'C:/Program Files/dotnet/shared' not in content)


# ── Test #14: JSON extraction with brace-counting ──
print("[#14] JSON extraction with brace-counting")
from reporting.dashboard import _extract_json_fragment, _safe_parse_json_fragment, STAGE_KEYS

# Deeply nested
text = '{"stage_benchmark": {"status": "passed", "details": {"ops": 100, "samples": [1,2,3]}}}'
p = _safe_parse_json_fragment(text, "stage_benchmark")
check("deeply nested JSON", p and p["status"] == "passed" and p["details"]["ops"] == 100)

# Escaped quotes
text = '{"status": {"message": "stage \\"passed\\" OK", "code": 0}}'
p = _safe_parse_json_fragment(text, "status")
check("escaped quotes in strings", p and p["code"] == 0)

# All STAGE_KEYS
for sk in STAGE_KEYS:
    text = f'{{"{sk}": {{"status": "passed", "duration": 1.5}}}}'
    p = _safe_parse_json_fragment(text, sk)
    check(f"stage key: {sk}", p and p["status"] == "passed")

# Real-world coverage
text = '... "coverage": {"stagesPassed": 8, "stagesTotal": 10, "stagesFailed": 2, "stagesSkipped": 0, "stagePassRate": 0.8}, "overall_status": "partial"'
p = _safe_parse_json_fragment(text, "coverage")
check("real-world coverage extraction", p and p["stagesTotal"] == 10)

# Truncated JSON
text = '{"coverage": {"stagesPassed": 3, "stagesTotal":'
f = _extract_json_fragment(text, "coverage")
check("truncated returns partial content", f is not None)


# ── Test #15: CodeSizeTracker parsers ──
print("[#15] Cross-platform section size parsers")
from analysis.code_size_tracker import CodeSizeTracker

dumpbin_out = """  .text   size = 0x1a2f4
  .data   size = 0x2000
  .pdata  size = 0x300"""

sizes = CodeSizeTracker._parse_dumpbin_output(dumpbin_out)
check("dumpbin parser: text", sizes.get("text") == 0x1a2f4)
check("dumpbin parser: data", sizes.get("data") == 0x2000)
check("dumpbin parser: pdata", sizes.get("pdata") == 0x300)

objdump_out = """Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         0001a2f4  00401000  00401000  00000400  2**4
  1 .data         00002000  0041b400  0041b400  0001b600  2**4
  2 .pdata        00000300  0041d400  0041d400  0001d600  2**2"""

sizes = CodeSizeTracker._parse_objdump_output(objdump_out)
check("objdump parser: text", sizes.get("text") == 0x1a2f4)
check("objdump parser: data", sizes.get("data") == 0x2000)
check("objdump parser: pdata", sizes.get("pdata") == 0x300)

readelf_out = """  [Nr] Name              Type            Address          Off    Size
  [ 0]                   NULL            0000000000000000 000000 000000
  [ 1] .text             PROGBITS        0000000000401000 001000 01a2f4
  [ 2] .data             PROGBITS        000000000041b400 01b600 002000
  [ 3] .pdata            PROGBITS        000000000041d400 01d600 000300"""

sizes = CodeSizeTracker._parse_readelf_output(readelf_out)
check("readelf parser: text", sizes.get("text") == 0x1a2f4)
check("readelf parser: data", sizes.get("data") == 0x2000)
check("readelf parser: pdata", sizes.get("pdata") == 0x300)

# Consistency check
all_same = (
    CodeSizeTracker._parse_dumpbin_output(dumpbin_out)["text"]
    == CodeSizeTracker._parse_objdump_output(objdump_out)["text"]
    == CodeSizeTracker._parse_readelf_output(readelf_out)["text"]
)
check("all 3 parsers produce same text size", all_same)


# ── Summary ──
print(f"\n=== {passed} passed, {failed} failed ===")
sys.exit(0 if failed == 0 else 1)

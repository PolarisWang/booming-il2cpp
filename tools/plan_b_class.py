#!/usr/bin/env python3
"""Plan the B-class (31 remaining Utf8JsonWriter factoryGap) workload.

The 31 subject ids are taken from the post-C-fix worktree run (verified: the 12
C-class subjects were confirmed gone, leaving exactly these 31).

For each subject we resolve:
  - the real argument list from the ATG subject source (CombinedSubjects.cs)
  - whether a native export with a compatible signature already exists
  - whether a shape registration with that exact key already exists

Read-only planning aid.
"""
import collections
import re

CS = "artifacts/foundation-dll/System.Text.Json/chunks/text-json/managed/combined/CombinedSubjects.cs"
S21 = "src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S21.cs"
STUBS_H = "src/native/runtime-core/runtime_stubs/json_writer_stubs.h"

# Authoritative remaining set (post-C-fix), from the worktree verification run.
REMAINING = [
    "WriteStartArray_8_JsonEncodedText_0", "WriteStartObject_9_JsonEncodedText_0",
    "WriteStartArray_10_string_0", "WriteStartObject_11_string_0",
    "WriteString_14_JsonEncodedText_System_DateTime_0",
    "WriteString_15_string_System_DateTime_0",
    "WriteString_16_JsonEncodedText_System_DateTimeOffset_0",
    "WriteString_17_string_System_DateTimeOffset_0",
    "WriteNumber_18_JsonEncodedText_decimal_0", "WriteNumber_19_string_decimal_0",
    "WriteNumber_20_JsonEncodedText_double_0", "WriteNumber_22_JsonEncodedText_float_0",
    "WriteString_24_JsonEncodedText_System_Guid_0", "WriteString_25_string_System_Guid_0",
    "WriteNull_26_JsonEncodedText_0", "WriteBoolean_28_JsonEncodedText_bool_0",
    "WriteNumber_30_JsonEncodedText_long_0", "WriteNumber_32_JsonEncodedText_int_0",
    "WriteNumber_33_string_int_0", "WritePropertyName_34_JsonEncodedText_0",
    "WriteString_36_JsonEncodedText_JsonEncodedText_0", "WriteString_37_string_JsonEncodedText_0",
    "WriteString_39_JsonEncodedText_string_0", "WriteNumber_40_JsonEncodedText_ulong_0",
    "WriteNumber_42_JsonEncodedText_uint_0", "WriteNumber_43_string_uint_0",
    "WriteRawValue_53_string_bool_0", "WriteRawValue_54_ReadOnlySequencebyte_bool_0",
    "WriteNumberValue_55_int_0", "WriteStringValue_57_JsonEncodedText_0",
    "WriteNumberValue_59_uint_0",
]

cs = open(CS, encoding="utf-8", errors="replace").read()
s21 = open(S21, encoding="utf-8", errors="replace").read()
stubs = open(STUBS_H, encoding="utf-8", errors="replace").read()

subject_re = re.compile(r'public long (\w+)\(\)\s*\n\s*\{(.*?)\n\s*\}', re.S)
by_subject = {m.group(1): m.group(2) for m in subject_re.finditer(cs)}

call_re = re.compile(r'Create<System\.Text\.Json\.Utf8JsonWriter>\(\)\.(\w+)\(([^;]*?)\)\s*;')


def classify(arg):
    a = arg.strip()
    m = re.match(r'default\(([^)]+)\)', a)
    if m:
        return m.group(1).strip()
    if a.startswith('"'):
        return "System.String"
    if a in ("true", "false"):
        return "System.Boolean"
    if re.fullmatch(r'-?\d+', a):
        return "Int32"
    return "?:" + a[:20]


# Existing registered shape keys in S21: (method, (paramTypes...))
reg_keys = set()
for m in re.finditer(r'RegisterUtf8WriterVoid\(\s*registry,\s*"(\w+)",\s*"[^"]*",\s*(.*?),\s*(.*?),\s*(.*?)\);',
                     s21, re.S):
    method = m.group(1)
    pts = re.findall(r'"(?:System\.[\w.]+|JsonEncodedText)"', m.group(4))
    pts = [p.strip('"') for p in pts]
    reg_keys.add((method, tuple(pts)))

# Existing native exports
native = set(re.findall(r'void (ChaosUtf8JsonWriter\w*)\(', stubs))

print(f"B-class subjects: {len(REMAINING)}")
rows = []
for gid in REMAINING:
    body = by_subject.get(gid, "")
    cm = call_re.search(body)
    if not cm:
        rows.append((gid, "?", "?", "PARSE-FAIL"))
        continue
    method, arglist = cm.group(1), cm.group(2).strip()
    types = tuple(classify(a) for a in arglist.split(",")) if arglist else ()
    shape = (method, types)
    registered = shape in reg_keys
    rows.append((gid, method, "(" + ", ".join(types) + ")", "registered" if registered else "MISSING"))

shapes = collections.defaultdict(list)
for gid, method, ts, st in rows:
    if st == "PARSE-FAIL":
        continue
    shapes[(method, ts)].append(st)

print(f"distinct shape keys: {len(shapes)}")
print()
print(f"{'method':20s} {'paramTypes':58s} {'status':10s} n")
print("-" * 100)
for (method, ts), sts in sorted(shapes.items()):
    print(f"{method:20s} {ts:58s} {sts[0]:10s} {len(sts)}")

parse_fails = [r for r in rows if r[3] == "PARSE-FAIL"]
if parse_fails:
    print()
    print("PARSE FAILURES (need manual inspection):")
    for gid, *_ in parse_fails:
        print("  ", gid, "->", by_subject.get(gid, "NO BODY")[:110])

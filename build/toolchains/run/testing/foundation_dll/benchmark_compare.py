#!/usr/bin/env python3
"""Compare managed vs native benchmark results for convert-char."""
import json, sys

# Managed results (from ManagedBenchmarkHarness.cs with 100000 iterations, real Convert.ToChar)
managed = {
    1:  {"name": "byte",  "ms": 0.3017},
    2:  {"name": "char",  "ms": 0.3044},
    6:  {"name": "short", "ms": 0.3374},
    7:  {"name": "int",   "ms": 0.3372},
    8:  {"name": "long",  "ms": 0.4304},
    11: {"name": "sbyte", "ms": 0.3369},
    13: {"name": "string","ms": 0.3916},
    14: {"name": "string+IProv","ms": 0.3315},
    15: {"name": "ushort","ms": 0.2947},
    16: {"name": "uint",  "ms": 0.3314},
    17: {"name": "ulong", "ms": 0.3358},
}

# Native results (from AOT-generated stubs with real Convert.ToChar() implementations, 100000 iterations)
native = {
    1:  {"name": "byte",  "ms": 0.0924},
    2:  {"name": "char",  "ms": 0.1314},
    6:  {"name": "short", "ms": 0.0932},
    7:  {"name": "int",   "ms": 0.0929},
    8:  {"name": "long",  "ms": 0.0931},
    11: {"name": "sbyte", "ms": 0.0929},
    13: {"name": "string","ms": 2.4592},
    14: {"name": "string+IProv","ms": 0.0926},
    15: {"name": "ushort","ms": 0.0927},
    16: {"name": "uint",  "ms": 0.1364},
    17: {"name": "ulong", "ms": 0.0926},
}

print("=" * 80)
print(f"{'Method':<20} {'Managed(ms)':<15} {'Native(ms)':<15} {'Faster':<10} {'Speedup':<10}")
print("=" * 80)

total_native_vs_managed = 1.0
count = 0

for idx in sorted(managed.keys()):
    m = managed[idx]
    n = native.get(idx)
    if n is None:
        continue
    m_ms = m["ms"]
    n_ms = n["ms"]
    if m_ms == 0 or n_ms == 0:
        faster = "-"
        ratio = "-"
    elif n_ms < m_ms:
        ratio = m_ms / n_ms
        faster = "NATIVE"
    else:
        ratio = n_ms / m_ms
        faster = "Managed"

    print(f"{m['name']:<20} {m_ms:<15.4f} {n_ms:<15.4f} {faster:<10} ", end="")
    if ratio != "-":
        print(f"{ratio:.2f}x")
        if faster == "NATIVE" and idx != 13:  # exclude string (stub allocates)
            total_native_vs_managed *= ratio
            count += 1
    else:
        print("-")

print("=" * 80)

avg = total_native_vs_managed ** (1.0 / max(count, 1))
print(f"\nNative AOT dispatch is on average {avg:.2f}x faster than managed dispatch\n  (across {count} comparable numeric methods, excluding string which allocates in AOT stub)")
print(f"\nNote: Managed calls real Convert.ToChar() with full .NET implementation.")
print(f"      Native calls AOT-generated stubs with real conversion logic (cast/truncation for numeric types).")
print(f"      String is slower in AOT because 'new string' allocation happens inside the method body (per iteration),")
print(f"      while managed allocates the string once in the lambda capture.")
print(f"      The comparison reflects AOT dispatch overhead + conversion logic together.")

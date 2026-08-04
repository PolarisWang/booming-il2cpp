# Gap Analysis Report

Generated: 2026-06-08 (Updated)

## Overview

Benchmark comparison of 22 chunks across chaos-aot (IL2CPP AOT), NET8 JIT, and NET10 JIT.
Overall average: **3.19x** (AOT vs NET8 JIT, where >1.0 means AOT is faster).

## Top 3 Optimization Targets

| Rank | Chunk | Raw Gap | % Slower | Fact Fail | Per-method Slowdown | Missing JIT |
|------|-------|---------|----------|-----------|---------------------|-------------|
| **1** | runtime-interop | +198M | +201% | **102 (worst)** | ~300% | 590/610 |
| **2** | text | +144M | +148% | 16 | N/A | **1,777 (92%)** |
| **3** | globalization | **+222M** | **+283%** | 12 | **2,031%** | 798 |

## Detail: runtime-interop (#1)

- 102 fact failures — worst of any chunk
- +198M units behind NET8 (+201%)
- AOT is ~4x slower on per-method benchmarks
- 590/610 methods lack a JIT baseline
- Key types: ArrayWithOffset, CollectionsMarshal, ComEventsHelper, ComWrappers, GCHandle, Marshal, SafeHandle

## Detail: text (#2)

- 1,777/1,937 methods (92%) have AOT data but zero NET8 baseline
- 16 fact failures, 148 net8_errors
- Largest chunk by method count (2,882 in benchmark)
- Key types: System.Text (Encoding, StringBuilder, ValueStringBuilder), System.Runtime.Serialization

## Detail: globalization (#3)

- +222M raw gap — largest absolute gap (+283%)
- AOT is ~21x slower per-method
- Recent CharUnicodeInfo stub additions (char_stubs.cpp) may reduce gap
- Key types: System.Globalization (CharUnicodeInfo, CultureInfo, CompareInfo, TextInfo)

## Foundation-DLL Verification Status

| Family | Chunks | Fact Result | Notes |
|--------|--------|-------------|-------|
| System.Collections | 1 | 16/16 ✅ | All pass |
| System.Collections.Immutable | 1 | 22/32 ⚠️ | 10 FrozenDictionary/FrozenSet failures - external runtime stubs |
| System.ComponentModel.TypeConverter | 3 | 74/74 ✅ | All pass |
| System.Data.Common | 4 | 21/21 ✅ | All pass |
| System.Diagnostics.DiagnosticSource | 2 | 172/172 ✅ | All pass |
| System.Formats.Asn1 | 1 | ❌ | entry.exe not found |
| System.IO.Compression.Brotli | 1 | 33/34 ⚠️ | 1 failure |
| System.IO.Pipelines | 1 | ❌ | entry.exe not found |
| System.Linq | 1 | 4/4 ✅ | All pass |
| System.Linq.Expressions | 1 | 737/737 ✅ | All pass |
| System.Net.Http | 2 | 109/111 ⚠️ | 2 failures |
| System.Net.ServerSentEvents | 1 | 2/6 ⚠️ | 4 failures |
| System.Net.Sockets | 1 | ❌ | entry.exe not found |
| System.ObjectModel | - | - | No subjects |
| System.Private.CoreLib | 21 | ❌ | entry.exe not found (different chunk structure) |
| System.Private.Xml | 4 | ❌ | entry.exe not found |
| System.Reflection.Metadata | 3 | 41/43 ⚠️ | 2 failures |
| System.Runtime | - | - | No subjects (forwarder assembly) |
| System.Runtime.InteropServices | 1 | 29/31 ⚠️ | 2 failures |
| System.Runtime.Intrinsics | - | - | No subjects |
| System.Runtime.Serialization.Formatters | 1 | 32/41 ⚠️ | 9 failures |
| System.Security.Claims | 1 | 35/35 ✅ | All pass |
| System.Security.Cryptography | 3 | ❌ | entry.exe not found |
| System.Security.Principal.Windows | 1 | 30/30 ✅ | All pass |
| System.Text.Json | 3 | 33/35 ⚠️ | 2 failures |
| System.Threading.Tasks.Parallel | 1 | 112/112 ✅ | All pass |
| System.Xml.ReaderWriter | - | - | No subjects |

## All Chunks Status

| Chunk | AOT/NET8 | Fact Pass | Notes |
|-------|----------|-----------|-------|
| runtime-intrinsics | 2.25x | — | ABI-blocked |
| text | 2.48x | 104/120 (87%) | 92% no JIT baseline |
| runtime-compiler | 2.51x (vs NET10) | — | — |
| collections | 2.71x | ✅ | Recently fixed |
| buffers | 2.77x | 30/36 (83%) | — |
| runtime-interop | 3.01x | 363/465 (78%) | Worst fact pass rate |
| threading | — | — | No benchmark data |
| diagnostics | — | — | No benchmark data |

## Next Optimization Target: runtime-interop

Highest ROI: most fact failures + largest performance gap + heaviest production usage.
Stub strategy: add COM/Interop stubs for Marshal, SafeHandle, GCHandle, ComWrappers.

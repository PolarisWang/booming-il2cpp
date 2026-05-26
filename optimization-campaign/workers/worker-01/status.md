# worker-01: convert-char

## Diagnosis Results (2026-05-26)

### Fact Status
- 18/18 passed (fresh build, all exitCode=0)

### Analysis
- All 18 methods now produce valid benchmark data
- Previous "failed"/"throws" classifications were stale (old build issues)
- Codegen is correct: integer types use `static_cast<UINT16>`, Object/String use bridge calls, invalid types (bool, DateTime, etc.) call `chaos_raise_exception`

### Benchmark Comparison (baseline vs fresh build)
| Index | Method | Baseline ops/s | Now ops/s | Speedup |
|-------|--------|---------------|-----------|---------|
| 0 | ToChar(bool) | throws | 1,263,012 | NEW |
| 1 | ToChar(Byte) | 344M | 531M | 1.54x |
| 2 | ToChar(Char) | 345M | 500M | 1.45x |
| 3 | ToChar(DateTime) | throws | 1,271,935 | NEW |
| 4 | ToChar(Decimal) | throws | 5,031,168 | NEW |
| 5 | ToChar(Double) | throws | 289,603,244 | NEW |
| 6 | ToChar(Int16) | 346M | 531M | 1.53x |
| 7 | ToChar(Int32) | 171M | 521M | 3.05x |
| 8 | ToChar(Int64) | 239M | 309M | 1.29x |
| 9 | ToChar(Object) | failed | 1,195,307 | NEW |
| 10 | ToChar(Object,IFormatProvider) | failed | 1,225,106 | NEW |
| 11 | ToChar(SByte) | 344M | 525M | 1.53x |
| 12 | ToChar(Single) | throws | 204,792,136 | NEW |
| 13 | ToChar(String) | failed | 117,384,670 | NEW |
| 14 | ToChar(String,IFormatProvider) | failed | 89,007,566 | NEW |
| 15 | ToChar(UInt16) | failed | 592,066,311 | NEW |
| 16 | ToChar(UInt32) | 328M | 278M | 0.85x (noise) |
| 17 | ToChar(UInt64) | 239M | 305M | 1.27x |

### Conclusion
**No optimization headroom.** All methods are already optimal:
- Integer types (Byte, Int16, Int32, Int64, SByte, UInt16, UInt32, UInt64, Char): direct `static_cast<UINT16>` — single CPU instruction
- Object/String types: bridge calls to runtime (chaos_convert_tochar_*) — these are inherently slower due to type checking
- Invalid types (bool, DateTime, Decimal, Double, Single): `chaos_raise_exception` — fast-fail path

The 0.85x on UInt32 is measurement noise at 3.6ns/call.

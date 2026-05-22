// Auto-generated managed benchmark harness
// Family: primitive-numeric-conversions, Assembly: System.Private.CoreLib
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_0(int i)
{
    Convert.ToBoolean((i % 2 == 0) ? "true" : "false");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_1(int i)
{
    Convert.ToByte("123");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_2(int i)
{
    Convert.ToInt16("12345");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_3(int i)
{
    Convert.ToInt32("1234567");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_4(int i)
{
    Convert.ToInt64("12345678901");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_5(int i)
{
    Convert.ToSingle("3.14");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_6(int i)
{
    Convert.ToDouble("3.14159");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_7(int i)
{
    Convert.ToDecimal("123.45");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_8(int i)
{
    Convert.ToString((i + 8));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_9(int i)
{
    Convert.ToString((double)((i + 9) & 0xFF));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_10(int i)
{
    Convert.ToDecimal((double)((i + 10) & 0xFF));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_11(int i)
{
    Convert.ToInt32((double)((i + 11) & 0xFF));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_12(int i)
{
    Int32.Parse((((i + 12)) % 100000 + 1).ToString());
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_13(int i)
{
    Int64.Parse((((i + 13)) % 100000 + 1).ToString());
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_14(int i)
{
    Double.Parse("3.14159");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_3(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_3(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_4(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_4(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_5(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_5(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_6(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_6(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_7(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_7(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_8(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_8(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_9(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_9(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_10(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_10(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_11(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_11(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_12(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_12(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_13(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_13(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_14(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_14(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Private.CoreLib/System.Double::Parse:System.Double(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}

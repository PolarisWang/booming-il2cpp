// Auto-generated managed benchmark harness
// Family: enum-parsing, Assembly: System.Private.CoreLib
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
    System.Enum.Format(typeof(System.StringComparison), System.StringComparison.Ordinal, "G");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_1(int i)
{
    System.Enum.GetName(typeof(System.StringComparison), System.StringComparison.Ordinal);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_2(int i)
{
    System.Enum.GetName(typeof(System.StringComparison), System.StringComparison.Ordinal);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_3(int i)
{
    _ = System.Enum.GetNames(typeof(System.StringComparison));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_4(int i)
{
    _ = System.Enum.GetValues(typeof(System.StringComparison));
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_5(int i)
{
    System.Enum.IsDefined(typeof(System.StringComparison), System.StringComparison.Ordinal);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_6(int i)
{
    System.Enum.Parse(typeof(System.StringComparison), "Ordinal");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_7(int i)
{
    System.Enum.Parse(typeof(System.StringComparison), "Ordinal", true);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_8(int i)
{
    System.StringComparison.Ordinal.ToString();
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_9(int i)
{
    System.StringComparison.Ordinal.ToString("G");
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_10(int i)
{
    System.Enum.TryParse(typeof(System.StringComparison), "Ordinal", true, out _);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_11(int i)
{
    System.Enum.TryParse(typeof(System.StringComparison), "Ordinal", out _);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::ToString:System.String()",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::ToString:System.String(System.String)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
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
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [12] System.Private.CoreLib/System.Enum::BoxToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Private.CoreLib/System.Enum::BoxToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}

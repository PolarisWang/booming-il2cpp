// Auto-generated managed benchmark harness
// Family: convert-char, Assembly: System.Private.CoreLib
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static long accum;  // static accumulator prevents dead-code elimination

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
static void H_0(int i)
{
    try { Convert.ToChar(((i) & 1) == 0); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_1(int i)
{
    return Convert.ToChar((byte)(((i + 1)) & 0xFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_2(int i)
{
    return Convert.ToChar((char)(((i + 2)) & 0xFFFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_3(int i)
{
    try { Convert.ToChar(DateTime.UtcNow); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_4(int i)
{
    try { Convert.ToChar((decimal)(((i + 4)) & 0xFF)); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_5(int i)
{
    try { Convert.ToChar((double)(((i + 5)) & 0xFF)); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_6(int i)
{
    return Convert.ToChar((short)(((i + 6)) & 0x7FFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_7(int i)
{
    return Convert.ToChar(((i + 7)) & 0x7FFF);
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_8(int i)
{
    return Convert.ToChar((long)(((i + 8)) & 0x7FFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_9(int i)
{
    return Convert.ToChar((object)(((i + 9)) & 0xFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_10(int i)
{
    return Convert.ToChar((object)(((i + 10)) & 0xFF), null);
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_11(int i)
{
    return Convert.ToChar((sbyte)(((i + 11)) & 0x7F));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_12(int i)
{
    try { Convert.ToChar((float)(((i + 12)) & 0xFF)); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_13(int i)
{
    try { Convert.ToChar("hello"); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static void H_14(int i)
{
    try { Convert.ToChar("hello", null); } catch { }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_15(int i)
{
    return Convert.ToChar((ushort)(((i + 15)) & 0xFFFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_16(int i)
{
    return Convert.ToChar((uint)(((i + 16)) & 0x7FFF));
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static char H_17(int i)
{
    return Convert.ToChar((ulong)(((i + 17)) & 0x7FFF));
}

    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_0(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_1(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_2(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_3(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_4(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_5(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_6(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_7(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_8(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_9(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_10(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_11(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_12(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_13(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    H_14(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = true,
                });
            }
            { // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_15(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_16(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
            { // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
                var sw = System.Diagnostics.Stopwatch.StartNew();
                for (int i = 0; i < 10000; i++) {
                    accum ^= (long)H_17(i);
                }
                sw.Stop();
                double ms = sw.Elapsed.TotalMilliseconds;
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                    ElapsedMilliseconds = ms,
                    Iterations = 10000,
                    IsBodyReal = true,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { accumulation = accum, results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}

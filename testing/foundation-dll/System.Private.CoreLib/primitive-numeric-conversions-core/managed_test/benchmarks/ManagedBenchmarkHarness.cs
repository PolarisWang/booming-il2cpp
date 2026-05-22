// Auto-generated managed benchmark harness
// Family: primitive-numeric-conversions-core, Assembly: System.Private.CoreLib
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



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()",
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

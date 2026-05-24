// Auto-generated managed benchmark harness
// Family: interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core, Assembly: System.Private.CoreLib
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
            { // [0] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::Run:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::Run:System.Int32(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestSingleImplSimple:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestSingleImplSimple:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestSingleImplDefault:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestSingleImplDefault:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestMultiImplCalc:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestMultiImplCalc:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIsCheck:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIsCheck:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestAsCheck:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestAsCheck:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondBase:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondBase:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondDerived:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondDerived:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondMulti:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDiamondMulti:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetCurrentProcess:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetCurrentProcess:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetCurrentThread:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetCurrentThread:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetProcessId:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetProcessId:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetModuleHandle:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestGetModuleHandle:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry/InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()",
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

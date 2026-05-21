// Auto-generated managed benchmark harness
// Family: options-errors, Assembly: System.Formats.Asn1
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
            { // [0] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String,System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String,System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_SkipSetSortOrderVerification:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_SkipSetSortOrderVerification:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_UtcTimeTwoDigitYearMax:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_UtcTimeTwoDigitYearMax:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32)",
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

// Auto-generated managed benchmark harness
// Family: time-date-time-timespan, Assembly: System.Private.CoreLib
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
            { // [0] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Private.CoreLib/System.DateTime::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)",
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

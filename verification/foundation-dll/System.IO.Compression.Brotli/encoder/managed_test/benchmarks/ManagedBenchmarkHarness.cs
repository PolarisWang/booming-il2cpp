// Auto-generated managed benchmark harness
// Family: encoder, Assembly: System.IO.Compression.Brotli
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
            { // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Compress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Compress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Flush:System.Buffers.OperationStatus(System.Span{System.Byte},System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Flush:System.Buffers.OperationStatus(System.Span{System.Byte},System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::GetMaxCompressedLength:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::GetMaxCompressedLength:System.Int32(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::.ctor:System.Void(System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::.ctor:System.Void(System.Int32,System.Int32)",
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

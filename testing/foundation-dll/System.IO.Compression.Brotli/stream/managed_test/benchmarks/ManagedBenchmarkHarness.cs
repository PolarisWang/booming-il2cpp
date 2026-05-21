// Auto-generated managed benchmark harness
// Family: stream, Assembly: System.IO.Compression.Brotli
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
            { // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginRead:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginRead:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginWrite:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginWrite:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::DisposeAsync:System.Threading.Tasks.ValueTask() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::DisposeAsync:System.Threading.Tasks.ValueTask()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndRead:System.Int32(System.IAsyncResult) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndRead:System.Int32(System.IAsyncResult)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndWrite:System.Void(System.IAsyncResult) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndWrite:System.Void(System.IAsyncResult)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Flush:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Flush:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_BaseStream:System.IO.Stream() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_BaseStream:System.IO.Stream()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanRead:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanRead:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanSeek:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanSeek:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanWrite:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanWrite:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Length:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Length:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Position:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Position:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Byte{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Byte{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Span{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Span{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.Task{System.Int32}(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.Task{System.Int32}(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.ValueTask{System.Int32}(System.Memory{System.Byte},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.ValueTask{System.Int32}(System.Memory{System.Byte},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadByte:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadByte:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Position:System.Void(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Position:System.Void(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::SetLength:System.Void(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::SetLength:System.Void(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.Byte{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.Byte{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.Task(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.Task(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteByte:System.Void(System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteByte:System.Void(System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.BrotliCompressionOptions,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.BrotliCompressionOptions,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode,System.Boolean)",
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

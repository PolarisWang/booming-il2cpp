// Auto-generated managed benchmark harness
// Family: reader-writer, Assembly: System.IO.Pipelines
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
            { // [0] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.IO.Stream,System.IO.Pipelines.StreamPipeReaderOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.IO.Stream,System.IO.Pipelines.StreamPipeReaderOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.Buffers.ReadOnlySequence{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.Buffers.ReadOnlySequence{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Create:System.IO.Pipelines.PipeWriter(System.IO.Stream,System.IO.Pipelines.StreamPipeWriterOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Create:System.IO.Pipelines.PipeWriter(System.IO.Stream,System.IO.Pipelines.StreamPipeWriterOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)",
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

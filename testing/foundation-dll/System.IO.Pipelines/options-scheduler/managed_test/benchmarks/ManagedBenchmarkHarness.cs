// Auto-generated managed benchmark harness
// Family: options-scheduler, Assembly: System.IO.Pipelines
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
            { // [0] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Default:System.IO.Pipelines.PipeOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Default:System.IO.Pipelines.PipeOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean)",
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

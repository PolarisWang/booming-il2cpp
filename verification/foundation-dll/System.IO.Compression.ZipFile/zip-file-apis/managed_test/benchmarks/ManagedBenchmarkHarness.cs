// Auto-generated managed benchmark harness
// Family: zip-file-apis, Assembly: System.IO.Compression.ZipFile
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
            { // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.Threading.CancellationToken)",
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

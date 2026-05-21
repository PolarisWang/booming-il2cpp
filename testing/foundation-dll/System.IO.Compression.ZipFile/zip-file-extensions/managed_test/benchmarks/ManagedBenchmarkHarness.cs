// Auto-generated managed benchmark harness
// Family: zip-file-extensions, Assembly: System.IO.Compression.ZipFile
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
            { // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken)",
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

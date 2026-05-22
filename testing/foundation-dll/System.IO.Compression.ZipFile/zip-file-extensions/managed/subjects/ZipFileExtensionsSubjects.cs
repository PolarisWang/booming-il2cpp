// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.ZipFile/zip/file/extensions
// Assembly: System.IO.Compression.ZipFile
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ZipFileExtensionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String)
    public static void Subject_0()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello")).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel)
    public static void Subject_1()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello", default)).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_2()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default)).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken)
    public static void Subject_3()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default, default)).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String)
    public static void Subject_4()
    {
        try { ZipFileExtensions.ExtractToDirectory(default, "hello"); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean)
    public static void Subject_5()
    {
        try { ZipFileExtensions.ExtractToDirectory(default, "hello", true); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { if (((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", default)).GetHashCode()) != ((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        try { if (((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", true, default)).GetHashCode()) != ((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String)
    public static void Subject_8()
    {
        try { ZipFileExtensions.ExtractToFile(default, "hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean)
    public static void Subject_9()
    {
        try { ZipFileExtensions.ExtractToFile(default, "hello", true); }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken)
    public static void Subject_10()
    {
        try { if (((ZipFileExtensions.ExtractToFileAsync(default, "hello", default)).GetHashCode()) != ((ZipFileExtensions.ExtractToFileAsync(default, "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_11()
    {
        try { if (((ZipFileExtensions.ExtractToFileAsync(default, "hello", true, default)).GetHashCode()) != ((ZipFileExtensions.ExtractToFileAsync(default, "hello", true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}
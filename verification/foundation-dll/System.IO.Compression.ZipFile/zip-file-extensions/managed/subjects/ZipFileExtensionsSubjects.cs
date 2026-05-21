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
        try { ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello"); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel)
    public static void Subject_1()
    {
        try { ZipFileExtensions.CreateEntryFromFile(default, "hello", "hello", default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_2()
    {
        if (((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default)).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken)
    public static void Subject_3()
    {
        if (((ZipFileExtensions.CreateEntryFromFileAsync(default, "hello", "hello", default, default)).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String)
    public static void Subject_4()
    {
        try { ZipFileExtensions.ExtractToDirectory(default, "hello"); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean)
    public static void Subject_5()
    {
        try { ZipFileExtensions.ExtractToDirectory(default, "hello", true); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        if (((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", default)).GetHashCode()) != 115000) _exitCode = 1;
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        if (((ZipFileExtensions.ExtractToDirectoryAsync(default, "hello", true, default)).GetHashCode()) != 1495009) _exitCode = 1;
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String)
    public static void Subject_8()
    {
        try { ZipFileExtensions.ExtractToFile(default, "hello"); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean)
    public static void Subject_9()
    {
        try { ZipFileExtensions.ExtractToFile(default, "hello", true); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken)
    public static void Subject_10()
    {
        if (((ZipFileExtensions.ExtractToFileAsync(default, "hello", default)).GetHashCode()) != 19435124) _exitCode = 1;
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_11()
    {
        if (((ZipFileExtensions.ExtractToFileAsync(default, "hello", true, default)).GetHashCode()) != 51330024) _exitCode = 1;
    }

}
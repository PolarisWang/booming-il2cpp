// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.ZipFile/zip/file/apis
// Assembly: System.IO.Compression.ZipFile
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

public static partial class ZipFileApisSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)
    public static void Subject_0()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_1()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", default, true); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Subject_2()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", default, true, default); }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)
    public static void Subject_3()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream()); }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_4()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream(), default, true); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Subject_5()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream(), default, true, default); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_8()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default, default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)
    public static void Subject_9()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_10()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_11()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default, default)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)
    public static void Subject_12()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean)
    public static void Subject_13()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean) could not be auto-generated
    }

    // [14] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding)
    public static void Subject_14()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding) could not be auto-generated
    }

    // [15] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)
    public static void Subject_15()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello", default, true); }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)
    public static void Subject_16()
    {
        try { ZipFile.ExtractToDirectory(new MemoryStream(), "hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean)
    public static void Subject_17()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean) could not be auto-generated
    }

    // [18] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding)
    public static void Subject_18()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding) could not be auto-generated
    }

    // [19] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)
    public static void Subject_19()
    {
        try { ZipFile.ExtractToDirectory(new MemoryStream(), "hello", default, true); }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_20()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", default)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_21()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken) could not be auto-generated
    }

    // [22] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_22()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken) could not be auto-generated
    }

    // [23] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_23()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", default, true, default)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", default, true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)
    public static void Subject_24()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_25()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken) could not be auto-generated
    }

    // [26] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_26()
    {
        // TODO: System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken) could not be auto-generated
    }

    // [27] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_27()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default, true, default)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default, true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode)
    public static void Subject_28()
    {
        try { if (((ZipFile.Open("hello", default)).GetHashCode()) != ((ZipFile.Open("hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding)
    public static void Subject_29()
    {
        try { if (((ZipFile.Open("hello", default, default)).GetHashCode()) != ((ZipFile.Open("hello", default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Threading.CancellationToken)
    public static void Subject_30()
    {
        try { if (((ZipFile.OpenAsync("hello", default, default)).GetHashCode()) != ((ZipFile.OpenAsync("hello", default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_31()
    {
        try { if (((ZipFile.OpenAsync("hello", default, default, default)).GetHashCode()) != ((ZipFile.OpenAsync("hello", default, default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String)
    public static void Subject_32()
    {
        try { if (((ZipFile.OpenRead("hello")).GetHashCode()) != ((ZipFile.OpenRead("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.Threading.CancellationToken)
    public static void Subject_33()
    {
        try { if (((ZipFile.OpenReadAsync("hello", default)).GetHashCode()) != ((ZipFile.OpenReadAsync("hello", default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}
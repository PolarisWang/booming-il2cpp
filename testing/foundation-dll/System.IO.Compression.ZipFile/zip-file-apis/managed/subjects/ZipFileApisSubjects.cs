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
        try { ZipFile.CreateFromDirectory("hello", "hello"); _exitCode = 1; }
        catch (IOException) { }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_1()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", default, true); _exitCode = 1; }
        catch (IOException) { }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Subject_2()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", default, true, default); _exitCode = 1; }
        catch (IOException) { }
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)
    public static void Subject_3()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream()); _exitCode = 1; }
        catch (IOException) { }
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_4()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream(), default, true); _exitCode = 1; }
        catch (IOException) { }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Subject_5()
    {
        try { ZipFile.CreateFromDirectory("hello", new MemoryStream(), default, true, default); _exitCode = 1; }
        catch (IOException) { }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default)).GetHashCode()) != 1542680) _exitCode = 1;
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default)).GetHashCode()) != 59386486) _exitCode = 1;
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_8()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", default, true, default, default)).GetHashCode()) != 33826822) _exitCode = 1;
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)
    public static void Subject_9()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default)).GetHashCode()) != 37095509) _exitCode = 1;
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Subject_10()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default)).GetHashCode()) != 12479570) _exitCode = 1;
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_11()
    {
        if (((ZipFile.CreateFromDirectoryAsync("hello", new MemoryStream(), default, true, default, default)).GetHashCode()) != 28016690) _exitCode = 1;
    }

    // [12] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)
    public static void Subject_12()
    {
        ZipFile.ExtractToDirectory("hello", "hello");
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
        ZipFile.ExtractToDirectory("hello", "hello", default, true);
    }

    // [16] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)
    public static void Subject_16()
    {
        try { ZipFile.ExtractToDirectory(new MemoryStream(), "hello"); _exitCode = 1; }
        catch (InvalidDataException) { }
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
        try { ZipFile.ExtractToDirectory(new MemoryStream(), "hello", default, true); _exitCode = 1; }
        catch (InvalidDataException) { }
    }

    // [20] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Subject_20()
    {
        if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", default)).GetHashCode()) != 37200188) _exitCode = 1;
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
        if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", default, true, default)).GetHashCode()) != 13840400) _exitCode = 1;
    }

    // [24] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)
    public static void Subject_24()
    {
        if (((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default)).GetHashCode()) != 45707473) _exitCode = 1;
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
        if (((ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default, true, default)).GetHashCode()) != 57326237) _exitCode = 1;
    }

    // [28] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode)
    public static void Subject_28()
    {
        if (((ZipFile.Open("hello", default)).GetHashCode()) != 7043582) _exitCode = 1;
    }

    // [29] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding)
    public static void Subject_29()
    {
        if (((ZipFile.Open("hello", default, default)).GetHashCode()) != 24457708) _exitCode = 1;
    }

    // [30] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Threading.CancellationToken)
    public static void Subject_30()
    {
        if (((ZipFile.OpenAsync("hello", default, default)).GetHashCode()) != 49514754) _exitCode = 1;
    }

    // [31] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Subject_31()
    {
        if (((ZipFile.OpenAsync("hello", default, default, default)).GetHashCode()) != 39712035) _exitCode = 1;
    }

    // [32] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String)
    public static void Subject_32()
    {
        if (((ZipFile.OpenRead("hello")).GetHashCode()) != 46494411) _exitCode = 1;
    }

    // [33] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.Threading.CancellationToken)
    public static void Subject_33()
    {
        if (((ZipFile.OpenReadAsync("hello", default)).GetHashCode()) != 447575) _exitCode = 1;
    }

}
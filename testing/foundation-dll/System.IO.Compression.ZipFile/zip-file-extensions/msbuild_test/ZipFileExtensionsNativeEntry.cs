// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.ZipFile/zip-file-extensions
// Assembly: System.IO.Compression.ZipFile
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ZipFileExtensionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String)
    public static void Method0()
    {
        try { if ((int)(ZipFileExtensions.CreateEntryFromFile(null!, "hello", "hello")) != (int)(ZipFileExtensions.CreateEntryFromFile(null!, "hello", "hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel)
    public static void Method1()
    {
        try { if ((int)(ZipFileExtensions.CreateEntryFromFile(null!, "hello", "hello", null!)) != (int)(ZipFileExtensions.CreateEntryFromFile(null!, "hello", "hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken)
    public static void Method2()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFileAsync(null!, "hello", "hello", null!)).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFileAsync(null!, "hello", "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken)
    public static void Method3()
    {
        try { if (((ZipFileExtensions.CreateEntryFromFileAsync(null!, "hello", "hello", null!, null!)).GetHashCode()) != ((ZipFileExtensions.CreateEntryFromFileAsync(null!, "hello", "hello", null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String)
    public static void Method4()
    {
        try { ZipFileExtensions.ExtractToDirectory(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean)
    public static void Method5()
    {
        try { ZipFileExtensions.ExtractToDirectory(null!, "hello", true); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken)
    public static void Method6()
    {
        try { if (((ZipFileExtensions.ExtractToDirectoryAsync(null!, "hello", null!)).GetHashCode()) != ((ZipFileExtensions.ExtractToDirectoryAsync(null!, "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Method7()
    {
        try { if (((ZipFileExtensions.ExtractToDirectoryAsync(null!, "hello", true, null!)).GetHashCode()) != ((ZipFileExtensions.ExtractToDirectoryAsync(null!, "hello", true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String)
    public static void Method8()
    {
        try { ZipFileExtensions.ExtractToFile(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean)
    public static void Method9()
    {
        try { ZipFileExtensions.ExtractToFile(null!, "hello", true); }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken)
    public static void Method10()
    {
        try { if (((ZipFileExtensions.ExtractToFileAsync(null!, "hello", null!)).GetHashCode()) != ((ZipFileExtensions.ExtractToFileAsync(null!, "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Method11()
    {
        try { if (((ZipFileExtensions.ExtractToFileAsync(null!, "hello", true, null!)).GetHashCode()) != ((ZipFileExtensions.ExtractToFileAsync(null!, "hello", true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.ZipFile/zip-file-apis
// Assembly: System.IO.Compression.ZipFile
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

public static partial class ZipFileApisNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)
    public static void Method0()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Method1()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", null!, true); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Method2()
    {
        try { ZipFile.CreateFromDirectory("hello", "hello", null!, true, null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)
    public static void Method3()
    {
        try { ZipFile.CreateFromDirectory("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Method4()
    {
        try { ZipFile.CreateFromDirectory("hello", null!, null!, true); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding)
    public static void Method5()
    {
        try { ZipFile.CreateFromDirectory("hello", null!, null!, true, null!); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Method6()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Method7()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!, true, null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!, true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Method8()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!, true, null!, null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", "hello", null!, true, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)
    public static void Method9()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", null!, null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Threading.CancellationToken)
    public static void Method10()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", null!, null!, true, null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", null!, null!, true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Method11()
    {
        try { if (((ZipFile.CreateFromDirectoryAsync("hello", null!, null!, true, null!, null!)).GetHashCode()) != ((ZipFile.CreateFromDirectoryAsync("hello", null!, null!, true, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)
    public static void Method12()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean)
    public static void Method13()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello", true); }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding)
    public static void Method14()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)
    public static void Method15()
    {
        try { ZipFile.ExtractToDirectory("hello", "hello", null!, true); }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)
    public static void Method16()
    {
        try { ZipFile.ExtractToDirectory(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean)
    public static void Method17()
    {
        try { ZipFile.ExtractToDirectory(null!, "hello", true); }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding)
    public static void Method18()
    {
        try { ZipFile.ExtractToDirectory(null!, "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)
    public static void Method19()
    {
        try { ZipFile.ExtractToDirectory(null!, "hello", null!, true); }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)
    public static void Method20()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Method21()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", true, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Method22()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)
    public static void Method23()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!, true, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync("hello", "hello", null!, true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)
    public static void Method24()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void Method25()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(null!, "hello", true, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(null!, "hello", true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Method26()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)
    public static void Method27()
    {
        try { if (((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!, true, null!)).GetHashCode()) != ((ZipFile.ExtractToDirectoryAsync(null!, "hello", null!, true, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode)
    public static void Method28()
    {
        try { if ((int)(ZipFile.Open("hello", null!)) != (int)(ZipFile.Open("hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:System.IO.Compression.ZipArchive(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding)
    public static void Method29()
    {
        try { if ((int)(ZipFile.Open("hello", null!, null!)) != (int)(ZipFile.Open("hello", null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Threading.CancellationToken)
    public static void Method30()
    {
        try { if (((ZipFile.OpenAsync("hello", null!, null!)).GetHashCode()) != ((ZipFile.OpenAsync("hello", null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.IO.Compression.ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)
    public static void Method31()
    {
        try { if (((ZipFile.OpenAsync("hello", null!, null!, null!)).GetHashCode()) != ((ZipFile.OpenAsync("hello", null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String)
    public static void Method32()
    {
        try { if ((int)(ZipFile.OpenRead("hello")) != (int)(ZipFile.OpenRead("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchive}(System.String,System.Threading.CancellationToken)
    public static void Method33()
    {
        try { if (((ZipFile.OpenReadAsync("hello", null!)).GetHashCode()) != ((ZipFile.OpenReadAsync("hello", null!)).GetHashCode())) _exitCode = 1; }
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
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: Method32(); break;
                case 33: Method33(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
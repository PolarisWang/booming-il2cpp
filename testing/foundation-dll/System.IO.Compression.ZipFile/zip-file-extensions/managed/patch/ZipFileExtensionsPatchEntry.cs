// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/zip/file/extensions
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ZipFileExtensionsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFile:System.IO.Compression.ZipArchiveEntry(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.Threading.CancellationToken)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::CreateEntryFromFileAsync:System.Threading.Tasks.Task{System.IO.Compression.ZipArchiveEntry}(System.IO.Compression.ZipArchive,System.String,System.String,System.IO.Compression.CompressionLevel,System.Threading.CancellationToken)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectory:System.Void(System.IO.Compression.ZipArchive,System.String,System.Boolean)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Threading.CancellationToken)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchive,System.String,System.Boolean,System.Threading.CancellationToken)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFile:System.Void(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Threading.CancellationToken)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ExtractToFileAsync:System.Threading.Tasks.Task(System.IO.Compression.ZipArchiveEntry,System.String,System.Boolean,System.Threading.CancellationToken)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
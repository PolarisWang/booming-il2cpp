// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using System.IO;
using System.IO.Compression;
using Chaos.TestFramework;

public static partial class ZipFileApisSubjects
{
    // [13] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Boolean)
    public static void CustomEntrySubject_13()
    {
        try {
            ZipFile.ExtractToDirectory("hello", "hello", true);
        }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding)
    public static void CustomEntrySubject_14()
    {
        try {
            ZipFile.ExtractToDirectory("hello", "hello", default);
        }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Boolean)
    public static void CustomEntrySubject_17()
    {
        try {
            ZipFile.ExtractToDirectory(new MemoryStream(), "hello", true);
        }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding)
    public static void CustomEntrySubject_18()
    {
        try {
            ZipFile.ExtractToDirectory(new MemoryStream(), "hello", default);
        }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void CustomEntrySubject_21()
    {
        try {
            Assert.IsNotNull(ZipFile.ExtractToDirectoryAsync("hello", "hello", true, default));
        }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void CustomEntrySubject_22()
    {
        try {
            Assert.IsNotNull(ZipFile.ExtractToDirectoryAsync("hello", "hello", default, default));
        }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Boolean,System.Threading.CancellationToken)
    public static void CustomEntrySubject_25()
    {
        try {
            Assert.IsNotNull(ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", true, default));
        }
        catch { _exitCode = 1; }
    }

    // [26] System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Threading.CancellationToken)
    public static void CustomEntrySubject_26()
    {
        try {
            Assert.IsNotNull(ZipFile.ExtractToDirectoryAsync(new MemoryStream(), "hello", default, default));
        }
        catch { _exitCode = 1; }
    }

}

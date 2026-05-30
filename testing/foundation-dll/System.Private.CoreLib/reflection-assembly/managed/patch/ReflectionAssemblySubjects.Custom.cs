// Handwritten partial class for reflection-assembly custom entries.
// Source: handwritten/ — READ ONLY, pipeline never overwrites.
// Pipeline copies this file to subjects dir for compilation.

using System;
using Chaos.TestFramework;

public static partial class ReflectionAssemblySubjects
{
    /// Custom entry for Subject_21 (Type.GetType with non-existent type).
    /// Type.GetType("NonExistentType") returns null in both managed and native AOT.
    /// We dereference with !.GetHashCode() to trigger codegen's RaiseNullReferenceException.
    public static void CustomEntrySubject_21()
    {
        try
        {
        Assert.Throws<NullReferenceException>(() => Type.GetType("NonExistentType")!.GetHashCode());
        }
        catch { _exitCode = 1; }

    }

    // [3] Assembly::GetName()
    public static void CustomEntrySubject_3()
    {
        try
        {
        _ = typeof(byte).Assembly.GetName().Version?.GetHashCode();
        }
        catch { _exitCode = 1; }

    }

    // [7] Assembly::GetTypes()
    public static void CustomEntrySubject_7()
    {
        try
        {
        _ = typeof(byte).Assembly.GetTypes().Length;
        }
        catch { _exitCode = 1; }

    }

    // [8] Assembly::GetExportedTypes()
    public static void CustomEntrySubject_8()
    {
        try
        {
        _ = typeof(byte).Assembly.GetExportedTypes().Length;
        }
        catch { _exitCode = 1; }

    }

    // [9] Assembly::GetForwardedTypes()
    public static void CustomEntrySubject_9()
    {
        try
        {
        _ = typeof(byte).Assembly.GetForwardedTypes().Length;
        }
        catch { _exitCode = 1; }

    }

    // [10] Assembly::GetType(System.String)
    public static void CustomEntrySubject_10()
    {
        try
        {
        _ = typeof(byte).Assembly.GetType("System.Byte")?.GetHashCode();
        }
        catch { _exitCode = 1; }

    }

    // [11] Assembly::GetType(System.String, System.Boolean)
    public static void CustomEntrySubject_11()
    {
        try
        {
        _ = typeof(byte).Assembly.GetType("System.Byte", true)?.GetHashCode();
        }
        catch { _exitCode = 1; }

    }

    // [12] Assembly::GetModule(System.String)
    public static void CustomEntrySubject_12()
    {
        try
        {
        _ = typeof(byte).Assembly.GetModule("System.Private.CoreLib.dll")?.GetHashCode();
        }
        catch { _exitCode = 1; }

    }

    // [13] Assembly::GetModules()
    public static void CustomEntrySubject_13()
    {
        try
        {
        _ = typeof(byte).Assembly.GetModules().Length;
        }
        catch { _exitCode = 1; }

    }

    // [14] Assembly::GetManifestResourceStream(System.String)
    public static void CustomEntrySubject_14()
    {
        try
        {
        _ = typeof(byte).Assembly.GetManifestResourceStream("NonExistent");
        }
        catch { _exitCode = 1; }

    }

    // [15] Assembly::GetManifestResourceNames()
    public static void CustomEntrySubject_15()
    {
        try
        {
        _ = typeof(byte).Assembly.GetManifestResourceNames().Length;
        }
        catch { _exitCode = 1; }

    }
}

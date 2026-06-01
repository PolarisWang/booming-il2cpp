// Handwritten partial class for reflection-assembly custom entries.
// Source: handwritten/ — READ ONLY, pipeline never overwrites.
// Pipeline copies this file to subjects dir for compilation.

using System;
using Chaos.TestFramework;

public static partial class ReflectionAssemblySubjects
{
    /// <summary>
    /// Custom entry for Subject_21 (Type.GetType with non-existent type).
    /// Type.GetType("NonExistentType") returns null in both managed and native AOT.
    /// We dereference with !.GetHashCode() to trigger codegen's RaiseNullReferenceException.
    /// </summary>
    [Fact]
    public static void CustomEntrySubject_21()
    {
        Assert.Throws<NullReferenceException>(() => Type.GetType("NonExistentType")!.GetHashCode());
    }

    // [3] Assembly::GetName()
    [Fact]
    public static void CustomEntrySubject_3()
    {
        _ = typeof(byte).Assembly.GetName().Version?.GetHashCode();
    }

    // [7] Assembly::GetTypes()
    [Fact]
    public static void CustomEntrySubject_7()
    {
        _ = typeof(byte).Assembly.GetTypes().Length;
    }

    // [8] Assembly::GetExportedTypes()
    [Fact]
    public static void CustomEntrySubject_8()
    {
        _ = typeof(byte).Assembly.GetExportedTypes().Length;
    }

    // [9] Assembly::GetForwardedTypes()
    [Fact]
    public static void CustomEntrySubject_9()
    {
        _ = typeof(byte).Assembly.GetForwardedTypes().Length;
    }

    // [10] Assembly::GetType(System.String)
    [Fact]
    public static void CustomEntrySubject_10()
    {
        _ = typeof(byte).Assembly.GetType("System.Byte")?.GetHashCode();
    }

    // [11] Assembly::GetType(System.String, System.Boolean)
    [Fact]
    public static void CustomEntrySubject_11()
    {
        _ = typeof(byte).Assembly.GetType("System.Byte", true)?.GetHashCode();
    }

    // [12] Assembly::GetModule(System.String)
    [Fact]
    public static void CustomEntrySubject_12()
    {
        _ = typeof(byte).Assembly.GetModule("System.Private.CoreLib.dll")?.GetHashCode();
    }

    // [13] Assembly::GetModules()
    [Fact]
    public static void CustomEntrySubject_13()
    {
        _ = typeof(byte).Assembly.GetModules().Length;
    }

    // [14] Assembly::GetManifestResourceStream(System.String)
    [Fact]
    public static void CustomEntrySubject_14()
    {
        _ = typeof(byte).Assembly.GetManifestResourceStream("NonExistent");
    }

    // [15] Assembly::GetManifestResourceNames()
    [Fact]
    public static void CustomEntrySubject_15()
    {
        _ = typeof(byte).Assembly.GetManifestResourceNames().Length;
    }
}

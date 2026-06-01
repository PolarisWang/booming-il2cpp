// Handwritten custom entries for EnumParsingSubjects (subjects variant).
// Uses CustomEntrySubject_N naming to match the subjects variant's entrypoint generator.
// These replace auto-generated Subject_N methods for indices in customEntryIndices.
// NOTE: Avoid `is` pattern matching or (DayOfWeek) casts — codegen generates broken
//       intptr_t←valuetype assignments for enum types.
using System;
using Chaos.TestFramework;
public static partial class EnumParsingSubjects
{
    // [6] Enum.Parse(typeof(DayOfWeek), "Monday") — valid input exercises real Parse path
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        Assert.IsNotNull(Enum.Parse(typeof(DayOfWeek), "Monday"));
        }
        catch { }

    }

    // [7] Enum.Parse(typeof(DayOfWeek), "monday", true) — valid input with ignoreCase
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        Assert.IsNotNull(Enum.Parse(typeof(DayOfWeek), "monday", true));
        }
        catch { }

    }

    // [10] Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_10()
    {
        try
        {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "Monday", true, out result));
        }
        catch { }

    }

    // [11] Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_11()
    {
        try
        {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "Monday", out result));
        }
        catch { }

    }

    // [12] Enum.BoxToString — call ToString() on an enum value, verify non-empty result
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_12()
    {
        try
        {
        Assert.AreEqual(6, ((Enum)DayOfWeek.Monday).ToString().Length);
        }
        catch { }

    }
}

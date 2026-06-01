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

    // [13] Enum.Parse(typeof(DayOfWeek), "1") — numeric string parsing
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_13()
    {
        try
        {
        var result = Enum.Parse(typeof(DayOfWeek), "1");
        Assert.IsNotNull(result);
        Assert.AreEqual(1, (int)result);
        }
        catch { }

    }

    // [14] Enum.Parse(typeof(DayOfWeek), "0x1") — hex string parsing
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_14()
    {
        try
        {
        var result = Enum.Parse(typeof(DayOfWeek), "0x1");
        Assert.IsNotNull(result);
        Assert.AreEqual(1, (int)result);
        }
        catch { }

    }

    // [15] Enum.TryParse(typeof(DayOfWeek), "1", out object _) — numeric TryParse
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_15()
    {
        try
        {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "1", out result));
        Assert.AreEqual(1, (int)result);
        }
        catch { }

    }

    // [16] Enum.Parse(typeof(DayOfWeek), "Monday, Tuesday") — comma-separated names
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_16()
    {
        try
        {
        var result = Enum.Parse(typeof(DayOfWeek), "Monday, Tuesday");
        Assert.IsNotNull(result);
        // Monday(1) | Tuesday(2) = 3
        Assert.AreEqual(3, (int)result);
        }
        catch { }

    }

    // [17] Enum.TryParse(typeof(DayOfWeek), "0x2", out object _) — hex TryParse
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_17()
    {
        try
        {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "0x2", out result));
        Assert.AreEqual(2, (int)result);
        }
        catch { }

    }

    // [18] Enum.TryParse(typeof(DayOfWeek), "Monday, 0x2", out object _) — mixed comma-separated (name + hex)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntrySubject_18()
    {
        try
        {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "Monday, 0x2", out result));
        // Monday(1) | Tuesday(2) = 3
        Assert.AreEqual(3, (int)result);
        }
        catch { }

    }
}

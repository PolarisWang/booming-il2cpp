// Handwritten custom entries for EnumParsingPatchEntry (patch variant).
// Uses CustomEntryMethodN naming to match the patch variant's entrypoint generator.
// NOTE: Avoid `is` pattern matching or (DayOfWeek) casts — codegen generates broken
//       intptr_t←valuetype assignments for enum types.
using System;
using Chaos.TestFramework;
public static partial class EnumParsingPatchEntry
{
    // [6] Enum.Parse(typeof(DayOfWeek), "Monday") — valid input exercises real Parse path
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod6()
    {
        Assert.IsNotNull(Enum.Parse(typeof(DayOfWeek), "Monday"));
    }

    // [7] Enum.Parse(typeof(DayOfWeek), "monday", true) — valid input with ignoreCase
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod7()
    {
        Assert.IsNotNull(Enum.Parse(typeof(DayOfWeek), "monday", true));
    }

    // [10] Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod10()
    {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "Monday", true, out result));
    }

    // [11] Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod11()
    {
        object result;
        Assert.IsTrue(Enum.TryParse(typeof(DayOfWeek), "Monday", out result));
    }

    // [12] Enum.BoxToString — call ToString() on an enum value, verify non-empty result
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod12()
    {
        Assert.AreEqual(6, ((Enum)DayOfWeek.Monday).ToString().Length);
    }
}

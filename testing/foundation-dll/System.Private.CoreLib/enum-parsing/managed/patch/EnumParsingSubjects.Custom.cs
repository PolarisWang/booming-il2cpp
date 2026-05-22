// Handwritten custom entries for EnumParsingSubjects (subjects variant).
// Uses CustomEntrySubject_N naming to match the subjects variant's entrypoint generator.
// These replace auto-generated Subject_N methods for indices in customEntryIndices.
// NOTE: Avoid `is` pattern matching or (DayOfWeek) casts — codegen generates broken
//       intptr_t←valuetype assignments for enum types.
using System;
public static partial class EnumParsingSubjects
{
    // [6] Enum.Parse(typeof(DayOfWeek), "Monday") — valid input exercises real Parse path
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_6()
    {
        // Simple null-check avoids complex is-pattern/boxing codegen
        if (null == Enum.Parse(typeof(DayOfWeek), "Monday")) _exitCode = 1;
    }

    // [7] Enum.Parse(typeof(DayOfWeek), "monday", true) — valid input with ignoreCase
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_7()
    {
        if (null == Enum.Parse(typeof(DayOfWeek), "monday", true)) _exitCode = 1;
    }

    // [10] Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_10()
    {
        object result;
        if (!Enum.TryParse(typeof(DayOfWeek), "Monday", true, out result)) _exitCode = 1;
    }

    // [11] Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_11()
    {
        object result;
        if (!Enum.TryParse(typeof(DayOfWeek), "Monday", out result)) _exitCode = 1;
    }

    // [12] Enum.BoxToString — call ToString() on an enum value, verify non-empty result
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_12()
    {
        if ((((Enum)DayOfWeek.Monday).ToString()).Length != 6) _exitCode = 1;
    }
}

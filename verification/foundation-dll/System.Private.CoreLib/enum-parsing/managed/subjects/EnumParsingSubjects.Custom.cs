// Handwritten custom entry for explicit box + call Enum::ToString() peephole verification.
// Uses ((Enum)DayOfWeek.Monday).ToString() to generate box IL instruction + callvirt Enum::ToString().
// The auto-generator marks this as customEntry to avoid trying to emit DayOfWeek.Monday.BoxToString().
using System;
public static partial class EnumParsingSubjects
{
    // [12] Explicit box + call Enum::ToString() — verifies Opt B ChaosEnumToStringRaw peephole
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntrySubject_12()
    {
        // (Enum) cast forces box DayOfWeek, then callvirt System.Enum::ToString() on the Enum-typed ref
        if ((((Enum)DayOfWeek.Monday).ToString()).Length != 6) _exitCode = 1;
    }
}

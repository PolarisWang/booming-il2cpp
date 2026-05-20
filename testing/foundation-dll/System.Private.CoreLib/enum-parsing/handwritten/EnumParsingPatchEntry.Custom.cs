// Handwritten custom entry for patch variant — naming matches patch dispatcher convention
using System;
public static partial class EnumParsingPatchEntry
{
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod12()
    {
        if ((((Enum)DayOfWeek.Monday).ToString()).Length != 6) _exitCode = 1;
    }
}

using System;
using System.Collections.Immutable;

public static partial class ImmutableInterlockedSubjects
{
    public static void CustomEntrySubject_0()
    {
        try
        {
            string value = "hello";
            bool result = ImmutableInterlocked.Update<string>(ref value, (s) => s + " world");
            if (!result) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }
}

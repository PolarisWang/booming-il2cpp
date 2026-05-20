using System;
using System.Collections.Immutable;

public static partial class ImmutableDictionarySubjects
{
    public static void CustomEntrySubject_0()
    { try { var r = ImmutableDictionary.Create<string, int>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = ImmutableDictionary.Create<byte, byte>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = ImmutableDictionary.Create<string, string>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
}

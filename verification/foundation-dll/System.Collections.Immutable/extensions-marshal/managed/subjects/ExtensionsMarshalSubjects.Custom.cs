using System;
using System.Collections.Immutable;

public static partial class ExtensionsMarshalSubjects
{
    public static void CustomEntrySubject_0()
    { try { var r = ImmutableArray.Create<byte>(); if (r.Length != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = ImmutableArray.Create<byte>((byte)42); if (r.Length != 1) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = ImmutableArray.Create<int>(); if (r.Length != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_3()
    { try { var r = ImmutableArray.Create<int>(42); if (r.Length != 1) _exitCode = 1; } catch { _exitCode = 1; } }
}

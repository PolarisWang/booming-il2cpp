using System;
using System.Collections.Immutable;

public static partial class ImmutableSortedSetSubjects
{
    public static void CustomEntrySubject_0()
    { try { var r = ImmutableSortedSet.Create<byte>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = ImmutableSortedSet.Create<byte>((byte)42); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = ImmutableSortedSet.Create<byte>((byte)42, (byte)43); if (r.Count != 2) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_3()
    { try { var r = ImmutableSortedSet.Create<int>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_4()
    { try { var r = ImmutableSortedSet.Create<int>(42); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
}

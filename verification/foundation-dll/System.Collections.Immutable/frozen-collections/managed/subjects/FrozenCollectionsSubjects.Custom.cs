using System;
using System.Collections.Frozen;

public static partial class FrozenCollectionsSubjects
{
    public static void CustomEntrySubject_0()
    { try { var r = FrozenDictionary.Create<string, int>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = FrozenDictionary.Create<byte, byte>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = FrozenDictionary.Create<string, string>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_3()
    { try { var r = FrozenSet.Create<byte>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_4()
    { try { var r = FrozenSet.Create<byte>((byte)42); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_5()
    { try { var r = FrozenSet.Create<byte>((byte)42, (byte)43); if (r.Count != 2) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_6()
    { try { var r = FrozenSet.Create<int>(); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_7()
    { try { var r = FrozenSet.Create<int>(42); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
}

using System;
using System.Collections.Frozen;
using System.Collections.Generic;

public static partial class FrozenCollectionsSubjects
{
    public static void CustomEntrySubject_0()
    { try { var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<string, int>()); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<byte, byte>()); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<string, string>()); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_3()
    { try { var r = FrozenSet.ToFrozenSet(new HashSet<byte>()); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_4()
    { try { var r = FrozenSet.ToFrozenSet(new byte[] { 42 }); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_5()
    { try { var r = FrozenSet.ToFrozenSet(new byte[] { 42, 43 }); if (r.Count != 2) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_6()
    { try { var r = FrozenSet.ToFrozenSet(new HashSet<int>()); if (r.Count != 0) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_7()
    { try { var r = FrozenSet.ToFrozenSet(new int[] { 42 }); if (r.Count != 1) _exitCode = 1; } catch { _exitCode = 1; } }
}

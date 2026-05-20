using System;
using System.Collections.Immutable;

public static partial class ImmutableQueueStackSubjects
{
    // ImmutableQueue<T> uses IsEmpty (not Count)
    public static void CustomEntrySubject_0()
    { try { var r = ImmutableQueue.Create<byte>(); if (!r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_1()
    { try { var r = ImmutableQueue.Create<byte>((byte)42); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_2()
    { try { var r = ImmutableQueue.Create<byte>((byte)42, (byte)43); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_3()
    { try { var r = ImmutableQueue.Create<int>(); if (!r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_4()
    { try { var r = ImmutableQueue.Create<int>(42); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    // ImmutableStack<T> uses IsEmpty (not Count)
    public static void CustomEntrySubject_5()
    { try { var r = ImmutableStack.Create<byte>(); if (!r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_6()
    { try { var r = ImmutableStack.Create<byte>((byte)42); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_7()
    { try { var r = ImmutableStack.Create<byte>((byte)42, (byte)43); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_8()
    { try { var r = ImmutableStack.Create<int>(); if (!r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
    public static void CustomEntrySubject_9()
    { try { var r = ImmutableStack.Create<int>(42); if (r.IsEmpty) _exitCode = 1; } catch { _exitCode = 1; } }
}

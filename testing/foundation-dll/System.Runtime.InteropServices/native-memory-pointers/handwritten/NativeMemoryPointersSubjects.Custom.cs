// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class NativeMemoryPointersSubjects
{
    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_8()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_9()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_10()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_11()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_12()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_13()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_14()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32)
    public static void CustomEntrySubject_15()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { _exitCode = 1; }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32)
    public static void CustomEntrySubject_16()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32)
    public static void CustomEntrySubject_17()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32)
    public static void CustomEntrySubject_18()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32)
    public static void CustomEntrySubject_19()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32)
    public static void CustomEntrySubject_20()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32)
    public static void CustomEntrySubject_21()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32)
    public static void CustomEntrySubject_22()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32)
    public static void CustomEntrySubject_23()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { _exitCode = 1; }    }

    // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)
    public static void CustomEntrySubject_92()
    {        try { _ = Marshal.QueryInterface(IntPtr.Zero, out Guid.Empty, out IntPtr.Zero); } catch { _exitCode = 1; }    }

    // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)
    public static void CustomEntrySubject_136()
    {        try { Marshal.ThrowExceptionForHR(42, out Guid.Empty, IntPtr.Zero); } catch { _exitCode = 1; }    }

}

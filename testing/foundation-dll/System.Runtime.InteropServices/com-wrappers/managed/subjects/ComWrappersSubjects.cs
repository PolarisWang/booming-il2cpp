// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/com/wrappers
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class ComWrappersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&)
    public static void Subject_0()
    {
        try { ComWrappers.GetIUnknownImpl(out _, out _, out _); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags)
    public static void Subject_1()
    {
        try { if ((int)(default(ComWrappers)!.GetOrCreateComInterfaceForObject(42, default)) != (int)(default(ComWrappers)!.GetOrCreateComInterfaceForObject(42, default))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags)
    public static void Subject_2()
    {
        try { if (((default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default)).GetHashCode()) != ((default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Subject_3()
    {
        try { if (((default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default, 42)).GetHashCode()) != ((default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Subject_4()
    {
        try { if (((default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42)).GetHashCode()) != ((default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr)
    public static void Subject_5()
    {
        try { if (((default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42, IntPtr.Zero)).GetHashCode()) != ((default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42, IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Subject_6()
    {
        try { ComWrappers.RegisterForMarshalling(default(ComWrappers)!); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Subject_7()
    {
        try { ComWrappers.RegisterForTrackerSupport(default(ComWrappers)!); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)
    public static void Subject_8()
    {
        try { if (((ComWrappers.TryGetComInstance(42, out _)) ? 1 : 0) != ((ComWrappers.TryGetComInstance(42, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)
    public static void Subject_9()
    {
        try { if (((ComWrappers.TryGetObject(IntPtr.Zero, out _)) ? 1 : 0) != ((ComWrappers.TryGetObject(IntPtr.Zero, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}
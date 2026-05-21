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
        ComWrappers.GetIUnknownImpl(out _, out _, out _);
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags)
    public static void Subject_1()
    {
        try { default(ComWrappers)!.GetOrCreateComInterfaceForObject(42, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags)
    public static void Subject_2()
    {
        try { default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Subject_3()
    {
        try { default(ComWrappers)!.GetOrCreateObjectForComInstance(IntPtr.Zero, default, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Subject_4()
    {
        try { default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr)
    public static void Subject_5()
    {
        try { default(ComWrappers)!.GetOrRegisterObjectForComInstance(IntPtr.Zero, default, 42, IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Subject_6()
    {
        try { ComWrappers.RegisterForMarshalling(default(ComWrappers)!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Subject_7()
    {
        try { ComWrappers.RegisterForTrackerSupport(default(ComWrappers)!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)
    public static void Subject_8()
    {
        if (((ComWrappers.TryGetComInstance(42, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)
    public static void Subject_9()
    {
        if (((ComWrappers.TryGetObject(IntPtr.Zero, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

}
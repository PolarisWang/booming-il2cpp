// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/com-wrappers
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class ComWrappersNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&)
    public static void Method0()
    {
    var refLocal_0 = IntPtr.Zero;
    var refLocal_1 = IntPtr.Zero;
    var refLocal_2 = IntPtr.Zero;
        try { ComWrappers.GetIUnknownImpl(ref refLocal_0, ref refLocal_1, ref refLocal_2); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags)
    public static void Method1()
    {
        try { if ((int)(ComWrappers.GetOrCreateComInterfaceForObject(42, null!)) != (int)(ComWrappers.GetOrCreateComInterfaceForObject(42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags)
    public static void Method2()
    {
        try { if (((ComWrappers.GetOrCreateObjectForComInstance(IntPtr.Zero, null!)).GetHashCode()) != ((ComWrappers.GetOrCreateObjectForComInstance(IntPtr.Zero, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Method3()
    {
        try { if (((ComWrappers.GetOrCreateObjectForComInstance(IntPtr.Zero, null!, 42)).GetHashCode()) != ((ComWrappers.GetOrCreateObjectForComInstance(IntPtr.Zero, null!, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static void Method4()
    {
        try { if (((ComWrappers.GetOrRegisterObjectForComInstance(IntPtr.Zero, null!, 42)).GetHashCode()) != ((ComWrappers.GetOrRegisterObjectForComInstance(IntPtr.Zero, null!, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr)
    public static void Method5()
    {
        try { if (((ComWrappers.GetOrRegisterObjectForComInstance(IntPtr.Zero, null!, 42, IntPtr.Zero)).GetHashCode()) != ((ComWrappers.GetOrRegisterObjectForComInstance(IntPtr.Zero, null!, 42, IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Method6()
    {
        try { ComWrappers.RegisterForMarshalling(null!); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static void Method7()
    {
        try { ComWrappers.RegisterForTrackerSupport(null!); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)
    public static void Method8()
    {
    var refLocal_1 = IntPtr.Zero;
        try { if (((ComWrappers.TryGetComInstance(42, ref refLocal_1)) ? 1 : 0) != ((ComWrappers.TryGetComInstance(42, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)
    public static void Method9()
    {
    var refLocal_1 = 42;
        try { if (((ComWrappers.TryGetObject(IntPtr.Zero, ref refLocal_1)) ? 1 : 0) != ((ComWrappers.TryGetObject(IntPtr.Zero, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
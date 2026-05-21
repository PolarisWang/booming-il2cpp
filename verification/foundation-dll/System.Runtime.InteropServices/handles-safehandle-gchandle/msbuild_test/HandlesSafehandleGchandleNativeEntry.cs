// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/handles-safehandle-gchandle
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class HandlesSafehandleGchandleNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void()
    public static void Method0()
    {
        try { SafeHandle.Close(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    public static void Method1()
    {
    var refLocal_0 = true;
        try { SafeHandle.DangerousAddRef(ref refLocal_0); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()
    public static void Method2()
    {
        try { if ((int)(SafeHandle.DangerousGetHandle()) != (int)(SafeHandle.DangerousGetHandle())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()
    public static void Method3()
    {
        try { SafeHandle.DangerousRelease(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()
    public static void Method4()
    {
        try { SafeHandle.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()
    public static void Method5()
    {
        try { if (((SafeHandle.IsClosed) ? 1 : 0) != ((SafeHandle.IsClosed) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()
    public static void Method6()
    {
        try { if (((SafeHandle.IsInvalid) ? 1 : 0) != ((SafeHandle.IsInvalid) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()
    public static void Method7()
    {
        try { SafeHandle.SetHandleAsInvalid(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()
    public static void Method8()
    {
        try { CriticalHandle.Close(); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()
    public static void Method9()
    {
        try { CriticalHandle.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()
    public static void Method10()
    {
        try { if (((CriticalHandle.IsClosed) ? 1 : 0) != ((CriticalHandle.IsClosed) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()
    public static void Method11()
    {
        try { if (((CriticalHandle.IsInvalid) ? 1 : 0) != ((CriticalHandle.IsInvalid) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()
    public static void Method12()
    {
        try { CriticalHandle.SetHandleAsInvalid(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::AddrOfPinnedObject:System.IntPtr()
    public static void Method13()
    {
        try { if ((int)(GCHandle.AddrOfPinnedObject()) != (int)(GCHandle.AddrOfPinnedObject())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object)
    public static void Method14()
    {
        try { if ((int)(GCHandle.Alloc(42)) != (int)(GCHandle.Alloc(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object,System.Runtime.InteropServices.GCHandleType)
    public static void Method15()
    {
        try { if ((int)(GCHandle.Alloc(42, null!)) != (int)(GCHandle.Alloc(42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Object)
    public static void Method16()
    {
        try { if (((GCHandle.Equals(42)) ? 1 : 0) != ((GCHandle.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Runtime.InteropServices.GCHandle)
    public static void Method17()
    {
        try { if (((GCHandle.Equals(null!)) ? 1 : 0) != ((GCHandle.Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Free:System.Void()
    public static void Method18()
    {
        try { GCHandle.Free(); }
        catch { _exitCode = 1; }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void Method19()
    {
        try { if ((int)(GCHandle.FromIntPtr(IntPtr.Zero)) != (int)(GCHandle.FromIntPtr(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_IsAllocated:System.Boolean()
    public static void Method20()
    {
        try { if (((GCHandle.IsAllocated) ? 1 : 0) != ((GCHandle.IsAllocated) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_Target:System.Object()
    public static void Method21()
    {
        try { if (((GCHandle.Target).GetHashCode()) != ((GCHandle.Target).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::GetHashCode:System.Int32()
    public static void Method22()
    {
        try { if (GCHandle.GetHashCode() != GCHandle.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Equality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)
    public static void Method23()
    {
        try { if (((GCHandle.op_Equality(null!, null!)) ? 1 : 0) != ((GCHandle.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void Method24()
    {
        try { if ((int)(GCHandle.op_Explicit(IntPtr.Zero)) != (int)(GCHandle.op_Explicit(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void Method25()
    {
        try { if ((int)(GCHandle.op_Explicit(null!)) != (int)(GCHandle.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Inequality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)
    public static void Method26()
    {
        try { if (((GCHandle.op_Inequality(null!, null!)) ? 1 : 0) != ((GCHandle.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    public static void Method27()
    {
        try { GCHandle.Target = 42; }
        catch { _exitCode = 1; }
    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void Method28()
    {
        try { if ((int)(GCHandle.ToIntPtr(null!)) != (int)(GCHandle.ToIntPtr(null!))) _exitCode = 1; }
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
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
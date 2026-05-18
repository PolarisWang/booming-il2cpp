// Copy of handwritten RcwBasicNativeEntry for managed subject compilation.
// Family: family/System.Private.CoreLib/rcw/basic
// Variant: subjects

using System;
using System.Runtime.InteropServices;

[Guid("7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F")]
public interface ISimpleMath
{
    int Add(int a, int b);
    int Multiply(int a, int b);
}

[Guid("9A8B7C6D-5E4F-3A2B-1C0D-9E8F7A6B5C4D")]
public interface IConstantValue
{
    int GetValue();
}

public class SimpleMath // was: ISimpleMath
{
    public int Add(int a, int b) => a + b;
    public int Multiply(int a, int b) => a * b;
}

public class ConstantFortyTwo // was: IConstantValue
{
    public int GetValue() => 42;
}

public static class RcwBasicNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return TestRcwRoundTripIdentity();
            case 1: return TestRcwRoundTripQi();
            case 2: return TestRcwMultipleWrappers();
            case 3: return TestRcwQiUnknownInterface();
            case 4: return TestRcwVtableMethodCall();
            case 5: return TestRcwDirectVtable();
            default: return -1;
        }
    }

    public static int TestRcwRoundTripIdentity()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        IntPtr rcwUnknown = MarshalGetRcwUnknown(rcw);
        if (rcwUnknown == IntPtr.Zero) return 4;

        if (rcwUnknown != ccwUnknown) return 5;

        MarshalReleaseRcw(rcw);
        return 0;
    }

    public static int TestRcwRoundTripQi()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        byte[] guidBytes = new byte[16] {
            0xC0, 0xCF, 0x4F, 0x7E, 0x8F, 0x3A, 0x0C, 0x4A,
            0x8C, 0x9D, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F
        };
        IntPtr guidPtr = Marshal.AllocHGlobal(16);
        Marshal.Copy(guidBytes, 0, guidPtr, 16);

        IntPtr ifacePtr = MarshalRcwQueryInterface(rcw, guidPtr);
        Marshal.FreeHGlobal(guidPtr);

        if (ifacePtr == IntPtr.Zero) return 4;

        IntPtr identityUnknown = MarshalGetRcwUnknown(rcw);
        if (ifacePtr == identityUnknown) return 5;

        MarshalReleaseRcw(rcw);
        return 0;
    }

    public static int TestRcwMultipleWrappers()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw1 = MarshalCreateRcw(ccwUnknown);
        if (rcw1 == IntPtr.Zero) return 3;

        IntPtr rcw2 = MarshalCreateRcw(ccwUnknown);
        if (rcw2 == IntPtr.Zero) { MarshalReleaseRcw(rcw1); return 4; }

        IntPtr identity1 = MarshalGetRcwUnknown(rcw1);
        IntPtr identity2 = MarshalGetRcwUnknown(rcw2);
        if (identity1 != identity2) { MarshalReleaseRcw(rcw2); MarshalReleaseRcw(rcw1); return 5; }

        if (identity1 != ccwUnknown) { MarshalReleaseRcw(rcw2); MarshalReleaseRcw(rcw1); return 6; }

        MarshalReleaseRcw(rcw2);
        MarshalReleaseRcw(rcw1);
        return 0;
    }

    public static int TestRcwQiUnknownInterface()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        byte[] guidBytes = new byte[16] {
            0xC0, 0xCF, 0x4F, 0x7E, 0x8F, 0x3A, 0x0C, 0x4A,
            0x8C, 0x9D, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F
        };
        IntPtr guidPtr = Marshal.AllocHGlobal(16);
        Marshal.Copy(guidBytes, 0, guidPtr, 16);

        IntPtr ifacePtr = MarshalRcwQueryInterface(rcw, guidPtr);
        Marshal.FreeHGlobal(guidPtr);

        if (ifacePtr != IntPtr.Zero) { MarshalReleaseRcw(rcw); return 4; }

        MarshalReleaseRcw(rcw);
        return 0;
    }

    // [4] RCW-aware method dispatch via ComVtable
    public static int TestRcwVtableMethodCall()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        int result = MarshalCallComMethod(rcw, 3, 3, 4);
        if (result != 7) return 4;

        MarshalReleaseRcw(rcw);
        return 0;
    }

    // [5] Direct ComVtable dispatch (no RCW check)
    public static int TestRcwDirectVtable()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        int result = MarshalCallDirectComMethod(ccwUnknown, 3, 10, 5);
        if (result != 15) return 3;

        return 0;
    }

    static IntPtr CreateCcwForSimpleMath(IntPtr runtimeState)
    {
        var obj = new SimpleMath();
        IntPtr objHandle = GCHandle.ToIntPtr(GCHandle.Alloc(obj));
        IntPtr ccw = MarshalCreateCcw(objHandle, runtimeState);
        if (ccw == IntPtr.Zero)
        {
            GCHandle.FromIntPtr(objHandle).Free();
            return IntPtr.Zero;
        }
        return ccw;
    }

    [DllImport("__Internal", EntryPoint = "MarshalCreateCcw")]
    static extern IntPtr MarshalCreateCcw(IntPtr managedObject, IntPtr runtimeState);

    [DllImport("__Internal", EntryPoint = "MarshalCreateRcw")]
    static extern IntPtr MarshalCreateRcw(IntPtr unknownPtr);

    [DllImport("__Internal", EntryPoint = "MarshalReleaseRcw")]
    static extern void MarshalReleaseRcw(IntPtr rcwNative);

    [DllImport("__Internal", EntryPoint = "MarshalGetRcwUnknown")]
    static extern IntPtr MarshalGetRcwUnknown(IntPtr rcwNative);

    [DllImport("__Internal", EntryPoint = "MarshalRcwQueryInterface")]
    static extern IntPtr MarshalRcwQueryInterface(IntPtr rcwNative, IntPtr iidBytes);

    [DllImport("__Internal", EntryPoint = "MarshalCallComMethod")]
    static extern int MarshalCallComMethod(IntPtr ptr, int slot, int a, int b);

    [DllImport("__Internal", EntryPoint = "MarshalCallDirectComMethod")]
    static extern int MarshalCallDirectComMethod(IntPtr comPtr, int slot, int a, int b);
}

internal static class RuntimeState
{
    static IntPtr _state;
    public static IntPtr Get() => _state;
    public static void Set(IntPtr state) => _state = state;
}

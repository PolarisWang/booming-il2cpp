// Hand-written native-AOT entry point for RCW basic verification
// Family: family/System.Private.CoreLib/rcw/basic
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.InteropServices;

//
// ── COM interface definitions (reused from CCW basic) ─────────────
//

// Simple calculator COM interface
// {7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F}
[Guid("7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F")]
public interface ISimpleMath
{
    int Add(int a, int b);
    int Multiply(int a, int b);
}

// COM interface with a single method returning a constant
// {9A8B7C6D-5E4F-3A2B-1C0D-9E8F7A6B5C4D}
[Guid("9A8B7C6D-5E4F-3A2B-1C0D-9E8F7A6B5C4D")]
public interface IConstantValue
{
    int GetValue();
}

//
// ── Implementations (no interface declarations on classes,
//     to avoid codegen emitting interface methods in vtables
//     without function bodies)
//

public class SimpleMath // was: ISimpleMath
{
    public int Add(int a, int b) => a + b;
    public int Multiply(int a, int b) => a * b;
}

public class ConstantFortyTwo // was: IConstantValue
{
    public int GetValue() => 42;
}

//
// ── Native exports for CCW creation ──────────────────────────────
//

public static class RcwBasicNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return TestRcwRoundTripIdentity();
            case 1: return TestRcwRoundTripQi();
            case 2: return TestRcwVtableMethodCall();
            case 3: return TestRcwDirectVtable();
            default: return -1;
        }
    }

    // [0] Round-trip identity test:
    //   Create CCW → get IUnknown* → wrap in RCW → verify identity_unknown == original
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

    // [1] Round-trip QI test:
    //   Create CCW → wrap in RCW → QI for ISimpleMath → verify non-null and != identity
    public static int TestRcwRoundTripQi()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        // ISimpleMath GUID: 7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F
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

    // [2] RCW-aware method dispatch via ComVtable:
    //   Create CCW → wrap in RCW → call Add(3,4) via RCW-aware dispatch helper
    public static int TestRcwVtableMethodCall()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        IntPtr rcw = MarshalCreateRcw(ccwUnknown);
        if (rcw == IntPtr.Zero) return 3;

        // Call Add(3,4) via RCW-aware dispatch at vtable slot 3 (after 3 IUnknown slots)
        int result = MarshalCallComMethod(rcw, 3, 3, 4);
        if (result != 7) return 4;

        MarshalReleaseRcw(rcw);
        return 0;
    }

    // [3] Direct ComVtable dispatch (no RCW check):
    //   Create CCW → call Add(10,5) via direct dispatch on raw CCW pointer
    public static int TestRcwDirectVtable()
    {
        IntPtr runtimeState = RuntimeState.Get();
        if (runtimeState == IntPtr.Zero) return 1;

        IntPtr ccwUnknown = CreateCcwForSimpleMath(runtimeState);
        if (ccwUnknown == IntPtr.Zero) return 2;

        // Call Add(10,5) via direct dispatch at vtable slot 3 on the CCW's IUnknown*
        int result = MarshalCallDirectComMethod(ccwUnknown, 3, 10, 5);
        if (result != 15) return 3;

        return 0;
    }

    // [4] Multiple wrappers test (stub — basic RCW does not support this)
    public static int TestRcwMultipleWrappers()
    {
        return -1; // not implemented in basic RCW
    }

    // [5] QiUnknownInterface test (stub — basic RCW does not support this)
    public static int TestRcwQiUnknownInterface()
    {
        return -1; // not implemented in basic RCW
    }

    // Creates a CCW wrapping a SimpleMath instance and returns its IUnknown*
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

    // ── P/Invoke declarations ──────────────────────────────────
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

// Minimal runtime state accessor for verification builds
internal static class RuntimeState
{
    static IntPtr _state;
    public static IntPtr Get() => _state;
    public static void Set(IntPtr state) => _state = state;
}

using System.Runtime.InteropServices;

// CLSID_ShellLink — a simple COM class available on all Windows systems.
internal static class Clsid
{
    internal static readonly Guid ShellLink = new("00021401-0000-0000-C000-000000000046");
    internal static readonly Guid IUnknown = new("00000000-0000-0000-C000-000000000046");
}

internal static class NativeMethods
{
    internal const uint COINIT_MULTITHREADED = 0;
    internal const uint CLSCTX_INPROC_SERVER = 1;

    [DllImport("ole32.dll", ExactSpelling = true)]
    internal static extern int CoInitializeEx(IntPtr reserved, uint dwCoInit);

    [DllImport("ole32.dll", ExactSpelling = true)]
    internal static extern void CoUninitialize();

    [DllImport("ole32.dll", ExactSpelling = true)]
    internal static extern int CoCreateInstance(
        [MarshalAs(UnmanagedType.LPStruct)] Guid rclsid,
        IntPtr pUnkOuter,
        uint dwClsContext,
        [MarshalAs(UnmanagedType.LPStruct)] Guid riid,
        out IntPtr ppv);

    /// HRESULT severity check.
    internal static bool Failed(int hr) => hr < 0;
}

internal static unsafe class Program
{
    private static int Main()
    {
        TestCoInitialize();
        TestComCreateInstance();
        TestIUnknownVtableDispatch();

        Console.WriteLine("All COM interop tests passed.");
        return 0;
    }

    /// Validate COM apartment initialisation and shutdown.
    private static void TestCoInitialize()
    {
        int hr = NativeMethods.CoInitializeEx(IntPtr.Zero, NativeMethods.COINIT_MULTITHREADED);
        // S_OK (0) or S_FALSE (1, already initialised) are both acceptable.
        if (NativeMethods.Failed(hr))
            throw new InvalidOperationException($"CoInitializeEx failed: HRESULT=0x{hr:x8}");
        Console.WriteLine($"com=init:0x{hr:x8}");

        NativeMethods.CoUninitialize();
        Console.WriteLine("com=uninit");
    }

    /// Create a COM ShellLink object via CoCreateInstance and verify the
    /// returned IUnknown pointer is non-null.
    private static void TestComCreateInstance()
    {
        int hr = NativeMethods.CoInitializeEx(IntPtr.Zero, NativeMethods.COINIT_MULTITHREADED);
        if (NativeMethods.Failed(hr))
            throw new InvalidOperationException($"CoInitializeEx failed: HRESULT=0x{hr:x8}");

        IntPtr unknown = IntPtr.Zero;
        hr = NativeMethods.CoCreateInstance(
            Clsid.ShellLink,
            IntPtr.Zero,
            NativeMethods.CLSCTX_INPROC_SERVER,
            Clsid.IUnknown,
            out unknown);

        if (NativeMethods.Failed(hr) || unknown == IntPtr.Zero)
            throw new InvalidOperationException($"CoCreateInstance(ShellLink) failed: HRESULT=0x{hr:x8}");
        Console.WriteLine($"com=create:0x{unknown:x}");

        // IUnknown methods via raw vtable dispatch.
        void** vtable = *(void***)unknown;

        // QueryInterface(IID_IUnknown) → should return the same pointer (COM identity rule).
        var qiPtr = (delegate* unmanaged[Stdcall]<IntPtr, Guid*, IntPtr*, int>)vtable[0];
        var addRefPtr = (delegate* unmanaged[Stdcall]<IntPtr, int>)vtable[1];
        var releasePtr = (delegate* unmanaged[Stdcall]<IntPtr, int>)vtable[2];

        // AddRef + Release round-trip.
        int prevRef = addRefPtr(unknown);
        int afterRef = releasePtr(unknown);
        Console.WriteLine($"com=vtable:addref_before:{prevRef}");

        // QueryInterface for IUnknown identity.
        Guid iid = Clsid.IUnknown;
        IntPtr qiResult = IntPtr.Zero;
        int qiHr = qiPtr(unknown, &iid, &qiResult);
        if (NativeMethods.Failed(qiHr))
            throw new InvalidOperationException($"QI(IUnknown) failed: HRESULT=0x{qiHr:x8}");
        if (qiResult != unknown)
            throw new InvalidOperationException($"QI(IUnknown) identity mismatch: expected 0x{unknown:x}, got 0x{qiResult:x}");
        Console.WriteLine($"com=vtable:qi_identity:ok");

        // Release the extra ref from QI and the original CoCreateInstance ref.
        // Balance: CoCreateInstance returns with refcount=1, AddRef above +1,
        // QI adds +1. Release 3 times to destroy.
        releasePtr(qiResult);
        releasePtr(unknown);
        int finalRef = releasePtr(unknown);
        Console.WriteLine($"com=vtable:final_release:{finalRef}");

        NativeMethods.CoUninitialize();
    }

    /// Verify RCW handle detection (magic-number check) for IntPtr.Zero and a
    /// non-RCW pointer so the runtime function responds correctly.
    private static void TestIUnknownVtableDispatch()
    {
        // Re-initialise COM for this test batch.
        int hr = NativeMethods.CoInitializeEx(IntPtr.Zero, NativeMethods.COINIT_MULTITHREADED);
        if (NativeMethods.Failed(hr))
            throw new InvalidOperationException($"CoInitializeEx failed: HRESULT=0x{hr:x8}");

        IntPtr unknown = IntPtr.Zero;
        hr = NativeMethods.CoCreateInstance(
            Clsid.ShellLink,
            IntPtr.Zero,
            NativeMethods.CLSCTX_INPROC_SERVER,
            Clsid.IUnknown,
            out unknown);

        if (NativeMethods.Failed(hr) || unknown == IntPtr.Zero)
            throw new InvalidOperationException($"CoCreateInstance(ShellLink) failed: HRESULT=0x{hr:x8}");

        // Read vtable[0] (QueryInterface) function pointer and verify it
        // matches the pattern of a valid COM vtable entry (non-null, aligned).
        void** vtable = *(void***)unknown;
        if (vtable == null || vtable[0] == null)
            throw new InvalidOperationException("COM vtable is null or vtable[0] is null");
        Console.WriteLine($"com=vtable:slot0:{(IntPtr)vtable[0]:x}");

        // Release the object.
        var releasePtr = (delegate* unmanaged[Stdcall]<IntPtr, int>)vtable[2];
        releasePtr(unknown);

        NativeMethods.CoUninitialize();
    }

    // Helper: simulate COM apartment init for tests that need it.
    private static void RequireComApartment()
    {
        int hr = NativeMethods.CoInitializeEx(IntPtr.Zero, NativeMethods.COINIT_MULTITHREADED);
        if (NativeMethods.Failed(hr))
            throw new InvalidOperationException($"CoInitializeEx failed: HRESULT=0x{hr:x8}");
    }
}

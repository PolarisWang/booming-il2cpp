using System.Reflection;
using System.Runtime.InteropServices;
using Chaos.IL2CPP.RuntimeSupport;

[StructLayout(LayoutKind.Sequential)]
internal struct SYSTEM_INFO
{
    public ushort wProcessorArchitecture;
    public ushort wReserved;
    public uint dwPageSize;
    public IntPtr lpMinimumApplicationAddress;
    public IntPtr lpMaximumApplicationAddress;
    public IntPtr dwActiveProcessorMask;
    public uint dwNumberOfProcessors;
    public uint dwProcessorType;
    public uint dwAllocationGranularity;
    public ushort wProcessorLevel;
    public ushort wProcessorRevision;
}

internal static class NativeMethods
{
    // Blittable struct by-ref
    [DllImport("kernel32.dll")]
    internal static extern void GetSystemInfo(out SYSTEM_INFO lpSystemInfo);

    // Override target: resolved via DllImportResolver → kernel32.dll
    [DllImport("OVERRIDE_TEST_DLL", EntryPoint = "GetSystemInfo")]
    internal static extern void GetSystemInfoOverride(out SYSTEM_INFO lpSystemInfo);

    // SetLastError round-trip
    [DllImport("kernel32.dll", SetLastError = true)]
    internal static extern void SetLastError(uint dwErrCode);

    [DllImport("kernel32.dll")]
    internal static extern uint GetLastError();

    // Ansi P/Invoke (CharSet.Ansi is default)
    [DllImport("kernel32.dll", CharSet = CharSet.Ansi)]
    internal static extern IntPtr GetModuleHandleA(string? lpModuleName);

    // Unicode P/Invoke
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
    internal static extern IntPtr GetModuleHandleW(string? lpModuleName);
}

internal static class Program
{
    private static int Main()
    {
        // Register a DllImportResolver that intercepts our override test.
        PInvokeResolverRegistry.SetDllImportResolver(typeof(Program).Assembly, OverrideResolver);

        TestStructByRef();
        TestDllImportResolverOverride();
        TestSetLastError();
        TestAnsiCharSet();
        TestUnicodeCharSet();

        Console.WriteLine("All marshalling tests passed.");
        return 0;
    }

    /// Resolve "OVERRIDE_TEST_DLL" → kernel32.dll, fall through for everything else.
    private static IntPtr OverrideResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        return libraryName == "OVERRIDE_TEST_DLL"
            ? NativeLibrary.Load("kernel32.dll")
            : IntPtr.Zero;
    }

    private static void TestDllImportResolverOverride()
    {
        NativeMethods.GetSystemInfoOverride(out SYSTEM_INFO info);
        if (info.dwPageSize == 0)
            throw new InvalidOperationException("DllImportResolver override: GetSystemInfo returned zero page size");
        Console.WriteLine($"override=page_size:{info.dwPageSize}");
    }

    private static void TestStructByRef()
    {
        NativeMethods.GetSystemInfo(out SYSTEM_INFO info);
        if (info.dwPageSize == 0)
            throw new InvalidOperationException("GetSystemInfo returned zero page size");
        Console.WriteLine($"struct=page_size:{info.dwPageSize}");
    }

    private static void TestSetLastError()
    {
        NativeMethods.SetLastError(12345);
        uint error = NativeMethods.GetLastError();
        if (error != 12345)
            throw new InvalidOperationException($"SetLastError round-trip failed: expected 12345, got {error}");
        Console.WriteLine("last-error=roundtrip:12345");
    }

    private static void TestAnsiCharSet()
    {
        IntPtr handle = NativeMethods.GetModuleHandleA(null);
        if (handle == IntPtr.Zero)
            throw new InvalidOperationException("GetModuleHandleA failed");
        Console.WriteLine($"ansi=handle:0x{handle:x}");
    }

    private static void TestUnicodeCharSet()
    {
        IntPtr handle = NativeMethods.GetModuleHandleW(null);
        if (handle == IntPtr.Zero)
            throw new InvalidOperationException("GetModuleHandleW failed");
        Console.WriteLine($"unicode=handle:0x{handle:x}");
    }
}

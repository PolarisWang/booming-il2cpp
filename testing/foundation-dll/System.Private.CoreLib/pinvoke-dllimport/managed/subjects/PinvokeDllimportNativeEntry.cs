// Hand-written native-AOT entry point for PInvoke DllImport verification
// Family: family/System.Private.CoreLib/pinvoke/dllimport
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.InteropServices;

public static class PinvokeDllimportNativeEntry
{
    // [0] Basic DllImport: GetCurrentProcess() always returns (HANDLE)-1
    [DllImport("kernel32")]
    static extern nint GetCurrentProcess();
    public static int TestGetCurrentProcess()
    {
        nint h = GetCurrentProcess();
        return h != -1 ? 1 : 0;
    }

    // [1] DllImport with explicit EntryPoint: GetCurrentThread() always returns (HANDLE)-2
    [DllImport("kernel32", EntryPoint = "GetCurrentThread")]
    static extern nint GetCurrentThread();
    public static int TestGetCurrentThread()
    {
        nint h = GetCurrentThread();
        return h != -2 ? 1 : 0;
    }

    // [2] DllImport with IntPtr parameter and uint return value
    [DllImport("kernel32", EntryPoint = "GetProcessId")]
    static extern uint GetProcessId(nint process);
    public static int TestGetProcessId()
    {
        nint h = GetCurrentProcess();
        uint pid = GetProcessId(h);
        return pid == 0 ? 1 : 0;  // Current process PID should be non-zero
    }

    // [3] DllImport with CharSet.Unicode and string parameter
    [DllImport("kernel32", CharSet = CharSet.Unicode, EntryPoint = "GetModuleHandleW")]
    static extern nint GetModuleHandle(string lpModuleName);
    public static int TestGetModuleHandle()
    {
        nint h = GetModuleHandle(null);
        return h == 0 ? 1 : 0;  // GetModuleHandle(NULL) returns base address of calling process
    }
}

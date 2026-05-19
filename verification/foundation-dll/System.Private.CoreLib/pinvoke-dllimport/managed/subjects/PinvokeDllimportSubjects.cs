// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/pinvoke/dllimport
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class PinvokeDllimportSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()
    public static void Subject_0()
    {
        try { if (PinvokeDllimportNativeEntry.TestGetCurrentProcess() != PinvokeDllimportNativeEntry.TestGetCurrentProcess()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()
    public static void Subject_1()
    {
        try { if (PinvokeDllimportNativeEntry.TestGetCurrentThread() != PinvokeDllimportNativeEntry.TestGetCurrentThread()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()
    public static void Subject_2()
    {
        try { if (PinvokeDllimportNativeEntry.TestGetProcessId() != PinvokeDllimportNativeEntry.TestGetProcessId()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()
    public static void Subject_3()
    {
        try { if (PinvokeDllimportNativeEntry.TestGetModuleHandle() != PinvokeDllimportNativeEntry.TestGetModuleHandle()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}
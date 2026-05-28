// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/pinvoke/dllimport
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class PinvokeDllimportSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()
    public static void Subject_0()
    {
        try { _ = PinvokeDllimportNativeEntry.TestGetCurrentProcess(); }
        catch { _exitCode = 1; }
    }

    // [1] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()
    public static void Subject_1()
    {
        try { _ = PinvokeDllimportNativeEntry.TestGetCurrentThread(); }
        catch { _exitCode = 1; }
    }

    // [2] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()
    public static void Subject_2()
    {
        try { _ = PinvokeDllimportNativeEntry.TestGetProcessId(); }
        catch { _exitCode = 1; }
    }

    // [3] PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()
    public static void Subject_3()
    {
        try { _ = PinvokeDllimportNativeEntry.TestGetModuleHandle(); }
        catch { _exitCode = 1; }
    }

}
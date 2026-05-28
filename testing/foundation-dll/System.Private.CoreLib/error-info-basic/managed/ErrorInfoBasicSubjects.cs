// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/error/info/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class ErrorInfoBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { _ = ErrorInfoBasicNativeEntry.Run(42); }
        catch { _exitCode = 1; }
    }

    // [1] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()
    public static void Subject_1()
    {
        try { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR(); }
        catch { _exitCode = 1; }
    }

    // [2] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()
    public static void Subject_2()
    {
        try { _ = ErrorInfoBasicNativeEntry.TestGetHRForException(); }
        catch { _exitCode = 1; }
    }

    // [3] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()
    public static void Subject_3()
    {
        try { _ = ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK(); }
        catch { _exitCode = 1; }
    }

    // [4] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()
    public static void Subject_4()
    {
        try { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero(); }
        catch { _exitCode = 1; }
    }

}
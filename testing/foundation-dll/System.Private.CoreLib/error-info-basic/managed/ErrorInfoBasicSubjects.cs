// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/error/info/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using ErrorInfoBasicNativeEntry;
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
        try { if (ErrorInfoBasicNativeEntry.Run(42) != ErrorInfoBasicNativeEntry.Run(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()
    public static void Subject_1()
    {
        try { if (ErrorInfoBasicNativeEntry.TestGetExceptionForHR() != ErrorInfoBasicNativeEntry.TestGetExceptionForHR()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()
    public static void Subject_2()
    {
        try { if (ErrorInfoBasicNativeEntry.TestGetHRForException() != ErrorInfoBasicNativeEntry.TestGetHRForException()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()
    public static void Subject_3()
    {
        try { if (ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK() != ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()
    public static void Subject_4()
    {
        try { if (ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero() != ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}
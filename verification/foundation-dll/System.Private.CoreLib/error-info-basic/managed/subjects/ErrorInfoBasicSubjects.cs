// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/error/info/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        if (ErrorInfoBasicNativeEntry.Run(42) != -1) _exitCode = 1;
    }

    // [1] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()
    public static void Subject_1()
    {
        if (ErrorInfoBasicNativeEntry.TestGetExceptionForHR() != 3) _exitCode = 1;
    }

    // [2] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()
    public static void Subject_2()
    {
        if (ErrorInfoBasicNativeEntry.TestGetHRForException() != 0) _exitCode = 1;
    }

    // [3] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()
    public static void Subject_3()
    {
        if (ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK() != 0) _exitCode = 1;
    }

    // [4] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()
    public static void Subject_4()
    {
        if (ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero() != 0) _exitCode = 1;
    }

}
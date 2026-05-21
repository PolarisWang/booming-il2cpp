// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/token/access
// Assembly: System.Security.Principal.Windows
// Variant: subjects

using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class TokenAccessSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_InvalidHandle:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()
    public static void Subject_0()
    {
        if (((SafeAccessTokenHandle.InvalidHandle).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [1] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_IsInvalid:System.Boolean()
    public static void Subject_1()
    {
        try { _ = default(SafeAccessTokenHandle)!.IsInvalid; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::.ctor:System.Void()
    public static void Subject_2()
    {
        new SafeAccessTokenHandle();
    }

    // [3] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::.ctor:System.Void(System.IntPtr)
    public static void Subject_3()
    {
        new SafeAccessTokenHandle(IntPtr.Zero);
    }

}
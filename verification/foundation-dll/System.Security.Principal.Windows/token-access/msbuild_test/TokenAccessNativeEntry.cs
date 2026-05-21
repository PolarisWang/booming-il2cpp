// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/token-access
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class TokenAccessNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_InvalidHandle:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()
    public static void Method0()
    {
        try { if ((int)(SafeAccessTokenHandle.InvalidHandle) != (int)(SafeAccessTokenHandle.InvalidHandle)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_IsInvalid:System.Boolean()
    public static void Method1()
    {
        try { if (((SafeAccessTokenHandle.IsInvalid) ? 1 : 0) != ((SafeAccessTokenHandle.IsInvalid) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::.ctor:System.Void()
    public static void Method2()
    {
        try { new SafeAccessTokenHandle(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::.ctor:System.Void(System.IntPtr)
    public static void Method3()
    {
        try { new SafeAccessTokenHandle(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}
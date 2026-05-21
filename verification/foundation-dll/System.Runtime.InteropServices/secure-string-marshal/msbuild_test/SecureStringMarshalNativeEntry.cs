// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/secure-string-marshal
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security;

public static partial class SecureStringMarshalNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)
    public static void Method0()
    {
        try { if ((int)(SecureStringMarshal.SecureStringToCoTaskMemAnsi(null!)) != (int)(SecureStringMarshal.SecureStringToCoTaskMemAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)
    public static void Method1()
    {
        try { if ((int)(SecureStringMarshal.SecureStringToCoTaskMemUnicode(null!)) != (int)(SecureStringMarshal.SecureStringToCoTaskMemUnicode(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)
    public static void Method2()
    {
        try { if ((int)(SecureStringMarshal.SecureStringToGlobalAllocAnsi(null!)) != (int)(SecureStringMarshal.SecureStringToGlobalAllocAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)
    public static void Method3()
    {
        try { if ((int)(SecureStringMarshal.SecureStringToGlobalAllocUnicode(null!)) != (int)(SecureStringMarshal.SecureStringToGlobalAllocUnicode(null!))) _exitCode = 1; }
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
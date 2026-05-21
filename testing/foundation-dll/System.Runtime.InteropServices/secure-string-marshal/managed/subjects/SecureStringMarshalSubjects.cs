// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/secure/string/marshal
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security;

public static partial class SecureStringMarshalSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_0()
    {
        try { SecureStringMarshal.SecureStringToCoTaskMemAnsi(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [1] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_1()
    {
        try { SecureStringMarshal.SecureStringToCoTaskMemUnicode(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [2] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_2()
    {
        try { SecureStringMarshal.SecureStringToGlobalAllocAnsi(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [3] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_3()
    {
        try { SecureStringMarshal.SecureStringToGlobalAllocUnicode(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}
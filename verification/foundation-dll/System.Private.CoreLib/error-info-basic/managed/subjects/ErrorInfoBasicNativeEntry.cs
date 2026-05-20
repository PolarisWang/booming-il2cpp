// Hand-written native-AOT entry point for COM ErrorInfo basic verification
// Family: family/System.Private.CoreLib/error/info/basic
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.InteropServices;

//
// -- Entry point --
//

public static class ErrorInfoBasicNativeEntry
{
    // Test methods: each returns an int checksum (0 = pass)
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return TestGetExceptionForHR();
            case 1: return TestGetHRForException();
            case 2: return TestThrowExceptionForHR_S_OK();
            case 3: return TestGetExceptionForHR_Zero();
            default: return -1;
        }
    }

    // [0] Marshal.GetExceptionForHR with E_FAIL (0x80004005)
    // Should return a non-null Exception with HRESULT in the message.
    public static int TestGetExceptionForHR()
    {
        int hr = unchecked((int)0x80004005); // E_FAIL
        Exception? ex = Marshal.GetExceptionForHR(hr);
        if (ex == null) return 1; // expected non-null
        if (string.IsNullOrEmpty(ex.Message)) return 2; // expected message
        // The message should contain the HRESULT hex string
        string msg = ex.Message;
        if (!msg.Contains("0x80004005", StringComparison.OrdinalIgnoreCase)
            && !msg.Contains("80004005", StringComparison.OrdinalIgnoreCase))
            return 3; // expected HRESULT in message
        return 0; // pass
    }

    // [1] Marshal.GetHRForException round-trip
    // Create an Exception, set HResult, read it back via Marshal.GetHRForException.
    public static int TestGetHRForException()
    {
        int hr = unchecked((int)0x80004001); // E_NOTIMPL
        var ex = new Exception("test error");
        // Use reflection or the _HResult field to set the HResult
        // In .NET, the HResult property is settable via reflection
        try
        {
            typeof(Exception).GetField("_HResult", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)
                ?.SetValue(ex, hr);
        }
        catch
        {
            // Fallback: try the HResult property (available in .NET 5+)
            try
            {
                typeof(Exception).GetProperty("HResult", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)
                    ?.SetValue(ex, hr);
            }
            catch
            {
                return 2; // could not set HResult
            }
        }

        int actualHr = Marshal.GetHRForException(ex);
        if (actualHr != hr) return 1; // expected HRESULT match
        return 0; // pass
    }

    // [2] Marshal.ThrowExceptionForHR with S_OK (0)
    // Should be a no-op and not throw.
    public static int TestThrowExceptionForHR_S_OK()
    {
        try
        {
            Marshal.ThrowExceptionForHR(0);
            return 0; // pass - no exception thrown for S_OK
        }
        catch
        {
            return 1; // should not throw for S_OK
        }
    }

    // [3] Marshal.GetExceptionForHR with 0 (S_OK)
    // Should return null since S_OK indicates success.
    public static int TestGetExceptionForHR_Zero()
    {
        Exception? ex = Marshal.GetExceptionForHR(0);
        if (ex != null) return 1; // expected null for S_OK
        return 0; // pass
    }
}

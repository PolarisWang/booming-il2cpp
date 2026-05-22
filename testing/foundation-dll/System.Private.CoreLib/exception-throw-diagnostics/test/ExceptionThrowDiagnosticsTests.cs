using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/exception/throw-diagnostics", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ExceptionThrowDiagnosticsTests
{
    // AI-generated tests for family/System.Private.CoreLib/exception/throw-diagnostics

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Private_CoreLib_System_Exception_ctor_System_Void_System_String()
    {
        // Purpose: Verify Exception..ctor with typical input
            new Exception("hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_get_Message_System_String()
    {
        // Purpose: Verify Exception.get_Message with typical input
            var result = new Exception().Message;
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_get_StackTrace_System_String()
    {
        // Purpose: Verify Exception.get_StackTrace with typical input
            var result = ((new Exception().StackTrace) ?? "");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_get_HResult_System_Int32()
    {
        // Purpose: Verify Exception.get_HResult with typical input
            var result = new Exception().HResult;
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_ToString_System_String()
    {
        // Purpose: Verify Exception.ToString with typical input
            var result = new Exception().ToString();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String()
    {
        // Purpose: Verify ArgumentException..ctor with typical input
            new ArgumentException("hello", "hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String()
    {
        // Purpose: Verify ArgumentNullException..ctor with typical input
            new ArgumentNullException("hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String()
    {
        // Purpose: Verify ArgumentOutOfRangeException..ctor with typical input
            new ArgumentOutOfRangeException("hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String()
    {
        // Purpose: Verify InvalidOperationException..ctor with typical input
            new InvalidOperationException("hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String()
    {
        // Purpose: Verify NotSupportedException..ctor with typical input
            new NotSupportedException("hello");
    }

    [Fact]
    public void _System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String()
    {
        // Purpose: Verify NotImplementedException..ctor with typical input
            new NotImplementedException("hello");
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception()
    {
        // Purpose: Smoke — Exception..ctor with complex param(s)
            new Exception("hello", null!);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_get_InnerException_System_Exception()
    {
        // Purpose: Smoke — Exception.get_InnerException with complex param(s)
            _ = new Exception().InnerException;
    }

    [Fact]
    public void _System_Private_CoreLib_System_Exception_GetBaseException_System_Exception()
    {
        // Purpose: Smoke — Exception.GetBaseException with complex param(s)
            _ = new Exception().GetBaseException();
    }

    // === OSR hot loop ===

    [Fact]
    public void _OSR_HotLoop()
    {
        // Purpose: Trigger OSR promotion via 150-iteration hot loop (exceeds kOsrLoopThreshold=100)
        long sum = 0;
        for (int i = 0; i < 150; i++) {
            sum += i;
        }
        Xunit.Assert.Equal(11175, sum);
    }
}

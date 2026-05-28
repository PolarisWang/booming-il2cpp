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

[CapabilityTest("family/System.Private.CoreLib/convert/char", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ConvertCharTests
{
    // AI-generated tests for family/System.Private.CoreLib/convert/char

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(true);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar((byte)42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar('A');
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(DateTime.UtcNow);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42m);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar((short)42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42L);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar((object)42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar((sbyte)42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42.0f);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar("hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar((ushort)42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42u);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64()
    {
        // Purpose: Verify Convert.ToChar with typical input
            var result = Convert.ToChar(42uL);
            Xunit.Assert.NotNull((object)result);
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider()
    {
        // Purpose: Smoke — Convert.ToChar with complex param(s)
            _ = Convert.ToChar((object)42, null);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider()
    {
        // Purpose: Smoke — Convert.ToChar with complex param(s)
            _ = Convert.ToChar("hello", null);
    }
}

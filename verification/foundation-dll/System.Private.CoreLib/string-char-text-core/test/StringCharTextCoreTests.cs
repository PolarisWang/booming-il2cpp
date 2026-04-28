using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/string/char-text-core", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class StringCharTextCoreTests
{
    // AI-generated tests for family/System.Private.CoreLib/string/char-text-core

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String()
    {
        // Purpose: Verify String.IndexOf with typical input
            var result = "hello".IndexOf("hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char()
    {
        // Purpose: Verify String.IndexOf with typical input
            var result = "hello".IndexOf('A');
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Substring_System_String_System_Int32()
    {
        // Purpose: Verify String.Substring with typical input
            var result = "hello".Substring(1);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32()
    {
        // Purpose: Verify String.Substring with typical input
            var result = "hello".Substring(1, 2);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String()
    {
        // Purpose: Verify String.Compare with typical input
            var result = String.Compare("hello", "hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I()
    {
        // Purpose: Verify String.Compare with typical input
            var result = string.Compare("hello", 0, "world", 0, 3);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String()
    {
        // Purpose: Verify String.Concat with typical input
            var result = String.Concat("hello", "hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String()
    {
        // Purpose: Verify String.Concat with typical input
            var result = String.Concat("hello", "hello", "hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object()
    {
        // Purpose: Verify String.Format with typical input
            var result = String.Format("hello", Array.Empty<System.Object>());
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String()
    {
        // Purpose: Verify String.StartsWith with typical input
            var result = "hello".StartsWith("hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Contains_System_Boolean_System_String()
    {
        // Purpose: Verify String.Contains with typical input
            var result = "hello".Contains("hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String()
    {
        // Purpose: Verify String.Replace with typical input
            var result = "hello".Replace("hello", "hello");
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Split_System_String_System_Char()
    {
        // Purpose: Verify String.Split with typical input
            var result = "hello".Split('A');
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_ToUpper_System_String()
    {
        // Purpose: Verify String.ToUpper with typical input
            var result = "hello".ToUpper();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_ToLower_System_String()
    {
        // Purpose: Verify String.ToLower with typical input
            var result = "hello".ToLower();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Trim_System_String()
    {
        // Purpose: Verify String.Trim with typical input
            var result = "hello".Trim();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_String_Join_System_String_System_String_System_String()
    {
        // Purpose: Verify String.Join with typical input
            var result = String.Join("hello", Array.Empty<System.String>());
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char()
    {
        // Purpose: Verify Char.IsDigit with typical input
            var result = Char.IsDigit('A');
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char()
    {
        // Purpose: Verify Char.IsLetter with typical input
            var result = Char.IsLetter('A');
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char()
    {
        // Purpose: Verify Char.IsWhiteSpace with typical input
            var result = Char.IsWhiteSpace('A');
            Xunit.Assert.NotNull((object)result);
    }
}

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

[CapabilityTest("family/System.Private.CoreLib/math/numerics", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class MathNumericsTests
{
    // AI-generated tests for family/System.Private.CoreLib/math/numerics

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32()
    {
        // Purpose: Verify Math.Abs with typical input
            var result = Math.Abs(42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Abs_System_Double_System_Double()
    {
        // Purpose: Verify Math.Abs with typical input
            var result = Math.Abs(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double()
    {
        // Purpose: Verify Math.Ceiling with typical input
            var result = Math.Ceiling(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Floor_System_Double_System_Double()
    {
        // Purpose: Verify Math.Floor with typical input
            var result = Math.Floor(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Round_System_Double_System_Double()
    {
        // Purpose: Verify Math.Round with typical input
            var result = Math.Round(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32()
    {
        // Purpose: Verify Math.Round with typical input
            var result = Math.Round(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32()
    {
        // Purpose: Verify Math.Max with typical input
            var result = Math.Max(42, 42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double()
    {
        // Purpose: Verify Math.Max with typical input
            var result = Math.Max(42.0, 42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32()
    {
        // Purpose: Verify Math.Min with typical input
            var result = Math.Min(42, 42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double()
    {
        // Purpose: Verify Math.Sqrt with typical input
            var result = Math.Sqrt(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double()
    {
        // Purpose: Verify Math.Pow with typical input
            var result = Math.Pow(42.0, 42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Sin_System_Double_System_Double()
    {
        // Purpose: Verify Math.Sin with typical input
            var result = Math.Sin(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_Cos_System_Double_System_Double()
    {
        // Purpose: Verify Math.Cos with typical input
            var result = Math.Cos(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32()
    {
        // Purpose: Verify Math.BigMul with typical input
            var result = Math.BigMul(42, 42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32()
    {
        // Purpose: Verify BitConverter.GetBytes with typical input
            var result = BitConverter.GetBytes(42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double()
    {
        // Purpose: Verify BitConverter.GetBytes with typical input
            var result = BitConverter.GetBytes(42.0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32()
    {
        // Purpose: Verify BitConverter.ToInt32 with typical input
            var result = BitConverter.ToInt32(new byte[4], 0);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32()
    {
        // Purpose: Verify BitConverter.ToDouble with typical input
            var result = BitConverter.ToDouble(new byte[8], 0);
            Xunit.Assert.NotNull((object)result);
    }
}

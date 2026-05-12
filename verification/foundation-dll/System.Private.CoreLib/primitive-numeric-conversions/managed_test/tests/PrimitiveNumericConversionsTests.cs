using System;
using Xunit;

namespace PrimitiveNumericConversionsTests;

public class PrimitiveNumericConversionsTests
{
    [Fact]
    public void ToBoolean_String_ReturnsExpected()
    {
        bool result = Convert.ToBoolean("true");
        Assert.True(result);
    }

    [Fact]
    public void ToBoolean_StringFalse_ReturnsFalse()
    {
        bool result = Convert.ToBoolean("false");
        Assert.False(result);
    }

    [Fact]
    public void ToByte_String_ReturnsExpected()
    {
        byte result = Convert.ToByte("42");
        Assert.Equal((byte)42, result);
    }

    [Fact]
    public void ToByte_StringMax_ReturnsExpected()
    {
        byte result = Convert.ToByte("255");
        Assert.Equal(byte.MaxValue, result);
    }

    [Fact]
    public void ToInt16_String_ReturnsExpected()
    {
        short result = Convert.ToInt16("42");
        Assert.Equal((short)42, result);
    }

    [Fact]
    public void ToInt16_StringNegative_ReturnsExpected()
    {
        short result = Convert.ToInt16("-32768");
        Assert.Equal(short.MinValue, result);
    }

    [Fact]
    public void ToInt32_String_ReturnsExpected()
    {
        int result = Convert.ToInt32("42");
        Assert.Equal(42, result);
    }

    [Fact]
    public void ToInt32_StringNegative_ReturnsExpected()
    {
        int result = Convert.ToInt32("-42");
        Assert.Equal(-42, result);
    }

    [Fact]
    public void ToInt64_String_ReturnsExpected()
    {
        long result = Convert.ToInt64("42");
        Assert.Equal(42L, result);
    }

    [Fact]
    public void ToInt64_StringLarge_ReturnsExpected()
    {
        long result = Convert.ToInt64("9223372036854775807");
        Assert.Equal(long.MaxValue, result);
    }

    [Fact]
    public void ToSingle_String_ReturnsExpected()
    {
        float result = Convert.ToSingle("42.5");
        Assert.Equal(42.5f, result);
    }

    [Fact]
    public void ToDouble_String_ReturnsExpected()
    {
        double result = Convert.ToDouble("42.5");
        Assert.Equal(42.5, result);
    }

    [Fact]
    public void ToDecimal_String_ReturnsExpected()
    {
        decimal result = Convert.ToDecimal("42.5");
        Assert.Equal(42.5m, result);
    }

    [Fact]
    public void ToString_Int32_ReturnsExpected()
    {
        string result = Convert.ToString(42);
        Assert.Equal("42", result);
    }

    [Fact]
    public void ToString_Int32Negative_ReturnsExpected()
    {
        string result = Convert.ToString(-42);
        Assert.Equal("-42", result);
    }

    [Fact]
    public void ToString_Double_ReturnsExpected()
    {
        string result = Convert.ToString(42.5);
        Assert.Equal("42.5", result);
    }

    [Fact]
    public void ToDecimal_Double_ReturnsExpected()
    {
        decimal result = Convert.ToDecimal(42.5);
        Assert.Equal(42.5m, result);
    }

    [Fact]
    public void ToInt32_Double_ReturnsExpected()
    {
        int result = Convert.ToInt32(42.5);
        Assert.Equal(42, result);
    }

    [Fact]
    public void ToInt32_DoubleNegative_ReturnsExpected()
    {
        int result = Convert.ToInt32(-42.5);
        Assert.Equal(-42, result);
    }

    [Fact]
    public void Int32_Parse_ReturnsExpected()
    {
        int result = int.Parse("42");
        Assert.Equal(42, result);
    }

    [Fact]
    public void Int32_ParseNegative_ReturnsExpected()
    {
        int result = int.Parse("-42");
        Assert.Equal(-42, result);
    }

    [Fact]
    public void Int64_Parse_ReturnsExpected()
    {
        long result = long.Parse("42");
        Assert.Equal(42L, result);
    }

    [Fact]
    public void Int64_ParseLarge_ReturnsExpected()
    {
        long result = long.Parse("9223372036854775807");
        Assert.Equal(long.MaxValue, result);
    }

    [Fact]
    public void Double_Parse_ReturnsExpected()
    {
        double result = double.Parse("42.5");
        Assert.Equal(42.5, result);
    }

    [Fact]
    public void Double_ParseNegative_ReturnsExpected()
    {
        double result = double.Parse("-42.5");
        Assert.Equal(-42.5, result);
    }
}
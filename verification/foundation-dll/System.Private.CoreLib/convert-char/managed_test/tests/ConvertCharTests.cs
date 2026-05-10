using System;
using Xunit;

namespace ConvertCharTests;

public class ConvertCharTests
{
    [Fact]
    public void ToChar_Byte_ReturnsExpected()
    {
        char result = Convert.ToChar((byte)65);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_SByte_ReturnsExpected()
    {
        char result = Convert.ToChar((sbyte)65);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Int16_ReturnsExpected()
    {
        char result = Convert.ToChar((short)65);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_UInt16_ReturnsExpected()
    {
        char result = Convert.ToChar((ushort)65);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Int32_ReturnsExpected()
    {
        char result = Convert.ToChar(65);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_UInt32_ReturnsExpected()
    {
        char result = Convert.ToChar(65u);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Int64_ReturnsExpected()
    {
        char result = Convert.ToChar(65L);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_UInt64_ReturnsExpected()
    {
        char result = Convert.ToChar(65UL);
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Char_ReturnsIdentity()
    {
        char result = Convert.ToChar('A');
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Object_ReturnsExpected()
    {
        char result = Convert.ToChar((object)'A');
        Assert.Equal('A', result);
    }

    [Fact]
    public void ToChar_Boolean_ThrowsInvalidCast()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(true));
    }

    [Fact]
    public void ToChar_DateTime_ThrowsInvalidCast()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(DateTime.Now));
    }

    [Fact]
    public void ToChar_Decimal_ThrowsInvalidCast()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(65m));
    }

    [Fact]
    public void ToChar_Double_ThrowsInvalidCast()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(65.0));
    }

    [Fact]
    public void ToChar_Single_ThrowsInvalidCast()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(65.0f));
    }
}
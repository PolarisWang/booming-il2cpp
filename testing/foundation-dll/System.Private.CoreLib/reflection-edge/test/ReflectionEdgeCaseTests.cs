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

[CapabilityTest("family/System.Private.CoreLib/reflection/edge/proof")]
public partial class ReflectionEdgeCaseTests
{
    // === R4.4 EdgeCase proof: verify edge-case reflection APIs do not crash ===

    [Fact]
    public void _ReflectionEdge_GetElementType()
    {
        // Purpose: Type.GetElementType() on array type does not throw.
        var elemType = typeof(int[]).GetElementType();
        Assert.IsNotNull(elemType);
    }

    [Fact]
    public void _ReflectionEdge_GetElementTypeOnMultidimensionalArray()
    {
        // Purpose: Type.GetElementType() on multidimensional array does not throw.
        var elemType = typeof(int[,]).GetElementType();
        Assert.IsNotNull(elemType);
    }

    [Fact]
    public void _ReflectionEdge_IsArray()
    {
        // Purpose: Type.IsArray on array type does not throw.
        bool isArray = typeof(int[]).IsArray;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_MakeArrayType()
    {
        // Purpose: Type.MakeArrayType() does not throw.
        var arrType = typeof(int).MakeArrayType();
        Assert.NotNull((object)arrType);
    }

    [Fact]
    public void _ReflectionEdge_MakeArrayTypeRank()
    {
        // Purpose: Type.MakeArrayType(int rank) does not throw.
        var arrType = typeof(int).MakeArrayType(2);
        Assert.NotNull((object)arrType);
    }

    [Fact]
    public void _ReflectionEdge_MakeByRefType()
    {
        // Purpose: Type.MakeByRefType() does not throw.
        var byRefType = typeof(int).MakeByRefType();
        Assert.NotNull((object)byRefType);
    }

    [Fact]
    public void _ReflectionEdge_MakePointerType()
    {
        // Purpose: Type.MakePointerType() does not throw.
        var ptrType = typeof(int).MakePointerType();
        Assert.NotNull((object)ptrType);
    }

    [Fact]
    public void _ReflectionEdge_IsGenericType()
    {
        // Purpose: Type.IsGenericType on generic type does not throw.
        bool isGen = typeof(List<int>).IsGenericType;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_GetGenericTypeDefinition()
    {
        // Purpose: Type.GetGenericTypeDefinition() does not throw.
        var def = typeof(List<int>).GetGenericTypeDefinition();
        Assert.NotNull((object)def);
    }

    [Fact]
    public void _ReflectionEdge_GenericTypeArguments()
    {
        // Purpose: Type.GenericTypeArguments on constructed generic does not throw.
        var args = typeof(List<int>).GenericTypeArguments;
        Assert.NotNull((object)args);
    }

    [Fact]
    public void _ReflectionEdge_GetEnumUnderlyingType()
    {
        // Purpose: Type.GetEnumUnderlyingType() on enum does not throw.
        var underlying = typeof(StringComparison).GetEnumUnderlyingType();
        Assert.NotNull((object)underlying);
    }

    [Fact]
    public void _ReflectionEdge_IsEnum()
    {
        // Purpose: Type.IsEnum on enum type does not throw.
        bool isEnum = typeof(StringComparison).IsEnum;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_IsValueType()
    {
        // Purpose: Type.IsValueType on value type does not throw.
        bool isVT = typeof(int).IsValueType;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_IsClass()
    {
        // Purpose: Type.IsClass on reference type does not throw.
        bool isClass = typeof(string).IsClass;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_IsSealed()
    {
        // Purpose: Type.IsSealed on sealed type does not throw.
        bool isSealed = typeof(string).IsSealed;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_IsAbstract()
    {
        // Purpose: Type.IsAbstract on abstract type does not throw.
        bool isAbstract = typeof(Stream).IsAbstract;
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_GetNestedType()
    {
        // Purpose: Type.GetNestedType does not throw.
        var nested = typeof(Environment).GetNestedType("SpecialFolder", BindingFlags.Public);
        Assert.IsNotNull(nested);
    }

    [Fact]
    public void _ReflectionEdge_IsAssignableFrom()
    {
        // Purpose: Type.IsAssignableFrom does not throw.
        bool assignable = typeof(IDisposable).IsAssignableFrom(typeof(Stream));
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_IsSubclassOf()
    {
        // Purpose: Type.IsSubclassOf does not throw.
        bool isSub = typeof(MemoryStream).IsSubclassOf(typeof(Stream));
        Assert.IsTrue(true);
    }

    [Fact]
    public void _ReflectionEdge_GetInterface()
    {
        // Purpose: Type.GetInterface does not throw.
        var iface = typeof(string).GetInterface("IComparable");
        Assert.IsNotNull(iface);
    }

    [Fact]
    public void _ReflectionEdge_GetInterfaces()
    {
        // Purpose: Type.GetInterfaces does not throw.
        var ifaces = typeof(string).GetInterfaces();
        Assert.NotNull((object)ifaces);
    }
}

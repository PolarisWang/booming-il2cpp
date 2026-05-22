using System.Reflection;
using System.Text;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Tests for additional private static utility methods in NativeAotLoweringPlanner
/// that are currently uncovered. Uses reflection to invoke private methods.
/// </summary>
public sealed class PlannerUtilityCoverageTests
{
    private static readonly BindingFlags s_static = BindingFlags.NonPublic | BindingFlags.Static;
    private static readonly Type s_plannerType = typeof(NativeAotLoweringPlanner);

    #region GetStructFieldKindValue (ObjectModelEmission.cs)

    [Theory]
    [InlineData("Blittable", 0)]
    [InlineData("BoolField", 1)]
    [InlineData("StringField", 2)]
    [InlineData("NestedStruct", 3)]
    [InlineData("ByValArray", 4)]
    [InlineData("LPArray", 5)]
    [InlineData("DecimalField", 6)]
    [InlineData("DateTimeField", 7)]
    [InlineData("ObjectField", 8)]
    [InlineData("GuidField", 9)]
    [InlineData("UnknownKind", 0)] // fallback
    public void GetStructFieldKindValue_ReturnsExpected(string kind, int expected)
    {
        var method = s_plannerType.GetMethod("GetStructFieldKindValue", s_static, new[] { typeof(string) })!;
        var result = (int)method.Invoke(null, new object[] { kind })!;
        Assert.Equal(expected, result);
    }

    #endregion

    #region GetNativeElementTypeValue (ObjectModelEmission.cs)

    [Theory]
    [InlineData("None", 0)]
    [InlineData("U1", 1)]
    [InlineData("I1", 2)]
    [InlineData("U2", 3)]
    [InlineData("I2", 4)]
    [InlineData("U4", 5)]
    [InlineData("I4", 6)]
    [InlineData("U8", 7)]
    [InlineData("I8", 8)]
    [InlineData("R4", 9)]
    [InlineData("R8", 10)]
    [InlineData("Struct", 11)]
    [InlineData("Void", 0)] // fallback
    [InlineData(null, 0)]   // null input
    public void GetNativeElementTypeValue_ReturnsExpected(string? elementType, int expected)
    {
        var method = s_plannerType.GetMethod("GetNativeElementTypeValue", s_static, new[] { typeof(string) })!;
        var result = (int)method.Invoke(null, new object[] { elementType! })!;
        Assert.Equal(expected, result);
    }

    #endregion

    #region CustomAttributeFieldSize (ModuleRegistration.cs)

    [Theory]
    [InlineData("Null", 0u)]
    [InlineData("Boolean", 1u)]
    [InlineData("Byte", 1u)]
    [InlineData("SByte", 1u)]
    [InlineData("Int16", 2u)]
    [InlineData("Int32", 4u)]
    [InlineData("Int64", 8u)]
    [InlineData("UInt16", 2u)]
    [InlineData("UInt32", 4u)]
    [InlineData("UInt64", 8u)]
    [InlineData("Single", 4u)]
    [InlineData("Double", 8u)]
    [InlineData("Char", 2u)]
    [InlineData("String", 2u)]
    [InlineData("Type", 2u)]
    [InlineData("Enum", 1u)]
    public void CustomAttributeFieldSize_AllEnumValues_ReturnsExpected(string kindName, uint expected)
    {
        var enumType = s_plannerType.GetNestedType("CustomAttributeLiteralKind", BindingFlags.NonPublic)!;
        var enumValue = Enum.Parse(enumType, kindName);
        var method = s_plannerType.GetMethod("CustomAttributeFieldSize", s_static, new[] { enumType })!;
        var result = (uint)method.Invoke(null, new object[] { enumValue })!;
        Assert.Equal(expected, result);
    }

    #endregion

    #region TryMatchConstructedGenericInterface (InvocationPlanning.cs)

    [Theory]
    [InlineData("MyAssembly/System.Collections.Generic.IList`1<System.Int32>", "System.Collections.Generic.IList`1<", "System.Int32", true)]
    [InlineData("MyAssembly/System.Collections.Generic.IList`1<System.Int32>", "System.Collections.Generic.IDictionary`2<", "System.Int32", false)]
    [InlineData("invalid-subject-id", "System.Collections.Generic.IList`1<", "System.Int32", false)]
    public void TryMatchConstructedGenericInterface_ReturnsExpected(
        string interfaceSubjectId, string prefix, string elementTypeDisplayName, bool expected)
    {
        var method = s_plannerType.GetMethod("TryMatchConstructedGenericInterface", s_static,
            new[] { typeof(string), typeof(string), typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { interfaceSubjectId, prefix, elementTypeDisplayName })!;
        Assert.Equal(expected, result);
    }

    #endregion

    #region TryGetAsyncStateMachineTypeName (TypeResolution.cs)

    [Theory]
    [InlineData("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Threading.Tasks.VoidTaskResult>::Start<MyType>(!!0&)", "MyType")]
    [InlineData("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Threading.Tasks.VoidTaskResult>::Start<MyType>(!!0&)", "MyType")]
    [InlineData("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Threading.Tasks.VoidTaskResult>::AwaitUnsafeOnCompleted<TaskAwaiter, MyType>(!!0&, !!1&)", "MyType")]
    [InlineData("simple-method", null)] // no state machine
    public void TryGetAsyncStateMachineTypeName_ReturnsExpected(string callee, string? expectedName)
    {
        var method = s_plannerType.GetMethod("TryGetAsyncStateMachineTypeName", s_static,
            new[] { typeof(string), typeof(string).MakeByRefType() })!;
        var parameters = new object?[] { callee, null };
        var result = (bool)method.Invoke(null, parameters)!;
        if (expectedName is not null)
        {
            Assert.True(result);
            Assert.Equal(expectedName, (string?)parameters[1]);
        }
        else
        {
            Assert.False(result);
        }
    }

    #endregion

    #region AppendStaticInitializationCall (StaticInitializationEmission.cs)

    [Fact]
    public void AppendStaticInitializationCall_AppendsCorrectly()
    {
        var method = s_plannerType.GetMethod("AppendStaticInitializationCall", s_static,
            new[] { typeof(StringBuilder), typeof(string), typeof(string) })!;
        var sb = new StringBuilder();
        method.Invoke(null, new object[] { sb, "my_init_function", "    " });
        Assert.Contains("my_init_function", sb.ToString());
        Assert.Contains("    ", sb.ToString());
    }

    #endregion
}

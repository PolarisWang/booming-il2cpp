using System.Collections.Generic;
using System.Reflection;
using System.Reflection.Metadata;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Tests for small uncovered static methods in NativeAotLoweringPlanner partial classes.
/// Each method is tested via reflection to exercise specific code paths.
/// </summary>
public sealed class NativeAotPlannerHelperTests
{
    private static readonly BindingFlags s_static = BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Public | BindingFlags.Instance;
    private static readonly Type s_plannerType = typeof(NativeAotLoweringPlanner);
    private static readonly Type s_aotCoreIrLoweringType = typeof(AotCoreIrLowering);

    // ── MatchesMethodSubject (CollectionAndReflection.cs) ──────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.String", "Concat", "System.String", "System.String")]
    [InlineData("System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)", "System.GC", "KeepAlive", "System.Object")]
    [InlineData("System.Private.CoreLib/System.String::get_Length:System.Int32()", "System.String", "get_Length")]
    [InlineData("System.Private.CoreLib/System.Environment::get_CurrentManagedThreadId:System.Int32()", "System.Environment", "get_CurrentManagedThreadId")]
    [InlineData("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)", "System.Exception", ".ctor", "System.String")]
    public void MatchesMethodSubject_ValidPattern_ReturnsTrue(string subjectId, string declaringType, string methodName, params string[] paramTypes)
    {
        var method = s_plannerType.GetMethod("MatchesMethodSubject", s_static, new[] { typeof(string), typeof(string), typeof(string), typeof(string[]) })!;
        var result = (bool)method.Invoke(null, new object[] { subjectId, declaringType, methodName, paramTypes })!;
        Assert.True(result);
    }

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.Int32", "Concat", "System.String")]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.String", "Concat", "System.Int32")]
    [InlineData("System.Private.CoreLib/System.String::get_Length:System.Int32()", "System.String", "get_Length", "System.Int32")]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.String", "Concat", "System.String", "System.String", "System.String")]
    public void MatchesMethodSubject_Mismatch_ReturnsFalse(string subjectId, string declaringType, string methodName, params string[] paramTypes)
    {
        var method = s_plannerType.GetMethod("MatchesMethodSubject", s_static, new[] { typeof(string), typeof(string), typeof(string), typeof(string[]) })!;
        var result = (bool)method.Invoke(null, new object[] { subjectId, declaringType, methodName, paramTypes })!;
        Assert.False(result);
    }

    // ── GetExternalRuntimeHelperSymbol ────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_")]
    [InlineData("System.Private.CoreLib/System.String::get_Length:System.Int32()", "chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__")]
    [InlineData("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)", "chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_")]
    public void GetExternalRuntimeHelperSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetExternalRuntimeHelperSymbol", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── SanitizeForEnumName (Registry helper) ─────────────────────────

    [Theory]
    [InlineData("System.String", "SYSTEM_STRING")]
    [InlineData("System.Collections.Generic.List`1", "SYSTEM_COLLECTIONS_GENERIC_LIST_1")]
    [InlineData("System.Int32[]", "SYSTEM_INT32__")]
    [InlineData(".ctor", "_CTOR")]
    public void SanitizeForEnumName_TransformsCorrectly(string input, string expected)
    {
        var registryType = s_plannerType.GetNestedType("RuntimeHelperShapeRegistry", s_static)!;
        var method = registryType.GetMethod("SanitizeForEnumName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── MatchesTypeSubjectId (InvocationPlanning.cs) ──────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "System.Private.CoreLib/System.String", "System.Private.CoreLib/System.String", "System.Private.CoreLib/System.String", true)]
    [InlineData("System.Private.CoreLib/System.Int32", "System.Private.CoreLib/System.Int32", "System.Private.CoreLib/System.Int32", "System.Private.CoreLib/System.Int32", true)]
    [InlineData("System.Private.CoreLib/System.String", "System.Private.CoreLib/System.String", "System.Private.CoreLib/System.Int32", "System.Private.CoreLib/System.Int32", false)]
    [InlineData("StubAssembly/StubMethods", "StubAssembly/StubMethods", "StubAssembly/StubMethods", "StubAssembly/StubMethods", true)]
    [InlineData("StubAssembly/StubMethods", "StubAssembly.StubMethods", "StubAssembly/StubMethods", "StubAssembly/StubMethods", true)]
    [InlineData("StubAssembly/OtherType", "StubAssembly/OtherType", "StubAssembly/StubMethods", "StubAssembly/StubMethods", false)]
    public void MatchesTypeSubjectId_ReturnsExpected(string candidateSubjectId, string candidateDefSubjectId, string slotSubjectId, string slotDefSubjectId, bool expected)
    {
        var method = s_plannerType.GetMethod("MatchesTypeSubjectId", s_static, new[] { typeof(string), typeof(string), typeof(string), typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { candidateSubjectId, candidateDefSubjectId, slotSubjectId, slotDefSubjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetDeclaringTypeSubjectId ─────────────────────────────────────

    [Theory]
    [InlineData("A.B::MyField", "A.B")]
    [InlineData("System.Private.CoreLib/System.String::Empty", "System.Private.CoreLib/System.String")]
    public void GetDeclaringTypeSubjectId_ExtractsPrefix(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("NoSeparator")]
    [InlineData("")]
    public void GetDeclaringTypeSubjectId_MissingSeparator_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("missing declaring type", ex.InnerException!.Message);
    }

    // ── GetFieldName ──────────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::MyField", "MyField")]
    [InlineData("System.Private.CoreLib/System.String::Empty", "Empty")]
    public void GetFieldName_ExtractsName(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetFieldName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("NoSeparator")]
    [InlineData("A.B::")]
    public void GetFieldName_Invalid_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetFieldName", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("missing field member", ex.InnerException!.Message);
    }

    // ── GetMethodSignatureSuffix ──────────────────────────────────────

    [Theory]
    [InlineData("A.B::M:System.Void()", "M:System.Void()")]
    [InlineData("T::Add:System.Int32(System.Int32,System.Int32)", "Add:System.Int32(System.Int32,System.Int32)")]
    public void GetMethodSignatureSuffix_ExtractsSuffix(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetMethodSignatureSuffix", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("")]
    [InlineData("NoSeparator")]
    public void GetMethodSignatureSuffix_Invalid_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetMethodSignatureSuffix", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("missing method signature", ex.InnerException!.Message);
    }

    // ── GetMethodDeclaringTypeSubjectId ───────────────────────────────

    [Theory]
    [InlineData("A.B::MethodName", "A.B")]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.Private.CoreLib/System.String")]
    public void GetMethodDeclaringTypeSubjectId_ExtractsDeclaringType(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetMethodDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("")]
    [InlineData("NoSeparator")]
    public void GetMethodDeclaringTypeSubjectId_Invalid_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetMethodDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("missing declaring type", ex.InnerException!.Message);
    }

    // ── GetMethodName ─────────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::Add(System.Int32,System.Int32)", "Add")]
    [InlineData("A.B::get_Length:System.Int32()", "get_Length")]
    [InlineData("T::.ctor:System.Void()", ".ctor")]
    public void GetMethodName_ExtractsMethodName(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetMethodName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("NoSeparator")]
    [InlineData("A.B::")]
    public void GetMethodName_Invalid_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetMethodName", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("failed to extract method name", ex.InnerException!.Message);
    }

    // ── GetTypeDisplayName ────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "System.String")]
    [InlineData("StubAssembly/MyType", "MyType")]
    [InlineData("NoSlash", "NoSlash")]
    public void GetTypeDisplayName_ReturnsExpected(string typeSubjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetTypeDisplayName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { typeSubjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetTypeNamespace ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "System")]
    [InlineData("System.Private.CoreLib/System.Collections.Generic.List`1", "System.Collections.Generic")]
    [InlineData("StubAssembly/MyType", "")]
    public void GetTypeNamespace_ReturnsExpected(string typeSubjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetTypeNamespace", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { typeSubjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetTypeShortName ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "String")]
    [InlineData("System.Private.CoreLib/System.Collections.Generic.List`1", "List`1")]
    [InlineData("NoDot", "NoDot")]
    public void GetTypeShortName_ReturnsExpected(string typeSubjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetTypeShortName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { typeSubjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetAssemblyNameFromSubjectId ──────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "System.Private.CoreLib")]
    [InlineData("StubAssembly/MyType", "StubAssembly")]
    public void GetAssemblyNameFromSubjectId_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetAssemblyNameFromSubjectId", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("NoSlash")]
    [InlineData("/")]
    public void GetAssemblyNameFromSubjectId_Invalid_Throws(string subjectId)
    {
        var method = s_plannerType.GetMethod("GetAssemblyNameFromSubjectId", s_static, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { subjectId }));
        Assert.Contains("missing assembly", ex.InnerException!.Message);
    }

    // ── TryGetNestedDeclaringTypeSubjectId ────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Exception+InnerException", "System.Private.CoreLib/System.Exception")]
    [InlineData("A/B+C", "A/B")]
    public void TryGetNestedDeclaringTypeSubjectId_Nested_ReturnsDeclaring(string typeSubjectId, string? expected)
    {
        var method = s_plannerType.GetMethod("TryGetNestedDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var result = (string?)method.Invoke(null, new object[] { typeSubjectId });
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.Private.CoreLib/System.String")]
    [InlineData("FlatType")]
    public void TryGetNestedDeclaringTypeSubjectId_NotNested_ReturnsNull(string typeSubjectId)
    {
        var method = s_plannerType.GetMethod("TryGetNestedDeclaringTypeSubjectId", s_static, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { typeSubjectId });
        Assert.Null(result);
    }

    // ── MatchesTypeName ───────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String", "System.String", true)]
    [InlineData("System.Private.CoreLib/System.String", "String", true)]
    [InlineData("System.Private.CoreLib/System.String", "System.Int32", false)]
    [InlineData("System.Private.CoreLib/System.Collections.Generic.List`1", "List`1", true)]
    [InlineData("A.B+C", "C", true)]
    public void MatchesTypeName_ReturnsExpected(string candidateTypeSubjectId, string typeName, bool expected)
    {
        var method = s_plannerType.GetMethod("MatchesTypeName", s_static, new[] { typeof(string), typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { candidateTypeSubjectId, typeName })!;
        Assert.Equal(expected, result);
    }

    // ── IsSpecialMethodName ───────────────────────────────────────────

    [Theory]
    [InlineData(null, true)]
    [InlineData("", true)]
    [InlineData(".ctor", true)]
    [InlineData(".cctor", true)]
    [InlineData("<Main>$", true)]
    [InlineData("get_Length", true)]
    [InlineData("set_Name", true)]
    [InlineData("add_Handler", true)]
    [InlineData("remove_Handler", true)]
    [InlineData("ToString", false)]
    [InlineData("DoWork", false)]
    public void IsSpecialMethodName_ReturnsExpected(string? name, bool expected)
    {
        var method = s_plannerType.GetMethod("IsSpecialMethodName", s_static, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object?[] { name })!;
        Assert.Equal(expected, result);
    }

    // ── SanitizeSubjectId ─────────────────────────────────────────────

    [Theory]
    [InlineData("System.String", "System_String")]
    [InlineData("System.Collections.Generic.List`1", "System_Collections_Generic_List_1")]
    [InlineData("System.Int32[]", "System_Int32__")]
    [InlineData("A.B+C", "A_B_C")]
    public void SanitizeSubjectId_TransformsCorrectly(string input, string expected)
    {
        var method = s_plannerType.GetMethod("SanitizeSubjectId", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── ComputeStableTypeId ───────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Int32")]
    [InlineData("System.Private.CoreLib/System.String")]
    [InlineData("StubAssembly/MyType")]
    public void ComputeStableTypeId_ReturnsNonZero(string subjectId)
    {
        var method = s_plannerType.GetMethod("ComputeStableTypeId", s_static, new[] { typeof(string) })!;
        var result = (ulong)method.Invoke(null, new object[] { subjectId })!;
        Assert.NotEqual(0ul, result);
    }

    // ── CreatePseudoMetadataHandle ────────────────────────────────────

    [Theory]
    [InlineData("System.String", 33554432u, 0xFA000000u)]
    [InlineData("System.Int32", 33554432u, 0xFB000000u)]
    [InlineData("", 33554432u, 0x83000000u)]
    public void CreatePseudoMetadataHandle_ReturnsNonZero(string subjectId, uint prefix, uint expectedTopBits)
    {
        var method = s_plannerType.GetMethod("CreatePseudoMetadataHandle", s_static, new[] { typeof(string), typeof(uint) })!;
        var result = (uint)method.Invoke(null, new object[] { subjectId, prefix })!;
        Assert.NotEqual(0u, result);
        Assert.Equal(expectedTopBits, result & 0xFF000000u);
    }

    // ── IsStructuredValueTypeSubjectId ────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Int32", false)]
    [InlineData("System.Private.CoreLib/System.Byte", false)]
    [InlineData("System.Private.CoreLib/System.Double", false)]
    [InlineData("System.Private.CoreLib/System.IntPtr", false)]
    [InlineData("System.Private.CoreLib/System.DateTime", true)]
    [InlineData("System.Private.CoreLib/System.Decimal", true)]
    [InlineData("StubAssembly/ValuePoint", true)]
    public void IsStructuredValueTypeSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = s_plannerType.GetMethod("IsStructuredValueTypeSubjectId", s_static, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── HasArrayElementReference ──────────────────────────────────────

    [Fact]
    public void HasArrayElementReference_WithElement_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("HasArrayElementReference", s_static, new[] { typeof(AotCoreIrReferenceArtifact) })!;
        var reference = new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Type,
            AssemblyName = "mscorlib",
            SubjectId = "System.Int32[]",
            ArrayElementSubjectId = "System.Int32",
        };
        var result = (bool)method.Invoke(null, new object[] { reference })!;
        Assert.True(result);
    }

    [Fact]
    public void HasArrayElementReference_WithoutElement_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("HasArrayElementReference", s_static, new[] { typeof(AotCoreIrReferenceArtifact) })!;
        var reference = new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Type,
            AssemblyName = "mscorlib",
            SubjectId = "System.Int32",
        };
        var result = (bool)method.Invoke(null, new object[] { reference })!;
        Assert.False(result);
    }

    // ── ExtractComMethodName ──────────────────────────────────────────

    [Theory]
    [InlineData("Assembly/Type::MethodName:Signature", "MethodName")]
    [InlineData("Assembly/Type::MethodName", "MethodName")]
    [InlineData("NoSeparator", "NoSeparator")]
    [InlineData("A/B::Method:RetType(Param)", "Method")]
    public void ExtractComMethodName_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("ExtractComMethodName", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── FormatGenericArgumentList ─────────────────────────────────────

    [Fact]
    public void FormatGenericArgumentList_Null_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("FormatGenericArgumentList", s_static, new[] { typeof(IReadOnlyList<string>) })!;
        var result = (string)method.Invoke(null, new object?[] { null })!;
        Assert.Equal("[]", result);
    }

    [Fact]
    public void FormatGenericArgumentList_Empty_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("FormatGenericArgumentList", s_static, new[] { typeof(IReadOnlyList<string>) })!;
        var result = (string)method.Invoke(null, new object[] { new List<string>() })!;
        Assert.Equal("[]", result);
    }

    [Fact]
    public void FormatGenericArgumentList_WithArgs_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("FormatGenericArgumentList", s_static, new[] { typeof(IReadOnlyList<string>) })!;
        var result = (string)method.Invoke(null, new object[] { new List<string> { "System.Int32", "System.String" } })!;
        Assert.Equal("[System.Int32, System.String]", result);
    }

    // ── ToCppStringLiteral ────────────────────────────────────────────

    [Theory]
    [InlineData("hello", "\"hello\"")]
    [InlineData("", "\"\"")]
    [InlineData("line1\nline2", "\"line1\\nline2\"")]
    [InlineData("tab\there", "\"tab\\there\"")]
    [InlineData("quote\"here", "\"quote\\\"here\"")]
    [InlineData("back\\slash", "\"back\\\\slash\"")]
    [InlineData("null\0term", "\"null\\0term\"")]
    public void ToCppStringLiteral_ReturnsExpected(string input, string expected)
    {
        var method = s_plannerType.GetMethod("ToCppStringLiteral", s_static, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── GetNativeTypeShapeValue ───────────────────────────────────────

    [Theory]
    [InlineData(AotCoreIrTypeShapeKind.ReferenceType, (byte)1)]
    [InlineData(AotCoreIrTypeShapeKind.ValueType, (byte)2)]
    [InlineData(AotCoreIrTypeShapeKind.InterfaceType, (byte)3)]
    public void GetNativeTypeShapeValue_ReturnsExpected(AotCoreIrTypeShapeKind kind, byte expected)
    {
        var method = s_plannerType.GetMethod("GetNativeTypeShapeValue", s_static, new[] { typeof(AotCoreIrTypeShapeKind) })!;
        var result = (byte)method.Invoke(null, new object[] { kind })!;
        Assert.Equal(expected, result);
    }

    // ── AreAllTypeArgsSystemObject (GenericSharing.cs) ────────────────

    private static AotCoreIrMethodArtifact MakeGenericMethod(string[]? typeArgs, string[]? methodArgs)
    {
        return new AotCoreIrMethodArtifact
        {
            MethodId = "m1",
            SubjectId = "Test/M::M",
            Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "Test",
                SubjectId = "Test/M::M",
                DeclaringTypeSubjectId = "Test/M",
                DefinitionSubjectId = "Test/M::M",
                MethodId = "m1",
                Signature = "void()",
            },
            NativeSymbol = "",
            IsStatic = true,
            ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0,
            ParameterAbis = [],
            LocalCount = 0,
            ExceptionRegionCount = 0,
            ExceptionRegions = [],
            Instructions = [],
            RuntimeGenericContext = typeArgs != null || methodArgs != null
                ? new RuntimeGenericContextArtifact
                {
                    InstantiationKey = new GenericInstantiationKey
                    {
                        ContextKind = GenericContextKind.TypeInstantiation,
                        DefinitionSubjectId = "Test/M::M",
                        TypeArguments = typeArgs,
                        MethodArguments = methodArgs,
                    },
                    SharedGenericBodyId = new SharedGenericBodyId { Value = "" },
                    InstantiationStubId = new InstantiationStubId { Value = "" },
                    SupportKindCode = GenericSupportKind.Shared,
                    SpecializationKindCode = GenericSpecializationKind.SharedBody,
                }
                : null,
        };
    }

    [Fact]
    public void AreAllTypeArgsSystemObject_AllObject_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("AreAllTypeArgsSystemObject", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var artifact = MakeGenericMethod(["System.Object"], null);
        var result = (bool)method.Invoke(null, new object[] { artifact })!;
        Assert.True(result);
    }

    [Fact]
    public void AreAllTypeArgsSystemObject_MixedArgs_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("AreAllTypeArgsSystemObject", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var artifact = MakeGenericMethod(["System.Int32"], null);
        var result = (bool)method.Invoke(null, new object[] { artifact })!;
        Assert.False(result);
    }

    [Fact]
    public void AreAllTypeArgsSystemObject_NoGenericContext_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("AreAllTypeArgsSystemObject", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var artifact = MakeGenericMethod(null, null);
        var result = (bool)method.Invoke(null, new object[] { artifact })!;
        Assert.False(result);
    }

    [Fact]
    public void AreAllTypeArgsSystemObject_EmptyArgs_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("AreAllTypeArgsSystemObject", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var artifact = MakeGenericMethod([], []);
        var result = (bool)method.Invoke(null, new object[] { artifact })!;
        Assert.False(result);
    }

    // ── IsAsyncRuntimeHelperSubjectId ─────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder::AwaitUnsafeOnCompleted(System.Object&,System.Object&)", false)]
    [InlineData("System.Private.CoreLib/System.String::Concat(System.String,System.String)", false)]
    public void IsAsyncRuntimeHelperSubjectId_ReturnsFalse(string subjectId, bool expected)
    {
        var method = s_plannerType.GetMethod("IsAsyncRuntimeHelperSubjectId", s_static, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── IsMonitorRuntimeHelperSubjectId ───────────────────────────────

    [Theory]
    [InlineData("System.Threading/Monitor::Enter(System.Object,System.Boolean&)", true)]
    [InlineData("System.Threading/Monitor::Exit(System.Object)", true)]
    [InlineData("System.Threading/Monitor::TryEnter(System.Object,System.TimeSpan,System.Boolean&)", true)]
    [InlineData("System.String::Concat(System.String,System.String)", false)]
    public void IsMonitorRuntimeHelperSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = s_plannerType.GetMethod("IsMonitorRuntimeHelperSubjectId", s_static, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── IsSpecialMethodName (for .cctor ────────────────────────────────

    [Fact]
    public void IsSpecialMethodName_DotCctor_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsSpecialMethodName", s_static, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object?[] { ".cctor" })!;
        Assert.True(result);
    }

    // ── RenderSimpleExternalRuntimeHelper (StringAndPlatform.cs) ──────

    private static readonly Type s_stringListType = typeof(List<string>);

    [Fact]
    public void RenderSimpleExternalRuntimeHelper_NormalPath_ReturnsRendered()
    {
        var method = s_plannerType.GetMethod("RenderSimpleExternalRuntimeHelper", s_static,
            new[] { typeof(string), typeof(string), typeof(string), s_stringListType })!;
        var bodyLines = new List<string> { "    return 42;" };
        var result = (string)method.Invoke(null, new object[] { "CHAOS_IL2CPP_INT32", "test_helper", "CHAOS_IL2CPP_INT32 chaos_arg_0", bodyLines })!;
        Assert.Contains("test_helper", result);
        Assert.Contains("return 42", result);
    }

    [Fact]
    public void RenderSimpleExternalRuntimeHelper_SentinelFix_Applies()
    {
        var method = s_plannerType.GetMethod("RenderSimpleExternalRuntimeHelper", s_static,
            new[] { typeof(string), typeof(string), typeof(string), s_stringListType })!;
        var bodyLines = new List<string> { "    return 0;" };
        var result = (string)method.Invoke(null, new object[] { "CHAOS_IL2CPP_INTPTR", "sentinel_helper", "CHAOS_IL2CPP_INTPTR chaos_arg_0", bodyLines })!;
        // Sentinel fix should add s_sentinel and replace return 0 with return &s_sentinel
        Assert.Contains("s_sentinel", result);
        Assert.Contains("reinterpret_cast", result);
    }

    [Fact]
    public void RenderSimpleExternalRuntimeHelper_SentinelNotApplied_WhenOtherReturnsExist()
    {
        var method = s_plannerType.GetMethod("RenderSimpleExternalRuntimeHelper", s_static,
            new[] { typeof(string), typeof(string), typeof(string), s_stringListType })!;
        // Has "return " in middle line but not the last line → isStub = false
        var bodyLines = new List<string> { "    if (x) return -1;", "    return 0;" };
        var result = (string)method.Invoke(null, new object[] { "CHAOS_IL2CPP_INTPTR", "nonstub_helper", "CHAOS_IL2CPP_INTPTR chaos_arg_0", bodyLines })!;
        // Sentinel fix should NOT apply because the first "return " is not the last line
        Assert.DoesNotContain("s_sentinel", result);
    }

    [Fact]
    public void RenderSimpleExternalRuntimeHelper_SentinelNotApplied_WhenNotIntPtr()
    {
        var method = s_plannerType.GetMethod("RenderSimpleExternalRuntimeHelper", s_static,
            new[] { typeof(string), typeof(string), typeof(string), s_stringListType })!;
        var bodyLines = new List<string> { "    return 0;" };
        var result = (string)method.Invoke(null, new object[] { "CHAOS_IL2CPP_INT32", "int32_helper", "CHAOS_IL2CPP_INT32 chaos_arg_0", bodyLines })!;
        // Not INTPTR → no sentinel fix
        Assert.DoesNotContain("s_sentinel", result);
    }

    // ── CreateVoidAbiSlot (CollectionAndReflection.cs) ────────────────

    [Fact]
    public void CreateVoidAbiSlot_ReturnsVoidSlot()
    {
        var method = s_plannerType.GetMethod("CreateVoidAbiSlot", s_static)!;
        var result = (AotCoreIrAbiSlotArtifact)method.Invoke(null, null)!;
        Assert.Equal(AotCoreIrAbiCarrierKind.Void, result.CarrierKindCode);
    }

    // ── ComputeStringId (StringIdEmission.cs) ─────────────────────────

    [Fact]
    public void ComputeStringId_ReturnsStableHash()
    {
        var method = s_plannerType.GetMethod("ComputeStringId", s_static, new[] { typeof(string) })!;
        var result1 = (ulong)method.Invoke(null, new object[] { "Hello" })!;
        var result2 = (ulong)method.Invoke(null, new object[] { "Hello" })!;
        Assert.Equal(result1, result2); // Same input → same hash
        Assert.NotEqual(0ul, result1);
        Assert.NotEqual(1ul, result1);
    }

    [Fact]
    public void ComputeStringId_DifferentInputs_DifferentHashes()
    {
        var method = s_plannerType.GetMethod("ComputeStringId", s_static, new[] { typeof(string) })!;
        var h1 = (ulong)method.Invoke(null, new object[] { "Alice" })!;
        var h2 = (ulong)method.Invoke(null, new object[] { "Bob" })!;
        Assert.NotEqual(h1, h2);
    }

    // ── GetRequiredIlOffset (ExceptionEmission.cs) ────────────────────

    [Fact]
    public void GetRequiredIlOffset_ReturnsIlOffset()
    {
        var method = s_plannerType.GetMethod("GetRequiredIlOffset", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "nop", IlOffset = 42 };
        var result = (int)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(42, result);
    }

    // ── GetRequiredIntOperand (ExceptionEmission.cs) ──────────────────

    [Fact]
    public void GetRequiredIntOperand_ReturnsIntOperand()
    {
        var method = s_plannerType.GetMethod("GetRequiredIntOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.i4", IlOffset = 0, Operand = 123 };
        var result = (int)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(123, result);
    }

    [Fact]
    public void GetRequiredIntOperand_NonInt_Throws()
    {
        var method = s_plannerType.GetMethod("GetRequiredIntOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.i4", IlOffset = 0, Operand = "not an int" };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { instr }));
    }

    // ── GetRequiredInt64Operand (ExceptionEmission.cs) ─────────────────

    [Fact]
    public void GetRequiredInt64Operand_ReturnsLongOperand()
    {
        var method = s_plannerType.GetMethod("GetRequiredInt64Operand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.i8", IlOffset = 0, Operand = 999_999_999_999L };
        var result = (long)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(999_999_999_999L, result);
    }

    [Fact]
    public void GetRequiredInt64Operand_IntBecomesLong()
    {
        var method = s_plannerType.GetMethod("GetRequiredInt64Operand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.i4", IlOffset = 0, Operand = 42 };
        var result = (long)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(42L, result);
    }

    // ── GetRequiredSingleOperand (ExceptionEmission.cs) ────────────────

    [Fact]
    public void GetRequiredSingleOperand_ReturnsFloat()
    {
        var method = s_plannerType.GetMethod("GetRequiredSingleOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.r4", IlOffset = 0, Operand = 3.14f };
        var result = (float)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(3.14f, result);
    }

    [Fact]
    public void GetRequiredSingleOperand_DoubleToFloat()
    {
        var method = s_plannerType.GetMethod("GetRequiredSingleOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.r4", IlOffset = 0, Operand = 2.71 };
        var result = (float)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(2.71f, result);
    }

    // ── GetRequiredDoubleOperand (ExceptionEmission.cs) ────────────────

    [Fact]
    public void GetRequiredDoubleOperand_ReturnsDouble()
    {
        var method = s_plannerType.GetMethod("GetRequiredDoubleOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.r8", IlOffset = 0, Operand = 3.14159 };
        var result = (double)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(3.14159, result);
    }

    [Fact]
    public void GetRequiredDoubleOperand_FloatToDouble()
    {
        var method = s_plannerType.GetMethod("GetRequiredDoubleOperand", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact { Op = "ldc.r8", IlOffset = 0, Operand = 1.23f };
        var result = (double)method.Invoke(null, new object[] { instr })!;
        Assert.Equal(1.23, result, precision: 4);
    }

    // ── IsEnumToStringCall (ExceptionEmission.cs) ─────────────────────

    [Fact]
    public void IsEnumToStringCall_MatchingCallee_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsEnumToStringCall", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "call", IlOffset = 0,
            Callee = "System.DayOfWeek::ToString:System.String()",
        };
        var result = (bool)method.Invoke(null, new object[] { instr })!;
        Assert.True(result);
    }

    [Fact]
    public void IsEnumToStringCall_NonMatchingCallee_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsEnumToStringCall", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "call", IlOffset = 0,
            Callee = "System.String::Concat(System.String,System.String)",
        };
        var result = (bool)method.Invoke(null, new object[] { instr })!;
        Assert.False(result);
    }

    // ── RequireInt32IntegralResultType (ExceptionEmission.cs) ──────────

    [Fact]
    public void RequireInt32IntegralResultType_Int32_DoesNotThrow()
    {
        var method = s_plannerType.GetMethod("RequireInt32IntegralResultType", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "add", IlOffset = 0, ResultType = "System.Int32",
        };
        var ex = Record.Exception(() => method.Invoke(null, new object[] { instr }));
        Assert.Null(ex);
    }

    [Fact]
    public void RequireInt32IntegralResultType_UInt32_DoesNotThrow()
    {
        var method = s_plannerType.GetMethod("RequireInt32IntegralResultType", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "add", IlOffset = 0, ResultType = "System.UInt32",
        };
        var ex = Record.Exception(() => method.Invoke(null, new object[] { instr }));
        Assert.Null(ex);
    }

    [Fact]
    public void RequireInt32IntegralResultType_Int64_Throws()
    {
        var method = s_plannerType.GetMethod("RequireInt32IntegralResultType", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "add", IlOffset = 0, ResultType = "System.Int64",
        };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { instr }));
    }

    // ── IdentifyStructLocalSlots (ExceptionEmission.cs) ───────────────

    [Fact]
    public void IdentifyStructLocalSlots_NoInitobj_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("IdentifyStructLocalSlots", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "nop", IlOffset = 0 },
        };
        var result = (HashSet<int>)method.Invoke(null, new object[] { instructions })!;
        Assert.Empty(result);
    }

    [Fact]
    public void IdentifyStructLocalSlots_InitobjWithoutLdloca_NotCounted()
    {
        var method = s_plannerType.GetMethod("IdentifyStructLocalSlots", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "initobj", IlOffset = 1, TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = AotCoreIrReferenceKind.Type,
                AssemblyName = "Test",
                SubjectId = "Test/SomeStruct",
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            }},
        };
        var result = (HashSet<int>)method.Invoke(null, new object[] { instructions })!;
        Assert.Empty(result); // no preceding ldloca
    }

    [Fact]
    public void IdentifyStructLocalSlots_LdlocaInitobj_ReturnsSlot()
    {
        var method = s_plannerType.GetMethod("IdentifyStructLocalSlots", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "ldloca", IlOffset = 0, Operand = 0 },
            new() { Op = "initobj", IlOffset = 1, TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = AotCoreIrReferenceKind.Type,
                AssemblyName = "Test",
                SubjectId = "Test/SomeStruct",
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            }},
        };
        var result = (HashSet<int>)method.Invoke(null, new object[] { instructions })!;
        Assert.Equal([0], result);
    }

    [Fact]
    public void IdentifyStructLocalSlots_NonValueTypeInitobj_NotCounted()
    {
        var method = s_plannerType.GetMethod("IdentifyStructLocalSlots", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "ldloca", IlOffset = 0, Operand = 0 },
            new() { Op = "initobj", IlOffset = 1, TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = AotCoreIrReferenceKind.Type,
                AssemblyName = "Test",
                SubjectId = "Test/SomeClass",
                TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
            }},
        };
        var result = (HashSet<int>)method.Invoke(null, new object[] { instructions })!;
        Assert.Empty(result); // ReferenceType, not ValueType
    }

    // ── FilterRedundantStoreReloadPairs (ExceptionEmission.cs) ────────

    [Fact]
    public void FilterRedundantStoreReloadPairs_NoRedundancy_ReturnsAll()
    {
        var method = s_plannerType.GetMethod("FilterRedundantStoreReloadPairs", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>),
                    typeof(IReadOnlySet<int>),
                    typeof(IReadOnlySet<int>) })!;
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "ldloc", IlOffset = 0, Operand = 0 },
            new() { Op = "ldc.i4", IlOffset = 1, Operand = 42 },
            new() { Op = "stloc", IlOffset = 2, Operand = 0 },
        };
        var result = (IReadOnlyList<AotCoreIrInstructionArtifact>)method.Invoke(null,
            new object[] { instructions, null, null })!;
        Assert.Equal(3, result.Count);
    }

    // ── GetRequiredSwitchTargets (ExceptionEmission.cs) ────────────────

    [Fact]
    public void GetRequiredSwitchTargets_IntArrayOperand_ReturnsTargets()
    {
        var method = s_plannerType.GetMethod("GetRequiredSwitchTargets", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact), typeof(IReadOnlySet<int>) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "switch", IlOffset = 0,
            Operand = new int[] { 10, 20, 30 },
        };
        var offsets = new HashSet<int> { 10, 20, 30 };
        var result = (IReadOnlyList<int>)method.Invoke(null, new object[] { instr, offsets })!;
        Assert.Equal([10, 20, 30], result);
    }

    [Fact]
    public void GetRequiredSwitchTargets_MissingOffset_FiltersOut()
    {
        var method = s_plannerType.GetMethod("GetRequiredSwitchTargets", s_static,
            new[] { typeof(AotCoreIrInstructionArtifact), typeof(IReadOnlySet<int>) })!;
        var instr = new AotCoreIrInstructionArtifact
        {
            Op = "switch", IlOffset = 0,
            Operand = new int[] { 99 },
        };
        var offsets = new HashSet<int> { 10, 20 };
        var result = (IReadOnlyList<int>)method.Invoke(null, new object[] { instr, offsets })!;
        Assert.Empty(result);
    }

    // ── CollectStringLiterals (StringIdEmission.cs) ────────────────────

    [Fact]
    public void CollectStringLiterals_NoLdstr_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("CollectStringLiterals", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrMethodArtifact>) })!;
        var methods = new List<AotCoreIrMethodArtifact>
        {
            MakeMinimalMethod([new AotCoreIrInstructionArtifact { Op = "nop", IlOffset = 0 }]),
        };
        var result = (IReadOnlySet<string>)method.Invoke(null, new object[] { methods })!;
        Assert.Empty(result);
    }

    [Fact]
    public void CollectStringLiterals_WithLdstr_ReturnsLiterals()
    {
        var method = s_plannerType.GetMethod("CollectStringLiterals", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrMethodArtifact>) })!;
        var methods = new List<AotCoreIrMethodArtifact>
        {
            MakeMinimalMethod([
                new AotCoreIrInstructionArtifact { Op = "ldstr", IlOffset = 0, Operand = "Hello" },
                new AotCoreIrInstructionArtifact { Op = "ldstr", IlOffset = 1, Operand = "World" },
            ]),
        };
        var result = (IReadOnlySet<string>)method.Invoke(null, new object[] { methods })!;
        Assert.Contains("Hello", result);
        Assert.Contains("World", result);
        Assert.Equal(2, result.Count);
    }

    // ── BuildStringIdMapping (StringIdEmission.cs) ─────────────────────

    [Fact]
    public void BuildStringIdMapping_EmptyInput_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("BuildStringIdMapping", s_static,
            new[] { typeof(IReadOnlySet<string>) })!;
        var result = (IReadOnlyDictionary<string, ulong>)method.Invoke(null,
            new object[] { new HashSet<string>() })!;
        Assert.Empty(result);
    }

    [Fact]
    public void BuildStringIdMapping_WithStrings_ReturnsMapping()
    {
        var method = s_plannerType.GetMethod("BuildStringIdMapping", s_static,
            new[] { typeof(IReadOnlySet<string>) })!;
        var literals = new HashSet<string> { "Hello", "World" };
        var result = (IReadOnlyDictionary<string, ulong>)method.Invoke(null,
            new object[] { literals })!;
        Assert.Equal(2, result.Count);
        Assert.True(result.ContainsKey("Hello"));
        Assert.True(result.ContainsKey("World"));
        Assert.NotEqual(0ul, result["Hello"]);
        Assert.NotEqual(0ul, result["World"]);
    }

    // ── Helper: MakeMinimalMethod ──────────────────────────────────────

    private static AotCoreIrMethodArtifact MakeMinimalMethod(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        return new AotCoreIrMethodArtifact
        {
            MethodId = "m1",
            SubjectId = "Test/M::M",
            Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "Test", SubjectId = "Test/M::M",
                DeclaringTypeSubjectId = "Test/M", DefinitionSubjectId = "Test/M::M",
                MethodId = "m1", Signature = "void()",
            },
            NativeSymbol = "", IsStatic = true, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = instructions,
        };
    }

    // ── StructuredControlFlow.cs: opcode predicates ────────────────────

    [Theory]
    [InlineData("br", true)]
    [InlineData("leave", true)]
    [InlineData("ret", true)]
    [InlineData("throw", true)]
    [InlineData("call", false)]
    [InlineData("add", false)]
    public void IsBlockTerminatorOpcode_ReturnsExpected(string op, bool expected)
    {
        var method = s_plannerType.GetMethod("IsBlockTerminatorOpcode", s_static, new[] { typeof(string) })!;
        Assert.Equal(expected, (bool)method.Invoke(null, new object[] { op })!);
    }

    [Theory]
    [InlineData("brfalse", true)]
    [InlineData("brtrue", true)]
    [InlineData("blt", true)]
    [InlineData("bgt", true)]
    [InlineData("beq", true)]
    [InlineData("bge", true)]
    [InlineData("ble", true)]
    [InlineData("bne.un", true)]
    [InlineData("br", false)]
    [InlineData("call", false)]
    public void IsConditionalBranchOpcode_ReturnsExpected(string op, bool expected)
    {
        var method = s_plannerType.GetMethod("IsConditionalBranchOpcode", s_static, new[] { typeof(string) })!;
        Assert.Equal(expected, (bool)method.Invoke(null, new object[] { op })!);
    }

    [Theory]
    [InlineData("switch", true)]
    [InlineData("br", false)]
    public void IsSwitchOpcode_ReturnsExpected(string op, bool expected)
    {
        var method = s_plannerType.GetMethod("IsSwitchOpcode", s_static, new[] { typeof(string) })!;
        Assert.Equal(expected, (bool)method.Invoke(null, new object[] { op })!);
    }

    // ── InvocationAbi.cs: Format literal helpers ───────────────────────

    [Fact]
    public void FormatInt32Literal_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("FormatInt32Literal", s_static, new[] { typeof(int) })!;
        var result = (string)method.Invoke(null, new object[] { 42 })!;
        Assert.Equal("42", result);
    }

    [Fact]
    public void FormatInt64Literal_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("FormatInt64Literal", s_static, new[] { typeof(long) })!;
        var result = (string)method.Invoke(null, new object[] { 999_999_999_999L })!;
        Assert.Equal("999999999999LL", result);
    }

    [Fact]
    public void FormatFloat32Literal_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("FormatFloat32Literal", s_static, new[] { typeof(float) })!;
        var result = (string)method.Invoke(null, new object[] { 3.14f })!;
        // FormatFloat32Literal appends "f" suffix; C++ requires decimal point
        Assert.EndsWith("f", result);
    }

    [Fact]
    public void FormatFloat64Literal_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("FormatFloat64Literal", s_static, new[] { typeof(double) })!;
        var result = (string)method.Invoke(null, new object[] { 3.14159 })!;
        Assert.Equal("3.14159", result);
    }

    // ── InvocationAbi.cs: ABI slot creators ────────────────────────────

    [Fact]
    public void CreateLegacyAbiSlot_Int32_ReturnsSlot()
    {
        var method = s_plannerType.GetMethod("CreateLegacyAbiSlot", s_static, new[] { typeof(string) })!;
        var result = (AotCoreIrAbiSlotArtifact)method.Invoke(null, new object?[] { "System.Int32" })!;
        Assert.NotNull(result);
    }

    [Fact]
    public void CreateLegacyAbiSlot_Null_Throws()
    {
        var method = s_plannerType.GetMethod("CreateLegacyAbiSlot", s_static, new[] { typeof(string) })!;
        Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object?[] { null }));
    }

    [Fact]
    public void CreateLegacyReturnAbiSlot_ReturnsSlot()
    {
        var method = s_plannerType.GetMethod("CreateLegacyReturnAbiSlot", s_static, new[] { typeof(string) })!;
        var result = (AotCoreIrAbiSlotArtifact)method.Invoke(null, new object?[] { "System.Int32" })!;
        Assert.NotNull(result);
    }

    [Fact]
    public void CreateNativeIntAbiSlot_ReturnsSlot()
    {
        var method = s_plannerType.GetMethod("CreateNativeIntAbiSlot", s_static,
            new[] { typeof(string), typeof(AotCoreIrTypeShapeKind) })!;
        var result = (AotCoreIrAbiSlotArtifact)method.Invoke(null,
            new object[] { "System.String", AotCoreIrTypeShapeKind.ReferenceType })!;
        Assert.NotNull(result);
    }

    [Fact]
    public void CreateInt32AbiSlot_ReturnsSlot()
    {
        var method = s_plannerType.GetMethod("CreateInt32AbiSlot", s_static,
            new[] { typeof(string), typeof(AotCoreIrTypeShapeKind) })!;
        var result = (AotCoreIrAbiSlotArtifact)method.Invoke(null,
            new object[] { "System.Int32", AotCoreIrTypeShapeKind.ValueType })!;
        Assert.NotNull(result);
    }

    [Fact]
    public void IsStringParameterSlot_StringSlot_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsStringParameterSlot", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        // Slot with NativeInt carrier and a string TypeSubjectId
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeSubjectId = "System.Private.CoreLib/System.String",
        };
        Assert.True((bool)method.Invoke(null, new object[] { slot })!);
    }

    [Fact]
    public void IsStringParameterSlot_IntSlot_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsStringParameterSlot", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
        };
        Assert.False((bool)method.Invoke(null, new object[] { slot })!);
    }

    // ── InvocationAbi.cs: ArgBufferSize ────────────────────────────────

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Int32, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.NativeInt, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.Float32, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.Float64, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.Int8, 4)]
    public void GetAbiSlotArgBufferSize_ReturnsExpected(AotCoreIrAbiCarrierKind kind, int expected)
    {
        var method = s_plannerType.GetMethod("GetAbiSlotArgBufferSize", s_static,
            new[] { typeof(AotCoreIrAbiCarrierKind) })!;
        var result = (int)method.Invoke(null, new object[] { kind })!;
        Assert.Equal(expected, result);
    }

    // ── InvocationPlanning.cs ──────────────────────────────────────────
    [Fact]
    public void ExtractNumericSortKey_NormalString_ReturnsMaxValue()
    {
        var method = s_plannerType.GetMethod("ExtractNumericSortKey", s_static,
            new[] { typeof(string) })!;
        var result = InvokeRefTuple(method!, "hello");
        Assert.Equal(int.MaxValue, result.Item1);
        Assert.Equal("hello", result.Item2);
    }

    [Fact]
    public void ExtractNumericSortKey_NumericPrefix_ReturnsMaxValue()
    {
        var method = s_plannerType.GetMethod("ExtractNumericSortKey", s_static,
            new[] { typeof(string) })!;
        var result = InvokeRefTuple(method!, "42abc");
        Assert.Equal(int.MaxValue, result.Item1);
        Assert.Equal("42abc", result.Item2);
    }

    [Fact]
    public void ExtractNumericSortKey_SubjectN_ReturnsNumber()
    {
        var method = s_plannerType.GetMethod("ExtractNumericSortKey", s_static,
            new[] { typeof(string) })!;
        var result = InvokeRefTuple(method!, "Test/Type::Subject_42:System.Void()");
        Assert.Equal(42, result.Item1);
    }

    // Helper to call (int, string) returning methods via reflection
    private static (int, string) InvokeRefTuple(MethodInfo method, string arg)
    {
        var raw = method.Invoke(null, new object[] { arg })!;
        var t = raw.GetType();
        return ((int)t.GetField("Item1")!.GetValue(raw)!, (string)t.GetField("Item2")!.GetValue(raw)!);
    }

    // ── TypeResolution.cs ──────────────────────────────────────────────

    [Fact]
    public void GetMethodParameterTypes_NormalSignature_ReturnsTypes()
    {
        var method = s_plannerType.GetMethod("GetMethodParameterTypes", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null,
            new object[] { "System.String::Concat(System.String,System.String)" })!;
        Assert.Contains("System.String", result);
    }

    [Fact]
    public void GetMethodParameterTypes_NoParams_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("GetMethodParameterTypes", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null,
            new object[] { "System.String::ToString()" })!;
        Assert.Empty(result);
    }

    [Theory]
    // No commas at depth-0 → returns the whole string as one element
    [InlineData("System.Collections.Generic.List`1[[System.Int32]]", new[] { "System.Collections.Generic.List`1[[System.Int32]]" })]
    // Brackets don't affect depth tracking; comma at depth-0 splits
    [InlineData("System.Tuple`2[[System.String],[System.Int32]]", new[] { "System.Tuple`2[[System.String]", "[System.Int32]]" })]
    // Single element with no generics → returns that element
    [InlineData("System.Int32", new[] { "System.Int32" })]
    public void SplitTopLevelGenericArguments_ReturnsExpected(string input, string[] expected)
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    [Fact]
    public void SplitTopLevelGenericArguments_NullOrEmpty_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { "" })!;
        Assert.Empty(result);
    }

    [Fact]
    public void GetSyntheticReferenceTypeBaseSubjectId_ValueType_ReturnsNull()
    {
        var method = s_plannerType.GetMethod("GetSyntheticReferenceTypeBaseSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "System.Int32" });
        Assert.Null(result);
    }

    // ── StaticInitializationPlanning.cs ────────────────────────────────

    [Theory]
    [InlineData(null, "")]
    [InlineData("System.Int32", "System.Int32")]
    [InlineData("System.Collections.Generic.List`1[[System.Int32]]", "System.Collections.Generic.List<!0>[[System.Int32]]")]
    public void NormalizeGenericTypeId_ReturnsExpected(string? input, string expected)
    {
        var method = s_plannerType.GetMethod("NormalizeGenericTypeId", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object?[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── ModuleRegistration.cs ──────────────────────────────────────────

    [Fact]
    public void EscapeCppStringLiteral_Normal_ReturnsEscaped()
    {
        var method = s_plannerType.GetMethod("EscapeCppStringLiteral", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "hello\nworld\tfoo\\bar" })!;
        Assert.Contains("\\n", result);
        Assert.Contains("\\t", result);
        Assert.Contains("\\\\", result);
    }

    [Fact]
    public void ParseGuidStringToBytes_ValidGuid_ReturnsBytes()
    {
        var method = s_plannerType.GetMethod("ParseGuidStringToBytes", s_static,
            new[] { typeof(string) })!;
        var result = (byte[]?)method.Invoke(null,
            new object[] { "ABCDEF01-2345-6789-ABCD-EF0123456789" })!;
        Assert.NotNull(result);
        Assert.Equal(16, result.Length);
    }

    [Fact]
    public void ParseGuidStringToBytes_Invalid_ReturnsNull()
    {
        var method = s_plannerType.GetMethod("ParseGuidStringToBytes", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "not-a-guid" });
        Assert.Null(result);
    }

    // ── MethodEmission.cs ──────────────────────────────────────────────

    [Theory]
    [InlineData(0, "")]
    [InlineData(0x100, "")]       // WinApi → empty (platform default)
    [InlineData(0x200, "__cdecl ")]
    [InlineData(0x300, "__stdcall ")]
    [InlineData(0x400, "__thiscall ")]
    [InlineData(0x500, "__fastcall ")]
    public void GetCallingConventionAnnotation_ReturnsExpected(int importFlags, string expected)
    {
        var method = s_plannerType.GetMethod("GetCallingConventionAnnotation", s_static,
            new[] { typeof(int) })!;
        var result = (string)method.Invoke(null, new object[] { importFlags })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData(0, false)]
    [InlineData(0x0002, false)]   // Ansi
    [InlineData(0x0004, true)]    // Unicode
    [InlineData(0x0006, true)]    // Auto (Unicode on Windows)
    public void IsUnicodeCharSet_ReturnsExpected(int charSet, bool expected)
    {
        var method = s_plannerType.GetMethod("IsUnicodeCharSet", s_static,
            new[] { typeof(int) })!;
        var result = (bool)method.Invoke(null, new object[] { charSet })!;
        Assert.Equal(expected, result);
    }

    // ── InvocationAbi.cs: CanEmitMethodBody ────────────────────────────

    [Fact]
    public void CanEmitMethodBody_WithInstructions_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("CanEmitMethodBody", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var m = MakeMinimalMethod([new AotCoreIrInstructionArtifact { Op = "ret", IlOffset = 0 }]);
        Assert.True((bool)method.Invoke(null, new object[] { m })!);
    }

    [Fact]
    public void CanEmitMethodBody_NoInstructions_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("CanEmitMethodBody", s_static,
            new[] { typeof(AotCoreIrMethodArtifact) })!;
        var m = MakeMinimalMethod([]);
        Assert.False((bool)method.Invoke(null, new object[] { m })!);
    }

    // ── InvocationAbi.cs: Infer from subjectId ─────────────────────────

    [Fact]
    public void InferParameterCountFromSubjectId_WithParams_ReturnsCount()
    {
        var method = s_plannerType.GetMethod("InferParameterCountFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (int)method.Invoke(null,
            new object[] { "System.String::Concat(System.String,System.String)" })!;
        Assert.Equal(2, result);
    }

    [Fact]
    public void InferParameterCountFromSubjectId_NoParams_ReturnsZero()
    {
        var method = s_plannerType.GetMethod("InferParameterCountFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (int)method.Invoke(null,
            new object[] { "System.String::ToString()" })!;
        Assert.Equal(0, result);
    }

    [Fact]
    public void InferReturnTypeFromSubjectId_ReturnsType()
    {
        var method = s_plannerType.GetMethod("InferReturnTypeFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null,
            new object[] { "System.Int32::Parse:System.Int32(System.String)" });
        Assert.NotNull(result);
    }

    // ── ObjectModelUtilities.cs: GetNativeSymbol etc. ───────────────────

    [Fact]
    public void GetNativeSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeSymbol", s_static,
            new[] { typeof(string), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "NS", "MyMethod" })!;
        Assert.Contains("NS", result);
        Assert.Contains("MyMethod", result);
    }

    [Fact]
    public void GetNativeTypeSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeTypeSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.String" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeValueTypeSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeValueTypeSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeBoxTypeSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeBoxTypeSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeTypeIdSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeTypeIdSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeTypeInfoSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeTypeInfoSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeVTableSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeVTableSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeMethodTableSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeMethodTableSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeBoxTypeIdSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeBoxTypeIdSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeFieldMemberName_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeFieldMemberName", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "Test/MyClass::myField" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetNativeStaticFieldSymbol_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetNativeStaticFieldSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "Test/MyClass::myField" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetTypeHandleLiteral_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetTypeHandleLiteral", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetMethodHandleLiteral_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetMethodHandleLiteral", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "Test/M::M" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void GetFieldHandleLiteral_ReturnsFormatted()
    {
        var method = s_plannerType.GetMethod("GetFieldHandleLiteral", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "Test/MyClass::myField" })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    // ── ObjectModelUtilities.cs: IsTypeReflectionHelperSubjectId etc. (covered by integration tests) ──

    // ── TypeResolution.cs: TryReadGenericArgumentList ────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Span`1<System.Int32>::get_Item(System.Int32)", "System.Private.CoreLib/System.Span`1<", "System.Int32")]
    [InlineData("NoAngleBrackets", "<", null)]
    [InlineData("Prefix<arg>suffix", "Prefix<", "arg")]
    [InlineData("Nested<Outer<Inner>>rest", "Nested<", "Outer<Inner>")]
    public void TryReadGenericArgumentList_ReturnsExpected(string value, string marker, string? expectedArg)
    {
        var method = s_plannerType.GetMethod("TryReadGenericArgumentList", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { value, marker, null };
        var result = (bool)method.Invoke(null, args)!;
        if (expectedArg is null)
        {
            Assert.False(result);
        }
        else
        {
            Assert.True(result);
            Assert.Equal(expectedArg, (string)args[2]!);
        }
    }

    // ── TypeResolution.cs: TryParseAsyncBuilderStartStateMachineType ────

    [Fact]
    public void TryParseAsyncBuilderStartStateMachineType_Matches()
    {
        var method = s_plannerType.GetMethod("TryParseAsyncBuilderStartStateMachineType", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType(), typeof(string).MakeByRefType() })!;
        // SubjectId format: assembly/Type<Arg>::Start<SM>(SM&)
        // Uses direct angle brackets (not `1 notation) in SubjectId convention.
        var callee = "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Threading.Tasks.Task<System.Int32>>::Start<StateMachine>(StateMachine&)";
        var args = new object?[] { callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", null, null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.NotNull(args[2]);
        Assert.NotNull(args[3]);
    }

    // ── TypeResolution.cs: TryParseAsyncBuilderAwaitUnsafeOnCompleted ──

    [Fact]
    public void TryParseAsyncBuilderAwaitUnsafeOnCompleted_Matches()
    {
        var method = s_plannerType.GetMethod("TryParseAsyncBuilderAwaitUnsafeOnCompleted", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType(), typeof(string).MakeByRefType(), typeof(string).MakeByRefType() })!;
        // Requires generic builder (AsyncTaskMethodBuilder<TResult>), not plain form.
        var callee = "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Threading.Tasks.Task<System.Int32>>::AwaitUnsafeOnCompleted<Awaiter,SM>(Awaiter&,SM&)";
        var args = new object?[] { callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", null, null, null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
    }

    // ── TypeResolution.cs: TryReadSingleGenericTypeArgument ─────────────

    [Fact]
    public void TryReadSingleGenericTypeArgument_Match_ReturnsArg()
    {
        var method = s_plannerType.GetMethod("TryReadSingleGenericTypeArgument", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Private.CoreLib/System.Span`1<System.Int32>::get_Item(System.Int32)", "System.Private.CoreLib/System.Span`1<", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("System.Int32", (string)args[2]!);
    }

    // ── TypeResolution.cs: TryParseClosedListElementType ────────────────

    [Fact]
    public void TryParseClosedListElementType_Matches()
    {
        var method = s_plannerType.GetMethod("TryParseClosedListElementType", s_static,
            new[] { typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Collections/System.Collections.Generic.List<System.Int32>", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("System.Int32", (string)args[1]!);
    }

    // ── TypeResolution.cs: TryParseClosedDictionaryStringIntType ────────

    [Theory]
    [InlineData("System.Collections/System.Collections.Generic.Dictionary<System.String,System.Int32>", true)]
    [InlineData("System.Collections/System.Collections.Generic.Dictionary<System.Int32,System.String>", false)]
    public void TryParseClosedDictionaryStringIntType_ReturnsExpected(string declaringType, bool expected)
    {
        var method = s_plannerType.GetMethod("TryParseClosedDictionaryStringIntType", s_static,
            new[] { typeof(string) })!;
        Assert.Equal(expected, (bool)method.Invoke(null, new object[] { declaringType })!);
    }

    // ── TypeResolution.cs: TryCreateCollectionArrayElementExpression ────

    [Fact]
    public void TryCreateCollectionArrayElementExpression_NativeInt_ReturnsExpression()
    {
        var method = s_plannerType.GetMethod("TryCreateCollectionArrayElementExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string), typeof(string).MakeByRefType() })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt };
        var args = new object?[] { slot, "arr", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("arr", (string)args[2]!);
    }

    [Fact]
    public void TryCreateCollectionArrayElementExpression_Int32_ReturnsExpression()
    {
        var method = s_plannerType.GetMethod("TryCreateCollectionArrayElementExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string), typeof(string).MakeByRefType() })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 };
        var args = new object?[] { slot, "val", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Contains("static_cast", (string)args[2]!);
    }

    // ── InvocationAbi.cs: MapAbiSlotReturnType ─────────────────────────

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Void, "void")]
    [InlineData(AotCoreIrAbiCarrierKind.Int32, "CHAOS_IL2CPP_INT32")]
    [InlineData(AotCoreIrAbiCarrierKind.Int8, "CHAOS_IL2CPP_INT8")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt8, "CHAOS_IL2CPP_UINT8")]
    [InlineData(AotCoreIrAbiCarrierKind.Int16, "CHAOS_IL2CPP_INT16")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt16, "CHAOS_IL2CPP_UINT16")]
    [InlineData(AotCoreIrAbiCarrierKind.Float32, "float")]
    [InlineData(AotCoreIrAbiCarrierKind.Float64, "double")]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, "CHAOS_IL2CPP_INT64")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt64, "CHAOS_IL2CPP_UINT64")]
    [InlineData(AotCoreIrAbiCarrierKind.NativeInt, "CHAOS_IL2CPP_INTPTR")]
    [InlineData(AotCoreIrAbiCarrierKind.ByRef, "CHAOS_IL2CPP_INTPTR")]
    public void MapAbiSlotReturnType_ReturnsExpected(AotCoreIrAbiCarrierKind kind, string expected)
    {
        var method = s_plannerType.GetMethod("MapAbiSlotReturnType", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var result = (string)method.Invoke(null, new object[] { new AotCoreIrAbiSlotArtifact { CarrierKindCode = kind } })!;
        Assert.Equal(expected, result);
    }

    // ── InvocationAbi.cs: FormatAbiSlotParameterSignature ─────────────

    [Fact]
    public void FormatAbiSlotParameterSignature_Empty_ReturnsVoid()
    {
        var method = s_plannerType.GetMethod("FormatAbiSlotParameterSignature", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var result = (string)method.Invoke(null, new object[] { Array.Empty<AotCoreIrAbiSlotArtifact>() })!;
        Assert.Equal("void", result);
    }

    [Fact]
    public void FormatAbiSlotParameterSignature_NonEmpty_ReturnsSignature()
    {
        var method = s_plannerType.GetMethod("FormatAbiSlotParameterSignature", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var slots = new[]
        {
            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 },
            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt },
        };
        var result = (string)method.Invoke(null, new object[] { slots })!;
        Assert.Contains("chaos_fn_arg_0", result);
        Assert.Contains("chaos_fn_arg_1", result);
    }

    // ── InvocationAbi.cs: FormatAbiSlotParameterTypes ─────────────────

    [Fact]
    public void FormatAbiSlotParameterTypes_Empty_ReturnsVoid()
    {
        var method = s_plannerType.GetMethod("FormatAbiSlotParameterTypes", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var result = (string)method.Invoke(null, new object[] { Array.Empty<AotCoreIrAbiSlotArtifact>() })!;
        Assert.Equal("void", result);
    }

    [Fact]
    public void FormatAbiSlotParameterTypes_NonEmpty_ReturnsTypes()
    {
        var method = s_plannerType.GetMethod("FormatAbiSlotParameterTypes", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var slots = new[]
        {
            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 },
        };
        var result = (string)method.Invoke(null, new object[] { slots })!;
        Assert.Contains("CHAOS_IL2CPP_INT32", result);
    }

    // ── InvocationAbi.cs: GetAbiSlotArgBufferSize (remaining cases) ───

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Int8, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.UInt8, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.Int16, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.UInt16, 4)]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.UInt64, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.Float64, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.ByRef, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.MultiReturn, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.ByRefToValueType, 8)]
    [InlineData(AotCoreIrAbiCarrierKind.ValueTypeByValue, 8)]
    public void GetAbiSlotArgBufferSize_MoreCases(AotCoreIrAbiCarrierKind kind, int expected)
    {
        var method = s_plannerType.GetMethod("GetAbiSlotArgBufferSize", s_static,
            new[] { typeof(AotCoreIrAbiCarrierKind) })!;
        Assert.Equal(expected, (int)method.Invoke(null, new object[] { kind })!);
    }

    // ── InvocationAbi.cs: CalculateArgBufferSize ──────────────────────

    [Fact]
    public void CalculateArgBufferSize_Empty_ReturnsZero()
    {
        var method = s_plannerType.GetMethod("CalculateArgBufferSize", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var result = (int)method.Invoke(null, new object[] { Array.Empty<AotCoreIrAbiSlotArtifact>() })!;
        Assert.Equal(0, result);
    }

    [Fact]
    public void CalculateArgBufferSize_MultipleSlots_ReturnsSum()
    {
        var method = s_plannerType.GetMethod("CalculateArgBufferSize", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrAbiSlotArtifact>) })!;
        var slots = new[]
        {
            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 },
            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt },
        };
        var result = (int)method.Invoke(null, new object[] { slots })!;
        Assert.Equal(12, result); // 4 + 8
    }

    // ── InvocationAbi.cs: GetArgBufferWriteCall ───────────────────────

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Int32, "WriteI32")]
    [InlineData(AotCoreIrAbiCarrierKind.Float32, "WriteF32")]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, "WriteI64")]
    [InlineData(AotCoreIrAbiCarrierKind.NativeInt, "WritePtr")]
    public void GetArgBufferWriteCall_ReturnsExpected(AotCoreIrAbiCarrierKind kind, string expected)
    {
        var method = s_plannerType.GetMethod("GetArgBufferWriteCall", s_static,
            new[] { typeof(AotCoreIrAbiCarrierKind), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { kind, "x" })!;
        Assert.StartsWith(expected, result);
    }

    // ── TypeResolution.cs: GetMethodParameterTypes (more cases) ────────

    [Fact]
    public void GetMethodParameterTypes_SingleParam_ReturnsOne()
    {
        var method = s_plannerType.GetMethod("GetMethodParameterTypes", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null,
            new object[] { "System.Int32::Parse(System.String)" })!;
        Assert.Single(result);
        Assert.Equal("System.String", result[0]);
    }

    // ── TypeResolution.cs: GetSyntheticReferenceTypeBaseSubjectId ──────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Int32", null)]
    [InlineData("System.Private.CoreLib/System.Delegate", "System.Private.CoreLib/System.Object")]
    [InlineData("System.Private.CoreLib/System.MulticastDelegate", "System.Private.CoreLib/System.Delegate")]
    [InlineData("System.Private.CoreLib/System.Action", "System.Private.CoreLib/System.MulticastDelegate")]
    [InlineData("System.Private.CoreLib/System.Func`2", "System.Private.CoreLib/System.MulticastDelegate")]
    public void GetSyntheticReferenceTypeBaseSubjectId_ReturnsExpected(string subjectId, string? expected)
    {
        var method = s_plannerType.GetMethod("GetSyntheticReferenceTypeBaseSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { subjectId });
        Assert.Equal(expected, result);
    }

    // ── TypeResolution.cs: TryParseClosedReadOnlyCollectionElementType ─

    [Fact]
    public void TryParseClosedReadOnlyCollectionElementType_Matches()
    {
        var method = s_plannerType.GetMethod("TryParseClosedReadOnlyCollectionElementType", s_static,
            new[] { typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Private.CoreLib/System.Collections.Generic.IReadOnlyCollection<System.Int32>", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("System.Int32", (string)args[1]!);
    }

    // ── TypeResolution.cs: TryParseClosedReadOnlyListElementType ───────

    [Fact]
    public void TryParseClosedReadOnlyListElementType_Matches()
    {
        var method = s_plannerType.GetMethod("TryParseClosedReadOnlyListElementType", s_static,
            new[] { typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Private.CoreLib/System.Collections.Generic.IReadOnlyList<System.Int32>", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("System.Int32", (string)args[1]!);
    }

    // ── TypeResolution.cs: TryParseDefaultInterpolatedStringHandlerAppendFormattedType ──

    [Fact]
    public void TryParseDefaultInterpolatedStringHandlerAppendFormattedType_NonHandler_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("TryParseDefaultInterpolatedStringHandlerAppendFormattedType", s_static,
            new[] { typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Private.CoreLib/System.String::Concat(System.String)", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsMonitorRuntimeHelperSubjectId ───────

    [Fact]
    public void IsMonitorRuntimeHelperSubjectId_MonitorEnter_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsMonitorRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        // Monitor.Enter(System.Object, System.Boolean&)
        var result = (bool)method.Invoke(null,
            new object[] { "System.Threading/Monitor::Enter(System.Object,System.Boolean&)" })!;
        Assert.True(result);
    }

    // ── ObjectModelUtilities.cs: IsAsyncRuntimeHelperSubjectId ─────────

    [Fact]
    public void IsAsyncRuntimeHelperSubjectId_TaskAwaiter_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsAsyncRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<System.Int32>::GetResult()" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsAsyncRuntimeHelperSubjectId_TaskGetAwaiter_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsAsyncRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Threading.Tasks.Task<System.Int32>::GetAwaiter()" })!;
        Assert.True(result);
    }

    // ── RequiredStringField ───────────────────────────────────────────

    [Fact]
    public void RequireStringField_NonNull_DoesNotThrow()
    {
        var method = s_plannerType.GetMethod("RequireStringField", s_static,
            new[] { typeof(string), typeof(string) })!;
        var ex = Record.Exception(() => method.Invoke(null, new object?[] { "hello", "fieldName" }));
        Assert.Null(ex);
    }

    [Fact]
    public void RequireStringField_Null_Throws()
    {
        var method = s_plannerType.GetMethod("RequireStringField", s_static,
            new[] { typeof(string), typeof(string) })!;
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object?[] { null, "fieldName" }));
    }

    // ── InvocationAbi.cs: MapAbiSlotParameterType ─────────────────────

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Int32, "CHAOS_IL2CPP_INT32")]
    [InlineData(AotCoreIrAbiCarrierKind.Int8, "CHAOS_IL2CPP_INT8")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt8, "CHAOS_IL2CPP_UINT8")]
    [InlineData(AotCoreIrAbiCarrierKind.Int16, "CHAOS_IL2CPP_INT16")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt16, "CHAOS_IL2CPP_UINT16")]
    [InlineData(AotCoreIrAbiCarrierKind.Float32, "float")]
    [InlineData(AotCoreIrAbiCarrierKind.Float64, "double")]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, "CHAOS_IL2CPP_INT64")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt64, "CHAOS_IL2CPP_UINT64")]
    [InlineData(AotCoreIrAbiCarrierKind.NativeInt, "CHAOS_IL2CPP_INTPTR")]
    [InlineData(AotCoreIrAbiCarrierKind.ByRef, "CHAOS_IL2CPP_INTPTR")]
    [InlineData(AotCoreIrAbiCarrierKind.MultiReturn, "CHAOS_IL2CPP_INTPTR")]
    [InlineData(AotCoreIrAbiCarrierKind.ByRefToValueType, "CHAOS_IL2CPP_INTPTR")]
    public void MapAbiSlotParameterType_ReturnsExpected(AotCoreIrAbiCarrierKind kind, string expected)
    {
        var method = s_plannerType.GetMethod("MapAbiSlotParameterType", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var result = (string)method.Invoke(null, new object[] { new AotCoreIrAbiSlotArtifact { CarrierKindCode = kind } })!;
        Assert.Equal(expected, result);
    }

    [Fact]
    public void MapAbiSlotParameterType_ValueTypeByValue_ReturnsSymbol()
    {
        var method = s_plannerType.GetMethod("MapAbiSlotParameterType", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
            TypeSubjectId = "System.Private.CoreLib/System.Int32",
        };
        var result = (string)method.Invoke(null, new object[] { slot })!;
        Assert.StartsWith("chaos_valuetype_", result);
    }

    [Fact]
    public void MapAbiSlotParameterType_UnknownKind_Throws()
    {
        var method = s_plannerType.GetMethod("MapAbiSlotParameterType", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)255 };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { slot })!);
    }

    // ── InvocationAbi.cs: FormatAbiArgumentExpression ──────────────────

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.Int32, "static_cast<CHAOS_IL2CPP_INT32>(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.Int8, "static_cast<CHAOS_IL2CPP_INT8>(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt8, "static_cast<CHAOS_IL2CPP_UINT8>(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.Int16, "static_cast<CHAOS_IL2CPP_INT16>(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt16, "static_cast<CHAOS_IL2CPP_UINT16>(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.Float32, "ChaosLoadFloat32(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.Float64, "ChaosLoadFloat64(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.Int64, "ChaosLoadInt64(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.UInt64, "chaos_load_uint64(x)")]
    [InlineData(AotCoreIrAbiCarrierKind.NativeInt, "x")]
    [InlineData(AotCoreIrAbiCarrierKind.ByRef, "x")]
    [InlineData(AotCoreIrAbiCarrierKind.MultiReturn, "x")]
    [InlineData(AotCoreIrAbiCarrierKind.ByRefToValueType, "x")]
    public void FormatAbiArgumentExpression_ReturnsExpected(AotCoreIrAbiCarrierKind kind, string expected)
    {
        var method = s_plannerType.GetMethod("FormatAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { new AotCoreIrAbiSlotArtifact { CarrierKindCode = kind }, "x" })!;
        Assert.Equal(expected, result);
    }

    [Fact]
    public void FormatAbiArgumentExpression_ValueTypeByValue_ReturnsDerefExpression()
    {
        var method = s_plannerType.GetMethod("FormatAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
            TypeSubjectId = "System.Private.CoreLib/System.Int32",
        };
        var result = (string)method.Invoke(null, new object[] { slot, "x" })!;
        Assert.StartsWith("*chaos_resolve_managed_value_pointer<", result);
    }

    [Fact]
    public void FormatAbiArgumentExpression_UnknownKind_Throws()
    {
        var method = s_plannerType.GetMethod("FormatAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)255 };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { slot, "x" })!);
    }

    // ── InvocationAbi.cs: FormatInboundAbiArgumentExpression ───────────

    [Fact]
    public void FormatInboundAbiArgumentExpression_NativeIntReferenceType_ReturnsNormalize()
    {
        var method = s_plannerType.GetMethod("FormatInboundAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
            TypeSubjectId = "StubAssembly/SomeClass",
        };
        var result = (string)method.Invoke(null, new object[] { slot, "arg" })!;
        Assert.Equal("chaos_normalize_native_int_argument(arg)", result);
    }

    [Fact]
    public void FormatInboundAbiArgumentExpression_NativeIntString_ReturnsPlain()
    {
        var method = s_plannerType.GetMethod("FormatInboundAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
            TypeSubjectId = "System.Private.CoreLib/System.String",
        };
        var result = (string)method.Invoke(null, new object[] { slot, "arg" })!;
        Assert.Equal("arg", result);
    }

    [Theory]
    [InlineData(AotCoreIrAbiCarrierKind.ByRef)]
    [InlineData(AotCoreIrAbiCarrierKind.ByRefToValueType)]
    [InlineData(AotCoreIrAbiCarrierKind.MultiReturn)]
    public void FormatInboundAbiArgumentExpression_ByRefKinds_ReturnsNormalize(AotCoreIrAbiCarrierKind kind)
    {
        var method = s_plannerType.GetMethod("FormatInboundAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = kind };
        var result = (string)method.Invoke(null, new object[] { slot, "arg" })!;
        Assert.Equal("chaos_normalize_native_int_argument(arg)", result);
    }

    [Fact]
    public void FormatInboundAbiArgumentExpression_Default_ReturnsPlain()
    {
        var method = s_plannerType.GetMethod("FormatInboundAbiArgumentExpression", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact), typeof(string) })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 };
        var result = (string)method.Invoke(null, new object[] { slot, "arg" })!;
        Assert.Equal("arg", result);
    }

    // ── InvocationAbi.cs: GetRequiredAbiValueTypeSymbol ────────────────

    [Fact]
    public void GetRequiredAbiValueTypeSymbol_ValidSlot_ReturnsSymbol()
    {
        var method = s_plannerType.GetMethod("GetRequiredAbiValueTypeSymbol", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
            TypeSubjectId = "System.Private.CoreLib/System.Int32",
        };
        var result = (string)method.Invoke(null, new object[] { slot })!;
        Assert.StartsWith("chaos_valuetype_", result);
    }

    [Fact]
    public void GetRequiredAbiValueTypeSymbol_NonValueTypeKind_Throws()
    {
        var method = s_plannerType.GetMethod("GetRequiredAbiValueTypeSymbol", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32 };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { slot })!);
    }

    [Fact]
    public void GetRequiredAbiValueTypeSymbol_NullTypeSubjectId_Throws()
    {
        var method = s_plannerType.GetMethod("GetRequiredAbiValueTypeSymbol", s_static,
            new[] { typeof(AotCoreIrAbiSlotArtifact) })!;
        var slot = new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
            TypeSubjectId = null,
        };
        Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { slot })!);
    }

    // ── MethodEmission.cs: GetCallingConventionAnnotation ──────────────

    [Fact]
    public void GetMetadataMethodNameForReflectionMemberDefinition_PlainName_ReturnsName()
    {
        var method = s_plannerType.GetMethod("GetMetadataMethodNameForReflectionMemberDefinition", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.String::Concat:System.String(System.String,System.String)" })!;
        Assert.Equal("Concat", result);
    }

    [Fact]
    public void GetMetadataMethodNameForReflectionMemberDefinition_GenericName_StripsArity()
    {
        var method = s_plannerType.GetMethod("GetMetadataMethodNameForReflectionMemberDefinition", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Action`1::Invoke:System.Void(!!0)" })!;
        Assert.Equal("Invoke", result);
    }

    [Fact]
    public void GetMetadataMethodNameForReflectionMemberDefinition_WithGenericArgs_StripsArgs()
    {
        var method = s_plannerType.GetMethod("GetMetadataMethodNameForReflectionMemberDefinition", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "T::Method<T>:System.Void(T)" })!;
        Assert.Equal("Method", result);
    }

    // ── ModuleRegistration.cs: EncodePackedString ────────────────────

    [Fact]
    public void EncodePackedString_ValidString_ReturnsLengthPrefixed()
    {
        var method = s_plannerType.GetMethod("EncodePackedString", s_static,
            new[] { typeof(string) })!;
        var result = (byte[])method.Invoke(null, new object[] { "hi" })!;
        Assert.Equal(4, result.Length);
        Assert.Equal(2, result[0]); // low byte of length
        Assert.Equal(0, result[1]); // high byte of length
        Assert.Equal((byte)'h', result[2]);
        Assert.Equal((byte)'i', result[3]);
    }

    [Fact]
    public void EncodePackedString_EmptyString_ReturnsTwoZeroBytes()
    {
        var method = s_plannerType.GetMethod("EncodePackedString", s_static,
            new[] { typeof(string) })!;
        var result = (byte[])method.Invoke(null, new object[] { "" })!;
        Assert.Equal(2, result.Length);
        Assert.Equal(0, result[0]);
        Assert.Equal(0, result[1]);
    }

    // ── ModuleRegistration.cs: EncodeEnumValue ───────────────────────

    [Fact]
    public void EncodeEnumValue_Int32_Returns4Bytes()
    {
        var method = s_plannerType.GetMethod("EncodeEnumValue", s_static,
            new[] { typeof(object) })!;
        var result = (byte[])method.Invoke(null, new object[] { 42 })!;
        Assert.Equal(4, result.Length);
    }

    [Fact]
    public void EncodeEnumValue_Byte_Returns1Byte()
    {
        var method = s_plannerType.GetMethod("EncodeEnumValue", s_static,
            new[] { typeof(object) })!;
        var result = (byte[])method.Invoke(null, new object[] { (byte)7 })!;
        Assert.Equal(1, result.Length);
        Assert.Equal(7, result[0]);
    }

    // ── ModuleRegistration.cs: FormatEnumLiteral ─────────────────────

    [Fact]
    public void FormatEnumLiteral_Int32_ReturnsExpected()
    {
        var method = s_plannerType.GetMethod("FormatEnumLiteral", s_static,
            new[] { typeof(object) })!;
        var result = (string)method.Invoke(null, new object[] { 42 })!;
        Assert.Equal("static_cast<CHAOS_IL2CPP_INTPTR>(42)", result);
    }

    [Fact]
    public void FormatEnumLiteral_Byte_ReturnsExpected()
    {
        var method = s_plannerType.GetMethod("FormatEnumLiteral", s_static,
            new[] { typeof(object) })!;
        var result = (string)method.Invoke(null, new object[] { (byte)7 })!;
        Assert.Equal("static_cast<CHAOS_IL2CPP_INTPTR>(7)", result);
    }

    // ── ObjectModelUtilities.cs: GetNativeType*Symbol methods ─────────

    [Theory]
    [InlineData("System.String", "chaos_ensure_type_initialized_System_String")]
    [InlineData("System.Int32", "chaos_ensure_type_initialized_System_Int32")]
    public void GetNativeTypeInitializationFunctionSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeTypeInitializationFunctionSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.String", "chaos_type_init_once_System_String")]
    [InlineData("System.Int32", "chaos_type_init_once_System_Int32")]
    public void GetNativeTypeInitializationOnceFlagSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeTypeInitializationOnceFlagSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.String", "chaos_mt_System_String")]
    [InlineData("System.Int32", "chaos_mt_System_Int32")]
    public void GetNativeTypeInfoV0Symbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeTypeInfoV0Symbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.String", "chaos_mt_System_String.warm")]
    [InlineData("System.Int32", "chaos_mt_System_Int32.warm")]
    public void GetNativeTypeInfoWarmSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeTypeInfoWarmSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.String", "chaos_marshal_desc_System_String")]
    [InlineData("System.Int32", "chaos_marshal_desc_System_Int32")]
    public void GetNativeStructMarshallingDescriptorSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeStructMarshallingDescriptorSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("System.String", "s_marshal_field_names_System_String")]
    [InlineData("System.Int32", "s_marshal_field_names_System_Int32")]
    public void GetNativeStructFieldNamesSymbol_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_plannerType.GetMethod("GetNativeStructFieldNamesSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── NativeAotLoweringPlanner.cs: TryParseStaticFieldDataSize ─────

    [Fact]
    public void TryParseStaticFieldDataSize_ValidMarker_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("TryParseStaticFieldDataSize", s_static,
            new[] { typeof(string), typeof(int).MakeByRefType() })!;
        var args = new object?[] { "__StaticArrayInitTypeSize=42", 0 };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal(42, (int)args[1]!);
    }

    [Fact]
    public void TryParseStaticFieldDataSize_NoMarker_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("TryParseStaticFieldDataSize", s_static,
            new[] { typeof(string), typeof(int).MakeByRefType() })!;
        var args = new object?[] { "System.Int32", 0 };
        var result = (bool)method.Invoke(null, args)!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsThreadRuntimeHelperSubjectId ──────

    [Fact]
    public void IsThreadRuntimeHelperSubjectId_EnvironmentThreadId_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsThreadRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Environment::get_CurrentManagedThreadId:System.Int32()" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsThreadRuntimeHelperSubjectId_ThreadStart_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsThreadRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Threading.Thread/System.Threading.Thread::Start:System.Void()" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsThreadRuntimeHelperSubjectId_NonThread_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsThreadRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── FormatInt32Literal edge case (int.MinValue) ───────────────────

    [Fact]
    public void FormatInt32Literal_MinValue_ReturnsMacro()
    {
        var method = s_plannerType.GetMethod("FormatInt32Literal", s_static, new[] { typeof(int) })!;
        var result = (string)method.Invoke(null, new object[] { int.MinValue })!;
        Assert.Equal("CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32)", result);
    }

    // ── FormatInt64Literal edge case (long.MinValue) ───────────────────

    [Fact]
    public void FormatInt64Literal_MinValue_ReturnsMacro()
    {
        var method = s_plannerType.GetMethod("FormatInt64Literal", s_static, new[] { typeof(long) })!;
        var result = (string)method.Invoke(null, new object[] { long.MinValue })!;
        Assert.Equal("CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT64)", result);
    }

    // ── FormatFloat32Literal edge case (no decimal point) ──────────────

    [Fact]
    public void FormatFloat32Literal_NoDecimal_AppendsDotZero()
    {
        var method = s_plannerType.GetMethod("FormatFloat32Literal", s_static, new[] { typeof(float) })!;
        var result = (string)method.Invoke(null, new object[] { 10f })!;
        Assert.Contains(".0f", result);
    }

    // ── ObjectModelUtilities.cs: IsCollectionRuntimeHelperSubjectId ──

    [Fact]
    public void IsCollectionRuntimeHelperSubjectId_List_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsCollectionRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Collections.Generic.IReadOnlyCollection<System.Int32>::get_Count()" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsCollectionRuntimeHelperSubjectId_Dictionary_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsCollectionRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Collections.Generic.IReadOnlyList<System.Int32>::get_Item(System.Int32)" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsCollectionRuntimeHelperSubjectId_NonCollection_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsCollectionRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsSpanRuntimeHelperSubjectId ────────

    [Fact]
    public void IsSpanRuntimeHelperSubjectId_InitializeArray_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsSpanRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray(System.Array,System.RuntimeFieldHandle)" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsSpanRuntimeHelperSubjectId_SpanMethod_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsSpanRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Span<System.Byte>::get_Item(System.Int32)" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsSpanRuntimeHelperSubjectId_NonSpan_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsSpanRuntimeHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsTypeReflectionHelperSubjectId ──────

    [Fact]
    public void IsTypeReflectionHelperSubjectId_TypeGetTypeFromHandle_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsTypeReflectionHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsTypeReflectionHelperSubjectId_NonType_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsTypeReflectionHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsAssemblyReflectionHelperSubjectId ──

    [Fact]
    public void IsAssemblyReflectionHelperSubjectId_AssemblyGetType_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsAssemblyReflectionHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Reflection.Assembly::GetType(System.String)" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsAssemblyReflectionHelperSubjectId_NonAssembly_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsAssemblyReflectionHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── ObjectModelUtilities.cs: IsReflectionMemberHelperSubjectId ────

    [Fact]
    public void IsReflectionMemberHelperSubjectId_MethodBaseInvoke_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsReflectionMemberHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Reflection.MethodBase::Invoke(System.Object,System.Object[])" })!;
        Assert.True(result);
    }

    [Fact]
    public void IsReflectionMemberHelperSubjectId_NonReflection_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("IsReflectionMemberHelperSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.String::Concat(System.String,System.String)" })!;
        Assert.False(result);
    }

    // ── AotCoreIrLowering.cs: ResolveExceptionRegionKind ──────────────

    [Fact]
    public void ResolveExceptionRegionKind_Catch_ReturnsCatch()
    {
        var method = s_aotCoreIrLoweringType.GetMethod("ResolveExceptionRegionKind", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "catch" })!;
        Assert.Equal("Catch", result.ToString());
    }

    [Theory]
    [InlineData("finally")]
    [InlineData("fault")]
    [InlineData("filter")]
    public void ResolveExceptionRegionKind_Others_ReturnsExpected(string kind)
    {
        var method = s_aotCoreIrLoweringType.GetMethod("ResolveExceptionRegionKind", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { kind })!;
        Assert.NotNull(result);
    }

    // ── AotCoreIrLowering.cs: ResolveArrayElementSubjectId ────────────

    [Fact]
    public void ResolveArrayElementSubjectId_ArrayType_ReturnsElementType()
    {
        var method = s_aotCoreIrLoweringType.GetMethod("ResolveArrayElementSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "System.Int32[]" })!;
        Assert.Equal("System.Int32", (string)result!);
    }

    [Fact]
    public void ResolveArrayElementSubjectId_NonArray_ReturnsNull()
    {
        var method = s_aotCoreIrLoweringType.GetMethod("ResolveArrayElementSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.Null(result);
    }

    // ── AotCoreIrLowering.cs: IsKnownValueTypeIdentity ────────────────

    [Theory]
    [InlineData("System.Int32", true)]
    [InlineData("System.Boolean", true)]
    [InlineData("System.String", false)]
    [InlineData("System.Object", false)]
    public void IsKnownValueTypeIdentity_ReturnsExpected(string identity, bool expected)
    {
        var method = s_aotCoreIrLoweringType.GetMethod("IsKnownValueTypeIdentity", s_static,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { identity })!;
        Assert.Equal(expected, result);
    }

    // ── ObjectModelUtilities.cs: GetRuntimeTypeInfoExpression ─────────

    [Fact]
    public void GetRuntimeTypeInfoExpression_NullOrEmpty_ReturnsNullptr()
    {
        var method = s_plannerType.GetMethod("GetRuntimeTypeInfoExpression", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object?[] { null })!;
        Assert.Equal("nullptr", result);
    }

    [Fact]
    public void GetRuntimeTypeInfoExpression_NonNull_ReturnsExpression()
    {
        var method = s_plannerType.GetMethod("GetRuntimeTypeInfoExpression", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.Contains("chaos_mt_System_Int32", result);
    }

    // ── TypeResolution.cs: TryReadGenericArgumentList ───────────────────

    [Fact]
    public void TryReadGenericArgumentList_Simple_ReturnsContent()
    {
        var method = s_plannerType.GetMethod("TryReadGenericArgumentList", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "List<System.Int32>", "<", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
        Assert.Equal("System.Int32", (string)args[2]!);
    }

    [Fact]
    public void TryReadGenericArgumentList_NoMarker_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("TryReadGenericArgumentList", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "System.Int32", "<", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.False(result);
    }

    // ── TypeResolution.cs: SplitTopLevelGenericArguments ────────────────

    [Fact]
    public void SplitTopLevelGenericArguments_Single_ReturnsSingle()
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { "System.Int32" })!;
        Assert.Single(result);
        Assert.Equal("System.Int32", result[0]);
    }

    [Fact]
    public void SplitTopLevelGenericArguments_Multiple_ReturnsMultiple()
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { "System.Int32,System.String" })!;
        Assert.Equal(2, result.Count);
        Assert.Equal("System.Int32", result[0]);
        Assert.Equal("System.String", result[1]);
    }

    [Fact]
    public void SplitTopLevelGenericArguments_NestedGeneric_RespectsDepth()
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null,
            new object[] { "System.Collections.Generic.List<System.Int32>,System.String" })!;
        Assert.Equal(2, result.Count);
    }

    [Fact]
    public void SplitTopLevelGenericArguments_Empty_ReturnsEmpty()
    {
        var method = s_plannerType.GetMethod("SplitTopLevelGenericArguments", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { "" })!;
        Assert.Empty(result);
    }

    // ── TypeResolution.cs: TryReadSingleGenericTypeArgument ─────────────

    [Fact]
    public void TryReadSingleGenericTypeArgument_Single_ReturnsType()
    {
        var method = s_plannerType.GetMethod("TryReadSingleGenericTypeArgument", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "Nullable<System.Int32>", "<", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.True(result);
    }

    [Fact]
    public void TryReadSingleGenericTypeArgument_Multiple_ReturnsFalse()
    {
        var method = s_plannerType.GetMethod("TryReadSingleGenericTypeArgument", s_static,
            new[] { typeof(string), typeof(string), typeof(string).MakeByRefType() })!;
        var args = new object?[] { "Dictionary<System.Int32,System.String>", "<", null };
        var result = (bool)method.Invoke(null, args)!;
        Assert.False(result);
    }

    // ── TypeResolution.cs: IsDelegateTypeSubjectId ──────────────────────

    [Fact]
    public void IsDelegateTypeSubjectId_Action_ReturnsTrue()
    {
        var method = s_plannerType.GetMethod("IsDelegateTypeSubjectId", s_static,
            new[] { typeof(string), typeof(IReadOnlyDictionary<string, string?>) })!;
        var baseTypes = new Dictionary<string, string?>();
        var result = (bool)method.Invoke(null,
            new object[] { "System.Private.CoreLib/System.Action", baseTypes })!;
        Assert.True(result);
    }

    // ── ModuleRegistration.cs: ParseGuidStringToBytes error paths ─────

    [Fact]
    public void ParseGuidStringToBytes_InvalidLength_ReturnsNull()
    {
        var method = s_plannerType.GetMethod("ParseGuidStringToBytes", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "short" });
        Assert.Null(result);
    }

    [Fact]
    public void ParseGuidStringToBytes_InvalidFormat_ReturnsNull()
    {
        var method = s_plannerType.GetMethod("ParseGuidStringToBytes", s_static,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "ABCDEF01-2345-6789-ABCD-EF01234567XX" });
        Assert.Null(result);
    }

    // ── RuntimeHelperShapeRegistry Types ─────────────────────────────────

    private static readonly Type s_shapeRegistryType = typeof(NativeAotLoweringPlanner).Assembly
        .GetType("Chaos.IL2CPP.Generator.NativeAotLoweringPlanner+RuntimeHelperShapeRegistry")!;

    // ── ExtractDelegateArity (RuntimeHelperShapeRegistry) ────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Func`3[[System.Int32],[System.String]]", 2)] // Func<int,string> → 2 params, N-1=2
    [InlineData("System.Private.CoreLib/System.Action`1[[System.Int32]]", 1)]             // Action<int> → 1 param
    [InlineData("System.Private.CoreLib/System.Func`2[[System.Int32],[System.Boolean]]", 1)] // Func<int,bool> → 1 param
    [InlineData("System.Private.CoreLib/System.Action`2[[System.Int32],[System.String]]", 2)]
    [InlineData("System.Private.CoreLib/System.Func`1[[System.Boolean]]", 0)]             // Func<bool> → 0 params (N-1=0)
    [InlineData("System.Private.CoreLib/System.String", 0)]                                // Non-delegate → 0
    [InlineData("NoSlash", 0)]                                                              // No slash → 0
    public void ExtractDelegateArity_ReturnsExpected(string subjectId, int expected)
    {
        var method = s_shapeRegistryType.GetMethod("ExtractDelegateArity", s_static,
            new[] { typeof(string) })!;
        var result = (int)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── TryParseGenericTypeArgsFromTypeName (RuntimeHelperShapeRegistry) ─

    [Theory]
    [InlineData("`1[[System.Int32]]", new[] { "System.Int32" })]
    [InlineData("`2[[System.Int32],[System.String]]", new[] { "System.Int32],[System.String" })] // inner content not split
    [InlineData("NoBrackets", null)]     // No [[...]] → null
    [InlineData("`0", null)]             // No brackets → null
    [InlineData("", null)]               // Empty → null
    [InlineData("`0[[]]", new string[0])] // Empty type args → empty array
    public void TryParseGenericTypeArgsFromTypeName_ReturnsExpected(
        string genericSuffix, string[]? expected)
    {
        var method = s_shapeRegistryType.GetMethod("TryParseGenericTypeArgsFromTypeName", s_static,
            new[] { typeof(string), typeof(IReadOnlyList<string>).MakeByRefType() })!;
        var parameters = new object[] { genericSuffix, null! };
        var result = (bool)method.Invoke(null, parameters)!;
        if (expected is null)
        {
            Assert.False(result);
        }
        else
        {
            Assert.True(result);
            var typeArgs = (IReadOnlyList<string>)parameters[1]!;
            Assert.Equal(expected, typeArgs);
        }
    }

    // ── ExtractDelegateArityFromSubjectId (NativeAotLoweringPlanner) ─────

    [Theory]
    [InlineData("System.Private.CoreLib/System.Func`3[[System.Int32],[System.String]]", 2)]
    [InlineData("System.Private.CoreLib/System.Action`1[[System.Int32]]", 1)]
    [InlineData("System.Private.CoreLib/System.Func`1[[System.Boolean]]", 0)]
    [InlineData("NoBacktick", 0)]
    [InlineData("System.Private.CoreLib/System.String", 0)]
    [InlineData("System.Private.CoreLib/System.Func`0", 0)] // arity 0 is invalid for Func
    public void ExtractDelegateArityFromSubjectId_ReturnsExpected(string subjectId, int expected)
    {
        var method = s_plannerType.GetMethod("ExtractDelegateArityFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (int)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── TryCreateMetadataEntityHandle (NativeAotLoweringPlanner) ─────────

    [Theory]
    [InlineData(0, false)]          // Zero token → false
    [InlineData(0x06000001, true)]  // Valid methoddef token → true
    [InlineData(0x02000002, true)]  // Valid typedef token → true
    [InlineData(-1, false)]         // Invalid token → false (ArgumentException caught)
    public void TryCreateMetadataEntityHandle_ReturnsExpected(int metadataToken, bool expected)
    {
        var method = s_plannerType.GetMethod("TryCreateMetadataEntityHandle", s_static,
            new[] { typeof(int), typeof(EntityHandle).MakeByRefType() })!;
        var handleType = typeof(System.Reflection.Metadata.EntityHandle);
        var parameters = new object[] { metadataToken, null! };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.Equal(expected, result);
    }

    // ── GetTypeDisplayNameFromSubjectId (RuntimeHelperShapeRegistry) ─────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "System.String")]
    [InlineData("StubAssembly/StubMethods::NoOp:System.Void()", "StubMethods")]
    [InlineData("NoSeparator", "NoSeparator")]
    [InlineData("System.Private.CoreLib/System.Int32", "System.Int32")]
    public void GetTypeDisplayNameFromSubjectId_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_shapeRegistryType.GetMethod("GetTypeDisplayNameFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetMethodNameFromSubjectId (RuntimeHelperShapeRegistry) ──────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", "Concat")]
    [InlineData("StubAssembly/StubMethods::NoOp:System.Void()", "NoOp")]
    [InlineData("NoSeparator", "")]
    [InlineData("System.Private.CoreLib/System.GC::Collect(System.Int32)", "Collect")]
    public void GetMethodNameFromSubjectId_ReturnsExpected(string subjectId, string expected)
    {
        var method = s_shapeRegistryType.GetMethod("GetMethodNameFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetMethodParameterTypesFromSubjectId (RuntimeHelperShapeRegistry) ─

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", new[] { "System.String", "System.String" })]
    [InlineData("StubAssembly/StubMethods::NoOp:System.Void()", new string[0])]
    [InlineData("NoParens", new string[0])]
    [InlineData("System.Private.CoreLib/System.GC::Collect(System.Int32)", new[] { "System.Int32" })]
    public void GetMethodParameterTypesFromSubjectId_ReturnsExpected(string subjectId, string[] expected)
    {
        var method = s_shapeRegistryType.GetMethod("GetMethodParameterTypesFromSubjectId", s_static,
            new[] { typeof(string) })!;
        var result = (IReadOnlyList<string>)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetToCharNativeSymbol (RuntimeHelperShapeRegistry) ───────────────

    [Theory]
    [InlineData("System.Boolean", "chaos_convert_tochar_boolean")]
    [InlineData("System.Byte", "chaos_convert_tochar_byte")]
    [InlineData("System.Char", "chaos_convert_tochar_char")]
    [InlineData("System.DateTime", "chaos_convert_tochar_datetime")]
    [InlineData("System.Decimal", "chaos_convert_tochar_decimal")]
    [InlineData("System.Double", "chaos_convert_tochar_double")]
    [InlineData("System.Int16", "chaos_convert_tochar_int16")]
    [InlineData("System.Int32", "chaos_convert_tochar_int32")]
    [InlineData("System.Int64", "chaos_convert_tochar_int64")]
    [InlineData("System.Object", "chaos_convert_tochar_object")]
    [InlineData("System.SByte", "chaos_convert_tochar_sbyte")]
    [InlineData("System.Single", "chaos_convert_tochar_single")]
    [InlineData("System.String", "chaos_convert_tochar_string")]
    [InlineData("System.UInt16", "chaos_convert_tochar_uint16")]
    [InlineData("System.UInt32", "chaos_convert_tochar_uint32")]
    [InlineData("System.UInt64", "chaos_convert_tochar_uint64")]
    [InlineData("UnknownType", "chaos_convert_tochar_int32")] // fallback
    public void GetToCharNativeSymbol_ReturnsExpected(string paramType, string expected)
    {
        var method = s_shapeRegistryType.GetMethod("GetToCharNativeSymbol", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { paramType })!;
        Assert.Equal(expected, result);
    }

    // ── SanitizeForEnumName (RuntimeHelperShapeRegistry) ─────────────────

    [Theory]
    [InlineData("None", "NONE")]
    [InlineData("CamelCase", "CAMELCASE")]
    [InlineData("with-dashes", "WITH_DASHES")]
    [InlineData("with spaces", "WITH_SPACES")]
    [InlineData("ABC123", "ABC123")]
    [InlineData("", "")]
    public void SanitizeForEnumName_ReturnsExpected(string name, string expected)
    {
        var method = s_shapeRegistryType.GetMethod("SanitizeForEnumName", s_static,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { name })!;
        Assert.Equal(expected, result);
    }
}

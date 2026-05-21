using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class ManagedNamingTests
{
    private static ManagedMethodModel MakeMethod(
        string assemblyName = "TestAssembly",
        string declaringTypeDisplayName = "MyClass",
        string name = "DoSomething",
        string returnType = "System.Int32",
        string subjectId = "TestAssembly/MyClass::DoSomething:System.Int32()",
        IReadOnlyList<ManagedParameterModel>? parameters = null)
    {
        return new ManagedMethodModel
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = "TestAssembly/MyClass",
            DeclaringTypeDisplayName = declaringTypeDisplayName,
            Name = name,
            ReturnType = returnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            Signature = subjectId,
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = 1,
            Parameters = parameters ?? Array.Empty<ManagedParameterModel>(),
            Body = new ManagedMethodBodyModel
            {
                Blocks = Array.Empty<ManagedBlockModel>(),
                ExceptionRegions = Array.Empty<ManagedExceptionRegionModel>(),
            },
        };
    }
    // ── CreateTypeSubjectId ─────────────────────────────────────

    [Fact]
    public void CreateTypeSubjectId_WithNamespace_IncludesNamespace()
    {
        var result = ManagedNaming.CreateTypeSubjectId("TestAssembly", "MyApp.Models", "MyClass");
        Assert.Equal("TestAssembly/MyApp.Models.MyClass", result);
    }

    [Fact]
    public void CreateTypeSubjectId_NullNamespace_UsesTypeNameOnly()
    {
        var result = ManagedNaming.CreateTypeSubjectId("TestAssembly", null, "MyClass");
        Assert.Equal("TestAssembly/MyClass", result);
    }

    [Fact]
    public void CreateTypeSubjectId_EmptyNamespace_UsesTypeNameOnly()
    {
        var result = ManagedNaming.CreateTypeSubjectId("TestAssembly", "", "MyClass");
        Assert.Equal("TestAssembly/MyClass", result);
    }

    [Fact]
    public void CreateTypeSubjectId_NamespaceEqualsAssembly_UsesTypeNameOnly()
    {
        var result = ManagedNaming.CreateTypeSubjectId("System.Private.CoreLib", "System.Private.CoreLib", "Object");
        Assert.Equal("System.Private.CoreLib/Object", result);
    }

    // ── CreateTypeDisplayName ────────────────────────────────────

    [Fact]
    public void CreateTypeDisplayName_WithNamespace_ReturnsQualified()
    {
        Assert.Equal("System.String", ManagedNaming.CreateTypeDisplayName(
            "System.Private.CoreLib", "System", "String"));
    }

    [Fact]
    public void CreateTypeDisplayName_NullNamespace_ReturnsTypeName()
    {
        Assert.Equal("MyClass", ManagedNaming.CreateTypeDisplayName("Asm", null, "MyClass"));
    }

    [Fact]
    public void CreateTypeDisplayName_NamespaceEqualsAssembly_ReturnsTypeName()
    {
        Assert.Equal("MyClass", ManagedNaming.CreateTypeDisplayName("MyAssembly", "MyAssembly", "MyClass"));
    }

    // ── CreateFieldSubjectId ─────────────────────────────────────

    [Fact]
    public void CreateFieldSubjectId_ReturnsExpectedFormat()
    {
        Assert.Equal("TestAssembly/MyType::_myField",
            ManagedNaming.CreateFieldSubjectId("TestAssembly/MyType", "_myField"));
    }

    // ── CreatePropertySubjectId ──────────────────────────────────

    [Fact]
    public void CreatePropertySubjectId_NoIndexParams_ReturnsSimple()
    {
        Assert.Equal("Test/Type::property:Name",
            ManagedNaming.CreatePropertySubjectId("Test/Type", "Name"));
    }

    [Fact]
    public void CreatePropertySubjectId_WithIndexParams_IncludesTypes()
    {
        Assert.Equal("Test/Type::property:Item[System.Int32,System.String]",
            ManagedNaming.CreatePropertySubjectId("Test/Type", "Item",
                new[] { "System.Int32", "System.String" }));
    }

    [Fact]
    public void CreatePropertySubjectId_EmptyIndexParams_ReturnsSimple()
    {
        Assert.Equal("Test/Type::property:Name",
            ManagedNaming.CreatePropertySubjectId("Test/Type", "Name",
                Array.Empty<string>()));
    }

    // ── CreateMethodSubjectId ────────────────────────────────────

    [Fact]
    public void CreateMethodSubjectId_NoGenericParams_ReturnsPlain()
    {
        Assert.Equal("Test/Type::Method:System.Void(System.Int32,System.String)",
            ManagedNaming.CreateMethodSubjectId("Test/Type", "Method", "System.Void",
                new[] { "System.Int32", "System.String" }));
    }

    [Fact]
    public void CreateMethodSubjectId_WithGenericParams_IncludesArity()
    {
        Assert.Equal("Test/Type::Method`2:System.Void(System.Int32)",
            ManagedNaming.CreateMethodSubjectId("Test/Type", "Method`2", "System.Void",
                new[] { "System.Int32" }, genericParameterCount: 2));
    }

    [Fact]
    public void CreateMethodSubjectId_ZeroParams_ReturnsEmptyParens()
    {
        Assert.Equal("Test/Type::Method:System.Int32()",
            ManagedNaming.CreateMethodSubjectId("Test/Type", "Method", "System.Int32",
                Array.Empty<string>()));
    }

    // ── CreateParameterSubjectId ─────────────────────────────────

    [Fact]
    public void CreateParameterSubjectId_ReturnsExpectedFormat()
    {
        var methodId = ManagedNaming.CreateMethodSubjectId("Test/Type", "Foo", "System.Void",
            new[] { "System.Int32" });
        Assert.Equal($"{methodId}::parameter[0]:arg",
            ManagedNaming.CreateParameterSubjectId(methodId, 0, "arg"));
    }

    // ── CreateMethodSignature ────────────────────────────────────

    [Fact]
    public void CreateMethodSignature_ReturnsExpectedFormat()
    {
        Assert.Equal("System.Int32 Test.Class::Add(System.Int32,System.Int32)",
            ManagedNaming.CreateMethodSignature("System.Int32", "Test.Class", "Add",
                new[] { "System.Int32", "System.Int32" }));
    }

    // ── CreateInstantiatedTypeSubjectId / DisplayName ────────────

    [Fact]
    public void CreateInstantiatedTypeSubjectId_IncludesAngleBrackets()
    {
        Assert.Equal("Test/GenericClass<System.Int32,System.String>",
            ManagedNaming.CreateInstantiatedTypeSubjectId("Test/GenericClass`2",
                new[] { "System.Int32", "System.String" }));
    }

    [Fact]
    public void CreateInstantiatedTypeDisplayName_IncludesAngleBrackets()
    {
        Assert.Equal("GenericClass<System.Int32>",
            ManagedNaming.CreateInstantiatedTypeDisplayName("GenericClass`1",
                new[] { "System.Int32" }));
    }

    // ── CreateGenericMethodName ─────────────────────────────────

    [Fact]
    public void CreateGenericMethodName_IncludesAngleBrackets()
    {
        Assert.Equal("Method<System.Int32,System.String>",
            ManagedNaming.CreateGenericMethodName("Method",
                new[] { "System.Int32", "System.String" }));
    }

    // ── CreateMethodIdentityName ─────────────────────────────────

    [Fact]
    public void CreateMethodIdentityName_NoGenericParams_ReturnsPlainName()
    {
        Assert.Equal("Method", ManagedNaming.CreateMethodIdentityName("Method"));
    }

    [Fact]
    public void CreateMethodIdentityName_WithGenericParams_StripsArity()
    {
        Assert.Equal("Method`2", ManagedNaming.CreateMethodIdentityName("Method`2", 2));
    }

    [Fact]
    public void CreateMethodIdentityName_ZeroGenericParams_ReturnsPlain()
    {
        Assert.Equal("Method", ManagedNaming.CreateMethodIdentityName("Method", 0));
    }

    // ── StripGenericArity ────────────────────────────────────────

    [Fact]
    public void StripGenericArity_NoBacktick_ReturnsSame()
    {
        Assert.Equal("System.String", ManagedNaming.StripGenericArity("System.String"));
    }

    [Fact]
    public void StripGenericArity_WithBacktick_StripsArity()
    {
        Assert.Equal("System.Collections.Generic.List",
            ManagedNaming.StripGenericArity("System.Collections.Generic.List`1"));
    }

    [Fact]
    public void StripGenericArity_MultipleBackticks_StripsAll()
    {
        Assert.Equal("Dict",
            ManagedNaming.StripGenericArity("Dict``2"));
    }

    // ── CanonicalizeAssemblyName ────────────────────────────────

    [Theory]
    [InlineData("System.Runtime", "System.Private.CoreLib")]
    [InlineData("mscorlib", "System.Private.CoreLib")]
    [InlineData("netstandard", "System.Private.CoreLib")]
    [InlineData("System.Runtime.InteropServices", "System.Private.CoreLib")]
    [InlineData("System.Threading", "System.Private.CoreLib")]
    [InlineData("System.Collections", "System.Private.CoreLib")]
    [InlineData("System.Private.CoreLib", "System.Private.CoreLib")]
    [InlineData("Unknown.Assembly", "Unknown.Assembly")]
    [InlineData("", "")]
    [InlineData(null, null)]
    public void CanonicalizeAssemblyName_ReturnsExpected(string? input, string? expected)
    {
        Assert.Equal(expected, ManagedNaming.CanonicalizeAssemblyName(input!));
    }

    // ── NormalizeSubjectIdAssembly ──────────────────────────────

    [Fact]
    public void NormalizeSubjectIdAssembly_KnownAlias_Canonicalizes()
    {
        // NormalizeSubjectIdAssembly only changes the assembly prefix,
        // the rest of the subject ID (including return type) stays intact.
        Assert.Equal(
            "System.Private.CoreLib/System.String::Concat:System.Void(System.String,System.String)",
            ManagedNaming.NormalizeSubjectIdAssembly(
                "System.Runtime/System.String::Concat:System.Void(System.String,System.String)"));
    }

    [Fact]
    public void NormalizeSubjectIdAssembly_AlreadyCanonical_ReturnsSame()
    {
        var input = "System.Private.CoreLib/System.String::Method()";
        Assert.Same(input, ManagedNaming.NormalizeSubjectIdAssembly(input));
    }

    [Fact]
    public void NormalizeSubjectIdAssembly_Empty_ReturnsEmpty()
    {
        Assert.Equal("", ManagedNaming.NormalizeSubjectIdAssembly(""));
    }

    [Fact]
    public void NormalizeSubjectIdAssembly_Null_ReturnsNull()
    {
        Assert.Null(ManagedNaming.NormalizeSubjectIdAssembly(null!));
    }

    [Fact]
    public void NormalizeSubjectIdAssembly_NoSlash_ReturnsSame()
    {
        var input = "JustAString";
        Assert.Same(input, ManagedNaming.NormalizeSubjectIdAssembly(input));
    }

    // ── GetTypeDisplayNameFromSubjectId ─────────────────────────

    [Fact]
    public void GetTypeDisplayNameFromSubjectId_MethodSubjectId_ExtractsType()
    {
        Assert.Equal("System.String",
            ManagedNaming.GetTypeDisplayNameFromSubjectId(
                "System.Private.CoreLib/System.String::Concat:System.Void(System.String,System.String)"));
    }

    [Fact]
    public void GetTypeDisplayNameFromSubjectId_TypeSubjectId_ExtractsType()
    {
        Assert.Equal("System.Int32",
            ManagedNaming.GetTypeDisplayNameFromSubjectId("System.Private.CoreLib/System.Int32"));
    }

    [Fact]
    public void GetTypeDisplayNameFromSubjectId_NoSlash_ReturnsWhole()
    {
        Assert.Equal("MyClass", ManagedNaming.GetTypeDisplayNameFromSubjectId("MyClass"));
    }

    // ── MatchesMethod ──────────────────────────────────────────

    [Fact]
    public void MatchesMethod_ExactMatch_ReturnsTrue()
    {
        Assert.True(ManagedNaming.MatchesMethod(
            "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
            "System.String", "Concat", "System.String", "System.String"));
    }

    [Fact]
    public void MatchesMethod_WrongType_ReturnsFalse()
    {
        Assert.False(ManagedNaming.MatchesMethod(
            "System.Private.CoreLib/System.Int32::CompareTo:System.Int32(System.Int32)",
            "System.String", "CompareTo", "System.Int32"));
    }

    [Fact]
    public void MatchesMethod_EmptySubjectId_ReturnsFalse()
    {
        Assert.False(ManagedNaming.MatchesMethod("", "System.String", "Concat"));
    }

    // ── MatchesMethodSubjectId ─────────────────────────────────

    [Fact]
    public void MatchesMethodSubjectId_ExactMatch_ReturnsTrue()
    {
        Assert.True(ManagedNaming.MatchesMethodSubjectId(
            "Asm/Type::Method:System.Void()",
            "Asm/Type::Method:System.Void()"));
    }

    [Fact]
    public void MatchesMethodSubjectId_DifferentReturnType_ReturnsTrueWhenOneIsEmpty()
    {
        // One has return type, the other doesn't — still matches
        Assert.True(ManagedNaming.MatchesMethodSubjectId(
            "Asm/Type::Method(System.Int32)",
            "Asm/Type::Method:System.Void(System.Int32)"));
    }

    [Fact]
    public void MatchesMethodSubjectId_DifferentName_ReturnsFalse()
    {
        Assert.False(ManagedNaming.MatchesMethodSubjectId(
            "Asm/Type::Foo()",
            "Asm/Type::Bar()"));
    }

    [Fact]
    public void MatchesMethodSubjectId_MalformedInput_ReturnsFalse()
    {
        // Two different strings with no meaningful method structure - should not match
        Assert.False(ManagedNaming.MatchesMethodSubjectId("NoColon", "NoSlash"));
    }

    // ── GetMethodSubjectIdDisplayString ─────────────────────────

    [Fact]
    public void GetMethodSubjectIdDisplayString_ValidSubjectId_ReturnsFormatted()
    {
        Assert.Equal("Asm/Type::Method(Int32,String)",
            ManagedNaming.GetMethodSubjectIdDisplayString(
                "Asm/Type::Method:System.Void(Int32,String)"));
    }

    [Fact]
    public void GetMethodSubjectIdDisplayString_Malformed_ReturnsOriginal()
    {
        Assert.Equal("bad", ManagedNaming.GetMethodSubjectIdDisplayString("bad"));
    }

    // ── CreateMethodId (ManagedMethodModel overload) ────────────

    [Fact]
    public void CreateMethodId_WithModel_ReturnsKebabCase()
    {
        var model = MakeMethod(
            assemblyName: "TestAssembly",
            declaringTypeDisplayName: "MyClass",
            name: "DoSomething");
        Assert.Equal("test-assembly.my-class.do-something",
            ManagedNaming.CreateMethodId(model));
    }

    [Fact]
    public void CreateMethodId_NullModel_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedNaming.CreateMethodId((ManagedMethodModel)null!));
    }

    // ── CreateMethodId (string overload) ────────────────────────

    [Fact]
    public void CreateMethodId_ThreeParts_ReturnsKebabCase()
    {
        Assert.Equal("asm.type.method",
            ManagedNaming.CreateMethodId("Asm", "Type", "Method"));
    }

    // ── CreateMethodSymbol ─────────────────────────────────────

    [Fact]
    public void CreateMethodSymbol_NoParams_ReturnsBaseSymbol()
    {
        var model = MakeMethod(
            assemblyName: "TestAssembly", declaringTypeDisplayName: "MyClass",
            name: "DoSomething", returnType: "System.Int32",
            subjectId: "TestAssembly/MyClass::DoSomething:System.Int32()");
        Assert.Equal("TestAssembly_MyClass_DoSomething",
            ManagedNaming.CreateMethodSymbol(model));
    }

    [Fact]
    public void CreateMethodSymbol_WithParams_IncludesParamTypes()
    {
        var model = MakeMethod(
            assemblyName: "TestAssembly", declaringTypeDisplayName: "MyClass",
            name: "Add", returnType: "System.Int32",
            subjectId: "TestAssembly/MyClass::Add:System.Int32(System.Int32,System.Int32)",
            parameters: new[]
            {
                new ManagedParameterModel { Type = "System.Int32", Name = "a" },
                new ManagedParameterModel { Type = "System.Int32", Name = "b" },
            });
        Assert.Equal("TestAssembly_MyClass_Add_System_Int32_System_Int32",
            ManagedNaming.CreateMethodSymbol(model));
    }

    // ── CreateInstantiationStubSymbol ──────────────────────────

    [Fact]
    public void CreateInstantiationStubSymbol_ReturnsExpected()
    {
        // ToSymbolPart converts "/" and "::" to underscores
        var stubId = new InstantiationStubId { Value = "test/MyType::Method" };
        Assert.Equal("chaos_test_MyType__Method",
            ManagedNaming.CreateInstantiationStubSymbol(stubId));
    }

    [Fact]
    public void CreateInstantiationStubSymbol_Null_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedNaming.CreateInstantiationStubSymbol(null!));
    }

    // ── NormalizePathForManifest ───────────────────────────────

    [Fact]
    public void NormalizePathForManifest_UnderBase_ReturnsRelative()
    {
        var result = ManagedNaming.NormalizePathForManifest(
            @"C:\work\output\build\gen\file.cpp",
            @"C:\work\output");
        Assert.Equal("build/gen/file.cpp", result);
    }

    [Fact]
    public void NormalizePathForManifest_OutsideBase_ReturnsAbsolute()
    {
        var result = ManagedNaming.NormalizePathForManifest(
            @"D:\other\file.cpp",
            @"C:\work\output");
        Assert.Equal("D:/other/file.cpp", result);
    }

    // ── CanonicalizeSubjectId ─────────────────────────────────

    [Fact]
    public void CanonicalizeSubjectId_ValidInput_ReturnsCanonical()
    {
        // No placeholder ordinals to renumber
        Assert.Equal("Asm/Type::Method()",
            ManagedNaming.CanonicalizeSubjectId("Asm/Type::Method()"));
    }

    [Fact]
    public void CanonicalizeSubjectId_Null_Throws()
    {
        // ThrowIfNullOrWhiteSpace throws ArgumentNullException for null input
        Assert.Throws<ArgumentNullException>(() =>
            ManagedNaming.CanonicalizeSubjectId(null!));
    }

    [Fact]
    public void CanonicalizeSubjectId_RenumbersPlaceholders()
    {
        // !!2 and !!5 should be renumbered to !!0 and !!1
        var result = ManagedNaming.CanonicalizeSubjectId("Test/Type::Method!!2!!5");
        Assert.Contains("!!0", result);
        Assert.Contains("!!1", result);
    }

    // ── TryCreateGenericInstantiationKey ────────────────────────

    [Fact]
    public void TryCreateGenericInstantiationKey_NonGeneric_ReturnsNull()
    {
        // Use non-method subject IDs (no "::") to avoid method parsing
        Assert.Null(ManagedNaming.TryCreateGenericInstantiationKey(
            "Asm/Type", "Asm/Type"));
    }

    // ── TryCreateGenericDiagnosticArtifact ─────────────────────

    [Fact]
    public void TryCreateGenericDiagnosticArtifact_NonGeneric_ReturnsNull()
    {
        Assert.Null(ManagedNaming.TryCreateGenericDiagnosticArtifact(
            "Asm/Type", "Asm/Type"));
    }

    // ── CreateSharedGenericBodyId / CreateInstantiationStubId ──

    [Fact]
    public void CreateSharedGenericBodyId_ReturnsBodyPrefix()
    {
        var key = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = "Asm/Type`1::Method()",
            TypeArguments = new[] { "System.Int32" },
            MethodArguments = Array.Empty<string>(),
        };
        var id = ManagedNaming.CreateSharedGenericBodyId(key);
        Assert.StartsWith("body:", id.Value);
    }

    [Fact]
    public void CreateInstantiationStubId_ReturnsStubPrefix()
    {
        var key = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = "Asm/Type`1::Method()",
            TypeArguments = new[] { "System.Int32" },
            MethodArguments = Array.Empty<string>(),
        };
        var id = ManagedNaming.CreateInstantiationStubId(key);
        Assert.StartsWith("stub:", id.Value);
    }

    [Fact]
    public void CreateSharedGenericBodyId_NullKey_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedNaming.CreateSharedGenericBodyId(null!));
    }

    // ── ToKebabCase (tested through CreateMethodId) ──────────────

    [Fact]
    public void CreateMethodId_MultiWordName_AllKebabCase()
    {
        Assert.Equal("test-assembly.my-class.my-method",
            ManagedNaming.CreateMethodId("TestAssembly", "MyClass", "MyMethod"));
    }

    [Fact]
    public void ToKebabCase_WithSpecialChars_HandlesGracefully()
    {
        // Non-alphanumeric chars become separators; "A!!" → "a", "B__" → "b", "C..D" → "c-d"
        Assert.Equal("a.b.c-d",
            ManagedNaming.CreateMethodId("A!!", "B__", "C..D"));
    }

    // ── .ctor handling in ToSymbolPart ─────────────────────────

    [Fact]
    public void CreateMethodSymbol_Ctor_ReturnsCtorSymbol()
    {
        var model = MakeMethod(
            assemblyName: "Test", declaringTypeDisplayName: "MyClass",
            name: ".ctor", returnType: "System.Void",
            subjectId: "Test/MyClass::.ctor:System.Void()");
        var symbol = ManagedNaming.CreateMethodSymbol(model);
        Assert.Contains("_ctor", symbol);
    }
}

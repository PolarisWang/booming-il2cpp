using System.Reflection;
using Xunit;

namespace Chaos.IL2CPP.Linker.Tests;

public sealed class LinkerStageHelperTests
{
    private static readonly Type s_stageType = typeof(LinkerStage);
    private const BindingFlags s_flags = BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public;

    private static T? InvokeStatic<T>(string methodName, params object?[] args)
    {
        var method = s_stageType.GetMethod(methodName, s_flags)
            ?? throw new InvalidOperationException($"Method '{methodName}' not found on LinkerStage");
        return (T?)method.Invoke(null, args);
    }

    // ──────────────────────────────────────────────
    // RequiresBoxing
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Boolean", true)]
    [InlineData("System.Byte", true)]
    [InlineData("System.Char", true)]
    [InlineData("System.Double", true)]
    [InlineData("System.Int16", true)]
    [InlineData("System.Int32", true)]
    [InlineData("System.Int64", true)]
    [InlineData("System.Single", true)]
    [InlineData("System.UInt16", true)]
    [InlineData("System.UInt32", true)]
    [InlineData("System.UInt64", true)]
    [InlineData("System.String", false)]
    [InlineData("System.Object", false)]
    [InlineData("System.IntPtr", false)]
    [InlineData("", false)]
    public void RequiresBoxing_VariousInputs(string typeName, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("RequiresBoxing", typeName));
    }

    // ──────────────────────────────────────────────
    // TryGetArrayElementType
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Int32[]", "System.Int32")]
    [InlineData("System.String[]", "System.String")]
    [InlineData("System.Int32[,]", null)]
    [InlineData("System.Int32", null)]
    [InlineData("", null)]
    public void TryGetArrayElementType_VariousInputs(string typeName, string? expected)
    {
        Assert.Equal(expected, InvokeStatic<string?>("TryGetArrayElementType", typeName));
    }

    // ──────────────────────────────────────────────
    // GetDeclaringTypeSubjectId
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::M", "A.B")]
    [InlineData("System.Private.CoreLib/System.String::Concat", "System.Private.CoreLib/System.String")]
    public void GetDeclaringTypeSubjectId_ValidInput(string subjectId, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string>("GetDeclaringTypeSubjectId", subjectId));
    }

    [Theory]
    [InlineData("")]
    [InlineData("NoSeparator")]
    public void GetDeclaringTypeSubjectId_InvalidInput_Throws(string subjectId)
    {
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<string>("GetDeclaringTypeSubjectId", subjectId));
        Assert.IsType<InvalidOperationException>(ex.InnerException);
    }

    // ──────────────────────────────────────────────
    // GetMethodSignatureSuffix
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::M", "M")]
    [InlineData("A.B::M:System.Void()", "M:System.Void()")]
    public void GetMethodSignatureSuffix_ValidInput(string subjectId, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string>("GetMethodSignatureSuffix", subjectId));
    }

    [Theory]
    [InlineData("")]
    [InlineData("NoSeparator")]
    public void GetMethodSignatureSuffix_InvalidInput_Throws(string subjectId)
    {
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<string>("GetMethodSignatureSuffix", subjectId));
        Assert.IsType<InvalidOperationException>(ex.InnerException);
    }

    // ──────────────────────────────────────────────
    // IsCompilerGeneratedAsyncStateMachine
    // ──────────────────────────────────────────────

    [Fact]
    public void IsCompilerGeneratedAsyncStateMachine_NameContains_d__()
    {
        var type = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "Something_d__1",
            DisplayName = "Something_d__1",
            NamespaceName = "TestNS",
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        Assert.True(InvokeStatic<bool>("IsCompilerGeneratedAsyncStateMachine", type));
    }

    [Fact]
    public void IsCompilerGeneratedAsyncStateMachine_NameWithout_d__()
    {
        var type = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "NormalType",
            DisplayName = "NormalType",
            NamespaceName = "TestNS",
            MetadataToken = 2,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        Assert.False(InvokeStatic<bool>("IsCompilerGeneratedAsyncStateMachine", type));
    }

    // ──────────────────────────────────────────────
    // IsMonitorEnterExitSubjectId
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Threading.Monitor::Enter(", true)]
    [InlineData("System.Threading.Monitor::Exit(", true)]
    [InlineData("System.Threading.Monitor::TryEnter(", false)]
    [InlineData("System.String", false)]
    [InlineData("", false)]
    public void IsMonitorEnterExitSubjectId_VariousInputs(string subjectId, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("IsMonitorEnterExitSubjectId", subjectId));
    }

    // ──────────────────────────────────────────────
    // IsCompilerGeneratedIteratorStateMachine
    // ──────────────────────────────────────────────

    [Fact]
    public void IsCompilerGeneratedIteratorStateMachine_ImplementsEnumerableAndEnumerator()
    {
        var type = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "Something_d__1",
            DisplayName = "Something_d__1",
            NamespaceName = "TestNS",
            MetadataToken = 3,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            ImplementedInterfaceSubjectIds = new List<string>
            {
                "System.Collections.Generic.IEnumerable<System.Int32>",
                "System.Collections.Generic.IEnumerator<System.Int32>",
            },
        };
        Assert.True(InvokeStatic<bool>("IsCompilerGeneratedIteratorStateMachine", type));
    }

    [Fact]
    public void IsCompilerGeneratedIteratorStateMachine_NoInterfaces_ReturnsFalse()
    {
        var type = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "Something_d__1",
            DisplayName = "Something_d__1",
            NamespaceName = "TestNS",
            MetadataToken = 4,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        Assert.False(InvokeStatic<bool>("IsCompilerGeneratedIteratorStateMachine", type));
    }

    [Fact]
    public void IsCompilerGeneratedIteratorStateMachine_NameWithout_d__()
    {
        var type = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "NormalType",
            DisplayName = "NormalType",
            NamespaceName = "TestNS",
            MetadataToken = 5,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            ImplementedInterfaceSubjectIds = new List<string>
            {
                "System.Collections.Generic.IEnumerable<System.Int32>",
            },
        };
        Assert.False(InvokeStatic<bool>("IsCompilerGeneratedIteratorStateMachine", type));
    }

    // ──────────────────────────────────────────────
    // ResolveLayoutDataKind
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Int32[]", "array")]
    [InlineData("System.String[]", "array")]
    [InlineData("System.Int32", "value")]
    [InlineData("System.String", "reference")]
    [InlineData("System.Object", "reference")]
    [InlineData("", "reference")]
    public void ResolveLayoutDataKind_VariousInputs(string typeName, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string>("ResolveLayoutDataKind", typeName));
    }

    // ──────────────────────────────────────────────
    // TryGetLocalIndex
    // ──────────────────────────────────────────────

    [Fact]
    public void TryGetLocalIndex_IntOperand_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { 42, null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.True(result);
        Assert.Equal(42, parameters[1]);
    }

    [Fact]
    public void TryGetLocalIndex_LongOperand_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { 99L, null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.True(result);
        Assert.Equal(99, parameters[1]);
    }

    [Fact]
    public void TryGetLocalIndex_StringOperand_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { "5", null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.True(result);
        Assert.Equal(5, parameters[1]);
    }

    [Fact]
    public void TryGetLocalIndex_NegativeInt_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { -1, null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.True(result);
        Assert.Equal(-1, parameters[1]);
    }

    [Fact]
    public void TryGetLocalIndex_NullOperand_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { null, null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.False(result);
    }

    [Fact]
    public void TryGetLocalIndex_NonParseableString_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("TryGetLocalIndex", s_flags)!;
        var parameters = new object?[] { "not-a-number", null };
        var result = (bool)method.Invoke(null, parameters)!;
        Assert.False(result);
    }

    // ──────────────────────────────────────────────
    // MatchesTypeSubjectId
    // ──────────────────────────────────────────────

    [Fact]
    public void MatchesTypeSubjectId_ExactSubjectIdMatch()
    {
        var method = s_stageType.GetMethod("MatchesTypeSubjectId", s_flags)!;
        var slotType = new Contracts.ManagedTypeModel
        {
            SubjectId = "TestType",
            DefinitionSubjectId = "TestType",
            Name = "TestType",
            DisplayName = "TestType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        var result = (bool)method.Invoke(null, new object?[] { "TestType", "TestType", slotType })!;
        Assert.True(result);
    }

    [Fact]
    public void MatchesTypeSubjectId_DefinitionSubjectIdMatch()
    {
        var method = s_stageType.GetMethod("MatchesTypeSubjectId", s_flags)!;
        var slotType = new Contracts.ManagedTypeModel
        {
            SubjectId = "FinalType",
            DefinitionSubjectId = "BaseType",
            Name = "FinalType",
            DisplayName = "FinalType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        var result = (bool)method.Invoke(null, new object?[] { "BaseType", "BaseType", slotType })!;
        Assert.True(result);
    }

    [Fact]
    public void MatchesTypeSubjectId_NoMatch_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("MatchesTypeSubjectId", s_flags)!;
        var slotType = new Contracts.ManagedTypeModel
        {
            SubjectId = "TargetType",
            DefinitionSubjectId = "TargetType",
            Name = "TargetType",
            DisplayName = "TargetType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        var result = (bool)method.Invoke(null, new object?[] { "OtherType", "OtherType", slotType })!;
        Assert.False(result);
    }

    // ──────────────────────────────────────────────
    // MatchesVirtualDispatchSignature
    // ──────────────────────────────────────────────

    private static Contracts.ManagedMethodModel MakeDispatchMethod(
        string name, bool isStatic, string returnType, params string[] paramTypes)
    {
        return new Contracts.ManagedMethodModel
        {
            SubjectId = $"Test::{name}",
            DefinitionSubjectId = $"Test::{name}",
            Signature = $"Test::{name}",
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "Test",
            DeclaringTypeDisplayName = "Test",
            Name = name,
            ReturnType = returnType,
            IsStatic = isStatic,
            IsVirtual = true,
            MetadataToken = 1,
            Parameters = paramTypes.Select((t, i) => new Contracts.ManagedParameterModel
            {
                Name = $"p{i}",
                Type = t,
            }).ToList(),
            Body = new Contracts.ManagedMethodBodyModel
            {
                Blocks = new List<Contracts.ManagedBlockModel>
                {
                    new() { BlockId = "0", Instructions = new List<Contracts.ManagedInstructionModel>() },
                },
                ExceptionRegions = new List<Contracts.ManagedExceptionRegionModel>(),
            },
        };
    }

    [Fact]
    public void MatchesVirtualDispatchSignature_MatchingMethods_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("MatchesVirtualDispatchSignature", s_flags)!;
        var candidate = MakeDispatchMethod("Execute", false, "System.Int32", "System.Int32");
        var slot = MakeDispatchMethod("Execute", false, "System.Int32", "System.Int32");
        Assert.True((bool)method.Invoke(null, new object[] { candidate, slot })!);
    }

    [Fact]
    public void MatchesVirtualDispatchSignature_DifferentName_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("MatchesVirtualDispatchSignature", s_flags)!;
        var candidate = MakeDispatchMethod("Run", false, "System.Int32");
        var slot = MakeDispatchMethod("Execute", false, "System.Int32");
        Assert.False((bool)method.Invoke(null, new object[] { candidate, slot })!);
    }

    [Fact]
    public void MatchesVirtualDispatchSignature_StaticVsVirtual_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("MatchesVirtualDispatchSignature", s_flags)!;
        var candidate = MakeDispatchMethod("Execute", true, "System.Int32");
        var slot = MakeDispatchMethod("Execute", false, "System.Int32");
        Assert.False((bool)method.Invoke(null, new object[] { candidate, slot })!);
    }

    [Fact]
    public void MatchesVirtualDispatchSignature_DifferentReturnType_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("MatchesVirtualDispatchSignature", s_flags)!;
        var candidate = MakeDispatchMethod("Execute", false, "System.Void");
        var slot = MakeDispatchMethod("Execute", false, "System.Int32");
        Assert.False((bool)method.Invoke(null, new object[] { candidate, slot })!);
    }

    [Fact]
    public void MatchesVirtualDispatchSignature_DifferentParameters_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("MatchesVirtualDispatchSignature", s_flags)!;
        var candidate = MakeDispatchMethod("Execute", false, "System.Int32", "System.Int32", "System.Int32");
        var slot = MakeDispatchMethod("Execute", false, "System.Int32", "System.Int32");
        Assert.False((bool)method.Invoke(null, new object[] { candidate, slot })!);
    }

    // ──────────────────────────────────────────────
    // ImplementsInterface
    // ──────────────────────────────────────────────

    [Fact]
    public void ImplementsInterface_TypeImplementsInterface_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("ImplementsInterface", s_flags,
            new[] { typeof(Contracts.ManagedTypeModel), typeof(string), typeof(IReadOnlyDictionary<string, Contracts.ManagedTypeModel>) })!;
        var candidateType = new Contracts.ManagedTypeModel
        {
            SubjectId = "MyType",
            DefinitionSubjectId = "MyType",
            Name = "MyType",
            DisplayName = "MyType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            ImplementedInterfaceSubjectIds = new List<string> { "System.IDisposable" },
        };
        var typeMap = new Dictionary<string, Contracts.ManagedTypeModel>
        {
            ["System.IDisposable"] = new()
            {
                SubjectId = "System.IDisposable",
                DefinitionSubjectId = "System.IDisposable",
                Name = "IDisposable",
                DisplayName = "IDisposable",
                NamespaceName = "System",
                MetadataToken = 1,
                AssemblyName = "mscorlib",
                IsInterface = true,
                IsPreserved = false,
            },
        };
        Assert.True((bool)method.Invoke(null, new object[] { candidateType, "System.IDisposable", typeMap })!);
    }

    [Fact]
    public void ImplementsInterface_TypeDoesNotImplement_ReturnsFalse()
    {
        var method = s_stageType.GetMethod("ImplementsInterface", s_flags,
            new[] { typeof(Contracts.ManagedTypeModel), typeof(string), typeof(IReadOnlyDictionary<string, Contracts.ManagedTypeModel>) })!;
        var candidateType = new Contracts.ManagedTypeModel
        {
            SubjectId = "MyType",
            DefinitionSubjectId = "MyType",
            Name = "MyType",
            DisplayName = "MyType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            ImplementedInterfaceSubjectIds = new List<string> { "System.IComparable" },
        };
        Assert.False((bool)method.Invoke(null, new object[] { candidateType, "System.IDisposable", new Dictionary<string, Contracts.ManagedTypeModel>() })!);
    }

    // ──────────────────────────────────────────────
    // IsCompatibleVirtualDispatchTargetType
    // ──────────────────────────────────────────────

    [Fact]
    public void IsCompatibleVirtualDispatchTargetType_InterfaceDispatch_Implements_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("IsCompatibleVirtualDispatchTargetType", s_flags)!;
        var candidateType = new Contracts.ManagedTypeModel
        {
            SubjectId = "MyType",
            DefinitionSubjectId = "MyType",
            Name = "MyType",
            DisplayName = "MyType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            ImplementedInterfaceSubjectIds = new List<string> { "System.IDisposable" },
        };
        var slotType = new Contracts.ManagedTypeModel
        {
            SubjectId = "System.IDisposable",
            DefinitionSubjectId = "System.IDisposable",
            Name = "IDisposable",
            DisplayName = "IDisposable",
            NamespaceName = "System",
            MetadataToken = 1,
            AssemblyName = "mscorlib",
            IsInterface = true,
            IsPreserved = false,
        };
        var typeMap = new Dictionary<string, Contracts.ManagedTypeModel>
        {
            ["System.IDisposable"] = slotType,
        };
        Assert.True((bool)method.Invoke(null, new object[] { candidateType, slotType, typeMap })!);
    }

    [Fact]
    public void IsCompatibleVirtualDispatchTargetType_ClassDispatch_DerivedType_ReturnsTrue()
    {
        var method = s_stageType.GetMethod("IsCompatibleVirtualDispatchTargetType", s_flags)!;
        var baseType = new Contracts.ManagedTypeModel
        {
            SubjectId = "BaseType",
            DefinitionSubjectId = "BaseType",
            Name = "BaseType",
            DisplayName = "BaseType",
            NamespaceName = null,
            MetadataToken = 1,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
        };
        var derivedType = new Contracts.ManagedTypeModel
        {
            SubjectId = "DerivedType",
            DefinitionSubjectId = "DerivedType",
            Name = "DerivedType",
            DisplayName = "DerivedType",
            NamespaceName = null,
            MetadataToken = 2,
            AssemblyName = "Test",
            IsInterface = false,
            IsPreserved = false,
            BaseTypeSubjectId = "BaseType",
        };
        var typeMap = new Dictionary<string, Contracts.ManagedTypeModel>
        {
            ["BaseType"] = baseType,
            ["DerivedType"] = derivedType,
        };
        Assert.True((bool)method.Invoke(null, new object[] { derivedType, baseType, typeMap })!);
    }
}

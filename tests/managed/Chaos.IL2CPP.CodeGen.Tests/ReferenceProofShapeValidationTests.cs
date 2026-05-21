using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class ReferenceProofShapeValidationTests
{
    // ── Shared reflection helpers ──────────────────────────────────────

    private static readonly Type s_emitter = typeof(NativeReferenceProofEmitter);
    private static readonly Type s_planner = typeof(NativeReferenceLoweringPlanner);
    private static readonly BindingFlags s_flags = BindingFlags.NonPublic | BindingFlags.Static;

    private static MethodInfo GetPredicate(string name, params Type[] paramTypes)
    {
        return s_emitter.GetMethod(name, s_flags, paramTypes)
            ?? throw new InvalidOperationException($"Method '{name}' not found on NativeReferenceProofEmitter");
    }

    private static MethodInfo GetShapeMethod(string name, params Type[] paramTypes)
    {
        return s_planner.GetMethod(name, s_flags, paramTypes)
            ?? throw new InvalidOperationException($"Method '{name}' not found on NativeReferenceLoweringPlanner");
    }

    private static bool InvokeBool(MethodInfo method, params object?[] args)
    {
        return (bool)method.Invoke(null, args)!;
    }

    // ════════════════════════════════════════════════════════════════════
    // NativeReferenceProofEmitter.DirectReferenceReturnTypePredicates
    // ════════════════════════════════════════════════════════════════════

    // ── IsEncodingDirectReferenceReturnType ────────────────────────────

    [Theory]
    [InlineData("System.Text.Encoding", true)]
    [InlineData("System.Text.StringBuilder", true)]
    [InlineData("System.Text.UTF8Encoding", true)]
    [InlineData("System.String", true)]
    [InlineData("System.Object", true)]
    [InlineData("System.IO.Stream", false)]
    [InlineData("System.Int32", false)]
    [InlineData("CustomType", false)]
    public void IsEncodingDirectReferenceReturnType_ReturnsExpected(string managedType, bool expected)
    {
        var method = GetPredicate("IsEncodingDirectReferenceReturnType", typeof(string));
        Assert.Equal(expected, InvokeBool(method, managedType));
    }

    // ── IsIODirectReferenceReturnType ─────────────────────────────────

    [Theory]
    [InlineData("System.IO.Stream", true)]
    [InlineData("System.IO.FileStream", true)]
    [InlineData("System.IO.MemoryStream", true)]
    [InlineData("System.String", true)]
    [InlineData("System.Object", true)]
    [InlineData("System.Text.Encoding", false)]
    [InlineData("System.Int32", false)]
    [InlineData("System.IO", false)]
    public void IsIODirectReferenceReturnType_ReturnsExpected(string managedType, bool expected)
    {
        var method = GetPredicate("IsIODirectReferenceReturnType", typeof(string));
        Assert.Equal(expected, InvokeBool(method, managedType));
    }

    // ── IsTracingDirectReferenceReturnType ────────────────────────────

    [Theory]
    [InlineData("System.Diagnostics.Tracing.EventSource", true)]
    [InlineData("System.Diagnostics.Tracing.EventCounter", true)]
    [InlineData("System.String", true)]
    [InlineData("System.Object", true)]
    [InlineData("System.Diagnostics.Tracing", false)]
    [InlineData("System.Text.Encoding", false)]
    [InlineData("System.Int32", false)]
    public void IsTracingDirectReferenceReturnType_ReturnsExpected(string managedType, bool expected)
    {
        var method = GetPredicate("IsTracingDirectReferenceReturnType", typeof(string));
        Assert.Equal(expected, InvokeBool(method, managedType));
    }

    // ── IsReflectionDirectReferenceReturnType ─────────────────────────

    [Theory]
    [InlineData("System.Reflection.Assembly", true)]
    [InlineData("System.Reflection.MethodInfo", true)]
    [InlineData("System.RuntimeType", true)]
    [InlineData("System.RuntimeTypeHandle", true)]
    [InlineData("System.Type", true)]
    [InlineData("System.TypeCode", true)]
    [InlineData("System.String", true)]
    [InlineData("System.Object", true)]
    [InlineData("System.IO.Stream", false)]
    [InlineData("System.Text.Encoding", false)]
    [InlineData("System.Int32", false)]
    public void IsReflectionDirectReferenceReturnType_ReturnsExpected(string managedType, bool expected)
    {
        var method = GetPredicate("IsReflectionDirectReferenceReturnType", typeof(string));
        Assert.Equal(expected, InvokeBool(method, managedType));
    }

    // ── IsStringBuilderDirectReferenceReturnType ──────────────────────

    [Theory]
    [InlineData("System.Text.StringBuilder", true)]
    [InlineData("System.String", true)]
    [InlineData("System.Object", true)]
    [InlineData("System.Text.Encoding", false)]
    [InlineData("System.IO.Stream", false)]
    [InlineData("System.Int32", false)]
    public void IsStringBuilderDirectReferenceReturnType_ReturnsExpected(string managedType, bool expected)
    {
        var method = GetPredicate("IsStringBuilderDirectReferenceReturnType", typeof(string));
        Assert.Equal(expected, InvokeBool(method, managedType));
    }

    // ════════════════════════════════════════════════════════════════════
    // NativeReferenceLoweringPlanner.ShapeValidation
    // ════════════════════════════════════════════════════════════════════

    // ── IsConstructorThenInstanceCallEntryPointShape (ManagedInstructionModel) ──

    private static IReadOnlyList<ManagedInstructionModel> CtorCallManagedInstrs() => new ManagedInstructionModel[]
    {
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Managed_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<ManagedInstructionModel>));
        Assert.True(InvokeBool(method, new object[] { CtorCallManagedInstrs() }));
    }

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Managed_WrongCount_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<ManagedInstructionModel>));
        var shortList = new ManagedInstructionModel[]
        {
            new() { Op = "ldstr" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { shortList }));
    }

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Managed_WrongFirstOp_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<ManagedInstructionModel>));
        var wrongOp = new ManagedInstructionModel[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newobj" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { wrongOp }));
    }

    // ── IsConstructorThenInstanceCallEntryPointShape (TypedIlInstructionArtifact) ──

    private static IReadOnlyList<TypedIlInstructionArtifact> CtorCallTypedInstrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Typed_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { CtorCallTypedInstrs() }));
    }

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Typed_WrongCount_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var shortList = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { shortList }));
    }

    [Fact]
    public void IsConstructorThenInstanceCallEntryPointShape_Typed_WrongLastOp_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var wrongOp = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "br" }, // should be "ret"
        };
        Assert.False(InvokeBool(method, new object[] { wrongOp }));
    }

    // ── IsConstructorThenInstanceCallProofEntryPointShape (both overloads) ──

    [Fact]
    public void IsConstructorThenInstanceCallProofEntryPointShape_Managed_DelegatesToBase()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallProofEntryPointShape",
            typeof(IReadOnlyList<ManagedInstructionModel>));
        Assert.True(InvokeBool(method, new object[] { CtorCallManagedInstrs() }));
    }

    [Fact]
    public void IsConstructorThenInstanceCallProofEntryPointShape_Typed_DelegatesToBase()
    {
        var method = GetShapeMethod(
            "IsConstructorThenInstanceCallProofEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { CtorCallTypedInstrs() }));
    }

    // ── IsStaticCallCtorGetterEntryPointShape ──────────────────────────

    /// <summary>
    /// Pattern: ldstr, call, newobj, call/callvirt, call, ldc.i4, ret (7 instructions)
    /// </summary>
    private static IReadOnlyList<TypedIlInstructionArtifact> StaticCtorGetterInstrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldstr" },
        new() { Op = "call" },
        new() { Op = "newobj" },
        new() { Op = "call" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsStaticCallCtorGetterEntryPointShape_Valid_Call_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsStaticCallCtorGetterEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { StaticCtorGetterInstrs() }));
    }

    [Fact]
    public void IsStaticCallCtorGetterEntryPointShape_Valid_Callvirt_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsStaticCallCtorGetterEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "call" },
            new() { Op = "newobj" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsStaticCallCtorGetterEntryPointShape_WrongIndex3Op_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsStaticCallCtorGetterEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "call" },
            new() { Op = "newobj" },
            new() { Op = "newobj" }, // should be call or callvirt
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsStaticCallCtorGetterEntryPointShape_WrongCount_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsStaticCallCtorGetterEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "call" },
            new() { Op = "newobj" },
            new() { Op = "call" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { instructions }));
    }

    // ── IsArrayBoxingReferenceArrayEntryPointShape (ManagedInstructionModel) ──

    private static IReadOnlyList<ManagedInstructionModel> ArrayBoxingManagedInstrs() => new ManagedInstructionModel[]
    {
        new() { Op = "ldc.i4" },
        new() { Op = "box" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "newarr" },
        new() { Op = "dup" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "stelem.ref" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldelem.ref" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsArrayBoxingReferenceArrayEntryPointShape_Managed_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayBoxingReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<ManagedInstructionModel>));
        Assert.True(InvokeBool(method, new object[] { ArrayBoxingManagedInstrs() }));
    }

    // ── IsArrayBoxingReferenceArrayEntryPointShape (TypedIlInstructionArtifact) ──

    private static IReadOnlyList<TypedIlInstructionArtifact> ArrayBoxingTypedInstrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldc.i4" },
        new() { Op = "box" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "newarr" },
        new() { Op = "dup" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "stelem.ref" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldelem.ref" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsArrayBoxingReferenceArrayEntryPointShape_Typed_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayBoxingReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { ArrayBoxingTypedInstrs() }));
    }

    // ── IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape (TypedIlInstructionArtifact) ──

    private static IReadOnlyList<TypedIlInstructionArtifact> ArrayCopyLenOnlyInt32Instrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldc.i4" },
        new() { Op = "newarr" },
        new() { Op = "dup" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "stelem.ref" },
        new() { Op = "ldc.i4" },
        new() { Op = "newarr" },
        new() { Op = "stloc" },
        new() { Op = "ldloc" },
        new() { Op = "ldc.i4" },
        new() { Op = "call" },
        new() { Op = "ldloc" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldelem.ref" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsArrayCopyReferenceArrayEntryPointShape_LengthOnlyInt32_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayCopyReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { ArrayCopyLenOnlyInt32Instrs() }));
    }

    // ── IsPInvokeDllImportMinimalEntryPointShape ──────────────────────

    private static IReadOnlyList<TypedIlInstructionArtifact> PInvokeMinimalInstrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldc.i4" },
        new() { Op = "ldc.i4" },
        new() { Op = "ldc.i4" },
        new() { Op = "call" },
        new() { Op = "ret" },
    };

    [Fact]
    public void IsPInvokeDllImportMinimalEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsPInvokeDllImportMinimalEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        Assert.True(InvokeBool(method, new object[] { PInvokeMinimalInstrs() }));
    }

    [Fact]
    public void IsPInvokeDllImportMinimalEntryPointShape_WrongCount_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsPInvokeDllImportMinimalEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var tooFew = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "call" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { tooFew }));
    }

    // ── IsCallLikeOp ──────────────────────────────────────────────────

    [Theory]
    [InlineData("call", true)]
    [InlineData("callvirt", true)]
    [InlineData("Call", false)]
    [InlineData("newobj", false)]
    [InlineData("ldstr", false)]
    [InlineData("", false)]
    [InlineData(null, false)]
    public void IsCallLikeOp_ReturnsExpected(string? op, bool expected)
    {
        var method = GetShapeMethod("IsCallLikeOp", typeof(string));
        var result = (bool)method.Invoke(null, new object?[] { op })!;
        Assert.Equal(expected, result);
    }

    // ── IsSpecialMethodName ───────────────────────────────────────────

    [Theory]
    [InlineData(".ctor", true)]
    [InlineData(".cctor", true)]
    [InlineData("<Main>$", true)]
    [InlineData("get_Value", true)]
    [InlineData("set_Value", true)]
    [InlineData("add_Event", true)]
    [InlineData("remove_Event", true)]
    [InlineData("ToString", false)]
    [InlineData("Main", false)]
    [InlineData(null, true)]
    [InlineData("", true)]
    public void IsSpecialMethodName_ReturnsExpected(string? name, bool expected)
    {
        var method = GetShapeMethod("IsSpecialMethodName", typeof(string));
        var result = (bool)method.Invoke(null, new object?[] { name })!;
        Assert.Equal(expected, result);
    }

    // ── IsCompilerGeneratedMemberName ─────────────────────────────────

    [Theory]
    [InlineData("<Main>$", true)]
    [InlineData("<>c", true)]
    [InlineData(null, true)]
    [InlineData("", true)]
    [InlineData("  ", true)]
    [InlineData("ToString", false)]
    [InlineData("Main", false)]
    public void IsCompilerGeneratedMemberName_ReturnsExpected(string? name, bool expected)
    {
        var method = GetShapeMethod("IsCompilerGeneratedMemberName", typeof(string));
        var result = (bool)method.Invoke(null, new object?[] { name })!;
        Assert.Equal(expected, result);
    }

    // ── GetDeclaringTypeSubjectId ─────────────────────────────────────

    [Theory]
    [InlineData("System.Console::WriteLine", "System.Console")]
    [InlineData("MyApp.MyClass::MyMethod", "MyApp.MyClass")]
    [InlineData("System.Array::Copy", "System.Array")]
    public void GetDeclaringTypeSubjectId_ExtractsCorrectly(string subjectId, string expected)
    {
        var method = GetShapeMethod("GetDeclaringTypeSubjectId", typeof(string));
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── GetMethodName ─────────────────────────────────────────────────

    [Theory]
    [InlineData("System.Console::WriteLine:System.Void(System.String)", "WriteLine")]
    [InlineData("MyType::MyMethod:System.Int32(System.Int32)", "MyMethod")]
    [InlineData("Ns.T::DoIt:System.Void()", "DoIt")]
    public void GetMethodName_ExtractsCorrectly(string subjectId, string expected)
    {
        var method = GetShapeMethod("GetMethodName", typeof(string));
        var result = (string)method.Invoke(null, new object[] { subjectId })!;
        Assert.Equal(expected, result);
    }

    // ── IsSupportedArrayCopyMethodSubjectId ───────────────────────────

    [Theory]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int64)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", true)]
    [InlineData("mscorlib/System.Array::ConstrainedCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::CopyTo:System.Void(System.Array,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::CopyTo:System.Void(System.Array,System.Int64)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Int32)", false)]
    [InlineData("mscorlib/System.Array::Reverse:System.Void(System.Array)", false)]
    [InlineData("mscorlib/System.String::Concat", false)]
    public void IsSupportedArrayCopyMethodSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = GetShapeMethod("IsSupportedArrayCopyMethodSubjectId", typeof(string));
        Assert.Equal(expected, InvokeBool(method, subjectId));
    }

    // ── IsSupportedArrayReverseMethodSubjectId ────────────────────────

    [Theory]
    [InlineData("mscorlib/System.Array::Reverse:System.Void(System.Array)", true)]
    [InlineData("mscorlib/System.Array::Reverse:System.Void(System.Array,System.Int32,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)", false)]
    [InlineData("mscorlib/System.String::Concat", false)]
    public void IsSupportedArrayReverseMethodSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = GetShapeMethod("IsSupportedArrayReverseMethodSubjectId", typeof(string));
        Assert.Equal(expected, InvokeBool(method, subjectId));
    }

    // ── IsSupportedArrayClearMethodSubjectId ──────────────────────────

    [Theory]
    [InlineData("mscorlib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)", true)]
    [InlineData("mscorlib/System.Array::Clear:System.Void(System.Array)", true)]
    [InlineData("mscorlib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)", false)]
    [InlineData("mscorlib/System.Array::Reverse:System.Void(System.Array)", false)]
    public void IsSupportedArrayClearMethodSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = GetShapeMethod("IsSupportedArrayClearMethodSubjectId", typeof(string));
        Assert.Equal(expected, InvokeBool(method, subjectId));
    }

    // ── IsDelegateClosedTargetRelayEntryPointShape ────────────────────

    [Fact]
    public void IsDelegateClosedTargetRelayEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsDelegateClosedTargetRelayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "ldftn" },
            new() { Op = "newobj" },
            new() { Op = "stloc" },
            new() { Op = "ldsfld" },
            new() { Op = "dup" },
            new() { Op = "brtrue" },
            new() { Op = "pop" },
            new() { Op = "ldnull" },
            new() { Op = "ldftn" },
            new() { Op = "newobj" },
            new() { Op = "dup" },
            new() { Op = "stsfld" },
            new() { Op = "ldloc" },
            new() { Op = "ldstr" },
            new() { Op = "callvirt" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsDelegateClosedTargetRelayEntryPointShape_WrongCount_ReturnsFalse()
    {
        var method = GetShapeMethod(
            "IsDelegateClosedTargetRelayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var tooFew = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "ret" },
        };
        Assert.False(InvokeBool(method, new object[] { tooFew }));
    }

    // ── GetDelegateClosedTargetRelay helper indices ───────────────────

    [Fact]
    public void GetDelegateClosedTargetRelayStaticMethodInstructionIndex_Returns10()
    {
        var method = GetShapeMethod(
            "GetDelegateClosedTargetRelayStaticMethodInstructionIndex",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var result = (int)method.Invoke(null, new object[] { DelegateRelayInstrs() })!;
        Assert.Equal(10, result);
    }

    [Fact]
    public void GetDelegateClosedTargetRelayWriteLineInstructionIndex_Returns18()
    {
        var method = GetShapeMethod(
            "GetDelegateClosedTargetRelayWriteLineInstructionIndex",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var result = (int)method.Invoke(null, new object[] { DelegateRelayInstrs() })!;
        Assert.Equal(18, result);
    }

    [Fact]
    public void GetDelegateClosedTargetRelayReturnValueInstructionIndex_Returns19()
    {
        var method = GetShapeMethod(
            "GetDelegateClosedTargetRelayReturnValueInstructionIndex",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var result = (int)method.Invoke(null, new object[] { DelegateRelayInstrs() })!;
        Assert.Equal(19, result);
    }

    [Fact]
    public void GetDelegateClosedTargetRelayMessagePrefixInstructionIndex_Returns15()
    {
        var method = GetShapeMethod(
            "GetDelegateClosedTargetRelayMessagePrefixInstructionIndex",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var result = (int)method.Invoke(null, new object[] { DelegateRelayInstrs() })!;
        Assert.Equal(15, result);
    }

    /// <summary>
    /// Shared valid 21-instruction delegate closed-target relay instruction list.
    /// </summary>
    private static IReadOnlyList<TypedIlInstructionArtifact> DelegateRelayInstrs() => new TypedIlInstructionArtifact[]
    {
        new() { Op = "ldstr" },
        new() { Op = "newobj" },
        new() { Op = "ldftn" },
        new() { Op = "newobj" },
        new() { Op = "stloc" },
        new() { Op = "ldsfld" },
        new() { Op = "dup" },
        new() { Op = "brtrue" },
        new() { Op = "pop" },
        new() { Op = "ldnull" },
        new() { Op = "ldftn" },
        new() { Op = "newobj" },
        new() { Op = "dup" },
        new() { Op = "stsfld" },
        new() { Op = "ldloc" },
        new() { Op = "ldstr" },
        new() { Op = "callvirt" },
        new() { Op = "callvirt" },
        new() { Op = "call" },
        new() { Op = "ldc.i4" },
        new() { Op = "ret" },
    };

    // ── ArrayCopyTo / ArrayReverse / ArrayClear shape variants ────────

    [Fact]
    public void IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "dup" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "stelem.ref" },
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "stloc" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "callvirt" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldelem.ref" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsArrayReverseWholeReferenceArrayEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayReverseWholeReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "dup" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "stelem.ref" },
            new() { Op = "dup" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldelem.ref" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsArrayReverseRangeReferenceArrayEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayReverseRangeReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "dup" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldstr" },
            new() { Op = "newobj" },
            new() { Op = "stelem.ref" },
            new() { Op = "dup" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldc.i4" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldelem.ref" },
            new() { Op = "callvirt" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsArrayClearWholeReferenceArrayEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayClearWholeReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "stloc" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldstr" },
            new() { Op = "stelem.ref" },
            new() { Op = "ldloc" },
            new() { Op = "call" },
            new() { Op = "ldstr" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldelem.ref" },
            new() { Op = "call" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    [Fact]
    public void IsArrayClearRangeReferenceArrayEntryPointShape_Valid_ReturnsTrue()
    {
        var method = GetShapeMethod(
            "IsArrayClearRangeReferenceArrayEntryPointShape",
            typeof(IReadOnlyList<TypedIlInstructionArtifact>));
        var instructions = new TypedIlInstructionArtifact[]
        {
            new() { Op = "ldc.i4" },
            new() { Op = "newarr" },
            new() { Op = "stloc" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldstr" },
            new() { Op = "stelem.ref" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldc.i4" },
            new() { Op = "call" },
            new() { Op = "ldstr" },
            new() { Op = "ldloc" },
            new() { Op = "ldc.i4" },
            new() { Op = "ldelem.ref" },
            new() { Op = "call" },
            new() { Op = "call" },
            new() { Op = "ldc.i4" },
            new() { Op = "ret" },
        };
        Assert.True(InvokeBool(method, new object[] { instructions }));
    }

    // ── GetSingleBlockInstructions / TryGetSingleBlockInstructions ────

    [Fact]
    public void GetSingleBlockInstructions_SingleBlock_ReturnsInstructions()
    {
        var method = GetShapeMethod(
            "GetSingleBlockInstructions",
            typeof(TypedIlMethodArtifact));
        var artifact = new TypedIlMethodArtifact
        {
            MethodId = "test",
            SubjectId = "Test::Method()",
            Signature = "Test::Method()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "Test",
                DeclaringTypeSubjectId = "Test",
                DefinitionSubjectId = "Test::Method()",
                SubjectId = "Test::Method()",
                MethodId = "Test::Method()",
                Signature = "Test::Method()",
            },
            MethodRole = "static-method",
            BodyAvailability = "has-canonical-body",
            BodyAvailabilityCode = BodyAvailabilityCode.InterpreterReady,
            Capabilities = Array.Empty<string>(),
            Parameters = Array.Empty<TypedIlParameterArtifact>(),
            Blocks = new TypedIlBlockArtifact[]
            {
                new()
                {
                    BlockId = "IL0000",
                    Instructions = new TypedIlInstructionArtifact[]
                    {
                        new() { Op = "ret" },
                    },
                },
            },
        };
        var result = (IReadOnlyList<TypedIlInstructionArtifact>)method.Invoke(null, new object[] { artifact })!;
        Assert.Single(result);
        Assert.Equal("ret", result[0].Op);
    }

    [Fact]
    public void TryGetSingleBlockInstructions_NullMethod_ReturnsNull()
    {
        var method = GetShapeMethod(
            "TryGetSingleBlockInstructions",
            typeof(ManagedMethodModel));
        var result = method.Invoke(null, new object?[] { null });
        Assert.Null(result);
    }

    // ── LooksLikeInterfaceMethodSubjectId ─────────────────────────────

    [Theory]
    [InlineData("Ns.IComparable::CompareTo", true)]
    [InlineData("Ns.IDisposable::Dispose", true)]
    [InlineData("Ns.ICloneable::Clone", true)]
    [InlineData("Ns.NotInterface::Method", false)]
    [InlineData("Ns.I::Method", false)]  // single char after I
    public void LooksLikeInterfaceMethodSubjectId_ReturnsExpected(string subjectId, bool expected)
    {
        var method = GetShapeMethod("LooksLikeInterfaceMethodSubjectId", typeof(string));
        Assert.Equal(expected, InvokeBool(method, subjectId));
    }
}

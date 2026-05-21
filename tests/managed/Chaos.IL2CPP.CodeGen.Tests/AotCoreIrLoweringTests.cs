using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class AotCoreIrLoweringTests
{
    private static readonly Type s_t = typeof(AotCoreIrLowering);
    private static readonly BindingFlags s_flags = BindingFlags.NonPublic | BindingFlags.Static;

    // ── IsBlittablePrimitiveType ────────────────────────────────────────

    [Theory]
    [InlineData("System.Boolean", true)]
    [InlineData("System.Byte", true)]
    [InlineData("System.SByte", true)]
    [InlineData("System.Int16", true)]
    [InlineData("System.UInt16", true)]
    [InlineData("System.Int32", true)]
    [InlineData("System.UInt32", true)]
    [InlineData("System.Int64", true)]
    [InlineData("System.UInt64", true)]
    [InlineData("System.IntPtr", true)]
    [InlineData("System.UIntPtr", true)]
    [InlineData("System.Single", true)]
    [InlineData("System.Double", true)]
    [InlineData("System.Char", true)]
    [InlineData("System.String", false)]
    [InlineData("System.Object", false)]
    [InlineData("CustomType", false)]
    public void IsBlittablePrimitiveType_ReturnsExpected(string typeIdentity, bool expected)
    {
        var method = s_t.GetMethod("IsBlittablePrimitiveType", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { typeIdentity })!;
        Assert.Equal(expected, result);
    }

    // ── IsKnownValueTypeIdentity ────────────────────────────────────────

    [Theory]
    [InlineData("System.Boolean", true)]
    [InlineData("System.Int32", true)]
    [InlineData("System.Double", true)]
    [InlineData("System.DateTime", false)]
    [InlineData("System.Decimal", false)]
    [InlineData("System.Guid", false)]
    [InlineData("System.IntPtr", true)]
    [InlineData("System.UIntPtr", true)]
    [InlineData("System.String", false)]
    [InlineData("System.Object", false)]
    [InlineData("CustomStruct", false)]
    public void IsKnownValueTypeIdentity_ReturnsExpected(string identity, bool expected)
    {
        var method = s_t.GetMethod("IsKnownValueTypeIdentity", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { identity })!;
        Assert.Equal(expected, result);
    }

    // ── GetNativePrimitiveSize ──────────────────────────────────────────

    [Theory]
    [InlineData("System.Boolean", 4)]    // Win32 BOOL = 4 bytes
    [InlineData("System.Byte", 1)]
    [InlineData("System.SByte", 1)]
    [InlineData("System.Int16", 2)]
    [InlineData("System.UInt16", 2)]
    [InlineData("System.Char", 2)]
    [InlineData("System.Int32", 4)]
    [InlineData("System.UInt32", 4)]
    [InlineData("System.Single", 4)]
    [InlineData("System.Int64", 8)]
    [InlineData("System.UInt64", 8)]
    [InlineData("System.Double", 8)]
    [InlineData("System.IntPtr", 8)]
    [InlineData("System.UIntPtr", 8)]
    [InlineData("System.String", null)]
    [InlineData("CustomType", null)]
    public void GetNativePrimitiveSize_ReturnsExpected(string fieldType, int? expected)
    {
        var method = s_t.GetMethod("GetNativePrimitiveSize", s_flags, new[] { typeof(string) })!;
        var result = (int?)method.Invoke(null, new object[] { fieldType });
        Assert.Equal(expected, result);
    }

    // ── IsPInvokeStringType ────────────────────────────────────────────

    [Theory]
    [InlineData("System.String", true)]
    [InlineData("System.Char[]", false)]
    [InlineData("System.Int32", false)]
    [InlineData("CustomType", false)]
    public void IsPInvokeStringType_ReturnsExpected(string typeIdentity, bool expected)
    {
        var method = s_t.GetMethod("IsPInvokeStringType", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { typeIdentity })!;
        Assert.Equal(expected, result);
    }

    // ── ResolveExceptionRegionKind ─────────────────────────────────────

    [Theory]
    [InlineData("catch", AotCoreIrExceptionRegionKind.Catch)]
    [InlineData("filter", AotCoreIrExceptionRegionKind.Filter)]
    [InlineData("fault", AotCoreIrExceptionRegionKind.Fault)]
    [InlineData("finally", AotCoreIrExceptionRegionKind.Finally)]
    public void ResolveExceptionRegionKind_ReturnsExpected(string handlingKind, AotCoreIrExceptionRegionKind expected)
    {
        var method = s_t.GetMethod("ResolveExceptionRegionKind", s_flags, new[] { typeof(string) })!;
        var result = (AotCoreIrExceptionRegionKind)method.Invoke(null, new object[] { handlingKind })!;
        Assert.Equal(expected, result);
    }

    [Theory]
    [InlineData("CATCH")]
    [InlineData("unknown")]
    [InlineData("")]
    public void ResolveExceptionRegionKind_Unknown_Throws(string handlingKind)
    {
        var method = s_t.GetMethod("ResolveExceptionRegionKind", s_flags, new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { handlingKind }));
        Assert.Contains("unsupported managed exception region kind", ex.InnerException!.Message);
    }

    // ── MapOpStringToEnum ──────────────────────────────────────────────

    [Fact]
    public void MapOpStringToEnum_ReturnsKnownMappings()
    {
        var method = s_t.GetMethod("MapOpStringToEnum", s_flags, new[] { typeof(string) })!;
        foreach (var op in new[] { "add", "sub", "ret", "call", "ldc.i4", "br", "brtrue" })
        {
            var result = method.Invoke(null, new object[] { op });
            Assert.NotNull(result);
        }
    }

    [Fact]
    public void MapOpStringToEnum_Nop_ReturnsNull()
    {
        var method = s_t.GetMethod("MapOpStringToEnum", s_flags, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "nop" });
        Assert.Null(result);
    }

    [Fact]
    public void MapOpStringToEnum_UnknownOp_ReturnsNull()
    {
        var stderr = new StringWriter();
        Console.SetError(stderr);
        var method = s_t.GetMethod("MapOpStringToEnum", s_flags, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "unknown_op_xyz" });
        Assert.Null(result);
    }

    // ── SequenceEqual (IReadOnlyList<string>) ──────────────────────────

    [Fact]
    public void SequenceEqual_BothNull_ReturnsTrue()
    {
        var method = s_t.GetMethod("SequenceEqual", s_flags,
            new[] { typeof(IReadOnlyList<string>), typeof(IReadOnlyList<string>) })!;
        var result = (bool)method.Invoke(null, new object?[] { null, null })!;
        Assert.True(result);
    }

    [Fact]
    public void SequenceEqual_FirstNull_ReturnsFalse()
    {
        var method = s_t.GetMethod("SequenceEqual", s_flags,
            new[] { typeof(IReadOnlyList<string>), typeof(IReadOnlyList<string>) })!;
        var result = (bool)method.Invoke(null, new object?[] { null, new[] { "a" } })!;
        Assert.False(result);
    }

    [Fact]
    public void SequenceEqual_EqualLists_ReturnsTrue()
    {
        var method = s_t.GetMethod("SequenceEqual", s_flags,
            new[] { typeof(IReadOnlyList<string>), typeof(IReadOnlyList<string>) })!;
        var result = (bool)method.Invoke(null, new object[] { new[] { "a", "b" }, new[] { "a", "b" } })!;
        Assert.True(result);
    }

    [Fact]
    public void SequenceEqual_DifferentLists_ReturnsFalse()
    {
        var method = s_t.GetMethod("SequenceEqual", s_flags,
            new[] { typeof(IReadOnlyList<string>), typeof(IReadOnlyList<string>) })!;
        var result = (bool)method.Invoke(null, new object[] { new[] { "a", "b" }, new[] { "a", "c" } })!;
        Assert.False(result);
    }

    // ── DetermineLocalCount ────────────────────────────────────────────

    [Fact]
    public void DetermineLocalCount_NoMaxLocalSlot_ReturnsZero()
    {
        var method = s_t.GetMethod("DetermineLocalCount", s_flags,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var result = (int)method.Invoke(null, new object[] { Array.Empty<AotCoreIrInstructionArtifact>() })!;
        Assert.Equal(0, result);
    }

    [Fact]
    public void DetermineLocalCount_WithStloc_ReturnsSlotPlusOne()
    {
        var method = s_t.GetMethod("DetermineLocalCount", s_flags,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new AotCoreIrInstructionArtifact[]
        {
            new() { Op = "stloc", Operand = 0, IlOffset = 0 },
            new() { Op = "stloc", Operand = 3, IlOffset = 1 },
        };
        var result = (int)method.Invoke(null, new object[] { instructions })!;
        Assert.Equal(4, result);
    }

    // ── GetDeclaringTypeSubjectId ──────────────────────────────────────

    [Fact]
    public void GetDeclaringTypeSubjectId_ExtractsFromSubjectId()
    {
        var method = s_t.GetMethod("GetDeclaringTypeSubjectId", s_flags, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "TestModule.MyType::myField" })!;
        Assert.Equal("TestModule.MyType", result);
    }

    // ── GetRequiredTypedMethod (throws when not found) ─────────────────

    [Fact]
    public void GetRequiredTypedMethod_NotFound_Throws()
    {
        var method = s_t.GetMethod("GetRequiredTypedMethod", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, TypedIlMethodArtifact>), typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { new Dictionary<string, TypedIlMethodArtifact>(), "missing.method" }));
        Assert.Contains("missing.method", ex.InnerException!.Message);
    }

    // ── GetMemberDeclaringTypeSubjectId ────────────────────────────────

    [Fact]
    public void GetMemberDeclaringTypeSubjectId_ExtractsType()
    {
        var method = s_t.GetMethod("GetMemberDeclaringTypeSubjectId", s_flags, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { "Ns.Type::Member" })!;
        Assert.Equal("Ns.Type", result);
    }

    // ── RequiresValueTypeByValueCarrier ─────────────────────────────────
    // The method returns false for System.Private.CoreLib/System.* primitives
    // and true for all other types (the caller gates on IsValueType).

    [Fact]
    public void RequiresValueTypeByValueCarrier_SystemInt32_ReturnsFalse()
    {
        var model = new ManagedTypeModel
        {
            AssemblyName = "System.Private.CoreLib",
            NamespaceName = "System",
            Name = "Int32",
            SubjectId = "System.Private.CoreLib/System.Int32",
            DefinitionSubjectId = "System.Private.CoreLib/System.Int32",
            DisplayName = "System.Int32",
            MetadataToken = 1,
            IsValueType = true,
        };
        var method = s_t.GetMethod("RequiresValueTypeByValueCarrier", s_flags,
            new[] { typeof(ManagedTypeModel) })!;
        var result = (bool)method.Invoke(null, new object[] { model })!;
        Assert.False(result);
    }

    [Fact]
    public void RequiresValueTypeByValueCarrier_CustomValueType_ReturnsTrue()
    {
        var model = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "MyApp",
            Name = "MyStruct",
            SubjectId = "TestAssembly/MyApp.MyStruct",
            DefinitionSubjectId = "TestAssembly/MyApp.MyStruct",
            DisplayName = "MyApp.MyStruct",
            MetadataToken = 2,
            IsValueType = true,
        };
        var method = s_t.GetMethod("RequiresValueTypeByValueCarrier", s_flags,
            new[] { typeof(ManagedTypeModel) })!;
        var result = (bool)method.Invoke(null, new object[] { model })!;
        Assert.True(result);
    }

    [Fact]
    public void RequiresValueTypeByValueCarrier_CustomType_ReturnsTrue()
    {
        // The method itself doesn't gate on IsValueType — it checks SubjectId prefix only.
        // The caller (ResolveAbiSlot) enforces IsValueType before calling this method.
        var model = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "MyApp",
            Name = "MyStruct",
            SubjectId = "TestAssembly/MyApp.MyStruct",
            DefinitionSubjectId = "TestAssembly/MyApp.MyStruct",
            DisplayName = "MyApp.MyStruct",
            MetadataToken = 2,
            IsValueType = false,
        };
        var method = s_t.GetMethod("RequiresValueTypeByValueCarrier", s_flags,
            new[] { typeof(ManagedTypeModel) })!;
        var result = (bool)method.Invoke(null, new object[] { model })!;
        Assert.True(result);
    }

    // ── ResolveArrayElementSubjectId ────────────────────────────────────
    // Note: only single-dimensional arrays (ending with "[]") are resolved.
    // Multi-dimensional ("[,]") and jagged ("[][]") arrays return null.

    [Theory]
    [InlineData("System.Int32[]", "System.Int32")]
    [InlineData("System.String[]", "System.String")]
    [InlineData("System.Byte[,]", null)]
    [InlineData("System.Int32[][],", null)]
    [InlineData("NoBrackets", null)]
    public void ResolveArrayElementSubjectId_ReturnsExpected(string subjectId, string? expected)
    {
        var method = s_t.GetMethod("ResolveArrayElementSubjectId", s_flags,
            new[] { typeof(string) })!;
        var result = (string?)method.Invoke(null, new object[] { subjectId });
        Assert.Equal(expected, result);
    }

    // ── ResolveTypeShape ────────────────────────────────────────────────

    [Fact]
    public void ResolveTypeShape_NullModel_ReturnsReferenceType()
    {
        var method = s_t.GetMethod("ResolveTypeShape", s_flags,
            new[] { typeof(ManagedTypeModel), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null,
            new object?[] { null, "System.Object" })!;
        Assert.Equal(AotCoreIrTypeShapeKind.ReferenceType, result);
    }

    // ── IsBlittablePrimitiveType (additional coverage) ──────────────────

    [Theory]
    [InlineData("System.Void", false)]
    [InlineData("System.RuntimeTypeHandle", false)]
    [InlineData("", false)]
    [InlineData("System.Decimal", false)]
    public void IsBlittablePrimitiveType_NonBlittable_ReturnsFalse(string typeIdentity, bool expected)
    {
        var method = s_t.GetMethod("IsBlittablePrimitiveType", s_flags,
            new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { typeIdentity })!;
        Assert.Equal(expected, result);
    }

    // ── IsSafeHandleDerivedType ─────────────────────────────────────────

    [Fact]
    public void IsSafeHandleDerivedType_NotInTypes_ReturnsFalse()
    {
        var method = s_t.GetMethod("IsSafeHandleDerivedType", s_flags,
            new[] { typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "MyType", new Dictionary<string, ManagedTypeModel>() })!;
        Assert.False(result);
    }

    // ── GetMemberDeclaringTypeSubjectId (edge cases — throws) ───────────
    // These inputs lack the "::" separator so the method throws.

    [Theory]
    [InlineData("NoSeparator")]
    [InlineData("")]
    [InlineData("::JustMember")]
    public void GetMemberDeclaringTypeSubjectId_NoSeparator_Throws(string input)
    {
        var method = s_t.GetMethod("GetMemberDeclaringTypeSubjectId", s_flags,
            new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { input }));
        Assert.Contains("missing declaring type information", ex.InnerException!.Message);
    }

    // ── GetDeclaringTypeSubjectId (no separator throws) ─────────────────

    [Fact]
    public void GetDeclaringTypeSubjectId_NoSeparator_Throws()
    {
        var method = s_t.GetMethod("GetDeclaringTypeSubjectId", s_flags,
            new[] { typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { "NoSeparator" }));
        Assert.Contains("missing declaring type information", ex.InnerException!.Message);
    }

    // ── UnknownOpWarning (known ops that return null) ───────────────────

    [Theory]
    [InlineData("add")]
    [InlineData("sub")]
    [InlineData("call")]
    [InlineData("ret")]
    [InlineData("ldc.i4")]
    [InlineData("br")]
    public void UnknownOpWarning_KnownOps_ReturnsNull(string op)
    {
        var method = s_t.GetMethod("UnknownOpWarning", s_flags,
            new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { op });
        Assert.Null(result);
    }

    // ── Additional IsKnownValueTypeIdentity edge cases ──────────────────

    [Theory]
    [InlineData("System.RuntimeArgumentHandle")]
    [InlineData("System.RuntimeFieldHandle")]
    [InlineData("System.RuntimeMethodHandle")]
    [InlineData("System.RuntimeTypeHandle")]
    [InlineData("System.TypedReference")]
    public void IsKnownValueTypeIdentity_AdditionalHandles_ReturnsTrue(string identity)
    {
        var method = s_t.GetMethod("IsKnownValueTypeIdentity", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { identity })!;
        Assert.True(result);
    }

    [Theory]
    [InlineData("")]
    [InlineData(null)]
    [InlineData("System.Nullable`1")]
    public void IsKnownValueTypeIdentity_SpecialCases_ReturnsFalse(string? identity)
    {
        var method = s_t.GetMethod("IsKnownValueTypeIdentity", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { identity! })!;
        Assert.False(result);
    }

    // ── MapOpStringToEnum more edge cases ───────────────────────────────

    [Theory]
    [InlineData("ldc.r4")]
    [InlineData("ldc.r8")]
    [InlineData("ldnull")]
    [InlineData("ldarg")]
    [InlineData("ldloc")]
    [InlineData("stloc")]
    [InlineData("conv.i4")]
    [InlineData("conv.i8")]
    [InlineData("ceq")]
    [InlineData("cgt")]
    [InlineData("clt")]
    [InlineData("box")]
    [InlineData("unbox")]
    [InlineData("newarr")]
    [InlineData("newobj")]
    [InlineData("ldfld")]
    [InlineData("stfld")]
    [InlineData("ldsfld")]
    [InlineData("ldstr")]
    [InlineData("ldlen")]
    [InlineData("isinst")]
    [InlineData("castclass")]
    [InlineData("initobj")]
    [InlineData("ldobj")]
    [InlineData("stobj")]
    [InlineData("sizeof")]
    [InlineData("ldelem")]
    [InlineData("stelem")]
    [InlineData("ldelema")]
    [InlineData("throw")]
    [InlineData("endfilter")]
    [InlineData("localloc")]
    [InlineData("cpblk")]
    [InlineData("initblk")]
    public void MapOpStringToEnum_AdditionalKnownOps_ReturnsNonNull(string op)
    {
        var method = s_t.GetMethod("MapOpStringToEnum", s_flags, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { op });
        Assert.NotNull(result);
    }

    [Theory]
    [InlineData("cpobj")]
    [InlineData("jmp")]
    [InlineData("ckfinite")]
    [InlineData("arglist")]
    [InlineData("tailcall")]
    [InlineData("readonly")]
    [InlineData("constrained")]
    public void MapOpStringToEnum_UnmappedOps_ReturnsNull(string op)
    {
        var stderr = new StringWriter();
        Console.SetError(stderr);
        var method = s_t.GetMethod("MapOpStringToEnum", s_flags, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { op });
        Assert.Null(result);
    }

    // ── Additional IsBlittablePrimitiveType ──────────────────────────────

    [Theory]
    [InlineData("System.TypedReference")]
    [InlineData("System.ArgIterator")]
    [InlineData("System.RuntimeArgumentHandle")]
    public void IsBlittablePrimitiveType_SpecialTypes_ReturnsFalse(string typeIdentity)
    {
        var method = s_t.GetMethod("IsBlittablePrimitiveType", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { typeIdentity })!;
        Assert.False(result);
    }

    // ── IsPInvokeStringType edge cases ──────────────────────────────────

    [Theory]
    [InlineData(null)]
    [InlineData("")]
    public void IsPInvokeStringType_EdgeCases_ReturnsFalse(string? typeIdentity)
    {
        var method = s_t.GetMethod("IsPInvokeStringType", s_flags, new[] { typeof(string) })!;
        var result = (bool)method.Invoke(null, new object[] { typeIdentity! })!;
        Assert.False(result);
    }
}

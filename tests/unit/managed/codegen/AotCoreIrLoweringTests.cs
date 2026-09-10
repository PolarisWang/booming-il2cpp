using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

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
        // The original method was renamed to MapUnknownOp during a refactor;
        // this retargets the stale test to the current implementation.
        var method = s_t.GetMethod("MapUnknownOp", s_flags,
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

    // ── ResolveAbiSlot ──────────────────────────────────────────────────
    // Tests all type-identity branches in the ResolveAbiSlot method.
    // An empty managedTypes dict is sufficient because primitive types
    // are matched by string comparison *after* TryResolveManagedType fails.

    private static object InvokeResolveAbiSlot(string typeIdentity, IReadOnlyDictionary<string, ManagedTypeModel>? managedTypes = null)
    {
        // ResolveAbiSlot has an optional 4th parameter (fullTypeSubjectId, default null).
        // Reflection GetMethod with explicit Type[] requires the exact arity, so we must
        // pass all 4 parameter types here.
        var method = s_t.GetMethod("ResolveAbiSlot", s_flags, new[] { typeof(string), typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>), typeof(string) })!;
        return method.Invoke(null, new object[] { typeIdentity, "TestAssembly", managedTypes ?? new Dictionary<string, ManagedTypeModel>(), null })!;
    }

    private static AotCoreIrAbiCarrierKind GetCarrierKind(object artifact)
    {
        return (AotCoreIrAbiCarrierKind)((AotCoreIrAbiSlotArtifact)artifact).CarrierKindCode;
    }

    [Fact]
    public void ResolveAbiSlot_Void_ReturnsVoid()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Void, GetCarrierKind(InvokeResolveAbiSlot("System.Void")));
    }

    [Fact]
    public void ResolveAbiSlot_Int32_ReturnsInt32()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Int32, GetCarrierKind(InvokeResolveAbiSlot("System.Int32")));
    }

    [Fact]
    public void ResolveAbiSlot_SByte_ReturnsInt8()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Int8, GetCarrierKind(InvokeResolveAbiSlot("System.SByte")));
    }

    [Fact]
    public void ResolveAbiSlot_Byte_ReturnsUInt8()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.UInt8, GetCarrierKind(InvokeResolveAbiSlot("System.Byte")));
    }

    [Fact]
    public void ResolveAbiSlot_Int16_ReturnsInt16()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Int16, GetCarrierKind(InvokeResolveAbiSlot("System.Int16")));
    }

    [Fact]
    public void ResolveAbiSlot_UInt16_ReturnsUInt16()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.UInt16, GetCarrierKind(InvokeResolveAbiSlot("System.UInt16")));
    }

    [Fact]
    public void ResolveAbiSlot_Single_ReturnsFloat32()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Float32, GetCarrierKind(InvokeResolveAbiSlot("System.Single")));
    }

    [Fact]
    public void ResolveAbiSlot_Double_ReturnsFloat64()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Float64, GetCarrierKind(InvokeResolveAbiSlot("System.Double")));
    }

    [Fact]
    public void ResolveAbiSlot_Int64_ReturnsInt64()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.Int64, GetCarrierKind(InvokeResolveAbiSlot("System.Int64")));
    }

    [Fact]
    public void ResolveAbiSlot_UInt64_ReturnsUInt64()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.UInt64, GetCarrierKind(InvokeResolveAbiSlot("System.UInt64")));
    }

    [Fact]
    public void ResolveAbiSlot_String_ReturnsNativeInt()
    {
        var result = InvokeResolveAbiSlot("System.String");
        Assert.Equal(AotCoreIrAbiCarrierKind.NativeInt, GetCarrierKind(result));
        var slot = (AotCoreIrAbiSlotArtifact)result;
        Assert.Equal("System.Private.CoreLib/System.String", slot.TypeSubjectId);
        Assert.Equal(AotCoreIrTypeShapeKind.ReferenceType, slot.TypeShape);
    }

    [Fact]
    public void ResolveAbiSlot_UnknownType_ReturnsNativeInt()
    {
        Assert.Equal(AotCoreIrAbiCarrierKind.NativeInt, GetCarrierKind(InvokeResolveAbiSlot("System.Threading.Thread")));
    }

    [Fact]
    public void ResolveAbiSlot_CustomValueType_ReturnsValueTypeByValue()
    {
        var managedTypes = new Dictionary<string, ManagedTypeModel>
        {
            ["TestAssembly/MyStruct"] = new ManagedTypeModel
            {
                AssemblyName = "TestAssembly",
                SubjectId = "TestAssembly/MyStruct",
                DefinitionSubjectId = "TestAssembly/MyStruct",
                DisplayName = "MyStruct",
                Name = "MyStruct",
                MetadataToken = 1,
                IsValueType = true,
                NamespaceName = null!,
            },
        };
        var result = InvokeResolveAbiSlot("TestAssembly/MyStruct", managedTypes);
        Assert.Equal(AotCoreIrAbiCarrierKind.ValueTypeByValue, GetCarrierKind(result));
    }

    // ── ResolveRuntimeServiceKind ─────────────────────────────────────────

    private static AotCoreIrRuntimeServiceKind? InvokeResolveRuntimeServiceKind(string op)
    {
        var method = s_t.GetMethod("ResolveRuntimeServiceKind", s_flags, new[] { typeof(TypedIlInstructionArtifact) })!;
        var instruction = new TypedIlInstructionArtifact { Op = op };
        return (AotCoreIrRuntimeServiceKind?)method.Invoke(null, new object[] { instruction });
    }

    [Theory]
    [InlineData("newobj", AotCoreIrRuntimeServiceKind.NewObject)]
    [InlineData("ldfld", AotCoreIrRuntimeServiceKind.LoadInstanceField)]
    [InlineData("stfld", AotCoreIrRuntimeServiceKind.StoreInstanceField)]
    [InlineData("ldsfld", AotCoreIrRuntimeServiceKind.LoadStaticField)]
    [InlineData("stsfld", AotCoreIrRuntimeServiceKind.StoreStaticField)]
    [InlineData("newarr", AotCoreIrRuntimeServiceKind.NewArray)]
    [InlineData("castclass", AotCoreIrRuntimeServiceKind.CastClass)]
    [InlineData("isinst", AotCoreIrRuntimeServiceKind.IsInst)]
    [InlineData("box", AotCoreIrRuntimeServiceKind.Box)]
    [InlineData("unbox", AotCoreIrRuntimeServiceKind.Unbox)]
    [InlineData("unbox.any", AotCoreIrRuntimeServiceKind.UnboxAny)]
    [InlineData("initobj", AotCoreIrRuntimeServiceKind.InitObject)]
    [InlineData("ldelem", AotCoreIrRuntimeServiceKind.LoadArrayElement)]
    [InlineData("ldelem.ref", AotCoreIrRuntimeServiceKind.LoadArrayElement)]
    [InlineData("stelem", AotCoreIrRuntimeServiceKind.StoreArrayElement)]
    [InlineData("stelem.ref", AotCoreIrRuntimeServiceKind.StoreArrayElement)]
    public void ResolveRuntimeServiceKind_ReturnsExpected(string op, AotCoreIrRuntimeServiceKind expected)
    {
        Assert.Equal(expected, InvokeResolveRuntimeServiceKind(op));
    }

    [Theory]
    [InlineData("add")]
    [InlineData("ret")]
    [InlineData("call")]
    [InlineData("nop")]
    public void ResolveRuntimeServiceKind_UnknownOp_ReturnsNull(string op)
    {
        Assert.Null(InvokeResolveRuntimeServiceKind(op));
    }

    // ── UnknownOpWarning ──────────────────────────────────────────────────

    [Fact]
    public void UnknownOpWarning_WritesWarningAndReturnsNull()
    {
        var stderr = new StringWriter();
        Console.SetError(stderr);
        // Original method renamed to MapUnknownOp; retarget stale test.
        var method = s_t.GetMethod("MapUnknownOp", s_flags, new[] { typeof(string) })!;
        var result = method.Invoke(null, new object[] { "bogus_op" });
        Assert.Null(result);
        Assert.Contains("bogus_op", stderr.ToString());
    }

    // ── GetRequiredNativeSymbol ───────────────────────────────────────────

    [Fact]
    public void GetRequiredNativeSymbol_Found_ReturnsSymbol()
    {
        var method = s_t.GetMethod("GetRequiredNativeSymbol", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, string>), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] {
            new Dictionary<string, string> { ["m1"] = "Sym_M1" }, "m1" })!;
        Assert.Equal("Sym_M1", result);
    }

    [Fact]
    public void GetRequiredNativeSymbol_NotFound_ReturnsFallback()
    {
        var method = s_t.GetMethod("GetRequiredNativeSymbol", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, string>), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { new Dictionary<string, string>(), "TestNs/TestType::TestMethod" })!;
        Assert.Contains("chaos_external_", result);
    }

    [Fact]
    public void GetRequiredNativeSymbol_EmptySymbol_ReturnsFallback()
    {
        var method = s_t.GetMethod("GetRequiredNativeSymbol", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, string>), typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { new Dictionary<string, string> { ["m1"] = "" }, "m1" })!;
        Assert.Contains("chaos_external_", result);
    }

    // ── ResolveDirectCallTarget ───────────────────────────────────────────

    [Fact]
    public void ResolveDirectCallTarget_NonCallOp_ReturnsNulls()
    {
        var method = s_t.GetMethod("ResolveDirectCallTarget", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(IReadOnlyDictionary<string, string>) })!;
        var instruction = new TypedIlInstructionArtifact { Op = "ldc.i4" };
        var result = method.Invoke(null, new object[] { instruction, new Dictionary<string, ManagedMethodModel>(), new Dictionary<string, string>() });
        Assert.Null(result.GetType().GetProperty("TargetSymbol")?.GetValue(result));
    }

    [Fact]
    public void ResolveDirectCallTarget_CallWithNoCallee_ReturnsNulls()
    {
        var method = s_t.GetMethod("ResolveDirectCallTarget", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(IReadOnlyDictionary<string, string>) })!;
        var instruction = new TypedIlInstructionArtifact { Op = "call", Callee = "" };
        var result = method.Invoke(null, new object[] { instruction, new Dictionary<string, ManagedMethodModel>(), new Dictionary<string, string>() });
        Assert.Null(result.GetType().GetProperty("TargetSymbol")?.GetValue(result));
    }

    [Fact]
    public void ResolveDirectCallTarget_CalleeNotInMethods_ReturnsNulls()
    {
        var method = s_t.GetMethod("ResolveDirectCallTarget", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(IReadOnlyDictionary<string, string>) })!;
        var instruction = new TypedIlInstructionArtifact { Op = "call", Callee = "NonExistent/M::M()" };
        var result = method.Invoke(null, new object[] { instruction, new Dictionary<string, ManagedMethodModel>(), new Dictionary<string, string>() });
        Assert.Null(result.GetType().GetProperty("TargetSymbol")?.GetValue(result));
    }

    // ── TryResolveManagedType ─────────────────────────────────────────────

    [Fact]
    public void TryResolveManagedType_ExactMatch_ReturnsType()
    {
        var method = s_t.GetMethod("TryResolveManagedType", s_flags,
            new[] { typeof(string), typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>) })!;
        var type = new ManagedTypeModel
        {
            AssemblyName = "Test", SubjectId = "Test/MyType", DefinitionSubjectId = "Test/MyType",
            DisplayName = "MyType", Name = "MyType", MetadataToken = 1, NamespaceName = null!,
        };
        var types = new Dictionary<string, ManagedTypeModel> { ["Test/MyType"] = type };
        var result = method.Invoke(null, new object[] { "Test/MyType", "Test", types });
        Assert.Same(type, result);
    }

    [Fact]
    public void TryResolveManagedType_DisplayNameFallback_ReturnsType()
    {
        var method = s_t.GetMethod("TryResolveManagedType", s_flags,
            new[] { typeof(string), typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>) })!;
        var type = new ManagedTypeModel
        {
            AssemblyName = "Test", SubjectId = "T/MyType", DefinitionSubjectId = "T/MyType",
            DisplayName = "MyType", Name = "MyType", MetadataToken = 1, NamespaceName = null!,
        };
        var types = new Dictionary<string, ManagedTypeModel> { ["T/MyType"] = type };
        var result = method.Invoke(null, new object[] { "MyType", "Test", types });
        Assert.Same(type, result);
    }

    [Fact]
    public void TryResolveManagedType_NotFound_ReturnsNull()
    {
        var method = s_t.GetMethod("TryResolveManagedType", s_flags,
            new[] { typeof(string), typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>) })!;
        var result = method.Invoke(null, new object[] { "UnknownType", "Test", new Dictionary<string, ManagedTypeModel>() });
        Assert.Null(result);
    }

    // ── DetermineLocalCount edge cases ────────────────────────────────────

    [Fact]
    public void DetermineLocalCount_NonLocalInstructions_ReturnsZero()
    {
        var method = s_t.GetMethod("DetermineLocalCount", s_flags,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new AotCoreIrInstructionArtifact[]
        {
            new() { Op = "add", Operand = 0, IlOffset = 0 },
            new() { Op = "ret", IlOffset = 1 },
        };
        var result = (int)method.Invoke(null, new object[] { instructions })!;
        Assert.Equal(0, result);
    }

    [Fact]
    public void DetermineLocalCount_Ldloca_UsesSlot()
    {
        var method = s_t.GetMethod("DetermineLocalCount", s_flags,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new AotCoreIrInstructionArtifact[]
        {
            new() { Op = "ldloca", Operand = 2, IlOffset = 0 },
        };
        var result = (int)method.Invoke(null, new object[] { instructions })!;
        Assert.Equal(3, result);
    }

    [Fact]
    public void DetermineLocalCount_NonIntOperand_Throws()
    {
        var method = s_t.GetMethod("DetermineLocalCount", s_flags,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>) })!;
        var instructions = new AotCoreIrInstructionArtifact[]
        {
            new() { Op = "stloc", Operand = "not_an_int", IlOffset = 0 },
        };
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { instructions }));
        Assert.Contains("Int32 operand", ex.InnerException!.Message);
    }

    // ── ResolveTypeShape (ManagedTypeModel? + optional string) ─────────

    [Fact]
    public void ResolveTypeShapeSingle_NullModel_ReturnsReferenceType()
    {
        var method = s_t.GetMethod("ResolveTypeShape", s_flags,
            new[] { typeof(ManagedTypeModel), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null, new object?[] { null, null })!;
        Assert.Equal(AotCoreIrTypeShapeKind.ReferenceType, result);
    }

    [Fact]
    public void ResolveTypeShapeSingle_ValueTypeModel_ReturnsValueType()
    {
        var model = new ManagedTypeModel
        {
            AssemblyName = "Test", SubjectId = "Test/S", DefinitionSubjectId = "Test/S",
            DisplayName = "S", Name = "S", MetadataToken = 1, IsValueType = true, NamespaceName = null!,
        };
        var method = s_t.GetMethod("ResolveTypeShape", s_flags,
            new[] { typeof(ManagedTypeModel), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null, new object[] { model, null! })!;
        Assert.Equal(AotCoreIrTypeShapeKind.ValueType, result);
    }

    [Fact]
    public void ResolveTypeShapeSingle_ReferenceTypeModel_ReturnsReferenceType()
    {
        var model = new ManagedTypeModel
        {
            AssemblyName = "Test", SubjectId = "Test/C", DefinitionSubjectId = "Test/C",
            DisplayName = "C", Name = "C", MetadataToken = 1, IsValueType = false, NamespaceName = null!,
        };
        var method = s_t.GetMethod("ResolveTypeShape", s_flags,
            new[] { typeof(ManagedTypeModel), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null, new object[] { model, null! })!;
        Assert.Equal(AotCoreIrTypeShapeKind.ReferenceType, result);
    }

    [Fact]
    public void ResolveTypeShapeSingle_InterfaceModel_ReturnsInterfaceType()
    {
        var model = new ManagedTypeModel
        {
            AssemblyName = "Test", SubjectId = "Test/I", DefinitionSubjectId = "Test/I",
            DisplayName = "I", Name = "I", MetadataToken = 1, IsValueType = false,
            IsInterface = true, NamespaceName = null!,
        };
        var method = s_t.GetMethod("ResolveTypeShape", s_flags,
            new[] { typeof(ManagedTypeModel), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null, new object[] { model, null! })!;
        Assert.Equal(AotCoreIrTypeShapeKind.InterfaceType, result);
    }

    // ── ResolveArrayElementTypeShape ──────────────────────────────────────

    [Fact]
    public void ResolveArrayElementTypeShape_NullElement_ReturnsDefault()
    {
        var method = s_t.GetMethod("ResolveArrayElementTypeShape", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, ManagedTypeModel>), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null,
            new object[] { new Dictionary<string, ManagedTypeModel>(), null! })!;
        Assert.Equal(default(AotCoreIrTypeShapeKind), result);
    }

    [Fact]
    public void ResolveArrayElementTypeShape_ValueTypeElement_ReturnsValueType()
    {
        var types = new Dictionary<string, ManagedTypeModel>
        {
            ["Test/MyStruct"] = new()
            {
                AssemblyName = "Test", SubjectId = "Test/MyStruct", DefinitionSubjectId = "Test/MyStruct",
                DisplayName = "MyStruct", Name = "MyStruct", MetadataToken = 1,
                IsValueType = true, NamespaceName = null!,
            },
        };
        var method = s_t.GetMethod("ResolveArrayElementTypeShape", s_flags,
            new[] { typeof(IReadOnlyDictionary<string, ManagedTypeModel>), typeof(string) })!;
        var result = (AotCoreIrTypeShapeKind)method.Invoke(null, new object[] { types, "Test/MyStruct" })!;
        Assert.Equal(AotCoreIrTypeShapeKind.ValueType, result);
    }

    // ── IsBlittableStructType ─────────────────────────────────────────────

    [Fact]
    public void IsBlittableStructType_EmptyTypes_ReturnsFalse()
    {
        var method = s_t.GetMethod("IsBlittableStructType", s_flags,
            new[] { typeof(string), typeof(IReadOnlyDictionary<string, ManagedTypeModel>), typeof(IReadOnlyDictionary<string, ManagedFieldModel>) })!;
        var result = (bool)method.Invoke(null,
            new object[] { "Test/MyType", new Dictionary<string, ManagedTypeModel>(), new Dictionary<string, ManagedFieldModel>() })!;
        Assert.False(result);
    }

    // ── GetMemberDeclaringTypeSubjectId additional edge cases ─────────────

    [Theory]
    [InlineData("A.B::Member", "A.B")]
    [InlineData("X/Y.Z::Method()", "X/Y.Z")]
    public void GetMemberDeclaringTypeSubjectId_ValidInputs(string input, string expected)
    {
        var method = s_t.GetMethod("GetMemberDeclaringTypeSubjectId", s_flags,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── GetDeclaringTypeSubjectId additional edge cases ───────────────────

    [Theory]
    [InlineData("A.B::field", "A.B")]
    [InlineData("X/Y.Z::Method()", "X/Y.Z")]
    public void GetDeclaringTypeSubjectId_ValidInputs(string input, string expected)
    {
        var method = s_t.GetMethod("GetDeclaringTypeSubjectId", s_flags,
            new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── BuildGenericDemandLookup ──────────────────────────────────────────

    [Fact]
    public void BuildGenericDemandLookup_NullGraph_ReturnsEmpty()
    {
        var method = s_t.GetMethod("BuildGenericDemandLookup", s_flags,
            new[] { typeof(GenericInstantiationDemandGraphModel) })!;
        var result = method.Invoke(null, new object?[] { null })!;
        Assert.NotNull(result);
        var dict = (IReadOnlyDictionary<string, GenericInstantiationDemandModel>)result;
        Assert.Empty(dict);
    }

    [Fact]
    public void BuildGenericDemandLookup_EmptyDemands_ReturnsEmpty()
    {
        var method = s_t.GetMethod("BuildGenericDemandLookup", s_flags,
            new[] { typeof(GenericInstantiationDemandGraphModel) })!;
        var graph = new GenericInstantiationDemandGraphModel { Demands = [] };
        var result = method.Invoke(null, new object[] { graph })!;
        var dict = (IReadOnlyDictionary<string, GenericInstantiationDemandModel>)result;
        Assert.Empty(dict);
    }

    // ── AreEquivalentInstantiationKeys ────────────────────────────────────

    [Fact]
    public void AreEquivalentInstantiationKeys_SameKeys_ReturnsTrue()
    {
        var method = s_t.GetMethod("AreEquivalentInstantiationKeys", s_flags,
            new[] { typeof(GenericInstantiationKey), typeof(GenericInstantiationKey) })!;
        var key1 = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = "Test/Type",
        };
        var key2 = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = "Test/Type",
        };
        Assert.True((bool)method.Invoke(null, new object[] { key1, key2 })!);
    }

    [Fact]
    public void AreEquivalentInstantiationKeys_DifferentContextKind_ReturnsFalse()
    {
        var method = s_t.GetMethod("AreEquivalentInstantiationKeys", s_flags,
            new[] { typeof(GenericInstantiationKey), typeof(GenericInstantiationKey) })!;
        var key1 = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "T" };
        var key2 = new GenericInstantiationKey { ContextKind = GenericContextKind.MethodInstantiation, DefinitionSubjectId = "T" };
        Assert.False((bool)method.Invoke(null, new object[] { key1, key2 })!);
    }

    [Fact]
    public void AreEquivalentInstantiationKeys_DifferentDefinitionId_ReturnsFalse()
    {
        var method = s_t.GetMethod("AreEquivalentInstantiationKeys", s_flags,
            new[] { typeof(GenericInstantiationKey), typeof(GenericInstantiationKey) })!;
        var key1 = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "A" };
        var key2 = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "B" };
        Assert.False((bool)method.Invoke(null, new object[] { key1, key2 })!);
    }

    // ── EnsureEquivalentDemand ────────────────────────────────────────────

    private static GenericInstantiationDemandModel MakeDemand(string def, GenericSupportKind support, GenericSpecializationKind spec, GenericDemandFamilyKind family) =>
        new()
        {
            RequestingAssemblyName = "T", OwningAssemblyName = "T", SubjectKind = "type",
            SubjectId = "T/X", DefinitionSubjectId = def, DemandSourceKind = "loader",
            InstantiationKey = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = def },
            SupportKindCode = support, SpecializationKindCode = spec, FamilyKindCode = family,
        };

    [Fact]
    public void EnsureEquivalentDemand_EqualDemands_ReturnsNormally()
    {
        var method = s_t.GetMethod("EnsureEquivalentDemand", s_flags,
            new[] { typeof(GenericInstantiationDemandModel), typeof(GenericInstantiationDemandModel) })!;
        var demand = MakeDemand("T/X", GenericSupportKind.Legal, GenericSpecializationKind.SharedBody, GenericDemandFamilyKind.ClosedGenericType);
        var ex = Record.Exception(() => method.Invoke(null, new object[] { demand, demand }));
        Assert.Null(ex);
    }

    [Fact]
    public void EnsureEquivalentDemand_DifferentDefId_Throws()
    {
        var method = s_t.GetMethod("EnsureEquivalentDemand", s_flags,
            new[] { typeof(GenericInstantiationDemandModel), typeof(GenericInstantiationDemandModel) })!;
        var d1 = MakeDemand("T/A", GenericSupportKind.Legal, GenericSpecializationKind.SharedBody, GenericDemandFamilyKind.ClosedGenericType);
        var d2 = MakeDemand("T/B", GenericSupportKind.Legal, GenericSpecializationKind.SharedBody, GenericDemandFamilyKind.ClosedGenericType);
        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object[] { d1, d2 }));
        Assert.Contains("conflicting generic instantiation demand", ex.InnerException!.Message);
    }

    // ── ResolveConstrainedValueTypeOverride ───────────────────────────────

    [Fact]
    public void ResolveConstrainedValueTypeOverride_NoConstrainedType_ReturnsNull()
    {
        var method = s_t.GetMethod("ResolveConstrainedValueTypeOverride", s_flags,
            new[] { typeof(TypedIlInstructionArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt", Callee = "System.Object::GetHashCode" };
        Assert.Null(method.Invoke(null, new object[] { instr }));
    }

    [Fact]
    public void ResolveConstrainedValueTypeOverride_NoCallee_ReturnsNull()
    {
        var method = s_t.GetMethod("ResolveConstrainedValueTypeOverride", s_flags,
            new[] { typeof(TypedIlInstructionArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt", ConstrainedTypeSubjectId = "System.Int32" };
        Assert.Null(method.Invoke(null, new object[] { instr }));
    }

    [Fact]
    public void ResolveConstrainedValueTypeOverride_Valid_ReturnsOverride()
    {
        var method = s_t.GetMethod("ResolveConstrainedValueTypeOverride", s_flags,
            new[] { typeof(TypedIlInstructionArtifact) })!;
        var instr = new TypedIlInstructionArtifact
        {
            Op = "callvirt",
            Callee = "System.Object::GetHashCode",
            ConstrainedTypeSubjectId = "System.Int32",
        };
        var result = (string?)method.Invoke(null, new object[] { instr });
        Assert.Equal("System.Int32::GetHashCode", result);
    }

    [Fact]
    public void ResolveConstrainedValueTypeOverride_CalleeWithoutDoubleColon_ReturnsNull()
    {
        var method = s_t.GetMethod("ResolveConstrainedValueTypeOverride", s_flags,
            new[] { typeof(TypedIlInstructionArtifact) })!;
        var instr = new TypedIlInstructionArtifact
        {
            Op = "callvirt",
            Callee = "NoSeparator",
            ConstrainedTypeSubjectId = "System.Int32",
        };
        Assert.Null(method.Invoke(null, new object[] { instr }));
    }

    // ── ComputeComVtableSlot ──────────────────────────────────────────────

    private static AotCoreIrReferenceArtifact MakeMethodRef(string declaringType) =>
        new()
        {
            Kind = AotCoreIrReferenceKind.Method,
            AssemblyName = "T",
            SubjectId = "IfaceType::Method",
            DeclaringTypeSubjectId = declaringType,
        };

    private static ManagedMethodModel CreateMethod(string subjectId, string declaringType, string name, int token)
    {
        return new ManagedMethodModel
        {
            SubjectId = subjectId,
            DeclaringTypeSubjectId = declaringType,
            DeclaringTypeDisplayName = declaringType,
            Name = name,
            MetadataToken = token,
            AssemblyName = "T",
            DefinitionSubjectId = subjectId,
            Signature = "void()",
            ReturnType = "System.Void",
            IsStatic = false,
            IsVirtual = true,
            Parameters = [],
            Body = new ManagedMethodBodyModel
            {
                Blocks = [],
                ExceptionRegions = [],
            },
        };
    }

    [Fact]
    public void ComputeComVtableSlot_NoDeclaringType_ReturnsNull()
    {
        var method = s_t.GetMethod("ComputeComVtableSlot", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(AotCoreIrReferenceArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt" };
        Assert.Null(method.Invoke(null, new object[] { instr, new Dictionary<string, ManagedMethodModel>(), MakeMethodRef("") }));
    }

    [Fact]
    public void ComputeComVtableSlot_NoCallee_ReturnsNull()
    {
        var method = s_t.GetMethod("ComputeComVtableSlot", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(AotCoreIrReferenceArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt" };
        Assert.Null(method.Invoke(null, new object[] { instr, new Dictionary<string, ManagedMethodModel>(), MakeMethodRef("I") }));
    }

    [Fact]
    public void ComputeComVtableSlot_NoInterfaceMethods_ReturnsNull()
    {
        var method = s_t.GetMethod("ComputeComVtableSlot", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(AotCoreIrReferenceArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt", Callee = "IfaceType::Method" };
        Assert.Null(method.Invoke(null, new object[] { instr, new Dictionary<string, ManagedMethodModel>(), MakeMethodRef("IfaceType") }));
    }

    [Fact]
    public void ComputeComVtableSlot_FirstMethod_ReturnsSlot3()
    {
        var method = s_t.GetMethod("ComputeComVtableSlot", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(AotCoreIrReferenceArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt", Callee = "IfaceType::MethodA" };
        var methods = new Dictionary<string, ManagedMethodModel>
        {
            ["IfaceType::MethodA"] = CreateMethod("IfaceType::MethodA", "IfaceType", "MethodA", 1),
            ["IfaceType::MethodB"] = CreateMethod("IfaceType::MethodB", "IfaceType", "MethodB", 2),
        };
        var result = (int?)method.Invoke(null, new object[] { instr, methods, MakeMethodRef("IfaceType") });
        Assert.Equal(3, result); // 3 IUnknown reserved + 0
    }

    [Fact]
    public void ComputeComVtableSlot_SecondMethod_ReturnsSlot4()
    {
        var method = s_t.GetMethod("ComputeComVtableSlot", s_flags,
            new[] { typeof(TypedIlInstructionArtifact), typeof(IReadOnlyDictionary<string, ManagedMethodModel>), typeof(AotCoreIrReferenceArtifact) })!;
        var instr = new TypedIlInstructionArtifact { Op = "callvirt", Callee = "IfaceType::MethodB" };
        var methods = new Dictionary<string, ManagedMethodModel>
        {
            ["IfaceType::MethodA"] = CreateMethod("IfaceType::MethodA", "IfaceType", "MethodA", 1),
            ["IfaceType::MethodB"] = CreateMethod("IfaceType::MethodB", "IfaceType", "MethodB", 2),
        };
        var result = (int?)method.Invoke(null, new object[] { instr, methods, MakeMethodRef("IfaceType") });
        Assert.Equal(4, result);
    }
}

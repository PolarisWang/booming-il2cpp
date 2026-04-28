using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal enum RuntimeSkeletonKernelBackendKind
{
    Generic,
    Platform,
    Intrinsic,
}

internal enum RuntimeSkeletonKernelOperationKind
{
    CharacterClassification,
    FloatingClassification,
    BitArithmetic,
    TickArithmetic,
    WideDecimalArithmetic,
}

internal sealed record RuntimeSkeletonKernelSemanticDescriptor(
    string SemanticId,
    string SubjectIdPrefix,
    RuntimeSkeletonKernelOperationKind OperationKind,
    RuntimeSkeletonKernelBackendKind PreferredBackend,
    string ManagedTypeName,
    string ScalarCppType,
    string HelperPrefix);

internal sealed record RuntimeSkeletonKernelCarrierSchema(
    string? ThisCppType,
    IReadOnlyList<string> ArgumentCppTypes,
    string ReturnCppType)
{
    public string ThisFieldDeclaration =>
        string.IsNullOrWhiteSpace(ThisCppType) ? string.Empty : $"{ThisCppType} this_arg;";

    public string ArgFieldDeclarations =>
        string.Join(
            "\n    ",
            ArgumentCppTypes.Select((cppType, index) => $"{cppType} arg{index};"));

    public string ReturnFieldDeclaration => $"{ReturnCppType}* return_value;";
}

internal sealed record RuntimeSkeletonKernelEmissionPlan(
    string ContractId,
    string SemanticId,
    RuntimeSkeletonKernelBackendKind BackendKind,
    RuntimeSkeletonKernelCarrierSchema CarrierSchema,
    string HelperCallExpression);

internal static class RuntimeSkeletonKernelCore
{
    public const string ContractId = "valuetype-kernel-v1";

    private static readonly RuntimeSkeletonKernelSemanticDescriptor[] Descriptors =
    [
        new(
            "char-classification",
            "/System.Char::",
            RuntimeSkeletonKernelOperationKind.CharacterClassification,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Char",
            "CHAOS_IL2CPP_UINT16",
            "Char"),
        new(
            "floating-classification",
            "/System.Half::",
            RuntimeSkeletonKernelOperationKind.FloatingClassification,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Half",
            "CHAOS_IL2CPP_UINT16",
            "Half"),
        new(
            "floating-classification",
            "/System.Single::",
            RuntimeSkeletonKernelOperationKind.FloatingClassification,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Single",
            "float",
            "Single"),
        new(
            "floating-classification",
            "/System.Double::",
            RuntimeSkeletonKernelOperationKind.FloatingClassification,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Double",
            "double",
            "Double"),
        new(
            "floating-classification",
            "/System.Runtime.InteropServices.NFloat::",
            RuntimeSkeletonKernelOperationKind.FloatingClassification,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Runtime.InteropServices.NFloat",
            "double",
            "NFloat"),
        new(
            "bit-arithmetic",
            "/System.Int128::",
            RuntimeSkeletonKernelOperationKind.BitArithmetic,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.Int128",
            "RuntimeSkeletonKernelWide128Carrier",
            "Int128"),
        new(
            "bit-arithmetic",
            "/System.UInt128::",
            RuntimeSkeletonKernelOperationKind.BitArithmetic,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.UInt128",
            "RuntimeSkeletonKernelWide128Carrier",
            "UInt128"),
        new(
            "bit-arithmetic",
            "/System.IntPtr::",
            RuntimeSkeletonKernelOperationKind.BitArithmetic,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.IntPtr",
            "CHAOS_IL2CPP_INTPTR",
            "IntPtr"),
        new(
            "bit-arithmetic",
            "/System.UIntPtr::",
            RuntimeSkeletonKernelOperationKind.BitArithmetic,
            RuntimeSkeletonKernelBackendKind.Intrinsic,
            "System.UIntPtr",
            "CHAOS_IL2CPP_UINTPTR",
            "UIntPtr"),
        new(
            "tick-arithmetic",
            "/System.DateTime::",
            RuntimeSkeletonKernelOperationKind.TickArithmetic,
            RuntimeSkeletonKernelBackendKind.Generic,
            "System.DateTime",
            "CHAOS_IL2CPP_UINT64",
            "DateTime"),
        new(
            "tick-arithmetic",
            "/System.TimeSpan::",
            RuntimeSkeletonKernelOperationKind.TickArithmetic,
            RuntimeSkeletonKernelBackendKind.Generic,
            "System.TimeSpan",
            "CHAOS_IL2CPP_INT64",
            "TimeSpan"),
        new(
            "tick-arithmetic",
            "/System.DateOnly::",
            RuntimeSkeletonKernelOperationKind.TickArithmetic,
            RuntimeSkeletonKernelBackendKind.Generic,
            "System.DateOnly",
            "CHAOS_IL2CPP_INT32",
            "DateOnly"),
        new(
            "tick-arithmetic",
            "/System.TimeOnly::",
            RuntimeSkeletonKernelOperationKind.TickArithmetic,
            RuntimeSkeletonKernelBackendKind.Generic,
            "System.TimeOnly",
            "CHAOS_IL2CPP_INT64",
            "TimeOnly"),
        new(
            "wide-decimal-arithmetic",
            "/System.Decimal+DecCalc::",
            RuntimeSkeletonKernelOperationKind.WideDecimalArithmetic,
            RuntimeSkeletonKernelBackendKind.Generic,
            "System.Decimal+DecCalc",
            string.Empty,
            "Decimal"),
    ];

    public static RuntimeSkeletonKernelSemanticDescriptor? TryResolveBySubjectId(string subjectId)
    {
        return Descriptors.FirstOrDefault(
            descriptor => subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal));
    }

    public static RuntimeSkeletonKernelBackendKind SelectBackend(
        TypedIlMethodArtifact method,
        RuntimeSkeletonKernelSemanticDescriptor descriptor)
    {
        _ = method;
        return descriptor.PreferredBackend;
    }

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;

        var descriptor = TryResolveBySubjectId(method.SubjectId);
        if (descriptor is null)
        {
            return false;
        }

        var backend = SelectBackend(method, descriptor);
        return TryCreatePlan(descriptor, method.SubjectId, backend, out plan);
    }

    private static bool TryCreatePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        if (!subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal))
        {
            return false;
        }

        var prefixIndex = subjectId.IndexOf(descriptor.SubjectIdPrefix, StringComparison.Ordinal);
        var suffix = subjectId[(prefixIndex + descriptor.SubjectIdPrefix.Length)..];
        return descriptor.SubjectIdPrefix switch
        {
            "/System.Char::" => TryCreateCharPlan(descriptor, suffix, backend, out plan),
            "/System.Half::" => TryCreateFloatingPlan(descriptor, suffix, backend, out plan),
            "/System.Single::" => TryCreateFloatingPlan(descriptor, suffix, backend, out plan),
            "/System.Double::" => TryCreateFloatingPlan(descriptor, suffix, backend, out plan),
            "/System.Runtime.InteropServices.NFloat::" => TryCreateFloatingPlan(descriptor, suffix, backend, out plan),
            "/System.Int128::" => TryCreateWideIntegerPlan(descriptor, suffix, backend, out plan),
            "/System.UInt128::" => TryCreateWideIntegerPlan(descriptor, suffix, backend, out plan),
            "/System.IntPtr::" => TryCreatePointerIntegerPlan(descriptor, suffix, backend, out plan),
            "/System.UIntPtr::" => TryCreatePointerIntegerPlan(descriptor, suffix, backend, out plan),
            "/System.DateTime::" => TryCreateDateTimeLikePlan(descriptor, suffix, backend, out plan),
            "/System.TimeSpan::" => TryCreateTimeSpanLikePlan(descriptor, suffix, backend, out plan),
            "/System.DateOnly::" => TryCreateDateOnlyLikePlan(descriptor, suffix, backend, out plan),
            "/System.TimeOnly::" => TryCreateTimeOnlyLikePlan(descriptor, suffix, backend, out plan),
            _ => false,
        };
    }

    private static bool TryCreateCharPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "CompareTo:System.Int32(System.Char)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::CharCompare(request->this_arg, request->arg0)",
                out plan),
            "Equals:System.Boolean(System.Char)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharEquals(request->this_arg, request->arg0)",
                out plan),
            "IsAscii:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAscii(request->arg0)",
                out plan),
            "IsAsciiDigit:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiDigit(request->arg0)",
                out plan),
            "IsAsciiHexDigit:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiHexDigit(request->arg0)",
                out plan),
            "IsAsciiHexDigitLower:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiHexDigitLower(request->arg0)",
                out plan),
            "IsAsciiHexDigitUpper:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiHexDigitUpper(request->arg0)",
                out plan),
            "IsAsciiLetter:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiLetter(request->arg0)",
                out plan),
            "IsAsciiLetterLower:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiLetterLower(request->arg0)",
                out plan),
            "IsAsciiLetterOrDigit:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiLetterOrDigit(request->arg0)",
                out plan),
            "IsAsciiLetterUpper:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsAsciiLetterUpper(request->arg0)",
                out plan),
            "IsBetween:System.Boolean(System.Char,System.Char,System.Char)" => TryCreateStaticTernaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsBetween(request->arg0, request->arg1, request->arg2)",
                out plan),
            "IsHighSurrogate:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsHighSurrogate(request->arg0)",
                out plan),
            "IsLatin1:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsLatin1(request->arg0)",
                out plan),
            "IsLowSurrogate:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsLowSurrogate(request->arg0)",
                out plan),
            "IsSeparatorLatin1:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsSeparatorLatin1(request->arg0)",
                out plan),
            "IsSurrogate:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsSurrogate(request->arg0)",
                out plan),
            "IsSurrogatePair:System.Boolean(System.Char,System.Char)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsSurrogatePair(request->arg0, request->arg1)",
                out plan),
            "IsWhiteSpaceLatin1:System.Boolean(System.Char)" => TryCreateStaticUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::CharIsWhiteSpaceLatin1(request->arg0)",
                out plan),
            _ => false,
        };
    }

    private static bool TryCreateFloatingPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorEquals(request->arg0, request->arg1)"
                : "request->arg0 == request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"!{GetHelperNamespace()}::HalfOperatorEquals(request->arg0, request->arg1)"
                : "request->arg0 != request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_LessThan:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorLessThan(request->arg0, request->arg1)"
                : "request->arg0 < request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_LessThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorLessThanOrEqual(request->arg0, request->arg1)"
                : "request->arg0 <= request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_GreaterThan:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorGreaterThan(request->arg0, request->arg1)"
                : "request->arg0 > request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_GreaterThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorGreaterThanOrEqual(request->arg0, request->arg1)"
                : "request->arg0 >= request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        var unaryHelperName = TryResolveFloatingUnaryHelperName(descriptor, suffix);
        if (unaryHelperName is null)
        {
            return false;
        }

        return TryCreateStaticUnaryPlan(
            descriptor,
            backend,
            "bool",
            $"{GetHelperNamespace()}::{unaryHelperName}(request->arg0)",
            out plan);
    }

    private static string? TryResolveFloatingUnaryHelperName(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix)
    {
        var managedTypeName = descriptor.ManagedTypeName;
        var helperPrefix = descriptor.HelperPrefix;
        return suffix switch
        {
            var current when current == $"IsEvenInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsEvenInteger",
            var current when current == $"IsFinite:System.Boolean({managedTypeName})" => $"{helperPrefix}IsFinite",
            var current when current == $"IsInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsInfinity",
            var current when current == $"IsInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsInteger",
            var current when current == $"IsNaN:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNaN",
            var current when current == $"IsNaNOrZero:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNaNOrZero",
            var current when current == $"IsNegative:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNegative",
            var current when current == $"IsNegativeInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNegativeInfinity",
            var current when current == $"IsNormal:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNormal",
            var current when current == $"IsOddInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsOddInteger",
            var current when current == $"IsPositive:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPositive",
            var current when current == $"IsPositiveInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPositiveInfinity",
            var current when current == $"IsPow2:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPow2",
            var current when current == $"IsRealNumber:System.Boolean({managedTypeName})" => $"{helperPrefix}IsRealNumber",
            var current when current == $"IsSubnormal:System.Boolean({managedTypeName})" => $"{helperPrefix}IsSubnormal",
            var current when current == $"IsZero:System.Boolean({managedTypeName})" => $"{helperPrefix}IsZero",
            _ => null,
        };
    }

    private static bool TryCreateWideIntegerPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateWideBinaryInstancePlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->this_arg, &request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateWideBinaryInstancePlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->this_arg, &request->arg0)",
                out plan);
        }

        return suffix switch
        {
            var current when current == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->arg0, &request->arg1)",
                    out plan),
            var current when current == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"!{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->arg0, &request->arg1)",
                    out plan),
            var current when current == $"op_LessThan:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) < 0",
                    out plan),
            var current when current == $"op_LessThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) <= 0",
                    out plan),
            var current when current == $"op_GreaterThan:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) > 0",
                    out plan),
            var current when current == $"op_GreaterThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) >= 0",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreatePointerIntegerPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->this_arg, request->arg0)",
                out plan);
        }

        var lessToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_LessThan:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_LessThan:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var lessOrEqualToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_LessThanOrEqual:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_LessThanOrEqual:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var greaterToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_GreaterThan:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_GreaterThan:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var greaterOrEqualToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_GreaterThanOrEqual:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_GreaterThanOrEqual:System.Boolean(System.UIntPtr,System.UIntPtr)";

        return suffix switch
        {
            var current when current == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    out plan),
            var current when current == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"!{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    out plan),
            var current when current == lessToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) < 0",
                    out plan),
            var current when current == lessOrEqualToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) <= 0",
                    out plan),
            var current when current == greaterToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) > 0",
                    out plan),
            var current when current == greaterOrEqualToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) >= 0",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateDateTimeLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "Compare:System.Int32(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1)",
                out plan),
            "CompareTo:System.Int32(System.DateTime)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateTime)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Equality:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }

    private static bool TryCreateTimeSpanLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "Compare:System.Int32(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1)",
                out plan),
            "CompareTo:System.Int32(System.TimeSpan)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeSpan)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Equality:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }

    private static bool TryCreateDateOnlyLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "CompareTo:System.Int32(System.DateOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->this_arg, request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->this_arg, request->arg0)",
                out plan),
            "op_Equality:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->arg0, request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->arg0, request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }

    private static bool TryCreateTimeOnlyLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "CompareTo:System.Int32(System.TimeOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->this_arg, request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->this_arg, request->arg0)",
                out plan),
            "op_Equality:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->arg0, request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->arg0, request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }

    private static bool TryCreateInstanceUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(descriptor.ScalarCppType, [descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(null, [descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticBinaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(null, [descriptor.ScalarCppType, descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticTernaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(
                null,
                [descriptor.ScalarCppType, descriptor.ScalarCppType, descriptor.ScalarCppType],
                returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateWideBinaryInstancePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateInstanceUnaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }

    private static bool TryCreateWideBinaryStaticPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateStaticBinaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }

    private static bool TryCreatePointerBackedStaticBinaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateStaticBinaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }

    private static bool TryCreatePointerBackedInstanceUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateInstanceUnaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }

    private static bool TryCreatePlanCore(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        RuntimeSkeletonKernelCarrierSchema carrierSchema,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = new RuntimeSkeletonKernelEmissionPlan(
            ContractId,
            descriptor.SemanticId,
            backend,
            carrierSchema,
            helperCallExpression);
        return true;
    }

    private static string GetHelperNamespace() => "chaos::il2cpp::runtime_core";
}

using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

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
}

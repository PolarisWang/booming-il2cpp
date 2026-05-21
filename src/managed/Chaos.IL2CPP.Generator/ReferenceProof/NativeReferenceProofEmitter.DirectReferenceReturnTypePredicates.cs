using System;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static bool IsEncodingDirectReferenceReturnType(string managedType) =>
        managedType.StartsWith("System.Text.", StringComparison.Ordinal) ||
        managedType is "System.String" or "System.Object";

    private static bool IsIODirectReferenceReturnType(string managedType) =>
        managedType.StartsWith("System.IO.", StringComparison.Ordinal) ||
        managedType is "System.String" or "System.Object";

    private static bool IsTracingDirectReferenceReturnType(string managedType) =>
        managedType.StartsWith("System.Diagnostics.Tracing.", StringComparison.Ordinal) ||
        managedType is "System.String" or "System.Object";

    private static bool IsReflectionDirectReferenceReturnType(string managedType) =>
        managedType.StartsWith("System.Reflection.", StringComparison.Ordinal) ||
        managedType.StartsWith("System.RuntimeType", StringComparison.Ordinal) ||
        managedType.StartsWith("System.Type", StringComparison.Ordinal) ||
        managedType is "System.String" or "System.Object";

    private static bool IsStringBuilderDirectReferenceReturnType(string managedType) =>
        managedType is "System.Text.StringBuilder" or "System.String" or "System.Object";
}

using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private bool TryCreateCustomAttributeRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
    {
        helperDefinition = null;
        // Skip constructors — let the catch-all path handle them with
        // a uniform CHAOS_IL2CPP_INTPTR return type to avoid C2733/C2556
        // overload conflicts with extern "C" ChaosExternalRuntimeFallback.
        if (callee.Contains(".ctor", StringComparison.Ordinal) ||
            callee.Contains(".cctor", StringComparison.Ordinal))
            return false;

        if (_customAttributeSupport.UsesMemberInfoIsDefined && MatchesMethodSubject(callee, "System.Private.CoreLib/System.Reflection.MemberInfo", "IsDefined", "System.Type", "System.Boolean"))
        {
            helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
                "CHAOS_IL2CPP_INT32",
                GetExternalRuntimeHelperSymbol(callee),
                "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                [
                    "    (void)chaos_arg_2;",
                    "    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
                    "    {",
                    "        return 0;",
                    "    }",
                    string.Empty,
                    $"    auto* chaos_type = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.Type")}*>(chaos_arg_1);",
                    "    if (chaos_type->runtime_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
                    "    {",
                    "        return 0;",
                    "    }",
                    string.Empty,
                    "    return chaos_reflection_get_custom_attribute(chaos_arg_0, chaos_type->runtime_type_handle) == static_cast<CHAOS_IL2CPP_INTPTR>(0)",
                    "        ? 0",
                    "        : 1;",
                ]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
            {
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                CreateInt32AbiSlot()
            }), CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
            return true;
        }
        if (_customAttributeSupport.QueryAttributeTypeByCallee.TryGetValue(callee, out string? value))
        {
            helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
                "CHAOS_IL2CPP_INTPTR",
                GetExternalRuntimeHelperSymbol(callee),
                "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    $"    return chaos_reflection_get_custom_attribute(chaos_arg_0, {GetTypeHandleLiteral(value)});",
                ]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(value, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
            return true;
        }
        if (_customAttributeSupport.SyntheticGetterFieldByMethodSubjectId.TryGetValue(callee, out string? value2))
        {
            string declaringTypeSubjectId = GetDeclaringTypeSubjectId(value2!);
            helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
                "CHAOS_IL2CPP_INTPTR",
                GetExternalRuntimeHelperSymbol(callee),
                "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
                    "    {",
                    "        return static_cast<CHAOS_IL2CPP_INTPTR>(0);",
                    "    }",
                    string.Empty,
                    $"    auto* chaos_attribute = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>(chaos_arg_0);",
                    $"    return chaos_attribute->{GetNativeFieldMemberName(value2)};",
                ]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(declaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
            return true;
        }
        return false;
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveInterfaceDispatchTargets(string interfaceMethodSubjectId)
    {
        string interfaceTypeSubjectId = GetMethodDeclaringTypeSubjectId(interfaceMethodSubjectId);
        string slotSignatureSuffix = GetMethodSignatureSuffix(interfaceMethodSubjectId);
        return _methodsBySubjectId.Values.Where(delegate (AotCoreIrMethodArtifact method)
        {
            if (method.IsStatic || !CanEmitMethodBody(method) || !string.Equals(GetMethodSignatureSuffix(method.SubjectId), slotSignatureSuffix, StringComparison.Ordinal))
            {
                return false;
            }
            string declaringTypeSubjectId = method.Identity.DeclaringTypeSubjectId;
            return !string.IsNullOrEmpty(declaringTypeSubjectId) && ImplementsInterface(declaringTypeSubjectId, interfaceTypeSubjectId, interfaceTypeSubjectId);
        }).OrderBy((AotCoreIrMethodArtifact method) => method.SubjectId, StringComparer.Ordinal).ToArray();
    }

    private static AotCoreIrAbiSlotArtifact CreateVoidAbiSlot()
    {
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
            TypeShape = (AotCoreIrTypeShapeKind)0
        };
    }

    private static bool MatchesMethodSubject(string subjectId, string declaringTypeSubjectId, string methodName, params string[] parameterTypes)
    {
        // Strip assembly prefix (e.g. "System.Private.CoreLib/System.String" -> "System.String")
        // to make matching assembly-agnostic. The typeDisplayName is everything after the first '/'.
        var slashIndex = declaringTypeSubjectId.IndexOf('/');
        var typeDisplayName = slashIndex >= 0
            ? declaringTypeSubjectId[(slashIndex + 1)..]
            : declaringTypeSubjectId;
        return ManagedNaming.MatchesMethod(subjectId, typeDisplayName, methodName, parameterTypes);
    }
}

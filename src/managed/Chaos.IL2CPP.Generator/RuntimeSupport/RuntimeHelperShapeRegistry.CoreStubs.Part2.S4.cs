using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// MemberInfo::get_ReflectedType
        /// </summary>
        private static void RegisterMemberInfogetReflectedType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MemberInfo", "get_ReflectedType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetReflectedType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Module::get_FullyQualifiedName
        /// </summary>
        private static void RegisterModulegetFullyQualifiedName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Module", "get_FullyQualifiedName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetModuleName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// ParameterInfo::get_ParameterType
        /// </summary>
        private static void RegisterParameterInfogetParameterType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_ParameterType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// FieldInfo::get_FieldType
        /// </summary>
        private static void RegisterFieldInfogetFieldType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.FieldInfo", "get_FieldType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetFieldType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// PropertyInfo::get_PropertyType
        /// </summary>
        private static void RegisterPropertyInfogetPropertyType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.PropertyInfo", "get_PropertyType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetPropertyType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Reflection accessors that existed natively but were never registered.
        ///
        /// Each of these has a real implementation in
        /// src/native/runtime-core/reflection_api.h (listed in the comment above each
        /// registration).  Without a shape entry the call falls through to the
        /// catch-all external-runtime fallback, which is declared with ZERO parameters
        /// and answers 0 — so the caller silently receives 0 for a method that takes
        /// one, and every downstream subject dies on its own null-guard before reaching
        /// any assertion.  That is the failure shape the reflection triage recorded as
        /// "raised before any assertion ran".
        ///
        /// These are all instance accessors taking `this` and nothing else, which is
        /// why they share the identical ABI shape below.
        /// </summary>
        private static void RegisterUnregisteredReflectionAccessors(RuntimeHelperShapeRegistry registry)
        {
            // FieldInfo::GetRawConstantValue → ChaosReflectionPropertyGetRawConstantValue
            // (the native entry is named ...Property... but is the shared constant-value
            //  decoder; FieldInfo and PropertyInfo both expose the method).
            registry.Register("System.Reflection.FieldInfo", "GetRawConstantValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetRawConstantValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // PropertyInfo::GetRawConstantValue → ChaosReflectionPropertyGetRawConstantValue
            registry.Register("System.Reflection.PropertyInfo", "GetRawConstantValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetRawConstantValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // PropertyInfo::GetSetMethod() / GetSetMethod(bool)
            // The native entry (ChaosReflectionPropertyGetSetMethod:70) takes only
            // `this` — the managed `nonPublic` bool is not forwarded, so BOTH arities
            // bind to the same 1-arg symbol.  Registering the 2-arg form with the
            // full managed parameter list would emit a 2-argument call into a
            // 1-parameter function (C2660), so the ABI slot list is `this` only for
            // both registrations while the parameter-type key keeps them distinct.
            registry.Register("System.Reflection.PropertyInfo", "GetSetMethod", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetSetMethod",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.PropertyInfo", "GetSetMethod", ["System.Boolean"],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetSetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[]
                {
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot()
                }),
                CreateNativeIntAbiSlot("System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // PropertyInfo::GetIndexParameters → already registered in
            // RuntimeHelperShapeRegistry.CoreStubs.Part3.S16.cs (its native entry
            // ChaosReflectionPropertyGetIndexParameters returns an empty array —
            // see the note below about what that means for the caller).

            // ParameterInfo::GetModifiedParameterType → ChaosReflectionParamGetModifiedParameterType
            registry.Register("System.Reflection.ParameterInfo", "GetModifiedParameterType", [],
                ShapeKind.SimpleForward, "ChaosReflectionParamGetModifiedParameterType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // ParameterInfo::GetRealObject(StreamingContext) → ChaosReflectionParamGetRealObject
            // Native entry takes only `this` (reflection_api.h:292); the
            // StreamingContext struct is not forwarded.
            registry.Register("System.Reflection.ParameterInfo", "GetRealObject",
                ["System.Runtime.Serialization.StreamingContext"],
                ShapeKind.SimpleForward, "ChaosReflectionParamGetRealObject",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // MemberInfo::HasSameMetadataDefinitionAs(MemberInfo) → returns bool
            registry.Register("System.Reflection.MemberInfo", "HasSameMetadataDefinitionAs",
                ["System.Reflection.MemberInfo"],
                ShapeKind.SimpleForward, "ChaosReflectionMemberHasSameMetadataDefinitionAs",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Reflection.MemberInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Reflection.MemberInfo", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── ParameterInfo::GetCustomAttributesData / Assembly / Module / MemberInfo ──
            // Four overloads with the same arity but different receivers; each needs its
            // own registration because the registry keys on (declaring type, name, arity).
            registry.Register("System.Reflection.ParameterInfo", "GetCustomAttributesData", [],
                ShapeKind.SimpleForward, "ChaosReflectionParamGetCustomAttributesData",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.Assembly", "GetCustomAttributesData", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetCustomAttributesData",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.Module", "GetCustomAttributesData", [],
                ShapeKind.SimpleForward, "ChaosReflectionModuleGetCustomAttributesData",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Module", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MemberInfo", "GetCustomAttributesData", [],
                ShapeKind.SimpleForward, "ChaosReflectionMemberGetCustomAttributesData",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.MemberInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }

    }
}

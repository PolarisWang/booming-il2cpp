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

    }
}

using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Type::get_IsGenericType
        /// </summary>
        private static void RegisterTypegetIsGenericType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsGenericType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsGenericType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// MethodBase::get_IsPublic
        /// </summary>
        private static void RegisterMethodBasegetIsPublic(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodBase", "get_IsPublic", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Module::GetTypes
        /// </summary>
        private static void RegisterModuleGetTypes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Module", "GetTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionModuleGetTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// ParameterInfo::get_Attributes
        /// </summary>
        private static void RegisterParameterInfogetAttributes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_Attributes", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParamAttributes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsAbstract
        /// </summary>
        private static void RegisterTypegetIsAbstract(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsAbstract", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsAbstract",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}

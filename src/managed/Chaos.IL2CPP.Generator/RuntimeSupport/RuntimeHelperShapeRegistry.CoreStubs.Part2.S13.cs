using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// RuntimeMethodHandle::GetHashCode
        /// </summary>
        private static void RegisterRuntimeMethodHandleGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.RuntimeMethodHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimemethodhandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeMethodHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Module::get_Assembly
        /// </summary>
        private static void RegisterModulegetAssembly(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Module", "get_Assembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetModuleAssembly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// ParameterInfo::get_RawDefaultValue
        /// </summary>
        private static void RegisterParameterInfogetRawDefaultValue(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_RawDefaultValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetRawDefaultValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_AssemblyQualifiedName
        /// </summary>
        private static void RegisterTypegetAssemblyQualifiedName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_AssemblyQualifiedName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyQualifiedName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// RuntimeHelpers::GetObjectValue
        /// </summary>
        private static void RegisterRuntimeHelpersGetObjectValue(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "GetObjectValue", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersGetObjectValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}

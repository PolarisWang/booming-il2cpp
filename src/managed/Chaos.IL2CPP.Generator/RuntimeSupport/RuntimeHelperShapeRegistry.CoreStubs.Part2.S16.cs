using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// MethodBase::get_CallingConvention
        /// </summary>
        private static void RegisterMethodBasegetCallingConvention(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodBase", "get_CallingConvention", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetCallingConvention",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Module::GetType
        /// </summary>
        private static void RegisterModuleGetType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Module", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionModuleGetType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// ParameterInfo::get_HasDefaultValue
        /// </summary>
        private static void RegisterParameterInfogetHasDefaultValue(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_HasDefaultValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionHasDefaultValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// RuntimeTypeHandle::GetHashCode
        /// </summary>
        private static void RegisterRuntimeTypeHandleGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.RuntimeTypeHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimetypehandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeTypeHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// RuntimeFieldHandle::GetHashCode
        /// </summary>
        private static void RegisterRuntimeFieldHandleGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.RuntimeFieldHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimefieldhandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeFieldHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}

using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Assembly::GetForwardedTypes (stub returning null)
        /// </summary>
        private static void RegisterAssemblyGetForwardedTypes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetForwardedTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetForwardedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::MakeByRefType (SimpleForward stub)
        /// </summary>
        private static void RegisterTypeMakeByRefType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "MakeByRefType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakeByRefType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsPointer (SimpleForward stub)
        /// </summary>
        private static void RegisterTypegetIsPointer(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsPointer", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsPointer",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// FieldInfo::get_IsInitOnly (stub returning false)
        /// </summary>
        private static void RegisterFieldInfogetIsInitOnly(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.FieldInfo", "get_IsInitOnly", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsInitOnly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Math::Sin (SimpleForward stub)
        /// </summary>
        private static void RegisterMathSin(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Sin", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathSin",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

    }
}

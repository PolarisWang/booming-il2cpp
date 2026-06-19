using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Guid::NewGuid
        /// </summary>
        private static void RegisterGuidNewGuid(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", "NewGuid", [],
                ShapeKind.SimpleForward, "ChaosGuidNewGuid",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.Guid"
                },
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Guid::GetHashCode
        /// </summary>
        private static void RegisterGuidGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosGuidGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.Guid"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Guid::ToString
        /// </summary>
        private static void RegisterGuidToString(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", "ToString", [],
                ShapeKind.SimpleForward, "ChaosGuidToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.Guid"
                    }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Int32::GetHashCode
        /// </summary>
        private static void RegisterInt32GetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Int32", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosInt32GetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),  // "this" is managed pointer to Int32
                CreateInt32AbiSlot(),            // returns CHAOS_IL2CPP_INT32
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Math::Abs (SimpleForward stubs)
        /// </summary>
        private static void RegisterMathAbs(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Abs", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathAbsInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Math", "Abs", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathAbsDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

    }
}

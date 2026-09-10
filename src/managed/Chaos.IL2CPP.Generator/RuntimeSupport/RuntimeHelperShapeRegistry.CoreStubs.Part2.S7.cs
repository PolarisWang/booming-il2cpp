using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Math::Ceiling (SimpleForward stub)
        /// </summary>
        private static void RegisterMathCeiling(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Ceiling", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathCeiling",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // Math::Ceiling(System.Decimal) → Decimal — DecimalCarrier* in/out.
            // Forward to the native DECIMAL impl so the ATG wrapper AOT-lowers the call
            // to a real 1-arg native (instead of the 0-arg catch-all that drops the
            // 16-byte Decimal carrier).
            registry.Register("System.Math", "Ceiling", ["System.Decimal"],
                ShapeKind.SimpleForward, "ChaosMathDecimalCeiling",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Math::Truncate(System.Decimal) → Decimal (SimpleForward stub)
        /// </summary>
        private static void RegisterMathTruncate(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Truncate", ["System.Decimal"],
                ShapeKind.SimpleForward, "ChaosMathDecimalTruncate",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Random::NextDouble
        /// </summary>
        private static void RegisterRandomNextDouble(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Random", "NextDouble", [],
                ShapeKind.SimpleForward, "ChaosRandomNextDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Array::CreateInstance (SimpleForward — elementType, length)
        /// </summary>
        private static void RegisterArrayCreateInstance(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Array", "CreateInstance", ["System.Type", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosArrayCreateInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Assembly::GetCallingAssembly
        /// </summary>
        private static void RegisterAssemblyGetCallingAssembly(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetCallingAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetCallingAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// MethodInfo::GetBaseDefinition
        /// </summary>
        private static void RegisterMethodInfoGetBaseDefinition(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodInfo", "GetBaseDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetBaseDefinition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}

using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Type::IsInstanceOfType
        /// </summary>
        private static void RegisterTypeIsInstanceOfType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "IsInstanceOfType", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosReflectionIsInstanceOfType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// CultureInfo::get_DisplayName
        /// </summary>
        private static void RegisterCultureInfogetDisplayName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_DisplayName", [],
                ShapeKind.SimpleForward, "ChaosCultureGetDisplayName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Math::Round (SimpleForward stubs)
        /// </summary>
        private static void RegisterMathRound(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Round", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathRound",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Math", "Round", ["System.Double", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathRound2",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // Math::Round(System.Decimal) → Decimal (DecimalCarrier* in/out)
            registry.Register("System.Math", "Round", ["System.Decimal"],
                ShapeKind.SimpleForward, "ChaosMathDecimalRound",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Assembly::get_FullName
        /// </summary>
        private static void RegisterAssemblygetFullName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "get_FullName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyFullName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_ContainsGenericParameters
        /// </summary>
        private static void RegisterTypegetContainsGenericParameters(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_ContainsGenericParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetContainsGenericParams",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}

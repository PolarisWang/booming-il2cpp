using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Exception::GetBaseException
        /// </summary>
        private static void RegisterExceptionGetBaseException(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Exception", "GetBaseException", [],
                ShapeKind.SimpleForward, "ChaosExceptionGetBaseException",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// CultureInfo::get_Name
        /// </summary>
        private static void RegisterCultureInfogetName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosCultureGetName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Math::Floor (SimpleForward stub)
        /// </summary>
        private static void RegisterMathFloor(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Floor", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathFloor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // Math::Floor(System.Decimal) → Decimal (DecimalCarrier* in/out)
            registry.Register("System.Math", "Floor", ["System.Decimal"],
                ShapeKind.SimpleForward, "ChaosMathDecimalFloor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Assembly::GetEntryAssembly
        /// </summary>
        private static void RegisterAssemblyGetEntryAssembly(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetEntryAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetEntryAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Type::get_GenericParameterPosition
        /// </summary>
        private static void RegisterTypegetGenericParameterPosition(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_GenericParameterPosition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericParamPos",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}

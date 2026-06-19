using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// CultureInfo::get_NumberFormat
        /// </summary>
        private static void RegisterCultureInfogetNumberFormat(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_NumberFormat", [],
                ShapeKind.SimpleForward, "ChaosCultureGetNumberFormat",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// HashCode::Add<T> (SimpleForward)
        /// </summary>
        private static void RegisterHashCodeAddT(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.HashCode", "Add", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosHashCodeAdd",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.HashCode", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Math::Min (SimpleForward stubs)
        /// </summary>
        private static void RegisterMathMin(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Min", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathMinInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Math", "Min", ["System.Double", "System.Double"],
                ShapeKind.SimpleForward, "ChaosMathMinDouble",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Assembly::get_ImageRuntimeVersion
        /// </summary>
        private static void RegisterAssemblygetImageRuntimeVersion(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "get_ImageRuntimeVersion", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetImageRuntimeVersion",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsGenericTypeDefinition
        /// </summary>
        private static void RegisterTypegetIsGenericTypeDefinition(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsGenericTypeDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsGenericTypeDef",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}

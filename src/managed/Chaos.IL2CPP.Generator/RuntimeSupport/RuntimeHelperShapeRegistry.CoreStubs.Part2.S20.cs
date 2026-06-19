using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// HashSet<T>::Remove (GenericShapeDescriptor -- calls CollectionHashSetRemove)
        /// </summary>
        private static void RegisterHashSetTRemove(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
                MethodName: "Remove",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return CollectionHashSetRemove(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// CultureInfo::get_DateTimeFormat
        /// </summary>
        private static void RegisterCultureInfogetDateTimeFormat(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_DateTimeFormat", [],
                ShapeKind.SimpleForward, "ChaosCultureGetDateTimeFormat",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Random::NextBytes
        /// </summary>
        private static void RegisterRandomNextBytes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Random", "NextBytes", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosRandomNextBytes",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Math::Max (SimpleForward stubs)
        /// </summary>
        private static void RegisterMathMax(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Max", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathMaxInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Math", "Max", ["System.Double", "System.Double"],
                ShapeKind.SimpleForward, "ChaosMathMaxDouble",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Assembly::get_Location
        /// </summary>
        private static void RegisterAssemblygetLocation(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "get_Location", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyLocation",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}

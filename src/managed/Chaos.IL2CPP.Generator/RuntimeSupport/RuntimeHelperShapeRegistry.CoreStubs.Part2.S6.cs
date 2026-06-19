using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// DateTime::ToString (SimpleForward stubs)
        /// </summary>
        private static void RegisterDateTimeToString(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "ToString", [],
                ShapeKind.SimpleForward, "ChaosDateTimeToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.DateTime", "ToString", ["System.String"],
                ShapeKind.SimpleForward, "ChaosDateTimeToStringFormat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });



        }

        /// <summary>
        /// CustomAttributeExtensions::IsDefined
        /// </summary>
        private static void RegisterCustomAttributeExtensionsIsDefined(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.CustomAttributeExtensions", "IsDefined",
                ["System.Reflection.Assembly", "System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsDefined",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Exception::get_HResult
        /// </summary>
        private static void RegisterExceptiongetHResult(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Exception", "get_HResult", [],
                ShapeKind.SimpleForward, "ChaosExceptionGetHresult",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// CultureInfo::get_InvariantCulture
        /// </summary>
        private static void RegisterCultureInfogetInvariantCulture(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_InvariantCulture", [],
                ShapeKind.SimpleForward, "ChaosCultureGetInvariant",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Random::Next (GenericShapeDescriptor -- handles all overloads)
        /// </summary>
        private static void RegisterRandomNext(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Random",
                MethodName: "Next",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "",
                        [
                            "    return ChaosRandomNext(0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        "    return ChaosRandomNextMax(0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

    }
}

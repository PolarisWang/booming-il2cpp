using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// BCL token stubs (Option C: value type as NativeInt token, primitives return 0)
        /// </summary>
        private static void RegisterBCLtokenstubs(RuntimeHelperShapeRegistry registry)
        {
            // These let the pipeline complete without lowering BCL methods.
            // Individual method failures are handled as runtime-semantics issues (Cat E).

        }

        /// <summary>
        /// Guid::.ctor(String)
        /// </summary>
        private static void RegisterGuidctor(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosGuidCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Guid::.ctor(Byte[])
        /// </summary>
        private static void RegisterGuidctor_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", ".ctor", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosGuidCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Guid::Parse(string)
        /// </summary>
        private static void RegisterGuidParse(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Guid", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosGuidParse",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.Guid"
                },
                new HashSet<int> { 0 });

            // Guid::NewGuid, GetHashCode, ToString registered as SimpleForward above
            // (removed shadowing stubs that returned 0)

            // ── Guid GenericShapeDescriptor registrations ─────────────────
            // These enable DirectNativeSymbol so emitted code calls
            // ChaosGuidNewGuid() directly instead of through
            // kChaosExternalRuntimeFnTable[idx](), eliminating indirect
            // dispatch overhead.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Guid",
                MethodName: "NewGuid",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        ["    return ChaosGuidNewGuid();"]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        new AotCoreIrAbiSlotArtifact
                        {
                            CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                            TypeShape = AotCoreIrTypeShapeKind.ValueType,
                            TypeSubjectId = "System.Private.CoreLib/System.Guid"
                        },
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosGuidNewGuid");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Guid",
                MethodName: "GetHashCode",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return ChaosGuidGetHashCode(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact
                            {
                                CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                                TypeShape = AotCoreIrTypeShapeKind.ValueType,
                                TypeSubjectId = "System.Private.CoreLib/System.Guid"
                            }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGuidGetHashCode");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Guid",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return ChaosGuidToString(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact
                            {
                                CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                                TypeShape = AotCoreIrTypeShapeKind.ValueType,
                                TypeSubjectId = "System.Private.CoreLib/System.Guid"
                            }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGuidToString");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Guid",
                MethodName: ".ctor",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.String")
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            ["    ChaosGuidCtor(chaos_arg_0, chaos_arg_1);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosGuidCtor");
                    }
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.Byte[]")
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            ["    ChaosGuidCtor(chaos_arg_0, chaos_arg_1);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosGuidCtor");
                    }
                    return null;
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Guid",
                MethodName: "Parse",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return ChaosGuidParse(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        new AotCoreIrAbiSlotArtifact
                        {
                            CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                            TypeShape = AotCoreIrTypeShapeKind.ValueType,
                            TypeSubjectId = "System.Private.CoreLib/System.Guid"
                        },
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGuidParse");
                }));

        }

        /// <summary>
        /// Random::.ctor
        /// </summary>
        private static void RegisterRandomctor(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Random", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosRandomCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Random",
                MethodName: "Next",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var instanceAbi = CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType);
                    if (paramTypes.Count == 0)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return ChaosRandomNext(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(instanceAbi),
                            CreateInt32AbiSlot(), new HashSet<int> { 0 });
                    }
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    (void)chaos_arg_0;",
                            "    return ChaosRandomNextMax(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                instanceAbi, CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
                    }
                    if (paramTypes.Count == 2)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                        [
                            "    (void)chaos_arg_0;",
                            "    return ChaosRandomNextMax(chaos_arg_0, chaos_arg_2 - chaos_arg_1) + chaos_arg_1;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                instanceAbi, CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(), new HashSet<int> { 0, 1, 2 });
                    }
                    return null;
                }));

            // Removed: GenericShapeDescriptor for NextDouble (uses SimpleForward ChaosRandomNextDouble)
            // Removed: GenericShapeDescriptor for NextBytes (uses SimpleForward ChaosRandomNextBytes)

        }

    }
}

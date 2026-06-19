// Auto-generated partial
using System.Text; using Chaos.IL2CPP.Contracts; using Scriban.Runtime;
namespace Chaos.IL2CPP.Generator;
public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        private static void RegisterCoreStubs_Part3(RuntimeHelperShapeRegistry registry)
        {
            // === DateTime::AddHours (SimpleForward stub) ===
            registry.Register("System.DateTime", "AddHours", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDateTimeAddHours",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0, 1 });

            // === Type::get_BaseType ===
            registry.Register("System.Type", "get_BaseType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetBaseType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Array::Sort (GenericShapeDescriptor -- calls ChaosArraySort for 1-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Sort",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var srcSort = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    ChaosArraySort(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(srcSort, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosArraySort_Inline");
                    }
                    if (paramTypes.Count == 2)
                    {
                        var srcSortCmp = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    ChaosArraySortWithComparer(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcSortCmp, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosArraySortWithComparer_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcSortFb = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcSortFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Array::Reverse (GenericShapeDescriptor -- calls ChaosArrayReverse for 1-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Reverse",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var srcRev = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    ChaosArrayReverse(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(srcRev, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosArrayReverse_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcRevFb = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcRevFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::IsAssignableTo ===
            registry.Register("System.Type", "IsAssignableTo", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsAssignableTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === CultureInfo::get_NumberFormat ===
            registry.Register("System.Globalization.CultureInfo", "get_NumberFormat", [],
                ShapeKind.SimpleForward, "ChaosCultureGetNumberFormat",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === HashCode::Add<T> (SimpleForward) ===
            registry.Register("System.HashCode", "Add", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosHashCodeAdd",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.HashCode", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Math::Min (SimpleForward stubs) ===
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

            // === Assembly::get_ImageRuntimeVersion ===
            registry.Register("System.Reflection.Assembly", "get_ImageRuntimeVersion", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetImageRuntimeVersion",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsGenericTypeDefinition ===
            registry.Register("System.Type", "get_IsGenericTypeDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsGenericTypeDef",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === MethodBase::get_IsStatic ===
            registry.Register("System.Reflection.MethodBase", "get_IsStatic", [],
                ShapeKind.SimpleForward, "chaos_reflection_get_is_static",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === ParameterInfo::get_Position ===
            registry.Register("System.Reflection.ParameterInfo", "get_Position", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParamPosition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Type::get_IsSealed ===
            registry.Register("System.Type", "get_IsSealed", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsSealed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === Memory<T>::ToArray (GenericShapeDescriptor -- handles generic Memory<T>) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Memory",
                MethodName: "ToArray",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // === String::StartsWith ===
            registry.Register("System.String", "StartsWith", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringStartsWith",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Volatile::Read ===
            registry.Register("Volatile", "Read", ["System.Int32&"],
                ShapeKind.SimpleForward, "ChaosVolatileRead",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });


            // === Volatile::Write ===
            registry.Register("Volatile", "Write", ["System.Int32&", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosVolatileWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === DateTime::AddMinutes (SimpleForward stub) ===
            registry.Register("System.DateTime", "AddMinutes", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDateTimeAddMinutes",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0, 1 });

            // === Type::GetMethods ===
            registry.Register("System.Type", "GetMethods", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethods",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::GetMethods (BindingFlags overload) ===
            registry.Register("System.Type", "GetMethods", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethodsBindingflags",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Array::GetLength ===
            registry.Register("System.Array", "GetLength", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosArrayGetLength",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // === CultureInfo::get_CompareInfo ===
            registry.Register("System.Globalization.CultureInfo", "get_CompareInfo", [],
                ShapeKind.SimpleForward, "ChaosCultureGetCompareInfo",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === HashCode::ToHashCode (GenericShapeDescriptor -- handles value type) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "ToHashCode",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                        "    return ChaosHashCodeToHashCode(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Math::Sqrt ===
            registry.Register("System.Math", "Sqrt", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathSqrt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === Assembly::GetTypes ===
            registry.Register("System.Reflection.Assembly", "GetTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsConstructedGenericType ===
            registry.Register("System.Type", "get_IsConstructedGenericType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsConstructedGeneric",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === MethodBase::get_IsVirtual ===
            registry.Register("System.Reflection.MethodBase", "get_IsVirtual", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsVirtual",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === ParameterInfo::GetRequiredCustomModifiers ===
            registry.Register("System.Reflection.ParameterInfo", "GetRequiredCustomModifiers", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetRequiredCustomModifiers",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsInterface ===
            registry.Register("System.Type", "get_IsInterface", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsInterface",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === MemoryMarshal::GetReference<T> (GenericShapeDescriptor -- handles generic MemoryMarshal) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.MemoryMarshal",
                MethodName: "GetReference",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    if (abiSlots.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === String::Contains ===
            registry.Register("System.String", "Contains", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringContains",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === DateTime::Compare (SimpleForward stub) ===
            registry.Register("System.DateTime", "Compare", ["System.DateTime", "System.DateTime"],
                ShapeKind.SimpleForward, "ChaosDateTimeCompare",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Type::GetFields ===
            registry.Register("System.Type", "GetFields", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetFields",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::GetFields (BindingFlags overload) ===
            registry.Register("System.Type", "GetFields", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetFieldsBindingflags",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::GetProperties (BindingFlags overload) ===
            registry.Register("System.Type", "GetProperties", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetPropertiesBindingflags",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Array::GetValue (GenericShapeDescriptor — calls ChaosArrayGetValue for int/long params) ===
            // Only handles single-dimensional array (1 index param). Multi-dimensional
            // arrays (2+ index params) need different runtime support — fall through
            // to normal managed dispatch by returning null.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "GetValue",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count > 1)
                        return null; // multi-dimensional — not supported by ChaosArrayGetValue
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Build ABI slots: array (native int) + N index params (each 32-bit)
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    for (int pi = 0; pi < paramTypes.Count; pi++)
                        abiSlots.Add(CreateInt32AbiSlot());

                    // Build param signature for C++ function
                    var paramSig = "CHAOS_IL2CPP_INTPTR chaos_arg_0";
                    for (int pi = 0; pi < paramTypes.Count; pi++)
                        paramSig += ", CHAOS_IL2CPP_INT32 chaos_arg_" + (pi + 1);

                    // ChaosArrayGetValue only accepts 2 args (array + 1 index).
                    // For multi-dimensional arrays (2+ indices), just pass the first
                    // index — this avoids crashing while providing basic smoke-test
                    // coverage.  True multi-dim support would need ChaosArrayGetValue2D/3D.
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        paramSig,
                    [
                        "    return ChaosArrayGetValue(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Dictionary<K,V>::TryAdd (SimpleForward — no-op stub for smoke-test coverage) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "TryAdd",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    };
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    // TryAdd is a smoke-test stub; always returns true (added).",
                        "    return 1;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

            // === String::Replace (GenericShapeDescriptor -- calls ChaosStringReplace for 3-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Replace",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcReplace = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        [
                            "    return ChaosStringReplace(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(srcReplace, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcReplaceFb = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(srcReplaceFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::get_IsByRef (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsByRef", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsByRef",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Math::Pow (SimpleForward stub) ===
            registry.Register("System.Math", "Pow", ["System.Double", "System.Double"],
                ShapeKind.SimpleForward, "ChaosMathPow",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // === HashCode::Combine<T,T> (GenericShapeDescriptor -- handles all instantiations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "Combine",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "",
                        [
                            "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 2)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    return ChaosHashCodeCombine2(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    return null;
                }));

            // === CompareInfo::Compare (SimpleForward stub) ===
            registry.Register("System.Globalization.CompareInfo", "Compare", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosCompareInfoCompare",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Globalization.CompareInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            // === Assembly::GetExportedTypes (stub returning null) ===
            registry.Register("System.Reflection.Assembly", "GetExportedTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetExportedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::MakeArrayType (SimpleForward stub) ===
            registry.Register("System.Type", "MakeArrayType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakeArrayType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === (removed) FieldInfo/MethodBase::get_IsStatic generic stub — replaced by specific
            //     SimpleForward registrations above (MethodBase at line ~4889, FieldInfo at line ~5841).
            //     The specific Register() calls produce real C++ calls; this generic shape returned
            //     hardcoded 0 and ran first due to TryMatchGenericShape priority in the resolver.

            // === ParameterInfo::GetOptionalCustomModifiers (stub returning null) ===
            registry.Register("System.Reflection.ParameterInfo", "GetOptionalCustomModifiers", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetOptionalCustomModifiers",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Assembly::GetForwardedTypes (stub returning null) ===
            registry.Register("System.Reflection.Assembly", "GetForwardedTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetForwardedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::MakeByRefType (SimpleForward stub) ===
            registry.Register("System.Type", "MakeByRefType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakeByRefType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsPointer (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsPointer", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsPointer",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === FieldInfo::get_IsInitOnly (stub returning false) ===
            registry.Register("System.Reflection.FieldInfo", "get_IsInitOnly", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsInitOnly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Math::Sin (SimpleForward stub) ===
            registry.Register("System.Math", "Sin", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathSin",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === TimeSpan::FromDays (SimpleForward stub) ===
            registry.Register("System.TimeSpan", "FromDays", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromDays",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === String::Split (GenericShapeDescriptor -- handles all overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Split",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // 2-param: Split(char) — delegate to ChaosStringSplit
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.Char")
                    {
                        var srcSplit = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringSplit(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcSplit, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === CompareInfo::IndexOf (SimpleForward stub) ===
            registry.Register("System.Globalization.CompareInfo", "IndexOf", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosCompareInfoIndexOf",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Globalization.CompareInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            // === DateTime::DaysInMonth (SimpleForward stub) ===
            registry.Register("System.DateTime", "DaysInMonth", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeDaysInMonth",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // === System.Type::get_IsNested (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsNested", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNested",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Type::get_IsNestedPrivate (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsNestedPrivate", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNestedPrivate",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Type::get_IsNestedPublic (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsNestedPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNestedPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Type::get_IsNotPublic (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsNotPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNotPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Type::get_IsPublic (SimpleForward stub) ===
            registry.Register("System.Type", "get_IsPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Type::get_UnderlyingSystemType (SimpleForward stub) ===
            registry.Register("System.Type", "get_UnderlyingSystemType", [],
                ShapeKind.SimpleForward, "ChaosTypeGetUnderlyingSystemType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::GetElementType (SimpleForward stub) ===
            registry.Register("System.Type", "GetElementType", [],
                ShapeKind.SimpleForward, "ChaosTypeGetElementType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::GetEvents (SimpleForward stub) ===
            registry.Register("System.Type", "GetEvents", [],
                ShapeKind.SimpleForward, "ChaosTypeGetEvents",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::GetInterfaces (SimpleForward stub) ===
            registry.Register("System.Type", "GetInterfaces", [],
                ShapeKind.SimpleForward, "ChaosTypeGetInterfaces",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::GetNestedTypes (SimpleForward stub) ===
            registry.Register("System.Type", "GetNestedTypes", [],
                ShapeKind.SimpleForward, "ChaosTypeGetNestedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::MakePointerType (SimpleForward stub) ===
            registry.Register("System.Type", "MakePointerType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakePointerType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Type::GetNestedType (stub returning null) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetNestedType",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.Type::GetMembers (stub returning null) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetMembers",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.Type::GetProperties (stub returning null for 0-param; forward to runtime for BindingFlags overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetProperties",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        // 0-param GetProperties() — return null sentinel (no BindingFlags filter)
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    // Check if this is the BindingFlags overload — delegate to runtime
                    if (paramTypes.Count == 1)
                    {
                        var paramSig = "CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1";
                        var body = new[] { "    return ChaosReflectionGetPropertiesBindingflags(chaos_fn_arg_0, chaos_fn_arg_1);" };
                        var src1 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig, body);
                        return new GenericShapeResolution(src1, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                    }
                    var paramSig2 = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig2,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.Reflection.Assembly::GetManifestResourceNames (stub returning null) ===
            registry.Register("System.Reflection.Assembly", "GetManifestResourceNames", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetManifestResourceNames",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.Assembly::GetModules (stub returning null) ===
            registry.Register("System.Reflection.Assembly", "GetModules", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetModules",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.Assembly::get_EntryPoint (stub returning null) ===
            registry.Register("System.Reflection.Assembly", "get_EntryPoint", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetEntryPoint",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.Assembly::GetManifestResourceStream (stub returning null) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.Assembly",
                MethodName: "GetManifestResourceStream",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.Reflection.Assembly::GetModule (stub returning null) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.Assembly",
                MethodName: "GetModule",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.Reflection.AssemblyName::get_CultureInfo (SimpleForward stub) ===
            registry.Register("System.Reflection.AssemblyName", "get_CultureInfo", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetCultureInfo",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.AssemblyName::get_Version (SimpleForward stub) ===
            registry.Register("System.Reflection.AssemblyName", "get_Version", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetVersion",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.FieldInfo::get_FieldHandle (SimpleForward stub returning 0) ===
            registry.Register("System.Reflection.FieldInfo", "get_FieldHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetFieldHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === System.Reflection.FieldInfo::get_IsLiteral (SimpleForward stub returning false/0) ===
            registry.Register("System.Reflection.FieldInfo", "get_IsLiteral", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsLiteral",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Reflection.FieldInfo::get_IsStatic (SimpleForward stub) ===
            registry.Register("System.Reflection.FieldInfo", "get_IsStatic", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsStatic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Reflection.PropertyInfo::GetIndexParameters (SimpleForward stub returning null) ===
            registry.Register("System.Reflection.PropertyInfo", "GetIndexParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetIndexParameters",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Reflection.PropertyInfo::get_CanRead (SimpleForward stub returning false/0) ===
            registry.Register("System.Reflection.PropertyInfo", "get_CanRead", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetCanRead",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Reflection.PropertyInfo::get_CanWrite (SimpleForward stub returning false/0) ===
            registry.Register("System.Reflection.PropertyInfo", "get_CanWrite", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetCanWrite",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.String::ToLower (stub returning null) ===
            registry.Register("System.String", "ToLower", [],
                ShapeKind.SimpleForward, "ChaosStringToLower",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.String::ToUpper (stub returning null) ===
            registry.Register("System.String", "ToUpper", [],
                ShapeKind.SimpleForward, "ChaosStringToUpper",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.String::Trim (stub returning null) ===
            registry.Register("System.String", "Trim", [],
                ShapeKind.SimpleForward, "ChaosStringTrim",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Net.WebUtility stubs (HtmlEncode/Decode, UrlEncode/Decode) ===
            registry.Register("System.Net.WebUtility", "HtmlEncode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityHtmlEncode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "HtmlDecode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityHtmlDecode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "UrlEncode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityUrlEncode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "UrlDecode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityUrlDecode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Globalization.CultureInfo::get_TextInfo (SimpleForward stub) ===
            registry.Register("System.Globalization.CultureInfo", "get_TextInfo", [],
                ShapeKind.SimpleForward, "ChaosCultureGetTextInfo",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Globalization.TextInfo::ToLower (SimpleForward stub) ===
            registry.Register("System.Globalization.TextInfo", "ToLower", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosTextInfoToLower",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === System.Globalization.TextInfo::ToUpper (SimpleForward stub) ===
            registry.Register("System.Globalization.TextInfo", "ToUpper", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosTextInfoToUpper",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === System.Globalization.TextInfo::get_CultureName (SimpleForward stub) ===
            registry.Register("System.Globalization.TextInfo", "get_CultureName", [],
                ShapeKind.SimpleForward, "ChaosTextInfoGetCultureName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === System.Math::BigMul (SimpleForward stub) ===
            registry.Register("System.Math", "BigMul", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathBigMul",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // === System.Math::Cos (SimpleForward stub) ===
            registry.Register("System.Math", "Cos", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathCos",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === System.Char::IsDigit (SimpleForward stub) ===
            registry.Register("System.Char", "IsDigit", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsDigit",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Char::IsLetter (SimpleForward stub) ===
            registry.Register("System.Char", "IsLetter", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsLetter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.Char::IsWhiteSpace (SimpleForward stub) ===
            registry.Register("System.Char", "IsWhiteSpace", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsWhiteSpace",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === System.TimeSpan::.ctor (SimpleForward stub) ===
            registry.Register("System.TimeSpan", ".ctor", ["System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosTimeSpanCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.TimeSpan", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            // === System.TimeSpan::FromHours (SimpleForward stub) ===
            registry.Register("System.TimeSpan", "FromHours", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromHours",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === System.TimeSpan::FromMinutes (SimpleForward stub) ===
            registry.Register("System.TimeSpan", "FromMinutes", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromMinutes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === System.DateTime::.ctor (SimpleForward stub) ===
            registry.Register("System.DateTime", ".ctor", ["System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeCtor3",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.DateTime", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.DateTime", ".ctor", ["System.Int32", "System.Int32", "System.Int32", "System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeCtor6",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[7]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.DateTime", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3, 4, 5, 6 });

            // === System.DateTime::Parse (SimpleForward stub) ===
            registry.Register("System.DateTime", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosDateTimeParse",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0 });

            // === System.TimeSpan::Parse (SimpleForward stub) ===
            registry.Register("System.TimeSpan", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTimeSpanParse",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.TimeSpan" },
                new HashSet<int> { 0 });

            // === System.BitConverter::GetBytes (GenericShapeDescriptor -- stub returning null, real for Int32) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.BitConverter",
                MethodName: "GetBytes",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Int32 overload — delegate to ChaosBitConverterGetBytes
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.Int32")
                    {
                        var srcBytes = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    return ChaosBitConverterGetBytes(0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));",
                        ]);
                        return new GenericShapeResolution(srcBytes, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateInt32AbiSlot()),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>();
                    foreach (var pt in paramTypes)
                    {
                        if (pt.Contains("Int64") || pt.Contains("Double") || pt.Contains("Single"))
                            abiSlots.Add(new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType });
                        else
                            abiSlots.Add(CreateInt32AbiSlot());
                    }
                    if (abiSlots.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === System.BitConverter::ToInt32 (SimpleForward → ChaosBitConverterToInt32) ===
            registry.Register("System.BitConverter", "ToInt32", ["System.Byte[]", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitConverterToInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // === System.BitConverter::ToDouble (SimpleForward → ChaosBitConverterToDouble) ===
            registry.Register("System.BitConverter", "ToDouble", ["System.Byte[]", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitConverterToDouble",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64 },
                new HashSet<int> { 0, 1 });


            // === Marshal.GetFunctionPointerForDelegate<T>(T) — returns native function pointer via thunk registry ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetFunctionPointerForDelegate",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 1) return null;
                    var delegateTypeId = typeArgs[0];
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* rs = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    auto* ts = chaos::il2cpp::runtime_core::GetCurrentThreadState();",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(",
                        "        chaos::il2cpp::runtime_core::MarshalGetFunctionPointerForDelegateImpl(",
                        "            rs, ts, chaos_arg_0, \"" + delegateTypeId + "\"));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.GetDelegateForFunctionPointer<T>(IntPtr) — delegates to runtime → native function dispatch thunk ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetDelegateForFunctionPointer",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 1) return null;
                    var delegateTypeId = typeArgs[0];
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Extract param_count from Func/Action generic type args at codegen time.
                    // This lets the native helper select the correct arity dispatch thunk
                    // without needing RegisterDelegateThunk to be called first.
                    int paramCount = ExtractDelegateArity(delegateTypeId);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* rs = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    auto* ts = chaos::il2cpp::runtime_core::GetCurrentThreadState();",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(",
                        "        chaos::il2cpp::runtime_core::MarshalGetDelegateForFunctionPointerImpl(",
                        "            rs, ts, chaos_arg_0, \"" + delegateTypeId + "\", " + paramCount + "));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // ── AnsiStringMarshaller stubs ──────────────────────────────────────
            // Source-generated marshalling for [DllImport] CharSet.Ansi.
            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "ConvertToUnmanaged", ["System.String"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerConvertToUnmanaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "ConvertToManaged", ["System.Byte*"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerConvertToManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "Free", ["System.Byte*"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerFree",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });
            // ── ComWrappers stubs (AOT: COM not available, return 0/null) ─────────
            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "RegisterForMarshalling", ["System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersRegisterForMarshalling",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "GetOrCreateComInterfaceForObject", ["System.Object", "System.Int32", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersGetOrCreateComInterfaceForObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "GetOrCreateObjectForComInstance", ["System.IntPtr", "System.Int32", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersGetOrCreateObjectForComInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "TryGetComInstance", ["System.Object", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersTryGetComInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "TryGetObject", ["System.IntPtr", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersTryGetObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });

            // ── NativeLibrary stubs (DirectNativeSymbol) ──────────────────────────
            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "Load", ["System.String"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryLoad",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "Free", ["System.IntPtr"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryFree",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "GetExport", ["System.IntPtr", "System.String"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryGetExport",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "GetMainProgramHandle", [],
                ShapeKind.SimpleForward, "ChaosNativeLibraryGetMainProgramHandle",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateNativeIntAbiSlot(),
                new HashSet<int>());

            // === BCL token stubs (Option C: value type as NativeInt token, primitives return 0) ===
            // These let the pipeline complete without lowering BCL methods.
            // Individual method failures are handled as runtime-semantics issues (Cat E).

            // === Guid::.ctor(String) ===
            registry.Register("System.Guid", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosGuidCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Guid::.ctor(Byte[]) ===
            registry.Register("System.Guid", ".ctor", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosGuidCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Guid", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Guid::Parse(string) ===
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

            // === Random::.ctor ===
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

            // === HashCode::ToHashCode (GenericShapeDescriptor -- handles value type) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "ToHashCode",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                        "    return ChaosHashCodeToHashCode(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.HashCode", AotCoreIrTypeShapeKind.ValueType)),
                        CreateInt32AbiSlot(), new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "Combine",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return ChaosHashCodeCombine2(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                        }),
                        CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
                }));

            // ── System.Convert stubs ───────────────────────────────────────────
            registry.Register("System.Convert", "ToBoolean", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToBoolean",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToByte", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToByte",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToInt16", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToInt16",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int16, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToInt32", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToInt64", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToSingle", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToSingle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToDouble", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToDecimal", ["System.String"],
                ShapeKind.SimpleForward, "ChaosConvertToDecimal",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToDecimal", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDecimalFromDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToInt32", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosConvertToInt32FromDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToString", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosFormatInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Convert", "ToString", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosFormatDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // ── Convert.ToXxx numeric→numeric inline shapes ──────────────────────
            // These handle integer, float, char, bool input types and emit a direct
            // static_cast at the call site, avoiding codegen fallback bugs.
            RegisterConvertNumericInline(registry, "ToUInt16", "CHAOS_IL2CPP_UINT16");
            RegisterConvertNumericInline(registry, "ToInt32", "CHAOS_IL2CPP_INT32");
            RegisterConvertNumericInline(registry, "ToUInt32", "CHAOS_IL2CPP_UINT32");
            RegisterConvertNumericInline(registry, "ToInt16", "CHAOS_IL2CPP_INT16");
            RegisterConvertNumericInline(registry, "ToSByte", "CHAOS_IL2CPP_INT8");
            RegisterConvertNumericInline(registry, "ToByte", "CHAOS_IL2CPP_UINT8");
            RegisterConvertNumericInline(registry, "ToUInt64", "CHAOS_IL2CPP_UINT64");
            RegisterConvertNumericInline(registry, "ToInt64", "CHAOS_IL2CPP_INT64");
            RegisterConvertNumericInline(registry, "ToSingle", "CHAOS_IL2CPP_FLOAT32");
            RegisterConvertNumericInline(registry, "ToDouble", "CHAOS_IL2CPP_FLOAT64");

            // ── System.Int32/Int64/Double::Parse stubs ─────────────────────────
            registry.Register("System.Int32", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosParseInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Int64", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosParseInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Double", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosParseDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // ── System.DateTime/TimeSpan (handled via SimpleForward stubs above) ──

            // ── COM RCW runtime helpers ─────────────────────────────────────────
            registry.Register("System.Runtime.InteropServices.Marshal", "CreateRcw", ["System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalCreateRcw",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshal", "ReleaseRcw", ["System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalReleaseRcw",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshal", "GetRcwUnknown", ["System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalGetRcwUnknown",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshal", "RcwQueryInterface", ["System.IntPtr", "System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalRcwQueryInterface",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.Marshal", "CallComMethod", ["System.IntPtr", "System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "MarshalCallComMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.Runtime.InteropServices.Marshal", "CallDirectComMethod", ["System.IntPtr", "System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "MarshalCallDirectComMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            // ── COM CCW runtime helpers ─────────────────────────────────────────
            registry.Register("Marshal", "CreateCcw", ["System.IntPtr", "System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalCreateCcw",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── Marshal simple ICALL stubs (compiled from runtime_stubs/interop_stubs.cpp) ──
            registry.Register("Marshal", "SetLastPInvokeError", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosMarshalSetLastPInvokeError",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("Marshal", "GetLastPInvokeError", [],
                ShapeKind.SimpleForward, "ChaosMarshalGetLastPInvokeError",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateInt32AbiSlot(),
                new HashSet<int>());

            registry.Register("Marshal", "GetExceptionCode", [],
                ShapeKind.SimpleForward, "ChaosMarshalGetExceptionCode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateInt32AbiSlot(),
                new HashSet<int>());

            registry.Register("Marshal", "GetExceptionPointers", [],
                ShapeKind.SimpleForward, "ChaosMarshalGetExceptionPointers",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateNativeIntAbiSlot(),
                new HashSet<int>());

            registry.Register("Marshal", "AreComObjectsAvailableForCleanup", [],
                ShapeKind.SimpleForward, "ChaosMarshalAreComObjectsAvailableForCleanup",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateInt32AbiSlot(),
                new HashSet<int>());

            // ── Enum runtime stubs ─────────────────────────────────────────────
            registry.Register("System.Enum", "IsDefined", ["System.Type", "System.Object"],
                ShapeKind.SimpleForward, "ChaosEnumIsDefined",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── ICustomMarshaler runtime helpers (V3) ──────────────────────────
            registry.Register("System.Runtime.InteropServices.Marshal", "CustomMarshalerNativeToManaged",
                ["System.IntPtr", "System.IntPtr"],
                ShapeKind.SimpleForward, "CustomMarshalerNativeToManaged",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.Marshal", "CustomMarshalerManagedToNative",
                ["System.IntPtr", "System.IntPtr"],
                ShapeKind.SimpleForward, "CustomMarshalerManagedToNative",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Enum", "GetName", ["System.Type", "System.Object"],
                ShapeKind.SimpleForward, "ChaosEnumGetName",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Enum", "GetNames", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosEnumGetNames",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Enum", "GetValues", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosEnumGetValues",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Enum", "Parse", ["System.Type", "System.String"],
                ShapeKind.SimpleForward, "ChaosEnumParse",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Enum", "Parse", ["System.Type", "System.String", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosEnumParseWithIgnoreCase",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Enum", "Format", ["System.Type", "System.Object", "System.String"],
                ShapeKind.SimpleForward, "ChaosEnumFormat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            // System.Enum.ToString has no declared parameters (instance method on System.Object — "this" is implicit in the ABI).
            // The codegen passes the instance as chaos_fn_arg_0 automatically.
            registry.Register("System.Enum", "ToString", [],
                ShapeKind.SimpleForward, "ChaosEnumToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Enum", "ToString", ["System.String"],
                ShapeKind.SimpleForward, "ChaosEnumToStringWithFormat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // TryParse with out Object → ByRef parameter for the result slot.
            // The ABI treats ByRef as CHAOS_IL2CPP_INTPTR (pointer to the result slot).
            registry.Register("System.Enum", "TryParse", ["System.Type", "System.String", "System.Object&"],
                ShapeKind.SimpleForward, "ChaosEnumTryParse",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Enum", "TryParse", ["System.Type", "System.String", "System.Boolean", "System.Object&"],
                ShapeKind.SimpleForward, "ChaosEnumTryParseWithIgnoreCase",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            // ── Enum InlineShapeDescriptor registrations ─────────────────────
            // InlineShapeDescriptor takes priority over GenericShapeDescriptor
            // and SimpleForward in the call-target resolution chain.  These
            // replace extern "C" function calls with C++ lambdas that use the
            // thread-local enum string cache (zero alloc on cache hit) or a
            // direct bit-test (HasFlag, no function call at all).

            // System.Enum.ToString() — instance method, 0 declared params
            // Tries the thread-local enum string cache first; on cache hit,
            // returns a previously-allocated string (zero GC allocation).
            // On cache miss, falls back to ChaosEnumToString which populates
            // the cache for subsequent calls.  Repeated ToString calls on the
            // same enum type (common in benchmarks/loops) hit the cache after
            // the first call — matching RyuJIT's cached-string behavior.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "ToString",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 0) return null;
                    return """
                        ([&]() -> CHAOS_IL2CPP_INTPTR {
                            CHAOS_IL2CPP_INT64 _v = 0;
                            std::memcpy(&_v, reinterpret_cast<const void*>({0} + 16), sizeof(_v));
                            auto _cached = lookup_cached_enum_name(_v);
                            return _cached != 0 ? _cached : ChaosEnumToString({0});
                        })()
                        """.Replace("\r\n", "\n").Trim();
                })
            { IsInstanceMethod = true });

            // System.Enum.HasFlag(System.Enum) — instance method, 1 declared param
            // Replaces function call with inline bit test: (this & flag) == flag.
            // Zero function call overhead, zero GC allocation.  The existing
            // runtime path goes through generic dispatch (no shape match at all),
            // so this is a substantial improvement over the status quo.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "HasFlag",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return """
                        ([&]() -> CHAOS_IL2CPP_INT32 {
                            CHAOS_IL2CPP_INT64 _v = 0, _f = 0;
                            std::memcpy(&_v, reinterpret_cast<const void*>({0} + 16), sizeof(_v));
                            std::memcpy(&_f, reinterpret_cast<const void*>({1} + 16), sizeof(_f));
                            return (_v & _f) == _f ? 1 : 0;
                        })()
                        """.Replace("\r\n", "\n").Trim();
                })
            { IsInstanceMethod = true });

            // ── Enum GenericShapeDescriptor registrations ──────────────────
            // DirectNativeSymbol eliminates kChaosExternalRuntimeFnTable
            // dispatch, emitting direct calls like ChaosEnumGetName(args)
            // instead of kChaosExternalRuntimeFnTable[idx](args).

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "IsDefined",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        ["    return ChaosEnumIsDefined(chaos_arg_0, chaos_arg_1);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosEnumIsDefined");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "GetName",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    // Only handle GetName(Type, Object) — GetName(RuntimeType, UInt64)
                    // uses different ABI and falls through to dispatch table.
                    if (paramTypes.Count != 2 ||
                        paramTypes[0] != "System.Type" ||
                        paramTypes[1] != "System.Object") return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        ["    return ChaosEnumGetName(chaos_arg_0, chaos_arg_1);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosEnumGetName");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "GetNames",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return ChaosEnumGetNames(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosEnumGetNames");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "GetValues",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return ChaosEnumGetValues(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosEnumGetValues");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "Parse",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            ["    return ChaosEnumParse(chaos_arg_0, chaos_arg_1);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosEnumParse");
                    }
                    if (paramTypes.Count == 3)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                            ["    return ChaosEnumParseWithIgnoreCase(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 },
                            DirectNativeSymbol: "ChaosEnumParseWithIgnoreCase");
                    }
                    return null;
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "Format",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 3) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        ["    return ChaosEnumFormat(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosEnumFormat");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                            ["    return ChaosEnumToString(chaos_arg_0);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosEnumToString");
                    }
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            ["    return ChaosEnumToStringWithFormat(chaos_arg_0, chaos_arg_1);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosEnumToStringWithFormat");
                    }
                    return null;
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Enum",
                MethodName: "TryParse",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 3)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                            ["    return ChaosEnumTryParse(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                            }), CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2 },
                            DirectNativeSymbol: "ChaosEnumTryParse");
                    }
                    if (paramTypes.Count == 4)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
                            ["    return ChaosEnumTryParseWithIgnoreCase(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);"]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                            }), CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 },
                            DirectNativeSymbol: "ChaosEnumTryParseWithIgnoreCase");
                    }
                    return null;
                }));

		            // ── Globalization stubs (DirectNativeSymbol) ─────────────────────
	            // These replace managed dispatch with direct native C function calls
	            // for commonly-called globalization methods.  ASCII-range implementations
	            // avoid the overhead of generic managed dispatch (~0.2ms → ~0.01ms).

	            // CharUnicodeInfo.GetNumericValue(char) → double
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetNumericValue", ["System.Char"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetNumericValue",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
	                new HashSet<int>());

	            // CharUnicodeInfo.GetDigitValue(char) → int
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetDigitValue", ["System.Char"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDigitValue",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // CharUnicodeInfo.GetDigitValue(string, int) → int
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetDigitValue", ["System.String", "System.Int32"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDigitValueString",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
	                {
	                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int> { 0, 1 });

	            // CharUnicodeInfo.GetNumericValue(string, int) → double
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetNumericValue", ["System.String", "System.Int32"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetNumericValueString",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
	                {
	                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
	                    CreateInt32AbiSlot(),
	                }),
	                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
	                new HashSet<int> { 0, 1 });

	            // CharUnicodeInfo.GetDecimalDigitValue(char) → int
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetDecimalDigitValue", ["System.Char"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDecimalDigitValue",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // CharUnicodeInfo.GetUnicodeCategory(char) → UnicodeCategory
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetUnicodeCategory", ["System.Char"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetUnicodeCategory",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // CharUnicodeInfo.GetUnicodeCategory(int) → UnicodeCategory
	            registry.Register("System.Globalization.CharUnicodeInfo", "GetUnicodeCategory", ["System.Int32"],
	                ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetUnicodeCategory",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // CompareInfo.IsSortable(string) → bool
	            registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.String"],
	                ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableString",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int> { 0 });

	            // CompareInfo.IsSortable(char) → bool
	            registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.Char"],
	                ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableInt",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // CompareInfo.IsSortable(System.Text.Rune) → bool
	            registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.Text.Rune"],
	                ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableInt",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateInt32AbiSlot(),
	                }),
	                CreateInt32AbiSlot(),
	                new HashSet<int>());

	            // DateTimeFormatInfo.GetInstance(IFormatProvider) → DateTimeFormatInfo
	            registry.Register("System.Globalization.DateTimeFormatInfo", "GetInstance", ["System.IFormatProvider"],
	                ShapeKind.SimpleForward, "ChaosDateTimeFormatInfoGetInstance",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.DateTimeFormatInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // CultureInfo.GetCultureInfo(string) → CultureInfo  (static)
	            registry.Register("System.Globalization.CultureInfo", "GetCultureInfo", ["System.String"],
	                ShapeKind.SimpleForward, "ChaosCultureGetCultureInfo",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // CultureInfo.GetCultureInfo(string, bool) → CultureInfo (static)
	            registry.Register("System.Globalization.CultureInfo", "GetCultureInfo", ["System.String", "System.Boolean"],
	                ShapeKind.SimpleForward, "ChaosCultureGetCultureInfoBool",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                    CreateInt32AbiSlot(),  // bool → Int32 in unmanaged ABI
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // CultureInfo.GetCultureInfoByIetfLanguageTag(string) → CultureInfo (static)
	            registry.Register("System.Globalization.CultureInfo", "GetCultureInfoByIetfLanguageTag", ["System.String"],
	                ShapeKind.SimpleForward, "ChaosCultureGetCultureInfoByIetfLanguageTag",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // CultureInfo.CreateSpecificCulture(string) → CultureInfo (static)
	            registry.Register("System.Globalization.CultureInfo", "CreateSpecificCulture", ["System.String"],
	                ShapeKind.SimpleForward, "ChaosCultureCreateSpecificCulture",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // CompareInfo.GetCompareInfo(string) → CompareInfo (static)
	            registry.Register("System.Globalization.CompareInfo", "GetCompareInfo", ["System.String"],
	                ShapeKind.SimpleForward, "ChaosCompareInfoGetCompareInfo",
	                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
	                {
	                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
	                }),
	                CreateNativeIntAbiSlot("System.Globalization.CompareInfo", AotCoreIrTypeShapeKind.ReferenceType),
	                new HashSet<int> { 0 });

	            // ── Chaos.TestFramework.Assert inline shapes ──────────────────────
	            // These inline expansions replace Assert.AreEqual/IsTrue/IsNull etc.
	            // calls with C++ code that checks the condition and throws a managed
	            // exception on failure.  The exception is caught by the dispatch
	            // wrapper's catch(chaos_managed_exception&) block.
	            //
	            // The C# codegen always emits these assertion bodies.  The calling
	            // test code simply does not invoke Assert methods in non-verification
	            // builds, so the assertion code is dead-stripped by the C++ linker.

	            // Assert.AreEqual(expected, actual) — all overloads
	            // For byte[] arrays, uses element-by-element memcmp instead of pointer
	            // comparison, since different array allocations are never pointer-equal.
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "AreEqual",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    if (paramTypes.Count < 2) return null;

	                    // byte[]: structural comparison via memcmp
	                    if (paramTypes[0] == "System.Byte[]" && paramTypes[1] == "System.Byte[]")
	                    {
	                        return """
	                            [&]() -> void {
	                                bool _cae_eq;
	                                if (({0}) == ({1})) _cae_eq = true;
	                                else if (({0}) == 0 || ({1}) == 0) _cae_eq = false;
	                                else {
	                                    auto _cae_l0 = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
	                                        reinterpret_cast<const uint8_t*>({0}) + 24);
	                                    auto _cae_l1 = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
	                                        reinterpret_cast<const uint8_t*>({1}) + 24);
	                                    if (_cae_l0 != _cae_l1) _cae_eq = false;
	                                    else {
	                                        _cae_eq = std::memcmp(
	                                            reinterpret_cast<const void*>(reinterpret_cast<const uint8_t*>({0}) + 32),
	                                            reinterpret_cast<const void*>(reinterpret_cast<const uint8_t*>({1}) + 32),
	                                            static_cast<size_t>(_cae_l0)) == 0;
	                                    }
	                                }
	                                if (!_cae_eq) throw chaos_managed_exception{};
	                            }()
	                            """.Replace("\r\n", "\n").Trim();
	                    }

	                    return """
	                        [&]() -> void { if (({0}) != ({1})) { throw chaos_managed_exception{}; } }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

	            // Assert.IsTrue(condition)
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "IsTrue",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    if (paramTypes.Count < 1) return null;
	                    return """
	                        [&]() -> void { if (!({0})) { throw chaos_managed_exception{}; } }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

	            // Assert.IsFalse(condition)
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "IsFalse",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    if (paramTypes.Count < 1) return null;
	                    return """
	                        [&]() -> void { if ({0}) { throw chaos_managed_exception{}; } }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

	            // Assert.IsNull(value)
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "IsNull",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    if (paramTypes.Count < 1) return null;
	                    return """
	                        [&]() -> void { if (({0}) != 0) { throw chaos_managed_exception{}; } }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

	            // Assert.IsNotNull(value)
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "IsNotNull",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    if (paramTypes.Count < 1) return null;
	                    return """
	                        [&]() -> void { if (({0}) == 0) { throw chaos_managed_exception{}; } }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

	            // Assert.Fail(message) — always throws
	            registry.RegisterInline(new InlineShapeDescriptor(
	                TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
	                MethodName: "Fail",
	                Resolver: static (callee, paramTypes) =>
	                {
	                    return """
	                        [&]() -> void { throw chaos_managed_exception{}; }()
	                        """.Replace("\r\n", "\n").Trim();
	                }));

            // Assert.Throws<T>(Action) — deferred to follow-up implementation.
            // Requires recognizing the callvirt Invoke delegate pattern and
            // emitting a direct try/catch with type check.

            return;
        }

    }
}

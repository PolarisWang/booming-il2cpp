"""Add ComWrappers and NativeLibrary RHS registrations"""
import sys
sys.dont_write_bytecode = True

with open(r'src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs',
          'r', encoding='utf-8') as f:
    content = f.read()

insert = r'''
            // === ComWrappers stubs (return 0 — COM not available in AOT) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.ComWrappers",
                MethodName: "RegisterForMarshalling",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    ["    return ChaosComWrappersRegisterForMarshalling(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(), new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosComWrappersRegisterForMarshalling");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.ComWrappers",
                MethodName: "GetOrCreateComInterfaceForObject",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    ["    return ChaosComWrappersGetOrCreateComInterfaceForObject(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateInt32AbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosComWrappersGetOrCreateComInterfaceForObject");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.ComWrappers",
                MethodName: "GetOrCreateObjectForComInstance",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    ["    return ChaosComWrappersGetOrCreateObjectForComInstance(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateInt32AbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosComWrappersGetOrCreateObjectForComInstance");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.ComWrappers",
                MethodName: "TryGetComInstance",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    ["    return ChaosComWrappersTryGetComInstance(chaos_arg_0, chaos_arg_1);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateInt32AbiSlot(), new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosComWrappersTryGetComInstance");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.ComWrappers",
                MethodName: "TryGetObject",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    ["    return ChaosComWrappersTryGetObject(chaos_arg_0, chaos_arg_1);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateInt32AbiSlot(), new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosComWrappersTryGetObject");
                }));

            // === NativeLibrary stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeLibrary",
                MethodName: "Load",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    ["    return ChaosNativeLibraryLoad(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(), new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeLibraryLoad");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeLibrary",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    ["    return ChaosNativeLibraryFree(chaos_arg_0);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateInt32AbiSlot(), new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeLibraryFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeLibrary",
                MethodName: "GetExport",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    ["    return ChaosNativeLibraryGetExport(chaos_arg_0, chaos_arg_1);"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType) }),
                        CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeLibraryGetExport");
                }))

'''

anchor = '// === String.Join (IEnumerable<T>'
if anchor in content:
    idx = content.index(anchor)
    content = content[:idx] + insert + content[idx:]
    with open(r'src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs',
              'w', encoding='utf-8') as f:
        f.write(content)
    print('ComWrappers/NativeLibrary RHS stubs added successfully')
else:
    print('Anchor not found')
    sys.exit(1)

"""Add NativeMemory and SafeBuffer RHS registrations to RuntimeHelperShapeRegistry.cs"""
import re

with open(r'src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs',
          'r', encoding='utf-8') as f:
    content = f.read()

native_stubs = r'''
            // === NativeMemory stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Alloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosNativeMemoryAlloc(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Realloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryRealloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        CreateAbiSlots(CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryRealloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedAlloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryAlignedAlloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        CreateAbiSlots(CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedFree",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryAlignedFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedRealloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    return ChaosNativeMemoryAlignedRealloc(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        CreateAbiSlots(CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedRealloc");
                }));

            // === SafeBuffer stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "get_ByteLength",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosSafeBufferGetByteLength(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosSafeBufferGetByteLength");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Read",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_UINT8", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosSafeBufferReadByte(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        CreateAbiSlots(CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosSafeBufferReadByte");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Write",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_UINT8 chaos_arg_2",
                    [
                        "    ChaosSafeBufferWriteByte(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        CreateAbiSlots(CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateInt32AbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosSafeBufferWriteByte");
                }));

'''

anchor = '// === String.Join (IEnumerable<T>'
if anchor in content:
    idx = content.index(anchor)
    content = content[:idx] + native_stubs + content[idx:]
    with open(r'src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs',
              'w', encoding='utf-8') as f:
        f.write(content)
    print('NativeMemory/SafeBuffer RHS stubs added successfully')
else:
    print('Anchor not found')
    # Try finding the GCHandle section to insert after
    anchor2 = 'DirectNativeSymbol: "ChaosMarshalGetHRForException");'
    if anchor2 in content:
        idx = content.index(anchor2) + len(anchor2)
        content = content[:idx] + '\n' + native_stubs + content[idx:]
        with open(r'src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs',
                  'w', encoding='utf-8') as f:
            f.write(content)
        print('NativeMemory/SafeBuffer RHS stubs added after Marshal HR section')
    else:
        print('Neither anchor found')

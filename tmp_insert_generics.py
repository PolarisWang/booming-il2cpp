"""Insert String.Join and Marshal.Copy GenericShapeDescriptor entries."""

with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/RuntimeHelperShapeRegistry.cs', 'r', encoding='utf-8') as f:
    content = f.read()

# ===== Insert String.Join GenericShapeDescriptor after String.Contains (after line ~573) =====
marker_contains_end = '''                        new HashSet<int> { 0, 1 });
                }));

            // === Exception operations ==='''

string_join_block = '''                        new HashSet<int> { 0, 1 });
                }));

            // === String.Join (IEnumerable<T> — resolves variants at planning time) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Join",
                Resolver: (callee, typeArgs) =>
                {
                    if (!TryGetStringJoinEnumerableElementType(callee, out var elementType))
                        return null;
                    var variants = planner.ResolveEnumerableJoinSupportVariants(elementType!);
                    if (variants.Count == 0) return null;
                    if (string.Equals(elementType, "System.Int32", StringComparison.Ordinal) ||
                        string.Equals(elementType, "System.Int64", StringComparison.Ordinal) ||
                        string.Equals(elementType, "System.Int16", StringComparison.Ordinal))
                    {
                        return planner.CreateStringJoinInt32EnumerableRuntimeHelperDefinition(callee, variants);
                    }
                    if (string.Equals(elementType, "System.String", StringComparison.Ordinal))
                    {
                        return planner.CreateStringJoinStringEnumerableRuntimeHelperDefinition(callee, variants);
                    }
                    return null;
                }));

            // === Exception operations ==='''

assert marker_contains_end in content, 'marker not found'
content = content.replace(marker_contains_end, string_join_block, 1)

# ===== Insert Marshal.Copy GenericShapeDescriptor after Marshal non-generic SimpleForward entries =====
marker_marshal_end = '''                EmptyRawArgumentIndices);

            // === Numeric formatting (GenericShapeDescriptor to match TryCreate behavior) ==='''

marshal_copy_block = '''                EmptyRawArgumentIndices);

            // === Marshal.Copy (element type and direction extracted from parameter types) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "Copy",
                Resolver: (callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 4) return null;

                    bool isArrayToPtr;
                    string elementTypeName;
                    if (paramTypes[0].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = true;
                        elementTypeName = paramTypes[0].Substring(0, paramTypes[0].Length - 2);
                    }
                    else if (string.Equals(paramTypes[0], "System.IntPtr", StringComparison.Ordinal) &&
                             paramTypes[1].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = false;
                        elementTypeName = paramTypes[1].Substring(0, paramTypes[1].Length - 2);
                    }
                    else
                    {
                        return null;
                    }

                    if (!MarshalCopyElementTypeMap.TryGetValue(elementTypeName, out var cppElementType))
                        return null;

                    var symbol = GetExternalRuntimeHelperSymbol(callee);

                    if (isArrayToPtr)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_0);",
                            "    auto chaos_start_index = chaos_arg_1;",
                            "    auto chaos_dest = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyArrayToPtr<" + cppElementType + ">(chaos_array, chaos_start_index, chaos_dest, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                    else
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto chaos_source = chaos_arg_0;",
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_1);",
                            "    auto chaos_start_index = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyPtrToArray<" + cppElementType + ">(chaos_source, chaos_array, chaos_start_index, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                }));

            // === Numeric formatting (GenericShapeDescriptor to match TryCreate behavior) ==='''

assert marker_marshal_end in content, 'marshal end marker not found'
content = content.replace(marker_marshal_end, marshal_copy_block, 1)

with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/RuntimeHelperShapeRegistry.cs', 'w', encoding='utf-8') as f:
    f.write(content)

print('Done - inserted String.Join and Marshal.Copy GenericShapeDescriptor entries')

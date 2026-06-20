using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Nullable<T>.get_HasValue — reads the hasValue field at offset sizeof(ThinLockableHeader)
        /// </summary>
        private static void RegisterNullableTget(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "get_HasValue",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        // get_HasValue() instance method: hasValue is stored as int32 at the start of the value struct
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    if (chaos_arg_0 == 0) return 0;",
                            "    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        "    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Nullable<T>.GetValueOrDefault() — reads the value field after ThinLockableHeader+hasValue
        /// </summary>
        private static void RegisterNullableTGetValueOrDefault(RuntimeHelperShapeRegistry registry)
        {
            // Overloads: GetValueOrDefault() and GetValueOrDefault(T)
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "GetValueOrDefault",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    bool hasDefaultArg = paramTypes.Count > 0;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var returnAbi = CreateNativeIntAbiSlot();
                    var retType = "CHAOS_IL2CPP_INTPTR";
                    List<AotCoreIrAbiSlotArtifact> paramAbis = new()
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)
                    };
                    string paramSig = "CHAOS_IL2CPP_INTPTR chaos_arg_0";
                    var rawIndices = new HashSet<int> { 0 };
                    var bodyLines = new List<string>
                    {
                        "    // nullable struct layout: ThinLockableHeader(16B) | hasValue(int32, 4B) | value(T)",
                        "    if (chaos_arg_0 == 0) return 0;",
                        "    auto* chaos_has_value = reinterpret_cast<CHAOS_IL2CPP_INT32*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader));",
                    };
                    if (hasDefaultArg)
                    {
                        paramAbis.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType));
                        paramSig += ", CHAOS_IL2CPP_INTPTR chaos_arg_1";
                        rawIndices.Add(1);
                        bodyLines.Add("    if (*chaos_has_value == 0) return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_1);");
                    }
                    if (typeArgs != null && typeArgs.Count > 0 && typeArgs[0] == "System.Int32")
                    {
                        retType = "CHAOS_IL2CPP_INT32";
                        returnAbi = CreateInt32AbiSlot();
                        bodyLines.Add("    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader) + 4);");
                    }
                    else
                    {
                        bodyLines.Add("    return *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader) + 4);");
                    }
                    var src = RenderSimpleExternalRuntimeHelper(retType, symbol,
                        paramSig,
                        bodyLines.ToArray());
                    return new GenericShapeResolution(src, symbol,
                        paramAbis,
                        returnAbi,
                        rawIndices);
                }));

        }

        /// <summary>
        /// Convert.ToChar (GenericShapeDescriptor — native bridge to convert.cpp)
        /// </summary>
        private static void RegisterConvertToChar(RuntimeHelperShapeRegistry registry)
        {
            // All overloads delegate to chaos_convert_tochar_* functions in convert.cpp.
            // This eliminates 200+ lines of duplicate StringId/exception body code per overload.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // Guard: only match methods named exactly "ToChar"
                    var resolverMethodName = GetMethodNameFromSubjectId(callee);
                    if (!string.Equals(resolverMethodName, "ToChar", StringComparison.Ordinal))
                        return null;
                    {
                        System.Console.Error.WriteLine($"[TMPDEBUG] ConvertToChar REJECTED methodName={resolverMethodName} callee={callee}");
                        return null;
                    }
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var retAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16, TypeShape = AotCoreIrTypeShapeKind.ValueType };

                    // Map parameter types to native chaos_convert_tochar_* symbols
                    string nativeFn = paramTypes.Count switch
                    {
                        0 => "chaos_convert_tochar_int32",  // no-arg → return 0
                        1 => GetToCharNativeSymbol(paramTypes[0]),
                        2 when paramTypes[0] == "System.Object" => "chaos_convert_tochar_object_provider",
                        2 when paramTypes[0] == "System.String" => "chaos_convert_tochar_string_provider",
                        _ => "chaos_convert_tochar_int32",
                    };

                    // Generate thin forwarding body that calls the native function.
                    // For String overloads: emit inline ResolveWithGlobalCache to eliminate
                    // the extern "C" call barrier and TLS indirect addressing.
                    var args = abiSlots.Count == 0 ? "" :
                        string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"chaos_arg_{i}"));
                    string[] bodyLines;
                    if (abiSlots.Count == 0)
                    {
                        bodyLines = ["    return static_cast<CHAOS_IL2CPP_UINT16>(0);"];
                    }
                    else if (abiSlots.Count == 1 && paramTypes[0] == "System.String")
                    {
                        bodyLines =
                        [
                            "    auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id(chaos_arg_0));",
                            "    if (_v.byte_count == 0)",
                            "    {",
                            "        chaos::il2cpp::runtime_core::chaos_raise_exception(0);",
                            "        return 0;",
                            "    }",
                            "    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));",
                        ];
                    }
                    else if (abiSlots.Count == 2 && paramTypes[0] == "System.String" && paramTypes[1] == "System.IFormatProvider")
                    {
                        bodyLines =
                        [
                            "    (void)chaos_arg_1;",
                            "    auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id(chaos_arg_0));",
                            "    if (_v.byte_count == 0)",
                            "    {",
                            "        chaos::il2cpp::runtime_core::chaos_raise_exception(0);",
                            "        return 0;",
                            "    }",
                            "    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));",
                        ];
                    }
                    else
                    {
                        bodyLines = [$"    return {nativeFn}({args});"];
                    }
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_UINT16", symbol, paramSig, bodyLines);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        retAbi,
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)),
                        DirectNativeSymbol: nativeFn,
                        DirectNativeHeader: "\"convert.h\"");
                }));

        }

        /// <summary>
        /// Convert.ToChar — inline shapes for value-type overloads
        /// </summary>
        private static void RegisterConvertToCharinline(RuntimeHelperShapeRegistry registry)
        {
            // These emit a direct static_cast at the call site, eliminating
            // external runtime helper function call overhead (dispatch table
            // lookup + prolog/epilog).  Managed JIT inlines these to a single
            // mov instruction — this makes native AOT match that perf.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var firstParam = paramTypes[0];
                    if (firstParam is "System.Byte" or "System.SByte" or "System.Int16" or "System.UInt16"
                        or "System.Int32" or "System.UInt32" or "System.Int64" or "System.UInt64"
                        or "System.Char")
                    {
                        // Direct truncation cast — matches JIT inlining for (char)intValue
                        return "static_cast<CHAOS_IL2CPP_UINT16>({0})";
                    }
                    return null;
                }));

        }

        /// <summary>
        /// Convert.ToChar — always-throw overloads (no possible valid conversion)
        /// </summary>
        private static void RegisterConvertToCharalways(RuntimeHelperShapeRegistry registry)
        {
            // These emit throw chaos_managed_exception{0} directly at the call site,
            // eliminating the extern "C" bridge function call, 3 TLS reads, and
            // ResolveTypeByName module iteration. The arg is consumed from eval stack
            // but unused in the template (the throw terminates execution).
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var firstParam = paramTypes[0];
                    if (firstParam is "System.Boolean" or "System.DateTime")
                    {
                        // Comma operator: throw terminates execution, second operand
                        // provides the result type for the caller's eval stack.
                        // The raw argument value ({0}) is NOT a valid managed object pointer —
                        // e.g., true → 1, DateTime → 64-bit ticks. Passing it as the exception
                        // object causes the catch handler to crash when it tries to dereference
                        // the "header" at that invalid address to check the exception type.
                        // Use nullptr instead: the catch handler skips type checking when the
                        // header is null, which is correct because this inline only applies to
                        // methods that always throw InvalidCastException.
                        return "(chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<CHAOS_IL2CPP_UINT16>(0))";
                    }
                    return null;
                }));

        }

    }
}

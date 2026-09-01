using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Common framework method stubs
        /// </summary>
        private static void RegisterCommonframeworkmethod(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.Unsafe",
                MethodName: "As",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Unsafe.As<U>(ref T source) → reinterpret the same address as U.
                    // Semantically a no-op pointer reinterpretation: the source ref's
                    // address is passed through unchanged.  A2-1 (cross-platform-unify):
                    // replaces the previous CHAOS_IL2CPP_FAIL() stub which forced every
                    // Unsafe.As call to the fallback return-0 (A1 gap: UnsafeTests::As*).
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    return chaos_arg_0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] { CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType) },
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType), EmptyRawArgumentIndices);
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.Unsafe",
                MethodName: "SkipInit",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Unsafe.SkipInit<T>(ref T value) — no-op: leave ref uninitialized.
                    // Accept the ref parameter as CHAOS_IL2CPP_INTPTR (by-ref pointer)
                    // to match the call site which passes one argument via ABI slot.
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR", ["    return 0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                        new HashSet<int> { 0 });
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.ArgumentNullException",
                MethodName: "ThrowIfNull",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        ["    (void)chaos_arg_0; (void)chaos_arg_1;", "    CHAOS_IL2CPP_FAIL();"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                        },
                        CreateVoidAbiSlot(), EmptyRawArgumentIndices);
                }));

        }

        /// <summary>
        /// Environment
        /// </summary>
        private static void RegisterEnvironment(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Environment", "get_CurrentManagedThreadId", [],
                ShapeKind.SimpleForward, "chaos_current_managed_thread_id",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// System.Runtime.InteropServices.RuntimeEnvironment stubs
        /// </summary>
        private static void RegisterRuntimeInteropEnvironmentstubs(RuntimeHelperShapeRegistry registry)
        {
            // RuntimeEnvironment.GetRuntimeDirectory() → string.
            // NOTE: the callee is normalized by ManagedNaming.NormalizeSubjectIdAssembly to
            // System.Private.CoreLib/RuntimeEnvironment, and GetTypeDisplayNameFromSubjectId
            // strips the assembly prefix → typeDisplayName == "RuntimeEnvironment".
            registry.Register("RuntimeEnvironment", "GetRuntimeDirectory", [],
                ShapeKind.SimpleForward, "ChaosRuntimeEnvironmentGetRuntimeDirectory",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // RuntimeEnvironment.FromGlobalAccessCache(Assembly) → bool
            registry.Register("RuntimeEnvironment", "FromGlobalAccessCache",
                ["System.Reflection.Assembly"],
                ShapeKind.SimpleForward, "ChaosRuntimeEnvironmentFromGlobalAccessCache",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // RuntimeEnvironment.GetRuntimeInterfaceAsIntPtr(Guid, Guid) → IntPtr
            registry.Register("RuntimeEnvironment", "GetRuntimeInterfaceAsIntPtr",
                ["System.Guid", "System.Guid"],
                ShapeKind.SimpleForward, "ChaosRuntimeEnvironmentGetRuntimeInterfaceAsIntPtr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(), // Guid struct on stack - IntPtr slot
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // RuntimeEnvironment.GetSystemVersion() → string
            registry.Register("RuntimeEnvironment", "GetSystemVersion", [],
                ShapeKind.SimpleForward, "ChaosRuntimeEnvironmentGetSystemVersion",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // RuntimeEnvironment.GetRuntimeInterfaceAsObject(Guid, Guid) → object
            registry.Register("RuntimeEnvironment", "GetRuntimeInterfaceAsObject",
                ["System.Guid", "System.Guid"],
                ShapeKind.SimpleForward, "ChaosRuntimeEnvironmentGetRuntimeInterfaceAsObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });
        }

        /// <summary>
        /// Console (stubs for verification pipelines — tests track via ChaosAssertState.ExitCode)
        /// </summary>
        private static void RegisterConsole(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Console", "get_Error", [],
                ShapeKind.SimpleForward, "ChaosConsoleGetError",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Console",
                MethodName: "WriteLine",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        // WriteLine() — 0-arg static
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol, "",
                        [
                            "",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateVoidAbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    // WriteLine(string) — 1-arg static: print to stdout
                    var src1 = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) { std::fputs(\"[null]\\n\", stdout); return; }",
                        "    const void* str_arg = reinterpret_cast<const void*>(chaos_arg_0);",
                        "    if (!chaos_is_string_id(chaos_arg_0))",
                        "    {",
                        "        auto data = stub_string_data(str_arg);",
                        "        if (data != nullptr) { std::fputs(data, stdout); std::fputc('\\n', stdout); }",
                        "        return;",
                        "    }",
                        "    auto view = string_table::Resolve(chaos_extract_string_id(chaos_arg_0));",
                        "    if (view.utf8_data != nullptr)",
                        "    {",
                        "        std::fwrite(view.utf8_data, sizeof(char), view.byte_count, stdout);",
                        "        std::fputc('\\n', stdout);",
                        "    }",
                    ]);
                    return new GenericShapeResolution(src1, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// OperatingSystem platform checks (dispatch via GenericShapeDescriptors below)
        /// </summary>
        private static void RegisterOperatingSystemplatformchecks(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsWindows",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(_WIN32)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsLinux",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(__linux__)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsMacOS",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(__APPLE__)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

        }

        /// <summary>
        /// Numeric formatting
        /// </summary>
        private static void RegisterNumericformatting(RuntimeHelperShapeRegistry registry)
        {
            // Int32.ToString() fully handled by GenericShapeDescriptor below (bypass Intern, direct GC alloc)
            // NOTE: The inline GcAllocateAtomic may trigger nursery GC. Conservative stack scanning
            // updates chaos_locals[] on the stack but NOT CPU registers, so callers that have cached
            // a GC-tracked local in a register across the call boundary will read a stale pointer.
            // GC-safe reloads are added at call sites that cross a GC safepoint.

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Int32",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
                        "    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot);",
                        "    // Compute digit count via branch chain (no generic formatting library).",
                        "    auto chaos_tmp = static_cast<CHAOS_IL2CPP_UINT32>(chaos_value);",
                        "    CHAOS_IL2CPP_SIZE chaos_len = 1;",
                        "    if (chaos_tmp >= 10000) { chaos_len = 5; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 1000) { chaos_len = 4; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 100)  { chaos_len = 3; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 10)  { chaos_len = 2; }",
                        "chaos_alloc:",
                        "    auto* chaos_raw = static_cast<char*>(",
                        "        chaos::il2cpp::runtime_core::GcAllocateAtomicFastNoZero(",
                        "            sizeof(chaos_type_System_Private_CoreLib_System_String) + chaos_len + 1));",
                        "    auto* chaos_str = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_raw);",
                        "    chaos_str->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();",
                        "    chaos_str->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_len);",
                        "    chaos_str->utf8_data = chaos_raw + sizeof(chaos_type_System_Private_CoreLib_System_String);",
                        "    chaos_str->string_id = 0;",
                        "    // Format digits directly into utf8_data (backward fill).",
                        "    auto* chaos_p = const_cast<char*>(chaos_str->utf8_data + chaos_len);",
                        "    *chaos_p = '\\0';",
                        "    do {",
                        "        *--chaos_p = static_cast<char>('0' + (chaos_tmp % 10));",
                        "        chaos_tmp /= 10;",
                        "    } while (chaos_tmp != 0);",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_str);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Single",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    const char* chaos_format_utf8 = nullptr;",
                        "    if (chaos_arg_1 != 0)",
                        "    {",
                        "        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_float(ChaosLoadFloat32(chaos_arg_0));",
                        "    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
                        "        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
                        "    return chaos_make_string_id_value(chaos_id);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Double",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    const char* chaos_format_utf8 = nullptr;",
                        "    if (chaos_arg_1 != 0)",
                        "    {",
                        "        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_double(ChaosLoadFloat64(chaos_arg_0));",
                        "    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
                        "        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
                        "    return chaos_make_string_id_value(chaos_id);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

        }

    }
}

using System.Diagnostics.CodeAnalysis;
using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Single source of truth for runtime helper shape registration, matching, and C++ header generation.
    /// Shapes are identified by canonical key (TypeDisplayName::MethodName(Param1,Param2,...))
    /// and matched via FNV-1a 32-bit hash. Assembly prefix is not part of shape identity.
    /// </summary>
    internal sealed partial class RuntimeHelperShapeRegistry
    {
        public enum ShapeKind : byte
        {
            SimpleForward = 0,
            InlineBody = 1,
        }

        public sealed record ShapeEntry(
            string TypeDisplayName,
            string MethodName,
            IReadOnlyList<string> ParamTypeDisplayNames,
            ShapeKind Kind,
            string NativeFnSymbol,
            IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
            AotCoreIrAbiSlotArtifact ReturnAbi,
            IReadOnlySet<int>? RawArgumentIndices = null,
            IReadOnlySet<string>? ReferencedStaticFieldSubjectIds = null)
        {
            public string CanonicalKey => BuildCanonicalKey(TypeDisplayName, MethodName, ParamTypeDisplayNames);

            public uint ShapeId => Fnv1aHash(CanonicalKey);

            public string EnumName => BuildShapeEnumName(TypeDisplayName, MethodName, ParamTypeDisplayNames);
        }

        /// <summary>Result of a GenericShapeDescriptor resolver lambda.</summary>
        public sealed record GenericShapeResolution(
            string CppSource,
            string Symbol,
            IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
            AotCoreIrAbiSlotArtifact ReturnAbi,
            IReadOnlySet<int> RawArgumentIndices,
            IReadOnlySet<string>? ReferencedStaticFieldSubjectIds = null,
            string? DirectNativeSymbol = null,
            string? DirectNativeHeader = null);

        /// <summary>Descriptor for a shape matched by type+method prefix with a resolver lambda.</summary>
        public sealed record GenericShapeDescriptor(
            string TypeDisplayNamePrefix,
            string MethodName,
            Func<NativeAotLoweringPlanner, string, IReadOnlyList<string>, GenericShapeResolution?> Resolver);

        /// <summary>Descriptor for an inline shape — generates a C++ expression template at call site instead of an external function call.</summary>
        public sealed record InlineShapeDescriptor(
            string TypeDisplayNamePrefix,
            string MethodName,
            Func<string, IReadOnlyList<string>, string?> Resolver)
        {
            /// <summary>True if this method has an implicit `this` parameter on the eval stack (instance method call via callvirt/call).</summary>
            public bool IsInstanceMethod { get; init; }
        }

        private readonly Dictionary<uint, ShapeEntry> _entriesByShapeId = new();
        private readonly Dictionary<string, ShapeEntry> _entriesByCanonicalKey = new(StringComparer.Ordinal);
        private readonly List<GenericShapeDescriptor> _genericDescriptors = new();
        private readonly List<InlineShapeDescriptor> _inlineDescriptors = new();

        /// <summary>FNV-1a 32-bit hash — must match the C++ constexpr implementation exactly.</summary>
        public static uint Fnv1aHash(string text)
        {
            uint hash = 2166136261;
            foreach (var ch in text)
            {
                hash ^= (byte)(ch & 0xFF);
                hash *= 16777619;
            }

            return hash;
        }

        /// <summary>Build canonical shape key: "TypeDisplayName::MethodName(Param1,Param2,...)".</summary>
        public static string BuildCanonicalKey(
            string typeDisplayName,
            string methodName,
            IReadOnlyList<string> paramTypeDisplayNames)
        {
            var sb = new StringBuilder(typeDisplayName.Length + methodName.Length + paramTypeDisplayNames.Sum(p => p.Length) + 4);
            sb.Append(typeDisplayName);
            sb.Append("::");
            sb.Append(methodName);
            sb.Append('(');
            for (var i = 0; i < paramTypeDisplayNames.Count; i++)
            {
                if (i > 0)
                {
                    sb.Append(',');
                }

                sb.Append(paramTypeDisplayNames[i]);
            }

            sb.Append(')');
            return sb.ToString();
        }

        /// <summary>Build a C++-safe enum name from a shape's type/method/params.</summary>
        public static string BuildShapeEnumName(
            string typeDisplayName,
            string methodName,
            IReadOnlyList<string> paramTypeDisplayNames)
        {
            var sb = new StringBuilder();
            sb.Append("SHAPE_");
            sb.Append(SanitizeForEnumName(typeDisplayName));
            sb.Append('_');
            sb.Append(SanitizeForEnumName(methodName));

            if (paramTypeDisplayNames.Count > 0)
            {
                sb.Append('_');
                for (var i = 0; i < paramTypeDisplayNames.Count; i++)
                {
                    if (i > 0) sb.Append('_');
                    sb.Append(SanitizeForEnumName(paramTypeDisplayNames[i]));
                }
            }

            return sb.ToString();
        }

        /// <summary>Register a runtime helper shape.</summary>
        public ShapeEntry Register(
            string typeDisplayName,
            string methodName,
            IReadOnlyList<string> paramTypeDisplayNames,
            ShapeKind kind,
            string nativeFnSymbol,
            IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis,
            AotCoreIrAbiSlotArtifact returnAbi,
            IReadOnlySet<int>? rawArgumentIndices = null,
            IReadOnlySet<string>? referencedStaticFieldSubjectIds = null)
        {
            var entry = new ShapeEntry(
                typeDisplayName,
                methodName,
                paramTypeDisplayNames,
                kind,
                nativeFnSymbol,
                parameterAbis,
                returnAbi,
                rawArgumentIndices,
                referencedStaticFieldSubjectIds);

            var canonicalKey = entry.CanonicalKey;
            if (_entriesByCanonicalKey.ContainsKey(canonicalKey))
            {
                throw new InvalidOperationException(
                    $"Shape already registered: '{canonicalKey}'");
            }

            _entriesByCanonicalKey[canonicalKey] = entry;
            _entriesByShapeId[entry.ShapeId] = entry;
            return entry;
        }

        /// <summary>Register a generic shape descriptor for planning-time resolution.</summary>
        public void RegisterGeneric(GenericShapeDescriptor descriptor)
        {
            _genericDescriptors.Add(descriptor);
        }

        /// <summary>Register an inline shape descriptor for call-site expression expansion.</summary>
        public void RegisterInline(InlineShapeDescriptor descriptor)
        {
            _inlineDescriptors.Add(descriptor);
        }

        /// <summary>Try to match a callee SubjectId to an inline shape descriptor.</summary>
        public bool TryMatchInlineShape(
            string callee,
            [NotNullWhen(true)] out string? cppExpression,
            [NotNullWhen(true)] out InlineShapeDescriptor? matchedDescriptor)
        {
            cppExpression = null;
            matchedDescriptor = null;
            if (string.IsNullOrEmpty(callee)) return false;

            var typeDisplayName = GetTypeDisplayNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(typeDisplayName)) return false;

            var methodName = GetMethodNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(methodName)) return false;
            if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                System.Console.Error.WriteLine($"[SHAPE_DEBUG] callee={callee} typeDisplayName={typeDisplayName} methodName={methodName}");

            bool _dbg = methodName.StartsWith("AddSaturate", StringComparison.Ordinal)
                || methodName.StartsWith("Ceiling", StringComparison.Ordinal)
                || methodName.StartsWith("IsZero", StringComparison.Ordinal);
            if (_dbg) System.Console.Error.WriteLine($"[SHAPE] callee={callee} td={typeDisplayName} mn={methodName} entries={_inlineDescriptors.Count}");

            foreach (var entry in _inlineDescriptors)
            {
                if (!string.Equals(entry.MethodName, methodName, StringComparison.Ordinal))
                {
                    // Try generic method matching: "Add<System.Int32>" → "Add"
                    var bracketStart = entry.MethodName + "<";
                    if (methodName.StartsWith(bracketStart, StringComparison.Ordinal) && methodName.EndsWith(">"))
                    {
                        // Generic method — method name matches with type args
                    }
                    else if (methodName.StartsWith(entry.MethodName + "[[", StringComparison.Ordinal) && methodName.EndsWith("]]"))
                    {
                        // Generic method with [[...]] syntax
                    }
                    else
                    {
                        continue;
                    }
                }
                if (!typeDisplayName!.StartsWith(entry.TypeDisplayNamePrefix, StringComparison.Ordinal))
                    continue;

                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                var result = entry.Resolver(callee, paramTypes);
                if (_dbg) System.Console.Error.WriteLine($"[SHAPE]   RESOLVER mn={entry.MethodName} prefix={entry.TypeDisplayNamePrefix} result={(result != null ? "OK:" + result[..Math.Min(50, result.Length)] : "NULL")}");
                if (result != null)
                {
                    cppExpression = result;
                    matchedDescriptor = entry;
                    return true;
                }
            }

            return false;
        }

        /// <summary>Try to match a callee SubjectId to a generic shape descriptor by type+method prefix.</summary>
        public bool TryMatchGenericShape(
            string callee,
            [NotNullWhen(true)] out GenericShapeDescriptor? descriptor,
            [NotNullWhen(true)] out IReadOnlyList<string>? typeArgs)
        {
            descriptor = null;
            typeArgs = null;
            if (string.IsNullOrEmpty(callee)) return false;

            var typeDisplayName = GetTypeDisplayNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(typeDisplayName)) return false;

            var methodName = GetMethodNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(methodName)) return false;
            if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                System.Console.Error.WriteLine($"[SHAPE_DEBUG] callee={callee} typeDisplayName={typeDisplayName} methodName={methodName}");

            foreach (var entry in _genericDescriptors)
            {
                if (callee.Contains("GreaterThanAny") || callee.Contains("LessThanAny"))
                    // [GENSHAPE] debug — disabled to avoid TPG timeout on large chunks

                if (!string.Equals(entry.MethodName, methodName, StringComparison.Ordinal))
                {
                    // Try method-level generic args: methodName = "Equal[[System.Int32]]", entry.MethodName = "Equal"
                    var genericMarkerStart = entry.MethodName + "[[";
                    if (methodName.StartsWith(genericMarkerStart, StringComparison.Ordinal))
                    {
                        // Extract type args from between [[...]]
                        var afterMarker = methodName.Substring(genericMarkerStart.Length);
                        var closeBracket = afterMarker.LastIndexOf("]]", StringComparison.Ordinal);
                        if (closeBracket < 0) continue;
                        var argsPart = afterMarker.Substring(0, closeBracket);
                        typeArgs = argsPart.Split(new[] { "],[", "," }, StringSplitOptions.None);
                        descriptor = entry;
                        return true;
                    }

                    // Also try <...> format (standard .NET metadata / subject ID syntax)
                    var angleStart = entry.MethodName + "<";
                    if (methodName.StartsWith(angleStart, StringComparison.Ordinal) && methodName.EndsWith(">"))
                    {
                        // BUG FIX: verify the type display name matches before matching a generic
                        // method through the angle-bracket fallback.  Without this check,
                        // Array::Sort<System.Byte> would match List<T>::Sort's descriptor because
                        // the method name "Sort<System.Byte>" passes the StartsWith check, but
                        // the type prefix "System.Array" does not match "System.Collections.Generic.List".
                        if (!typeDisplayName.StartsWith(entry.TypeDisplayNamePrefix, StringComparison.Ordinal))
                            continue;
                        var inner = methodName.Substring(angleStart.Length, methodName.Length - angleStart.Length - 1);
                        typeArgs = inner.Split(',');
                        descriptor = entry;
                        return true;
                    }
                    continue; // method name doesn't match, skip to next descriptor
                }
                if (!typeDisplayName!.StartsWith(entry.TypeDisplayNamePrefix, StringComparison.Ordinal))
                {
                    if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                        System.Console.Error.WriteLine($"[SHAPE_DEBUG] typeDisplayName MISMATCH method={methodName} typeDisplayName=|{typeDisplayName}| TypeDisplayNamePrefix=|{entry.TypeDisplayNamePrefix}| entry.MethodName={entry.MethodName}");

                    // For generic type descriptors where the SubjectId uses <T> syntax (e.g. "Nullable<System.Int32>")
                    // but the descriptor registered with backtick syntax (e.g. "Nullable`1"):
                    // try matching by stripping the backtick arity suffix and checking for <...> brackets.
                    var backtickIndex = entry.TypeDisplayNamePrefix.IndexOf('`');
                    if (backtickIndex >= 0)
                    {
                        var baseTypeName = entry.TypeDisplayNamePrefix.Substring(0, backtickIndex);
                        if (typeDisplayName.StartsWith(baseTypeName, StringComparison.Ordinal))
                        {
                            var afterBase = typeDisplayName.Substring(baseTypeName.Length);
                            if (afterBase.StartsWith("<"))
                            {
                                // Extract type args from <...>
                                var closeBracket = afterBase.LastIndexOf(">", StringComparison.Ordinal);
                                if (closeBracket >= 0)
                                {
                                    var innerArgs = afterBase.Substring(1, closeBracket - 1);
                                    typeArgs = innerArgs.Split(',');
                                    descriptor = entry;
                                    return true;
                                }
                            }
                        }
                    }
                    continue;
                }

                // Try to extract generic type arguments from the declaring type (e.g. Span`1 -> System.Int32)
                var genericPart = typeDisplayName!.Substring(entry.TypeDisplayNamePrefix.Length);
                if (genericPart.StartsWith('`'))
                {
                    // Open generic — try to extract closed type args from [[...]] syntax
                    // (e.g. "List`1[[System.Int32]]" -> ["System.Int32"]).
                    // If no [[...]] is present (open generic subject ID like "List`1"),
                    // still match the descriptor — these resolvers work with generic
                    // pointer types and don't depend on concrete type args.
                    if (TryParseGenericTypeArgsFromTypeName(genericPart, out var parsedTypeArgs))
                    {
                        typeArgs = parsedTypeArgs;
                        descriptor = entry;
                        return true;
                    }
                    // Open generic without resolved type args — match with empty list
                    // (the resolver's Resolver lambda receives the callee subject ID
                    // and can inspect type parameters from the method signature).
                    typeArgs = Array.Empty<string>();
                    descriptor = entry;
                    return true;
                }

                // Non-generic descriptor match (no <...> brackets expected).
                // The Resolver inspects the callee directly to confirm the overload.
                if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                    System.Console.Error.WriteLine($"[SHAPE_DEBUG] TryMatchGenericShape SUCCESS at non-generic fallthrough method={methodName} typeDisplayName=|{typeDisplayName}| prefix=|{entry.TypeDisplayNamePrefix}| genericPart=|{genericPart}|");
                typeArgs = Array.Empty<string>();
                descriptor = entry;
                return true;
            }

            if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                System.Console.Error.WriteLine($"[SHAPE_DEBUG] TryMatchGenericShape EXHAUSTED method={methodName} typeDisplayName=|{typeDisplayName}| _genericDescriptors.Count={_genericDescriptors.Count}");

            return false;
        }

        /// <summary>Try to match a callee SubjectId to a registered shape by display-name-based shape key.</summary>
        public bool TryMatchShape(string callee, [NotNullWhen(true)] out ShapeEntry? entry)
        {
            entry = null;
            if (string.IsNullOrEmpty(callee)) return false;

            var typeDisplayName = GetTypeDisplayNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(typeDisplayName)) return false;

            var methodName = GetMethodNameFromSubjectId(callee);
            if (string.IsNullOrEmpty(methodName)) return false;
            if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
                System.Console.Error.WriteLine($"[SHAPE_DEBUG] callee={callee} typeDisplayName={typeDisplayName} methodName={methodName}");

            var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
            var canonicalKey = BuildCanonicalKey(typeDisplayName!, methodName!, paramTypes);
            var hash = Fnv1aHash(canonicalKey);

            if (methodName == "GetHRForLastWin32Error" || methodName == "GetLastPInvokeError")
            {
                var found = _entriesByShapeId.TryGetValue(hash, out _);
                System.Console.Error.WriteLine($"[SHAPE_DEBUG] TryMatchShape hash={hash} canonicalKey=|{canonicalKey}| found={found} entriesCount={_entriesByShapeId.Count}");
            }

            return _entriesByShapeId.TryGetValue(hash, out entry);
        }

        public IReadOnlyCollection<ShapeEntry> Entries => _entriesByShapeId.Values;
        public int EntryCount => _entriesByShapeId.Count;

        /// <summary>Build the C++ "runtime_helper_shapes.h" header content.</summary>
        public string GenerateCppShapeHeader()
        {
            var template = NativeAotTemplateCatalog.GetRuntimeHelperShapeDispatchHeaderTemplate();

            var nativeIntEntries = new List<ScriptObject>();
            var voidEntries = new List<ScriptObject>();
            var int32Entries = new List<ScriptObject>();

            var shapeEntries = Entries.OrderBy(e => e.EnumName, StringComparer.Ordinal).ToList();
            var dispatchEntryIndex = 0;
            for (var i = 0; i < shapeEntries.Count; i++)
            {
                var entry = shapeEntries[i];

                // Only dispatchable entries (SimpleForward) participate in constexpr dispatch branches.
                // InlineBody entries have no callable native function — their NativeFnSymbol is not a real
                // function call (e.g. "CHAOS_IL2CPP_ABORT") — so they must be excluded from Dispatch* templates
                // to avoid generating invalid C++ code like "CHAOS_IL2CPP_ABORT(args...)".
                if (entry.Kind != ShapeKind.SimpleForward)
                    continue;

                var constexprPrefix = dispatchEntryIndex == 0 ? "if constexpr" : "else if constexpr";
                dispatchEntryIndex++;
                var templateEntry = new ScriptObject
                {
                    ["enum_name"] = entry.EnumName,
                    ["shape_id"] = entry.ShapeId.ToString("X8"),
                    ["native_fn_symbol"] = entry.NativeFnSymbol,
                    ["constexpr_prefix"] = constexprPrefix,
                };

                var returnKind = entry.ReturnAbi.CarrierKindCode;
                if (returnKind == AotCoreIrAbiCarrierKind.Void)
                    voidEntries.Add(templateEntry);
                else if (returnKind == AotCoreIrAbiCarrierKind.Int32)
                    int32Entries.Add(templateEntry);
                else
                    nativeIntEntries.Add(templateEntry);
            }

            var model = new ScriptObject
            {
                ["shape_entries"] = shapeEntries.Select(e => new ScriptObject
                {
                    ["enum_name"] = e.EnumName,
                    ["shape_id"] = e.ShapeId.ToString("X8"),
                }).ToList(),
                ["shape_entry_count"] = shapeEntries.Count,
                ["native_int_entries"] = nativeIntEntries,
                ["void_entries"] = voidEntries,
                ["int32_entries"] = int32Entries,
            };

            return ScribanTemplateRenderer.RenderTemplate(template, model);
        }

        /// <summary>Build the default registry containing all known runtime helper shapes.</summary>
        public static RuntimeHelperShapeRegistry BuildDefault()
        {
            var registry = new RuntimeHelperShapeRegistry();

            // ── Marshal interop stubs (registered early, before IL method size cutoff) ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetLastPInvokeError",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetLastPInvokeError();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalGetLastPInvokeError");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetHRForLastWin32Error",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetHRForLastWin32Error();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            // ── Marshal simple ICALL stubs (GenericShapeDescriptor for interop_stubs.cpp) ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "SetLastPInvokeError",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    ChaosMarshalSetLastPInvokeError(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetExceptionCode",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetExceptionCode();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalGetExceptionCode");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetExceptionPointers",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "", [
                        "    return ChaosMarshalGetExceptionPointers();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalAreComObjectsAvailableForCleanup");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "AreComObjectsAvailableForCleanup",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalAreComObjectsAvailableForCleanup();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices);
                }));



            // Helper: register a precompiled JsonSerializer::Serialize<T> stub.
            void RegisterJsonSerialize(string typeArg, string nativeFn)
            {
                var fn = nativeFn;
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "JsonSerializer",
                    MethodName: "Serialize",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var t = typeArgs != null && typeArgs.Count > 0 ? typeArgs[0] : null;
                        if (!string.Equals(t, typeArg, StringComparison.Ordinal)) return null;
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                            new[] { $"    return {fn}(chaos_arg_0);" });
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: fn);
                    }));
            }

            // Helper: register a precompiled JsonSerializer::Deserialize<T> stub.
            void RegisterJsonDeserialize(string typeArg, string nativeFn)
            {
                var fn = nativeFn;
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "JsonSerializer",
                    MethodName: "Deserialize",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var t = typeArgs != null && typeArgs.Count > 0 ? typeArgs[0] : null;
                        if (!string.Equals(t, typeArg, StringComparison.Ordinal)) return null;
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                            new[] { $"    return static_cast<CHAOS_IL2CPP_INTPTR>({fn}(chaos_arg_0));" });
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: fn);
                    }));
            }

            // ── System.Text.Json.JsonSerializer stubs (PrecompiledJsonTypeInfo) ──
            // Pre-compiled primitive type serialization that bypasses managed
            // JsonSerializer's runtime JsonTypeInfo<T> building and reflection.
            // Native stubs in interop_stubs.cpp format values directly via snprintf.

            // Primitives: format via snprintf → ChaosStringCreateFromUtf8
            RegisterJsonSerialize("System.Int32", "ChaosJsonSerializeInt32");
            RegisterJsonSerialize("System.Int64", "ChaosJsonSerializeInt64");
            RegisterJsonSerialize("System.Int16", "ChaosJsonSerializeInt32");
            RegisterJsonSerialize("System.Byte", "ChaosJsonSerializeInt32");
            RegisterJsonSerialize("System.SByte", "ChaosJsonSerializeInt32");
            RegisterJsonSerialize("System.UInt16", "ChaosJsonSerializeInt32");
            RegisterJsonSerialize("System.UInt32", "ChaosJsonSerializeInt64");
            RegisterJsonSerialize("System.UInt64", "ChaosJsonSerializeInt64");
            RegisterJsonSerialize("System.Boolean", "ChaosJsonSerializeBool");
            RegisterJsonSerialize("System.String", "ChaosJsonSerializeString");

            // ── Deserialize<T> stubs ──
            RegisterJsonDeserialize("System.Int32", "ChaosJsonDeserializeInt32");
            RegisterJsonDeserialize("System.Int64", "ChaosJsonDeserializeInt64");
            RegisterJsonDeserialize("System.Boolean", "ChaosJsonDeserializeBool");

            // ── Dictionary<K,V>::TryAdd (smoke-test stub) ──
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

            // ── Array::GetValue (GenericShapeDescriptor — passes first index to ChaosArrayGetValue) ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "GetValue",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    for (int pi = 0; pi < paramTypes.Count; pi++)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = "CHAOS_IL2CPP_INTPTR chaos_arg_0";
                    for (int pi = 0; pi < paramTypes.Count; pi++)
                        paramSig += ", CHAOS_IL2CPP_INT32 chaos_arg_" + (pi + 1);
                    var stubBody = paramTypes.Count > 0
                        ? "    return ChaosArrayGetValue(chaos_arg_0, chaos_arg_1);"
                        : "    return ChaosArrayGetValue(chaos_arg_0, 0);";
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        stubBody,
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // Delegate remaining ~7700 registrations to a helper method
            // to stay within the CLR 64KB IL method size limit.

            // ── Marshal short-name stubs (AOT IR uses "Marshal" not "System.Runtime.InteropServices.Marshal") ──
            RegisterMarshalShortNameStubs(registry);

            RegisterCoreStubs(registry);
            RegisterCryptoStubs(registry);
            return registry;
        }

        private static void RegisterCoreStubs(RuntimeHelperShapeRegistry registry)
        {
            RegisterCoreStubs_Part1(registry);
            RegisterCoreStubs_Part2(registry);
            RegisterCoreStubs_Part3(registry);
        }

        private static void RegisterMarshalShortNameStubs(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "GetLastPInvokeError",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetLastPInvokeError();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalGetLastPInvokeError");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "GetHRForLastWin32Error",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetHRForLastWin32Error();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalGetHRForLastWin32Error");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "SetLastPInvokeError",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    ChaosMarshalSetLastPInvokeError(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "GetExceptionCode",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalGetExceptionCode();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalGetExceptionCode");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "GetExceptionPointers",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "", [
                        "    return ChaosMarshalGetExceptionPointers();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosMarshalAreComObjectsAvailableForCleanup");
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Marshal",
                MethodName: "AreComObjectsAvailableForCleanup",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        "    return ChaosMarshalAreComObjectsAvailableForCleanup();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateInt32AbiSlot(),
                        EmptyRawArgumentIndices);
                }));
        }

        private static string? MapTypeArgToCppType(string typeArg)
        {
            return typeArg switch
            {
                "System.Byte" => "CHAOS_IL2CPP_UINT8",
                "System.SByte" => "CHAOS_IL2CPP_INT8",
                "System.Int16" => "CHAOS_IL2CPP_INT16",
                "System.UInt16" => "CHAOS_IL2CPP_UINT16",
                "System.Int32" => "CHAOS_IL2CPP_INT32",
                "System.UInt32" => "CHAOS_IL2CPP_UINT32",
                "System.Int64" => "CHAOS_IL2CPP_INT64",
                "System.UInt64" => "CHAOS_IL2CPP_UINT64",
                "System.Single" => "float",
                "System.Double" => "double",
                "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
                "System.UIntPtr" => "CHAOS_IL2CPP_UINTPTR",
                _ => null,
            };
        }

        private static string InferVectorCarrierType(string callee)
        {
            if (callee.Contains("Vector256")) return "RuntimeIntrinsicVector256Carrier";
            return "RuntimeIntrinsicVector128Carrier"; // default for Vector128
        }
    }
}

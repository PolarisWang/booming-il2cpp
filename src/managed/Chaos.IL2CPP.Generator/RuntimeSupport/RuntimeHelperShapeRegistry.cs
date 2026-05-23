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
    internal sealed class RuntimeHelperShapeRegistry
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

            foreach (var entry in _inlineDescriptors)
            {
                if (!string.Equals(entry.MethodName, methodName, StringComparison.Ordinal))
                    continue;
                if (!typeDisplayName!.StartsWith(entry.TypeDisplayNamePrefix, StringComparison.Ordinal))
                    continue;

                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                var result = entry.Resolver(callee, paramTypes);
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

            foreach (var entry in _genericDescriptors)
            {
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
                    // Open generic — extract type args from the method name's suffix
                    // e.g. "System.Collections.Immutable.ImmutableArray`1::get_Item" -> needs method-level generic parsing
                    // For now, extract from the declaring type's `` suffix
                    if (TryParseGenericTypeArgsFromTypeName(genericPart, out var parsedTypeArgs))
                    {
                        typeArgs = parsedTypeArgs;
                        descriptor = entry;
                        return true;
                    }
                    continue;
                }

                // Non-generic descriptor match (no <...> brackets expected).
                // The Resolver inspects the callee directly to confirm the overload.
                typeArgs = Array.Empty<string>();
                descriptor = entry;
                return true;
            }

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

            var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
            var canonicalKey = BuildCanonicalKey(typeDisplayName!, methodName!, paramTypes);
            var hash = Fnv1aHash(canonicalKey);

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

            // === String operations ===
            registry.Register("System.String", "Concat", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // Concat 3-arg and 4-arg use pair-call composition (not single forward), so they need GenericShapeDescriptors.
            // The InlineBody entries below are used for C++ dispatch header generation only.
            registry.Register("System.String", "Concat", ["System.String", "System.String", "System.String"],
                ShapeKind.InlineBody, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.String", "Concat", ["System.String", "System.String", "System.String", "System.String"],
                ShapeKind.InlineBody, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2, 3 });

            // GenericShapeDescriptor for String.Concat — resolves 3-arg/4-arg by checking parameter count
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Concat",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var stringRetAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);
                    var stringRefAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);

                    if (paramTypes.Count == 3)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        [
                            "    const auto chaos_left_pair = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);",
                            "    return ChaosReflectionConcatStringPairValues(chaos_left_pair, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                stringRefAbi, stringRefAbi, stringRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1, 2 });
                    }

                    if (paramTypes.Count == 4)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
                        [
                            "    const auto chaos_left_pair = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);",
                            "    const auto chaos_right_pair = ChaosReflectionConcatStringPairValues(chaos_arg_2, chaos_arg_3);",
                            "    return ChaosReflectionConcatStringPairValues(chaos_left_pair, chaos_right_pair);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                stringRefAbi, stringRefAbi, stringRefAbi, stringRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1, 2, 3 });
                    }

                    return null;
                }));

            registry.Register("System.String", "op_Equality", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "chaos_object_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.String", "get_Length", [],
                ShapeKind.SimpleForward, "chaos_string_get_length",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.String", "StartsWith", ["System.String", "System.StringComparison"],
                ShapeKind.SimpleForward, "ChaosStringStartsWith",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.String", "Contains", ["System.String", "System.StringComparison"],
                ShapeKind.SimpleForward, "ChaosStringContains",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // GenericShapeDescriptors for String shapes with complex body logic

            // String.op_Equality — returns chaos_object_equals result as 0/1 native int
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "op_Equality",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_object_equals(chaos_arg_0, chaos_arg_1)",
                        "        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // String.get_Length — handles string_id and native string pointer
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "get_Length",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_is_string_id(chaos_arg_0))",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INT32>(",
                        "            chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count);",
                        "    }",
                        string.Empty,
                        "    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // String.StartsWith — ordinal comparison with memcmp
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "StartsWith",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    if (chaos_arg_2 != 4)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_arg_1 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
                        "    const char* chaos_prefix_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_0)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0)->length));",
                        "    const auto chaos_prefix_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_1)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_1)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_1)->length));",
                        "    if (chaos_prefix_length > chaos_instance_length)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    if (chaos_prefix_length == 0)",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "    }",
                        string.Empty,
                        "    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
                        "        (chaos_prefix_utf8 == nullptr && chaos_prefix_length != 0))",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    return CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8, chaos_prefix_utf8, chaos_prefix_length) == 0",
                        "        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // String.Contains — ordinal substring search with memcmp loop
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Contains",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    if (chaos_arg_2 != 4)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_arg_1 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
                        "    const char* chaos_value_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_0)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0)->length));",
                        "    const auto chaos_value_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_1)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_1)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_1)->length));",
                        "    if (chaos_value_length == 0)",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_value_length > chaos_instance_length)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
                        "        (chaos_value_utf8 == nullptr && chaos_value_length != 0))",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const auto chaos_last_start = chaos_instance_length - chaos_value_length;",
                        "    for (CHAOS_IL2CPP_SIZE chaos_index = 0; chaos_index <= chaos_last_start; chaos_index++)",
                        "    {",
                        "        if (CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8 + chaos_index, chaos_value_utf8, chaos_value_length) == 0)",
                        "        {",
                        "            return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "        }",
                        "    }",
                        string.Empty,
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // === Exception operations ===
            registry.Register("System.Exception", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosReflectionInitDefaultException",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Exception", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.InvalidOperationException", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.ArgumentOutOfRangeException", ".ctor", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Exception", "get_Message", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetExceptionMessage",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.ArgumentException", "get_ParamName", [],
                ShapeKind.SimpleForward, "chaos_reflection_get_exception_param_name",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === GC operations (inline body — native fn symbol is not a real function call) ===
            registry.Register("System.GC", "Collect", [],
                ShapeKind.InlineBody, "chaos_gc_collect()",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "WaitForPendingFinalizers", [],
                ShapeKind.InlineBody, "chaos_gc_wait_for_pending_finalizers()",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "KeepAlive", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_keepalive",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "SuppressFinalize", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_suppress_finalize",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "ReRegisterForFinalize", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_reregister_finalize",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Managed GC API (P2-1: GetTotalMemory, AddMemoryPressure, RemoveMemoryPressure) ===
            registry.Register("System.GC", "GetTotalMemory", ["System.Boolean"],
                ShapeKind.SimpleForward, "chaos_gc_get_total_memory",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),  // bool -> Int32 in unmanaged ABI
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.GC", "AddMemoryPressure", ["System.Int64"],
                ShapeKind.SimpleForward, "chaos_gc_add_memory_pressure",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "RemoveMemoryPressure", ["System.Int64"],
                ShapeKind.SimpleForward, "chaos_gc_remove_memory_pressure",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === GC.Collect(int, GCCollectionMode) — overload with mode ===
            registry.Register("System.GC", "Collect", ["System.Int32", "System.GCCollectionMode"],
                ShapeKind.SimpleForward, "chaos_gc_collect_with_mode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),  // generation
                    CreateInt32AbiSlot(),  // mode
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === GCLatencyMode property getter/setter ===
            registry.Register("System.GC", "get_LatencyMode", [],
                ShapeKind.SimpleForward, "chaos_gc_get_latency_mode",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "set_LatencyMode", ["System.GCLatencyMode"],
                ShapeKind.SimpleForward, "chaos_gc_set_latency_mode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === GC.CollectionCount(int) — returns collection count per generation ===
            registry.Register("System.GC", "CollectionCount", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_get_collection_count",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === GC.GetTotalPauseDuration — returns total accumulated pause (ns) as Int64 ===
            registry.Register("System.GC", "GetTotalPauseDuration", [],
                ShapeKind.SimpleForward, "chaos_gc_get_total_pause_duration",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

            // === GC.GetAllocatedBytesForCurrentThread — returns per-thread allocated bytes (Int64) ===
            registry.Register("System.GC", "GetAllocatedBytesForCurrentThread", [],
                ShapeKind.SimpleForward, "chaos_gc_get_allocated_bytes_for_current_thread",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

            // === GC.TryStartNoGCRegion — two overloads ===
            registry.Register("System.GC", "TryStartNoGCRegion", ["System.Int64"],
                ShapeKind.SimpleForward, "chaos_gc_try_start_no_gc_region",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),  // returns bool (0/1)
                new HashSet<int> { 0 });

            registry.Register("System.GC", "TryStartNoGCRegion", ["System.Int64", "System.Boolean"],
                ShapeKind.SimpleForward, "chaos_gc_try_start_no_gc_region",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),  // returns bool (0/1)
                new HashSet<int> { 0, 1 });

            // === GC.EndNoGCRegion — returns Int32 (0=Success, 1=GCTriggered) ===
            registry.Register("System.GC", "EndNoGCRegion", [],
                ShapeKind.SimpleForward, "chaos_gc_end_no_gc_region",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Full GC Notification ===
            registry.Register("System.GC", "RegisterForFullGCNotification", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_enable_full_gc_notification",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.GC", "CancelFullGCNotification", [],
                ShapeKind.SimpleForward, "chaos_gc_disable_full_gc_notification",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "WaitForFullGCApproach", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_wait_for_full_gc_approach",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "WaitForFullGCComplete", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_wait_for_full_gc_complete",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === GCMemoryInfo ===
            // Matches the BCL InternalCall signature: GetMemoryInfo(GCMemoryInfoData data, int kind)
            // GCMemoryInfoData is a class; codegen passes the object reference as a native int (pointer),
            // and the native function computes the interior pointer (past MethodTable*).
            registry.Register("System.GC", "GetMemoryInfo", ["System.GCMemoryInfoData", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_get_memory_info",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Delegate ===
            registry.Register("System.Delegate", "Combine", ["System.Delegate", "System.Delegate"],
                ShapeKind.SimpleForward, "chaos_delegate_combine",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Delegate", "Remove", ["System.Delegate", "System.Delegate"],
                ShapeKind.SimpleForward, "chaos_delegate_remove",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Object ===
            registry.Register("System.Object", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosObjectCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Object", "Equals", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_object_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });


            // === Object.GetHashCode (SimpleForward) ===
            registry.Register("System.Object", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosObjectGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Object.ToString (SimpleForward) ===
            registry.Register("System.Object", "ToString", [],
                ShapeKind.SimpleForward, "ChaosObjectToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Object.GetType (SimpleForward) ===
            registry.Register("System.Object", "GetType", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectGetType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Object::Equals static (two-arg overload) ===
            registry.Register("System.Object", "Equals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectEqualsStatic",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Object::ReferenceEquals ===
            registry.Register("System.Object", "ReferenceEquals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "chaos_object_reference_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Stream/IO (SimpleForward) ===
            registry.Register("System.IO.Stream", "Flush", [],
                ShapeKind.SimpleForward, "ChaosStreamFlush",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "Seek", ["System.Int64", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamSeek",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.IO.Stream", "SetLength", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosStreamSetLength",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.Stream", "get_Length", [],
                ShapeKind.SimpleForward, "ChaosStreamGetLength",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "get_Position", [],
                ShapeKind.SimpleForward, "ChaosStreamGetPosition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.MemoryStream", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.StringReader", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.StringWriter", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosStringWriterCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadLine", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadToEnd", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadToEnd",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.TextWriter", "WriteLine", [],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "WriteLine", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLineStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── Stream.Read/Write/CopyTo ──
            registry.Register("System.IO.Stream", "Read", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamRead",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "Write", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "CopyTo", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosStreamCopyTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── MemoryStream(byte[]) ──
            registry.Register("System.IO.MemoryStream", ".ctor", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtorWithBuffer",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── BinaryReader ──
            registry.Register("System.IO.BinaryReader", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryReader", "ReadInt32", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadString", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadDouble", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // ── BinaryWriter ──
            registry.Register("System.IO.BinaryWriter", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteString",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Array.Empty<T> (SimpleForward — returns null stub) ===
            registry.Register("System.Array", "Empty", [],
                ShapeKind.SimpleForward, "ChaosArrayEmpty",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === Type::GetMethod generic handler (GenericShapeDescriptor -- handles various overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetMethod",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    (void)chaos_arg_0; (void)chaos_arg_1;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src2, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1, 2 });
                }));


            // === Environment ===
            registry.Register("System.Environment", "get_CurrentManagedThreadId", [],
                ShapeKind.SimpleForward, "chaos_current_managed_thread_id",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Console (stubs for verification pipelines — tests track via ChaosAssertState.ExitCode) ===
            registry.Register("System.Console", "get_Error", [],
                ShapeKind.SimpleForward, "ChaosConsoleGetError",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Console",
                MethodName: "WriteLine",
                Resolver: static (planner, callee, typeArgs) =>
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
                    // WriteLine(string) — 1-arg static
                    var src1 = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                    ]);
                    return new GenericShapeResolution(src1, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === OperatingSystem platform checks (dispatch via GenericShapeDescriptors below) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsWindows",
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Numeric formatting ===
            registry.Register("System.Int32", "ToString", [],
                ShapeKind.SimpleForward, "chaos_format_int32_to_string",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Single", "ToString", ["System.String"],
                ShapeKind.SimpleForward, "chaos_format_single_to_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Double", "ToString", ["System.String"],
                ShapeKind.SimpleForward, "chaos_format_double_to_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Int32",
                MethodName: "ToString",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));",
                        "    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
                        "        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
                        "    return chaos_make_string_id_value(chaos_id);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Single",
                MethodName: "ToString",
                Resolver: static (planner, callee, typeArgs) =>
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
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_float(chaos_load_float32(chaos_arg_0));",
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
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Marshal non-generic operations (inline body with runtime core calls) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "AllocHGlobal",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    if (chaos_arg_0 < 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalAllocHGlobal(",
                        "        chaos_runtime, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeHGlobal",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeHGlobal(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeCoTaskMem",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.StringToHGlobalAnsi(String) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalAnsi",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalAnsi(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.StringToHGlobalUni(String) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalUni",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalUni(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.PtrToStringAnsi(IntPtr, int) → String ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringAnsi",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalPtrToStringAnsiIcall(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

            // === Marshal.UnsafeAddrOfPinnedArrayElement(Array, int) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "UnsafeAddrOfPinnedArrayElement",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalUnsafeAddrOfPinnedArrayElement(",
                        "        reinterpret_cast<void*>(chaos_arg_0), chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToCoTaskMemUTF8",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                        "    (void)chaos_length; (void)chaos_utf8_data;",
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringUTF8",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_arg_0 == 0",
                        "        ? 0",
                        "        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.SizeOf<T> — return marshalled struct size via descriptor ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "SizeOf",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    return chaos_desc != nullptr",
                        "        ? static_cast<CHAOS_IL2CPP_INT32>(chaos_desc->total_size)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                        CreateInt32AbiSlot(),
                        new HashSet<int>());
                }));

            // === Marshal.OffsetOf<T>(string) — return field offset by name via descriptor ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "OffsetOf",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var fieldNamesSymbol = NativeAotLoweringPlanner.GetNativeStructFieldNamesSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ::chaos::il2cpp::runtime_core::MarshalOffsetOf(",
                        $"        {structDescSymbol},",
                        $"        {fieldNamesSymbol},",
                        "        chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.Copy (element type and direction extracted from parameter types) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "Copy",
                Resolver: static (planner, callee, typeArgs) =>
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

                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);

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

            // === Marshal.StructureToPtr<T> — deep-copy managed struct to native memory ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StructureToPtr",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc != nullptr) {",
                        "        if (chaos_arg_2 != 0) {",
                        "            chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                        "                chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_1), chaos_runtime);",
                        "        }",
                        "        chaos::il2cpp::struct_marshal::MarshalStructManagedToNative(",
                        "            chaos_desc,",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_1),",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_0),",
                        "            chaos_runtime, nullptr);",
                        "    }",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

            // === Marshal.PtrToStructure<T> — deep-copy native memory to managed struct ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStructure",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc == nullptr) return 0;",
                        "    auto* chaos_blob = static_cast<unsigned char*>(",
                        "        CHAOS_IL2CPP_MALLOC(chaos_desc->total_size));",
                        "    if (chaos_blob == nullptr) return 0;",
                        "    chaos::il2cpp::struct_marshal::MarshalStructNativeToManaged(",
                        "        chaos_desc, chaos_blob,",
                        "        reinterpret_cast<const unsigned char*>(chaos_arg_0),",
                        "        chaos_runtime, nullptr);",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_blob);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.DestroyStructure — free native resources for marshalled struct ===
            // Generic overload: DestroyStructure<T>(IntPtr) — uses static descriptor.
            // Non-generic overload: DestroyStructure(IntPtr, Type) — V1: no-op (runtime reflection path).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "DestroyStructure",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 1)
                    {
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                            "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                            $"    const auto* chaos_desc = {structDescSymbol};",
                            "    if (chaos_desc != nullptr) {",
                            "        chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                            "            chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_0), chaos_runtime);",
                            "    }",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    // Non-generic overload: Marshal.DestroyStructure(IntPtr, Type)
                    // Extracts TypeInfoHot* from the managed Type object via runtime helper.
                    var nonGenericSymbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var nonGenericSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", nonGenericSymbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ::chaos::il2cpp::runtime_core::ChaosDestroyStructureByType(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(nonGenericSrc, nonGenericSymbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // === String.Join (IEnumerable<T> — resolves variants at planning time) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Join",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!TryGetStringJoinEnumerableElementType(callee, out var elementType))
                        return null;
                    var variants = planner.ResolveEnumerableJoinSupportVariants(elementType!);
                    if (variants.Count == 0) return null;
                    ExternalRuntimeHelperDefinition? def = null;
                    if (string.Equals(elementType, "System.Int32", StringComparison.Ordinal) ||
                        string.Equals(elementType, "System.Int64", StringComparison.Ordinal) ||
                        string.Equals(elementType, "System.Int16", StringComparison.Ordinal))
                    {
                        def = planner.CreateStringJoinInt32EnumerableRuntimeHelperDefinition(callee, variants);
                    }
                    if (string.Equals(elementType, "System.String", StringComparison.Ordinal))
                    {
                        def = planner.CreateStringJoinStringEnumerableRuntimeHelperDefinition(callee, variants);
                    }
                    if (def == null) return null;
                    return new GenericShapeResolution(def.Source, def.TargetSymbol,
                        def.ParameterAbis, def.ReturnAbi, def.RawArgumentIndices,
                        def.ReferencedStaticFieldSubjectIds);
                }));

            // === String::Join(System.String,System.String[]) (non-generic overload, stub returning null) ===
            registry.Register("System.String", "Join", ["System.String", "System.String[]"],
                ShapeKind.SimpleForward, "ChaosStringJoinSs",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === CustomAttribute: MemberInfo.IsDefined (planning-time condition checked in Resolver) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.MemberInfo",
                MethodName: "IsDefined",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!planner._customAttributeSupport.UsesMemberInfoIsDefined)
                        return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    if (!string.Equals(paramTypes[1], "System.Boolean", StringComparison.Ordinal))
                        return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var typeTypeSymbol = NativeAotLoweringPlanner.GetNativeTypeSymbol("System.Private.CoreLib/System.Type");
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    (void)chaos_arg_2;",
                        "    if (chaos_arg_0 == 0 || chaos_arg_1 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        $"    auto* chaos_type = reinterpret_cast<{typeTypeSymbol}*>(chaos_arg_1);",
                        "    if (chaos_type->runtime_type_handle == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    return ChaosReflectionGetCustomAttribute(chaos_arg_0, chaos_type->runtime_type_handle) == 0",
                        "        ? 0",
                        "        : 1;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
                }));

            // === InterpolatedStringHandler ===
            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", ".ctor",
                ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_reset",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendFormatted", ["System.String"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_append_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendLiteral", ["System.String"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_append_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "ToStringAndClear", [],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_to_string_and_clear",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === RuntimeHelpers ===
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "InitializeArray",
                ["System.Array", "System.RuntimeFieldHandle"],
                ShapeKind.SimpleForward, "chaos_initialize_array_from_field_data_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 1 });

            // === Monitor ===
            registry.Register("Monitor", "Enter", ["System.Object", "System.Boolean&"],
                ShapeKind.SimpleForward, "chaos_monitor_enter",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("Monitor", "Exit", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_monitor_exit",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread ===
            registry.Register("System.Threading.Thread", ".ctor",
                ["System.Threading.ThreadStart"],
                ShapeKind.SimpleForward, "chaos_thread_ctor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Threading.ThreadStart", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Threading.Thread", "Start", [],
                ShapeKind.SimpleForward, "chaos_thread_start",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.Thread", "Join", [],
                ShapeKind.SimpleForward, "chaos_thread_join",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.Thread", "get_CurrentThread", [],
                ShapeKind.SimpleForward, "chaos_thread_get_current",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);


            // === Thread::Sleep ===
            registry.Register("System.Threading.Thread", "Sleep", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_thread_sleep",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    }),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            // === Thread::Yield ===
            registry.Register("System.Threading.Thread", "Yield", [],
                ShapeKind.SimpleForward, "chaos_thread_yield",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Thread::Abort ===
            registry.Register("System.Threading.Thread", "Abort", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_thread_abort",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Thread::Interrupt ===
            registry.Register("System.Threading.Thread", "Interrupt", [],
                ShapeKind.SimpleForward, "chaos_thread_interrupt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_IsBackground ===
            registry.Register("System.Threading.Thread", "get_IsBackground", [],
                ShapeKind.SimpleForward, "chaos_thread_is_background",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::set_IsBackground ===
            registry.Register("System.Threading.Thread", "set_IsBackground", ["System.Boolean"],
                ShapeKind.SimpleForward, "chaos_thread_set_background",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_Priority ===
            registry.Register("System.Threading.Thread", "get_Priority", [],
                ShapeKind.SimpleForward, "chaos_thread_get_priority",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::set_Priority ===
            registry.Register("System.Threading.Thread", "set_Priority", ["System.Threading.ThreadPriority"],
                ShapeKind.SimpleForward, "chaos_thread_set_priority",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_ThreadState ===
            registry.Register("System.Threading.Thread", "get_ThreadState", [],
                ShapeKind.SimpleForward, "chaos_thread_get_state",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_IsThreadPoolThread ===
            registry.Register("System.Threading.Thread", "get_IsThreadPoolThread", [],
                ShapeKind.SimpleForward, "chaos_thread_is_threadpool",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === DateTime ===
            registry.Register("System.DateTime", "get_UtcNow", [],
                ShapeKind.SimpleForward, "ChaosDatetimeGetUtcNow",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.DateTime"
                },
                EmptyRawArgumentIndices);

            // === TimeSpan ===
            registry.Register("System.TimeSpan", "FromMilliseconds", ["System.Double"],
                ShapeKind.SimpleForward, "chaos::il2cpp::support::TimeSpanFromMilliseconds",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
                    TypeSubjectId = "System.Private.CoreLib/System.TimeSpan",
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // === Reflection: Type ===
            registry.Register("System.Type", "GetTypeFromHandle", ["System.RuntimeTypeHandle"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "op_Inequality", ["System.Type", "System.Type"],
                ShapeKind.SimpleForward, "ChaosTypeInequality",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "get_Assembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssembly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericTypeDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericTypeDefinition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericArguments", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericArguments",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: Assembly ===
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "chaos_reflection_get_type_from_assembly",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Reflection.Assembly", "GetName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.AssemblyName", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyNameValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: MethodBase ===
            registry.Register("System.Reflection.MethodBase", "get_MethodHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethodHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MethodBase", "GetParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameters",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MethodBase", "Invoke", ["System.Object", "System.Object[]"],
                ShapeKind.SimpleForward, "ChaosReflectionInvokeMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            // === MethodBase::Invoke with extra params (GenericShapeDescriptor -- handles BindingFlags/Binder/CultureInfo overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.MethodBase",
                MethodName: "Invoke",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Build parameter ABI slots from param types
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count + 1);
                    abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)); // this
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)); // all refs
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Reflection: MemberInfo ===
            registry.Register("System.Reflection.MemberInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMemberName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MemberInfo", "get_DeclaringType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetDeclaringType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MemberInfo", "get_MetadataToken", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMetadataToken",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Activator ===
            registry.Register("System.Activator", "CreateInstance", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionCreateInstance",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: Type (additional) ===
            registry.Register("System.Type", "get_TypeHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetField", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetField",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Type::GetField with BindingFlags (GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetField",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::GetEvent (all overloads via GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetEvent",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.EventInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::GetProperty (all overloads via GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetProperty",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            registry.Register("System.Type", "GetMethod", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeByName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetConstructors", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructors",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::GetConstructors (0-param fallback) ===
            registry.Register("System.Type", "GetConstructors", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructorsDefault",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetMethod", ["System.String", "System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Reflection: MethodInfo ===
            registry.Register("System.Reflection.MethodInfo", "MakeGenericMethod", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Assembly::GetType(System.String,System.Boolean) overload ===
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromAssemblyBool",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Reflection: ParameterInfo ===
            registry.Register("System.Reflection.ParameterInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Async: Task.Yield / YieldAwaitable ===
            registry.Register("System.Threading.Tasks.Task", "Yield", [],
                ShapeKind.SimpleForward, "chaos_async_yield_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable", "GetAwaiter", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_awaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "get_IsCompleted", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_is_completed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_result",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Decimal ===
            registry.Register("System.Decimal", ".ctor", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_decimal_ctor_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // Decimal::op_Explicit(Decimal) -> Int32 — forward to ChaosDecimalToInt32
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: "op_Explicit",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosDecimalToInt32(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Nullable<T>.get_HasValue — reads the hasValue field at offset sizeof(ThinLockableHeader) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "get_HasValue",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Nullable<T>.GetValueOrDefault() — reads the value field after ThinLockableHeader+hasValue ===
            // Overloads: GetValueOrDefault() and GetValueOrDefault(T)
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "GetValueOrDefault",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Convert.ToChar (GenericShapeDescriptor — native bridge to convert.cpp) ===
            // All overloads delegate to chaos_convert_tochar_* functions in convert.cpp.
            // This eliminates 200+ lines of duplicate StringId/exception body code per overload.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (planner, callee, typeArgs) =>
                {
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
                    var args = abiSlots.Count == 0 ? "" :
                        string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"chaos_arg_{i}"));
                    string[] bodyLines;
                    if (abiSlots.Count == 0)
                    {
                        bodyLines = ["    return static_cast<CHAOS_IL2CPP_UINT16>(0);"];
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

            // === Convert.ToChar — inline shapes for value-type overloads ===
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

            // === Convert.ToChar — always-throw overloads (no possible valid conversion) ===
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

                        // === Array::Copy (GenericShapeDescriptor -- calls ChaosArrayCopy for 5-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Copy",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 5)
                    {
                        var srcFive = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3, CHAOS_IL2CPP_INT32 chaos_arg_4",
                        [
                            "    ChaosArrayCopy(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);",
                        ]);
                        return new GenericShapeResolution(srcFive, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[5]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3, 4 });
                    }
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("void", symbol, "",
                        [
                            "    return;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateVoidAbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt.Contains("Int64") || pt.Contains("Double") || pt.Contains("Single"))
                            abiSlots.Add(new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType });
                        else if (pt == "System.Int32" || pt == "System.Int32&" || pt == "System.Boolean")
                            abiSlots.Add(CreateInt32AbiSlot());
                        else
                            abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === CustomAttributeExtensions.GetCustomAttribute ===
            registry.Register("System.Reflection.CustomAttributeExtensions", "GetCustomAttribute",
                ["System.Reflection.Assembly", "System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionGetCustomAttribute",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Convert::ChangeType (GenericShapeDescriptor -- handles all overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ChangeType",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
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
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
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

            // === Buffer::ByteLength ===
            registry.Register("System.Buffer", "ByteLength", ["System.Array"],
                ShapeKind.SimpleForward, "ChaosBufferByteLength",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Exception::get_InnerException ===
            registry.Register("System.Exception", "get_InnerException", [],
                ShapeKind.SimpleForward, "ChaosExceptionGetInnerException",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === CultureInfo::get_CurrentCulture ===
            registry.Register("System.Globalization.CultureInfo", "get_CurrentCulture", [],
                ShapeKind.SimpleForward, "ChaosCultureGetCurrent",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === Guid::NewGuid ===
            registry.Register("System.Guid", "NewGuid", [],
                ShapeKind.SimpleForward, "ChaosGuidNewGuid",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.Guid"
                },
                EmptyRawArgumentIndices);

            // === Guid::GetHashCode ===
            registry.Register("System.Guid", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosGuidGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.Guid"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Guid::ToString ===
            registry.Register("System.Guid", "ToString", [],
                ShapeKind.SimpleForward, "ChaosGuidToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.Guid"
                    }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Int32::GetHashCode ===
            registry.Register("System.Int32", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosInt32GetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),  // "this" is managed pointer to Int32
                CreateInt32AbiSlot(),            // returns CHAOS_IL2CPP_INT32
                new HashSet<int> { 0 });

            // === Math::Abs (SimpleForward stubs) ===
            registry.Register("System.Math", "Abs", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathAbsInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Math", "Abs", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathAbsDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === Activator::CreateInstance with param array ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Activator",
                MethodName: "CreateInstance",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Assembly::GetExecutingAssembly ===
            registry.Register("System.Reflection.Assembly", "GetExecutingAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetExecutingAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === Type::MakeGenericType ===
            registry.Register("System.Type", "MakeGenericType", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === MemberInfo::get_ReflectedType ===
            registry.Register("System.Reflection.MemberInfo", "get_ReflectedType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetReflectedType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Module::get_FullyQualifiedName ===
            registry.Register("System.Reflection.Module", "get_FullyQualifiedName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetModuleName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === ParameterInfo::get_ParameterType ===
            registry.Register("System.Reflection.ParameterInfo", "get_ParameterType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === FieldInfo::get_FieldType ===
            registry.Register("System.Reflection.FieldInfo", "get_FieldType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetFieldType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === PropertyInfo::get_PropertyType ===
            registry.Register("System.Reflection.PropertyInfo", "get_PropertyType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetPropertyType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_FullName ===
            registry.Register("System.Type", "get_FullName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFullName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsValueType ===
            registry.Register("System.Type", "get_IsValueType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsValueType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === RuntimeHelpers::GetHashCode ===
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "GetHashCode", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Monitor::TryEnter (SimpleForward) ===
            registry.Register("Monitor", "TryEnter", ["System.Object", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMonitorTryEnter",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // === String::IndexOf (GenericShapeDescriptor -- handles various overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "IndexOf",
                Resolver: static (planner, callee, typeArgs) =>
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
                            Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.String")
                    {
                        var srcIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === DateTime::ToString (SimpleForward stubs) ===
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



            // === CustomAttributeExtensions::IsDefined ===
            registry.Register("System.Reflection.CustomAttributeExtensions", "IsDefined",
                ["System.Reflection.Assembly", "System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsDefined",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Exception::get_HResult ===
            registry.Register("System.Exception", "get_HResult", [],
                ShapeKind.SimpleForward, "ChaosExceptionGetHresult",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === CultureInfo::get_InvariantCulture ===
            registry.Register("System.Globalization.CultureInfo", "get_InvariantCulture", [],
                ShapeKind.SimpleForward, "ChaosCultureGetInvariant",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === Random::Next (GenericShapeDescriptor -- handles all overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Random",
                MethodName: "Next",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Math::Ceiling (SimpleForward stub) ===
            registry.Register("System.Math", "Ceiling", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathCeiling",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === Random::NextDouble ===
            registry.Register("System.Random", "NextDouble", [],
                ShapeKind.SimpleForward, "ChaosRandomNextDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === Array::CreateInstance (SimpleForward — elementType, length) ===
            registry.Register("System.Array", "CreateInstance", ["System.Type", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosArrayCreateInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Assembly::GetCallingAssembly ===
            registry.Register("System.Reflection.Assembly", "GetCallingAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetCallingAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === MethodInfo::GetBaseDefinition ===
            registry.Register("System.Reflection.MethodInfo", "GetBaseDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetBaseDefinition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Module::get_Name ===
            registry.Register("System.Reflection.Module", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetModuleNameOnly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === ParameterInfo::get_DefaultValue ===
            registry.Register("System.Reflection.ParameterInfo", "get_DefaultValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetDefaultValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_Namespace ===
            registry.Register("System.Type", "get_Namespace", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetNamespace",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsEnum ===
            registry.Register("System.Type", "get_IsEnum", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsEnum",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === Type::GetGenericParameterConstraints ===
            registry.Register("System.Type", "GetGenericParameterConstraints", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericParamConstraints",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === RuntimeHelpers::Equals ===
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "Equals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersEquals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === String::Substring (GenericShapeDescriptor -- calls ChaosStringSubstring for 3-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Substring",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcSubstr = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                        [
                            "    return ChaosStringSubstring(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(srcSubstr, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 });
                    }
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === List<T>::Clear — InlineShapeDescriptor (no function call) ===
            // Generates direct field assignment at call site, matching JIT inlining.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Clear",
                Resolver: static (callee, paramTypes) =>
                {
                    // Skip paramTypes check for diagnostic — accept any param count
                    // Comma expression: size=0, version++, result ignored (void return)
                    return "(reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8)->size = 0, reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8)->version++)";
                })
            { IsInstanceMethod = true });

            // === List<T>::Contains — InlineShapeDescriptor (no function call) ===
            // IILE lambda performs linear scan on the inline field buffer at call site.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Contains",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) return 1; } return 0; })()";
                })
            { IsInstanceMethod = true });

            // === List<T>::IndexOf — InlineShapeDescriptor (no function call) ===
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "IndexOf",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) return _i; } return -1; })()";
                })
            { IsInstanceMethod = true });

            // === List<T>::Remove — InlineShapeDescriptor (no function call) ===
            // IILE lambda: linear scan + std::memmove shift on inline field buffer.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Remove",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) { auto _shift = static_cast<CHAOS_IL2CPP_SIZE>(_list->size - _i - 1); if (_shift > 0) std::memmove(&_elems[_i], &_elems[_i + 1], _shift * sizeof(CHAOS_IL2CPP_INTPTR)); _list->size--; _list->version++; return 1; } } return 0; })()";
                })
            { IsInstanceMethod = true });

            // === List<T>::Clear (GenericShapeDescriptor for dispatch table) ===
            // Kept as fallback for hotpatch dispatch / indirect call paths.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Clear",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    _list->size = 0;",
                        "    _list->version++;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === List<T>::Add (inline field operations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Add",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size >= hdr->capacity) {",
                        "        auto old_cap = (hdr != nullptr) ? hdr->capacity : 0;",
                        "        auto new_cap = (old_cap == 0) ? 4 : old_cap * 2;",
                        "        auto* new_buf = static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_MALLOC(sizeof(CHAOS_IL2CPP_INT32) + static_cast<CHAOS_IL2CPP_SIZE>(new_cap) * sizeof(CHAOS_IL2CPP_INTPTR)));",
                        "        auto* new_hdr = reinterpret_cast<chaos_list_array_header*>(new_buf);",
                        "        new_hdr->capacity = new_cap;",
                        "        auto* new_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(new_hdr + 1);",
                        "        if (hdr != nullptr && _list->size > 0) {",
                        "            auto* old_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "            std::memcpy(new_elems, old_elems, static_cast<CHAOS_IL2CPP_SIZE>(_list->size) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "            CHAOS_IL2CPP_FREE(hdr);",
                        "        }",
                        "        _list->items_array = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_hdr);",
                        "        hdr = new_hdr;",
                        "    }",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    elems[_list->size] = chaos_arg_1;",
                        "    _list->size++;",
                        "    _list->version++;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // === List<T>::Contains (inline field operations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Contains",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size == 0) return 0;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    for (CHAOS_IL2CPP_INT32 i = 0; i < _list->size; i++) {",
                        "        if (elems[i] == chaos_arg_1) return 1;",
                        "    }",
                        "    return 0;",
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

            // === List<T>::IndexOf (inline field operations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "IndexOf",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size == 0) return -1;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    for (CHAOS_IL2CPP_INT32 i = 0; i < _list->size; i++) {",
                        "        if (elems[i] == chaos_arg_1) return i;",
                        "    }",
                        "    return -1;",
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

            // === List<T>::Remove (inline field operations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Remove",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size == 0) return 0;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    for (CHAOS_IL2CPP_INT32 i = 0; i < _list->size; i++) {",
                        "        if (elems[i] == chaos_arg_1) {",
                        "            auto move_count = _list->size - i - 1;",
                        "            if (move_count > 0) std::memmove(&elems[i], &elems[i + 1], static_cast<CHAOS_IL2CPP_SIZE>(move_count) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "            _list->size--;",
                        "            _list->version++;",
                        "            return 1;",
                        "        }",
                        "    }",
                        "    return 0;",
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

            // === List<T>::RemoveAt (inline field operations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "RemoveAt",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || chaos_arg_1 < 0 || chaos_arg_1 >= _list->size) return;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    auto move_count = _list->size - chaos_arg_1 - 1;",
                        "    if (move_count > 0) std::memmove(&elems[chaos_arg_1], &elems[chaos_arg_1 + 1], static_cast<CHAOS_IL2CPP_SIZE>(move_count) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "    _list->size--;",
                        "    _list->version++;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // === Span<T>::CopyTo (GenericShapeDescriptor -- handles generic Span<T>) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Span",
                MethodName: "CopyTo",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    (void)chaos_arg_0; (void)chaos_arg_1;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));


            // === Array::Clear ===
            registry.Register("System.Array", "Clear", ["System.Array", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosArrayClear",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            // === Type::IsAssignableFrom ===
            registry.Register("System.Type", "IsAssignableFrom", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsAssignableFrom",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Exception::GetBaseException ===
            registry.Register("System.Exception", "GetBaseException", [],
                ShapeKind.SimpleForward, "ChaosExceptionGetBaseException",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Exception", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === CultureInfo::get_Name ===
            registry.Register("System.Globalization.CultureInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosCultureGetName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Math::Floor (SimpleForward stub) ===
            registry.Register("System.Math", "Floor", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathFloor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // === Assembly::GetEntryAssembly ===
            registry.Register("System.Reflection.Assembly", "GetEntryAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetEntryAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // === Type::get_GenericParameterPosition ===
            registry.Register("System.Type", "get_GenericParameterPosition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericParamPos",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === RuntimeMethodHandle::GetHashCode ===
            registry.Register("System.RuntimeMethodHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimemethodhandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeMethodHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Module::get_Assembly ===
            registry.Register("System.Reflection.Module", "get_Assembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetModuleAssembly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === ParameterInfo::get_RawDefaultValue ===
            registry.Register("System.Reflection.ParameterInfo", "get_RawDefaultValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetRawDefaultValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_AssemblyQualifiedName ===
            registry.Register("System.Type", "get_AssemblyQualifiedName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyQualifiedName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === RuntimeHelpers::GetObjectValue ===
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "GetObjectValue", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersGetObjectValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === String::Compare (GenericShapeDescriptor -- handles all overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Compare",
                Resolver: static (planner, callee, typeArgs) =>
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
                    if (paramTypes.Count == 2 && paramTypes.All(p => p == "System.String"))
                    {
                        var srcCmp = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringCompare(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcCmp, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt == "System.String" || pt.Contains("System.String"))
                            abiSlots.Add(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType));
                        else
                            abiSlots.Add(CreateInt32AbiSlot());
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === DateTime::AddDays (SimpleForward stub) ===
            registry.Register("System.DateTime", "AddDays", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDateTimeAddDays",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0, 1 });

            // === Type::get_IsArray ===
            registry.Register("System.Type", "get_IsArray", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsArray",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === List<T>::Sort (inline field operations — zero-param overload uses std::sort) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Sort",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    if (abiSlots.Count == 1)
                    {
                        // Zero-param overload: Sort() — inline std::sort on elements
                        var src0 = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                            "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                            "    if (hdr == nullptr || _list->size < 2) return;",
                            "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                            "    std::sort(elems, elems + _list->size);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(abiSlots[0]),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Span<T>::get_Empty (GenericShapeDescriptor -- handles generic Span<T>) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Span",
                MethodName: "get_Empty",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices);
                }));


            // === Type::IsInstanceOfType ===
            registry.Register("System.Type", "IsInstanceOfType", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosReflectionIsInstanceOfType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === CultureInfo::get_DisplayName ===
            registry.Register("System.Globalization.CultureInfo", "get_DisplayName", [],
                ShapeKind.SimpleForward, "ChaosCultureGetDisplayName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Math::Round (SimpleForward stubs) ===
            registry.Register("System.Math", "Round", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathRound",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.Math", "Round", ["System.Double", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathRound2",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // === Assembly::get_FullName ===
            registry.Register("System.Reflection.Assembly", "get_FullName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyFullName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_ContainsGenericParameters ===
            registry.Register("System.Type", "get_ContainsGenericParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetContainsGenericParams",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === MethodBase::get_CallingConvention ===
            registry.Register("System.Reflection.MethodBase", "get_CallingConvention", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetCallingConvention",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Module::GetType ===
            registry.Register("System.Reflection.Module", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionModuleGetType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === ParameterInfo::get_HasDefaultValue ===
            registry.Register("System.Reflection.ParameterInfo", "get_HasDefaultValue", [],
                ShapeKind.SimpleForward, "ChaosReflectionHasDefaultValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === RuntimeTypeHandle::GetHashCode ===
            registry.Register("System.RuntimeTypeHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimetypehandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeTypeHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === RuntimeFieldHandle::GetHashCode ===
            registry.Register("System.RuntimeFieldHandle", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosRuntimefieldhandleGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.RuntimeFieldHandle"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === FormattableStringFactory::Create ===
            registry.Register("System.Runtime.CompilerServices.FormattableStringFactory", "Create",
                ["System.String", "System.Object[]"],
                ShapeKind.SimpleForward, "ChaosFormattablestringFactoryCreate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === DateTime::GetHashCode ===
            registry.Register("System.DateTime", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosDatetimeGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        TypeSubjectId = "System.Private.CoreLib/System.DateTime"
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Array::BinarySearch (GenericShapeDescriptor -- calls ChaosArrayBinarySearch for 2-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "BinarySearch",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcBinSearch = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosArrayBinarySearch(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcBinSearch, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "",
                        [
                            "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 4)
                    {
                        // BinarySearch(System.Array, System.Int32, System.Int32, System.Object)
                        // — routes to ChaosArrayBinarySearchRange
                        var src4 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
                        [
                            "    return ChaosArrayBinarySearchRange(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), chaos_arg_3);",
                        ]);
                        return new GenericShapeResolution(src4, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt == "System.Int32" || pt.Contains("System.Int32"))
                            abiSlots.Add(CreateInt32AbiSlot());
                        else
                            abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === List<T>::ToArray (stub call — uses GC allocation, kept in collection_stubs.cpp) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "ToArray",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return CollectionListToArray(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // === ReadOnlySpan<T>::ToArray (GenericShapeDescriptor -- handles generic ReadOnlySpan<T>) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.ReadOnlySpan",
                MethodName: "ToArray",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices);
                }));

            // === Array::IndexOf (GenericShapeDescriptor -- calls ChaosArrayIndexOf for standard 2-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "IndexOf",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosArrayIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Array::LastIndexOf (GenericShapeDescriptor -- calls ChaosArrayLastIndexOf for standard 2-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "LastIndexOf",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcLastIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosArrayLastIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcLastIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcLastIdxFb = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(srcLastIdxFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::IsSubclassOf ===
            registry.Register("System.Type", "IsSubclassOf", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsSubclassOf",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Dictionary<K,V>::get_Count (GenericShapeDescriptor -- calls CollectionDictionaryGetCount) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "get_Count",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return CollectionDictionaryGetCount(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Dictionary<K,V>::Add (GenericShapeDescriptor -- calls CollectionDictionaryAdd) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "Add",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    CollectionDictionaryAdd(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

            // === Dictionary<K,V>::TryGetValue (GenericShapeDescriptor -- calls CollectionDictionaryTryGetValue) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "TryGetValue",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    return CollectionDictionaryTryGetValue(chaos_arg_0, chaos_arg_1, reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_arg_2));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

            // === Dictionary<K,V>::ContainsKey (GenericShapeDescriptor -- calls CollectionDictionaryContainsKey) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "ContainsKey",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return CollectionDictionaryContainsKey(chaos_arg_0, chaos_arg_1);",
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

            // === Dictionary<K,V>::Remove (GenericShapeDescriptor -- calls CollectionDictionaryRemove) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "Remove",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return CollectionDictionaryRemove(chaos_arg_0, chaos_arg_1);",
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

            // === HashSet<T>::Add (GenericShapeDescriptor -- calls CollectionHashSetAdd) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
                MethodName: "Add",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return CollectionHashSetAdd(chaos_arg_0, chaos_arg_1);",
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

            // === HashSet<T>::Contains (GenericShapeDescriptor -- calls CollectionHashSetContains) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
                MethodName: "Contains",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return CollectionHashSetContains(chaos_arg_0, chaos_arg_1);",
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

            // === HashSet<T>::Remove (GenericShapeDescriptor -- calls CollectionHashSetRemove) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
                MethodName: "Remove",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === CultureInfo::get_DateTimeFormat ===
            registry.Register("System.Globalization.CultureInfo", "get_DateTimeFormat", [],
                ShapeKind.SimpleForward, "ChaosCultureGetDateTimeFormat",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Random::NextBytes ===
            registry.Register("System.Random", "NextBytes", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosRandomNextBytes",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Math::Max (SimpleForward stubs) ===
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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // === Assembly::get_Location ===
            registry.Register("System.Reflection.Assembly", "get_Location", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyLocation",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::get_IsGenericType ===
            registry.Register("System.Type", "get_IsGenericType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsGenericType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === MethodBase::get_IsPublic ===
            registry.Register("System.Reflection.MethodBase", "get_IsPublic", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === Module::GetTypes ===
            registry.Register("System.Reflection.Module", "GetTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionModuleGetTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === ParameterInfo::get_Attributes ===
            registry.Register("System.Reflection.ParameterInfo", "get_Attributes", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParamAttributes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Type::get_IsAbstract ===
            registry.Register("System.Type", "get_IsAbstract", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsAbstract",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // === RuntimeWrappedException::get_WrappedException ===
            registry.Register("System.Runtime.CompilerServices.RuntimeWrappedException", "get_WrappedException", [],
                ShapeKind.SimpleForward, "ChaosRuntimewrappedGetWrappedException",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Memory<T>::get_Span (GenericShapeDescriptor -- handles generic Memory<T>) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Memory",
                MethodName: "get_Span",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === String::Format (GenericShapeDescriptor -- dispatches by overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Format",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var stringRetAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);
                    var stringRefAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);
                    var objectRefAbi = CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType);

                    // Format(string) — return format string unchanged
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    return chaos_arg_0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(stringRefAbi),
                            stringRetAbi, new HashSet<int> { 0 });
                    }

                    // Format(string, object[]) — the core overload
                    if (paramTypes.Count == 2 && paramTypes[1].Contains("Object["))
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringFormatArray(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                stringRefAbi, objectRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1 });
                    }

                    // Format(string, object) — single arg convenience overload
                    if (paramTypes.Count == 2)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringFormat1(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                stringRefAbi, objectRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1 });
                    }

                    // Format(string, object, object) — two arg convenience overload
                    if (paramTypes.Count == 3)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        [
                            "    return ChaosStringFormat2(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                stringRefAbi, objectRefAbi, objectRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1, 2 });
                    }

                    // Fallback: unknown overload (IFormatProvider variants etc.) — return 0 stub
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt == "System.String" || pt.Contains("System.String"))
                            abiSlots.Add(stringRefAbi);
                        else
                            abiSlots.Add(objectRefAbi);
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFb = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(srcFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        stringRetAbi,
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Interlocked::MemoryBarrier ===
            registry.Register("Interlocked", "MemoryBarrier", [],
                ShapeKind.SimpleForward, "ChaosInterlockedMemoryBarrier",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);


            // === Interlocked::Read ===
            registry.Register("Interlocked", "Read", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedReadInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // === Interlocked::Increment ===
            registry.Register("Interlocked", "Increment", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedIncrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // === Interlocked::Decrement ===
            registry.Register("Interlocked", "Decrement", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedDecrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // === Interlocked::Exchange ===
            registry.Register("Interlocked", "Exchange", ["System.Int64&", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // === Interlocked::CompareExchange (Int32) ===
            registry.Register("Interlocked", "CompareExchange", ["System.Int32&", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Interlocked::CompareExchange (Int64) ===
            registry.Register("Interlocked", "CompareExchange", ["System.Int64&", "System.Int64", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

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
                Resolver: static (planner, callee, typeArgs) =>
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
                            new HashSet<int> { 0 });
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
                Resolver: static (planner, callee, typeArgs) =>
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
                            new HashSet<int> { 0 });
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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
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
                Resolver: static (planner, callee, typeArgs) =>
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

            // === Array::GetValue (GenericShapeDescriptor -- calls ChaosArrayGetValue for 2-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "GetValue",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var srcGetVal = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    return ChaosArrayGetValue(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcGetVal, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcGetValFb = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(srcGetValFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === String::Replace (GenericShapeDescriptor -- calls ChaosStringReplace for 3-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Replace",
                Resolver: static (planner, callee, typeArgs) =>
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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // === HashCode::Combine<T,T> (GenericShapeDescriptor -- handles all instantiations) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "Combine",
                Resolver: static (planner, callee, typeArgs) =>
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

            // === FieldInfo/MethodBase::get_IsStatic (stub returning false; broad prefix covers all System.Reflection.*::get_IsStatic) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.",
                MethodName: "get_IsStatic",
                Resolver: static (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
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
                Resolver: static (planner, callee, typeArgs) =>
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


            // === Marshal.GetFunctionPointerForDelegate<T>(T) — returns native function pointer via thunk registry ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetFunctionPointerForDelegate",
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
            registry.Register("System.Runtime.InteropServices.Marshal", "CreateCcw", ["System.IntPtr", "System.IntPtr"],
                ShapeKind.SimpleForward, "MarshalCreateCcw",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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
                Resolver: static (planner, callee, typeArgs) =>
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

            return registry;
        }

        private static string GetTypeDisplayNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            var typePart = separatorIndex >= 0 ? subjectId[..separatorIndex] : subjectId;
            var slashIndex = typePart.IndexOf('/');
            return slashIndex >= 0 ? typePart[(slashIndex + 1)..] : typePart;
        }

        private static string GetMethodNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            if (separatorIndex < 0) return string.Empty;
            var afterSeparator = subjectId[(separatorIndex + 2)..];
            var colonIndex = afterSeparator.IndexOf(':');
            var parenIndex = afterSeparator.IndexOf('(');
            if (colonIndex >= 0 && parenIndex >= 0)
            {
                return afterSeparator[..Math.Min(colonIndex, parenIndex)];
            }
            if (colonIndex >= 0)
            {
                return afterSeparator[..colonIndex];
            }
            return parenIndex >= 0 ? afterSeparator[..parenIndex] : afterSeparator;
        }

        private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectId(string subjectId)
        {
            var parenOpen = subjectId.IndexOf('(');
            var parenClose = subjectId.LastIndexOf(')');
            if (parenOpen < 0 || parenClose <= parenOpen) return [];
            var paramsPart = subjectId[(parenOpen + 1)..parenClose];
            if (string.IsNullOrEmpty(paramsPart)) return [];
            return paramsPart.Split(',');
        }

        /// <summary>Map Convert.ToChar parameter type to the corresponding chaos_convert_tochar_* native function.</summary>
        private static string GetToCharNativeSymbol(string paramType)
        {
            return paramType switch
            {
                "System.Boolean" => "chaos_convert_tochar_boolean",
                "System.Byte" => "chaos_convert_tochar_byte",
                "System.Char" => "chaos_convert_tochar_char",
                "System.DateTime" => "chaos_convert_tochar_datetime",
                "System.Decimal" => "chaos_convert_tochar_decimal",
                "System.Double" => "chaos_convert_tochar_double",
                "System.Int16" => "chaos_convert_tochar_int16",
                "System.Int32" => "chaos_convert_tochar_int32",
                "System.Int64" => "chaos_convert_tochar_int64",
                "System.Object" => "chaos_convert_tochar_object",
                "System.SByte" => "chaos_convert_tochar_sbyte",
                "System.Single" => "chaos_convert_tochar_single",
                "System.String" => "chaos_convert_tochar_string",
                "System.UInt16" => "chaos_convert_tochar_uint16",
                "System.UInt32" => "chaos_convert_tochar_uint32",
                "System.UInt64" => "chaos_convert_tochar_uint64",
                _ => "chaos_convert_tochar_int32",
            };
        }

        private static string SanitizeForEnumName(string name)
        {
            var sb = new StringBuilder(name.Length);
            foreach (var ch in name)
                sb.Append(char.IsAsciiLetterOrDigit(ch) || ch == '_' ? ch : '_');
            return sb.ToString().ToUpperInvariant();
        }

        /// <summary>Parse generic type arguments from a type name suffix like "`1[[System.Int32]]".</summary>
        private static bool TryParseGenericTypeArgsFromTypeName(
            string genericSuffix,
            [NotNullWhen(true)] out IReadOnlyList<string>? typeArgs)
        {
            typeArgs = null;
            // Format: `1[[System.Int32]] or `2[[System.Int32],[System.String]]
            if (genericSuffix.Length < 2 || genericSuffix[0] != '`') return false;

            var bracketStart = genericSuffix.IndexOf("[[", StringComparison.Ordinal);
            if (bracketStart < 0) return false;

            var inner = genericSuffix.Substring(bracketStart + 2);
            var bracketEnd = inner.LastIndexOf("]]", StringComparison.Ordinal);
            if (bracketEnd < 0) return false;

            inner = inner.Substring(0, bracketEnd);
            if (string.IsNullOrEmpty(inner))
            {
                typeArgs = Array.Empty<string>();
                return true;
            }

            var args = new List<string>();
            var depth = 0;
            var start = 0;
            for (var i = 0; i < inner.Length; i++)
            {
                if (inner[i] == '[') depth++;
                else if (inner[i] == ']') depth--;
                else if (inner[i] == ',' && depth == 0)
                {
                    args.Add(inner.Substring(start, i - start));
                    start = i + 1;
                }
            }
            if (start < inner.Length)
                args.Add(inner.Substring(start));

            typeArgs = args;
            return args.Count > 0;
        }

        /// <summary>
        /// Extract the delegate Invoke parameter count from the delegate type's subjectId.
        /// For Func`N types: N-1 parameters (one type arg is return type).
        /// For Action`N types: N parameters.
        /// For other delegate types: returns 0 (caller must handle).
        /// </summary>
        private static int ExtractDelegateArity(string delegateTypeSubjectId)
        {
            // SubjectId format: "Assembly/TypeName`N[[...]]"
            var slash = delegateTypeSubjectId.IndexOf('/');
            if (slash < 0) return 0;
            var typeName = delegateTypeSubjectId.Substring(slash + 1);

            // Check for backtick-arity suffix: TypeName`N
            var backtick = typeName.IndexOf('`');
            if (backtick < 0) return 0;

            var arityStr = string.Empty;
            for (int i = backtick + 1; i < typeName.Length; i++)
            {
                if (char.IsDigit(typeName[i]))
                    arityStr += typeName[i];
                else
                    break;
            }
            if (string.IsNullOrEmpty(arityStr) || !int.TryParse(arityStr, out int arity))
                return 0;

            // Extract display name before backtick for Func/Action detection.
            var baseName = typeName.Substring(0, backtick);
            // Get the segment after the last '.' for short name.
            var lastDot = baseName.LastIndexOf('.');
            var shortName = lastDot >= 0 ? baseName.Substring(lastDot + 1) : baseName;

            return shortName switch
            {
                "Func" when arity >= 1 => arity - 1,  // One type arg is return type
                "Action" => arity,                     // All type args are parameters
                _ => 0,  // Custom delegate type — arity unknown from name alone
            };
        }
    }
}

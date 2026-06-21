using System.Linq;
using System.Reflection;
using System.Text.Json;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

public sealed class DllScanner
{
    private static readonly HashSet<string> ObjectMethods = new(StringComparer.Ordinal)
    {
        "ToString", "Equals", "GetHashCode", "Finalize", "MemberwiseClone", "GetType"
    };

    /// <summary>Methods blacklisted from ATG probing regardless of codegen capabilities.</summary>
    private static readonly HashSet<string> BaseUnprobableMethods = new(StringComparer.Ordinal)
    {
        "GetObjectData",          // requires SerializationInfo setup
        "OnDeserialization",      // requires deserialization infrastructure
        "CompileToAssembly",      // uses RegexCompilationInfo (removed in .NET 10+)
        "GetAlternateLookup",     // MLC leak from AlternateLookup nested type
        "TryGetAlternateLookup",  // MLC leak from AlternateLookup nested type
        "SetEntryAssembly",       // internal API not in reference assemblies
        "StartDeserialization",   // returns DeserializationToken (internal type not in ref assemblies)
        "Invoke",                 // MLC delegate leak: nested delegate Invoke flattened onto parent type
        "BeginInvoke",            // MLC delegate leak: BeginInvoke from delegate pattern
        "EndInvoke",              // MLC delegate leak: EndInvoke from delegate pattern
        // CS0315: concretized <int,int> doesn't satisfy the interface constraint on TAccessor
        "TryGetMemoryManager",
        // CS1061: ILGenerator.MarkSequencePoint not in reference assemblies.
        "MarkSequencePoint",
        // CS0117: Task.WhenEach is a .NET 9 API not in ref assemblies building combined subjects DLL.
        "WhenEach",
        // CS1615: Marshal.WriteInt16(object, int, char)
        "WriteInt16",
        // CS0117: RuntimeHelpers.Box/RuntimeHelpers.SizeOf are .NET 9 APIs
        "Box",
        "SizeOf",
        // CS1061: ModuleBuilder.DefineDocument is .NET 9
        "DefineDocument",
        // CS0117: .NET 9 Vector<T> SIMD APIs
        "AsVector4", "AsVector4Unsafe", "AsPlane", "AsQuaternion",
        "AsVector2", "AsVector3",
        // CS0019: Vector.Clamp/Normalize/Lerp etc return value type tuples
        "Clamp", "Normalize", "Lerp", "Reflect", "Abs", "Min", "Max", "Sum",
        "Distance", "DistanceSquared", "Dot", "Transform",
        // CS0117: Vector.WidenLower/WidenUpper are .NET 9 APIs
        "WidenLower", "WidenUpper",
        // CS0117: Vector.Create<int>(int) is .NET 9
        "Create",
        // Hex string formatting
        "ToHexStringLower", "TryToHexStringLower",
        // Half math methods: JsonReaderException from System.Text.Json
        "Acosh", "Ieee754Remainder", "Log", "Log10", "Log2",
        "ReciprocalEstimate", "ReciprocalSqrtEstimate", "RootN",
        // MultiplyAddEstimate is .NET 9
        "MultiplyAddEstimate",
        // CS0117: TypeDescriptor .NET 9+ APIs
        "GetConverterFromRegisteredType", "GetEventsFromRegisteredType",
        "GetPropertiesFromRegisteredType",
        // CS1061: GetArgument() is .NET 9 on expression tree types
        "GetArgument",
    };

    /// <summary>Methods gated by codegen capabilities. Key = method name, Value = capabilities.json feature flag.</summary>
    private static readonly Dictionary<string, string> CapabilityGatedMethods = new(StringComparer.Ordinal)
    {
        // Async state machine methods: gated by "async_methods" capability.
        // When codegen supports async (features.async_methods = true), these are unblocked.
        ["AwaitOnCompleted"] = "async_methods",
        ["AwaitUnsafeOnCompleted"] = "async_methods",
        ["Start"] = "async_methods",
        ["MoveNext"] = "async_methods",
    };

    private string? _capabilitiesPath;
    private HashSet<string>? _effectiveBlacklist;
    private CapabilitiesFile? _capabilities;

    public DllScanner(string? capabilitiesPath = null)
    {
        _capabilitiesPath = capabilitiesPath;
    }

    /// <summary>Get the effective blacklist, applying capabilities filtering.</summary>
    private HashSet<string> GetEffectiveUnprobableMethods()
    {
        if (_effectiveBlacklist != null) return _effectiveBlacklist;

        var result = new HashSet<string>(BaseUnprobableMethods, StringComparer.Ordinal);

        // Load capabilities if available.
        if (_capabilities == null && _capabilitiesPath != null && File.Exists(_capabilitiesPath))
        {
            try
            {
                string json = File.ReadAllText(_capabilitiesPath);
                _capabilities = JsonSerializer.Deserialize<CapabilitiesFile>(json);
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"[ATG] Failed to load capabilities.json: {ex.Message}");
            }
        }

        // Add capability-gated methods only if their feature is NOT supported.
        bool asyncSupported = _capabilities?.Codegen?.Features?.AsyncMethods == true;
        foreach (var kvp in CapabilityGatedMethods)
        {
            bool isSupported = kvp.Value switch
            {
                "async_methods" => asyncSupported,
                _ => false,
            };
            if (!isSupported)
                result.Add(kvp.Key);
        }

        _effectiveBlacklist = result;
        return result;
    }

    // Capabilities JSON model
    private class CapabilitiesFile
    {
        public int Version { get; set; }
        public CapabilitiesCodegen? Codegen { get; set; }
    }

    private class CapabilitiesCodegen
    {
        public CapabilitiesFeatures? Features { get; set; }
    }

    private class CapabilitiesFeatures
    {
        [System.Text.Json.Serialization.JsonPropertyName("async_methods")]
        public bool AsyncMethods { get; set; }
    }

    // Types that require complex infrastructure (e.g. JsonSerializerOptions) and
    // can't produce meaningful results in isolated auto-generated tests.
    // NOTE: Same capabilities.json dependency as UnprobableMethods.
    // When codegen supports a type pattern, remove the corresponding entry.
    private static readonly HashSet<string> UnprobableTypeNames = new(StringComparer.Ordinal)
    {
        "System.Text.Json.Serialization.Metadata.JsonTypeInfo",
        "System.Text.Json.Serialization.Metadata.JsonPropertyInfo",
        "System.Text.Json.Serialization.Metadata.JsonParameterInfo",
        // Types present in the implementation DLL but not in the target framework's reference assemblies.
        // These cause CS0234 errors when the combined subjects project compiles against reference APIs.
        "System.Buffers.StringSearchValuesHelper",
        "System.Buffers.Text.Base64Url",
        // Debugger types: Launch()/Break() interact with debugger UI and hang probes.
        "System.Diagnostics.Debugger",
        // BCL internal implementation types — public in System.Private.CoreLib
        // but absent from reference assemblies, causing CS0234 at compile time.
        "System.OrdinalComparer",
        "System.CultureAwareComparer",
        "System.UnitySerializationHolder",
        // MLC-flattened nested types: MLC reports these as top-level public types
        // but they're actually internal nested types inside Comparer<T> or
        // EqualityComparer<T>.  They can't be referenced from C# as standalone types
        // (CS0234: "type does not exist in namespace").
        "System.Collections.Generic.ByteEqualityComparer",
        "System.Collections.Generic.EnumEqualityComparer`1",
        "System.Collections.Generic.GenericComparer`1",
        "System.Collections.Generic.GenericEqualityComparer`1",
        "System.Collections.Generic.NullableComparer`1",
        "System.Collections.Generic.NullableEqualityComparer`1",
        "System.Collections.Generic.ObjectComparer`1",
        "System.Collections.Generic.ObjectEqualityComparer`1",
        // Types in System.Private.CoreLib implementation that don't exist in
        // reference assemblies (CS0234 in combined subjects DLL build).
        "System.Collections.Generic.NonRandomizedStringEqualityComparer",
        "System.Diagnostics.DebugProvider",
        "System.Diagnostics.DiagnosticMethodInfo",
        // MLC-exposed internal type: ListDictionaryInternal is an implementation detail
        // absent from reference assemblies (CS0234).
        "System.Collections.ListDictionaryInternal",
        // Type forwarding mismatch: MLC resolves AssemblyHashAlgorithm as
        // System.Configuration.Assemblies.AssemblyHashAlgorithm, but reference assemblies
        // have it as System.Reflection.AssemblyHashAlgorithm (CS1503).
        "System.Configuration.Assemblies.AssemblyHashAlgorithm",
        // MLC-exposed internal type: DeserializationToken is returned by
        // SerializationInfo.StartDeserialization() but doesn't exist in reference assemblies.
        "System.Runtime.Serialization.DeserializationToken",
        // MLC-exposed internal type aliases: Internal.Console and its nested Error type
        // are internal implementation details in System.Private.CoreLib. MLC exposes
        // them as public types in the "Internal" namespace, but "Internal" is not a
        // real namespace in any reference assembly, causing CS0400 at compile time.
        // Note: MLC uses "+" as nested type separator in FullName.
        "Internal.Console",
        "Internal.Console+Error",
        // .NET 9 API not available in reference assemblies: Task.WhenEach
        // returns IAsyncEnumerable<T> via the System.Linq.AsyncEnumerable helper type.
        "System.Linq.AsyncEnumerable",
        // .NET 9 compiler infrastructure types: PoolingAsyncValueTaskMethodBuilder
        // is used internally by the compiler for async ValueTask methods but
        // not available in .NET 8 reference assemblies (CS0234).
        "System.Runtime.CompilerServices.PoolingAsyncValueTaskMethodBuilder",
        "System.Runtime.CompilerServices.PoolingAsyncValueTaskMethodBuilder`1",
        // Type whose methods reference System.Linq.AsyncEnumerable API (.NET 9) in
        // expression-builder-generated code (CS0234 when building combined subjects DLL).
        "System.Threading.Tasks.TaskAsyncEnumerableExtensions",
        // NFloat is a .NET 8+ struct used for interop (maps to C float/double based on
        // platform). Its ConvertToInteger / MultiplyAddEstimate are .NET 9 APIs not
        // available in reference assemblies for the combined subjects DLL.
        "System.Runtime.InteropServices.NFloat",
        // ContractHelper.RaiseContractFailedEvent references ContractFailureKind enum
        // which doesn't exist in reference assemblies (CS0246).
        "System.Runtime.CompilerServices.ContractHelper",
        // JsonReaderException is specific to System.Text.Json
        "System.Text.Json.JsonReaderException",
        // .NET 9 COM marshalling implementation types absent from reference assemblies.
        "System.Runtime.InteropServices.Marshalling.ComVariantMarshaller",
        "System.Runtime.InteropServices.Marshalling.ComVariant",
        // DLR internal implementation types: RuntimeOps and CallSiteOps are
        // in System.Runtime.CompilerServices namespace but absent from reference
        // assemblies (CS0234 when used as parameter/return types in combined subjects).
        "System.Runtime.CompilerServices.RuntimeOps",
        "System.Runtime.CompilerServices.CallSiteOps",
        // System.Xml.Xsl.Runtime types exist only in System.Private.Xml.dll (the
        // internal implementation assembly) but NOT in the System.Xml.ReaderWriter
        // facade/reference assembly.  The ATG discovers them via MLC scanning of
        // System.Private.Xml.dll, but they can never compile in the combined subjects
        // DLL because no reference assembly exposes them — CS0433 if we add a direct
        // ref (type exists in both System.Private.Xml and the facade), CS0234 if not.
        "System.Xml.Xsl.Runtime",
        // System.Net.WebUtility methods (HtmlEncode/Decode, UrlEncode/Decode) are
        // complex string manipulation routines that reference internal .NET types
        // (e.g. URI parser internals) which the AOT codegen cannot lower.  The
        // generated hotpatch entries are empty ({0u,0u}), causing crashes at runtime.
        // Excluding from ATG probing avoids false-positive fact failures.
        // NOTE: Runtime stubs were added (ChaosWebUtility* in web_stubs.h/cpp) but
        // the managed methods call internal helpers that the AOT codegen can't resolve.
        // Remove this exclusion when the codegen handles the full internal call chain.
        "System.Net.WebUtility",
    };

    // Marshaller type names that fail C# compilation when concretized.
    // Checked against t.Name in ListPublicTypesCore because MLC's FullName
    // format for these generic types doesn't reliably match backtick notation.
    private static readonly HashSet<string> UnprobableMarshallerTypeNames = new(StringComparer.Ordinal)
    {
        "ArrayMarshaller",
        "PointerArrayMarshaller",
        "ReadOnlySpanMarshaller",
        "SpanMarshaller",
        "SafeHandleMarshaller",
        "ConfiguredCancelableAsyncEnumerable",
        // Generic awaitable types: methods like OnCompleted/UnsafeOnCompleted
        // are on the nested awaiter, but the expression builder misattributes
        // them to the awaitable. Only the generic versions (ConfiguredTaskAwaitable`1)
        // cause this; the non-generic versions work fine.
        "ConfiguredTaskAwaitable",
        "ConfiguredValueTaskAwaitable",
    };

    public DllScanResult Scan(string dllPath, string typeFullName)
    {
        var assemblyName = Path.GetFileNameWithoutExtension(dllPath);
        var fullPath = Path.GetFullPath(dllPath);

        var probeDirs = GetProbeDirectories(fullPath);
        var resolver = new AssemblyResolver(probeDirs);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");

        var assembly = mlc.LoadFromAssemblyPath(fullPath);
        return ScanInContext(mlc, assembly, assemblyName, typeFullName);
    }

    /// <summary>
    /// Scan a single type using a pre-loaded MetadataLoadContext and Assembly.
    /// Allows batch scanning (from ScanAll) to reuse one MLC across many types.
    /// </summary>
    private DllScanResult ScanInContext(
        MetadataLoadContext mlc, Assembly assembly, string assemblyName, string typeFullName)
    {
        // Read target framework from assembly metadata via MLC
        var tfm = "";
        try
        {
            var tfaData = assembly.GetCustomAttributesData()
                .FirstOrDefault(a => a.AttributeType.Name == "TargetFrameworkAttribute");
            if (tfaData?.ConstructorArguments is { Count: > 0 } args)
                tfm = TfmFromMoniker(args[0].Value as string ?? "");
        }
        catch { /* best-effort — fall back to net8.0 */ }

        // Fallback: detect from DLL path or SDK
        if (string.IsNullOrEmpty(tfm))
            tfm = DetectTfm(assembly.Location);

        var targetType = assembly.GetType(typeFullName);
        if (targetType is null)
            throw new InvalidOperationException(
                $"Type '{typeFullName}' not found in assembly '{assemblyName}'. " +
                $"Available types: {string.Join(", ", assembly.GetTypes().Take(20).Select(t => t.FullName))}");

        // ── Skip interface types ──
        // Interface methods (instance abstract, static virtual) cannot be invoked
        // without a concrete implementation. Concretizing an interface like
        // IFloatingPointIeee754<T> → IFloatingPointIeee754<double> would produce
        // C# code that fails with CS8926 (static virtual member on non-type-param).
        if (targetType.IsInterface)
            throw new InvalidOperationException(
                $"Interface type '{typeFullName}' is not supported. " +
                "Interface methods cannot be invoked without a concrete implementation.");

        // ── Concretize generic type definitions (e.g. Stack`1 → Stack<int>) ──
        if (targetType.IsGenericTypeDefinition)
        {
            if (TryConcretizeGenericType(mlc, targetType, out var concreteType))
            {
                targetType = concreteType;
                // Clean up the assembly-qualified FullName for downstream C# generation
                typeFullName = CSharpSerializer.StripAssemblyQualification(
                    concreteType.FullName ?? typeFullName);
            }
            else
            {
                throw new InvalidOperationException(
                    $"Cannot concretize generic type definition '{typeFullName}'. " +
                    "Generic type definitions are not directly supported.");
            }
        }

        // ── Skip generic types where a type argument violates new() constraint ──
        // Some constructed generic types in the assembly use abstract types as type
        // arguments for generic parameters with 'new()' constraint (e.g.,
        // ReferenceHandler<ReferenceResolver> where ReferenceResolver is abstract).
        // C# requires all type arguments to satisfy constraints even for default(T)!.
        if (targetType.IsGenericType && !targetType.IsGenericTypeDefinition)
        {
            var gtd = targetType.GetGenericTypeDefinition();
            var genericArgs = targetType.GetGenericArguments();
            var genericParams = gtd.GetGenericArguments();
            for (int i = 0; i < genericParams.Length; i++)
            {
                var attrs = genericParams[i].GenericParameterAttributes;
                if (attrs.HasFlag(GenericParameterAttributes.DefaultConstructorConstraint))
                {
                    if (genericArgs[i] is { IsAbstract: true, IsInterface: false })
                    {
                        throw new InvalidOperationException(
                            $"Type '{typeFullName}' has new() constraint on type argument " +
                            $"'{genericArgs[i].FullName}' which is abstract. Skipping.");
                    }
                }
            }
        }

        var signatures = new List<MethodSignature>();
        var skippedMethods = new List<string>();

        foreach (var rawMethod in targetType.GetMethods(
            BindingFlags.Public | BindingFlags.Static |
            BindingFlags.Instance | BindingFlags.DeclaredOnly))
        {
            // MLC sometimes leaks interface methods on closed generic types
            // even with DeclaredOnly (e.g., List<int> appears to have MoveNext
            // from List<int>.Enumerator). Skip methods whose declaring type
            // is an interface — they can't be directly invoked on the target.
            if (rawMethod.DeclaringType is not null && rawMethod.DeclaringType.IsInterface)
                continue;

            // Skip property accessors and operator overloads
            if (rawMethod.IsSpecialName && (
                    rawMethod.Name.StartsWith("get_") ||
                    rawMethod.Name.StartsWith("set_") ||
                    rawMethod.Name.StartsWith("add_") ||
                    rawMethod.Name.StartsWith("remove_") ||
                    rawMethod.Name.StartsWith("op_")))
                continue;

            // Skip CastUp/CastDown on Span/ReadOnlySpan — they require specific
            // type hierarchy relationships (TDerived : T) that general concretization
            // can't satisfy (e.g. CastUp<string> on ReadOnlySpan<int> — string doesn't
            // derive from int).
            if (rawMethod.Name is "CastUp" or "CastDown")
            {
                var dt = rawMethod.DeclaringType;
                if (dt is not null && (
                        dt.FullName is "System.Span`1" or "System.ReadOnlySpan`1" ||
                        dt.Name is "Span`1" or "ReadOnlySpan`1"))
                {
                    skippedMethods.Add($"{rawMethod.Name} (Span type hierarchy)");
                    continue;
                }
            }

            // Skip interface-leaked enumerator methods on ref struct types.
            // MLC sometimes leaks IEnumerator.MoveNext/Dispose/Reset onto
            // types like Span<T>, ReadOnlySpan<T>, ArraySegment<T> that use
            // a custom GetEnumerator pattern and have a direct MoveNext().
            // Check by method name + parameterless pattern.
            if (rawMethod.GetParameters().Length == 0 &&
                (rawMethod.Name == "MoveNext" || rawMethod.Name == "Dispose" || rawMethod.Name == "Reset") &&
                targetType.GetNestedTypes(BindingFlags.Public | BindingFlags.NonPublic)
                    .Any(nt => nt.Name.Contains("Enumerator")))
            {
                skippedMethods.Add($"{rawMethod.Name} (enumerator leak - target has nested Enumerator type)");
                continue;
            }

            if (rawMethod is { Name: "MoveNext" or "Reset" } &&
                rawMethod.GetParameters().Length == 0)
            {
                // MoveNext/Reset with 0 params are NEVER legitimate methods on
                // non-enumerator types.  MLC always projects them from an
                // enumerator implementation onto the enclosing collection type
                // (List<T>, Dictionary<TKey,TValue>, HashSet<T>, Queue<T>, etc.).
                skippedMethods.Add($"{rawMethod.Name} (enumerator method projected by MLC)");
                continue;
            }
            if (rawMethod is { Name: "Dispose" } && rawMethod.GetParameters().Length == 0)
            {
                // MLC leaks Enumerator.Dispose() onto generic collection types
                // (List<T>, Dictionary<TKey,TValue>, HashSet<T>, Queue<T>) even
                // with DeclaredOnly flag.  Skip parameterless Dispose() for these
                // known affected types (they don't have Dispose() in reality).
                if (targetType.IsGenericType)
                {
                    var defFull = targetType.GetGenericTypeDefinition().FullName;
                    if (defFull is "System.Collections.Generic.List`1" or
                        "System.Collections.Generic.Dictionary`2" or
                        "System.Collections.Generic.HashSet`1" or
                        "System.Collections.Generic.Queue`1" or
                        "System.Collections.Generic.Stack`1" or
                        "System.Collections.Generic.LinkedList`1")
                    {
                        skippedMethods.Add($"{rawMethod.Name} (enumerator leak on {targetType.Name})");
                        continue;
                    }
                }
                // Skip parameterless Dispose if the target type IS an Enumerator nested type
                // (e.g. List<int>.Enumerator.Dispose). These are collection implementation
                // details that produce useless benchmarks and cause CS1061 in combined builds.
                if (targetType.Name.Contains("Enumerator"))
                {
                    skippedMethods.Add($"{rawMethod.Name} (enumerator type itself)");
                    continue;
                }
                // Existing checks for interface/grandparent/declaring-type mismatch
                if (rawMethod.DeclaringType is not null &&
                    (rawMethod.DeclaringType.IsInterface ||
                     rawMethod.DeclaringType.FullName is "System.Span`1" or "System.ReadOnlySpan`1" or "System.ArraySegment`1"))
                {
                    skippedMethods.Add($"{rawMethod.Name} (interface leak)");
                    continue;
                }
                if (rawMethod.DeclaringType?.DeclaringType is { } grandParent)
                {
                    var gpFull = grandParent.IsGenericType
                        ? grandParent.GetGenericTypeDefinition().FullName
                        : grandParent.FullName;
                    if (gpFull is "System.Span`1" or "System.ReadOnlySpan`1" or "System.ArraySegment`1")
                    {
                        skippedMethods.Add($"{rawMethod.Name} (nested enumerator leak)");
                        continue;
                    }
                }
                if (rawMethod.DeclaringType is not null && rawMethod.DeclaringType != targetType)
                {
                    skippedMethods.Add($"{rawMethod.Name} (MLC enumerator leak on {targetType.Name})");
                    continue;
                }
            }

            // Skip parameterless Dispose() on MemoryHandle — a default-initialized
            // MemoryHandle has null internal fields (void* _pointer, object _owner)
            // and Dispose() crashes (NullReferenceException / SIGSEGV) when called on
            // the default value.  This is a runtime behavior issue that can't be fixed
            // at the stub level.
            if (rawMethod is { Name: "Dispose" } && rawMethod.GetParameters().Length == 0 &&
                rawMethod.DeclaringType?.FullName == "System.Buffers.MemoryHandle")
            {
                skippedMethods.Add($"{rawMethod.Name} (MemoryHandle default crash)");
                continue;
            }

            // Skip string.Trim/TrimStart/TrimEnd with ReadOnlySpan<char> parameter.
            // MLC reports the ReadOnlySpan<char> overload but C# resolves to Trim(char),
            // causing CS1503: "cannot convert from ReadOnlySpan<char> to char".
            if (rawMethod.Name is "Trim" or "TrimStart" or "TrimEnd")
            {
                var hasSpanParam = rawMethod.GetParameters().Any(p =>
                {
                    var pt = p.ParameterType.IsByRef ? p.ParameterType.GetElementType()! : p.ParameterType;
                    return (pt.FullName ?? pt.Name)?.Contains("ReadOnlySpan") == true;
                });
                if (hasSpanParam)
                {
                    skippedMethods.Add($"{rawMethod.Name} (ReadOnlySpan<char> overload)");
                    continue;
                }
            }

            // Skip CopyTo methods where the first parameter is a simple type array
            // (e.g., int[]) rather than the expected collection element type array
            // (e.g., KeyValuePair<TKey,TValue>[]). MLC leaks ICollection.CopyTo(Array, int)
            // from non-generic interface and resolves Array to the wrong concrete type.
            if (rawMethod.Name == "CopyTo" && rawMethod.GetParameters().Length == 2)
            {
                var pt0 = rawMethod.GetParameters()[0].ParameterType;
                if (pt0.IsArray)
                {
                    var elemType = pt0.GetElementType();
                    if (elemType is not null && !elemType.Name.Contains("KeyValuePair"))
                    {
                        skippedMethods.Add($"{rawMethod.Name} (ICollection non-generic leak: {elemType.Name}[])");
                        continue;
                    }
                }
            }

            // Skip TrimExcess with parameters — the real API is parameterless.
            // MLC leaks an internal TrimExcess(int capacity) overload.
            if (rawMethod.Name == "TrimExcess" && rawMethod.GetParameters().Length > 0)
            {
                skippedMethods.Add($"{rawMethod.Name} (parameterized internal overload)");
                continue;
            }

            // Dictionary<K,V>.Contains: MLC leaks the explicit interface implementation
            // ICollection<KeyValuePair<K,V>>.Contains(KVP) as a public "Contains" method,
            // but it's not directly callable on a Dictionary reference without casting to
            // the interface. The real public methods are ContainsKey and ContainsValue.
            // Use typeFullName string check (not targetType.IsGenericType) because MLC
            // may not reliably report IsGenericType for concretized types in batch scan.
            if (rawMethod.Name == "Contains" &&
                typeFullName.StartsWith("System.Collections.Generic.Dictionary<", StringComparison.Ordinal))
            {
                Console.Error.WriteLine($"  [SKIP] Contains (explicit interface impl on {typeFullName})");
                skippedMethods.Add("Contains (explicit interface impl on Dictionary)");
                continue;
            }

            // Skip BinaryPrimitives.ReverseEndianness with Int128/UInt128 parameter.
            // The InlineShape handler returns null for Int128 (no CHAOS_IL2CPP_INT128 type),
            // and the fallback AOT codegen cannot produce correct 16-byte struct return ABI.
            // These are extreme edge cases that do not justify codegen-wide ABI work.
            if (rawMethod.Name == "ReverseEndianness")
            {
                var pt = rawMethod.GetParameters();
                if (pt.Length == 1)
                {
                    var pn = pt[0].ParameterType.FullName;
                    if (pn == "System.Int128" || pn == "System.UInt128")
                    {
                        skippedMethods.Add($"{rawMethod.Name} (Int128/UInt128 struct return ABI)");
                        continue;
                    }
                }
            }

            // Skip object inherited methods
            if (ObjectMethods.Contains(rawMethod.Name))
                continue;

            // Skip methods that need infrastructure not available in isolated tests
            if (GetEffectiveUnprobableMethods().Contains(rawMethod.Name))
            {
                skippedMethods.Add(rawMethod.Name);
                continue;
            }

            var method = rawMethod;

            // Skip methods on unprobable declaring types (e.g. ComVariantMarshaller
            // or its nested types like ComVariantMarshaller+RefPropagate).
            if (IsUnprobableType(method.DeclaringType!))
            {
                skippedMethods.Add($"{method.Name} (unprobable declaring type: {method.DeclaringType?.FullName ?? "?"})");
                continue;
            }

            // Try to concretize generic methods
            if (method.ContainsGenericParameters)
            {
                if (TryConcretizeGenericMethod(mlc, method, out var constructed))
                {
                    // Skip if concretization produced ref struct parameter types
                    // (Span, ReadOnlySpan) that can't work as generic placeholders
                    // and would produce invalid C# (e.g. Span<int>.MoveNext()).
                    if (ConcretizedToRefStruct(constructed))
                    {
                        skippedMethods.Add($"{method.Name} (ref struct concretization)");
                        continue;
                    }
                    method = constructed;
                }
                else
                {
                    skippedMethods.Add(method.Name);
                    continue;
                }
            }

            // Skip methods that still have unresolved generic type parameters in their
            // signature (e.g. static interface methods from generic math interfaces)
            if (HasUnresolvedGenericParameters(method))
            {
                skippedMethods.Add($"{method.Name} (unresolved generics)");
                continue;
            }

            // Capture generic type args for concretized generic methods
            IReadOnlyList<string>? genericTypeArgs = null;
            if (method.IsGenericMethod && !method.ContainsGenericParameters)
            {
                genericTypeArgs = method.GetGenericArguments()
                    .Select(a => GetTypeName(a))
                    .ToList();
            }

            var paramList = method.GetParameters();
            var parameters = new List<MethodParameter>();
            var hasRefParam = false;

            foreach (var p in paramList)
            {
                var isOut = p.IsOut;
                var isRef = !isOut && p.ParameterType.IsByRef;
                if (isRef) hasRefParam = true;

                var paramTypeName = GetTypeName(p.ParameterType);
                // Normalize: strip any assembly qualification that may leak from MLC
                paramTypeName = CSharpSerializer.StripAssemblyQualification(paramTypeName);

                // If GetTypeName returned a short name without namespace or generic args
                // for a non-primitive type, MLC likely couldn't resolve the type.
                // Skip this method to avoid generating invalid code.
                if (LooksIncomplete(paramTypeName))
                {
                    skippedMethods.Add($"{method.Name} (unresolved parameter type: {paramTypeName})");
                    parameters.Clear(); // discard partial param list
                    break;
                }

                // Skip methods with non-public parameter types (CS0122 in generated code)
                var rawParamType = p.ParameterType.IsByRef
                    ? p.ParameterType.GetElementType()!
                    : p.ParameterType;
                if (!rawParamType.IsGenericParameter && !rawParamType.IsVisible)
                {
                    skippedMethods.Add($"{method.Name} (internal parameter type: {paramTypeName})");
                    parameters.Clear();
                    break;
                }

                // Skip methods with parameter types that are known unprobable types
                // (present in System.Private.CoreLib but absent from reference assemblies).
                // Also checks nested type declaring chain (e.g. ComVariantMarshaller+RefPropagate).
                if (IsUnprobableType(rawParamType))
                {
                    skippedMethods.Add($"{method.Name} (unprobable parameter type: {rawParamType.FullName})");
                    parameters.Clear();
                    break;
                }

                var isRefStruct = IsRefStructType(p.ParameterType);
                if (isRefStruct)
                {
                    skippedMethods.Add($"{method.Name} (ref struct parameter: {paramTypeName})");
                    parameters.Clear();
                    break;
                }
                                parameters.Add(new MethodParameter(p.Name ?? $"p{parameters.Count}", paramTypeName, isOut, isRef, isRefStruct));
            }

            // If we broke out due to an incomplete type, skip to next method
            if (parameters.Count == 0 && skippedMethods.Count > 0 &&
                skippedMethods[^1].Contains(method.Name))
                continue;

            var returnType = method.ReturnType;
            var isVoid = returnType.FullName == "System.Void";
            var isRefStructReturn = !isVoid && IsRefStructType(returnType);

            var returnTypeName = GetTypeName(returnType);
            returnTypeName = CSharpSerializer.StripAssemblyQualification(returnTypeName);
            if (!isVoid && LooksIncomplete(returnTypeName))
            {
                skippedMethods.Add($"{method.Name} (unresolved return type: {returnTypeName})");
                continue;
            }

            // Skip methods with non-public return types (CS0122 in generated code)
            if (!isVoid)
            {
                var rawReturnType = returnType.IsByRef ? returnType.GetElementType()! : returnType;
                if (!rawReturnType.IsGenericParameter && !rawReturnType.IsVisible)
                {
                    skippedMethods.Add($"{method.Name} (internal return type: {returnTypeName})");
                    continue;
                }
                if (IsUnprobableType(rawReturnType))
                {
                    skippedMethods.Add($"{method.Name} (unprobable return type: {rawReturnType.FullName ?? returnTypeName})");
                    continue;
                }
                // Skip methods returning types that trigger LNK2001 in the AOT codegen
                // because the codegen doesn't emit MethodTable entries for these types
                // when they appear in boxing contexts (e.g. GetResultToLongExpression).
                // This includes Vector<T> specializations and ValueTuples of Vector<T>.
                if (IsUnboxableType(rawReturnType))
                {
                    skippedMethods.Add($"{method.Name} (unboxable return type: {rawReturnType.FullName ?? returnTypeName})");
                    continue;
                }
            }

            // String-based ref struct detection: MLC's Type.IsByRefLike may not
            // work for generic instantiations (e.g. ReadOnlySpan<char>), so we
            // also check the base type name (without generic args or namespace).
            if (!isRefStructReturn && !isVoid)
            {
                var rtBase = returnTypeName;
                var gaIdx = rtBase.IndexOf('<');
                if (gaIdx >= 0) rtBase = rtBase[..gaIdx];
                var lastDot = rtBase.LastIndexOf('.');
                if (lastDot >= 0) rtBase = rtBase[(lastDot + 1)..];
                // Check both with and without backtick arity suffix
                if (RefStructTypeNames.Contains(rtBase) ||
                    RefStructTypeNames.Contains(rtBase + "`1"))
                    isRefStructReturn = true;
            }

            // Skip methods returning ref struct types (Span<T>, ReadOnlySpan<T>,
            // etc.) because GetResultToLongExpression boxes the return value via
            // (object)(returnValue), which is illegal for ref structs (CS0030).
            if (isRefStructReturn)
            {
                skippedMethods.Add($"{method.Name} (ref struct return type: {returnTypeName})");
                continue;
            }

            signatures.Add(new MethodSignature(
                method.Name,
                targetType.FullName is not null ? GetTypeName(targetType) : typeFullName,
                returnTypeName,
                method.IsStatic,
                isVoid,
                hasRefParam,
                parameters,
                isRefStructReturn,
                genericTypeArgs
            ));
        }

        return new DllScanResult(assemblyName,
            CSharpSerializer.StripAssemblyQualification(typeFullName),
            targetType.Namespace ?? string.Empty,
            signatures, skippedMethods, tfm,
            targetType.IsAbstract, targetType.IsInterface);
    }

    /// <summary>
    /// Scan all public types in the assembly that have public methods.
    /// Optionally filter by namespace prefix (comma-separated).
    /// Returns one DllScanResult per type. Types with no public methods,
    /// enums, or types that fail to scan are skipped with a warning.
    /// </summary>
    public IReadOnlyList<DllScanResult> ScanAll(string dllPath, string? namespaceFilter = null)
    {
        var results = new List<DllScanResult>();
        var fullPath = Path.GetFullPath(dllPath);
        var assemblyName = Path.GetFileNameWithoutExtension(dllPath);

        // Load assembly once — reuse the same MetadataLoadContext for all types
        var probeDirs = GetProbeDirectories(fullPath);
        var resolver = new AssemblyResolver(probeDirs);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");
        var assembly = mlc.LoadFromAssemblyPath(fullPath);

        var types = ListPublicTypesCore(assembly);

        // Parse namespace filter: comma-separated prefixes (e.g. "System.IO,System.Text")
        var nsFilters = string.IsNullOrEmpty(namespaceFilter)
            ? null
            : new HashSet<string>(namespaceFilter.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries));

        Console.WriteLine($"  Scanning {types.Count} types...");
        int skipCount = 0;
        var seenTypes = new HashSet<string>(); // deduplicate: same concrete type from open generic + closed form
        foreach (var (typeName, _) in types)
        {
            if (UnprobableTypeNames.Contains(typeName))
            {
                Console.WriteLine($"  [SKIP] {typeName}: infrastructure-dependent type");
                skipCount++;
                continue;
            }

            // Apply namespace filter: check if type namespace matches any filter prefix
            if (nsFilters is not null)
            {
                var lastDot = typeName.LastIndexOf('.');
                var ns = lastDot >= 0 ? typeName[..lastDot] : "";
                var matches = nsFilters.Any(f => string.Equals(ns, f, StringComparison.Ordinal) ||
                                                  (f.Length == 0 && ns.Length == 0));
                if (!matches)
                {
                    skipCount++;
                    continue;
                }
            }

            try
            {
                var result = ScanInContext(mlc, assembly, assemblyName, typeName);
                if (result.Methods.Count > 0)
                {
                    // Deduplicate: MLC may return both an open generic (concretized in
                    // ScanInContext) and an already-concrete closed form with the same
                    // TypeFullName.  Normalise by removing all spaces so that subtle
                    // differences (e.g. "Dictionary<K, V>" vs "Dictionary<K,V>") do not
                    // defeat the set lookup.  See also: Type.FullName uses a space after
                    // the comma, but assembly.GetType() resolution may produce one without.
                    var dedupKey = result.TypeFullName.Replace(" ", "");
                    if (!seenTypes.Add(dedupKey))
                    {
                        Console.WriteLine($"  [SKIP] {typeName}: duplicate (already scanned as {result.TypeFullName})");
                        skipCount++;
                    }
                    else
                    {
                        results.Add(result);
                    }
                }
                else
                    skipCount++;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"  [SKIP] {typeName}: {ex.Message}");
                skipCount++;
            }
        }

        Console.WriteLine($"  Scanned {results.Count} types ({skipCount} skipped)");
        return results;
    }

    /// <summary>
    /// List all public types in the assembly with their public method counts.
    /// Creates its own MetadataLoadContext. For batch operations, use
    /// <see cref="ListPublicTypesCore"/> with a pre-loaded assembly.
    /// </summary>
    public List<(string FullName, int MethodCount)> ListPublicTypes(string dllPath)
    {
        var fullPath = Path.GetFullPath(dllPath);
        var probeDirs = GetProbeDirectories(fullPath);
        var resolver = new AssemblyResolver(probeDirs);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");
        var assembly = mlc.LoadFromAssemblyPath(fullPath);
        return ListPublicTypesCore(assembly);
    }

    /// <summary>
    /// List all public types in a pre-loaded assembly with their public method counts.
    /// Used by ScanAll to avoid redundant MetadataLoadContext creation.
    /// </summary>
    private static List<(string FullName, int MethodCount)> ListPublicTypesCore(Assembly assembly)
    {

        var result = new List<(string, int)>();
        foreach (var t in assembly.GetTypes().OrderBy(t => t.FullName))
        {
            if (!t.IsVisible) continue;
            if (t.IsEnum) continue;
            if (IsUnprobableType(t)) continue;
            // Skip GENERIC versions of marshaller/compiler-infrastructure types
            // whose concretization with int produces invalid C#. Non-generic
            // versions (e.g. ConfiguredTaskAwaitable without type params) are
            // kept — only generic variants (ConfiguredTaskAwaitable`1) fail.
            if (t.Name.Contains('`'))
            {
                var baseName = t.Name.AsSpan(0, t.Name.IndexOf('`')).ToString();
                if (UnprobableMarshallerTypeNames.Contains(baseName))
                    continue;
            }
            // Also check nested types inside generic containers, which MLC
            // reports with FullName like "Container`1+Nested" but whose Name
            // is just the simple nested name without backtick.
            if (t.FullName is not null && !t.Name.Contains('`') && t.FullName.Contains('`'))
            {
                var btIdx = t.FullName.IndexOf('`');
                var dotBefore = t.FullName.LastIndexOf('.', btIdx, btIdx);
                var containerName = dotBefore > 0
                    ? t.FullName[(dotBefore + 1)..btIdx]
                    : t.FullName[..btIdx];
                if (UnprobableMarshallerTypeNames.Contains(containerName))
                    continue;
            }

            var count = t.GetMethods(
                BindingFlags.Public | BindingFlags.Static |
                BindingFlags.Instance | BindingFlags.DeclaredOnly)
                .Count(m => m.Name is not ("get_" or "set_" or "add_" or "remove_")
                    && !m.Name.StartsWith("op_")
                    && !m.IsSpecialName);

            if (count > 0)
                result.Add((t.FullName ?? t.Name, count));
        }
        return result;
    }

    /// <summary>
    /// Try to concretize a generic method by substituting common type arguments.
    /// Works within the MetadataLoadContext.
    /// </summary>
    private static bool TryConcretizeGenericMethod(MetadataLoadContext mlc, MethodInfo method, out MethodInfo constructed)
    {
        constructed = null!;
        try
        {
            var genericParams = method.GetGenericArguments();
            if (genericParams.Length == 0) return false;

            var concreteTypes = new Type[genericParams.Length];

            for (int i = 0; i < genericParams.Length; i++)
            {
                var gp = genericParams[i];
                var constraints = gp.GetGenericParameterConstraints();
                var attrs = gp.GenericParameterAttributes;

                if (constraints.Length > 0)
                {
                    var hasValueTypeConstraint = constraints.Any(c =>
                        c.Name is "ValueType" or "Enum" || c.FullName == "System.ValueType" || c.FullName == "System.Enum");

                    if (hasValueTypeConstraint)
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, SelectConcreteValueType(constraints))
                                           ?? LoadTypeInContext(mlc, "System.Int32")!;
                    }
                    else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                    }
                    else
                    {
                        var resolved = ResolveInterfaceConstrainedType(mlc, gp, constraints)
                                       ?? TryResolveClassConstrainedType(mlc, constraints);
                        if (resolved is not null)
                        {
                            concreteTypes[i] = resolved;
                        }
                        else
                        {
                            // Can't resolve any constraint — falling to Int32 would
                            // produce CS0315 (e.g. int doesn't implement IDictionary).
                            // Skip this method/type entirely.
                            return false;
                        }
                    }
                }
                else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                }
                else
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.Int32")!;
                }

                if (concreteTypes[i] is null) return false;
            }

            constructed = method.MakeGenericMethod(concreteTypes);
            return true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Map from open-generic interface FullName to a concrete type FullName used for
    /// concretizing generic parameters constrained by those interfaces.
    /// E.g. IList&lt;T&gt; → List&lt;Int32&gt; so that methods with "where T : IList&lt;int&gt;"
    /// get T = List&lt;int&gt; instead of T = int (which doesn't implement IList).
    /// </summary>
    private static readonly Dictionary<string, string> InterfaceConcreteMap = new(StringComparer.Ordinal)
    {
        // Generic collection interfaces → List<T> as default
        ["System.Collections.Generic.IList`1"] = "System.Collections.Generic.List`1",
        ["System.Collections.Generic.ICollection`1"] = "System.Collections.Generic.List`1",
        ["System.Collections.Generic.IEnumerable`1"] = "System.Collections.Generic.List`1",
        ["System.Collections.Generic.IReadOnlyList`1"] = "System.Collections.Generic.List`1",
        ["System.Collections.Generic.IReadOnlyCollection`1"] = "System.Collections.Generic.List`1",
        ["System.Collections.Generic.ISet`1"] = "System.Collections.Generic.HashSet`1",
        // Non-generic collection interfaces
        ["System.Collections.IList"] = "System.Collections.ArrayList",
        ["System.Collections.ICollection"] = "System.Collections.ArrayList",
        ["System.Collections.IEnumerable"] = "System.Collections.ArrayList",
        // Dictionary
        ["System.Collections.Generic.IDictionary`2"] = "System.Collections.Generic.Dictionary`2",
        ["System.Collections.Generic.IReadOnlyDictionary`2"] = "System.Collections.Generic.Dictionary`2",
        // Comparison
        ["System.IComparable`1"] = "System.Int32",
        ["System.IEquatable`1"] = "System.Int32",
        // Constrained generic parameters for comparers (used in BinarySearch, Sort, etc.)
        ["System.Collections.Generic.IComparer`1"] = "System.Collections.Generic.Comparer`1",
        ["System.Collections.Generic.IEqualityComparer`1"] = "System.Collections.Generic.EqualityComparer`1",
        // Numeric interfaces (used in generic math / System.Numerics)
        // These map directly to concrete value types that implement the interface.
        ["System.Numerics.INumber`1"] = "System.Int32",
        ["System.Numerics.INumberBase`1"] = "System.Int32",
        ["System.Numerics.IBinaryInteger`1"] = "System.Int64",
        ["System.Numerics.IBinaryNumber`1"] = "System.Int64",
        ["System.Numerics.IFloatingPoint`1"] = "System.Double",
        ["System.Numerics.IFloatingPointIeee754`1"] = "System.Double",
        ["System.Numerics.IExponentialFunctions`1"] = "System.Double",
        ["System.Numerics.ILogarithmicFunctions`1"] = "System.Double",
        ["System.Numerics.ITrigonometricFunctions`1"] = "System.Double",
        ["System.Numerics.IHyperbolicFunctions`1"] = "System.Double",
        ["System.Numerics.IRootFunctions`1"] = "System.Double",
        ["System.Numerics.IPowerFunctions`1"] = "System.Double",
    };

    /// <summary>
    /// Try to find a concrete type that satisfies the interface constraints on a generic parameter.
    /// Checks the interface constraints against InterfaceConcreteMap and (if needed) concretizes
    /// the mapped type's own generic arguments with Int32. Returns null if no mapping found.
    /// </summary>
    private static Type? ResolveInterfaceConstrainedType(
        MetadataLoadContext mlc, Type genericParameter, Type[] constraints)
    {
        foreach (var constraint in constraints)
        {
            if (!constraint.IsInterface) continue;

            string? constraintFullName;
            if (constraint.IsGenericType)
            {
                var gtd = constraint.GetGenericTypeDefinition();
                constraintFullName = gtd.FullName;
                // MLC returns empty FullName for cross-type-parameter constraints
                // (e.g. IComparer<T> where T is another generic param).
                // Reconstruct from Namespace.Name in that case.
                if (string.IsNullOrEmpty(constraintFullName) && gtd.Namespace != null)
                    constraintFullName = $"{gtd.Namespace}.{gtd.Name}";
            }
            else
            {
                constraintFullName = constraint.FullName;
            }

            if (string.IsNullOrEmpty(constraintFullName)) continue;

            if (!InterfaceConcreteMap.TryGetValue(constraintFullName, out var concreteName))
                continue;

            // Load the concrete type (e.g. "System.Collections.Generic.List`1")
            var concreteType = LoadTypeInContext(mlc, concreteName);
            if (concreteType is null)
                continue;

            // If the concrete type itself is generic, concretize with Int32
            if (concreteType.IsGenericTypeDefinition)
            {
                var concreteArgs = concreteType.GetGenericArguments();
                var concreteArgTypes = new Type[concreteArgs.Length];
                for (int j = 0; j < concreteArgs.Length; j++)
                {
                    concreteArgTypes[j] = LoadTypeInContext(mlc, "System.Int32")!;
                }
                try
                {
                    concreteType = concreteType.MakeGenericType(concreteArgTypes);
                }
                catch
                {
                    continue; // fallback to Int32
                }
            }

            return concreteType;
        }

        return null;
    }

    /// <summary>
    /// When a generic parameter has non-interface class-type constraints (e.g. "where T : Delegate"),
    /// use the first such constraint as the concrete type.  This avoids concretizing to Int32
    /// (which violates CS0315 for class-constrained parameters like TDelegate).
    /// </summary>
    private static Type? TryResolveClassConstrainedType(MetadataLoadContext mlc, Type[] constraints)
    {
        foreach (var constraint in constraints)
        {
            if (constraint.IsInterface) continue;
            if (constraint.IsGenericParameter) continue;

            // Handle generic types with unresolved type parameters from other
            // generic params (e.g., List<TElement> as a constraint on TCollection
            // where TElement is another method generic param).  FullName is null
            // for these, so concretize the GTD with Int32 → List<Int32>.
            if (constraint.IsGenericType && constraint.ContainsGenericParameters)
            {
                var gtd = constraint.GetGenericTypeDefinition();
                if (gtd is not null)
                {
                    var gtdParams = gtd.GetGenericArguments();
                    var concreteArgs = new Type[gtdParams.Length];
                    bool ok = true;
                    for (int j = 0; j < gtdParams.Length; j++)
                    {
                        concreteArgs[j] = LoadTypeInContext(mlc, "System.Int32")!;
                        if (concreteArgs[j] is null) { ok = false; break; }
                    }
                    if (ok)
                    {
                        try { return gtd.MakeGenericType(concreteArgs); }
                        catch { /* fall through to FullName-based load */ }
                    }
                }
            }

            var cn = constraint.FullName;
            if (cn is null) continue;
            // Skip special value type markers
            if (cn is "System.ValueType" or "System.Enum") continue;
            // Must be a class type → use it directly
            var loaded = LoadTypeInContext(mlc, cn);
            if (loaded is not null) return loaded;
        }
        return null;
    }

    /// <summary>
    /// Try to concretize a generic type definition (e.g., Stack`1 → Stack&lt;System.Int32&gt;)
    /// by substituting common type arguments for its type-level generic parameters.
    /// Works within the MetadataLoadContext using the same heuristic as TryConcretizeGenericMethod.
    /// </summary>
    private static bool TryConcretizeGenericType(MetadataLoadContext mlc, Type type, out Type constructed)
    {
        constructed = null!;
        try
        {
            var genericParams = type.GetGenericArguments();
            if (genericParams.Length == 0) return false;

            // Nested types of generic parents inherit parent type parameters.
            // GetGenericArguments returns ALL args (parent + own), but our
            // heuristic fills ALL with Int32/String, which creates a synthetic
            // type like Dictionary<int,int,int> that doesn't compile.
            // Skip concretization for nested generic types with their own params.
            if (type.DeclaringType is { IsGenericType: true } declaringType)
            {
                var parentArity = declaringType.GetGenericArguments().Length;
                if (genericParams.Length > parentArity)
                {
                    // This nested type adds its OWN type parameters beyond
                    // inherited ones. Don't concretize — the generated C#
                    // type name would be incorrect.
                    return false;
                }
            }

            var concreteTypes = new Type[genericParams.Length];

            for (int i = 0; i < genericParams.Length; i++)
            {
                var gp = genericParams[i];
                var constraints = gp.GetGenericParameterConstraints();
                var attrs = gp.GenericParameterAttributes;

                if (constraints.Length > 0)
                {
                    var hasValueTypeConstraint = constraints.Any(c =>
                        c.Name is "ValueType" or "Enum" || c.FullName == "System.ValueType" || c.FullName == "System.Enum");

                    if (hasValueTypeConstraint)
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, SelectConcreteValueType(constraints))
                                           ?? LoadTypeInContext(mlc, "System.Int32")!;
                    }
                    else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                    }
                    else
                    {
                        var resolved = ResolveInterfaceConstrainedType(mlc, gp, constraints)
                                       ?? TryResolveClassConstrainedType(mlc, constraints);
                        if (resolved is not null)
                        {
                            concreteTypes[i] = resolved;
                        }
                        else
                        {
                            // Can't resolve any constraint — falling to Int32 would
                            // produce CS0315 (e.g. int doesn't implement IDictionary).
                            // Skip this method/type entirely.
                            return false;
                        }
                    }
                }
                else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                }
                else
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.Int32")!;
                }

                if (concreteTypes[i] is null) return false;
            }

            constructed = type.MakeGenericType(concreteTypes);
            return true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Pick a concrete type name for a ValueType-constrained generic parameter based on
    /// its interface constraints.
    ///   IFloatingPoint&lt;T&gt; / IFloatingPointIeee754&lt;T&gt;  → double
    ///   IBinaryInteger&lt;T&gt; / IBinaryNumber&lt;T&gt;            → long
    ///   INumber&lt;T&gt; / INumberBase&lt;T&gt; + operator ifaces   → int
    ///   Enum                                                 → DayOfWeek
    ///   default (IComparable, IEquatable, etc.)              → int
    /// </summary>
    private static string SelectConcreteValueType(Type[] constraints)
    {
        bool hasEnum = false;
        foreach (var c in constraints)
        {
            if (c.FullName is "System.Enum") { hasEnum = true; continue; }
            var name = c.Name;
            var bt = name.IndexOf('`');
            var baseName = bt >= 0 ? name[..bt] : name;
            switch (baseName)
            {
                case "IFloatingPointIeee754":
                case "IFloatingPoint":
                    return "System.Double";
                case "IBinaryInteger":
                case "IBinaryNumber":
                    return "System.Int64";
                case "INumber":
                case "INumberBase":
                case "IAdditionOperators":
                case "ISubtractionOperators":
                case "IMultiplyOperators":
                case "IDivisionOperators":
                case "IModulusOperators":
                case "IBitwiseOperators":
                case "IShiftOperators":
                case "IComparisonOperators":
                case "IEqualityOperators":
                case "IParsable":
                case "ISpanParsable":
                case "IFormattable":
                case "ISpanFormattable":
                case "IComparable":
                case "IComparable`1":
                case "IEquatable":
                case "IEquatable`1":
                case "IConvertible":
                case "IMinMaxValue":
                    return "System.Int32";
            }
        }
        return hasEnum ? "System.DayOfWeek" : "System.Int32";
    }

    /// <summary>
    /// Check if a concretized method involves ref struct types (Span, ReadOnlySpan)
    /// as parameter or return types or declaring type.  Such methods produce invalid C#
    /// because ref structs can't serve as generic type arguments, generating calls
    /// like Span&lt;int&gt;.MoveNext().
    /// </summary>
    private static bool RefStructTypeName(string name) =>
        name is "System.Span`1" or "System.ReadOnlySpan`1" or "System.ArraySegment`1"
        || name.StartsWith("System.Span<") || name.StartsWith("System.ReadOnlySpan<") || name.StartsWith("System.ArraySegment<");

    private static bool ConcretizedToRefStruct(MethodInfo method)
    {
        // Check parameters
        foreach (var p in method.GetParameters())
        {
            var pt = p.ParameterType;
            if (pt.IsByRef) pt = pt.GetElementType()!;
            var name = pt.FullName ?? pt.Name;
            if (RefStructTypeName(name)) return true;
            // Check generic args of parameter type
            if (pt.IsGenericType)
            {
                foreach (var ga in pt.GetGenericArguments())
                {
                    var gn = ga.FullName ?? ga.Name;
                    if (RefStructTypeName(gn)) return true;
                }
            }
        }
        // Check return type
        var rt = method.ReturnType;
        var rn = rt.FullName ?? rt.Name;
        if (RefStructTypeName(rn)) return true;
        // Check declaring type — for MoveNext() on Span<T>.Enumerator, the
        // declaring type is the nested enumerator type WITHIN the ref struct.
        var dt = method.DeclaringType;
        if (dt is not null)
        {
            var dn = dt.FullName ?? dt.Name;
            if (RefStructTypeName(dn)) return true;
            // Check if declaring type is nested inside a ref struct
            for (var decl = dt.DeclaringType; decl is not null; decl = decl.DeclaringType)
            {
                var dn2 = decl.FullName ?? decl.Name;
                if (RefStructTypeName(dn2)) return true;
            }
        }
        // Also skip string.Trim(ReadOnlySpan<char>) — MLC picks the ReadOnlySpan<char>
        // overload but C# resolves to Trim(char), causing CS1503.
        if (method.Name is "Trim" or "TrimStart" or "TrimEnd")
        {
            foreach (var p in method.GetParameters())
            {
                var pt = p.ParameterType;
                var fn = pt.FullName ?? pt.Name;
                if (fn.Contains("ReadOnlySpan"))
                    return true;
            }
        }
        return false;
    }

    /// <summary>
    /// Find a type by full name across all assemblies loaded in the MetadataLoadContext.
    /// </summary>
    private static Type? LoadTypeInContext(MetadataLoadContext mlc, string typeFullName)
    {
        foreach (var asm in mlc.GetAssemblies())
        {
            var type = asm.GetType(typeFullName);
            if (type is not null) return type;
        }
        return null;
    }

    private static string GetTypeName(Type type)
    {
        // Generic parameter (e.g. T, TOther) — use base type constraint or object
        if (type.IsGenericParameter)
        {
            var constraints = type.GetGenericParameterConstraints();
            if (constraints.Length > 0 &&
                constraints[0].FullName is { } cn &&
                cn != "System.ValueType" &&
                cn != "System.Enum")
                return cn;
            return "System.Object";
        }

        if (type.IsGenericType)
        {
            // For nested types: C# compiler inherits parent type params.
            // CLR includes ALL args (parent + own), but C# only wants the nested type's own.
            // Subtract parent arity to get the right C# generic signature.
            var allArgs = type.GetGenericArguments();
            var parentArity = type.DeclaringType?.GetGenericArguments().Length ?? 0;
            var ownArgs = allArgs.Skip(parentArity).ToArray();
            var args = string.Join(",", ownArgs.Select(GetTypeName));
            var defName = type.GetGenericTypeDefinition().Name;
            var backtick = defName.IndexOf('`');
            if (backtick >= 0) defName = defName[..backtick];

            // For non-nested generic types, use FullName for consistency with
            // the non-generic code path (line 515 returns type.FullName).
            // This ensures Task`1 → "System.Threading.Tasks.Task" so downstream
            // async detection (TestEmitter, ProbeEmitter) works correctly.
            if (type.DeclaringType is null && type.GetGenericTypeDefinition().FullName is { } nsName)
            {
                var fb = nsName.IndexOf('`');
                defName = fb >= 0 ? nsName[..fb].Replace('+', '.') : nsName.Replace('+', '.');
            }

            // Nested types: include parent type name for C# qualification.
            // MLC may keep DeclaringType as the generic type definition even after
            // parent concretization — in that case, reconstruct parent name from
            // the all-inclusive generic arguments.
            if (type.DeclaringType is { } declaringType)
            {
                string parentName;
                if (parentArity > 0 && declaringType.IsGenericTypeDefinition)
                {
                    var parentArgs = allArgs.Take(parentArity).Select(GetTypeName);
                    var parentDefName = declaringType.Name;
                    var parentBt = parentDefName.IndexOf('`');
                    if (parentBt >= 0) parentDefName = parentDefName[..parentBt];
                    parentName = $"{parentDefName}<{string.Join(",", parentArgs)}>";
                }
                else
                {
                    parentName = GetTypeName(declaringType);
                }
                var csharpName = $"{parentName}.{defName}";
                if (ownArgs.Length > 0)
                    csharpName += $"<{args}>";
                return csharpName;
            }

            return $"{defName}<{args}>";
        }

        // Handle array types: delegate to element type + append [].
        // Must come before the FullName check because FullName for arrays of
        // generic types uses CLR backtick notation (e.g.
        // "KeyValuePair`2[[Int32],[Int32]][]") which is not valid C#.
        if (type.IsArray)
        {
            var elType = type.GetElementType();
            if (elType is not null)
                return GetTypeName(elType) + "[]";
        }

        // Pointer types: GetTypeName(elementType) + "*".
        // Must come before FullName check because MLC's FullName for
        // System.Void* is "System.Void*" which gets mapped to "Void*" by
        // KeywordMap — but C# only recognizes "void*", not "Void*" or "System.Void*".
        if (type.IsPointer && type.GetElementType() is { } pointerElement)
            return GetTypeName(pointerElement) + "*";

        // Unwrap ByRef types (ref/out params) before checking FullName,
        // because MLC may set FullName for ByRef types (including nested generic args)
        // and the FullName branch below would produce a mangled name.
        if (type.IsByRef && type.GetElementType() is { } elementType)
            return GetTypeName(elementType) + "&";

        var fullName = type.FullName;
        if (fullName is not null)
        {
            // CLR nested type: produce C# parent-qualified name, e.g.
            // "System.Text.StringBuilder+AppendInterpolatedStringHandler"
            // → "StringBuilder.AppendInterpolatedStringHandler"
            if (fullName.Contains('+'))
            {
                // MLC may not populate DeclaringType for nested types
                if (type.DeclaringType is not null)
                {
                    // Use parent's short name to avoid namespace prefix pollution.
                    // MapToCSharpType would convert the FullName to a C# type name,
                    // but downstream callers already call MapToCSharpType again,
                    // so keeping the parent fully-qualified causes double-processing
                    // that strips the parent prefix (e.g. "MemoryExtensions.").
                    var parentFullName = type.DeclaringType.FullName ?? type.DeclaringType.Name;
                    var parentDot = parentFullName.LastIndexOf('.');
                    var parentShort = parentDot >= 0 ? parentFullName[(parentDot + 1)..] : parentFullName;
                    return $"{parentShort}.{type.Name}";
                }
                // Fallback: extract parent short name for C# compatibility
                // e.g. "System.Text.StringBuilder+AppendInterpolatedStringHandler"
                // → "StringBuilder.AppendInterpolatedStringHandler"
                var plusIdx = fullName.LastIndexOf('+');
                if (plusIdx >= 0)
                {
                    var parentPart = fullName[..plusIdx];
                    var parentLastDot = parentPart.LastIndexOf('.');
                    var parentShort = parentLastDot >= 0 ? parentPart[(parentLastDot + 1)..] : parentPart;
                    return $"{parentShort}.{type.Name}";
                }
                return fullName;
            }
            return fullName;
        }

        // MetadataLoadContext sometimes loses FullName for constructed generics
        // and their nested types. Fall back to Name, but preserve Namespace when available.
        var name = type.Name;
        var ns = type.Namespace;
        if (ns is { Length: > 0 } && !name.Contains(ns))
            name = $"{ns}.{name}";

        // If Namespace is also null, try to find the type by Name in loaded assemblies.
        // MLC can lose Namespace for types from assemblies not directly loaded into
        // the MLC context but referenced by the target assembly.
        if (ns is null && type.FullName is null && type.Assembly is { } asm)
        {
            var asmQualified = type.AssemblyQualifiedName;
            if (asmQualified is not null)
            {
                var comma = asmQualified.IndexOf(',');
                if (comma > 0) name = asmQualified[..comma];
            }
        }

        // For nested types with no FullName (MLC limitation), qualify with parent name.
        // E.g. Dictionary<int,int>.Enumerator instead of bare "Enumerator".
        if (type.DeclaringType is { } dt)
        {
            var parentName = GetTypeName(dt);
            return $"{parentName}.{name}";
        }

        var bt = name.IndexOf('`');
        if (bt < 0) return name;

        var arityStr = name[(bt + 1)..];
        if (int.TryParse(arityStr, out var arity) && arity > 0)
        {
            var args = string.Join(",", Enumerable.Repeat("System.Byte", arity));
            return $"{name[..bt]}<{args}>";
        }
        return name;
    }

    /// <summary>
    /// Check if a method has any ref struct parameters (e.g. ReadOnlySpan&lt;char&gt;).
    /// </summary>
    private static bool HasRefStructParameter(MethodInfo method)
    {
        try
        {
            return method.GetParameters().Any(p => IsRefStructType(p.ParameterType));
        }
        catch { return false; }
    }

    /// <summary>
    /// Check if a method returns a ref struct type.
    /// </summary>
    private static bool IsRefStructReturn(MethodInfo method)
    {
        try { return IsRefStructType(method.ReturnType); }
        catch { return false; }
    }

    private static readonly HashSet<string> RefStructTypeNames = new(StringComparer.Ordinal)
    {
        "ReadOnlySpan", "ReadOnlySpan`1",
        "Span", "Span`1",
    };

    /// <summary>
    /// Check if a type is a ref struct (IsByRefLike).
    /// For MLC's limited detection of CLR nested ref structs (e.g.
    /// StringBuilder+AppendInterpolatedStringHandler), falls back to
    /// heuristic: value type whose FullName contains '+' (CLR nested marker).
    /// Also checks known ref struct type names since MLC may not report
    /// IsByRefLike for generic instantiations like ReadOnlySpan&lt;char&gt;.
    /// </summary>
    private static bool IsRefStructType(Type type)
    {
        // Direct IsByRefLike check (may not work for generic instantiations in MLC)
        if (type.IsByRefLike) return true;

        // Name-based check for known ref struct types
        if (RefStructTypeNames.Contains(type.Name))
            return true;

        // For ByRef-wrapped types (ref/out parameters)
        if (type.IsByRef && type.GetElementType() is { } elementType)
        {
            if (elementType.IsByRefLike) return true;
            // Heuristic: CLR nested struct is likely a ref struct
            if (elementType.IsValueType && elementType.FullName?.Contains('+') == true)
                return true;
            return false;
        }

        // Heuristic: CLR nested value type that isn't a normal struct
        if (type.IsValueType && type.FullName?.Contains('+') == true)
            return true;

        return false;
    }

    /// <summary>
    /// Check if a method still has unresolved generic parameters in its signature
    /// after concretization (e.g. interface-level type params from generic math interfaces
    /// like INumberBase&lt;TOther&gt; where TOther comes from the interface, not the method).
    /// </summary>
    private static bool HasUnresolvedGenericParameters(MethodInfo method)
    {
        try
        {
            if (method.ReturnType.ContainsGenericParameters ||
                HasIncompleteGenericName(method.ReturnType))
                return true;
            if (method.GetParameters().Any(p =>
                p.ParameterType.ContainsGenericParameters ||
                HasIncompleteGenericName(p.ParameterType)))
                return true;
            return false;
        }
        catch
        {
            return true; // conservative: skip if we can't inspect
        }
    }

    /// <summary>
    /// Detect types whose FullName is null and Name is a short name without generic
    /// — this means MLC couldn't resolve the type and GetTypeName will return
    /// an incomplete name (e.g. "Dictionary" instead of "Dictionary&lt;int,int&gt;").
    /// </summary>
    private static bool HasIncompleteGenericName(Type type)
    {
        var t = type.IsByRef ? type.GetElementType()! : type;
        if (t.IsGenericParameter) return false;
        if (t.IsGenericType) return false;  // has proper generic args
        if (t.FullName is not null) return false;  // has proper name
        // Name without FullName and not a generic type — MLC couldn't resolve it
        return t.Name.Contains('`') || char.IsUpper(t.Name[0]);
    }

    /// <summary>
    /// Check if a return type triggers LNK2001 in the AOT codegen because the
    /// codegen doesn't emit MethodTable entries when boxing these types.
    /// Vector&lt;T&gt; specializations and ValueTuples containing Vector&lt;T&gt;
    /// are the primary cases.
    /// </summary>
    private static bool IsUnboxableType(Type type)
    {
        try
        {
            var t = type.IsByRef ? type.GetElementType()! : type;
            if (t.IsGenericParameter) return false;
            if (t.FullName is null) return false;

            // Vector<T>: any specialization (Vector<byte>, Vector<float>, etc.)
            // NOTE: FullName for constructed generics includes generic args
            // (e.g. "Vector`1[[System.Int32,...]]"), so extract the base name.
            var baseName = t.FullName;
            var bracketIdx = baseName.IndexOf("[");
            if (bracketIdx >= 0) baseName = baseName.Substring(0, bracketIdx);
            if (baseName == "System.Numerics.Vector`1")
                return true;

            // Non-generic System.Numerics struct types that cause LNK2001 when boxed
            if (baseName is "System.Numerics.Vector2" or
                "System.Numerics.Vector3" or
                "System.Numerics.Vector4" or
                "System.Numerics.Plane" or
                "System.Numerics.Quaternion" or
                "System.Numerics.Matrix3x2" or
                "System.Numerics.Matrix4x4")
                return true;

            // ValueTuple types containing Vector<T>: check generic args
            if (baseName.StartsWith("System.ValueTuple`"))
            {
                foreach (var ga in t.GetGenericArguments())
                {
                    if (IsUnboxableType(ga))
                        return true;
                }
                // ValueTuple without unboxable types is fine
                return false;
            }

            return false;
        }
        catch
        {
            return true; // conservative: skip if we can't inspect
        }
    }

    /// <summary>
    /// Check if a type (or any type in its declaring type chain) is in UnprobableTypeNames.
    /// Catches nested types like ComVariantMarshaller+RefPropagate whose parent is unprobable,
    /// even though their own FullName ("...ComVariantMarshaller+RefPropagate") doesn't
    /// directly match the set entry ("...ComVariantMarshaller").
    /// </summary>
    private static bool IsUnprobableType(Type type)
    {
        var current = type;
        while (current is not null)
        {
            if (current.FullName is not null && UnprobableTypeNames.Contains(current.FullName))
                return true;
            current = current.DeclaringType;
        }

        // MLC can report nested types either as "Parent+Nested" (metadata format)
        // or "Parent.Nested" (C# format) with no DeclaringType.  Check the FullName
        // prefix against every UnprobableTypeNames entry.
        if (type.FullName is not null)
        {
            foreach (var unprobable in UnprobableTypeNames)
            {
                if (type.FullName.StartsWith(unprobable, StringComparison.Ordinal) &&
                    type.FullName.Length > unprobable.Length &&
                    (type.FullName[unprobable.Length] == '.' || type.FullName[unprobable.Length] == '+'))
                    return true;
            }
        }

        return false;
    }

    /// <summary>
    /// Check if a type name returned by GetTypeName looks incomplete — i.e. is a
    /// short name without namespace or generic args, and is not a primitive/keyword.
    /// This catches cases where MLC returns a type with a non-null FullName but
    /// the constructed generic args are lost (e.g. "Dictionary" instead of "Dictionary&lt;int,int&gt;").
    /// </summary>
    private static bool LooksIncomplete(string typeName)
    {
        var name = typeName.EndsWith('&') ? typeName[..^1].Trim() : typeName;
        if (name.Contains('.') || name.Contains('{')) return false;

        // Has generic args but no namespace dot — likely a nested type missing parent
        // qualification (e.g. "AlternateLookup<int,int,int>" → should be
        // "Dictionary<int,int>.AlternateLookup<int,int,int>").
        var ga = name.IndexOf('<');
        if (ga >= 0)
        {
            var baseName = name[..ga];
            if (baseName is "int" or "uint" or "long" or "ulong" or "short" or "ushort"
                or "byte" or "sbyte" or "float" or "double" or "decimal" or "char"
                or "bool" or "string" or "object") return false;
            return true; // non-primitive with generic args but no namespace → nested type
        }

        if (name is "bool" or "byte" or "sbyte" or "short" or "ushort"
            or "int" or "uint" or "long" or "ulong"
            or "float" or "double" or "decimal" or "char" or "string"
            or "object" or "void") return false;
        if (name.Contains('`')) return true;
        return true;
    }

    private static string[] GetProbeDirectories(string dllPath)
    {
        var dirs = new List<string>
        {
            Path.GetDirectoryName(dllPath)!
        };

        // Add .NET reference assembly path
        var dotnetRoot = Path.GetDirectoryName(typeof(object).Assembly.Location);
        if (dotnetRoot is not null && !dirs.Contains(dotnetRoot))
            dirs.Add(dotnetRoot);

        // Add self directory for System.Runtime etc.
        var selfDir = Path.GetDirectoryName(typeof(DllScanner).Assembly.Location);
        if (selfDir is not null && !dirs.Contains(selfDir))
            dirs.Add(selfDir);

        return dirs.ToArray();
    }

    private static string TfmFromMoniker(string frameworkName)
    {
        // ".NETCoreApp,Version=v10.0" → "net10.0"
        if (string.IsNullOrEmpty(frameworkName)) return "";
        var parts = frameworkName.Split(',');
        if (parts.Length < 2) return "";
        var namePart = parts[0].Trim();
        var verPart = parts[1].Trim();
        if (!verPart.StartsWith("Version=v", StringComparison.Ordinal)) return "";
        var ver = verPart["Version=v".Length..];
        return namePart switch
        {
            ".NETCoreApp" => $"net{ver}",
            ".NETStandard" => $"netstandard{ver}",
            ".NETFramework" => $"net{ver.Replace(".", "")}",
            _ => ""
        };
    }

    private static string DetectTfm(string dllPath)
    {
        // Try to infer TFM from the directory path:
        // ".../shared/Microsoft.NETCore.App/10.0.6/System.Runtime.dll" → "net10.0"
        var parts = dllPath.Replace('\\', '/').Split('/');
        for (int i = 0; i < parts.Length - 1; i++)
        {
            if (parts[i] == "Microsoft.NETCore.App" && System.Version.TryParse(parts[i + 1], out var ver))
                return $"net{ver.Major}.{ver.Minor}";
        }
        return "net8.0";
    }

    /// <summary>
    /// Resolves assemblies by probing known directories.
    /// </summary>
    private sealed class AssemblyResolver : MetadataAssemblyResolver
    {
        private readonly string[] _probePaths;
        private readonly Dictionary<string, Assembly> _cache = new(StringComparer.OrdinalIgnoreCase);

        public AssemblyResolver(string[] probePaths)
        {
            _probePaths = probePaths;
        }

        public override Assembly? Resolve(MetadataLoadContext context, AssemblyName assemblyName)
        {
            var name = assemblyName.Name;
            if (name is null) return null;

            if (_cache.TryGetValue(name, out var cached))
                return cached;

            foreach (var dir in _probePaths)
            {
                var path = Path.Combine(dir, $"{name}.dll");
                if (File.Exists(path))
                {
                    var asm = context.LoadFromAssemblyPath(path);
                    _cache[name] = asm;
                    return asm;
                }
            }

            // For core assemblies that aren't found, try the runtime directory
            var runtimeDir = Path.GetDirectoryName(typeof(object).Assembly.Location);
            if (runtimeDir is not null)
            {
                var runtimePath = Path.Combine(runtimeDir, $"{name}.dll");
                if (File.Exists(runtimePath))
                {
                    var asm = context.LoadFromAssemblyPath(runtimePath);
                    _cache[name] = asm;
                    return asm;
                }
            }

            return null;
        }
    }
}

public sealed record DllScanResult(
    string AssemblyName,
    string TypeFullName,
    string TypeNamespace,
    IReadOnlyList<MethodSignature> Methods,
    IReadOnlyList<string> SkippedMethods,
    string TargetFramework,
    bool IsAbstract = false,
    bool IsInterface = false
);

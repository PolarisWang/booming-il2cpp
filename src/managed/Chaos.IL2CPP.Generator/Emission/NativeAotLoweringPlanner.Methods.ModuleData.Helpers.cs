using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{



    /// <summary>
    /// Resolve a TypeDef/TypeRef parent handle to a "Namespace.Name" string.
    /// Returns null for nil handles or unsupported handle kinds (TypeSpec).
    /// </summary>
    private static string? ResolveBaseTypeName(MetadataReader reader, EntityHandle parentHandle)
    {
        if (parentHandle.IsNil) return null;

        string? parentNs;
        string? parentName;

        switch (parentHandle.Kind)
        {
            case HandleKind.TypeDefinition:
                var parentDef = reader.GetTypeDefinition((TypeDefinitionHandle)parentHandle);
                parentNs = reader.GetString(parentDef.Namespace);
                parentName = reader.GetString(parentDef.Name);
                break;
            case HandleKind.TypeReference:
                var parentRef = reader.GetTypeReference((TypeReferenceHandle)parentHandle);
                parentNs = reader.GetString(parentRef.Namespace);
                parentName = reader.GetString(parentRef.Name);
                break;
            default:
                return null;
        }

        return string.IsNullOrEmpty(parentNs) ? parentName : $"{parentNs}.{parentName}";
    }





    /// <summary>
    /// Compute the type-flags bitmask for a TypeDef entry.
    /// Flags correspond to the kFlag* constants in module_registry.h.
    /// </summary>
    private static uint ComputeTypeFlags(MetadataReader reader, TypeDefinition typeDef, EntityHandle parentHandle)
    {
        uint flags = 0;
        var attributes = typeDef.Attributes;

        // Access flags from metadata attributes
        if ((attributes & TypeAttributes.Public) != 0 || (attributes & TypeAttributes.NestedPublic) != 0)
            flags |= 1u << 10;  // kFlagIsPublic
        if ((attributes & (TypeAttributes.NestedAssembly | TypeAttributes.NestedFamANDAssem
                         | TypeAttributes.NestedFamily | TypeAttributes.NestedFamORAssem
                         | TypeAttributes.NestedPrivate | TypeAttributes.NestedPublic)) != 0)
            flags |= 1u << 9;   // kFlagIsNested
        if ((attributes & TypeAttributes.Abstract) != 0)
            flags |= 1u << 2;   // kFlagIsAbstract
        if ((attributes & TypeAttributes.Sealed) != 0)
            flags |= 1u << 3;   // kFlagIsSealed
        if ((attributes & TypeAttributes.Interface) != 0)
            flags |= 1u << 4;   // kFlagIsInterface

        // ValueType / Enum via base type
        if (!parentHandle.IsNil)
        {
            var parentFullName = ResolveBaseTypeName(reader, parentHandle);
            if (parentFullName != null)
            {
                // System.Enum extends System.ValueType but is NOT a value type in reflection
                // System.ValueType itself extends System.Object — no special handling needed
                if (string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
                    flags |= 1u << 1;  // kFlagIsEnum
                else if (string.Equals(parentFullName, "System.ValueType", StringComparison.Ordinal))
                    flags |= 1u << 0;  // kFlagIsValueType
            }
        }

        // Generic type definition: has generic parameters
        if (typeDef.GetGenericParameters().Count > 0)
            flags |= (1u << 6) | (1u << 7);  // kFlagIsGenericType | kFlagIsGenericTypeDef

        return flags;
    }





    /// <summary>
    /// Compute the SubjectId for a TypeDef from its metadata row.
    /// Format: "AssemblyName/Namespace.TypeName" or "AssemblyName/DeclaringType+NestedName".
    /// </summary>
    private static string ComputeTypeDefSubjectId(MetadataReader reader, TypeDefinitionHandle handle, string assemblyName)
    {
        var typeDef = reader.GetTypeDefinition(handle);
        var name = reader.GetString(typeDef.Name);
        var ns = reader.GetString(typeDef.Namespace);

        // Nested types use the declaring type's full name as prefix
        var declaringHandle = typeDef.GetDeclaringType();
        if (!declaringHandle.IsNil)
        {
            var declaringSubjectId = ComputeTypeDefSubjectId(reader, declaringHandle, assemblyName);
            return $"{declaringSubjectId}+{name}";
        }

        string fullName = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
        return $"{assemblyName}/{fullName}";
    }





    /// <summary>
    /// Build a NativeSymbol → dispatch table slot index mapping by replicating
    /// the same sorting logic used in <see cref="BuildHotpatchTable"/>.
    ///
    /// Only methods with metadata tokens are included (the only ones
    /// that appear in s_hotpatch_entries).
    /// </summary>
    // ── Pre-computed O(1) index builders for invocation planning ──

    private static Dictionary<string, AotCoreIrMethodArtifact> BuildAsyncMoveNextIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            if (!method.IsStatic &&
                string.Equals(GetMethodName(method.SubjectId), "MoveNext", StringComparison.Ordinal))
            {
                index[method.Identity.DeclaringTypeSubjectId] = method;
            }
        }
        return index;
    }





    private static HashSet<string> BuildAllDeclaringTypeSubjectIds(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds,
        IReadOnlyDictionary<string, HashSet<string>> referenceTypeImplementedInterfaceSubjectIds)
    {
        var capacity = referenceTypeBaseSubjectIds.Count
            + referenceTypeImplementedInterfaceSubjectIds.Count
            + methodsBySubjectId.Count;
        var index = new HashSet<string>(capacity, StringComparer.Ordinal);
        foreach (var key in referenceTypeBaseSubjectIds.Keys)
            index.Add(key);
        foreach (var key in referenceTypeImplementedInterfaceSubjectIds.Keys)
            index.Add(key);
        foreach (var method in methodsBySubjectId.Values)
        {
            var declaringType = method.Identity.DeclaringTypeSubjectId;
            if (!string.IsNullOrEmpty(declaringType))
                index.Add(declaringType);
        }
        return index;
    }





    private static Dictionary<string, List<AotCoreIrMethodArtifact>> BuildMethodsByDeclaringTypeIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, List<AotCoreIrMethodArtifact>>(StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            var declaringType = method.Identity.DeclaringTypeSubjectId;
            if (string.IsNullOrEmpty(declaringType))
                continue;
            if (!index.TryGetValue(declaringType, out var list))
            {
                list = new List<AotCoreIrMethodArtifact>();
                index[declaringType] = list;
            }
            list.Add(method);
        }
        return index;
    }





    /// <summary>
    /// Build symbol → dispatch slot map for ALL methods with IL bodies,
    /// not just reachable ones. This ensures hotpatch can target any method
    /// in the module, regardless of reachability from the AOT entry point.
    ///
    /// Methods without ECMA metadata tokens receive synthetic tokens
    /// (0x80000000 | syntheticIndex) so they still occupy dispatch table
    /// slots reachable by name-based lookup.
    /// </summary>
    /// <summary>
    /// Returns all methods that should get hotpatch dispatch slots:
    /// all non-abstract methods with IL bodies, deduplicated by NativeSymbol
    /// (shared generics share the same native symbol), sorted for deterministic
    /// slot assignment.
    /// NOTE: We intentionally do NOT deduplicate by NativeSymbol here so that
    /// s_hotpatch_entries[] has the same count and ordering as kMethodTable[]
    /// (which is built from methodsForLowering with the same sort).  Without
    /// this, the subject slot map (kSubjectSlotMap → kMethodTable index) would
    /// be misaligned when used against s_hotpatch_entries at dispatch time,
    /// causing the wrong method to be called for all entries past the first
    /// duplicate NativeSymbol.  Method body deduplication is handled separately
    /// at the emission level (NativeSymbol → function definition).
    /// </summary>
    private IReadOnlyList<AotCoreIrMethodArtifact> GetHotpatchableMethods()
    {
        return _methodsBySubjectId.Values
            .Where(m => m.Instructions.Count > 0 || m.IsPInvoke) // has IL body or P/Invoke with wrapper — excludes abstract/interface stubs
            .OrderBy(m => ExtractNumericSortKey(m.SubjectId))
            .ThenBy(m => m.SubjectId, StringComparer.Ordinal)
            .ToList();
    }





    /// <summary>
    /// Build symbol → dispatch slot map for ALL methods with IL bodies,
    /// not just reachable ones. This ensures hotpatch can target any method
    /// in the module, regardless of reachability from the AOT entry point.
    ///
    /// Methods without ECMA metadata tokens receive synthetic tokens
    /// (0x80000000 | syntheticIndex) so they still occupy dispatch table
    /// slots reachable by name-based lookup.
    ///
    /// The return value maps NativeSymbol → slot index, matching the
    /// s_hotpatch_entries[] array order from BuildHotpatchTable.
    /// </summary>
    private Dictionary<string, int> BuildDispatchSlotMap(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Use ALL hotpatchable methods, not just reachableMethods, so
        // unreachable methods also get dispatch slots for hotpatch.
        var allMethods = GetHotpatchableMethods();
        int syntheticTokenCounter = 1;

        var entries = new List<(string TypeName, string TypeNamespace, string NativeSymbol, uint Token)>();
        foreach (var method in allMethods)
        {
            string typeSubjectId;
            try
            {
                typeSubjectId = GetMethodDeclaringTypeSubjectId(method.SubjectId);
            }
            catch
            {
                continue;
            }

            var typeName = GetTypeDisplayName(typeSubjectId);
            var typeNamespace = GetTypeNamespace(typeSubjectId);
            uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
            if (token == 0)
            {
                // Assign synthetic token: high bit set to avoid collision with ECMA tokens.
                token = 0x80000000u | (uint)(syntheticTokenCounter++);
            }

            entries.Add((typeName, typeNamespace, method.NativeSymbol, token));
        }

        // Assign slots in entries iteration order, matching the s_hotpatch_entries[]
        // array order from BuildHotpatchTable.
        var result = new Dictionary<string, int>(entries.Count, StringComparer.Ordinal);
        for (int slot = 0; slot < entries.Count; slot++)
        {
            result[entries[slot].NativeSymbol] = slot;
        }

        return result;
    }





    /// <summary>
    /// Returns true if the given subjectId belongs to the current AOT module,
    /// based on the "AssemblyName/..." prefix convention.
    /// </summary>
    private bool IsSameModuleMethod(string subjectId)
    {
        int slashIndex = subjectId.IndexOf('/');
        if (slashIndex < 0)
            return false;
        string subjectAssembly = subjectId.Substring(0, slashIndex);
        return string.Equals(subjectAssembly, _assemblyName, StringComparison.Ordinal);
    }





    /// <summary>
    /// Attempts to resolve a call target as a module-local direct symbol.
    /// Returns true when the callee subjectId is in <see cref="_moduleSymbolTable"/>,
    /// meaning it belongs to the current codegen output and can be called as a
    /// direct C++ function (skipping the extern runtime dispatch table).
    ///
    /// This is a defense-in-depth check: even if <see cref="ResolveDirectInvocationTarget"/>
    /// returns an InvocationTarget with ExternalRuntimeTableIndex set (due to a future
    /// code path), this check ensures same-module calls always use direct symbols.
    /// </summary>
    private bool TryResolveModuleLocalCall(
        AotCoreIrInstructionArtifact instruction,
        InvocationTarget invocationTarget,
        out string nativeSymbol)
    {
        string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (callee != null)
        {
            callee = ManagedNaming.NormalizeSubjectIdAssembly(callee);
            if (_moduleSymbolTable.TryGetValue(callee, out nativeSymbol!))
            {
                // Only use the local symbol when the invocation target doesn't already
                // have a DirectNativeSymbol (which is already optimized) and isn't
                // going through the hotpatch path (handled earlier).
                if (invocationTarget.DirectNativeSymbol == null)
                    return true;
            }
        }
        nativeSymbol = null!;
        return false;
    }





    /// <summary>
    /// Determines whether a call target should use method_table dispatch (cross-module call).
    /// If so, allocates or retrieves the method table index.
    /// </summary>
    private bool TryGetMethodTableIndex(string? callee, string nativeSymbol, out uint index)
    {
        index = 0;
        if (string.IsNullOrEmpty(callee))
            return false;

        // Canonicalize assembly prefix so cross-assembly detection works
        // even when callee and _assemblyName use different aliases
        // (e.g. "System.Runtime.InteropServices" vs "System.Private.CoreLib").
        callee = ManagedNaming.NormalizeSubjectIdAssembly(callee);

        // Extract assembly name from callee SubjectId (format: "AssemblyName/Type::Method").
        int slashIndex = callee.IndexOf('/');
        if (slashIndex < 0)
            return false;

        string calleeAssembly = callee.Substring(0, slashIndex);
        if (string.Equals(calleeAssembly, _assemblyName, StringComparison.Ordinal))
            return false;

        // Cross-module call: allocate or retrieve method table index.
        if (!_methodTableIndices.TryGetValue(callee, out index))
        {
            index = _nextMethodTableIndex++;
            _methodTableIndices[callee] = index;
            _methodTableEntries.Add((index, nativeSymbol));
        }

        return true;
    }





    /// <summary>
    /// Builds C++ code that initializes method table entries for cross-module calls.
    /// Called once during template model creation, after all methods are emitted.
    /// </summary>
    private string BuildMethodTableInitialization()
    {
        var entries = new ScriptObject[_methodTableEntries.Count];
        for (int i = 0; i < _methodTableEntries.Count; i++)
        {
            var (entryIndex, nativeSymbol) = _methodTableEntries[i];
            bool hasManifestIndex = _methodNativeSymbolToManifestIndex.TryGetValue(nativeSymbol, out int manifestIdx);
            var entryModel = new ScriptObject
            {
                ["index"] = entryIndex,
                ["native_symbol"] = nativeSymbol,
                ["has_manifest_index"] = hasManifestIndex,
            };
            if (hasManifestIndex)
            {
                entryModel["manifest_index"] = manifestIdx;
            }
            entries[i] = entryModel;
        }

        var model = new ScriptObject
        {
            ["entries"] = entries,
        };

        var result = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetMethodTableInitializationTemplate(), model);
        return "\n" + result;
    }





    /// <summary>
    /// Computes the set of method subjectIds reachable via AOT call graph
    /// traversal from the given entry point. BFS through call/callvirt/newobj/
    /// ldftn/ldvirtftn instructions, resolving callees through _methodsBySubjectId.
    /// </summary>
    private static HashSet<string> ComputeAotReachableSubjectIds(
        string? entrySubjectId,
        IReadOnlyList<AotCoreIrMethodArtifact> methods,
        string[]? additionalSeeds = null)
    {
        var reachable = new HashSet<string>(StringComparer.Ordinal);
        var queue = new Queue<string>();

        // Seed from entry point
        if (!string.IsNullOrEmpty(entrySubjectId))
        {
            queue.Enqueue(entrySubjectId);
            reachable.Add(entrySubjectId);
        }

        // Seed from additional seeds (subject methods)
        if (additionalSeeds != null)
        {
            foreach (var seed in additionalSeeds)
            {
                if (reachable.Add(seed))
                    queue.Enqueue(seed);
            }
        }

        // BFS through call graph
        var bySubjectId = methods.ToLookup(m => m.SubjectId);
        while (queue.Count > 0)
        {
            var current = queue.Dequeue();
            foreach (var method in bySubjectId[current])
            {
                foreach (var instr in method.Instructions)
                {
                    if (instr.Op is "call" or "callvirt" or "newobj" or "ldftn" or "ldvirtftn")
                    {
                        var callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                        if (!string.IsNullOrEmpty(callee) && reachable.Add(callee))
                            queue.Enqueue(callee);
                    }
                }
            }
        }

        return reachable;
    }
            private static string BuildAotUnreachableMethodStub(AotCoreIrMethodArtifact method)
    {
        var returnAbi = method.ReturnAbi;
        var returnType = MapAbiSlotReturnType(returnAbi);
        var paramAbis = GetMethodAbiParameterSlots(method);
        var paramList = FormatAbiSlotParameterSignature(paramAbis);
        var symbol = method.NativeSymbol;

        // Phase A+B: detect subject methods (Subject_N / CustomEntrySubject_N / CombinedSubjects) that
        // would silently produce empty stubs — WARNING at codegen time, FAIL at runtime.
        bool isSubjectMethod = method.SubjectId is not null &&
            (method.SubjectId.Contains("::Subject_") || method.SubjectId.Contains("::CustomEntrySubject_")
             || method.SubjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal));

        if (isSubjectMethod)
        {
            Console.Error.WriteLine($"[WARNING] Subject method '{method.SubjectId}' is AOT-unreachable — generated body will be empty. Add to --subject-methods or fix reachability.");
        }
        else
        {
            // Non-subject methods silently get return {}; stubs — log once for diagnostics.
            Console.Error.WriteLine($"[STUB] Non-subject method '{method.SubjectId}' emitted as unreachable stub ({method.Instructions.Count} IL instructions)");
        }

        var builder = new StringBuilder();
        builder.AppendLine($"// AOT-unreachable stub: {method.SubjectId}");
        builder.AppendLine($"extern \"C\" {returnType} {symbol}({paramList})");
        builder.AppendLine("{");
        if (!string.IsNullOrEmpty(returnType) && returnType != "void")
        {
            builder.AppendLine($"    return {{}};");
        }
        builder.AppendLine("}");

        // Also emit the generic instantiation stub definition if this method
        // has an InstantiationStubId. The codegen may reference the stub symbol
        // from other method bodies even when this method is not AOT-reachable,
        // so the stub must exist as a valid C++ symbol.
        if (method.InstantiationStubId is not null)
        {
            var stubSymbol = ManagedNaming.CreateInstantiationStubSymbol(method.InstantiationStubId);
            builder.AppendLine();
            builder.AppendLine($"// AOT-unreachable generic instantiation stub: {method.SubjectId}");
            builder.AppendLine($"extern \"C\" {returnType} {stubSymbol}({paramList})");
            builder.AppendLine("{");
            if (!string.IsNullOrEmpty(returnType) && returnType != "void")
            {
                builder.AppendLine($"    return {{}};");
            }
            builder.AppendLine("}");
        }

        return builder.ToString();
    }





    private string BuildMethodSourceSafe(AotCoreIrMethodArtifact method)
    {
        try
        {
            return BuildMethodSource(method);
        }
        catch (Exception ex)
        {
            var msg = $"[codegen] WARNING: codegen failed for {method.SubjectId}, emitting stub. Root cause: {ex.GetType().Name}: {ex.Message}";
            Console.Error.WriteLine(msg);
            CodegenFailureCount++;
            var exType = ex.GetType().Name;
            lock (CodegenFailureByType)
            {
                CodegenFailureByType.TryGetValue(exType, out var ct);
                CodegenFailureByType[exType] = ct + 1;
            }
            var chunk = method.SubjectId?.Split('/').FirstOrDefault() ?? "unknown";
            lock (CodegenFailureByChunk)
            {
                CodegenFailureByChunk.TryGetValue(chunk, out var cc);
                CodegenFailureByChunk[chunk] = cc + 1;
            }
            return BuildAotUnreachableMethodStub(method);
        }
    }





    /// <summary>
    /// Emit one method's body, collecting reverse-P/Invoke entries as a side effect.
    /// Extracted as a separate method to give the JIT a clear stack-cleanup boundary
    /// (avoiding stack accumulation observed with Select().ToList() lambda closure).
    /// </summary>
    private NativeAotMethodTemplateModel EmitOneMethod(
        AotCoreIrMethodArtifact method,
        HashSet<string> aotReachableSubjectIds)
    {
        if (method.IsUnmanagedCallersOnly)
            lock (_reversePInvokeLock) _reversePInvokeEntries.Add((method.SubjectId, method.NativeSymbol));

        return new NativeAotMethodTemplateModel
        {
            SubjectId = method.SubjectId,
            NativeSymbol = method.NativeSymbol,
            MethodSource = aotReachableSubjectIds.Contains(method.SubjectId)
                ? BuildMethodSourceSafe(method)
                : BuildAotUnreachableMethodStub(method),
        };
    }





    private string BuildMethodSource(AotCoreIrMethodArtifact method)
    {
        var builder = StringBuilderPool.Rent(4096);
        if (!string.IsNullOrWhiteSpace(method.OpenDefinitionSubjectId) ||
            method.SharedGenericBodyId is not null ||
            method.InstantiationStubId is not null ||
            method.RuntimeGenericContext is not null)
        {
            builder.AppendLine(FormatGenericExecutionAuthorityComment(
                method.OpenDefinitionSubjectId,
                method.SharedGenericBodyId,
                method.InstantiationStubId,
                method.RuntimeGenericContext));
        }

        EmitManagedMethod(builder, method);
        EmitGenericInstantiationStub(builder, method);
        var result = builder.ToString().TrimEnd();
        StringBuilderPool.Return(builder);
        return result;
    }





    private static void EmitExternalRuntimeHelperDefinitions(
        StringBuilder builder,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        foreach (var helper in externalRuntimeHelpers)
        {
            builder.AppendLine(helper.Source);
            builder.AppendLine();
        }
    }





    private void EmitDelegateRuntimeSupportDefinitions(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        bool hasCombineOrRemove = externalRuntimeHelpers.Any(helper =>
            helper.TargetSymbol.Contains("Delegate__Combine", StringComparison.Ordinal) ||
            helper.TargetSymbol.Contains("Delegate__Remove", StringComparison.Ordinal));
        if (!hasCombineOrRemove)
        {
            return;
        }

        var delegateTypeSubjectIds = CollectReachableDelegateTypeSubjectIds(reachableMethods)
            .Where(subjectId =>
                !string.Equals(subjectId, DelegateTypeSubjectId, StringComparison.Ordinal) &&
                !string.Equals(subjectId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal));

        // Build delegate entry models for the Scriban switch block
        var delegateEntryModels = delegateTypeSubjectIds
            .Select(subjectId =>
            {
                var model = new ScriptObject
                {
                    ["type_id_symbol"] = GetNativeTypeIdSymbol(subjectId),
                    ["native_type_symbol"] = GetNativeTypeSymbol(subjectId),
                    ["type_info_symbol"] = GetNativeTypeInfoSymbol(subjectId),
                    ["has_vtable"] = _vtableTypes?.Contains(subjectId) == true,
                };
                if (_vtableTypes?.Contains(subjectId) == true)
                {
                    model["vtable_symbol"] = GetNativeVTableSymbol(subjectId);
                }
                return model;
            })
            .ToArray();

        var model = new ScriptObject
        {
            ["delegate_entries"] = delegateEntryModels,
        };

        var result = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetDelegateRuntimeSupportTemplate(), model);
        builder.AppendLine(result);

        // ── Emit delegate thunk registrations (for Marshal.GetFunctionPointerForDelegate) ──
        // Build per-delegate-type thunk entries for the DelegateThunks template.
        var delegateThunkModels = delegateTypeSubjectIds
            .Select(subjectId =>
            {
                // Resolve param_count from the delegate's Invoke method signature when available.
                int paramCount = 0;
                if (_methodsByDeclaringType.TryGetValue(subjectId, out var methods))
                {
                    var invokeMethod = methods.FirstOrDefault(m =>
                        string.Equals(GetMethodName(m.SubjectId), "Invoke", StringComparison.Ordinal));
                    if (invokeMethod != null)
                    {
                        paramCount = invokeMethod.ParameterAbis.Count;
                    }
                }
                if (paramCount == 0)
                {
                    // Fallback: extract from Func/Action generic type name.
                    paramCount = ExtractDelegateArityFromSubjectId(subjectId);
                }
                if (paramCount > 8) paramCount = 8;

                var thunkModel = new ScriptObject
                {
                    ["type_id"] = subjectId,
                    ["has_custom_thunk"] = false,
                    ["thunk_symbol"] = "NativeDfnThunkArity" + paramCount,
                    ["native_type_symbol"] = GetNativeTypeSymbol(subjectId),
                    ["param_count"] = paramCount,
                };
                return thunkModel;
            })
            .ToArray();

        if (delegateThunkModels.Length > 0)
        {
            // Forward declarations are emitted by DelegateRuntimeSupport.cpp.scriban
            // (void* parameter variant — intentionally not typed RuntimeState*/ThreadState*
            // because those types are defined in header_layouts.cpp not a header).

            var thunkResult = ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetDelegateThunksTemplate(),
                new ScriptObject { ["delegate_thunks"] = delegateThunkModels });
            builder.AppendLine(thunkResult);
        }
    }





    private static int ExtractDelegateArityFromSubjectId(string subjectId)
    {
        // SubjectId format: "Assembly/TypeName`N[[...]]"
        var slash = subjectId.IndexOf('/');
        if (slash < 0) return 0;
        var typeName = subjectId.Substring(slash + 1);

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

        var baseName = typeName.Substring(0, backtick);
        var lastDot = baseName.LastIndexOf('.');
        var shortName = lastDot >= 0 ? baseName.Substring(lastDot + 1) : baseName;

        return shortName switch
        {
            "Func" when arity >= 1 => arity - 1,
            "Action" => arity,
            _ => 0,
        };
    }





    private IReadOnlyList<string> CollectReachableDelegateTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var subjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference is { Kind: AotCoreIrReferenceKind.Type } targetReference &&
                    IsDelegateTypeSubjectId(targetReference.SubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(targetReference.SubjectId);
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    instruction.Callee.Contains("::Invoke(", StringComparison.Ordinal))
                {
                    var declaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee);
                    if (IsDelegateTypeSubjectId(declaringTypeSubjectId, _referenceTypeBaseSubjectIds))
                    {
                        subjectIds.Add(declaringTypeSubjectId);
                    }
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetFunctionPointerForDelegateMethodPrefix,
                        out var marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetDelegateForFunctionPointerMethodPrefix,
                        out marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }
            }
        }

        if (subjectIds.Count > 0)
        {
            subjectIds.Add(DelegateTypeSubjectId);
            subjectIds.Add(MulticastDelegateTypeSubjectId);
        }

        return subjectIds
            .OrderBy(subjectId => subjectId, StringComparer.Ordinal)
            .ToArray();
    }





    private static IReadOnlyDictionary<string, string> BuildClosureAssemblyPathByNameCore(
        IReadOnlyList<string> closureAssemblyPaths)
    {
        var pathsByAssemblyName = new Dictionary<string, string>(closureAssemblyPaths.Count, StringComparer.Ordinal);
        foreach (var assemblyPath in closureAssemblyPaths)
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
            pathsByAssemblyName[assemblyName] = assemblyPath;
        }

        return pathsByAssemblyName;
    }





    private static bool TryParseStaticFieldDataSize(
        string memberType,
        out int size)
    {
        const string marker = "__StaticArrayInitTypeSize=";
        size = 0;
        var markerIndex = memberType.IndexOf(marker, StringComparison.Ordinal);
        if (markerIndex < 0)
        {
            return false;
        }

        return int.TryParse(
            memberType[(markerIndex + marker.Length)..],
            NumberStyles.None,
            CultureInfo.InvariantCulture,
            out size) &&
               size >= 0;
    }

}

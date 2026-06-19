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
        IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        var reachable = new HashSet<string>(StringComparer.Ordinal);
        if (string.IsNullOrEmpty(entrySubjectId))
            return reachable;

        var bySubjectId = methods
            .ToLookup(m => m.SubjectId, StringComparer.Ordinal);

        var queue = new Queue<string>();
        queue.Enqueue(entrySubjectId);
        reachable.Add(entrySubjectId);

        while (queue.Count > 0)
        {
            var current = queue.Dequeue();
            if (!bySubjectId.Contains(current))
                continue;

            foreach (var method in bySubjectId[current])
            {
                if (method.Instructions == null)
                    continue;

                foreach (var instr in method.Instructions)
                {
                    string op = instr.Op;
                    if (op != "call" && op != "callvirt" && op != "newobj" &&
                        op != "ldftn" && op != "ldvirtftn")
                        continue;

                    string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    if (string.IsNullOrEmpty(callee))
                        continue;

                    // Add both the exact callee and any open-definition variant
                    if (reachable.Add(callee))
                        queue.Enqueue(callee);

                    // Also follow to the resolved instantiation if available
                    if (instr.TargetReference?.SubjectId is { } targetRef &&
                        targetRef != callee &&
                        reachable.Add(targetRef))
                        queue.Enqueue(targetRef);
                }
            }
        }

        return reachable;
    }





    /// <summary>
    /// Builds a minimal C++ function body stub for AOT-unreachable methods.
    /// Unreachable methods still need a dispatchable entry point (for the
    /// interpreter dispatch table) but do not require a full native body.
    /// Returns a default value to avoid crashing when the fact loop runs
    /// every dispatch-table entry including unreachable interface stubs.
    /// </summary>
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
            _reversePInvokeEntries.Add((method.SubjectId, method.NativeSymbol));

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
        var builder = new StringBuilder(4096);
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
        return builder.ToString().TrimEnd();
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





    private static bool TryLoadStaticFieldDataBytes(
        string assemblyPath,
        string fieldSubjectId,
        int size,
        out IReadOnlyList<byte> bytes)
    {
        bytes = Array.Empty<byte>();
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            return false;
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        if (!TryResolveFieldDefinitionHandle(
                metadataReader,
                assemblyName,
                fieldSubjectId,
                out var fieldHandle))
        {
            return false;
        }

        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
        var relativeVirtualAddress = fieldDefinition.GetRelativeVirtualAddress();
        if (relativeVirtualAddress <= 0)
        {
            return false;
        }

        var sectionData = peReader.GetSectionData(relativeVirtualAddress);
        if (sectionData.Length < size)
        {
            return false;
        }

        bytes = sectionData.GetReader(0, size).ReadBytes(size);
        return true;
    }





    private static bool TryResolveFieldDefinitionHandle(
        MetadataReader metadataReader,
        string assemblyName,
        string fieldSubjectId,
        out FieldDefinitionHandle fieldHandle)
    {
        fieldHandle = default;
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(fieldSubjectId);
        var fieldName = GetFieldName(fieldSubjectId);

        foreach (var candidateTypeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    assemblyName,
                    candidateTypeHandle,
                    out var typeIdentity) ||
                !string.Equals(typeIdentity.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal))
            {
                continue;
            }

            var typeDefinition = metadataReader.GetTypeDefinition(candidateTypeHandle);
            foreach (var candidateFieldHandle in typeDefinition.GetFields())
            {
                var candidateFieldDefinition = metadataReader.GetFieldDefinition(candidateFieldHandle);
                if (string.Equals(
                        metadataReader.GetString(candidateFieldDefinition.Name),
                        fieldName,
                        StringComparison.Ordinal))
                {
                    fieldHandle = candidateFieldHandle;
                    return true;
                }
            }

            return false;
        }

        return false;
    }





    private static bool TryResolveTypeDefinitionHandleForReflectionMemberEntry(
        SupplementalMetadataTypeTemplateEntry typeEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out TypeDefinitionHandle typeDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(typeEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrEmpty(typeEntry.DefinitionSubjectId) ||
            string.Equals(typeEntry.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal))
        {
            typeDefinitionHandle = default;
            return false;
        }

        var definitionEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal));
        if (definitionEntry is null)
        {
            typeDefinitionHandle = default;
            return false;
        }

        if (TryCreateMetadataEntityHandle(definitionEntry.MetadataToken, out handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        typeDefinitionHandle = default;
        return false;
    }





    private static bool TryResolveMethodDefinitionForReflectionMemberEntry(
        MetadataReader metadataReader,
        SupplementalMetadataMethodTemplateEntry methodEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out MethodDefinitionHandle methodDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(methodEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.MethodDefinition)
        {
            methodDefinitionHandle = (MethodDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrEmpty(methodEntry.DefinitionSubjectId))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var definitionDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(methodEntry.DefinitionSubjectId);
        var definitionTypeEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, definitionDeclaringTypeSubjectId, StringComparison.Ordinal));
        if (definitionTypeEntry is null ||
            !TryResolveTypeDefinitionHandleForReflectionMemberEntry(
                definitionTypeEntry,
                assemblyTypeEntries,
                out var typeDefinitionHandle))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var metadataMethodName = GetMetadataMethodNameForReflectionMemberDefinition(methodEntry.DefinitionSubjectId);
        var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
        foreach (var candidateHandle in typeDefinition.GetMethods())
        {
            var candidateDefinition = metadataReader.GetMethodDefinition(candidateHandle);
            if (!string.Equals(
                    metadataReader.GetString(candidateDefinition.Name),
                    metadataMethodName,
                    StringComparison.Ordinal))
            {
                continue;
            }

            if (GetMethodParameterCount(metadataReader, candidateDefinition) != methodEntry.ParameterCount)
            {
                continue;
            }

            methodDefinitionHandle = candidateHandle;
            return true;
        }

        methodDefinitionHandle = default;
        return false;
    }





    private static bool TryCreateMetadataEntityHandle(int metadataToken, out EntityHandle handle)
    {
        if (metadataToken == 0)
        {
            handle = default;
            return false;
        }

        try
        {
            handle = MetadataTokens.EntityHandle(metadataToken);
            return !handle.IsNil;
        }
        catch (ArgumentException)
        {
            handle = default;
            return false;
        }
    }





    private static string GetMetadataMethodNameForReflectionMemberDefinition(string methodSubjectId)
    {
        var methodName = GetMethodName(methodSubjectId);
        var genericArgumentIndex = methodName.IndexOf('<');
        if (genericArgumentIndex >= 0)
        {
            methodName = methodName[..genericArgumentIndex];
        }

        return ManagedNaming.StripGenericArity(methodName);
    }





    private static int GetMethodParameterCount(MetadataReader metadataReader, MethodDefinition methodDefinition)
    {
        return methodDefinition.GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Count(parameter => parameter.SequenceNumber > 0);
    }





    private void CollectCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string currentAssemblyName,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        uint entityMetadataToken,
        CustomAttributeHandleCollection attributeHandles,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        foreach (var attributeHandle in attributeHandles)
        {
            if (!TryGetAttributeTypeIdentity(metadataReader, currentAssemblyName, attributeHandle, out var attributeTypeIdentity))
            {
                continue;
            }

            var isExplicitQuery = queriedDisplayNames.Contains(attributeTypeIdentity.DisplayName);
            var isRequestedByIsDefined = memberInfoIsDefinedAttributeTypeSubjectIds.Contains(attributeTypeIdentity.SubjectId);
            if (!isExplicitQuery && !isRequestedByIsDefined)
            {
                continue;
            }

            if (isExplicitQuery)
            {
                RegisterCustomAttributeTypeSubjectId(
                    attributeTypeIdentity.DisplayName,
                    attributeTypeIdentity.SubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)targetKind}:{targetSubjectId}:{attributeTypeIdentity.SubjectId}";
            if (!materializationKeys.Add(key))
            {
                continue;
            }

            materializations.Add(CreateCustomAttributeMaterializationPlan(
                metadataReader,
                targetSubjectId,
                targetKind,
                entityMetadataToken,
                attributeHandle,
                attributeTypeIdentity.SubjectId));
        }
    }





    private void CollectSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string assemblyName,
        string targetSubjectId,
        uint methodToken,
        MethodDefinition methodDefinition,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        if ((memberInfoIsDefinedAttributeTypeSubjectIds.Contains(DllImportAttributeTypeSubjectId) ||
             queriedDisplayNames.Contains(DllImportAttributeDisplayName)) &&
            methodDefinition.Attributes.HasFlag(MethodAttributes.PinvokeImpl))
        {
            if (queriedDisplayNames.Contains(DllImportAttributeDisplayName))
            {
                RegisterCustomAttributeTypeSubjectId(
                    DllImportAttributeDisplayName,
                    DllImportAttributeTypeSubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)CustomAttributeTargetKind.Method}:{targetSubjectId}:{DllImportAttributeTypeSubjectId}";
            if (materializationKeys.Add(key))
            {
                materializations.Add(CreateDllImportAttributeMaterializationPlan(metadataReader, targetSubjectId, methodDefinition, methodToken));
            }
        }

        if (!memberInfoIsDefinedAttributeTypeSubjectIds.Contains(UnmanagedCallersOnlyAttributeTypeSubjectId) &&
            !queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
        {
            return;
        }

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (!TryGetAttributeTypeIdentity(
                    metadataReader,
                    assemblyName,
                    attributeHandle,
                    out var attributeTypeIdentity) ||
                !string.Equals(
                    attributeTypeIdentity.SubjectId,
                    UnmanagedCallersOnlyAttributeTypeSubjectId,
                    StringComparison.Ordinal))
            {
                continue;
            }

            if (queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
            {
                RegisterCustomAttributeTypeSubjectId(
                    UnmanagedCallersOnlyAttributeDisplayName,
                    UnmanagedCallersOnlyAttributeTypeSubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)CustomAttributeTargetKind.Method}:{targetSubjectId}:{UnmanagedCallersOnlyAttributeTypeSubjectId}";
            if (!materializationKeys.Add(key))
            {
                continue;
            }

            materializations.Add(CreateCustomAttributeMaterializationPlan(
                metadataReader,
                targetSubjectId,
                CustomAttributeTargetKind.Method,
                methodToken,
                attributeHandle,
                UnmanagedCallersOnlyAttributeTypeSubjectId));
        }
    }





    private void CollectClosureWideSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string assemblyName,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        if (!memberInfoIsDefinedAttributeTypeSubjectIds.Contains(DllImportAttributeTypeSubjectId) &&
            !memberInfoIsDefinedAttributeTypeSubjectIds.Contains(UnmanagedCallersOnlyAttributeTypeSubjectId) &&
            !queriedDisplayNames.Contains(DllImportAttributeDisplayName) &&
            !queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
        {
            return;
        }

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    assemblyName,
                    typeHandle,
                    out var declaringTypeIdentity))
            {
                continue;
            }

            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                if (!TryCreateMetadataMethodSubjectId(
                        metadataReader,
                        assemblyName,
                        declaringTypeIdentity.SubjectId,
                        methodHandle,
                        out var targetSubjectId) ||
                    string.IsNullOrEmpty(targetSubjectId))
                {
                    continue;
                }

                CollectSyntheticMethodCustomAttributeMaterializations(
                    metadataReader,
                    assemblyName,
                    targetSubjectId!,
                    (uint)MetadataTokens.GetToken(methodHandle),
                    metadataReader.GetMethodDefinition(methodHandle),
                    queriedDisplayNames,
                    memberInfoIsDefinedAttributeTypeSubjectIds,
                    displayNameToSubjectId,
                    materializations,
                    materializationKeys);
            }
        }
    }





    private static bool TryCreateMetadataMethodSubjectId(
        MetadataReader metadataReader,
        string assemblyName,
        string declaringTypeSubjectId,
        MethodDefinitionHandle methodHandle,
        out string? methodSubjectId)
    {
        methodSubjectId = null;
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
        var signature = methodDefinition.DecodeSignature(
            new MetadataMethodSignatureTypeNameProvider(metadataReader, assemblyName),
            genericContext: null);
        var methodName = metadataReader.GetString(methodDefinition.Name);
        if (string.IsNullOrEmpty(methodName) ||
            string.IsNullOrEmpty(signature.ReturnType))
        {
            return false;
        }

        methodSubjectId = ManagedNaming.CreateMethodSubjectId(
            declaringTypeSubjectId,
            methodName,
            signature.ReturnType,
            signature.ParameterTypes.ToArray(),
            methodDefinition.GetGenericParameters().Count);
        return true;
    }





    private CustomAttributeMaterializationPlan CreateDllImportAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        MethodDefinition methodDefinition,
        uint methodToken)
    {
        var import = methodDefinition.GetImport();
        var moduleReference = metadataReader.GetModuleReference(import.Module);
        var moduleName = metadataReader.GetString(moduleReference.Name);
        var entryPointName = import.Name.IsNil
            ? metadataReader.GetString(methodDefinition.Name)
            : metadataReader.GetString(import.Name);
        var assignments = new List<CustomAttributeFieldAssignment>
        {
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "Value"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, moduleName)),
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "EntryPoint"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, entryPointName)),
        };

        if (import.Attributes.HasFlag(MethodImportAttributes.ExactSpelling))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "ExactSpelling"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        if (import.Attributes.HasFlag(MethodImportAttributes.SetLastError))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "SetLastError"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        // P1.3: CallingConvention mapping (MethodImportAttributes → System.Runtime.InteropServices.CallingConvention).
        var ccBits = import.Attributes & MethodImportAttributes.CallingConventionMask;
        if (ccBits != 0) // 0 = WinApi (default, skip)
        {
            int ccValue = ccBits switch
            {
                MethodImportAttributes.CallingConventionWinApi => 1,   // CallingConvention.WinApi
                MethodImportAttributes.CallingConventionCDecl => 2,    // CallingConvention.CDecl
                MethodImportAttributes.CallingConventionStdCall => 3,  // CallingConvention.StdCall
                MethodImportAttributes.CallingConventionThisCall => 4, // CallingConvention.ThisCall
                MethodImportAttributes.CallingConventionFastCall => 5, // CallingConvention.FastCall
                _ => 1, // fallback: WinApi
            };
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "CallingConvention"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, ccValue)));
        }

        // P1.3: CharSet mapping (MethodImportAttributes → System.Runtime.InteropServices.CharSet).
        var csBits = import.Attributes & MethodImportAttributes.CharSetMask;
        // 0x0000 = default (Ansi, skip); explicit Ansi=0x0002, Unicode=0x0004, Auto=0x0006
        if (csBits != 0)
        {
            int csValue = csBits switch
            {
                MethodImportAttributes.CharSetAnsi => 2,    // CharSet.Ansi
                (MethodImportAttributes)0x0004 => 3,        // CharSet.Unicode
                MethodImportAttributes.CharSetAuto => 4,    // CharSet.Auto
                _ => 2, // fallback: Ansi
            };
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "CharSet"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, csValue)));
        }

        return new CustomAttributeMaterializationPlan(
            CustomAttributeTargetKind.Method,
            methodToken,
            targetSubjectId,
            DllImportAttributeTypeSubjectId,
            assignments);
    }





    private CustomAttributeMaterializationPlan CreateCustomAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        uint entityMetadataToken,
        CustomAttributeHandle attributeHandle,
        string attributeTypeSubjectId)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        var decodedValue = attribute.DecodeValue(NativeAotCustomAttributeTypeProvider.Instance);
        var constructorParameterNames = GetAttributeConstructorParameterNames(metadataReader, attribute.Constructor);
        var assignments = new List<CustomAttributeFieldAssignment>();

        for (var index = 0; index < decodedValue.FixedArguments.Length; index++)
        {
            var memberName = ResolveFixedArgumentMemberName(
                attributeTypeSubjectId,
                constructorParameterNames,
                index,
                decodedValue.FixedArguments.Length);
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, memberName),
                CreateCustomAttributeLiteralValue(decodedValue.FixedArguments[index].Value)));
        }

        foreach (var namedArgument in decodedValue.NamedArguments)
        {
            if (string.IsNullOrEmpty(namedArgument.Name))
            {
                throw new NotSupportedException(
                    $"native-aot custom-attribute materialization found an unnamed argument on '{attributeTypeSubjectId}'.");
            }

            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, namedArgument.Name),
                CreateCustomAttributeLiteralValue(namedArgument.Value)));
        }

        return new CustomAttributeMaterializationPlan(
            targetKind,
            entityMetadataToken,
            targetSubjectId,
            attributeTypeSubjectId,
            assignments);
    }





    private static IReadOnlySet<string> CollectMemberInfoIsDefinedAttributeTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var attributeTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            for (var index = 0; index < method.Instructions.Count; index++)
            {
                var instruction = method.Instructions[index];
                if (!MatchesMethodSubject(
                        instruction.Callee ?? string.Empty,
                        "System.Private.CoreLib/System.Reflection.MemberInfo",
                        "IsDefined",
                        "System.Type",
                        "System.Boolean"))
                {
                    continue;
                }

                if (!TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(method.Instructions, index, out var attributeTypeSubjectId) ||
                    string.IsNullOrEmpty(attributeTypeSubjectId))
                {
                    continue;
                }

                attributeTypeSubjectIds.Add(attributeTypeSubjectId!);
            }
        }

        return attributeTypeSubjectIds;
    }





    private static bool TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int callIndex,
        out string? attributeTypeSubjectId)
    {
        attributeTypeSubjectId = null;
        if (callIndex < 3 ||
            !string.Equals(instructions[callIndex - 1].Op, "ldc.i4", StringComparison.Ordinal) ||
            !MatchesMethodSubject(
                instructions[callIndex - 2].Callee ?? string.Empty,
                "System.Private.CoreLib/System.Type",
                "GetTypeFromHandle",
                "System.RuntimeTypeHandle"))
        {
            return false;
        }

        var loadTokenInstruction = instructions[callIndex - 3];
        if (!string.Equals(loadTokenInstruction.Op, "ldtoken", StringComparison.Ordinal))
        {
            return false;
        }

        if (loadTokenInstruction.TargetReference?.Kind == AotCoreIrReferenceKind.Type &&
            !string.IsNullOrEmpty(loadTokenInstruction.TargetReference.SubjectId))
        {
            attributeTypeSubjectId = loadTokenInstruction.TargetReference.SubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is string directSubjectId &&
            !string.IsNullOrEmpty(directSubjectId))
        {
            attributeTypeSubjectId = directSubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is JsonElement { ValueKind: JsonValueKind.String } element)
        {
            var jsonSubjectId = element.GetString();
            if (!string.IsNullOrEmpty(jsonSubjectId))
            {
                attributeTypeSubjectId = jsonSubjectId;
                return true;
            }
        }

        return false;
    }





    private static IReadOnlyList<string> GetAttributeConstructorParameterNames(
        MetadataReader metadataReader,
        EntityHandle constructorHandle)
    {
        if (constructorHandle.Kind != HandleKind.MethodDefinition)
        {
            return [];
        }

        var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
        return methodDefinition.GetParameters()
            .Select(handle => metadataReader.GetString(metadataReader.GetParameter(handle).Name))
            .Where(name => !string.IsNullOrEmpty(name))
            .ToArray();
    }





    private static void RegisterCustomAttributeTypeSubjectId(
        string displayName,
        string subjectId,
        IDictionary<string, string> displayNameToSubjectId)
    {
        if (displayNameToSubjectId.TryGetValue(displayName, out var existingSubjectId) &&
            !string.Equals(existingSubjectId, subjectId, StringComparison.Ordinal))
        {
            return;
        }

        displayNameToSubjectId[displayName] = subjectId;
    }





    private string ResolveFixedArgumentMemberName(
        string attributeTypeSubjectId,
        IReadOnlyList<string> constructorParameterNames,
        int argumentIndex,
        int totalArgumentCount)
    {
        if (argumentIndex < constructorParameterNames.Count &&
            !string.IsNullOrEmpty(constructorParameterNames[argumentIndex]))
        {
            var parameterName = constructorParameterNames[argumentIndex];
            return char.ToUpperInvariant(parameterName[0]) + parameterName[1..];
        }

        if (totalArgumentCount == 1)
        {
            return "Value";
        }

        throw new NotSupportedException(
            $"native-aot custom-attribute materialization could not bind fixed argument #{argumentIndex} for '{attributeTypeSubjectId}'.");
    }





    private string ResolveAttributeStorageField(
        string attributeTypeSubjectId,
        string memberName)
    {
        var key = $"{attributeTypeSubjectId}:{memberName}";
        if (_attributeStorageFieldIndex.TryGetValue(key, out var cached))
        {
            return cached;
        }

        return ManagedNaming.CreateFieldSubjectId(attributeTypeSubjectId, memberName);
    }





    private static Dictionary<string, string> BuildAttributeStorageFieldIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, string>(methodsBySubjectId.Count, StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            if (method.IsStatic || method.ParameterCount != 0)
            {
                continue;
            }

            // Check if this method is a property getter (auto-property pattern)
            var getterMemberName = ExtractGetterMemberName(method);
            if (getterMemberName is null)
            {
                continue;
            }

            if (!TryGetAutoGetterStorageFieldSubjectId(method, out var fieldSubjectId) ||
                string.IsNullOrEmpty(fieldSubjectId))
            {
                continue;
            }

            var declaringType = method.Identity.DeclaringTypeSubjectId;
            var key = $"{declaringType}:{getterMemberName}";
            index.TryAdd(key, fieldSubjectId!);
        }

        return index;
    }





    private static string? ExtractGetterMemberName(AotCoreIrMethodArtifact method)
    {
        var subjectId = method.SubjectId;
        var getterPrefix = "::get_";
        var getterSuffix = ":";
        var startIndex = subjectId.IndexOf(getterPrefix, StringComparison.Ordinal);
        if (startIndex < 0)
        {
            return null;
        }

        startIndex += getterPrefix.Length;
        var endIndex = subjectId.IndexOf(getterSuffix, startIndex, StringComparison.Ordinal);
        if (endIndex < 0)
        {
            return null;
        }

        return subjectId[startIndex..endIndex];
    }





    private static bool TryGetAutoGetterStorageFieldSubjectId(
        AotCoreIrMethodArtifact method,
        out string? fieldSubjectId)
    {
        fieldSubjectId = null;
        if (method.IsStatic || method.ParameterCount != 0)
        {
            return false;
        }

        var fieldLoads = method.Instructions
            .Where(instruction =>
                string.Equals(instruction.Op, "ldfld", StringComparison.Ordinal) &&
                instruction.TargetReference?.Kind == AotCoreIrReferenceKind.Field &&
                !string.IsNullOrWhiteSpace(instruction.TargetReference.SubjectId))
            .Select(instruction => instruction.TargetReference!.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToArray();
        if (fieldLoads.Length != 1)
        {
            return false;
        }

        fieldSubjectId = fieldLoads[0];
        return true;
    }





    private static CustomAttributeLiteralValue CreateCustomAttributeLiteralValue(object? value)
    {
        return value switch
        {
            null => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Null, null),
            bool booleanValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, booleanValue),
            byte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Byte, byteValue),
            sbyte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.SByte, byteValue),
            short shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, shortValue),
            int intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, intValue),
            long longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int64, longValue),
            ushort shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, shortValue),
            uint intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt32, intValue),
            ulong longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt64, longValue),
            float floatValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Single, floatValue),
            double doubleValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Double, doubleValue),
            char charValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Char, charValue),
            string stringValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, stringValue),
            _ => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String,
                value.GetType().FullName switch
                {
                    not null when value.GetType().FullName!.StartsWith("System.Collections.Immutable.ImmutableArray")
                        => $"(immutable-array:{value})",
                    not null => $"(literal:{value})",
                    null => $"(literal)",
                }),
        };
    }





    private static IEnumerable<string> EnumerateClosureAssemblyPaths(
        ManagedClosureManifestArtifact closureManifest)
    {
        if (closureManifest.ResolvedAssemblies is { Count: > 0 })
        {
            foreach (var resolvedAssembly in closureManifest.ResolvedAssemblies)
            {
                if (!string.IsNullOrEmpty(resolvedAssembly.Path))
                {
                    yield return Path.GetFullPath(resolvedAssembly.Path);
                }
            }

            yield break;
        }

        yield return Path.GetFullPath(closureManifest.InputAssemblyPath);

        if (closureManifest.AdditionalAssemblyPaths is null)
        {
            yield break;
        }

        foreach (var assemblyPath in closureManifest.AdditionalAssemblyPaths)
        {
            yield return Path.GetFullPath(assemblyPath);
        }
    }

}

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
    /// Phase 0: Collect ModuleRegistry Tier 0 type data from the assembly PE metadata.
    /// Populates _moduleTypeFlags, _moduleTypeNames, _moduleTypeNamespaces,
    /// _moduleTypeParentTokens, _moduleTypeInfoSymbols indexed by TokenToIndex(token).
    /// On failure (e.g., missing assembly), Tier 0 arrays remain empty — the runtime
    /// falls back to Tier 2 metadata (ReflectionQueryImageDescriptor).
    /// </summary>
    private void CollectModuleTypeData(string assemblyPath)
    {
        if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
            return;

        try
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
                return;

            var metadataReader = peReader.GetMetadataReader();

            // Verify the metadata assembly name matches our lowering plan
            var assemblyDef = metadataReader.GetAssemblyDefinition();
            var metadataAssemblyName = metadataReader.GetString(assemblyDef.Name);
            if (!string.Equals(metadataAssemblyName, _assemblyName, StringComparison.Ordinal))
                return;

            // Enumerate all TypeDef entries (row-indexed by ECMA TypeDef table)
            var nestedTypeMap = new Dictionary<uint, List<uint>>();
            var typeTokenToConstraints = new Dictionary<uint, List<uint>>();
            foreach (var handle in metadataReader.TypeDefinitions)
            {
                var typeDef = metadataReader.GetTypeDefinition(handle);
                var name = metadataReader.GetString(typeDef.Name);
                var ns = metadataReader.GetString(typeDef.Namespace);
                var attributes = typeDef.Attributes;
                var parentHandle = typeDef.BaseType;

                // Compute type flags
                uint flags = ComputeTypeFlags(metadataReader, typeDef, parentHandle);

                // Compute subjectId for cross-referencing with emitted TypeInfo set
                string subjectId = ComputeTypeDefSubjectId(metadataReader, handle, _assemblyName);

                // Parent token (same-assembly TypeDef only; cross-assembly → 0)
                uint parentToken = 0;
                if (!parentHandle.IsNil && parentHandle.Kind == HandleKind.TypeDefinition)
                    parentToken = (uint)MetadataTokens.GetToken(parentHandle);

                // Check if this type has a TypeInfo emitted (reachable types only)
                string? typeInfoSymbol = _allEmittedTypeSubjectIds?.Contains(subjectId) == true
                    ? GetNativeTypeInfoSymbol(subjectId)
                    : null;

                // Nested type relationship: record parent → child token
                var declaringHandle = typeDef.GetDeclaringType();
                if (!declaringHandle.IsNil)
                {
                    uint declaringToken = (uint)MetadataTokens.GetToken(declaringHandle);
                    uint childToken = (uint)MetadataTokens.GetToken(handle);
                    if (!nestedTypeMap.TryGetValue(declaringToken, out var childList))
                    {
                        childList = new List<uint>();
                        nestedTypeMap[declaringToken] = childList;
                    }
                    childList.Add(childToken);
                }

                // Generic param constraint extraction: build per-type flat array
                // of (param_index << 29 | constraint_token) entries.
                uint typeToken = (uint)MetadataTokens.GetToken(handle);
                var gpHandles = typeDef.GetGenericParameters();
                if (gpHandles.Count > 0)
                {
                    var constraintEntries = new List<uint>();
                    foreach (var gpHandle in gpHandles)
                    {
                        var gp = metadataReader.GetGenericParameter(gpHandle);
                        int paramIdx = gp.Index;
                        foreach (var gpcHandle in gp.GetConstraints())
                        {
                            var gpc = metadataReader.GetGenericParameterConstraint(gpcHandle);
                            uint ct = (uint)MetadataTokens.GetToken(gpc.Type);
                            constraintEntries.Add(((uint)paramIdx << 29) | (ct & 0x1FFFFFFFu));
                        }
                    }
                    if (constraintEntries.Count > 0)
                        typeTokenToConstraints[typeToken] = constraintEntries;
                }

                _moduleTypeFlags.Add(flags);
                _moduleTypeNames.Add(name);
                _moduleTypeNamespaces.Add(ns);
                _moduleTypeParentTokens.Add(parentToken);
                _moduleTypeInfoSymbols.Add(typeInfoSymbol);
                _moduleTypeSubjectIds.Add(subjectId);
                _moduleTypeCount++;
            }

            // Build prefix-sum nested type arrays from the collected parent→children map.
            // Type index i corresponds to token 0x02000000 | (i + 1).
            for (int i = 0; i < _moduleTypeCount; i++)
            {
                uint typeToken = 0x02000000u | (uint)(i + 1);
                _moduleNestedTypeOffsets.Add((uint)_moduleNestedTypeChildren.Count);
                if (nestedTypeMap.TryGetValue(typeToken, out var children))
                {
                    _moduleNestedTypeChildren.AddRange(children);
                }
            }
            _moduleNestedTypeOffsets.Add((uint)_moduleNestedTypeChildren.Count);

            // Build prefix-sum generic param constraint arrays (same token→index mapping).
            for (int i = 0; i < _moduleTypeCount; i++)
            {
                uint typeToken = 0x02000000u | (uint)(i + 1);
                _moduleGenericParamConstraintOffsets.Add((uint)_moduleGenericParamConstraintData.Count);
                if (typeTokenToConstraints.TryGetValue(typeToken, out var entries))
                {
                    _moduleGenericParamConstraintData.AddRange(entries);
                }
            }
            _moduleGenericParamConstraintOffsets.Add((uint)_moduleGenericParamConstraintData.Count);
        }
        catch (Exception ex) when (ex is IOException or InvalidOperationException or BadImageFormatException)
        {
            // Graceful fallback: Tier 0 arrays remain empty (code emits nullptr)
            System.Console.Error.WriteLine(
                $"[warning] CollectModuleTypeData: failed to read assembly metadata from '{assemblyPath}': {ex.GetType().Name}: {ex.Message}");
            _moduleTypeCount = 0;
            _moduleTypeFlags.Clear();
            _moduleTypeNames.Clear();
            _moduleTypeNamespaces.Clear();
            _moduleTypeParentTokens.Clear();
            _moduleTypeInfoSymbols.Clear();
            _moduleNestedTypeChildren.Clear();
            _moduleNestedTypeOffsets.Clear();
            _moduleGenericParamConstraintData.Clear();
            _moduleGenericParamConstraintOffsets.Clear();
        }
    }



    /// <summary>
    /// Resolve a TypeDef/TypeRef parent handle to a "Namespace.Name" string.
    /// Returns null for nil handles or unsupported handle kinds (TypeSpec).
    /// </summary>



    /// <summary>
    /// Compute the type-flags bitmask for a TypeDef entry.
    /// Flags correspond to the kFlag* constants in module_registry.h.
    /// </summary>



    /// <summary>
    /// Compute the SubjectId for a TypeDef from its metadata row.
    /// Format: "AssemblyName/Namespace.TypeName" or "AssemblyName/DeclaringType+NestedName".
    /// </summary>



    /// <summary>
    /// Build a NativeSymbol → dispatch table slot index mapping by replicating
    /// the same sorting logic used in <see cref="BuildHotpatchTable"/>.
    ///
    /// Only methods with metadata tokens are included (the only ones
    /// that appear in s_hotpatch_entries).
    /// </summary>
    // ── Pre-computed O(1) index builders for invocation planning ──



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



    /// <summary>
    /// Returns true if the given subjectId belongs to the current AOT module,
    /// based on the "AssemblyName/..." prefix convention.
    /// </summary>



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



    /// <summary>
    /// Determines whether a call target should use method_table dispatch (cross-module call).
    /// If so, allocates or retrieves the method table index.
    /// </summary>



    /// <summary>
    /// Builds C++ code that initializes method table entries for cross-module calls.
    /// Called once during template model creation, after all methods are emitted.
    /// </summary>



    /// <summary>
    /// Computes the set of method subjectIds reachable via AOT call graph
    /// traversal from the given entry point. BFS through call/callvirt/newobj/
    /// ldftn/ldvirtftn instructions, resolving callees through _methodsBySubjectId.
    /// </summary>



    /// <summary>
    /// Builds a minimal C++ function body stub for AOT-unreachable methods.
    /// Unreachable methods still need a dispatchable entry point (for the
    /// interpreter dispatch table) but do not require a full native body.
    /// Returns a default value to avoid crashing when the fact loop runs
    /// every dispatch-table entry including unreachable interface stubs.
    /// </summary>



    /// <summary>
    /// Emit one method's body, collecting reverse-P/Invoke entries as a side effect.
    /// Extracted as a separate method to give the JIT a clear stack-cleanup boundary
    /// (avoiding stack accumulation observed with Select().ToList() lambda closure).
    /// </summary>

}

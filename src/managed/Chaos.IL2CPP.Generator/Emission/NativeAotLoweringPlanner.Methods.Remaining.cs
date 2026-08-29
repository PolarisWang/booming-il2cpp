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

    private static string SanitizeCppIdentifier(string name)
    {
        // Replace non-alphanumeric characters (except underscore) with underscores
        // to produce a valid C++ identifier from an assembly name.
        var sb = new System.Text.StringBuilder(name.Length);
        foreach (char c in name)
        {
            sb.Append(char.IsLetterOrDigit(c) ? c : '_');
        }
        return sb.ToString();
    }



    /// <summary>
    /// Builds C++ declarations for the shared header (native-aot.generated.header.h).
    /// When TU paging is active, each translation unit page needs visibility to
    /// all type symbols, static variables, and runtime helpers defined on other
    /// pages without ODR violations. This method emits:
    ///   - struct chaos_type_&lt;id&gt;; forward declarations
    ///   - struct chaos_boxed_type_&lt;id&gt;; forward declarations
    ///   - struct chaos_valuetype_&lt;id&gt;; forward declarations
    ///   - extern MethodTable chaos_mt_&lt;id&gt;; (not TypeInfoV0 — MethodTable is the
    ///     actual definition type; &lt;chaos/type_info.h&gt; brings it into scope)
    ///   - extern "C" kChaosExternalRuntimeFnTable[]; (if bridge thunks use it)
    ///   - extern "C" HotpatchEntryV0 s_hotpatch_entries[]; (if dispatch slots exist)
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_static_&lt;id&gt;; for each static field
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_string_materialize(...); (if string IDs)
    ///   - extern bool chaos_is_array_store_compatible(...);
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_*(...);
    ///     (if interpolated string helpers are reachable)
    ///   - extern declarations for all chaos_external_runtime_* helpers
    /// </summary>
    /// <param name="codegenNamespace">C++ namespace for the codegen (e.g. "CombinedSubjects").
    /// Declarations are wrapped in `namespace chaos::il2cpp::codegen::{codegenNamespace}` to
    /// match the definition namespace on page 0, avoiding linker unresolved externals.</param>
    private string BuildTypeDeclarationsCode(string codegenNamespace, IReadOnlySet<string>? extraValuetypes = null)
    {
        if (_allEmittedTypeSubjectIds is not { Count: > 0 })
            return string.Empty;

        var sb = new System.Text.StringBuilder();
        sb.AppendLine("// Auto-generated type declarations (TU paging)");
        sb.AppendLine("#pragma once");
        sb.AppendLine();
        sb.AppendLine("#include <chaos/native_types.h>");
        sb.AppendLine("#include <chaos/type_info.h>  // MethodTable, TypeInfoV0 (complete type)");
        sb.AppendLine("#include \"generated_code_compat.h\"  // PureTypeHeader for delegate type definitions");
        sb.AppendLine("#include \"ChaosGeneratedRuntimePrelude.h\"  // chaos_managed_array for array-compat checks");
        sb.AppendLine();

        // chaos_valuetype_* definitions — these are opaque 32-bit value types in the
        // managed ABI surface. Must come BEFORE boxed type struct definitions (below)
        // because boxed types contain "chaos_valuetype_X value{};" and require complete
        // value type structs.
        // 1. Types with fields or _backing: emit struct definitions (from
        //    _valueTypeStructCode) so sizeof and ABI are correct.
        // 2. Types without fields (pure enums): emit typedef CHAOS_IL2CPP_INT32
        //    for correct ABI (int32_t register passing, not empty-struct sizeof=1).
        // Structs are NOT re-emitted in page file object model, avoiding C2556/C2371.
        bool hasAnyForwardDeclarations = false;
        var vtCode = _valueTypeStructCode;
        if (vtCode is { Length: > 0 })
        {
            sb.Append(vtCode);
            hasAnyForwardDeclarations = true;
        }
        // Emit typedef for remaining value types (enum-like, no struct definition).
        var typedefNames = new HashSet<string>(StringComparer.Ordinal);
        // Phase 1: collect from _emittedValueTypeSubjectIds (ObjectModelEmission types)
        if (_emittedValueTypeSubjectIds is { Count: > 0 })
        {
            HashSet<string>? structSubjectIds = _valueTypeStructSubjectIds;
            foreach (var typeId in _emittedValueTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (structSubjectIds?.Contains(typeId) == true)
                    continue;
                typedefNames.Add(GetNativeValueTypeSymbol(typeId));
            }
        }
        // Phase 2: collect from extraValuetypes (method body scan)
        if (extraValuetypes is { Count: > 0 })
        {
            foreach (var name in extraValuetypes)
                typedefNames.Add(name);
        }
        // Phase 3: scan _methodDeclarations for chaos_valuetype_* references.
        // Method declarations (extern "C" function signatures) are rendered into page
        // files via Scriban templates.  They reference chaos_valuetype_* types from
        // external assemblies (e.g. System.Data.CommandBehavior) that are NOT in
        // _emittedValueTypeSubjectIds or extraValuetypes because they appear only as
        // ABI parameter types in extern declarations, not in method bodies or IR slots.
        // Exclude names that have struct definitions (tracked by _valueTypeStructSubjectIds).
        HashSet<string>? structSubjectIds3 = _valueTypeStructSubjectIds;
        var structVtNames = new HashSet<string>(StringComparer.Ordinal);
        if (structSubjectIds3 is { Count: > 0 })
        {
            foreach (var sid in structSubjectIds3)
                structVtNames.Add(GetNativeValueTypeSymbol(sid));
        }
        if (_methodDeclarations is { Count: > 0 })
        {
            foreach (var decl in _methodDeclarations)
            {
                int pos = 0;
                while ((pos = decl.IndexOf("chaos_valuetype_", pos, StringComparison.Ordinal)) >= 0)
                {
                    int start = pos;
                    int end = pos + 16;
                    while (end < decl.Length && (char.IsLetterOrDigit(decl[end]) || decl[end] == '_'))
                        end++;
                    var name = decl.Substring(start, end - start);
                    if (!structVtNames.Contains(name))
                        typedefNames.Add(name);
                    pos = end;
                }
            }
        }
        if (typedefNames.Count > 0)
        {
            // Exclude names that have struct definitions in vtCode.
            var skipNames = new HashSet<string>(StringComparer.Ordinal);
            if (vtCode is { Length: > 0 })
            {
                int pos = 0;
                while ((pos = vtCode.IndexOf("struct chaos_valuetype_", pos, StringComparison.Ordinal)) >= 0)
                {
                    int end = vtCode.IndexOfAny(new[] { ' ', '{' }, pos);
                    if (end < 0) break;
                    skipNames.Add(vtCode[pos..end]);
                    pos = end;
                }
            }
            foreach (var name in typedefNames.OrderBy(n => n, StringComparer.Ordinal))
            {
                if (skipNames.Contains(name)) continue;
                sb.Append("typedef CHAOS_IL2CPP_INT32 ");
                sb.Append(name);
                sb.AppendLine(";");
            }
            hasAnyForwardDeclarations = true;
        }
        if (hasAnyForwardDeclarations)
            sb.AppendLine();

        // ── Reference type struct definitions (complete) ──
        // Emitted in the shared header so that non-page-0 TUs have complete types for
        // CHAOS_IL2CPP_NEW_GC (sizeof), field access via reinterpret_cast, etc.
        // Page 0 skips these struct definitions to avoid C2011 redefinition.
        // Including these BEFORE the forward declaration loop below — C++ allows
        // forward declarations after definitions, so the redundant forward decls
        // in the loop are harmless.
        if (_referenceTypeStructCode is { Length: > 0 })
        {
            sb.Append(DeduplicateStructDefs(_referenceTypeStructCode));
        }

        // Track seen chaos_type_ symbols to prevent duplicate definitions from
        // FSharp.Core type forwarding in the inline loop below.
        _seenStructSymbols ??= CollectStructSymbols(_referenceTypeStructCode);

        // ── Struct forward declarations ──
        // Page files use reinterpret_cast<chaos_type_<id>*>(ptr),
        // reinterpret_cast<chaos_boxed_type_<id>*>(ptr), and
        // CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_<id>, ...).
        // For types in _boxedTypeSubjectIds we emit the complete struct definition
        // in the shared header so that CHAOS_IL2CPP_NEW_GC compiles in page files.
        // The object model section on page 0 skips these types to avoid C2011 redefinition.
        bool hasAnyStructDeclarations = false;
        foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
        {
            // Concrete delegate types (e.g. System.Action, System.Func<,>) need full
            // flat struct definitions in the shared header so that page files can access
            // delegate members (chaos_delegate_invocation_count, etc.) via reinterpret_cast.
            // The root Delegate/MulticastDelegate types keep forward declarations — their
            // full inherited definitions are only on page 0 (object model section).
            bool isConcreteDelegate = IsDelegateTypeSubjectId(typeId, _referenceTypeBaseSubjectIds)
                && !string.Equals(typeId, DelegateTypeSubjectId, StringComparison.Ordinal)
                && !string.Equals(typeId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal);

            if (isConcreteDelegate)
            {
                sb.Append("struct ");
                sb.Append(GetNativeTypeSymbol(typeId));
                sb.AppendLine(" {");
                sb.AppendLine("    PureTypeHeader header{};");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_UINT32 _pad = 0;");
                sb.AppendLine("};");
            }
            else
            {
                sb.Append("struct ");
                sb.Append(GetNativeTypeSymbol(typeId));
                sb.AppendLine(";");
            }

            // Boxed type: emit complete struct for types that appear as heap allocations
            // (CHAOS_IL2CPP_NEW_GC) in codegen output, so page files can compile.
            if (_boxedTypeSubjectIds?.Contains(typeId) == true)
            {
                var boxSym = GetNativeBoxTypeSymbol(typeId);
                if (_seenStructSymbols is null || _seenStructSymbols.Add(boxSym))
                {
                    sb.Append("struct ");
                    sb.Append(boxSym);
                    sb.AppendLine(" {");
                    sb.AppendLine("    PureTypeHeader header{};");
                    if (IsStructuredValueTypeSubjectId(typeId))
                    {
                        sb.Append("    ");
                        sb.Append(GetNativeValueTypeSymbol(typeId));
                        sb.AppendLine(" value{};");
                    }
                    else
                    {
                        sb.AppendLine("    CHAOS_IL2CPP_INTPTR value = 0;");
                    }
                    sb.AppendLine("};");
                }
            }
            else
            {
                sb.Append("struct ");
                sb.Append(GetNativeBoxTypeSymbol(typeId));
                sb.AppendLine(";");
            }

            hasAnyStructDeclarations = true;
        }
        if (hasAnyStructDeclarations)
            sb.AppendLine();

        // ── Hotpatch dispatch table (global scope) ──
        // Must be at GLOBAL scope because HotpatchEntryV0 is a C typedef defined in
        // codegen_bridge.h (global scope).  Page files include codegen_bridge.h before
        // the shared header, so ::HotpatchEntryV0 is a complete type when the header
        // is read.  If we forward-declared it inside the codegen namespace it would
        // create an unrelated incomplete type, and sizeof would fail.
        if (_nativeSymbolToDispatchSlot is { Count: > 0 })
        {
            sb.AppendLine("struct HotpatchEntryV0;");
            sb.AppendLine("extern \"C\" HotpatchEntryV0 s_hotpatch_entries[];");
            sb.AppendLine();
        }

        // ── Codegen namespace ──
        // MethodTable definitions, static field variables, type ID constants, and
        // runtime helper functions (chaos_string_materialize, chaos_is_array_store_compatible,
        // chaos_default_interpolated_string_handler_*, chaos_external_runtime_*,
        // ChaosReflectionSetExceptionMetadata_2params) are all defined inside
        // `namespace chaos::il2cpp::codegen::{codegenNamespace}` on page 0.
        // Their extern declarations MUST be in the same namespace to avoid LNK2001
        // unresolved external symbols (the linker treats global-scope and namespace-scoped
        // symbols as different entities).  HotpatchEntryV0 is excluded from this namespace
        // block because it is a C typedef from codegen_bridge.h.
        sb.Append("namespace chaos::il2cpp::codegen::");
        sb.AppendLine(codegenNamespace);
        sb.AppendLine("{");

        foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
        {
            var symbol = GetNativeMethodTableSymbol(typeId);
            sb.Append("extern MethodTable ");
            sb.Append(symbol);
            sb.AppendLine(";");
        }
        sb.AppendLine();

        // ── VTable extern declarations (inside codegen namespace) ──
        // Types with vtables defined in the object model (page 0) need extern
        // declarations so that type-info-defs.generated.cpp (paged build) can
        // reference them without C2065 undeclared identifier.
        // Use _vtableLengths cross-referenced with _allEmittedTypeSubjectIds
        // rather than _vtableTypes, since the latter may be null/empty in some
        // code paths while _vtableLengths is the authoritative source.
        if (_vtableLengths is { Count: > 0 } && _allEmittedTypeSubjectIds is { Count: > 0 })
        {
            foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (_vtableLengths.TryGetValue(typeId, out int vtLen) && vtLen > 0)
                {
                    var symbol = GetNativeVTableSymbol(typeId);
                    sb.Append("extern const void* ");
                    sb.Append(symbol);
                    sb.AppendLine("[];");
                }
            }
            sb.AppendLine();
        }

        // ── Static field extern declarations (inside codegen namespace) ──
        // Declared as TU-scoped variables in the object model (page 0) with actual types
        // determined by MapFieldTypeToCppType. Page files reference them by name and need
        // extern declarations with matching types to compile without C2371 redefinition.
        if (_staticFieldDeclarations is { Count: > 0 })
        {
            foreach (var kvp in _staticFieldDeclarations.OrderBy(k => k.Key, StringComparer.Ordinal))
            {
                var cppType = MapFieldTypeToCppType(kvp.Value);
                sb.Append("extern ");
                sb.Append(cppType);
                sb.Append(' ');
                sb.Append(GetNativeStaticFieldSymbol(kvp.Key));
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // ── Interface type ID constants (inline constexpr, inside namespace) ──
        // Interface map arrays in page files reference chaos_type_id_* constants.
        // Emit as `inline constexpr` in the shared header so all TUs get their own
        // compile-time constant with the same value.
        var allInterfaceTypeIds = new HashSet<string>(StringComparer.Ordinal);
        if (_interfaceTypeSubjectIds is { Count: > 0 })
        {
            allInterfaceTypeIds.UnionWith(_interfaceTypeSubjectIds);
        }
        if (_referenceTypeImplementedInterfaceSubjectIds is { Count: > 0 })
        {
            foreach (var ifaceSet in _referenceTypeImplementedInterfaceSubjectIds.Values)
            {
                if (ifaceSet is { Count: > 0 })
                    allInterfaceTypeIds.UnionWith(ifaceSet);
            }
        }
        if (allInterfaceTypeIds.Count > 0)
        {
            foreach (string ifaceId in allInterfaceTypeIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                sb.Append("extern const CHAOS_IL2CPP_UINT64 ");
                sb.Append(GetNativeTypeIdSymbol(ifaceId));
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // Non-interface type ID extern declarations
        // These are `inline constexpr` in page 0's object model but need
        // extern declarations in the shared header so page files can reference
        // them without triggering "unused inline variable" or ODR issues.
        if (_allEmittedTypeSubjectIds is { Count: > 0 })
        {
            foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (allInterfaceTypeIds.Count > 0 && allInterfaceTypeIds.Contains(typeId))
                    continue; // already declared as inline constexpr above
                sb.Append("extern const CHAOS_IL2CPP_UINT64 ");
                sb.Append(GetNativeTypeIdSymbol(typeId));
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // ── Type initialization function extern declarations ──
        // chaos_ensure_type_initialized_* functions are defined on page 0 but
        // called from page files for types with non-empty static constructors.
        // Emit extern declarations so page files compile without C3861.
        foreach (var typeId in _allEmittedTypeSubjectIds ?? [])
        {
            sb.Append("extern \"C\" void ");
            sb.Append(GetNativeTypeInitializationFunctionSymbol(typeId));
            sb.AppendLine("(void);");
        }
        if (_allEmittedTypeSubjectIds is { Count: > 0 })
            sb.AppendLine();

        // ── Span runtime helper declarations (inside codegen namespace) ──
        // chaos_initialize_array_from_field_data_int32 and
        // chaos_create_field_data_span_int32 are defined in the object model
        // (SpanRuntimePrelude at BuildObjectModel line 864) when
        // RuntimeHelpers.InitializeArray / CreateSpan are reachable.
        // Page files that call these functions need extern declarations
        // to compile without C3861.
        if (_externalRuntimeHelpers?.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)) == true)
        {
            sb.AppendLine("extern void chaos_initialize_array_from_field_data_int32(CHAOS_IL2CPP_INTPTR chaos_array_value, CHAOS_IL2CPP_INTPTR chaos_field_handle);");
            sb.AppendLine("extern CHAOS_IL2CPP_INTPTR chaos_create_field_data_span_int32(CHAOS_IL2CPP_INTPTR chaos_field_handle);");
            sb.AppendLine();
        }

        // ── Runtime helper declarations (inside codegen namespace) ──
        // These are DEFINED inside the codegen namespace on page 0 (in the object
        // model section), so their extern declarations MUST also be inside the
        // namespace to avoid LNK2019 from namespace-scoped vs global-scope mismatch.
        // Unused declarations are harmless — the linker only resolves referenced symbols.

        // chaos_string_materialize: declaration only in the shared header.
        // The definition (inline identity or real) is emitted in the main file
        // to avoid ODR conflicts when _stringIdMapping changes between phases.
        sb.AppendLine("CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept;");
        sb.AppendLine();

        // chaos_is_array_store_compatible: always emitted in object model
        sb.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept;");
        sb.AppendLine();

        // ── DefaultInterpolatedStringHandler declarations (inside codegen namespace) ──
        // These are defined inside the codegen namespace on page 0 (via
        // EmitDelegateRuntimeSupportDefinitions).  Page files call them via unqualified
        // lookup within the same namespace, so extern declarations must also be inside
        // the namespace to avoid LNK2019 (namespace-scoped vs global-scope mismatch).
        // Unused declarations are harmless — the linker only resolves referenced symbols.
        sb.AppendLine("void chaos_default_interpolated_string_handler_reset(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_literal_length, CHAOS_IL2CPP_INT32 chaos_trailing_count);");
        sb.AppendLine("void chaos_default_interpolated_string_handler_append_string(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INTPTR chaos_string_value);");
        sb.AppendLine("void chaos_default_interpolated_string_handler_append_int32(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_value);");
        sb.AppendLine("CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_to_string_and_clear(CHAOS_IL2CPP_INTPTR chaos_handler_ref);");
        sb.AppendLine();

        sb.AppendLine("} // namespace chaos::il2cpp::codegen::" + codegenNamespace);
        sb.AppendLine();

        // ── External runtime dispatch table (global scope) ──
        // Bridge thunks in page files call through kChaosExternalRuntimeFnTable[idx].
        // The array is defined in the module registration section.
        // This is `extern "C"` and must be at global scope.
        if ((_bridgeImportThunks is { Count: > 0 } &&
             _bridgeImportThunks.Values.Any(t => t.ExternalRuntimeTableIndex >= 0)) ||
            _externalRuntimeSubjects is { Count: > 0 })
        {
            sb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[];");
            sb.AppendLine();
        }
        // Always declare kChaosExternalRuntimeCount for page files that use
        // BS-5 external runtime table bounds checks (defined in main file).
        sb.AppendLine("extern \"C\" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount;");
        // ── Crypto AOT IR data for interpreter fallback ──
        if (_cryptoAotIrEntries.Count > 0)
        {
            sb.AppendLine("extern \"C\" const int kCryptoAotIrCount;");
            sb.AppendLine("extern \"C\" const char* kCryptoAotIrSubjects[];");
            sb.AppendLine("extern \"C\" const char* kCryptoAotIrJson[];");
        }

        // ── Runtime helper function declarations (global scope) ──
        // These functions are DEFINED in the native runtime library (not inside the
        // codegen namespace), so their extern declarations MUST be at global scope
        // to avoid LNK2001 unresolved external symbols.

        // ── chaos_external_runtime_* declarations (inside codegen namespace) ──
        // These helpers are DEFINED on page 0 inside the codegen namespace, so their
        // extern declarations MUST also be inside the namespace to match.
        // Deduplication by TargetSymbol: SanitizeSubjectId used by GetExternalRuntimeHelperSymbol
        // is non-injective — different SubjectIds can map to the same sanitized symbol
        // (e.g. generic instantiations).  Emitting two extern "C" declarations with
        // the same name but different signatures causes C2733.
        var seenTargetSymbols = new HashSet<string>(StringComparer.Ordinal);
        if (_externalRuntimeHelpers is { Count: > 0 })
        {
            sb.Append("namespace chaos::il2cpp::codegen::");
            sb.AppendLine(codegenNamespace);
            sb.AppendLine("{");
            foreach (var helper in _externalRuntimeHelpers)
            {
                // Deduplicate by TargetSymbol — two different SubjectIds may
                // sanitize to the same symbol name, and extern "C" doesn't
                // allow overloading by parameter types (C2733).
                if (!string.IsNullOrEmpty(helper.TargetSymbol) &&
                    !seenTargetSymbols.Add(helper.TargetSymbol))
                    continue;

                // Extract the first line of the source (the function signature)
                // and convert it to a declaration by appending ";".
                var source = helper.Source;
                if (string.IsNullOrEmpty(source))
                    continue;
                int newlineIdx = source.IndexOf('\n');
                string signatureLine = newlineIdx >= 0
                    ? source.Substring(0, newlineIdx).Trim()
                    : source.Trim();
                if (string.IsNullOrEmpty(signatureLine))
                    continue;
                // Remove `static ` prefix if present (should be gone after template fix,
                // but handle gracefully for any remaining static helpers).
                // Also skip `extern "C" ` prefix if present — the declaration already
                // has C linkage and prepending another `extern` would cause C2159
                // ("more than one storage class specified") on MSVC.
                if (signatureLine.StartsWith("static ", StringComparison.Ordinal))
                    signatureLine = signatureLine.Substring(7);
                if (signatureLine.StartsWith("extern \"C\" ", StringComparison.Ordinal))
                {
                    // Already has extern "C" linkage — emit as-is (don't double extern)
                    sb.Append(signatureLine);
                }
                else
                {
                    sb.Append("extern ");
                    sb.Append(signatureLine);
                }
                sb.AppendLine(";");
            }
            sb.AppendLine();
            sb.AppendLine("} // namespace chaos::il2cpp::codegen::" + codegenNamespace);
            sb.AppendLine();
        }

        // ── chaos_external_runtime_* re-declarations (file scope) ──
        // Call sites in page*.cpp are at file scope.  The namespace-scoped extern
        // declarations above are not visible outside the namespace block, so we
        // re-declare every symbol at file scope as well.
        // Using `extern "C++"` is implicit (default for extern in .cpp files at
        // file scope in C++).
        //
        // Only emit for helpers with non-empty Source (they have real signatures).
        // DirectNativeSymbol-only helpers (empty Source) get a separate minimal
        // extern "C" fallback below.
        // Dedup by TargetSymbol (same seenTargetSymbols from Section 1).
        if (_externalRuntimeHelpers is { Count: > 0 })
        {
            foreach (var helper in _externalRuntimeHelpers)
            {
                // Deduplicate by TargetSymbol (same set from Section 1 above).
                if (!string.IsNullOrEmpty(helper.TargetSymbol) &&
                    !seenTargetSymbols.Contains(helper.TargetSymbol))
                    continue;

                var source = helper.Source;
                if (string.IsNullOrEmpty(source))
                    continue;
                int newlineIdx = source.IndexOf('\n');
                string signatureLine = newlineIdx >= 0
                    ? source.Substring(0, newlineIdx).Trim()
                    : source.Trim();
                if (string.IsNullOrEmpty(signatureLine))
                    continue;
                if (signatureLine.StartsWith("static ", StringComparison.Ordinal))
                    signatureLine = signatureLine.Substring(7);
                if (signatureLine.StartsWith("extern \"C\" ", StringComparison.Ordinal))
                {
                    // Already has extern "C" linkage — emit as-is
                    sb.Append(signatureLine);
                }
                else
                {
                    sb.Append("extern ");
                    sb.Append(signatureLine);
                }
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // ── chaos_external_runtime_* fallback declarations (global scope) ──
        // For _externalRuntimeSubjects entries that lack a namespace-scoped extern
        // declaration from ExternalRuntimeHelpers (e.g., DirectNativeSymbol-only
        // entries with empty Source), emit minimal extern "C" declarations at global
        // scope so the dispatch table's &chaos_external_runtime_* references compile.
        //
        // We build a set of SubjectIds that already have namespace-scoped declarations
        // from _externalRuntimeHelpers (non-empty Source => real signature emitted).
        // Entries with helpers that have empty source (DirectNativeSymbol-only) also
        // need a fallback declaration.
        if (_externalRuntimeSubjects is { Count: > 0 })
        {
            // Build set of SubjectIds that already have file-scope extern re-declarations
            var helpersWithSource = _externalRuntimeHelpers?
                .Where(h => !string.IsNullOrEmpty(h.Source))
                .Select(h => h.SubjectId)
                .ToHashSet(StringComparer.Ordinal) ?? new HashSet<string>();
            // Pre-compute methods that have AOT extern "C" declarations
            // (from helpers with source or static inline stubs).
            var aotDeclaredSymbols = new HashSet<string>(StringComparer.Ordinal);
            if (_externalRuntimeHelpers is { Count: > 0 })
            {
                foreach (var h in _externalRuntimeHelpers)
                    if (!string.IsNullOrEmpty(h.TargetSymbol))
                        aotDeclaredSymbols.Add(h.TargetSymbol);
            }
            var fallbackSb = new System.Text.StringBuilder(1024);
            int fallbackCount = 0;
            foreach (var kvp in _externalRuntimeSubjects)
            {
                if (helpersWithSource.Contains(kvp.Key))
                    continue;
                var symbol = GetExternalRuntimeHelperSymbol(kvp.Key);
                // Skip if the symbol already has a declaration from any earlier section
                // (helper template declarations or previously-processed subjects).
                if (aotDeclaredSymbols.Contains(symbol) || !seenTargetSymbols.Add(symbol))
                    continue;
                // Look up the correct parameter count from _emittedExternalRuntimeSymbolParams
                // (populated during method body emission from InvocationTarget.ParameterAbis).
                // Without this, SkipInit(ref int) → 1 ABI param gets declared as 0-param,
                // causing C2660 when the call site passes chaos_arg_0.
                // Default to 1 param (CHAOS_IL2CPP_INTPTR) because ALL external runtime
                // stubs are called with at least chaos_arg_0 (the this pointer or sole arg).
                // Using 0-param () would conflict with AddExternalRuntimeStubs which correctly
                // counts call-site args and emits (CHAOS_IL2CPP_INTPTR), causing C2733
                // (extern "C" overloading disallowed) when both declarations are visible.
                int extParamCount = _emittedExternalRuntimeSymbolParams.TryGetValue(symbol, out var epc) ? epc : 1;
                // Generate extern "C" CHAOS_IL2CPP_INTPTR (same format as
                // BuildAbiExportDeclarations) to avoid conflicting return type
                // declarations at global scope.
                fallbackSb.Append("extern \"C\" CHAOS_IL2CPP_INTPTR ");
                fallbackSb.Append(symbol);
                fallbackSb.Append('(');
                for (int __pi = 0; __pi < extParamCount; __pi++)
                {
                    if (__pi > 0) fallbackSb.Append(", ");
                    fallbackSb.Append("CHAOS_IL2CPP_INTPTR");
                }
                fallbackSb.AppendLine(") noexcept;");
                fallbackCount++;
            }
            if (fallbackCount > 0)
            {
                sb.Append(fallbackSb.ToString());
                sb.AppendLine();
            }
        }

        // ── ChaosReflectionSetExceptionMetadata_2params (global scope) ──
        // Called from ArgumentOutOfRangeException..ctor(string,string) in page
        // files.  Declared in exception_api.cpp in the runtime — at global scope.
        sb.AppendLine("void ChaosReflectionSetExceptionMetadata_2params(CHAOS_IL2CPP_INTPTR chaos_exception, CHAOS_IL2CPP_INTPTR chaos_message, CHAOS_IL2CPP_INTPTR chaos_param_name);");
        sb.AppendLine();

        // ── Post-scan: emit extern "C" declarations for ALL chaos_external_runtime_*
        // symbols referenced in method instructions but not in _externalRuntimeSubjects.
        // These are methods that the codegen resolves via TargetSymbol but never adds
        // to the dispatch table (e.g. System.Attribute..ctor called from Brotli tests
        // where the codegen generates a direct symbol reference without a dispatch entry).
        var externalRuntimeSymbolsReferenced = new HashSet<string>(StringComparer.Ordinal);
        if (_methodsBySubjectId is { Count: > 0 })
        {
            var declaredExtSymbols = new HashSet<string>(StringComparer.Ordinal);
            if (_externalRuntimeSubjects is { Count: > 0 })
            {
                foreach (var kvp in _externalRuntimeSubjects)
                    declaredExtSymbols.Add(GetExternalRuntimeHelperSymbol(kvp.Key));
            }
            if (_externalRuntimeHelpers is { Count: > 0 })
            {
                foreach (var h in _externalRuntimeHelpers)
                    declaredExtSymbols.Add(h.TargetSymbol);
            }
            // Also consider symbols deduped by seenTargetSymbols to avoid
            // emitting static inline stubs for symbols already declared as extern "C".
            foreach (var s in seenTargetSymbols)
                declaredExtSymbols.Add(s);

            foreach (var m in _methodsBySubjectId.Values)
            {
                if (m.Instructions == null) continue;
                foreach (var instr in m.Instructions)
                {
                    // Check TargetSymbol directly (set by codegen for resolved calls)
                    if (!string.IsNullOrEmpty(instr.TargetSymbol) &&
                        instr.TargetSymbol.StartsWith("chaos_external_runtime_", StringComparison.Ordinal) &&
                        !declaredExtSymbols.Contains(instr.TargetSymbol))
                    {
                        externalRuntimeSymbolsReferenced.Add(instr.TargetSymbol);
                    }
                    // Check Callee for methods that fall through to catch-all dispatch
                    // (TargetSymbol may be null, but the emission code will generate
                    // a chaos_external_runtime_* symbol from the Callee SubjectId).
                    else if (!string.IsNullOrEmpty(instr.Callee) &&
                             instr.Callee.Contains('/') &&
                             !instr.Callee.StartsWith("CombinedSubjects/", StringComparison.Ordinal))
                    {
                        var sym = GetExternalRuntimeHelperSymbol(instr.Callee);
                        if (!declaredExtSymbols.Contains(sym))
                            externalRuntimeSymbolsReferenced.Add(sym);
                    }
                }
            }
        }
        if (externalRuntimeSymbolsReferenced.Count > 0)
        {
            sb.AppendLine("// ── External runtime function stubs (post-scan) ──");
            // Skip symbols that will get typed stubs from _emittedExternalRuntimeSymbols
            // to avoid C2556/C2371: extern "C" cannot overload by return type.
            var typedStubSymbols = _emittedExternalRuntimeSymbols is { Count: > 0 }
                ? new HashSet<string>(_emittedExternalRuntimeSymbols.Keys, StringComparer.Ordinal)
                : new HashSet<string>(StringComparer.Ordinal);
            foreach (var sym in externalRuntimeSymbolsReferenced.OrderBy(s => s))
            {
                if (typedStubSymbols.Contains(sym)) continue;
                sb.Append("static inline CHAOS_IL2CPP_INTPTR ");
                sb.Append(sym);
                sb.AppendLine("() noexcept { return 0; }");
            }
            sb.AppendLine();
        }

        // ── Emit stub declarations for all chaos_external_runtime_* symbols
        // collected during method body emission (EmitInvocation path) that were
        // not already declared by the post-scan above.  These are symbols whose
        // DirectNativeSymbol was set after IR instruction processing, so the
        // post-scan (which reads IR instructions) could not detect them.
        //
        // Skip symbols that already have extern declarations from helpers or
        // fallback paths to avoid C2732 linkage contradiction (extern vs static).
        if (_emittedExternalRuntimeSymbols is { Count: > 0 })
        {
            // Build set of symbols that already have extern declarations.
            var alreadyDeclaredExtSyms = new HashSet<string>(StringComparer.Ordinal);
            if (_externalRuntimeHelpers is { Count: > 0 })
            {
                foreach (var h in _externalRuntimeHelpers)
                    if (!string.IsNullOrEmpty(h.TargetSymbol))
                        alreadyDeclaredExtSyms.Add(h.TargetSymbol);
            }
            if (_externalRuntimeSubjects is { Count: > 0 })
            {
                var helpersWithSource = _externalRuntimeHelpers?
                    .Where(h => !string.IsNullOrEmpty(h.Source))
                    .Select(h => h.SubjectId)
                    .ToHashSet(StringComparer.Ordinal) ?? new HashSet<string>();
                foreach (var kvp in _externalRuntimeSubjects)
                {
                    if (helpersWithSource.Contains(kvp.Key)) continue;
                    var sym = GetExternalRuntimeHelperSymbol(kvp.Key);
                    alreadyDeclaredExtSyms.Add(sym);
                }
            }
            foreach (var kvp in _emittedExternalRuntimeSymbols.OrderBy(kv => kv.Key))
            {
                if (alreadyDeclaredExtSyms.Contains(kvp.Key)) continue;
                // Use correct C++ return type matching ABI carrier: Float32->float (XMM0),
                // Float64->double (XMM0), others->CHAOS_IL2CPP_INTPTR (RAX).
                string cppType = kvp.Value switch
                {
                    AotCoreIrAbiCarrierKind.Void => "void",
                    AotCoreIrAbiCarrierKind.Float32 => "float",
                    AotCoreIrAbiCarrierKind.Float64 => "double",
                    _ => "CHAOS_IL2CPP_INTPTR",
                };
                // Look up the correct parameter count from _emittedExternalRuntimeSymbolParams
                // (set at same time as _emittedExternalRuntimeSymbols from InvocationTarget).
                // Without this, the stub declares () noexcept but the caller passes arguments,
                // causing C2660 (function does not take N arguments).
                int paramCount = _emittedExternalRuntimeSymbolParams.TryGetValue(kvp.Key, out var pc) ? pc : 0;
                sb.Append("static inline ");
                sb.Append(cppType);
                sb.Append(' ');
                sb.Append(kvp.Key);
                sb.Append('(');
                for (int __pi = 0; __pi < paramCount; __pi++)
                {
                    if (__pi > 0) sb.Append(", ");
                    sb.Append("CHAOS_IL2CPP_INTPTR chaos_arg_");
                    sb.Append(__pi);
                }
                sb.Append(") noexcept");
                if (kvp.Value == AotCoreIrAbiCarrierKind.Void)
                    sb.AppendLine(" {}");
                else
                    sb.AppendLine(" { return 0; }");
            }
            sb.AppendLine();
        }

        // ── Safety net: append missing chaos_valuetype_* typedefs ──
        // Must run BEFORE BuildAbiExportDeclarations (which may return a modified
        // string and skip the rest).  Scan sb for chaos_valuetype_* references
        // lacking a corresponding typedef and append them.
        AppendMissingValueTypeTypedefsForHeader(sb);

        // ── Comprehensive extern declarations for ALL emitted external-runtime
        // symbols ──
        // Every chaos_external_runtime_* symbol the codegen emitted a call to during
        // method body emission (page TUs included) must be declared in the shared
        // header.  _emittedExternalRuntimeSymbols is the authoritative set (keyed by
        // symbol, value = return ABI carrier).  The BuildAbiExportDeclarations
        // post-scan at the end of this method only sees symbols *textually* present
        // in the header body, which misses calls that appear only in separate page
        // translation units (e.g. surfaced reflection/delegate helpers such as
        // SubjectInstanceFactory__Create_System_Type__System_Type__).  Emitting the
        // full set here (as extern "C" with correct arg counts) avoids C3861 in the
        // pages while staying ODR-safe (extern declarations, no definitions).  Symbol
        // already handled by _externalRuntimeSubjects/_externalRuntimeHelpers are
        // skipped to avoid C2733 / C2371 conflicts.
        if (_emittedExternalRuntimeSymbols is { Count: > 0 })
        {
            // Symbols already declared above via their SubjectId registrations.
            var declaredViaSubjects = new HashSet<string>(StringComparer.Ordinal);
            if (_externalRuntimeSubjects is { Count: > 0 })
                foreach (var kvp in _externalRuntimeSubjects)
                    declaredViaSubjects.Add(GetExternalRuntimeHelperSymbol(kvp.Key));
            if (_externalRuntimeHelpers is { Count: > 0 })
                foreach (var h in _externalRuntimeHelpers)
                    if (!string.IsNullOrEmpty(h.TargetSymbol))
                        declaredViaSubjects.Add(h.TargetSymbol);

            var extHeaderSb = new System.Text.StringBuilder(2048);
            int extDeclCount = 0;
            foreach (var kvp in _emittedExternalRuntimeSymbols.OrderBy(kv => kv.Key, StringComparer.Ordinal))
            {
                if (declaredViaSubjects.Contains(kvp.Key)) continue;
                if (!seenTargetSymbols.Add(kvp.Key)) continue;   // dedup / skip link-contradiction
                int pc = _emittedExternalRuntimeSymbolParams.TryGetValue(kvp.Key, out var epc) ? epc : 1;
                extHeaderSb.Append("extern \"C\" CHAOS_IL2CPP_INTPTR ");
                extHeaderSb.Append(kvp.Key);
                extHeaderSb.Append('(');
                for (int __pi = 0; __pi < pc; __pi++)
                {
                    if (__pi > 0) extHeaderSb.Append(", ");
                    extHeaderSb.Append("CHAOS_IL2CPP_INTPTR");
                }
                extHeaderSb.AppendLine(") noexcept;");
                extDeclCount++;
            }
            if (extDeclCount > 0)
            {
                sb.Append("// ── Emitted external-runtime symbol declarations (comprehensive) ──\n");
                sb.Append(extHeaderSb.ToString());
                sb.AppendLine();
            }
        }

        // Phase 1a: ChaosAbiExportCollector — ensure every chaos_external_runtime_*
        // symbol referenced in the generated header has a visible extern "C" declaration.
        string postCollector = BuildAbiExportDeclarations(sb);
        if (postCollector.Length > 0)
        {
            return postCollector;
        }

        return sb.ToString();
    }

    /// <summary>Remove duplicate struct/boxed-type definitions from C++ code.
    /// FSharp.Core type forwarding may produce the same chaos_type_* symbol
    /// from different SubjectIds. Keeps the first definition, removes subsequent
    /// ones along with their body lines.</summary>
    private static string DeduplicateStructDefs(string code)
    {
        var seen = new HashSet<string>(StringComparer.Ordinal);
        var result = new System.Text.StringBuilder(code.Length);
        int brace = 0; bool skip = false;

        foreach (var line in code.Split('\n'))
        {
            var t = line.Trim();

            if (brace == 0 && (t.StartsWith("struct chaos_type_", StringComparison.Ordinal) ||
                               t.StartsWith("struct chaos_boxed_type_", StringComparison.Ordinal)))
            {
                var prefixLen = t.StartsWith("struct chaos_boxed_type_")
                    ? "struct chaos_boxed_type_".Length : "struct chaos_type_".Length;
                var end = t.IndexOfAny(new[] { ' ', ':' }, prefixLen);
                var sym = end >= 0 ? t[prefixLen..end] : t[prefixLen..];
                if (!seen.Add(sym))
                {
                    skip = true;
                    brace = t.Contains('{') ? 1 : 0;
                    continue;
                }
            }

            if (skip)
            {
                foreach (char c in line) { if (c == '{') brace++; if (c == '}') brace--; }
                if (brace > 0) continue;
                skip = false; brace = 0;
                continue;
            }

            result.AppendLine(line);
        }
        return result.ToString();
    }



    /// <summary>Collect all chaos_type_ / chaos_boxed_type_ symbols from a struct definition string.
    /// Used to seed _seenStructSymbols so the inline loop doesn't re-emit struct definitions
    /// that were already emitted via _referenceTypeStructCode.</summary>
    private static HashSet<string> CollectStructSymbols(string? code)
    {
        var symbols = new HashSet<string>(StringComparer.Ordinal);
        if (string.IsNullOrEmpty(code)) return symbols;
        foreach (var line in code.Split('\n'))
        {
            var t = line.Trim();
            if (t.StartsWith("struct chaos_type_", StringComparison.Ordinal) ||
                t.StartsWith("struct chaos_boxed_type_", StringComparison.Ordinal))
            {
                var prefixLen = t.StartsWith("struct chaos_boxed_type_")
                    ? "struct chaos_boxed_type_".Length : "struct chaos_type_".Length;
                var end = t.IndexOfAny(new[] { ' ', ':' }, prefixLen);
                var sym = end >= 0 ? t[prefixLen..end] : t[prefixLen..];
                symbols.Add(sym);
            }
        }
        return symbols;
    }



    /// <summary>
    /// Safety net: scan the generated header StringBuilder for any chaos_valuetype_*
    /// references that lack a corresponding typedef and append the missing ones.
    /// This catches value types referenced in TPG stub method declarations that are
    /// not part of the AOT subject methods' type closure (e.g. System.Data.Common
    /// internal enums like DataRowVersion, ConflictOption, CommandBehavior).
    /// </summary>
    private static void AppendMissingValueTypeTypedefsForHeader(StringBuilder sb)
    {
        var content = sb.ToString();

        // Collect existing typedefs and struct definitions.
        var existing = new HashSet<string>(StringComparer.Ordinal);
        const string typedefPrefix = "typedef CHAOS_IL2CPP_INT32 chaos_valuetype_";
        const string structPrefix = "struct chaos_valuetype_";
        int pos = 0;
        while ((pos = content.IndexOf(typedefPrefix, pos, StringComparison.Ordinal)) >= 0)
        {
            int end = content.IndexOf(';', pos);
            if (end < 0) break;
            var name = content.Substring(pos, end - pos).TrimEnd();
            // Strip "typedef CHAOS_IL2CPP_INT32 " prefix to get bare symbol name
            existing.Add(name[typedefPrefix.Length..]);
            pos = end + 1;
        }
        pos = 0;
        while ((pos = content.IndexOf(structPrefix, pos, StringComparison.Ordinal)) >= 0)
        {
            // The struct symbol name follows the full structPrefix ("struct chaos_valuetype_").
            // Search for the first terminator AFTER the prefix; the prefix itself contains a
            // space, so we must not use IndexOfAny from pos (it would match inside the keyword).
            int nameStart = pos + structPrefix.Length;
            int end = content.IndexOfAny(new[] { ' ', '{', ';' }, nameStart);
            if (end < 0) end = content.Length;
            if (end <= nameStart) { pos = end + 1; continue; }
            existing.Add(content.Substring(nameStart, end - nameStart));
            pos = end + 1;
        }

        // Scan for referenced chaos_valuetype_* symbols.
        var needed = new HashSet<string>(StringComparer.Ordinal);
        const string refPrefix = "chaos_valuetype_";
        pos = 0;
        while ((pos = content.IndexOf(refPrefix, pos, StringComparison.Ordinal)) >= 0)
        {
            int start = pos;
            int end = start + refPrefix.Length;
            while (end < content.Length && (char.IsLetterOrDigit(content[end]) || content[end] == '_'))
                end++;
            var symbol = content[start..end];
            if (!existing.Contains(symbol))
                needed.Add(symbol);
            pos = end;
        }

        if (needed.Count == 0)
            return;

        sb.AppendLine();
        sb.AppendLine("// chaos_valuetype_* typedefs (safety net: TPG stub declarations)");
        foreach (var name in needed.OrderBy(n => n, StringComparer.Ordinal))
        {
            sb.Append("typedef CHAOS_IL2CPP_INT32 ");
            sb.Append(name);
            sb.AppendLine(";");
        }
        sb.AppendLine();
    }

    private string BuildCryptoAotIrCode()
    {
        if (_cryptoAotIrEntries.Count == 0) return string.Empty;
        var sb = new StringBuilder(4096);
        sb.Append("extern \"C\" const int kCryptoAotIrCount = ");
        sb.Append(_cryptoAotIrEntries.Count);
        sb.AppendLine(";");
        sb.AppendLine("extern \"C\" const char* kCryptoAotIrSubjects[] = {");
        foreach (var e in _cryptoAotIrEntries)
            sb.AppendLine("    \"" + e.SubjectId.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\",");
        sb.AppendLine("};");
        sb.AppendLine("extern \"C\" const char* kCryptoAotIrJson[] = {");
        foreach (var e in _cryptoAotIrEntries)
        {
            var esc = e.Json.Replace("\\", "\\\\").Replace("\"", "\\\"");
            sb.AppendLine("    \"" + esc + "\",");
        }
        sb.AppendLine("};");
        return sb.ToString();
    }



    private static string BuildEntryBridgeArguments(AotCoreIrMethodArtifact entryMethod)
    {
        ArgumentNullException.ThrowIfNull(entryMethod);

        if (entryMethod.ParameterCount == 0)
        {
            return "";
        }

        if (entryMethod.ParameterCount != 1 || entryMethod.ParameterAbis.Count != 1)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' has {entryMethod.ParameterCount} parameters.");
        }

        var carrierKind = entryMethod.ParameterAbis[0].CarrierKindCode;
        if (carrierKind != AotCoreIrAbiCarrierKind.Int32)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only System.Int32 entry parameters, but '{entryMethod.SubjectId}' uses carrier '{carrierKind}'.");
        }

        return "chaos_entry_index";
    }

}

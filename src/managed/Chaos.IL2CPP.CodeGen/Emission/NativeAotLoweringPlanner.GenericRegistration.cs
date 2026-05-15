using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

/// <summary>
/// Emits the GenericTypeRegistrationEntryV0[] and GenericMethodRegistrationEntryV0[]
/// constant arrays that the AOT MetadataRegistrationV0 references. Bootstrap iterates
/// these arrays to populate the GenericContextRegistry.
///
/// Generated output pattern (in ObjectModelCode):
/// <code>
/// static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[] = { 0x01000008, 0x0100000E };
/// static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[] = {
///     { 0x02000010, 0x02000050, 1, 0 },
/// };
/// static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[] = { 0x01000008 };
/// static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[] = {
///     { 0x06000100, 0, 1, 0 },
/// };
/// </code>
/// </summary>
public partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Emit the constexpr arrays into the object-model StringBuilder,
    /// and produce a C-linkage helper function that exposes the arrays
    /// for the proof host to populate MetadataRegistrationV0.
    /// Also emits per-module GenericMethodAotEntryV0[] and returns the
    /// corresponding registration-call snippet for the module registration.
    /// Called from <see cref="Create"/> after EmitRuntimePrelude etc.
    /// </summary>
    private void EmitGenericRegistration(
        StringBuilder builder,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
        MetadataRegistrationArtifact metadataRegistration,
        out string helperCode,
        out string aotRegistrationCode)
    {
        // Phase 1: collect generic types and methods with their token info.
        BuildGenericTypeRegistration(
            supplementalMetadataTemplate,
            metadataRegistration,
            out var typeEntries,
            out var typeArgTokens);

        BuildGenericMethodRegistration(
            supplementalMetadataTemplate,
            metadataRegistration,
            out var methodEntries,
            out var methodArgTokens);

        // Phase 2: collector generic method AOT entries (pre-compiled).
        BuildMethodAotRegistration(
            supplementalMetadataTemplate,
            metadataRegistration,
            out var methodAotEntries,
            out var methodAotArgTokens);

        // ── Format and render via Scriban template ──
        var typeEntryStrings = typeEntries
            .Select(e => $"{{ 0x{e.OpenToken:X8}u, 0x{e.ClosedToken:X8}u, {e.ArgCount}u, {e.ArgsStartIndex}u }}")
            .ToArray();
        var methodEntryStrings = methodEntries
            .Select(e => $"{{ 0x{e.MethodToken:X8}u, {e.ClassArgCount}u, {e.MethodArgCount}u, {e.ArgsStartIndex}u }}")
            .ToArray();
        var methodAotEntryStrings = methodAotEntries
            .Select(e => $"{{ 0x{e.OpenToken:X8}u, 0x{e.ClosedToken:X8}u, {e.ArgCount}u, {e.ArgsStartIndex}u }}")
            .ToArray();

        var model = new ScriptObject
        {
            ["indentation"] = "",
            ["has_type_arg_tokens"] = typeArgTokens.Count > 0,
            ["type_arg_tokens_text"] = string.Join(", ", typeArgTokens),
            ["has_type_entries"] = typeEntries.Count > 0,
            ["type_entries"] = typeEntryStrings,
            ["has_method_arg_tokens"] = methodArgTokens.Count > 0,
            ["method_arg_tokens_text"] = string.Join(", ", methodArgTokens),
            ["has_method_entries"] = methodEntries.Count > 0,
            ["method_entries"] = methodEntryStrings,
            ["has_method_aot_entries"] = methodAotEntries.Count > 0,
            ["method_aot_entries"] = methodAotEntryStrings,
            ["method_aot_arg_tokens_text"] = string.Join(", ", methodAotArgTokens),
        };

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetGenericRegistrationTemplate(), model).TrimEnd());
        builder.AppendLine();

        // ── AOT registration code (emitted into module registration section) ──
        int methodAotEntryCount = methodAotEntries.Count;
        int methodAotArgTokenCount = methodAotArgTokens.Count;
        var aotModel = new ScriptObject
        {
            ["has_aot_entries"] = methodAotEntryCount > 0,
            ["aot_entry_count"] = methodAotEntryCount,
            ["aot_arg_token_count"] = methodAotArgTokenCount,
        };
        aotRegistrationCode = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGenericAotRegistrationTemplate(), aotModel).TrimEnd();

        // ── Generic registration helper for proof host ──
        var typeEntryCount = typeEntries.Count;
        var typeArgCount = typeArgTokens.Count;
        var methodEntryCount = methodEntries.Count;
        var methodArgCount = methodArgTokens.Count;

        var helperModel = new ScriptObject
        {
            ["type_entry_count"] = typeEntryCount,
            ["type_arg_count"] = typeArgCount,
            ["method_entry_count"] = methodEntryCount,
            ["method_arg_count"] = methodArgCount,
        };
        helperCode = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGenericRegistrationHelperTemplate(), helperModel).TrimEnd();
    }

    private sealed record TypeEntry(uint OpenToken, uint ClosedToken, uint ArgCount, uint ArgsStartIndex);
    private sealed record MethodEntry(uint MethodToken, uint ClassArgCount, uint MethodArgCount, uint ArgsStartIndex);
    private sealed record MethodAotEntry(uint OpenToken, uint ClosedToken, uint ArgCount, uint ArgsStartIndex);

    private void BuildGenericTypeRegistration(
        SupplementalMetadataTemplateArtifact supplemental,
        MetadataRegistrationArtifact metadataRegistration,
        out List<TypeEntry> entries,
        out List<string> argTokens)
    {
        entries = new List<TypeEntry>();
        argTokens = new List<string>();

        var lookup = new MetadataTokenLookup(metadataRegistration.Registrations);
        uint argPoolIndex = 0;

        foreach (var typeEntry in supplemental.RegisteredTypes)
        {
            if (typeEntry.RuntimeGenericContext is not { } ctx)
                continue;

            // Only closed generic types: definition != subject
            if (string.IsNullOrEmpty(typeEntry.DefinitionSubjectId) ||
                string.Equals(typeEntry.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal))
                continue;

            // Get closed type token
            uint closedToken = (uint)(typeEntry.MetadataToken > 0 ? typeEntry.MetadataToken : 0);
            if (closedToken == 0)
                continue;

            // Get open definition token
            uint openToken = lookup.TryGetTypeToken(typeEntry.DefinitionSubjectId);
            if (openToken == 0)
                continue;

            // Collect type argument tokens
            var typeArgs = ctx.InstantiationKey.TypeArguments;
            uint argCount = 0;
            if (typeArgs is { Count: > 0 })
            {
                // NOTE: typeArgs contains subject IDs like "System.Int32".
                // They must be in the MetadataRegistration's "type" registrations.
                // For primitive/built-in types not in the registration, we use
                // pseudo-tokens (hash-based, matching CreatePseudoTypeToken logic).
                foreach (var argSubjectId in typeArgs)
                {
                    uint argToken = lookup.TryGetTypeToken(argSubjectId);
                    if (argToken == 0)
                    {
                        // This is expected for type args that are not concrete
                        // type definitions (e.g., generic parameters from open
                        // types that appear in the type argument list of the
                        // closed type). When the type arg is itself a generic
                        // parameter reference (not a closed type), we cannot
                        // emit a static token — this instantiation requires
                        // runtime resolution and will be handled by the
                        // RuntimeInstantiationBridge in Phase 3.
                        //
                        // For Phase 1 we only emit registrations where all
                        // type arguments have known tokens.
                        argTokens.Clear();
                        break;
                    }
                    argTokens.Add($"0x{argToken:X8}u");
                    argCount++;
                }
            }

            if (argCount == 0 && typeArgs is { Count: > 0 })
            {
                // Skipped due to unresolvable type args
                continue;
            }

            entries.Add(new TypeEntry(openToken, closedToken, argCount, argPoolIndex));
            argPoolIndex += argCount;
        }
    }

    private void BuildGenericMethodRegistration(
        SupplementalMetadataTemplateArtifact supplemental,
        MetadataRegistrationArtifact metadataRegistration,
        out List<MethodEntry> entries,
        out List<string> argTokens)
    {
        entries = new List<MethodEntry>();
        argTokens = new List<string>();

        var lookup = new MetadataTokenLookup(metadataRegistration.Registrations);
        uint argPoolIndex = 0;

        foreach (var methodEntry in supplemental.RegisteredMethods)
        {
            if (methodEntry.RuntimeGenericContext is not { } ctx)
                continue;

            if (methodEntry.MetadataToken <= 0)
                continue;

            uint methodToken = (uint)methodEntry.MetadataToken;
            uint classArgCount = 0;
            uint methodArgCount = 0;

            // Collect class-level type arguments
            var classArgs = new List<string>();
            if (ctx.InstantiationKey.TypeArguments is { Count: > 0 } typeArgs)
            {
                foreach (var arg in typeArgs)
                {
                    uint token = lookup.TryGetTypeToken(arg);
                    if (token == 0) { classArgs.Clear(); break; }
                    classArgs.Add($"0x{token:X8}u");
                }
                if (classArgs.Count == 0 && typeArgs.Count > 0)
                    continue; // skip unresolvable
            }

            // Collect method-level type arguments
            var methodArgs = new List<string>();
            if (ctx.InstantiationKey.MethodArguments is { Count: > 0 } methArgs)
            {
                foreach (var arg in methArgs)
                {
                    uint token = lookup.TryGetTypeToken(arg);
                    if (token == 0) { methodArgs.Clear(); break; }
                    methodArgs.Add($"0x{token:X8}u");
                }
                if (methodArgs.Count == 0 && methArgs.Count > 0)
                    continue; // skip unresolvable
            }

            classArgCount = (uint)classArgs.Count;
            methodArgCount = (uint)methodArgs.Count;

            // Append to flat pool: class args first, then method args
            argTokens.AddRange(classArgs);
            argTokens.AddRange(methodArgs);

            entries.Add(new MethodEntry(
                methodToken, classArgCount, methodArgCount, argPoolIndex));
            argPoolIndex += classArgCount + methodArgCount;
        }
    }

    // ── Method AOT entry builder (for GenericMethodAotEntryV0[]) ──────────────

    /// <summary>
    /// Build per-module AOT method entries by scanning supplemental registered
    /// methods that have a RuntimeGenericContext AND a resolvable open-definition
    /// metadata token in the same module.
    ///
    /// Each entry maps (open_definition_token, type_args[]) → closed_token,
    /// enabling the runtime to bypass the GenericContextRegistry/Interpreter for
    /// generic method calls whose instantiation was pre-compiled by AOT.
    ///
    /// Cross-module definitions (where the open method is defined in a different
    /// assembly) are skipped — they fall back to the interpreter gracefully.
    ///
    /// The emitted array is sorted by open_token for per-module binary search.
    /// </summary>
    private void BuildMethodAotRegistration(
        SupplementalMetadataTemplateArtifact supplemental,
        MetadataRegistrationArtifact metadataRegistration,
        out List<MethodAotEntry> entries,
        out List<string> argTokens)
    {
        entries = new List<MethodAotEntry>();
        argTokens = new List<string>();

        // ── Build metadata-token lookups from supplemental entries ──────
        // The AOT map uses REAL metadata tokens (0x060000xx / 0x020000xx),
        // NOT the Slot indices used by the GenericContextRegistry.
        var methodTokenBySubjectId = new Dictionary<string, uint>(StringComparer.Ordinal);
        foreach (var m in supplemental.RegisteredMethods)
        {
            if (m.MetadataToken > 0)
                methodTokenBySubjectId[m.SubjectId] = (uint)m.MetadataToken;
        }

        var typeTokenBySubjectId = new Dictionary<string, uint>(StringComparer.Ordinal);
        foreach (var t in supplemental.RegisteredTypes)
        {
            if (t.MetadataToken > 0)
                typeTokenBySubjectId[t.SubjectId] = (uint)t.MetadataToken;
        }

        uint ResolveTypeToken(string subjectId)
        {
            return typeTokenBySubjectId.TryGetValue(subjectId, out var t) ? t : 0u;
        }

        // ── Collect entries (with temp args) before sorting ─────────────
        var unsorted = new List<(MethodAotEntry Entry, List<string> Args)>();

        foreach (var methodEntry in supplemental.RegisteredMethods)
        {
            if (methodEntry.RuntimeGenericContext is not { } ctx)
                continue;
            if (methodEntry.MetadataToken <= 0)
                continue;

            uint closedToken = (uint)methodEntry.MetadataToken;

            // Resolve open definition token from DefinitionSubjectId.
            // This succeeds only when the open method is defined in the same
            // module (cross-module definitions → 0 → skip → interpreter).
            if (string.IsNullOrEmpty(methodEntry.DefinitionSubjectId))
                continue;
            if (!methodTokenBySubjectId.TryGetValue(methodEntry.DefinitionSubjectId, out var openToken))
                continue;

            // Collect ALL type arguments (class-level + method-level) into
            // one flat pool.  Each value must be a resolvable metadata token.
            var args = new List<string>();

            // Class-level type arguments
            if (ctx.InstantiationKey.TypeArguments is { Count: > 0 } typeArgs)
            {
                foreach (var arg in typeArgs)
                {
                    uint token = ResolveTypeToken(arg);
                    if (token == 0) { args.Clear(); break; }
                    args.Add($"0x{token:X8}u");
                }
                if (args.Count == 0 && typeArgs.Count > 0)
                    continue;
            }

            // Method-level type arguments
            if (ctx.InstantiationKey.MethodArguments is { Count: > 0 } methArgs)
            {
                foreach (var arg in methArgs)
                {
                    uint token = ResolveTypeToken(arg);
                    if (token == 0) { args.Clear(); break; }
                    args.Add($"0x{token:X8}u");
                }
                if (args.Count == 0 && methArgs.Count > 0)
                    continue;
            }

            unsorted.Add((new MethodAotEntry(openToken, closedToken, (uint)args.Count, 0), args));
        }

        // ── Sort by open_token for binary search ────────────────────────
        unsorted.Sort((a, b) => a.Entry.OpenToken.CompareTo(b.Entry.OpenToken));

        // ── Rebuild flat arg pool in sorted order ───────────────────────
        uint argPoolIndex = 0;
        foreach (var (entry, entryArgs) in unsorted)
        {
            entries.Add(entry with { ArgsStartIndex = argPoolIndex });
            argTokens.AddRange(entryArgs);
            argPoolIndex += entry.ArgCount;
        }
    }

    /// <summary>
    /// Minimal token lookup for the codegen phase.
    /// In Phase 1 this handles the common case where subject IDs are
    /// registered in the MetadataRegistrationArtifact.
    /// </summary>
    private sealed class MetadataTokenLookup
    {
        private readonly Dictionary<string, uint> _typeTokens = new(StringComparer.Ordinal);
        private readonly Dictionary<string, uint> _methodTokens = new(StringComparer.Ordinal);

        public MetadataTokenLookup(IReadOnlyList<MetadataRegistrationEntry> registrations)
        {
            foreach (var reg in registrations)
            {
                if (reg.Slot <= 0) continue;
                uint token = (uint)reg.Slot;

                switch (reg.RegistrationKind)
                {
                    case "type":
                        _typeTokens[reg.SubjectId] = token;
                        break;
                    case "method":
                        _methodTokens[reg.SubjectId] = token;
                        break;
                }
            }
        }

        public uint TryGetTypeToken(string subjectId)
        {
            return _typeTokens.TryGetValue(subjectId, out var token) ? token : 0;
        }

        public uint TryGetMethodToken(string subjectId)
        {
            return _methodTokens.TryGetValue(subjectId, out var token) ? token : 0;
        }
    }

    /// <summary>
    /// Emit the MetadataRegistrationV0 generic field initializers that
    /// reference the constexpr arrays emitted by EmitGenericRegistration.
    /// </summary>
    private void EmitMetadataRegistrationGenericFields(StringBuilder builder)
    {
        // These field initializers appear inside the MetadataRegistrationV0
        // struct definition that the codegen emits.
        builder.AppendLine("    .generic_types         = kGenericTypeEntries,");
        builder.AppendLine("    .generic_type_count    = sizeof(kGenericTypeEntries) / sizeof(kGenericTypeEntries[0]),");
        builder.AppendLine("    .generic_type_args     = kGenericTypeArgTokens,");
        builder.AppendLine("    .generic_type_arg_count = sizeof(kGenericTypeArgTokens) / sizeof(kGenericTypeArgTokens[0]),");
        builder.AppendLine("    .generic_methods       = kGenericMethodEntries,");
        builder.AppendLine("    .generic_method_count  = sizeof(kGenericMethodEntries) / sizeof(kGenericMethodEntries[0]),");
        builder.AppendLine("    .generic_method_args   = kGenericMethodArgTokens,");
        builder.AppendLine("    .generic_method_arg_count = sizeof(kGenericMethodArgTokens) / sizeof(kGenericMethodArgTokens[0]),");
        builder.AppendLine("    .method_aot_entries       = s_method_aot_entries,");
        builder.AppendLine("    .method_aot_entry_count  = sizeof(s_method_aot_entries) / sizeof(s_method_aot_entries[0]),");
        builder.AppendLine("    .method_aot_entry_args   = s_method_aot_entry_args,");
        builder.AppendLine("    .method_aot_entry_arg_count = sizeof(s_method_aot_entry_args) / sizeof(s_method_aot_entry_args[0]),");
    }
}

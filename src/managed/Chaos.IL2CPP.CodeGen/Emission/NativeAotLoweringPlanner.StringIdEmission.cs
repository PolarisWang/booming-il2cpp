using System.Collections.Immutable;
using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private IReadOnlyDictionary<string, ulong>? _stringIdMapping;

    private static IReadOnlySet<string> CollectStringLiterals(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var literals = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (string.Equals(instruction.Op, "ldstr", StringComparison.Ordinal))
                {
                    var operand = GetRequiredStringOperand(instruction);
                    if (!string.IsNullOrEmpty(operand))
                    {
                        literals.Add(operand);
                    }
                }
            }
        }
        return literals.ToImmutableHashSet(StringComparer.Ordinal);
    }

    private static ulong ComputeStringId(string utf8)
    {
        // FNV-1a 64-bit hash — must match the runtime string_table.cpp algorithm exactly.
        const ulong fnvOffsetBasis = 14695981039346656037UL;
        const ulong fnvPrime = 1099511628211UL;

        var bytes = Encoding.UTF8.GetBytes(utf8);
        var hash = fnvOffsetBasis;
        foreach (var b in bytes)
        {
            hash ^= b;
            hash *= fnvPrime;
        }

        // Clear bit 63 (reserved as CHAOS_STRING_ID_TAG at runtime) and ensure non-zero.
        hash &= ~(1UL << 63);
        return hash | 1UL;
    }

    private static IReadOnlyDictionary<string, ulong> BuildStringIdMapping(
        IReadOnlySet<string> stringLiterals)
    {
        if (stringLiterals.Count == 0)
        {
            return ImmutableDictionary<string, ulong>.Empty;
        }

        // Compute FNV-1a hash for each string literal.
        var mapping = new Dictionary<string, ulong>(stringLiterals.Count, StringComparer.Ordinal);
        foreach (var literal in stringLiterals)
        {
            var hash = ComputeStringId(literal);
            mapping[literal] = hash;
        }

        // Detect collisions.  FNV-1a 63-bit collision in <1000 entries is
        // virtually impossible, but we check anyway since the C++ constexpr
        // macro (CHAOS_IL2CPP_STRING_ID) always uses the raw hash.
        var collisions = mapping
            .GroupBy(kvp => kvp.Value)
            .Where(g => g.Skip(1).Any())
            .ToList();

        if (collisions.Count > 0)
        {
            var msgs = collisions.Select(g =>
                $"hash {g.Key} collides between: {string.Join(", ", g.Select(kvp => $"'{kvp.Key}'"))}");
            throw new InvalidOperationException(
                $"StringId FNV-1a collision in this family -- rename a literal to resolve.\n{string.Join("\n", msgs)}");
        }

        return mapping.ToImmutableDictionary(StringComparer.Ordinal);
    }

    private string BuildStringIdTable(
        IReadOnlySet<string> stringLiterals)
    {
        if (stringLiterals.Count == 0)
        {
            return string.Empty;
        }

        var mapping = _stringIdMapping ??= BuildStringIdMapping(stringLiterals);

        var entries = stringLiterals
            .OrderBy(literal => mapping[literal])
            .Select(literal => new ScriptObject
            {
                ["id"] = (object)mapping[literal],
                ["cpp_literal"] = ToCppStringLiteral(literal),
                ["byte_count"] = Encoding.UTF8.GetByteCount(literal),
            })
            .ToArray();

        var model = new ScriptObject
        {
            ["string_entries"] = entries,
            ["indentation"] = ScribanTemplateRenderer.Indentation(1),
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetStringIdTableTemplate(), model);
    }

    private bool TryGetStringId(string literal, out ulong id)
    {
        if (_stringIdMapping is not null && _stringIdMapping.TryGetValue(literal, out id))
        {
            return true;
        }

        id = 0u;
        return false;
    }
}

using System.Collections.Immutable;
using System.Text;
using Chaos.IL2CPP.Contracts;

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

        // First pass: compute raw hashes.
        var mapping = new Dictionary<string, ulong>(stringLiterals.Count, StringComparer.Ordinal);
        var usedValues = new HashSet<ulong>(stringLiterals.Count);
        foreach (var literal in stringLiterals)
        {
            var hash = ComputeStringId(literal);
            mapping[literal] = hash;
            usedValues.Add(hash);
        }

        // Second pass: detect and resolve collisions.
        var collisionGroups = mapping
            .GroupBy(kvp => kvp.Value)
            .Where(g => g.Skip(1).Any())
            .ToList();

        if (collisionGroups.Count == 0)
        {
            return mapping.ToImmutableDictionary(StringComparer.Ordinal);
        }

        var nextSyntheticId = (ulong)mapping.Values.Max() + 1;

        foreach (var group in collisionGroups)
        {
            // Keep the first entry with its content hash; assign overflow IDs to the rest.
            var skipFirst = true;
            foreach (var kvp in group)
            {
                if (skipFirst)
                {
                    skipFirst = false;
                    continue;
                }
                // Ensure synthetic ID does not collide with any existing hash.
                while (!usedValues.Add(nextSyntheticId))
                {
                    nextSyntheticId++;
                }
                mapping[kvp.Key] = nextSyntheticId;
                nextSyntheticId++;
            }
        }

        return mapping.ToImmutableDictionary(StringComparer.Ordinal);
    }

    private void EmitStringIdTable(
        StringBuilder builder,
        IReadOnlySet<string> stringLiterals)
    {
        if (stringLiterals.Count == 0)
        {
            return;
        }

        var mapping = _stringIdMapping ??= BuildStringIdMapping(stringLiterals);

        builder.AppendLine();
        builder.AppendLine("// StringId constants for compile-time-known string literals.");

        // Emit constexpr CHAOS_IL2CPP_UINT64 constant for each string literal.
        foreach (var literal in stringLiterals.OrderBy(l => l, StringComparer.Ordinal))
        {
            var id = mapping[literal];
            builder.AppendLine($"constexpr CHAOS_IL2CPP_UINT64 {GetNativeStringIdSymbol(id)} = {id}U;");
        }

        builder.AppendLine();
        builder.AppendLine("// AOT-baked string table: sorted by StringId for binary search at runtime.");
        builder.AppendLine("constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {");

        foreach (var literal in stringLiterals.OrderBy(l => mapping[l]))
        {
            var id = mapping[literal];
            var byteCount = Encoding.UTF8.GetByteCount(literal);
            var cppLiteral = ToCppStringLiteral(literal);
            builder.AppendLine($"    {{ {GetNativeStringIdSymbol(id)}, {cppLiteral}, {byteCount}u }},");
        }

        builder.Append('}');
        builder.AppendLine(";");
        builder.AppendLine();
        builder.AppendLine("constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);");
        builder.AppendLine();
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

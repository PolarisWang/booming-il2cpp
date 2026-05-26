"""Extract LINQ method subject IDs and generate capability family contracts.

Categorizes System.Linq.Enumerable public static methods into
12 semantic families defined in the capability-family-ledger.
"""
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_LEDGER_PATH = _REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"

# Method name → family slug mapping
METHOD_TO_FAMILY: dict[str, str] = {
    # ── filtering ──
    "Where": "filtering",
    "OfType": "filtering",
    "Cast": "filtering",
    "DistinctBy": "filtering",
    # ── projection ──
    "Select": "projection",
    "SelectMany": "projection",
    # ── partitioning ──
    "Skip": "partitioning",
    "SkipWhile": "partitioning",
    "SkipLast": "partitioning",
    "Take": "partitioning",
    "TakeWhile": "partitioning",
    "TakeLast": "partitioning",
    "Chunk": "partitioning",
    # ── ordering ──
    "OrderBy": "ordering",
    "OrderByDescending": "ordering",
    "ThenBy": "ordering",
    "ThenByDescending": "ordering",
    "Order": "ordering",
    "OrderDescending": "ordering",
    "Reverse": "ordering",
    # ── grouping-lookup ──
    "GroupBy": "grouping-lookup",
    "ToLookup": "grouping-lookup",
    # ── joins ──
    "Join": "joins",
    "GroupJoin": "joins",
    "LeftJoin": "joins",
    "RightJoin": "joins",
    # ── set-operations ──
    "Distinct": "set-operations",
    "Except": "set-operations",
    "ExceptBy": "set-operations",
    "Intersect": "set-operations",
    "IntersectBy": "set-operations",
    "Union": "set-operations",
    "UnionBy": "set-operations",
    # ── aggregation ──
    "Aggregate": "aggregation",
    "AggregateBy": "aggregation",
    "Count": "aggregation",
    "LongCount": "aggregation",
    # ── numeric-aggregation ──
    "Sum": "numeric-aggregation",
    "Min": "numeric-aggregation",
    "Max": "numeric-aggregation",
    "Average": "numeric-aggregation",
    "MinBy": "numeric-aggregation",
    "MaxBy": "numeric-aggregation",
    # ── materialization ──
    "ToArray": "materialization",
    "ToList": "materialization",
    "ToDictionary": "materialization",
    "ToHashSet": "materialization",
    "ToLookup": "materialization",
    "ToImmutableArray": "materialization",
    "ToImmutableList": "materialization",
    "ToImmutableDictionary": "materialization",
    "ToImmutableHashSet": "materialization",
    "ToImmutableSortedDictionary": "materialization",
    "ToImmutableSortedSet": "materialization",
    "ToImmutableLinkedList": "materialization",
    # ── generation-concat ──
    "Empty": "generation-concat",
    "Range": "generation-concat",
    "Repeat": "generation-concat",
    "DefaultIfEmpty": "generation-concat",
    "Concat": "generation-concat",
    "Append": "generation-concat",
    "Prepend": "generation-concat",
    # ── shuffle-index ──
    "ElementAt": "shuffle-index",
    "ElementAtOrDefault": "shuffle-index",
    "First": "shuffle-index",
    "FirstOrDefault": "shuffle-index",
    "Last": "shuffle-index",
    "LastOrDefault": "shuffle-index",
    "Single": "shuffle-index",
    "SingleOrDefault": "shuffle-index",
    "Shuffle": "shuffle-index",
    "Index": "shuffle-index",
    # ── other (misc) ──
    "All": "filtering",
    "Any": "filtering",
    "Contains": "set-operations",
    "SequenceEqual": "set-operations",
    "CountBy": "aggregation",
    "AsEnumerable": "materialization",
    "TryGetNonEnumeratedCount": "aggregation",
    "Zip": "projection",
    "InfiniteSequence": "generation-concat",
    "Sequence": "generation-concat",
}

FAMILY_DISPLAY_NAMES: dict[str, str] = {
    "filtering": "LINQ Filtering",
    "projection": "LINQ Projection",
    "partitioning": "LINQ Partitioning",
    "ordering": "LINQ Ordering",
    "grouping-lookup": "LINQ Grouping & Lookup",
    "joins": "LINQ Joins",
    "set-operations": "LINQ Set Operations",
    "aggregation": "LINQ Aggregation",
    "numeric-aggregation": "LINQ Numeric Aggregation",
    "materialization": "LINQ Materialization",
    "generation-concat": "LINQ Generation & Concat",
    "shuffle-index": "LINQ Shuffle & Index",
}

FAMILY_SLUGS = [
    "filtering",
    "projection",
    "partitioning",
    "ordering",
    "grouping-lookup",
    "joins",
    "set-operations",
    "aggregation",
    "numeric-aggregation",
    "materialization",
    "generation-concat",
    "shuffle-index",
]


def _clr_type_to_subject_id(asm_name: str, type_full_name: str) -> str:
    """Convert CLR type name to subject ID format.

    System.Int32 -> System.Int32
    System.Collections.Generic.IEnumerable[TSource] -> System.Collections.Generic.IEnumerable{TSource}
    System.Func[TSource,System.Boolean] -> System.Func{TSource,System.Boolean}
    """
    # Convert brackets to CLR braces
    result = type_full_name.replace("[", "{").replace("]", "}")
    return result


def _build_method_subject_id(
    method_name: str,
    declaring_type: str,
    generic_args: list[str],
    param_types: list[str],
    return_type: str,
) -> str:
    """Build CLR method subject ID.

    Format: AssemblyName/FullTypeName::Method{GenArgs}:ReturnType(Params)
    """
    asm_name = "System.Linq"
    # Build the method part
    gen_part = ""
    if generic_args:
        args = ",".join(_clr_type_to_subject_id(asm_name, a) for a in generic_args)
        gen_part = "{" + args + "}"

    params_part = ",".join(_clr_type_to_subject_id(asm_name, p) for p in param_types)
    ret_part = _clr_type_to_subject_id(asm_name, return_type)

    return f"{asm_name}/{declaring_type}::{method_name}{gen_part}:{ret_part}({params_part})"


def _has_delegate_params(param_types: list[str]) -> bool:
    """Check if any param type is a delegate (Func/Action)."""
    for p in param_types:
        if "System.Func" in p or "System.Action" in p:
            return True
    return False


def _has_ref_param(param_types: list[str]) -> bool:
    """Check if any parameter is ref/out."""
    # In our extraction, ref params like Int32& aren't explicitly marked
    # We'll just check for known ref patterns
    return False


def enumerate_linq_methods() -> list[dict[str, Any]]:
    """Run C# code to enumerate System.Linq.Enumerable methods."""
    code = r"""
using System;
using System.Linq;
using System.Reflection;
using System.Text;

var asm = typeof(Enumerable).Assembly;
var types = asm.GetTypes().Where(t => t.Namespace == "System.Linq" && t.IsPublic);
foreach (var t in types.OrderBy(tt => tt.Name))
{
    foreach (var m in t.GetMethods(BindingFlags.Public | BindingFlags.Static)
        .Where(mi => mi.DeclaringType == t)
        .OrderBy(mm => mm.Name))
    {
        var parms = string.Join(",", m.GetParameters()
            .Select(p => FormatType(p.ParameterType)));
        var ret = FormatType(m.ReturnType);
        var genArgs = m.IsGenericMethodDefinition
            ? string.Join(",", m.GetGenericArguments().Select(FormatType))
            : "";
        Console.WriteLine(t.FullName + "|" + m.Name + "|" + genArgs + "|" + parms + "|" + ret);
    }
}

static string FormatType(Type t)
{
    if (t.IsGenericType)
    {
        var def = t.GetGenericTypeDefinition();
        var name = def.FullName ?? def.Name;
        var idx = name.IndexOf('`');
        if (idx >= 0) name = name.Substring(0, idx);
        var args = string.Join(",", t.GetGenericArguments().Select(FormatType));
        return name + "[" + args + "]";
    }
    return t.FullName ?? t.Name;
}
"""
    tmp = Path.home() / "AppData" / "Local" / "Temp" / "linq_extract2"
    tmp.mkdir(parents=True, exist_ok=True)
    (tmp / "Program.cs").write_text(code)
    csproj = tmp / "linq_extract2.csproj"
    if not csproj.exists():
        csproj.write_text(
            '<Project Sdk="Microsoft.NET.Sdk">\n'
            "  <PropertyGroup>\n"
            "    <OutputType>Exe</OutputType>\n"
            "    <TargetFramework>net10.0</TargetFramework>\n"
            "  </PropertyGroup>\n"
            "</Project>\n"
        )

    result = subprocess.run(
        ["dotnet", "run", "--project", str(tmp)],
        capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        print("FAILED:", result.stderr[:500])
        return []

    methods = []
    for line in result.stdout.strip().split("\n"):
        if not line.strip():
            continue
        parts = line.split("|", 4)
        if len(parts) != 5:
            continue
        decl_type, name, gen_args_str, params_str, ret_type = parts
        gen_args = [g.strip() for g in gen_args_str.split(",") if g.strip()]
        param_types = [p.strip() for p in params_str.split(",") if p.strip()]
        methods.append({
            "declaring_type": decl_type,
            "name": name,
            "gen_args": gen_args,
            "param_types": param_types,
            "return_type": ret_type,
        })

    return methods


def categorize_methods(methods: list[dict[str, Any]]) -> dict[str, list[dict[str, Any]]]:
    """Categorize methods by family based on method name mapping."""
    families: dict[str, list[dict[str, Any]]] = {slug: [] for slug in FAMILY_SLUGS}

    # Check for unmapped methods
    unmapped_count = 0
    # Track which methods are mapped to avoid duplicates in multi-family methods
    mapped_names = set()

    for m in methods:
        name = m["name"]
        # Some methods appear in multiple overloads (different param counts)
        # Map by name only
        family = METHOD_TO_FAMILY.get(name)
        if family:
            families[family].append(m)
            mapped_names.add(name)
        else:
            # Try suffix matching
            matched = False
            for key, fam in METHOD_TO_FAMILY.items():
                if name.startswith(key):
                    families[fam].append(m)
                    mapped_names.add(name)
                    matched = True
                    break
            if not matched and name not in mapped_names:
                if unmapped_count == 0:
                    print(f"  Unmapped methods: ", end="")
                print(name, end=", ")
                unmapped_count += 1

    if unmapped_count:
        print()  # end the unmapped line

    for slug in FAMILY_SLUGS:
        print(f"  {slug}: {len(families[slug])} methods")

    return families


def generate_contract(
    family_slug: str,
    methods: list[dict[str, Any]],
) -> dict[str, Any]:
    """Generate capability-family-contract.json for a family."""
    subject_ids = []
    custom_indices = []

    for i, m in enumerate(methods):
        sid = _build_method_subject_id(
            method_name=m["name"],
            declaring_type=m["declaring_type"],
            generic_args=m["gen_args"],
            param_types=m["param_types"],
            return_type=m["return_type"],
        )
        subject_ids.append(sid)
        # All LINQ methods involve generic type parameters and/or Func<> delegates
        # that the auto-generator can't construct proper C# call expressions for.
        # Mark all as custom entries.
        custom_indices.append(i)

    return {
        "schemaVersion": 1,
        "assemblyName": "System.Linq",
        "ownerSubjectId": "SolutionCorePack",
        "familyId": f"family/System.Linq/{family_slug}",
        "displayName": FAMILY_DISPLAY_NAMES.get(family_slug, family_slug),
        "methodSubjectIds": subject_ids,
        "methodSubjectCount": len(subject_ids),
        "customEntryIndices": custom_indices,
        "functionalObligation": {
            "required": True,
            "benchmarkRequired": False,
            "proofRequired": True,
        },
        "performanceObligation": {"required": False},
        "hotupdateFunctionalObligation": {"required": False},
        "hotupdatePerformanceObligation": {"required": False},
        "reviewBundleRequired": True,
    }


def main():
    print("Enumerating System.Linq methods...")
    methods = enumerate_linq_methods()
    print(f"Total: {len(methods)} methods\n")

    print("Categorizing by family...")
    families = categorize_methods(methods)
    print()

    # Load the ledger
    ledger = json.loads(_LEDGER_PATH.read_text(encoding="utf-8"))

    # Find System.Linq in the ledger
    linq_entry = None
    for dll in ledger["dlls"]:
        if dll["assemblyName"] == "System.Linq":
            linq_entry = dll
            break

    if linq_entry is None:
        print("ERROR: System.Linq not found in ledger")
        return 1

    # Create verification directories and generate contracts
    asm_dir = _REPO_ROOT / "testing" / "foundation-dll" / "System.Linq"

    total_mids = 0
    for fam_entry in linq_entry.get("families", []):
        fid = fam_entry["familyId"]
        slug = fid.split("/")[-1]

        if slug not in families:
            print(f"SKIP {slug}: no methods mapped")
            continue

        fam_methods = families[slug]
        if not fam_methods:
            print(f"SKIP {slug}: 0 methods")
            continue

        contract = generate_contract(slug, fam_methods)

        # Write contract
        fam_dir = asm_dir / slug
        fam_dir.mkdir(parents=True, exist_ok=True)
        contract_path = fam_dir / "capability-family-contract.json"
        contract_path.write_text(json.dumps(contract, indent=2) + "\n", encoding="utf-8")

        auto_count = len(fam_methods) - len(contract["customEntryIndices"])
        print(f"  {slug:25s} {len(fam_methods):3d} methods ({auto_count} auto, {len(contract['customEntryIndices'])} custom)")
        total_mids += len(fam_methods)

    print(f"\nTotal MIDs: {total_mids} across {sum(1 for f in families.values() if f)} families")
    print("Done!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
